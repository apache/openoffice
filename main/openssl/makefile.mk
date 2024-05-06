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

PRJNAME=openssl
TARGET=openssl

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

# --- Files --------------------------------------------------------

.IF "$(SYSTEM_OPENSSL)" == "YES"
@all:
	@echo "Using system openssl...."
.ENDIF


OPENSSL_NAME=openssl-3.0.13

TARFILE_NAME=$(OPENSSL_NAME)
TARFILE_MD5=c15e53a62711002901d3515ac8b30b86

CONFIGURE_DIR=.
CONFIGURE_ACTION=config
CONFIGURE_FLAGS=-I$(SYSBASE)$/usr$/include -L$(SYSBASE)$/usr$/lib shared

BUILD_DIR=.
BUILD_ACTION=make CC='$(CC)' build_libs

OUT2LIB = libssl.*
OUT2LIB += libcrypto.*
OUT2INC += include/openssl/*

UNAME=$(shell uname)

.IF "$(COM)"=="GCC" && "$(CCNUMVER)">="000400060000" || "$(OS)"=="WNT" && "$(NASM_PATH)"=="NO_NASM_HOME"
  NO_ASM="no-asm"
.ELSE
  NO_ASM=
.ENDIF

.IF "$(OS)" == "LINUX" || "$(OS)" == "FREEBSD"
	PATCH_FILES=opensslunx.patch
	.IF "$(CPU)" == "I"
		.IF "$(UNAME)" == "GNU/kFreeBSD"
			CONFIGURE_ACTION=Configure debian-kfreebsd-i386 no-dso no-shared $(NO_ASM)
		.ELIF "$(UNAME)" == "FreeBSD"
			CONFIGURE_ACTION=Configure BSD-x86-elf no-dso no-shared $(NO_ASM)
		.ELSE
			CONFIGURE_ACTION=Configure linux-generic32 no-dso no-shared $(NO_ASM)
		.ENDIF
	.ELIF "$(BUILD64)" == "1"
		.IF "$(UNAME)" == "GNU/kFreeBSD"
			CONFIGURE_ACTION=Configure debian-kfreebsd-amd64 no-dso no-shared $(NO_ASM)
		.ELIF "$(UNAME)" == "FreeBSD"
			CONFIGURE_ACTION=Configure BSD-x86_64 no-dso no-shared $(NO_ASM)
		.ELSE
			CONFIGURE_ACTION=Configure linux-x86_64 no-dso no-shared $(NO_ASM)
		.ENDIF
	.ELSE
		CONFIGURE_ACTION=Configure linux-generic32 no-dso no-shared $(NO_ASM)
	.ENDIF
	# if you build openssl as shared library you have to patch the Makefile.Shared "LD_LIBRARY_PATH=$$LD_LIBRARY_PATH \"
	#BUILD_ACTION=make 'SHARED_LDFLAGS=-Wl,--version-script=./lib$$(SHLIBDIRS)_OOo_0_9_8e.map'
.ENDIF

.IF "$(OS)" == "SOLARIS"
	PATCH_FILES=opensslsol.patch
	#BUILD_ACTION=make 'SHARED_LDFLAGS=-G -dy -z text -M./lib$$$$$$$$(SHLIBDIRS)_OOo_0_9_8e.map'

	# Use BUILD64 when 1 to select new specific 64bit Configurations if necessary

	.IF "$(CPUNAME)" == "INTEL" # Solaris INTEL
		.IF "$(CPU)" == "X"
		   CONFIGURE_ACTION=Configure solaris64-x86_64-cc
		.ELSE
		   CONFIGURE_ACTION=Configure solaris-x86-cc
		.ENDIF
	.ELIF "$(CPU)" == "U" # Solaris SPARC
	   CONFIGURE_ACTION=Configure solaris64-sparcv9-cc
	.ELSE
	   CONFIGURE_ACTION=Configure solaris-sparcv9-cc
	.ENDIF
.ENDIF

.IF "$(OS)" == "MACOSX"
	CONFIGURE_ACTION=Configure darwin64-x86_64-cc no-dso no-shared $(NO_ASM)
.ENDIF

.IF "$(OS)" == "WNT"

.IF "$(COM)"=="GCC"

PATCH_FILES=opensslmingw.patch
.IF "$(USE_MINGW)" == "cygwin"
CONFIGURE_ACTION=$(PERL) configure
CONFIGURE_FLAGS=mingw shared
INSTALL_ACTION=mv libcrypto.a libcrypto_static.a && mv libcrypto.dll.a libcrypto.a && mv libssl.a libssl_static.a && mv libssl.dll.a libssl.a
OUT2LIB = libcrypto_static.*
OUT2LIB += libssl_static.*
OUT2LIB += libcrypto.*
OUT2LIB += libssl.*
OUT2BIN = ssleay32.dll
OUT2BIN += libeay32.dll
.ELSE # "$(USE_MINGW)" == "cygwin"
CONFIGURE_ACTION=
BUILD_ACTION=cmd /c "ms\mingw32"
OUT2LIB = out/libcrypto_static.*
OUT2LIB += out/libssl_static.*
OUT2LIB += out/libcrypto.*
OUT2LIB += out/libssl.*
OUT2BIN = out/ssleay32.dll
OUT2BIN += out/libeay32.dll
.ENDIF # "$(USE_MINGW)" == "cygwin"

.ELSE # "$(COM)"=="GCC"

	PATCH_FILES=openssl.patch

	# Extract Strawberry Perl and use CONFIGURE_ACTION to insert it, and NASM which it needs, into the PATH.
	# Also define:
	# - certain constants absent in the old MSVC we use (INT64_MAX, INT64_MIN, UINT64_MAX).
	# - the minimum Windows version to support (_WIN32_WINDOWS=0x0400 for Windows 95, _WIN32_WINNT=0x0400 for Windows NT 4.0).
	# - OPENSSL_NO_ASYNC=1 to stop using async functions that require old Windows versions.
	STRAWBERRY_PERL_DIR=$(shell cygpath -u $(SOLARSRC)/openssl/$(INPATH)/misc/build/strawberry-perl)
	.IF "$(NASM_PATH)"=="NASM_IN_PATH"
		CONFIGURE_PATH=$(STRAWBERRY_PERL_DIR)/perl/bin:$(PATH)
	.ELSE
		NASM_PATH_NIX=$(shell cygpath -u $(NASM_PATH))
		CONFIGURE_PATH=$(STRAWBERRY_PERL_DIR)/perl/bin:$(NASM_PATH_NIX):$(PATH)
	.ENDIF
	CONFIGURE_ACTION=\
		if test -d "$(STRAWBERRY_PERL_DIR)"; \
		then echo Found Strawberry Perl; \
		else mkdir "$(STRAWBERRY_PERL_DIR)" && unzip -d $(STRAWBERRY_PERL_DIR) $(SOLARSRC)/../ext_sources/93fdfe261588bc82ab3a0bd4f5945b60-strawberry-perl-5.32.1.1-32bit-portable.zip ; \
		fi && \
		PATH="$(CONFIGURE_PATH)" \
		PERL= \
		CPPFLAGS="$(SOLARINC) \
			-DOPENSSL_NO_ASYNC=1 \
			-DWINVER=0x0400 \
			-D_WIN32_WINNT=0x0400 \
			-D_WIN32_WINDOWS=0x0400 \
			-DINT64_MAX=9223372036854775807i64 \
			-DINT64_MIN=(-9223372036854775807i64-1) \
			-DUINT64_MAX=0xffffffffffffffffui64" \
		RCFLAGS="$(SOLARINC)" \
		perl configure $(NO_ASM)
	.IF "$(CPUNAME)"=="INTEL"
		CONFIGURE_FLAGS=VC-WIN32
	.ELIF "$(CPUNAME)"=="X86_64"
		CONFIGURE_FLAGS=VC-WIN64A
	.ENDIF
	BUILD_ACTION=LIB="$(ILIB)" nmake

	OUT2BIN = libcrypto-3.dll
	OUT2BIN += libssl-3.dll
	OUT2INC = include$/openssl$/*

.ENDIF # "$(COM)"=="GCC"

.ENDIF # "$(OS)" == "WNT"


# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk

