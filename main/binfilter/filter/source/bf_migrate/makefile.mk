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

PRJ=..$/..$/..
BFPRJ=..$/..

PRJNAME=binfilter
TARGET=bf_migratefilter

#GEN_HID=TRUE
#GEN_HID_OTHER=TRUE
NO_HIDS=TRUE

#UNOUCRRDB=$(SOLARBINDIR)$/applicat.rdb
ENABLE_EXCEPTIONS=TRUE
USE_DEFFILE=TRUE

# --- Settings ----------------------------------

.INCLUDE :  	settings.mk

# --- Files -------------------------------------

CXXFILES=	bf_migrateexports.cxx \
			bf_migratefilter.cxx

SLOFILES=	$(SLO)$/bf_migrateexports.obj \
			$(SLO)$/bf_migratefilter.obj

# --- Library -----------------------------------

.IF "$(OS)" == "OS2"
SHL1TARGET=bf_migra$(DLLPOSTFIX)
.ELSE
SHL1TARGET=$(TARGET)$(DLLPOSTFIX)
.ENDIF
SHL1STDLIBS=$(CPPULIB)			\
			$(CPPUHELPERLIB)	\
			$(SALLIB)			\
            $(BFSO3LIB)                       \
			$(ONELIB)			\
			$(TOOLSLIB)			\
			$(LEGACYSMGRLIB)	\
			$(BFSVXLIB)

SHL1DEPN=
SHL1IMPLIB=	i$(SHL1TARGET)
SHL1LIBS=	$(SLB)$/$(TARGET).lib
SHL1DEF=	$(MISC)$/$(SHL1TARGET).def

DEF1NAME=$(SHL1TARGET)
DEF1EXPORTFILE=exports.dxp

# --- Targets ----------------------------------

.INCLUDE : target.mk

ALLTAR : $(MISC)/bf_migratefilter.component

$(MISC)/bf_migratefilter.component .ERRREMOVE : \
        $(SOLARENV)/bin/createcomponent.xslt bf_migratefilter.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL1TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt bf_migratefilter.component
