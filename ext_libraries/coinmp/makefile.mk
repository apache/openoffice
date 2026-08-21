#**************************************************************
#
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#
#**************************************************************

PRJ=.

PRJNAME=coinmp
TARGET=coinmp

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

# --- Files --------------------------------------------------------

.IF "$(ENABLE_COINMP)"!="YES"

all:
	@echo "CoinMP is disabled and will not be built."

.ELIF "$(SYSTEM_COINMP)"=="YES"

all:
	@echo "Using system CoinMP.  Does not have to be built."

.ELSE

TARFILE_NAME=CoinMP-1.8.4
TARFILE_MD5=dd85ff540997a95ebb233bb5ae7bcc61

# Only one patch survives the move from 1.7.6: the libtool freebsd1*/freebsd2*
# glob fix (FreeBSD 10 and up match the pattern for FreeBSD 1) plus dropping a
# CBC_SVN_REV printf.  1.8.4 still ships the old libtool, so it is still
# needed, and all 127 hunks still apply.
#
# The other three are gone because 1.8.4 already contains them:
#   coinmp-1.7.6-clang.patch      CoinSignal.hpp now uses __decltype(SIG_DFL)
#                                 throughout rather than bare typeof
#   coinmp-1.7.6-clang-1.8.patch  Cbc_C_Interface.cpp now prints results as
#                                 static_cast<const void*>
#   coinmp-1.7.6-nopedantic.patch -pedantic-errors is no longer added by
#                                 coin.m4 or by any generated configure
PATCH_FILES=coinmp-1.8.4.patch
.IF "$(OS)"=="OS2"
PATCH_FILES+=coinmp-1.6.0-os2.patch
.ENDIF

# vcbuild.exe reads .vcproj and shipped with VS2005 and VS2008 only.  VS2010
# replaced it with MSBuild and changed the project format to .vcxproj at the
# same time, so a modern toolset cannot use the v9 solution at all.  CoinMP
# ships both: v9 for vcbuild, v10 for MSBuild.  Each toolset gets the pair it
# can actually drive, which is also what keeps the VC9 command line identical
# to what it has always been.
.IF "$(COM)$(CPU)"=="MSCI"
.IF "$(COMEX)"=="14"
BUILD_ACTION=$(MSBUILD_PATH)$/MSBuild.exe CoinMP\\MSVisualStudio\\v10\\CoinMP.sln /p:Configuration=Release /p:Platform=Win32 /p:PlatformToolset=$(VCTOOLSET) /p:WindowsTargetPlatformVersion=$(WINDOWS_SDK_VERSION)
.ELSE
BUILD_ACTION=$(COMPATH)$/vcpackages$/vcbuild.exe -useenv CoinMP\\MSVisualStudio\\v9\\CoinMP.sln "Release|Win32"
.ENDIF
.ELIF "$(COM)$(CPU)"=="MSCX"
.IF "$(COMEX)"=="14"
BUILD_ACTION=$(MSBUILD_PATH)$/MSBuild.exe CoinMP\\MSVisualStudio\\v10\\CoinMP.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=$(VCTOOLSET) /p:WindowsTargetPlatformVersion=$(WINDOWS_SDK_VERSION)
.ELSE
BUILD_ACTION=$(COMPATH)$/vcpackages$/vcbuild.exe -useenv CoinMP\\MSVisualStudio\\v9\\CoinMP.sln "Release|x64"
.ENDIF
.ELSE
CONFIGURE_ACTION=./configure
#CONFIGURE_FLAGS=--disable-pkg-config --disable-bzlib --disable-zlib CC='$(CC) $(ARCH_FLAGS)' CXX='$(CXX) $(ARCH_FLAGS)' CFLAGS='$(ARCH_FLAGS) -Wc,-arch -Wc,i386' CPPFLAGS='$(ARCH_FLAGS)' LDFLAGS='$(ARCH_FLAGS)' compiler_flags='$(ARCH_FLAGS)'
CONFIGURE_FLAGS=--disable-pkg-config --with-blas=BUILD --with-lapack=BUILD --disable-bzlib --disable-zlib CC='$(CC) $(ARCH_FLAGS)' CXX='$(CXX) $(ARCH_FLAGS)'
.IF "$(OS)"=="MACOSX"
# Each CoinMP sub-library (Osi, Clp, ...) depends on symbols from its siblings
# (CoinUtils, ...).  CoinMP's bundled 2013-era libtool only adds the Darwin
# "-undefined dynamic_lookup" escape hatch for MACOSX_DEPLOYMENT_TARGET=10.x;
# at 11.0+ (Apple Silicon baseline) that case falls through, leaving the flag
# empty, so each .dylib is linked with unresolved sibling symbols and the
# two-level-namespace linker rejects it ("symbol(s) not found for arm64").
# --enable-dependency-linking makes the inter-library dependencies explicit on
# each link line (and sets -no-undefined), which is the correct fix.
CONFIGURE_FLAGS+=--enable-dependency-linking
.ENDIF
#BUILD_ACTION= CC="$(CC) $(ARCH_FLAGS)" CPP="$(CXX) $(ARCH_FLAGS)" $(GNUMAKE) -j8
BUILD_ACTION= $(GNUMAKE) -j$(MAXPROCESS)
.ENDIF

.IF "$(OS)"=="OS2"
CONFIGURE_FLAGS+= --disable-shared --enable-static
.ENDIF

OUT2INC+=CoinMP$/src/CoinMP.h

.IF "$(OS)$(CPU)"=="WNTI"
.IF "$(COMEX)"=="14"
OUT2BIN+=CoinMP$/MSVisualStudio$/v10$/Release$/CoinMP.dll
OUT2LIB+=CoinMP$/MSVisualStudio$/v10$/Release$/CoinMP.lib
.ELSE
OUT2BIN+=CoinMP$/MSVisualStudio$/v9$/release$/CoinMP.dll
OUT2LIB+=CoinMP$/MSVisualStudio$/v9$/release$/CoinMP.lib
.ENDIF
.ELIF "$(OS)$(CPU)"=="WNTX"
.IF "$(COMEX)"=="14"
OUT2BIN+=CoinMP$/MSVisualStudio$/v10$/x64$/Release$/CoinMP.dll
OUT2LIB+=CoinMP$/MSVisualStudio$/v10$/x64$/Release$/CoinMP.lib
.ELSE
OUT2BIN+=CoinMP$/MSVisualStudio$/v9$/x64$/Release$/CoinMP.dll
OUT2LIB+=CoinMP$/MSVisualStudio$/v9$/x64$/Release$/CoinMP.lib
.ENDIF
.ELIF "$(OS)"=="OS2"
OUT2BIN+=CoinMP$/src$/CoinMP.dll
OUT2LIB+=CoinMP$/src$/.libs/CoinMP.lib
.ELSE
OUT2LIB+=CoinMP$/src$/.libs$/libCoinMP*$(DLLPOST)*
OUT2LIB+=CoinUtils$/src$/.libs$/libCoinUtils*$(DLLPOST)*
OUT2LIB+=Cbc$/src$/.libs$/libCbc*$(DLLPOST)*
OUT2LIB+=Cbc$/src$/OsiCbc$/.libs$/libOsiCbc*$(DLLPOST)*
OUT2LIB+=Cgl$/src$/.libs$/libCgl*$(DLLPOST)*
OUT2LIB+=Clp$/src$/.libs$/libClp*$(DLLPOST)*
OUT2LIB+=Clp$/src$/OsiClp$/.libs$/libOsiClp*$(DLLPOST)*
OUT2LIB+=Osi$/src$/Osi$/.libs$/libOsi*$(DLLPOST)*
.ENDIF



# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk

.ENDIF
