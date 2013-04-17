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

TARFILE_NAME=CoinMP-1.6.0
TARFILE_MD5=2b5f1ca58d6ef30f18f1415b65bed81c

PATCH_FILES=coinmp-1.6.0.patch coinmp-1.6.0-clang.patch

.IF "$(COM)"=="MSC"
BUILD_ACTION=$(COMPATH)$/vcpackages$/vcbuild.exe CoinMP\\MSVisualStudio\\v9\\CoinMP.sln "Release|Win32"
.ELSE
CONFIGURE_ACTION=./configure
#CONFIGURE_FLAGS=--disable-pkg-config --disable-bzlib --disable-zlib CC='$(CC) $(ARCH_FLAGS)' CXX='$(CXX) $(ARCH_FLAGS)' CFLAGS='$(ARCH_FLAGS) -Wc,-arch -Wc,i386' CPPFLAGS='$(ARCH_FLAGS)' LDFLAGS='$(ARCH_FLAGS)' compiler_flags='$(ARCH_FLAGS)'
CONFIGURE_FLAGS=--disable-pkg-config --disable-bzlib --disable-zlib CC='$(CC) $(ARCH_FLAGS)' CXX='$(CXX) $(ARCH_FLAGS)'
#BUILD_ACTION= CC="$(CC) $(ARCH_FLAGS)" CPP="$(CXX) $(ARCH_FLAGS)" $(GNUMAKE) -j8
BUILD_ACTION= $(GNUMAKE) -j8
.ENDIF

OUT2INC+=CoinMP$/src/CoinMP.h

.IF "$(OS)"=="WNT"
OUT2BIN+=CoinMP$/MSVisualStudio$/v9$/release$/CoinMP.dll
OUT2LIB+=CoinMP$/MSVisualStudio$/v9$/release$/CoinMP.lib
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
