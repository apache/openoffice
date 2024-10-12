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

PRJNAME=icu
TARGET=so_icu

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

# --- Files --------------------------------------------------------

.INCLUDE :	icuversion.mk

.IF "$(ICU_MICRO)"!="0"
TARFILE_NAME=icu4c-$(ICU_MAJOR)_$(ICU_MINOR)_$(ICU_MICRO)-src
TARFILE_MD5=e3738abd0d3ce1870dc1fd1f22bba5b1
.ELSE
TARFILE_NAME=icu4c-$(ICU_MAJOR)_$(ICU_MINOR)-src
TARFILE_MD5=
.ENDIF
TARFILE_ROOTDIR=icu

# TODO file icu-mp.patch does not seem to be required
PATCH_FILES=${TARFILE_NAME}.patch icu-win-layout.patch \
	icu-format-security.patch icu-win-icutu-dll-version.patch

# ADDITIONAL_FILES=

.IF "$(GUI)"=="UNX"
.IF "$(COMNAME)"=="sunpro5"
#.IF "$(BUILD_TOOLS)$/cc"=="$(shell +-which cc)"
#CC:=$(COMPATH)$/bin$/cc
#CXX:=$(COMPATH)$/bin$/CC
#.ENDIF          # "$(BUILD_TOOLS)$/cc"=="$(shell +-which cc)"
.ENDIF          # "$(COMNAME)"=="sunpro5"

.IF "$(SYSBASE)"!=""
icu_CFLAGS+=-I$(SYSBASE)$/usr$/include
.IF "$(COMNAME)"=="sunpro5"
icu_CFLAGS+=$(C_RESTRICTIONFLAGS)
.ENDIF			# "$(COMNAME)"=="sunpro5"
# add SYSBASE libraries and make certain that they are found *after* the
# icu build internal libraries - in case that icu is available in SYSBASE
# as well
icu_LDFLAGS+= -L../lib  -L../../lib -L../stubdata -L../../stubdata  -L$(SYSBASE)$/usr$/lib
.ENDIF			# "$(SYSBASE)"!=""

icu_CFLAGS+=-O $(ARCH_FLAGS) $(EXTRA_CDEFS)
icu_LDFLAGS+=$(EXTRA_LINKFLAGS)
icu_CXXFLAGS+=-O $(ARCH_FLAGS) $(EXTRA_CDEFS)

# remove conversion and transliteration data to reduce binary size.
CONFIGURE_ACTION=rm data/mappings/ucm*.mk data/translit/trn*.mk ;

# until someone introduces SOLARIS 64-bit builds
.IF "$(OS)"=="SOLARIS"
DISABLE_64BIT=--enable-64bit-libs=no
.ENDIF			# "$(OS)"=="SOLARIS"

.IF "$(HAVE_LD_HASH_STYLE)"  == "TRUE"
LDFLAGSADD += -Wl,--hash-style=both
.ENDIF

.IF "$(HAVE_LD_BSYMBOLIC_FUNCTIONS)"  == "TRUE"
LDFLAGSADD += -Wl,-Bsymbolic-functions -Wl,--dynamic-list-cpp-new -Wl,--dynamic-list-cpp-typeinfo
.ENDIF

# FreeBSD with gcc from ports needs -Wl,-rpath= to find the matching libstdc++ 
.IF "$(OS)"=="FREEBSD"
LDFLAGSADD+=$(FBSD_GCC_RPATH)
.ENDIF

CONFIGURE_DIR=source

CONFIGURE_ACTION+=sh -c 'CFLAGS="$(icu_CFLAGS)" CXXFLAGS="$(icu_CXXFLAGS)" LDFLAGS="$(icu_LDFLAGS) $(LDFLAGSADD)" ./configure --enable-layout --enable-static --enable-shared=yes $(DISABLE_64BIT)'

#CONFIGURE_FLAGS=--enable-layout --enable-static --enable-shared=yes --enable-64bit-libs=no
CONFIGURE_FLAGS=

# Use of
# CONFIGURE_ACTION=sh -c 'CFLAGS=-O CXXFLAGS=-O ./configure'
# CONFIGURE_FLAGS=--enable-layout --enable-static --enable-shared=yes --enable-64bit-libs=no
# doesn't work as it would result in
# sh -c 'CFLAGS=-O CXXFLAGS=-O ./configure' --enable-layout ...
# note the position of the single quotes.

BUILD_DIR=$(CONFIGURE_DIR)
BUILD_ACTION=$(AUGMENT_LIBRARY_PATH) $(GNUMAKE) -j$(EXTMAXPROCESS)
OUT2LIB= \
	$(BUILD_DIR)$/lib$/libicudata$(DLLPOST).$(ICU_MAJOR)$(ICU_MINOR).$(ICU_MICRO) \
	$(BUILD_DIR)$/lib$/libicudata$(DLLPOST).$(ICU_MAJOR)$(ICU_MINOR) \
	$(BUILD_DIR)$/lib$/libicudata$(DLLPOST) \
	$(BUILD_DIR)$/lib$/libicuuc$(DLLPOST).$(ICU_MAJOR)$(ICU_MINOR).$(ICU_MICRO) \
	$(BUILD_DIR)$/lib$/libicuuc$(DLLPOST).$(ICU_MAJOR)$(ICU_MINOR) \
	$(BUILD_DIR)$/lib$/libicuuc$(DLLPOST) \
	$(BUILD_DIR)$/lib$/libicui18n$(DLLPOST).$(ICU_MAJOR)$(ICU_MINOR).$(ICU_MICRO) \
	$(BUILD_DIR)$/lib$/libicui18n$(DLLPOST).$(ICU_MAJOR)$(ICU_MINOR) \
	$(BUILD_DIR)$/lib$/libicui18n$(DLLPOST) \
	$(BUILD_DIR)$/lib$/libicule$(DLLPOST).$(ICU_MAJOR)$(ICU_MINOR).$(ICU_MICRO) \
	$(BUILD_DIR)$/lib$/libicule$(DLLPOST).$(ICU_MAJOR)$(ICU_MINOR) \
	$(BUILD_DIR)$/lib$/libicule$(DLLPOST) \
	$(BUILD_DIR)$/lib$/libicutu$(DLLPOST).$(ICU_MAJOR)$(ICU_MINOR).$(ICU_MICRO) \
	$(BUILD_DIR)$/lib$/libicutu$(DLLPOST).$(ICU_MAJOR)$(ICU_MINOR) \
	$(BUILD_DIR)$/lib$/libicutu$(DLLPOST)

OUT2BIN= \
	$(BUILD_DIR)$/bin$/genccode \
	$(BUILD_DIR)$/bin$/genbrk \
	$(BUILD_DIR)$/bin$/gencmn

.ENDIF

.IF "$(GUI)"=="WNT"
CONFIGURE_DIR=source
.IF "$(COM)"=="GCC"
CONFIGURE_ACTION=rm data/mappings/ucm*.mk data/translit/trn*.mk ;
.IF "$(MINGW_SHARED_GCCLIB)"=="YES"
icu_LDFLAGS+=-shared-libgcc
.ENDIF
.IF "$(USE_MINGW)"=="cygwin"
icu_LDFLAGS+=-L$(COMPATH)/lib/mingw -L$(COMPATH)/lib/w32api
.ENDIF
icu_LDFLAGS+=-L$(COMPATH)$/lib
icu_LIBS=
.IF "$(MINGW_SHARED_GXXLIB)"=="YES"
icu_LIBS+=$(MINGW_SHARED_LIBSTDCPP)
.ENDIF
icu_LDFLAGS+=-Wl,--enable-runtime-pseudo-reloc-v2
CONFIGURE_ACTION+=sh -c 'CFLAGS="-O -D_MT" CXXFLAGS="-O -D_MT" LDFLAGS="$(icu_LDFLAGS)" LIBS="$(icu_LIBS)" ./configure --build=i586-pc-mingw32 --enable-layout --enable-static --enable-shared=yes --enable-64bit-libs=no'

#CONFIGURE_FLAGS=--enable-layout --enable-static --enable-shared=yes --enable-64bit-libs=no
CONFIGURE_FLAGS=

# Use of
# CONFIGURE_ACTION=sh -c 'CFLAGS=-O CXXFLAGS=-O ./configure'
# CONFIGURE_FLAGS=--enable-layout --enable-static --enable-shared=yes --enable-64bit-libs=no
# doesn't work as it would result in
# sh -c 'CFLAGS=-O CXXFLAGS=-O ./configure' --enable-layout ...
# note the position of the single quotes.

BUILD_DIR=$(CONFIGURE_DIR)
BUILD_ACTION=$(GNUMAKE)
OUT2LIB=

OUT2BIN= \
	$(BUILD_DIR)$/lib$/icudt$(ICU_MAJOR)$(ICU_MINOR)$(DLLPOST) \
	$(BUILD_DIR)$/lib$/icuuc$(ICU_MAJOR)$(ICU_MINOR)$(DLLPOST) \
	$(BUILD_DIR)$/lib$/icuin$(ICU_MAJOR)$(ICU_MINOR)$(DLLPOST) \
	$(BUILD_DIR)$/lib$/icule$(ICU_MAJOR)$(ICU_MINOR)$(DLLPOST) \
	$(BUILD_DIR)$/lib$/icutu$(ICU_MAJOR)$(ICU_MINOR)$(DLLPOST) \
	$(BUILD_DIR)$/bin$/genccode.exe \
	$(BUILD_DIR)$/bin$/genbrk.exe \
	$(BUILD_DIR)$/bin$/gencmn.exe

.ELSE
BUILD_DIR=source
.IF "full_debug" == ""

# Activating the debug mechanism produces incompatible libraries, you'd have
# at least to relink all modules that are directly using ICU. Note that library
# names get a 'd' appended and you'd have to edit the solenv/inc/libs.mk
# ICU*LIB macros as well. Normally you don't want all this.
#
# Instead, use the normal already existing Release build and edit the
# corresponding *.vcproj file of the section you're interested in. Make sure
# that
# - for the VCCLCompilerTool section the following line exists:
#   DebugInformationFormat="3"
# - and for the VCLinkerTool the line
#   GenerateDebugInformation="TRUE"
# Then delete the corresponding Release output directory, and delete the target
# flag files
# $(OUTPATH)/misc/build/so_built_so_icu
# $(OUTPATH)/misc/build/so_predeliver_so_icu
# and run dmake again, after which you may copy the resulting libraries to your
# OOo/SO installation.
ICU_BUILD_VERSION=Debug
ICU_BUILD_LIBPOST=d
.ELSE
ICU_BUILD_VERSION=Release
ICU_BUILD_LIBPOST=
.ENDIF

.IF "$(CPUNAME)"=="INTEL"
CONFIGURE_ACTION+= $(PERL) ..$/..$/..$/..$/..$/createmak.pl ..$/..$/..$/..$/..$/createmak.cfg .
.ELIF "$(CPUNAME)"=="X86_64"
CONFIGURE_ACTION+= $(PERL) ..$/..$/..$/..$/..$/createmak.pl ..$/..$/..$/..$/..$/createmakWin64.cfg .
.ENDIF

.IF "$(CCNUMVER)"<="001400000000"
BUILD_ACTION=cd allinone && nmake /f all.mak EXFLAGS="-EHsc" && cd ..$/..
.ELSE
BUILD_ACTION=cd allinone && nmake /f all.mak EXFLAGS="-EHa -Zc:wchar_t-" && cd ..$/..
.ENDIF

