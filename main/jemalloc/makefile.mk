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


PRJ=.

PRJNAME=so_jemalloc
TARGET=so_jemalloc

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

.IF "$(ENABLE_JEMALLOC)" != "YES"
all:
	@echo "jemalloc is not enabled; configure with --enable-jemalloc to build it."
.ENDIF

# --- Files --------------------------------------------------------

TARFILE_NAME=jemalloc-5.4.0
TARFILE_MD5=e9abfa20270beb35e54043240ba37369

CONFIGURE_DIR=.$/
CONFIGURE_ACTION=.$/configure
CONFIGURE_FLAGS=--disable-static --disable-cxx --disable-doc
CONFIGURE_FLAGS+=CC="$(CC) $(ARCH_FLAGS)"

.IF "$(OS)"=="LINUX"
# libjemalloc is loaded together with libraries that dlopen() it late, which
# the initial-exec TLS model does not allow.
CONFIGURE_FLAGS+=--disable-initial-exec-tls
.IF "$(CPUNAME)"=="AARCH64"
# Linux/arm64 kernels come with 4K, 16K or 64K pages; 64K covers all of them.
CONFIGURE_FLAGS+=--with-lg-page=16
.ENDIF
.ENDIF

BUILD_DIR=$(CONFIGURE_DIR)
BUILD_ACTION=$(GNUMAKE) build_lib_shared
BUILD_FLAGS+= -j$(EXTMAXPROCESS)

.IF "$(OS)"=="MACOSX"
OUT2LIB=lib$/libjemalloc.2.dylib
.ELSE
OUT2LIB=lib$/libjemalloc.so.2
.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk
