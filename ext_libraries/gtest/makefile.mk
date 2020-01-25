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

PRJNAME=gtest
TARGET=oo_gtest

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk
.INCLUDE :	gtest_version.mk

# --- Files --------------------------------------------------------

.IF "$(ENABLE_UNIT_TESTS)"!="YES"

all:
	@echo "Unit tests are disabled.  Does not have to be built."

.ELSE

# Assemble the full version number from the parts defined in gtest_version.mk
LIBGTESTVERSION=$(GTEST_MAJOR).$(GTEST_MINOR).$(GTEST_MICRO)

TARFILE_NAME=$(PRJNAME)-$(LIBGTESTVERSION)
# This is the SHA1 checksum, not MD5 but tg_ext.mk does not now about this and,
# thankfully, does not care.
TARFILE_MD5=2d6ec8ccdf5c46b05ba54a9fd1d130d7

PATCH_FILES= use-own-tuple.patch gtest-python3.patch

.IF "$(OS)"=="WNT"

CONFIGURE_DIR=
# dirty hack for gbuild whihc relies on .cxx extension for C++ files
CONFIGURE_ACTION=cp src/gtest-all.cc src/gtest-all.cxx
CONFIGURE_FLAGS=

BUILD_DIR=
BUILD_ACTION=$(GNUMAKE)
BUILD_FLAGS+= -f ../../../../win/Makefile -j$(EXTMAXPROCESS)

.ELIF "$(GUI)" == "OS2"

@all:
	@echo "using system serf. nothing do do."

.ELSE

.IF "$(OS)"=="LINUX"
PATCH_FILES+=linux-pthread.patch
.ENDIF

CONFIGURE_DIR=
CONFIGURE_ACTION=.$/configure

.IF "$(OS)"=="MACOSX"
CONFIGURE_FLAGS='CXX=$(CXX)'
CONFIGURE_FLAGS+='CXXFLAGS=-stdlib=libc++ -U__STRICT_ANSI__ -isysroot $(MACOSX_SDK_PATH)'
.ENDIF

BUILD_DIR=$(CONFIGURE_DIR)
BUILD_ACTION=$(GNUMAKE)
BUILD_FLAGS+= -j$(EXTMAXPROCESS)


OUTDIR2INC=include/gtest

.IF "$(OS)"=="MACOSX"
OUT2LIB=lib/.libs/libgtest.*dylib
#OUT2LIB+=lib/.libs/libgtest_main.*dylib
.ELSE
OUT2LIB=lib/.libs/libgtest.so*
#OUT2LIB+=lib/.libs/libgtest_main.so*
.ENDIF

.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk

.ENDIF