OUT2LIB= \
	$(BUILD_DIR)$/..$/lib$/icudata.lib \
	$(BUILD_DIR)$/..$/lib$/icuin$(ICU_BUILD_LIBPOST).lib \
	$(BUILD_DIR)$/..$/lib$/icuuc$(ICU_BUILD_LIBPOST).lib \
	$(BUILD_DIR)$/..$/lib$/icule$(ICU_BUILD_LIBPOST).lib \
	$(BUILD_DIR)$/..$/lib$/icutu$(ICU_BUILD_LIBPOST).lib

OUT2BIN= \
	$(BUILD_DIR)$/..$/bin$/icudt$(ICU_MAJOR)$(ICU_MINOR).dll \
	$(BUILD_DIR)$/..$/bin$/icuin$(ICU_MAJOR)$(ICU_MINOR)$(ICU_BUILD_LIBPOST).dll \
	$(BUILD_DIR)$/..$/bin$/icuuc$(ICU_MAJOR)$(ICU_MINOR)$(ICU_BUILD_LIBPOST).dll \
	$(BUILD_DIR)$/..$/bin$/icule$(ICU_MAJOR)$(ICU_MINOR)$(ICU_BUILD_LIBPOST).dll \
	$(BUILD_DIR)$/..$/bin$/icutu$(ICU_MAJOR)$(ICU_MINOR)$(ICU_BUILD_LIBPOST).dll \
	$(BUILD_DIR)$/..$/bin$/genccode.exe \
	$(BUILD_DIR)$/..$/bin$/genbrk.exe \
    $(BUILD_DIR)$/..$/bin$/gencmn.exe

.ENDIF
.ENDIF		# "$(GUI)"=="WNT"

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE :	target.mk
.INCLUDE :	tg_ext.mk

.IF "$(BINARY_PATCH_FILES)"!=""

$(PACKAGE_DIR)$/so_add_binary :  $(PACKAGE_DIR)$/$(ADD_FILES_FLAG_FILE)
	cd $(PACKAGE_DIR) && gunzip -c $(BACK_PATH)$(BINARY_PATCH_FILES) | tar -xvf -
	$(TOUCH) $(PACKAGE_DIR)$/so_add_binary

$(PACKAGE_DIR)$/$(CONFIGURE_FLAG_FILE) : $(PACKAGE_DIR)$/so_add_binary

.ENDIF

.IF "$(GUI)$(COM)"=="WNTGCC"
ALLTAR : \
	$(LB)$/icudata.lib \
	$(LB)$/icuin$(ICU_BUILD_LIBPOST).lib \
	$(LB)$/icuuc$(ICU_BUILD_LIBPOST).lib \
	$(LB)$/icule$(ICU_BUILD_LIBPOST).lib \
	$(LB)$/icutu$(ICU_BUILD_LIBPOST).lib

$(LB)$/icudata.lib : $(PACKAGE_DIR)$/$(PREDELIVER_FLAG_FILE)
	$(TOUCH) $@

$(LB)$/icuin$(ICU_BUILD_LIBPOST).lib : $(PACKAGE_DIR)$/$(PREDELIVER_FLAG_FILE)
	$(TOUCH) $@

$(LB)$/icuuc$(ICU_BUILD_LIBPOST).lib : $(PACKAGE_DIR)$/$(PREDELIVER_FLAG_FILE)
	$(TOUCH) $@

$(LB)$/icule$(ICU_BUILD_LIBPOST).lib : $(PACKAGE_DIR)$/$(PREDELIVER_FLAG_FILE)
	$(TOUCH) $@

$(LB)$/icutu$(ICU_BUILD_LIBPOST).lib : $(PACKAGE_DIR)$/$(PREDELIVER_FLAG_FILE)
	$(TOUCH) $@
.ENDIF

# Since you never know what will be in a patch (for example, it may already
# patch at configure level) or in the case of a binary patch, we remove the
# entire package directory if a patch is newer.
# Changes in this makefile could also make a complete build necessary if
# configure is affected.
$(PACKAGE_DIR)$/$(UNTAR_FLAG_FILE) : makefile.mk

