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

PRJNAME=serf
TARGET=so_serf

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk
.INCLUDE :	serf_version.mk

# --- Files --------------------------------------------------------

.IF "$(SYSTEM_SERF)"=="YES"

all:
	@echo "Using system serf.  Does not have to be built."

.ELSE

# Assemble the full version number from the parts defined in serf_version.mk
LIBSERFVERSION=$(SERF_MAJOR).$(SERF_MINOR).$(SERF_MICRO)

TARFILE_NAME=$(PRJNAME)-$(LIBSERFVERSION)
# This is the SHA1 checksum, not MD5 but tg_ext.mk does not now about this and,
# thankfully, does not care.
TARFILE_MD5=f65fbbd72926c8e7cf0dbd4ada03b0d226f461fd

PATCH_FILES=

.IF "$(OS)"=="WNT"

CONFIGURE_DIR=
CONFIGURE_ACTION=
CONFIGURE_FLAGS=

BUILD_DIR=
BUILD_ACTION=$(GNUMAKE)
BUILD_FLAGS+= -f ../../../../win/Makefile -j$(EXTMAXPROCESS)

.ELIF "$(GUI)" == "OS2"

@all:
	@echo "using system serf. nothing do do."

.ELSE

.IF "$(OS)"=="MACOSX" || "$(OS)"=="FREEBSD" || "$(OS)"=="LINUX"
# Do not link against expat.  It is not necessary (apr-util is already linked against it)
# and does not work (we use a different expat library schema.)
PATCH_FILES+=$(TARFILE_NAME).libs.patch
.ENDIF

.IF "$(OS)"=="LINUX"
# Add -ldl as last library so that the linker has no trouble resolving dependencies.
PATCH_FILES+=$(TARFILE_NAME).ldl.patch
.ENDIF

# Export ENABLE_SERF_LOGGING=YES to enable serf logging
.IF "$(ENABLE_SERF_LOGGING)" == "YES"
PATCH_FILES+=$(TARFILE_NAME).logging.patch
CDEFS+=-DENABLE_SERF_VERBOSE -DSERF_VERBOSE
.ENDIF

CONFIGURE_DIR=
CONFIGURE_ACTION=autoconf && .$/configure

.IF "$(OS)"=="LINUX"
.IF "$(SYSTEM_OPENSSL)"=="YES"
CDEFS+=$(OPENSSL_CFLAGS)
.ELSE
OPENSSLINCDIR=external
CDEFS+=-I$(SOLARINCDIR)$/$(OPENSSLINCDIR)
.ENDIF
.ENDIF

# On Linux/Mac we need the content of CDEFS in CFLAGS so that the ssl headers are searched for
.IF "$(OS)"=="MACOSX" || "$(OS)"=="LINUX"
# in a directory that corresponds to the directory that is searched for the ssl library.
CONFIGURE_FLAGS='CFLAGS=$(CDEFS)'
.ENDIF

BUILD_DIR=$(CONFIGURE_DIR)
BUILD_ACTION=$(GNUMAKE)
BUILD_FLAGS+= -j$(EXTMAXPROCESS)

.IF "$(OS)"=="MACOSX"
# Serf names its library only with the major number.
# We are using minor and micro as well.  Fix that here
# by creating a copy with the right name.
SERF_ORIGINAL_LIB=.libs/libserf-$(SERF_MAJOR).0.0.0.dylib
SERF_FIXED_LIB=.libs/libserf-$(LIBSERFVERSION).0.dylib
INSTALL_ACTION=if [ -f "$(SERF_ORIGINAL_LIB)" -a ! -f "$(SERF_FIXED_LIB)" ]; then cp $(SERF_ORIGINAL_LIB) $(SERF_FIXED_LIB); fi	 
.ENDIF

OUT2INC+=serf*.h
OUT2INC_SUBDIR=serf

.IF "$(OS)"=="MACOSX"
OUT2LIB+=$(SERF_FIXED_LIB)
OUT2LIB+=.libs/libserf-1.*dylib
.ELSE
OUT2LIB=.libs/libserf-1.so*
.ENDIF

.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk

.ENDIF
