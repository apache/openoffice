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

PRJNAME=binfilter
TARGET=bf_xo

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# --- Allgemein ----------------------------------------------------
NO_BSYMBOLIC=TRUE
ENABLE_EXCEPTIONS=TRUE

LIB1TARGET= $(SLB)$/bf_xo.lib
LIB1FILES=	\
    $(SLB)$/xmloff_core.lib \
    $(SLB)$/xmloff_meta.lib \
    $(SLB)$/xmloff_script.lib \
    $(SLB)$/xmloff_style.lib \
    $(SLB)$/xmloff_text.lib \
    $(SLB)$/xmloff_draw.lib \
    $(SLB)$/xmloff_chart.lib \
	$(SLB)$/xmloff_forms.lib

# --- Shared-Library -----------------------------------------------

.IF "$(GUI)"!="UNX" && "$(GUI)"!="OS2"
LIB4TARGET= $(LB)$/ibf_xo.lib
LIB4FILES=	$(LB)$/_ibf_xo.lib
.IF "$(GUI)"!="OS2"
LIB4OBJFILES=\
	$(OBJ)$/xmloff_xmlkywd.obj
.ENDIF
.ENDIF

SHL1TARGET= bf_xo$(DLLPOSTFIX)
.IF "$(GUI)" == "OS2"
SHL1IMPLIB= ibf_xo
.ELSE
SHL1IMPLIB= _ibf_xo
.ENDIF

SHL1STDLIBS= \
		$(BFSVTOOLLIB)	\
		$(TOOLSLIB)		\
		$(I18NISOLANGLIB) \
		$(VOSLIB)		\
		$(CPPULIB)		\
		$(CPPUHELPERLIB)\
		$(COMPHELPERLIB)\
		$(VCLLIB)		\
		$(TKLIB)		\
		$(SALLIB)		\
		$(UNOTOOLSLIB)	\
        $(BFGOODIESLIB) \
        $(BFSO3LIB)

# SCO: the linker does know about weak symbols, but we can't ignore multiple defined symbols
.IF "(OS)"=="SCO"
SHL1STDLIBS+=-licg617mxp
.ENDIF

SHL1DEF=    $(MISC)$/$(SHL1TARGET).def
SHL1LIBS=   $(LIB1TARGET)

# --- Def-File ---------------------------------------------------------

DEF1NAME    =$(SHL1TARGET)
.IF "$(GUI)" != "OS2"
DEF1DEPN    =$(MISC)$/$(SHL1TARGET).flt
.ENDIF
DEFLIB1NAME =bf_xo
DEF1DES     =XML Office Lib
DEF1EXPORTFILE=	exports.dxp
# --- Targets ----------------------------------------------------------

.INCLUDE :  target.mk

# --- Filter -----------------------------------------------------------

$(MISC)$/$(SHL1TARGET).flt: makefile.mk
	$(TYPE) bf_xo.flt > $@

ALLTAR : $(MISC)/bf_xo.component

$(MISC)/bf_xo.component .ERRREMOVE : $(SOLARENV)/bin/createcomponent.xslt \
        bf_xo.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL1TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt bf_xo.component
