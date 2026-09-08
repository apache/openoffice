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



PRJ=..$/..

PRJNAME=sdext
TARGET=pdfimport
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

MKDEPENDSOLVER:=
.INCLUDE: settings.mk

.IF "$(L10N_framework)"==""

.INCLUDE: pdfisettings.pmk
INCPRE=-I$(PRJ)$/source$/pdfimport$/inc

# --- Files --------------------------------------------------------

EXTENSIONNAME:=PDFImport
EXTENSION_ZIPNAME:=pdfimport

.IF "$(ENABLE_PDFIMPORT)" == "NO"
@all:
	@echo "PDF Import extension disabled."
.ENDIF

SLOFILES=\
    $(SLO)$/filterdet.obj   \
    $(SLO)$/pdfiadaptor.obj \
    $(SLO)$/services.obj

SHL1DLLPRE=
SHL1TARGET=$(TARGET).uno

# NB: xpdfwrapper.lib is wrapper/, which is ours -- wrapper/makefile.mk
# still calls its TARGET that.  The poppler helper was xpdfwrapper/, a
# different directory, and is gone.
SHL1LIBS=\
	$(SLB)$/pdfmisc.lib \
	$(SLB)$/pdfparse.lib \
	$(SLB)$/pdfsax.lib \
	$(SLB)$/pdfparsetree.lib \
	$(SLB)$/pdfodf.lib \
	$(SLB)$/xpdfwrapper.lib \
	$(SLB)$/$(TARGET).lib

# jvmfwk: pdfbackend.cxx asks jfw_getSelectedJRE which JRE the office is
# configured to use, so the PDFBox helper starts under the one the user
# actually chose in Tools - Options - Java rather than whatever is on PATH.
SHL1STDLIBS=\
	$(JVMFWKLIB)	\
    $(CPPUHELPERLIB)	\
	$(ZLIB3RDLIB)	    \
	$(CPPULIB)			\
	$(SALLIB)

.IF "$(GUI)" == "UNX"
    SHL1STDLIBS+=-lbasegfx_s
.ELSE
.IF "$(GUI)" == "WNT"
.IF "$(COM)"=="GCC"
    SHL1STDLIBS+=-lbasegfx_s
.ELSE
    SHL1LIBS+=$(SOLARLIBDIR)$/basegfx_s.lib
.ENDIF

.ENDIF # WNT
.ENDIF # UNX

SHL1DEPN=
SHL1IMPLIB=	i$(SHL1TARGET)
SHL1DEF=	$(MISC)$/$(SHL1TARGET).def
SHL1VERSIONMAP=$(SOLARENV)/src/reg-component.map
SHL1RPATH=OXT

DEF1NAME=$(SHL1TARGET)

# --- Extension packaging ------------------------------------------

DESCRIPTION_SRC:=config$/description.xml
MANIFEST_SRC:=config$/manifest.xml
COMPONENT_CONFIGDIR:=config
COMPONENT_CONFIGDEST:=.
COMPONENT_XCU= \
    $(EXTENSIONDIR)$/pdf_import_filter.xcu \
    $(EXTENSIONDIR)$/pdf_types.xcu

# The backend, and the jars it needs, packed into the extension.
# pdfbackend.cxx probes the extension directory for pdfimport-pdfbox.jar, so
# this is the lookup path rather than a convention.
CONVERTER_FILE= \
    $(EXTENSIONDIR)$/pdfimport-pdfbox.jar \
    $(EXTENSIONDIR)$/pdfbox-3.0.8.jar \
    $(EXTENSIONDIR)$/pdfbox-io-3.0.8.jar \
    $(EXTENSIONDIR)$/fontbox-3.0.8.jar \
    $(EXTENSIONDIR)$/commons-logging-1.4.0.jar

COMPONENT_DIALOGS= \
    $(EXTENSIONDIR)$/basic$/Module1.xba \
    $(EXTENSIONDIR)$/basic$/TargetChooser.xdl \
    $(EXTENSIONDIR)$/basic$/dialog.xlb \
    $(EXTENSIONDIR)$/basic$/impress.png \
    $(EXTENSIONDIR)$/basic$/script.xlb \
    $(EXTENSIONDIR)$/basic$/writer.png \
    $(EXTENSIONDIR)$/xpdfimport_err.pdf

COMPONENT_HELP= \
	$(EXTENSIONDIR)$/help/component.txt

# native libraries
COMPONENT_LIBRARIES= \
    $(EXTENSIONDIR)$/$(SHL1TARGET)$(DLLPOST)

COMPONENT_IMAGES=\
	$(EXTENSIONDIR)$/images$/extension_32.png \
	$(EXTENSIONDIR)$/images$/extension_32_h.png

EXTENSION_PACKDEPS=$(CONVERTER_FILE) $(COMPONENT_DIALOGS) $(COMPONENT_HELP) $(COMPONENT_IMAGES) makefile.mk

.INCLUDE : extension_pre.mk
.ENDIF # L10N_framework
.INCLUDE : target.mk
.IF "$(L10N_framework)"==""
.INCLUDE : extension_post.mk

$(CONVERTER_FILE) : $(BIN)$/$$(@:f)
	@@-$(MKDIRHIER) $(@:d)
	$(COPY) $< $@

$(COMPONENT_DIALOGS) : dialogs$/$$(@:f)
	@@-$(MKDIRHIER) $(@:d)
	$(COPY) $< $@

$(COMPONENT_HELP) : help$/$$(@:f)
	@@-$(MKDIRHIER) $(@:d)
	$(COPY) $< $@

$(COMPONENT_IMAGES) :  $(SOLARSRC)$/$(RSCDEFIMG)$/desktop$/res$/$$(@:f)
	@@-$(MKDIRHIER) $(@:d)
	$(COPY) $< $@

.ENDIF # L10N_framework
