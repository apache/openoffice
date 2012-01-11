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
TARGET=sc_app

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  scpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sc.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_sc
# --- Files --------------------------------------------------------

SLOFILES =  \
	$(SLO)$/sc_scmod.obj  \
	$(SLO)$/sc_scmod2.obj  \
	$(SLO)$/sc_scdll.obj  \
	$(SLO)$/sc_rfindlst.obj \
	$(SLO)$/sc_uiitems.obj  \
	$(SLO)$/sc_msgpool.obj

EXCEPTIONSFILES= \
	$(SLO)$/sc_scmod2.obj \
	$(SLO)$/sc_scmod.obj

DEPOBJFILES=$(SLO)$/sc_sclib.obj

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

ALLTAR : $(DEPOBJFILES)

$(SLO)$/sc_sclib.obj :	$(INCCOM)$/scdll0.hxx
#$(OBJ)$/sc_sclib.obj :	$(INCCOM)$/scdll0.hxx

$(INCCOM)$/scdll0.hxx: makefile.mk
.IF "$(GUI)"=="UNX"
	echo \#define DLL_NAME \"libbf_sc$(DLLPOSTFIX)$(DLLPOST)\" >$@
.ELSE			# "$(GUI)"=="UNX"
	echo \#define DLL_NAME \"bf_sc$(DLLPOSTFIX).DLL\" >$@
.ENDIF			# "$(GUI)"=="UNX"

$(SRS)$/sc_app.srs: $(PRJ)$/inc$/bf_svx$/globlmn.hrc

