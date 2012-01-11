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
TARGET=bf_frm

NO_HIDS=TRUE

USE_DEFFILE=TRUE

# --- Settings ----------------------------------

.INCLUDE :	settings.mk
INC+= -I$(PRJ)$/inc$/bf_forms
LDUMP=ldump2.exe

# --- Library -----------------------------------

LIB1TARGET=$(SLB)$/bf_forms.lib
LIB1FILES=\
		$(SLB)$/forms_common.lib \
		$(SLB)$/forms_resource.lib \
		$(SLB)$/forms_component.lib

SHL1TARGET=$(TARGET)$(DLLPOSTFIX)

SHL1STDLIBS= \
		$(SALLIB) \
		$(OSLLIB) \
		$(ONELIB) \
		$(CPPULIB) \
		$(CPPUHELPERLIB) \
		$(TOOLSLIB) \
		$(I18NISOLANGLIB) \
		$(VCLLIB) \
		$(BFSVTOOLLIB) \
		$(BFSVXLIB) \
		$(VOSLIB) \
		$(UNOTOOLSLIB) \
		$(COMPHELPERLIB) \
		$(DBTOOLSLIB) \
        $(TKLIB) \
        $(BFSO3LIB)

SHL1LIBS=$(LIB1TARGET)
SHL1DEPN=	$(LIB1TARGET)	\
		makefile.mk


SHL1VERSIONMAP=$(SOLARENV)/src/component.map
SHL1DEF=$(MISC)$/$(SHL1TARGET).def
DEF1NAME=$(SHL1TARGET)

# === .res file ==========================================================

RES1FILELIST=\
	$(SRS)$/forms_resource.srs \

RESLIB1NAME=$(TARGET)
RESLIB1SRSFILES=$(RES1FILELIST)

# --- Targets ----------------------------------

.INCLUDE : target.mk

# --- Filter-Datei ---

$(MISC)$/$(SHL1TARGET).flt: makefile.mk
	@echo ------------------------------
	@echo Making: $@
	@echo CLEAR_THE_FILE			> $@
	@echo OControl					>> $@
	@echo OBoundControl				>> $@
	@echo OCombo					>> $@
	@echo OList						>> $@
	@echo OFixedText				>> $@
	@echo OCheckBox					>> $@
	@echo OGroupBox					>> $@
	@echo RadioButton				>> $@
	@echo OHidden					>> $@
	@echo OEdit						>> $@
	@echo OEditBase					>> $@
	@echo ONumeric					>> $@
	@echo OPattern					>> $@
	@echo OCurrency					>> $@
	@echo ODate						>> $@
	@echo OTime						>> $@
	@echo OFile						>> $@
	@echo OFormatted				>> $@
	@echo OComponent				>> $@
	@echo OButton					>> $@
	@echo OImage					>> $@
	@echo OInterfaceContainer		>> $@
	@echo OFormsCollection			>> $@
	@echo OGroup					>> $@
	@echo HtmlSuccess				>> $@
	@echo OSuccess					>> $@
	@echo OParameter				>> $@
	@echo ODatabaseForm				>> $@
	@echo OFormComponents			>> $@
	@echo OFormSubmitResetThread	>> $@
	@echo OGrid						>> $@
	@echo FieldColumn				>> $@
	@echo BoxColumn					>> $@
	@echo StandardFormatsSupplier	>> $@
	@echo OGuard					>> $@
	@echo OPropertyChange			>> $@
	@echo OEnumeration				>> $@
	@echo Weak						>> $@
	@echo OUString					>> $@
	@echo Any@uno@star@sun@com		>> $@
	@echo _C						>> $@
	@echo _TI2						>> $@
	@echo _real						>> $@

ALLTAR : $(MISC)/bf_frm.component

$(MISC)/bf_frm.component .ERRREMOVE : $(SOLARENV)/bin/createcomponent.xslt \
        bf_frm.component
    $(XSLTPROC) --nonet --stringparam uri \
        '$(COMPONENTPREFIX_BASIS_NATIVE)$(SHL1TARGETN:f)' -o $@ \
        $(SOLARENV)/bin/createcomponent.xslt bf_frm.component
