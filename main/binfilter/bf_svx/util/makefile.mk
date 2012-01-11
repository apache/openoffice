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



EXTERNAL_WARNINGS_NOT_ERRORS := TRUE

PRJ=..$/..
BFPRJ=..

PRJNAME=binfilter
TARGET=bf_svx

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

INC+= -I$(PRJ)$/inc$/bf_svx

# --- Svx - DLL ----------

SHL1TARGET= bf_svx$(DLLPOSTFIX)
SHL1IMPLIB= bf_svx
SHL1BASE  = 0x1d800000

SHL1STDLIBS= \
			$(BFSVTOOLLIB) \
			$(BFXMLOFFLIB) \
			$(BFGOODIESLIB) \
			$(BFBASICLIB) \
			$(BFSO3LIB) \
			$(TKLIB) \
			$(VCLLIB) \
			$(SOTLIB) \
			$(UNOTOOLSLIB) \
			$(TOOLSLIB) \
			$(I18NISOLANGLIB) \
			$(COMPHELPERLIB) \
			$(UCBHELPERLIB) \
			$(CPPUHELPERLIB) \
			$(CPPULIB) \
			$(VOSLIB) \
			$(SALLIB) \
			$(ICUUCLIB)\
			$(LEGACYSMGRLIB) \
                        $(SVTOOLLIB) \
			$(XMLSCRIPTLIB)

.IF "$(GUI)"=="WNT"

SHL1STDLIBS+=\
        $(OLEAUT32LIB) \
		$(UWINAPILIB) \
		$(ADVAPI32LIB) \
		$(SHELL32LIB) \
		$(GDI32LIB) \
		$(OLE32LIB) \
		$(UUIDLIB)

SHL1STDLIBS+=\
            $(SHELLLIB)

SHL1DEPN=       $(SLB)$/svx_svx.lib
.ENDIF # WNT

SHL1LIBS=       $(SLB)$/svx_svx.lib

SHL1DEF=        $(MISC)$/$(SHL1TARGET).def
DEF1NAME        =$(SHL1TARGET)
DEF1DEPN        =$(MISC)$/$(SHL1TARGET).flt
DEFLIB1NAME=svx_svx
DEF1DES		= Rtf, Edt, Outliner, SvDraw, Form, Fmcomp, Engine3D, MSFilter
# THB: exports list svx checked for 6.0 Final 6.12.2001
DEF1EXPORTFILE	= svx.dxp

LIB1TARGET      =$(SLB)$/svx_svx.lib
LIB1FILES       = \
        $(SLB)$/svx_dialogs.lib \
        $(SLB)$/svx_editeng.lib \
        $(SLB)$/svx_engine3d.lib \
        $(SLB)$/svx_form.lib \
        $(SLB)$/svx_items.lib \
        $(SLB)$/svx_options.lib \
        $(SLB)$/svx_outliner.lib \
        $(SLB)$/svx_svdraw.lib \
        $(SLB)$/svx_svxlink.lib \
        $(SLB)$/svx_unodraw.lib \
        $(SLB)$/svx_unoedit.lib \
        $(SLB)$/svx_xml.lib \
        $(SLB)$/svx_xout.lib \
		$(SLB)$/bf_sfx.lib

.IF "(GUIBASE)" == "WIN"
LIB1FILES  += \
            $(SLB)$/ibrwimp.lib
.ENDIF


SRS1FILELIST=\
                $(SRS)$/svx_svdstr.srs      \
                $(SRS)$/svx_editeng.srs     \
                $(SRS)$/svx_outliner.srs \
                $(SRS)$/svx_dialogs.srs     \
                $(SRS)$/svx_drawdlgs.srs \
                $(SRS)$/svx_svxitems.srs \
                $(SRS)$/svx_engine3d.srs \
                $(SRS)$/svx_svxlink.srs

RESLIB1NAME=bf_svx
RESLIB1SRSFILES= $(SRS1FILELIST)

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

$(MISC)$/$(SHL1TARGET).flt: makefile.mk
    @echo ------------------------------
    @echo Making: $@
	$(TYPE) bf_svx.flt >$@
	$(TYPE) bf_sfxwin.flt > $@

ALLTAR : $(MISC)/bf_svx.component

$(MISC)/bf_svx.component .ERRREMOVE : $(SOLARENV)/bin/createcomponent.xslt \
        bf_svx.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL1TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt bf_svx.component
