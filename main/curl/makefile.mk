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

PRJNAME=so_curl
TARGET=so_curl

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

.IF "$(SYSTEM_CURL)" == "YES"
all:
	@echo "An already available installation of curl should exist on your system."
	@echo "Therefore the version provided here does not need to be built in addition."
.ENDIF

# --- Files --------------------------------------------------------

TARFILE_NAME=curl-7.72.0
TARFILE_MD5=7422feb126df677d2d33294a1fd079ea
PATCH_FILES=buildssh.patch

#ADDITIONAL_FILES= lib$/config-os2.h lib$/Makefile.os2

.IF "$(GUI)"=="UNX"

.IF "$(SYSBASE)"!=""
curl_CFLAGS+=-I$(SYSBASE)$/usr$/include
curl_LDFLAGS+=-L$(SYSBASE)$/usr$/lib
.ENDIF			# "$(SYSBASE)"!=""

.IF "$(OS)$(CPU)"=="SOLARISU"
curl_CFLAGS+:=$(ARCH_FLAGS)
curl_LDFLAGS+:=$(ARCH_FLAGS)
.ENDIF

CONFIGURE_DIR=.$/
#relative to CONFIGURE_DIR
CONFIGURE_ACTION=.$/configure
CONFIGURE_FLAGS= --without-ssl --without-libidn --enable-ftp --enable-ipv6 --enable-http --disable-gopher --disable-file --disable-ldap --disable-telnet --disable-dict --disable-static CPPFLAGS="$(curl_CFLAGS)"  LDFLAGS="$(curl_LDFLAGS)"

BUILD_DIR=$(CONFIGURE_DIR)$/lib
BUILD_ACTION=$(GNUMAKE)
BUILD_FLAGS+= -j$(EXTMAXPROCESS)

OUT2LIB=$(BUILD_DIR)$/.libs$/libcurl$(DLLPOST).4
.ENDIF			# "$(GUI)"=="UNX"


.IF "$(GUI)"=="WNT"
.IF "$(COM)"=="GCC"
curl_CC=$(CC) -mthreads
.IF "$(MINGW_SHARED_GCCLIB)"=="YES"
curl_CC+=-shared-libgcc
.ENDIF
curl_LIBS=-lws2_32 -lwinmm
.IF "$(MINGW_SHARED_GXXLIB)"=="YES"
curl_LIBS+=$(MINGW_SHARED_LIBSTDCPP)
.ENDIF
CONFIGURE_DIR=.$/
#relative to CONFIGURE_DIR
CONFIGURE_ACTION=.$/configure
CONFIGURE_FLAGS= --without-ssl --enable-ftp --enable-ipv6 --disable-http --disable-gopher --disable-file --disable-ldap --disable-telnet --disable-dict --build=i586-pc-mingw32 --host=i586-pc-mingw32 CC="$(curl_CC)" CPPFLAGS="$(INCLUDE)" OBJDUMP="objdump" LDFLAGS="-L$(ILIB:s/;/ -L/)" LIBS="$(curl_LIBS)"
BUILD_DIR=$(CONFIGURE_DIR)$/lib
BUILD_ACTION=make
OUT2BIN=$(BUILD_DIR)$/.libs$/libcurl*.dll
OUT2LIB=$(BUILD_DIR)$/.libs$/libcurl*.dll.a
.ELSE
# make use of stlport headerfiles
EXT_USE_STLPORT=TRUE

.IF "$(CCNUMVER)" > "001399999999"
EXCFLAGS="/EHa /Zc:wchar_t- /D "_CRT_SECURE_NO_DEPRECATE""
.ELSE
EXCFLAGS="/EHsc /YX"
.ENDIF

.IF "$(CPUNAME)"=="INTEL"
curl_MACHINE:="X86"
.ELIF "$(CPUNAME)"=="X86_64"
curl_MACHINE:="X64"
.ENDIF

BUILD_DIR=.$/winbuild
.IF "$(debug)"==""
BUILD_ACTION=CC="cl.exe" nmake -f Makefile.vc mode=dll VC=9 EXCFLAGS=$(EXCFLAGS) MACHINE=$(curl_MACHINE)
.ELSE
BUILD_ACTION=CC="cl.exe" nmake -f Makefile.vc mode=dll VC=9 DEBUG=yes EXCFLAGS=$(EXCFLAGS) MACHINE=$(curl_MACHINE)
.ENDIF

.IF "$(CPUNAME)"=="INTEL"
OUT2BIN=$(BUILD_DIR)$/../builds/libcurl-vc9-X86-release-dll-ipv6-sspi-schannel/bin/libcurl.dll
OUT2LIB=$(BUILD_DIR)$/../builds/libcurl-vc9-X86-release-dll-ipv6-sspi-schannel/lib/libcurl.lib
.ELIF "$(CPUNAME)"=="X86_64"
OUT2BIN=$(BUILD_DIR)$/../builds/libcurl-vc9-X64-release-dll-ipv6-sspi-schannel/bin/libcurl.dll
OUT2LIB=$(BUILD_DIR)$/../builds/libcurl-vc9-X64-release-dll-ipv6-sspi-schannel/lib/libcurl.lib
.ENDIF

.ENDIF
.ENDIF			# "$(GUI)"=="WNT"

.IF "$(GUI)"=="OS2"
# make use of stlport headerfiles
EXT_USE_STLPORT=TRUE

BUILD_DIR=.$/lib
.IF "$(debug)"==""
BUILD_ACTION=make -f Makefile.os2
.ELSE
BUILD_ACTION=make -f Makefile.os2
.ENDIF

OUT2BIN=$(BUILD_DIR)$/libcurl.dll
OUT2LIB=$(BUILD_DIR)$/libcurl.lib

.ENDIF			# "$(GUI)"=="OS2"

OUT2INC= \
	include$/curl$/easy.h  			\
	include$/curl$/multi.h  		\
	include$/curl$/curl.h  			\
	include$/curl$/curlver.h  		\
	include$/curl$/typecheck-gcc.h  	\
	include$/curl$/stdcheaders.h  	\
	include$/curl$/mprintf.h	    \
	include$/curl$/system.h			\
	include$/curl$/urlapi.h


# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk
