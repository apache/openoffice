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

PRJNAME=so_python
TARGET=so_python

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk
.INCLUDE :      pyversion_dmake.mk

.IF "$(SYSTEM_PYTHON)" == "YES"
all:
	@echo "An already available installation of python should exist on your system."
	@echo "Therefore the version provided here does not need to be built in addition."
.ENDIF


# --- Files --------------------------------------------------------


TARFILE_NAME=Python-$(PYVERSION)
TARFILE_MD5=57f84cbd92d478ceff55217d88a07ded
PATCH_FILES=\
	python-md5.patch \
	python-solver-before-std.patch \
	python-$(PYVERSION)-sysbase.patch \
	python-$(PYVERSION)-nohardlink.patch
# python-freebsd.patch: all hunks obsolete in Python 3.11 (upstream fixes).
# python-ssl.patch: dropped. Python 3.11's setup.py finds the bundled OpenSSL
#   through the compiler include/lib search paths instead of the old hard-coded
#   source edit; see python_CFLAGS/python_LDFLAGS in the UNX branch below.

# Windows build deferred — Python 3 requires modern MSVC (VS2008/msvs9 not supported).
# The msvs9 patch files below are retained for reference but are NOT applied until
# the Windows toolchain is modernised. See python3 migration notes.
#.IF "$(GUI)"=="WNT"
#.IF "$(CPUNAME)"=="INTEL"
#PATCH_FILES += python-$(PYVERSION)-msvs9.patch
#.ELIF "$(CPUNAME)"=="X86_64"
#PATCH_FILES += \
#	python-$(PYVERSION)-msvs9-win64.patch \
#	python-$(PYVERSION)-msvs9-win64-target.patch \
#	python-$(PYVERSION)-msvs9-subsystem.patch \
#	python-$(PYVERSION)-msvs9-dir.patch \
#	python-$(PYVERSION)-msvs9-no-host-python.patch \
#	python-$(PYVERSION)-msvs9-python-path.patch
#.ENDIF
#.ENDIF

CONFIGURE_DIR=

.IF "$(GUI)"=="UNX"
BUILD_DIR=
MYCWD=$(shell @pwd)/$(INPATH)/misc/build

# Point Python's setup.py at the bundled OpenSSL delivered to the solver.
# (Replaces the old python-ssl.patch: Python 3.11 discovers _ssl/_hashlib via
# the C compiler include/lib search paths, so feeding the solver's external
# include dir and lib dir through CFLAGS/LDFLAGS is sufficient. Headers live
# in $(SOLARINCDIR)/external/openssl/, so the search dir is .../external so
# that setup.py's "openssl/ssl.h" probe resolves.)
python_CFLAGS+=-I$(SOLARINCDIR)$/external
python_LDFLAGS+=-L$(SOLARLIBDIR)

# CLFLAGS get overwritten in Makefile.pre.in
.IF "$(SYSBASE)"!=""
CC+:=-I$(SYSBASE)$/usr$/include
python_LDFLAGS+=-L$(SYSBASE)/usr/lib
.IF "$(COMNAME)"=="sunpro5"
CC+:=$(C_RESTRICTIONFLAGS)
.ENDIF			# "$(COMNAME)"=="sunpro5"
.ENDIF			# "$(SYSBASE)"!=""

.IF "$(OS)$(CPU)"=="SOLARISU"
CC+:=$(ARCH_FLAGS)
python_LDFLAGS+=$(ARCH_FLAGS)
.ENDIF

