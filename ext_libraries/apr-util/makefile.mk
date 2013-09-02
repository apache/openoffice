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

PRJNAME=apr-util
TARGET=aprutil

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk
.INCLUDE :	aprutil_version.mk

# --- Files --------------------------------------------------------

.IF "$(SYSTEM_APR_UTIL)"=="YES"

all:
	@echo "Using system apr-util.  Does not have to be built."

.ELSE

# Assemble the full version number from the parts defined in aprutil_version.mk
APR_UTIL_VERSION=$(APR_UTIL_MAJOR).$(APR_UTIL_MINOR).$(APR_UTIL_MICRO)


TARFILE_NAME=$(PRJNAME)-$(APR_UTIL_VERSION)
TARFILE_MD5=666a5d56098a9debf998510e304c8095

.IF "$(OS)"=="WNT"

ADDITIONAL_FILES=Makefile Module_apr-util.mk Library_apr-util.mk Package_inc.mk

PATCH_FILES= \
    $(TARFILE_NAME).apu.hw.patch \
    $(TARFILE_NAME).makewin32.patch

CONFIGURE_DIR=
CONFIGURE_ACTION=cp include/apu.hw include/apu.h;							\
	cp include/apr_ldap.hw include/apr_ldap.h;								\
	cp include/private/apu_config.hw include/private/apu_config.h;			\
	cp include/private/apu_select_dbm.hw include/private/apu_select_dbm.h
CONFIGURE_FLAGS=

BUILD_DIR=$(CONFIGURE_DIR)
BUILD_ACTION=$(GNUMAKE)
#BUILD_FLAGS+= -j$(EXTMAXPROCESS)

.ELIF "$(GUI)" == "OS2"

@all:
	@echo "using system apr-util. nothing do do."


.ELSE



CONFIGURE_DIR=
CONFIGURE_ACTION=autoconf && .$/configure
CONFIGURE_FLAGS=								\
	--with-apr=$(OUTDIR)/bin/apr-1-config		\
	--prefix=$(OUTDIR)							\
	--includedir=$(OUTDIR)$/inc$/apr-util		\
	--with-iconv="no"				


# Use our own expat on the Mac.  Maybe we should do this on Linux, too? Yes!
.IF "$(OS)" == "MACOSX" || ("$(OS)" == "LINUX" && "$(SYSTEM_EXPAT)"!="YES")

expat_CPPFLAGS=-I$(SOLARINCDIR)$/external
expat_LDFLAGS+=-L$(SOLARLIBDIR)
CONFIGURE_FLAGS+= SOLARINCDIR=$(SOLARINCDIR) SOLARLIBDIR=$(SOLARLIBDIR) \
	CPPFLAGS=$(expat_CPPFLAGS) LDFLAGS=$(expat_LDFLAGS)

# The non-standard names of our expat libraries (yes, plural) make 
# a special handling in apr-utils configure necessary.
PATCH_FILES+= $(TARFILE_NAME).expat.patch

.ENDIF


BUILD_DIR=$(CONFIGURE_DIR)
BUILD_ACTION=$(GNUMAKE)
BUILD_FLAGS+= -j$(EXTMAXPROCESS)

# Make apu-1-config believe that apr-util has been installed.
INSTALL_ACTION=if [ -f apu-1-config.orig ]; then mv -f apu-1-config.orig apu-1-config; fi;	\
	cp apu-1-config apu-1-config.orig;							\
	sed -e "s/^location=source/location=installed/;s/^\(installbuilddir=.*\)\/.*/\\1\/lib\/apr-util\"/" apu-1-config > apu-1-config.installed;								\
	mv -f apu-1-config.installed apu-1-config;	\
	chmod +x apu-1-config

OUT2INC+=include$/apr*.h
OUT2INC+=include$/apu.h
OUT2INC_SUBDIR=apr-util

.IF "$(OS)"=="MACOSX"
OUT2LIB+=.libs/libaprutil-1.*dylib
.ELSE
OUT2LIB=.libs/libaprutil-1.so*
.ENDIF
OUT2BIN=apu-1-config

.ENDIF

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk

.ENDIF
