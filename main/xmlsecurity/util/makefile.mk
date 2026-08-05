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



PRJ=..

PRJNAME=xmlsecurity
TARGET=xmlsecurity

# Disable '-z defs' due to broken libxpcom.
#LINKFLAGSDEFS=$(0)
USE_DEFFILE=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :	$(PRJ)$/util$/target.pmk

.IF "$(ENABLE_NSS_MODULE)" != "YES"
@all:
	@echo "No nss -> no libxmlsec -> no xmlsecurity..."
.ENDIF

# --- Files --------------------------------------------------------

BMP_IN=$(PRJ)$/res

# --- Shared-Library -----------------------------------------------

#
# The 1st shared library
#
SHL1NAME=xsec_fw
SHL1TARGET= $(SHL1NAME)
SHL1LIBS= $(SLB)$/fw.lib

SHL1STDLIBS +=		\
	$(SALLIB)		\
	$(CPPULIB)		\
	$(CPPUHELPERLIB)

SHL1IMPLIB = $(SHL1TARGET)
SHL1DEF = $(MISC)$/$(SHL1TARGET).def
DEF1NAME = $(SHL1TARGET)
DEF1EXPORTFILE = xsec_fw.dxp

#
# The 2nd shared library
#

SHL2NAME=xsec_xmlsec
.IF "$(GUI)"=="OS2"
SHL2NAME=xsec_xs
.ENDIF
SHL2TARGET= $(SHL2NAME)


SHL2LIBS= \
	$(SLB)$/xs_comm.lib

.IF "$(CRYPTO_ENGINE)" == "mscrypto"
SHL2LIBS += \
	$(SLB)$/xs_mscrypt.lib
.ENDIF

SHL2LIBS += \
	$(SLB)$/xs_nss.lib


SHL2STDLIBS +=			\
	$(SALLIB)			\
	$(CPPULIB)			\
	$(CPPUHELPERLIB)	\
	$(SALLIB)	\
	$(SVLLIB)			\
	$(TOOLSLIB)			\
	$(COMPHELPERLIB)	\
	$(CPPUHELPERLIB)	\
	$(XMLOFFLIB)

.IF "$(OS)"=="SOLARIS"
SHL2STDLIBS +=-ldl
.ENDIF

.IF "$(ENABLE_NSS_MODULE)"=="YES"
.IF "$(SYSTEM_NSS)"!="YES"
.IF "$(NSPR_LIB)" != ""
SHL2STDLIBS += $(NSPR_LIB)
.ENDIF
.IF "$(NSS_LIB)" != ""
SHL2STDLIBS += $(NSS_LIB)
.ENDIF
.ELSE
SHL2STDLIBS += $(NSS_LIBS)
.ENDIF
.ENDIF

.IF "$(CRYPTO_ENGINE)" == "mscrypto"
SHL2STDLIBS+= $(MSCRYPTOLIBS)
# SHL2STDLIBS+= $(XMLSECLIB) $(LIBXML2LIB) $(NSS3LIB) $(NSPR4LIB) $(PLC4LIB)
SHL2STDLIBS+= $(NSS3LIB) $(NSPR4LIB)
.ELIF "$(OS)$(SYSTEM_LIBXML)"=="MACOSXYES"
# The macOS shared-library link template (tg_shl.mk) always searches
# $(SOLARLIB) -- which includes /usr/lib -- before any of this makefile's
# own libs, and the SDK bundles its own older libxml2 under /usr/lib. A
# plain "-lxml2" (as pulled in by NSSCRYPTOLIBS via LIBXML2LIB) resolves
# to that bundled copy instead of the configured --with-system-libxml one,
# and it is missing symbols (xmlCtxtPushInput, xmlXPathValuePush) that
# xmlsec1 needs. Link the intended libxml2 by absolute path so the search
# order can't shadow it.
#
# The path is built from $(LIBXML_PREFIX), not a live `xml2-config`
# lookup: $(LIBXML_PREFIX) is resolved exactly once by `./configure`
# (honoring the directory given to --with-system-libxml=DIR, see
# configure.ac's libxml check) and exported via the generated
# *Env.Set.sh script -- the same mechanism that already delivers
# $(LIBXML_CFLAGS)/$(LIBXML_LIBS) (aka $(LIBXML2LIB), solenv/inc/libs.mk)
# to every other module. Re-running `xml2-config` here at build time
# instead would re-do that PATH-dependent lookup a second time,
# independently of configure -- if the build-resume shell's PATH ever
# differs from the configure shell's (e.g. MacPorts' /opt/local ends up
# ahead of the intended prefix), that would silently link a stray
# libxml2.a instead of the intended one, pulling in a different, unrelated
# set of missing transitive symbols (seen in practice: ICU + GNU libiconv
# from a MacPorts copy on one machine, liblzma from a different stray copy
# on another). Going through $(LIBXML_PREFIX) removes that ambiguity.
#
# Prefer the dylib when one is actually present at that prefix: unlike a
# static archive, a dylib records its own transitive links (e.g. to libz),
# so the linker resolves those automatically and no extra -lz/-llzma is
# needed. Fall back to the static archive (community builds normally only
# install that, via --enable-shared=no) when no dylib exists there.
#
# The existence check is resolved into a plain macro first, then branched
# on -- dmake's .IF does not reliably evaluate a $(shell ...) call written
# directly inside the condition string itself.
XMLSECURITY_LIBXML2_HAS_DYLIB:=$(shell test -f $(LIBXML_PREFIX)/lib/libxml2.dylib && echo yes)
.IF "$(XMLSECURITY_LIBXML2_HAS_DYLIB)"=="yes"
XMLSECURITY_SYSTEM_LIBXML2:=$(LIBXML_PREFIX)/lib/libxml2.dylib
XMLSECURITY_SYSTEM_LIBXML2_EXTRALIBS:=
.ELSE
XMLSECURITY_SYSTEM_LIBXML2:=$(LIBXML_PREFIX)/lib/libxml2.a
# The static archive carries no dependency info, so zlib must be linked
# explicitly here to satisfy libxml2's HTTP/gzip symbols (deflate,
# inflate, gzopen, ...). liblzma is linked only if actually present next
# to it: it's not part of the documented static-lib bundle, so only
# needed if this libxml2 build happens to have xz support compiled in.
XMLSECURITY_SYSTEM_LIBLZMA:=$(shell test -f $(LIBXML_PREFIX)/lib/liblzma.a -o -f $(LIBXML_PREFIX)/lib/liblzma.dylib && echo $(LIBXML_PREFIX)/lib/liblzma.a)
XMLSECURITY_SYSTEM_LIBXML2_EXTRALIBS:=$(ZLIB3RDLIB) $(XMLSECURITY_SYSTEM_LIBLZMA)
.ENDIF
SHL2STDLIBS+= $(XMLSECLIB-NSS) $(XMLSECLIB) $(XMLSECURITY_SYSTEM_LIBXML2) $(XMLSECURITY_SYSTEM_LIBXML2_EXTRALIBS) $(NSS3LIB) $(NSPR4LIB) $(PLC4LIB)
.ELSE
SHL2STDLIBS+= $(NSSCRYPTOLIBS)
.ENDIF