CONFIGURE_ACTION=$(AUGMENT_LIBRARY_PATH) ./configure --prefix=$(MYCWD)/python-inst --enable-shared CFLAGS="$(python_CFLAGS)" LDFLAGS="$(python_LDFLAGS)"
.IF "$(OS)$(CPU)" == "SOLARISI"
CONFIGURE_ACTION += --disable-ipv6
.ENDIF
BUILD_ACTION=$(ENV_BUILD) $(GNUMAKE) -j$(EXTMAXPROCESS) && $(GNUMAKE) install && chmod -R ug+w $(MYCWD)/python-inst && chmod g+w Include
.ELSE
# ----------------------------------
# WINDOWS
# ----------------------------------
.IF "$(COM)"=="GCC"
#PATCH_FILES=python-$(PYVERSION)-mingw.patch
BUILD_DIR=
MYCWD=$(shell cygpath -m $(shell @pwd))/$(INPATH)/misc/build
python_CFLAGS=-mno-cygwin -mthreads
python_LDFLAGS=-mno-cygwin -mthreads
.IF "$(MINGW_SHARED_GCCLIB)"=="YES"
python_LDFLAGS+=-shared-libgcc
.ENDIF
python_LDFLAGS+=-shared-libgcc -Wl,--enable-runtime-pseudo-reloc-v2
CONFIGURE_ACTION=./configure --prefix=$(MYCWD)/python-inst --enable-shared CC="$(CC:s/guw.exe //)" CXX="$(CXX:s/guw.exe //)" MACHDEP=MINGW32 LN="cp -p" CFLAGS="$(python_CFLAGS)" LDFLAGS="$(python_LDFLAGS)"
BUILD_ACTION=$(ENV_BUILD) make && make install
.ELSE
#PYTHONPATH:=..$/Lib
#.EXPORT : PYTHONPATH

#.IF "$(CCNUMVER)" <= "001400000000"
#EXFLAGS="/GX /YX"
#.ELSE
#.IF "$(WINDOWS_VISTA_PSDK)"!=""
#EXFLAGS="/EHa /Zc:wchar_t- /D "_CRT_SECURE_NO_DEPRECATE""
#ADDITIONALLIBS=ws2_32.lib
#.ELSE  #"$(WINDOWS_VISTA_PSDK)"!=""
#EXFLAGS="/EHa /Zc:wchar_t- /D "_CRT_SECURE_NO_DEPRECATE""
#.ENDIF #"$(WINDOWS_VISTA_PSDK)"!=""
#.ENDIF

# Windows build deferred — Python 3 requires modern MSVC (PCbuild); VS9.0/vcbuild
# no longer supported. Retained for reference. See python3 migration notes.
#BUILD_DIR=PC/VS9.0
#.IF "$(CPUNAME)"=="INTEL"
#BUILD_ACTION=$(COMPATH)$/vcpackages$/vcbuild.exe -useenv pcbuild.sln "Release|Win32"
#.ELIF "$(CPUNAME)"=="X86_64"
#BUILD_ACTION=$(COMPATH)$/vcpackages$/vcbuild.exe -useenv pcbuild.sln "Release|x64"
#.ENDIF
.ENDIF
.ENDIF

PYVERSIONFILE_DMAKE=$(MISC)$/pyversion_dmake.mk
PYVERSIONFILE=$(MISC)$/pyversion.mk

# --- Targets ------------------------------------------------------


.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk

.IF "$(L10N_framework)"==""
.IF "$(GUI)" != "UNX"
.IF "$(COM)"!="GCC"
PYCONFIG:=$(MISC)$/build$/pyconfig.h
$(MISC)$/build$/$(TARFILE_NAME)$/PC$/pyconfig.h : $(PACKAGE_DIR)$/$(CONFIGURE_FLAG_FILE)

$(PACKAGE_DIR)$/$(BUILD_FLAG_FILE) : $(PYCONFIG)

$(PYCONFIG) : $(MISC)$/build$/$(TARFILE_NAME)$/PC$/pyconfig.h
	-rm -f $@
	cat $(MISC)$/build$/$(TARFILE_NAME)$/PC$/pyconfig.h > $@
.ENDIF
.ENDIF

ALLTAR : $(PYVERSIONFILE_DMAKE) $(PYVERSIONFILE)
.ENDIF          # "$(L10N_framework)"==""


$(PYVERSIONFILE_DMAKE) : pyversion_dmake.mk $(PACKAGE_DIR)$/$(PREDELIVER_FLAG_FILE)
	-rm -f $@
	cat $? > $@

$(PYVERSIONFILE) : pyversion.mk $(PACKAGE_DIR)$/$(PREDELIVER_FLAG_FILE)
	-rm -f $@
	cat $? > $@
