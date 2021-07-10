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

PRJNAME=apr
TARGET=apr

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk
.INCLUDE :      apr_version.mk

# --- Files --------------------------------------------------------

.IF "$(SYSTEM_APR)"=="YES"

all:
	@echo "Using system apr.  Does not have to be built."

.ELSE

# Assemble the full version number from the parts defined in apr_version.mk
APRVERSION=$(APR_MAJOR).$(APR_MINOR).$(APR_MICRO)

TARFILE_NAME=$(PRJNAME)-$(APRVERSION)
TARFILE_MD5=0759294408daace192c935b7c9e76e04

.IF "$(OS)"=="WNT"

PATCH_FILES+= $(TARFILE_NAME)-no-devenv.patch
PATCH_FILES+= $(TARFILE_NAME)-windows.patch
CONFIGURE_ACTION=cp include/apr.hw include/apr.h
BUILD_DIR=
BUILD_ACTION=INCLUDE="$(INCLUDE);./include" USEMAK=1  nmake -f Makefile.win buildall

.ELIF "$(GUI)" == "OS2"

@all:
	@echo "using system apr. nothing do do."

.ELSE

CONFIGURE_DIR=
CONFIGURE_ACTION=.$/configure --prefix=$(OUTDIR) --includedir=$(OUTDIR)$/inc$/apr

# Recent versions of clang (clang-12xxx) on macOS break some tests in APR 1.6 (and older)
# configure (mostly around testing sizeof) due to errors now being fatal.
# Work around this by ignoring all errors but only with the later versions of Xcode
.IF "$(OS)"=="MACOSX" && "$(COM)"=="CLANG" && "$(CCNUMVER)" > "001100000003"
CONFIGURE_FLAGS=CPPFLAGS="-Wno-error=all"
.ELSE
CONFIGURE_FLAGS=
.ENDIF

BUILD_DIR=$(CONFIGURE_DIR)
BUILD_ACTION=$(GNUMAKE) --debug=b
#BUILD_FLAGS+= -j$(EXTMAXPROCESS)

# Make apr-1-config believe that apr has been installed.
INSTALL_ACTION=if [ -f apr-1-config.orig ]; then mv -f apr-1-config.orig apr-1-config; fi;	\
	cp apr-1-config apr-1-config.orig;							\
	sed -e "s/^location=source/location=installed/;s/^\(installbuilddir=.*\)\/.*/\\1\/lib\/apr\"/" apr-1-config > apr-1-config.installed;								\
	mv -f apr-1-config.installed apr-1-config;	\
	chmod +x apr-1-config

.ENDIF

OUT2INC+=include$/apr*.h
OUT2INC_SUBDIR=apr

.IF "$(OS)"=="WNT"
.IF "$(CPUNAME)"=="INTEL"
OUT2LIB+=LibR$/*.lib
OUT2LIB+=LibR$/*.pdb
OUT2LIB+=Release$/libaprapp-1.lib
OUT2LIB+=Release$/libaprapp-1.pdb
OUT2LIB+=Release$/libapr-1.lib
OUT2LIB+=Release$/libapr-1.exp
OUT2BIN+=Release$/libapr-1.dll
OUT2BIN+=Release$/libapr-1.pdb
.ELIF "$(CPUNAME)"=="X86_64"
OUT2LIB+=x64/LibR$/*.lib
OUT2LIB+=x64/LibR$/*.pdb
OUT2LIB+=x64/Release$/libaprapp-1.lib
OUT2LIB+=x64/Release$/libaprapp-1.pdb
OUT2LIB+=x64/Release$/libapr-1.lib
OUT2LIB+=x64/Release$/libapr-1.exp
OUT2BIN+=x64/Release$/libapr-1.dll
OUT2BIN+=x64/Release$/libapr-1.pdb
.ENDIF
.ELSE
.IF "$(OS)"=="MACOSX"
OUT2LIB+=.libs/libapr-1.*dylib
.ELSE
OUT2LIB+=.libs/libapr-1.so*
.ENDIF
OUT2BIN=build/apr_rules.mk
OUT2BIN+=libtool
OUT2BIN+=apr-1-config
.ENDIF


# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk

.ENDIF
