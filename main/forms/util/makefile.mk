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
PRJNAME=forms
TARGET=frm
USE_DEFFILE=TRUE

# --- Settings ----------------------------------

.INCLUDE :	settings.mk
.INCLUDE: $(PRJ)$/makefile.pmk

# --- Library -----------------------------------
# --- frm ---------------------------------------
LIB1TARGET=$(SLB)$/forms.lib
LIB1FILES=\
		$(SLB)$/common.lib \
		$(SLB)$/resource.lib \
		$(SLB)$/component.lib \
        $(SLB)$/helper.lib \
        $(SLB)$/solarcomponent.lib  \
        $(SLB)$/solarcontrol.lib \
        $(SLB)$/richtext.lib \
        $(SLB)$/runtime.lib \
		$(SLB)$/xforms.lib \
        $(SLB)$/xformssubmit.lib \
        $(SLB)$/xformsxpath.lib

SHL1TARGET=$(TARGET)$(DLLPOSTFIX)

.IF "$(OS)$(SYSTEM_LIBXML)"=="MACOSXYES"
# The macOS shared-library link template (tg_shl.mk) always searches
# $(SOLARLIB) -- which includes /usr/lib -- before any of this makefile's
# own libs, and the SDK bundles its own older libxml2 under /usr/lib. A
# plain "-lxml2" (as pulled in by LIBXML2LIB) resolves to that bundled
# copy instead of the configured --with-system-libxml one, and it is
# missing symbols (xmlXPathValuePush, xmlXPathValuePop) that xformsxpath
# needs. Link the intended libxml2 by absolute path so the search order
# can't shadow it.
#
# The path is built from $(LIBXML_PREFIX), not a live `xml2-config`
# lookup -- see the matching comment in xmlsecurity/util/makefile.mk for
# why: $(LIBXML_PREFIX) is resolved exactly once by `./configure`
# (honoring the directory given to --with-system-libxml=DIR) and exported
# via the generated *Env.Set.sh script, so it can't drift from what
# configure actually resolved the way a fresh `xml2-config` shell-out at
# dmake time could.
#
# Prefer the dylib when one is actually present at that prefix (it
# records its own transitive links, e.g. to libz, so nothing extra needs
# to be added); fall back to the static archive -- which carries no
# dependency info, so zlib must be linked explicitly to satisfy libxml2's
# HTTP/gzip symbols (deflate, inflate, gzopen, ...) -- when no dylib
# exists there.
#
# The existence check is resolved into a plain macro first, then branched
# on -- dmake's .IF does not reliably evaluate a $(shell ...) call written
# directly inside the condition string itself.
#
# The leading "-" is load-bearing: dmake runs $(shell ...) as a phony
# recipe named "Shell escape" and, same as any other recipe line, treats
# a non-zero exit as a fatal build error unless the line is marked
# ignore-errors (the same leading "-" convention as normal recipes,
# handled by Rcp_attribute() in dag.c). "test -f X && echo yes" exits 1
# whenever X does not exist -- the expected, common case here, since
# community builds normally only install libxml2 as a static archive
# (--enable-shared=no, no dylib at all) -- so without the "-" this aborts
# the whole build the moment no dylib is found, instead of just leaving
# the macro empty.
#
# --with-static-system-libs=libxml pins this to the archive, so a dylib
# appearing at the prefix can't silently turn a release build dynamic.
.IF "$(STATIC_SYSTEM_LIBXML)"=="YES"
FORMS_LIBXML2_HAS_DYLIB:=
.ELSE
FORMS_LIBXML2_HAS_DYLIB:=$(shell -test -f $(LIBXML_PREFIX)/lib/libxml2.dylib && echo yes)
.ENDIF
.IF "$(FORMS_LIBXML2_HAS_DYLIB)"=="yes"
FORMS_LIBXML2LIB:=$(LIBXML_PREFIX)/lib/libxml2.dylib
.ELSE
FORMS_LIBXML2LIB:=$(LIBXML_PREFIX)/lib/libxml2.a $(ZLIB3RDLIB)
.ENDIF
.ELSE
FORMS_LIBXML2LIB=$(LIBXML2LIB)
.ENDIF

SHL1STDLIBS= \
		$(EDITENGLIB) \
		$(SALLIB) \
		$(CPPULIB) \
		$(CPPUHELPERLIB) \
		$(TOOLSLIB) \
		$(I18NISOLANGLIB) \
		$(VCLLIB) \
		$(SVTOOLLIB) \
		$(SVLLIB)	\
		$(TKLIB) \
		$(SFX2LIB) \
		$(VOSLIB) \
		$(UNOTOOLSLIB) \
		$(COMPHELPERLIB) \
		$(DBTOOLSLIB) \
		$(TKLIB) \
		$(SVXCORELIB) \
        $(UCBHELPERLIB) \
        $(FORMS_LIBXML2LIB) \
        $(ICUUCLIB) \
        $(ICUINLIB)

SHL1LIBS=$(LIB1TARGET)
SHL1DEPN=$(LIB1TARGET)	\
		makefile.mk


SHL1VERSIONMAP=$(SOLARENV)/src/component.map
SHL1DEF=$(MISC)$/$(SHL1TARGET).def
DEF1NAME=$(SHL1TARGET)

# === .res file ==========================================================

RES1FILELIST=\
	$(SRS)$/resource.srs \

RESLIB1NAME=$(TARGET)
RESLIB1SRSFILES=$(RES1FILELIST)

# --- Targets ----------------------------------

.INCLUDE : target.mk

# --- Filter-Datei ---

$(MISC)$/$(SHL1TARGET).flt: makefile.mk
	@echo ------------------------------
	@echo __CT				    >$@
	@echo createRegistryInfo    >>$@
	@echo queryInterface        >>$@
	@echo queryAggregation      >>$@
	@echo NavigationToolBar     >>$@
	@echo ONavigationBar        >>$@


ALLTAR : $(MISC)/frm.component

$(MISC)/frm.component .ERRREMOVE : $(SOLARENV)/bin/createcomponent.xslt \
        frm.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL1TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt frm.component
