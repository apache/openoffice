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
TARGET=sdraw3

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------------

.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_sd

.IF "$(GUI)"=="WIN"
LINKFLAGS+=/PACKCODE:65500 /SEG:16000 /NOE /NOD /MAP
MAPSYM=tmapsym
.ENDIF

.IF "$(COM)"=="ICC"
LINKFLAGS+=/SEGMENTS:512 /PACKD:32768
.ENDIF

# --- Resourcen ----------------------------------------------------

RESLIB1NAME=bf_sd
RESLIB1SRSFILES=\
         $(SRS)$/sd_app.srs $(SRS)$/sd_core.srs  

# --- StarDraw DLL

SHL1TARGET= bf_sd$(DLLPOSTFIX)
SHL1VERSIONMAP= bf_sd.map
SHL1IMPLIB= bf_sdi

# static libraries
SHL1STDLIBS= $(BFSCHLIB) $(BFSCLIB) $(BFSMLIB)

# dynamic libraries
SHL1STDLIBS+= \
	$(BFSVXLIB) \
	$(LEGACYSMGRLIB)	\
	$(BFSO3LIB) \
	$(BFSVTOOLLIB) \
	$(TKLIB) \
	$(VCLLIB) \
	$(SOTLIB) \
	$(UNOTOOLSLIB) \
	$(TOOLSLIB) \
	$(I18NISOLANGLIB) \
	$(COMPHELPERLIB) \
	$(CPPUHELPERLIB) \
	$(CPPULIB) \
	$(SALLIB)


SHL1LIBS=   $(LIB3TARGET)


SHL1DEF=    $(MISC)$/$(SHL1TARGET).def
.IF "$(GUI)" == "WNT" || "$(GUI)" == "WIN"
SHL1RES=    $(RCTARGET)
.ENDIF

# --- Linken der Applikation ---------------------------------------

LIB2TARGET=$(SLB)$/sd_sdmod.lib
LIB2OBJFILES=   \
			$(SLO)$/sd_sdmod.obj

LIB3TARGET=$(SLB)$/sd_sdraw3.lib
LIB3FILES=      \
			$(SLB)$/sd_view.lib			\
			$(SLB)$/sd_app.lib			\
			$(SLB)$/sd_docshell.lib    \
			$(SLB)$/sd_core.lib		\
			$(SLB)$/sd_xml.lib			\
			$(SLB)$/sd_bin.lib			\
			$(SLB)$/sd_filter.lib		\
			$(SLB)$/sd_unoidl.lib

LIB4TARGET=$(LB)$/bf_sdlib.lib
LIB4ARCHIV=$(LB)$/libbf_sdlib.a
LIB4OBJFILES=$(SLO)$/sd_sdlib.obj \
		  $(SLO)$/sd_sdresid.obj

# --- Targets -------------------------------------------------------------

.INCLUDE :  target.mk

.IF "$(depend)" == ""

.IF "$(GUI)" == "WNT"

$(MISC)$/$(SHL1TARGET).def:
    @echo ------------------------------
    @echo Making: $@
    @echo LIBRARY     $(SHL1TARGET)                                  >$@
.IF "$(COM)"!="GCC"
    @echo DESCRIPTION 'SDRAW3 DLL'                                 >>$@
    @echo DATA        READ WRITE NONSHARED                          >>$@
.ENDIF
    @echo EXPORTS                                                   >>$@
    @echo   CreateSdDrawDocShellDll @20                            >>$@
    @echo   CreateSdGraphicDocShellDll @21                         >>$@
    @echo   CreateObjSdDrawDocShellDll @22                         >>$@
    @echo   CreateObjSdGraphicDocShellDll @23                      >>$@
    @echo   InitSdDll @24                                          >>$@
    @echo   DeInitSdDll @25                                        >>$@
	@echo component_getImplementationEnvironment 				   >>$@
	@echo component_getFactory									   >>$@
.ENDIF

.IF "$(GUI)" == "OS2"

$(MISC)$/$(SHL1TARGET).def:
    @echo ------------------------------
    @echo Making: $@
    @echo LIBRARY     $(SHL1TARGET8)  INITINSTANCE TERMINSTANCE    >$@
    @echo DATA        MULTIPLE                                     >>$@
    @echo EXPORTS                                                   >>$@
    @echo   _CreateSdDrawDocShellDll                                >>$@
    @echo   _CreateSdGraphicDocShellDll                             >>$@
    @echo   _CreateObjSdDrawDocShellDll                             >>$@
    @echo   _CreateObjSdGraphicDocShellDll                          >>$@
    @echo   _InitSdDll                                              >>$@
    @echo   _DeInitSdDll                                            >>$@
    @echo _component_getImplementationEnvironment 		    >>$@
    @echo _component_getFactory					    >>$@
.ENDIF

.ENDIF

$(MISCX)$/$(SHL1TARGET).flt:
    @echo ------------------------------
    @echo Making: $@
    @echo WEP>$@
    @echo LIBMAIN>>$@
    @echo LibMain>>$@

ALLTAR : $(MISC)/bf_sd.component

$(MISC)/bf_sd.component .ERRREMOVE : $(SOLARENV)/bin/createcomponent.xslt \
        bf_sd.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL1TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt bf_sd.component
