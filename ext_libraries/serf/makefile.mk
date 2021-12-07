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
TARFILE_MD5=26015c63e3bbb108c1689bf2090e4c26351db674

PATCH_FILES+=$(TARFILE_NAME).scons.patch

.IF "$(OS)"=="WNT"

PATCH_FILES+=$(TARFILE_NAME).wnt.patch
CONFIGURE_DIR=
CONFIGURE_ACTION=
CONFIGURE_FLAGS=
SCONS_FLAGS=MSVC_VERSION=9.0

OPENSSLINCDIR=external
SCONS_FLAGS+=OPENSSL=$(SOLARINCDIR)/$(OPENSSLINCDIR)
SCONS_FLAGS+=APR=$(OUTDIR)
SCONS_FLAGS+=APU=$(OUTDIR)
ZLIBINCDIR=external
SCONS_FLAGS+=ZLIB=$(SOLARINCDIR)/$(ZLIBINCDIR)

BUILD_DIR=$(CONFIGURE_DIR)
BUILD_ACTION=$(SCONS)
BUILD_FLAGS=$(SCONS_FLAGS)

OUT2INC+=serf*.h
OUT2INC_SUBDIR=serf
OUT2LIB+=libserf-1.lib libserf-1.dll

.ELIF "$(GUI)" == "OS2"

@all:
	@echo "using system serf. nothing do do."

.ELSE

# Export ENABLE_SERF_LOGGING=YES to enable serf logging
.IF "$(ENABLE_SERF_LOGGING)" == "YES"
PATCH_FILES+=$(TARFILE_NAME).logging.patch
CDEFS+=-DENABLE_SERF_VERBOSE -DSERF_VERBOSE
.ENDIF

CONFIGURE_DIR=
CONFIGURE_ACTION=
CONFIGURE_FLAGS=
SCONS_FLAGS=

.IF "$(SYSTEM_OPENSSL)"!="YES"
OPENSSLINCDIR=external
SCONS_FLAGS+=OPENSSL=$(SOLARINCDIR)/$(OPENSSLINCDIR)
.ENDIF

.IF "$(SYSTEM_APR)"!="YES"
SCONS_FLAGS+=APR=$(OUTDIR)
SCONS_FLAGS+=APU=$(OUTDIR)
.ENDIF

BUILD_DIR=$(CONFIGURE_DIR)
BUILD_ACTION=$(SCONS)
BUILD_FLAGS=$(SCONS_FLAGS)

OUT2INC+=serf*.h
OUT2INC_SUBDIR=serf

.IF "$(OS)"=="MACOSX"
# Serf names its library only with the major number.
# We are using minor and micro as well.  Fix that here
# by creating a copy with the right name.
SERF_ORIGINAL_LIB=libserf-$(SERF_MAJOR).0.dylib
SERF_FIXED_LIB=libserf-$(LIBSERFVERSION).0.dylib
INSTALL_ACTION=if [ -f "$(SERF_ORIGINAL_LIB)" -a ! -f "$(SERF_FIXED_LIB)" ]; then cp $(SERF_ORIGINAL_LIB) $(SERF_FIXED_LIB); fi	 
.ENDIF

OUT2INC+=serf*.h
OUT2INC_SUBDIR=serf

.IF "$(OS)"=="MACOSX"
OUT2LIB+=$(SERF_FIXED_LIB)
OUT2LIB+=/libserf-1.*dylib
.ELSE
OUT2LIB=libserf-1.so*
.ENDIF

.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk

.ENDIF
