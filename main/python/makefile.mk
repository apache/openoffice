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

# CPython's own vendored expat (Modules/expat/expat.h) renames every public
# symbol via pyexpatns.h so it can't clash with any expat also loaded into the
# process. The solver's external include dir below also carries AOO's own
# flat expat.h (main/expat delivers it to inc/external/expat.h for other
# modules), which has no such renaming. Since -I dirs are searched in the
# order given, put Python's own Modules/expat ahead of the solver dir so
# "expat.h" resolves to CPython's copy — otherwise pyexpat.c/_elementtree.c
# compile against the un-renamed declarations while linking against the
# renamed symbols in libexpat.a, leaving XML_SetCommentHandler (and friends)
# undefined at import time.
python_CFLAGS+=-I$(MYCWD)$/$(TARFILE_NAME)$/Modules$/expat

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

.IF "$(OS)"=="LINUX"
# CPython's sysconfig module names its generated data module after the
# platform/ABI (e.g. _sysconfigdata__linux_x86_64-linux-gnu.py) instead of
# the plain "_sysconfigdata.py" that main/python/prj/d.lst delivers. Rename
# it in place so delivery finds the name it's looking for.
BUILD_ACTION+= && for d in $(MYCWD)/python-inst/lib/python3.11 $(MYCWD)/python-inst/lib64/python3.11; do test -d "$$d" && (cd "$$d" && for f in _sysconfigdata_*.py; do test -f "$$f" && mv -f "$$f" _sysconfigdata.py; done); done; true
.ENDIF

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

# The deferral above comes due here.  VS2008 cannot build Python 3.11 at all --
# that is why trunk left this commented out -- but the UCRT generation can,
# through PCbuild and MSBuild.  Same move CoinMP made on this branch, for the
# same reason: vcbuild.exe does not exist any more.  VC9 keeps the deferral,
# because for VC9 it is still true.
.IF "$(COMEX)"=="14"

# pcbuild.proj, not pcbuild.sln: the solution builds every project
# unconditionally, while the .proj is what understands the switches below.
BUILD_DIR=PCbuild

# The projects are named one by one rather than letting pcbuild.proj pick them,
# because its selection is all-or-nothing in the wrong place.  Switching its
# IncludeExternals off drops _bz2, _lzma and _sqlite3 -- which we do want
# dropped, their sources are downloaded by get_externals.bat and an offline
# build cannot run it -- but the SAME property also decides whether pythoncore
# gets zlib, and pythoncore without zlib does not compile:  binascii.c is
# handed USE_ZLIB_CRC32 unconditionally and then cannot find zlib.h.  CPython
# says as much itself, in a _WarnAboutZlib target reading "Not including zlib
# is not a supported configuration."
#
# So: build from the solution, name the targets, and give pythoncore the zlib
# this tree already has.  Everything below needs no external source at all.
# Built one project at a time, and NOT through pcbuild.sln, because the
# solution carries dependencies that have nothing to do with compilation
# order: python.vcxproj declares one on _ctypes purely so that a developer
# building 'python' in the IDE gets a usable interpreter.  Going through the
# solution therefore drags in libffi, which we do not have and do not want.
# A .vcxproj built directly honours its own ProjectReferences -- python still
# builds pythoncore first, and pythoncore still builds _freeze_module -- and
# ignores the solution's editorial ones.
PYTHON_PROJECTS=pythoncore python pythonw _socket select unicodedata pyexpat _elementtree _multiprocessing _overlapped _asyncio _queue _uuid _zoneinfo winsound _decimal _msi

# main/zlib's unpacked source, in the form MSBuild wants.  Globbed rather than
# spelled out so a zlib version bump does not silently miss it -- pythoncore
# compiles these sources into itself, so it needs the .c files and not just the
# headers we deliver to solver.
PYTHON_ZLIB_DIR=$(shell cygpath -m `ls -d $(PRJ)$/..$/zlib$/$(INPATH)$/misc$/build$/zlib-*$/ | head -1`)

# PlatformToolset and WindowsTargetPlatformVersion are passed for the reason
# CoinMP passes them: the projects default to a newer toolset than is installed.
PYTHON_MSBUILDFLAGS=\
	/p:Configuration=Release \
	/p:PlatformToolset=$(VCTOOLSET) \
	/p:WindowsTargetPlatformVersion=$(WINDOWS_SDK_VERSION) \
	/p:zlibDir=$(PYTHON_ZLIB_DIR) \
	/p:IncludeExternals=true \
	/p:IncludeSSL=false \
	/p:IncludeCTypes=false \
	/p:IncludeTkinter=false \
	/p:IncludeTests=false

.IF "$(CPUNAME)"=="INTEL"
PYTHON_MSBUILDPLATFORM=Win32
.ELIF "$(CPUNAME)"=="X86_64"
PYTHON_MSBUILDPLATFORM=x64
.ENDIF

BUILD_ACTION=for p in $(PYTHON_PROJECTS) ; do $(MSBUILD_PATH)$/MSBuild.exe $$p.vcxproj /p:Platform=$(PYTHON_MSBUILDPLATFORM) $(PYTHON_MSBUILDFLAGS) || exit 1 ; done

.ENDIF			# "$(COMEX)"=="14"
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
