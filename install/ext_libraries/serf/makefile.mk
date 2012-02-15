#/**************************************************************
# * 
# * Licensed to the Apache Software Foundation (ASF) under one
# * or more contributor license agreements.  See the NOTICE file
# * distributed with this work for additional information
# * regarding copyright ownership.  The ASF licenses this file
# * to you under the Apache License, Version 2.0 (the
# * "License"); you may not use this file except in compliance
# * with the License.  You may obtain a copy of the License at
# * 
# *   http://www.apache.org/licenses/LICENSE-2.0
# * 
# * Unless required by applicable law or agreed to in writing,
# * software distributed under the License is distributed on an
# * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# * KIND, either express or implied.  See the License for the
# * specific language governing permissions and limitations
# * under the License.
# * 
# *************************************************************/

PRJ=.

PRJNAME=serf
TARGET=so_serf

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

# --- Files --------------------------------------------------------

LIBSERFVERSION=1.0.0

TARFILE_NAME=$(PRJNAME)-$(LIBSERFVERSION)
TARFILE_MD5=3b179ed18f65c43141528aa6d2440db4

.IF "$(OS)"=="WNT"

ADDITIONAL_FILES=Makefile Module_serf.mk Library_serf.mk Package_inc.mk

PATCH_FILES=$(TARFILE_NAME).makewin32.patch

CONFIGURE_DIR=
CONFIGURE_ACTION=
CONFIGURE_FLAGS=

BUILD_DIR=$(CONFIGURE_DIR)
BUILD_ACTION=$(GNUMAKE)
BUILD_FLAGS+= -j$(EXTMAXPROCESS)

.ELSE

.IF "$(OS)"=="MACOSX" || "$(OS)"=="FREEBSD"
# Do not link against expat.  It is not necessary (apr-util is already linked against it)
# and does not work (we use a different expat library schema.)
PATCH_FILES=$(TARFILE_NAME).mac.patch
.ELSE
# Add -ldl as last library so that the linker has no trouble resolving dependencies.
PATCH_FILES=$(TARFILE_NAME).ldl.patch
.ENDIF

CONFIGURE_DIR=
CONFIGURE_ACTION=autoconf && .$/configure
.IF "$(OS)"=="MACOSX"
# On Mac we need the content of CDEFS in CFLAGS so that the ssl headers are searched for 
# in a directory that corresponds to the directory that is searched for the ssl library.
CONFIGURE_FLAGS='CFLAGS=$(CDEFS)'
.ENDIF

BUILD_DIR=$(CONFIGURE_DIR)
BUILD_ACTION=$(GNUMAKE)
BUILD_FLAGS+= -j$(EXTMAXPROCESS)

OUT2INC+=serf*.h
OUT2INC_SUBDIR=serf

.IF "$(OS)"=="MACOSX"
OUT2LIB+=.libs/libserf-1.*dylib
.ELSE
OUT2LIB=.libs/libserf-1.so*
.ENDIF

.ENDIF


# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk

