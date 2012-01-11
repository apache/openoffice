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

PROJECTPCH=sch
PROJECTPCHSOURCE=$(BFPRJ)$/util$/sch_sch

PRJNAME=binfilter
TARGET=sch_app

NO_HIDS=TRUE

BMP_OUT=$(RES)
BMP_IN =$(BFPRJ)$/res

# --- Settings -----------------------------------------------------
.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_sch
# --- Files --------------------------------------------------------

#IMGLST_SRS=$(SRS)$/app.srs

SRS1NAME=$(TARGET)
SRC1FILES =	sch_app.src		\
		sch_strings.src

DEPOBJFILES =	\
		$(SLO)$/sch_schlib.obj

SLOFILES =	\
		$(SLO)$/sch_schdll.obj	\
		$(SLO)$/sch_schmod.obj      \
		$(SLO)$/sch_schresid.obj	\
		$(SLO)$/sch_schopt.obj	\
		$(SLO)$/sch_globopt.obj

LIB2TARGET =	$(SLB)$/bf_ysch.lib
LIB2ARCHIV =	$(LB)$/libbf_ysch.a
LIB2OBJFILES  =	$(SLO)$/sch_schlib.obj


.IF "$(GUI)" == "WNT"
NOOPTFILES=\
	$(SLO)$/sch_schdll.obj
.ENDIF



# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

$(INCCOM)$/schlib.hxx: makefile.mk
.IF "$(GUI)"=="UNX"
	$(RM) $@
	echo \#define DLL_NAME \"libbf_sch$(DLLPOSTFIX)$(DLLPOST)\" >$@
.ELSE
	echo $(EMQ)#define DLL_NAME $(EMQ)"bf_sch$(DLLPOSTFIX)$(DLLPOST)$(EMQ)" >$@
.ENDIF

$(SRS)$/sch_app.srs: $(PRJ)$/inc$/bf_svx$/globlmn.hrc

$(SLO)$/sch_schlib.obj : $(INCCOM)$/schlib.hxx