SHL2IMPLIB = $(SHL2TARGET)
SHL2DEF = $(MISC)$/$(SHL2TARGET).def
DEF2NAME = $(SHL2TARGET)
.IF "$(CRYPTO_ENGINE)" == "mscrypto"
DEF2EXPORTFILE = exports_xsmscrypt.dxp
.ENDIF

DEF2EXPORTFILE = exports_xsnss.dxp

SRSFILELIST=	\
                $(SRS)$/component.srs   \
                $(SRS)$/dialogs.srs

RESLIB1NAME=xmlsec
RESLIB1IMAGES=$(PRJ)$/res
RESLIB1SRSFILES= $(SRSFILELIST)

SHL4TARGET=$(TARGET)
.IF "$(GUI)"=="OS2"
SHL4TARGET=xmlsecur
.ENDIF
SHL4LIBS=\
                $(SLB)$/helper.lib      \
                $(SLB)$/dialogs.lib     \
                $(SLB)$/component.lib

SHL4STDLIBS=\
                $(CPPULIB)			\
				$(CPPUHELPERLIB)	\
				$(COMPHELPERLIB)	\
				$(UCBHELPERLIB)	    \
				$(UNOTOOLSLIB)	    \
				$(VCLLIB)			\
				$(TOOLSLIB) 		\
				$(SVTOOLLIB) 		\
				$(SALLIB)			\
				$(SVLLIB)			\
				$(XMLOFFLIB)		\
				$(SVXCORELIB)

SHL4VERSIONMAP = $(SOLARENV)/src/component.map
SHL4DEPN=
SHL4IMPLIB=i$(TARGET)
SHL4DEF=$(MISC)$/$(SHL4TARGET).def
DEF4NAME=$(SHL4TARGET)

# --- Targets ----------------------------------------------------------

.INCLUDE :  target.mk

# --- Filter -----------------------------------------------------------

$(MISC)$/$(SHL3TARGET).flt: makefile.mk
	$(TYPE) $(SHL3TARGET).flt > $@

ALLTAR : \
    $(MISC)/xmlsecurity.component \
    $(MISC)/xsec_fw.component \
    $(MISC)/xsec_xmlsec.component

.IF "$(OS)" == "WNT"
my_platform = .windows
.END

$(MISC)/xmlsecurity.component .ERRREMOVE : \
        $(SOLARENV)/bin/createcomponent.xslt xmlsecurity.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL4TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt xmlsecurity.component

$(MISC)/xsec_fw.component .ERRREMOVE : $(SOLARENV)/bin/createcomponent.xslt \
        xsec_fw.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL1TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt xsec_fw.component

$(MISC)/xsec_xmlsec.component .ERRREMOVE : \
        $(SOLARENV)/bin/createcomponent.xslt xsec_xmlsec.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL2TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt xsec_xmlsec$(my_platform).component
