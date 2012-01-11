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

PRJ=..$/..$/..$/..
BFPRJ=..$/..$/..

PRJNAME=binfilter
TARGET=sw_app

NO_HIDS=TRUE


# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/sw.mk

INC+= -I$(PRJ)$/inc$/bf_sw
RSCUPDVER=$(RSCREVISION)(SV$(UPD)$(UPDMINOR))

#IMGLST_SRS=$(SRS)$/app.srs
#BMP_IN=$(PRJ)$/win/imglst

# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES =\
        sw_app.src     \
        sw_error.src

SLOFILES = \
        $(SLO)$/sw_apphdl.obj   \
		$(SLO)$/sw_docsh.obj    \
		$(SLO)$/sw_docsh2.obj   \
		$(SLO)$/sw_docshdrw.obj \
		$(SLO)$/sw_docshini.obj \
		$(SLO)$/sw_docstyle.obj \
		$(SLO)$/sw_mainwn.obj   \
		$(SLO)$/sw_version.obj  \
		$(SLO)$/sw_swmodule.obj \
		$(SLO)$/sw_swmodul1.obj \
		$(SLO)$/sw_swdll.obj


EXCEPTIONSFILES= \
		$(SLO)$/sw_swmodule.obj \
		$(SLO)$/sw_swmodul1.obj \
		$(SLO)$/sw_docsh2.obj


DEPOBJFILES= \
    $(SLO)$/sw_swlib.obj \
    $(SLO)$/sw_swcomlib.obj


# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

ALLTAR : $(DEPOBJFILES)

$(SLO)$/sw_swdll.obj : $(INCCOM)$/swdll0.hxx
$(SLO)$/sw_swlib.obj : $(INCCOM)$/swdll0.hxx
$(SLO)$/sw_swmodule.obj : $(INCCOM)$/swdll0.hxx

$(INCCOM)$/swdll0.hxx: makefile.mk
	echo \#define DLL_NAME \"$(DLLPRE)bf_sw$(DLLPOSTFIX)$(DLLPOST)\" >$@

$(SRS)$/sw_app.srs: $(PRJ)$/inc$/bf_svx$/globlmn.hrc

