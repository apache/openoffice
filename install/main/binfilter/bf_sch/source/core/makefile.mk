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

PROJECTPCH=sch
PROJECTPCHSOURCE=$(BFPRJ)$/util$/sch_sch

PRJNAME=binfilter
TARGET=sch_core

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------
.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_sch
# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES = sch_glob.src

SLOFILES =  \
	$(SLO)$/sch_chtscene.obj	\
	$(SLO)$/sch_globfunc.obj	\
	$(SLO)$/sch_chtmodel.obj	\
	$(SLO)$/sch_chtmode1.obj	\
	$(SLO)$/sch_chtmode2.obj	\
	$(SLO)$/sch_chtmode3.obj	\
	$(SLO)$/sch_chtmode4.obj	\
	$(SLO)$/sch_chtmode5.obj	\
	$(SLO)$/sch_chtmode6.obj	\
	$(SLO)$/sch_chtmode7.obj	\
	$(SLO)$/sch_chtmode8.obj	\
	$(SLO)$/sch_charttyp.obj	\
	$(SLO)$/sch_chtmode9.obj	\
	$(SLO)$/sch_chartbar.obj	\
	$(SLO)$/sch_chtmod2a.obj	\
	$(SLO)$/sch_chtmod3d.obj	\
	$(SLO)$/sch_chtm3d2.obj		\
	$(SLO)$/sch_chartdoc.obj	\
	$(SLO)$/sch_stlsheet.obj	\
	$(SLO)$/sch_schgroup.obj	\
	$(SLO)$/sch_stlpool.obj		\
	$(SLO)$/sch_objfac.obj		\
	$(SLO)$/sch_axisid.obj		\
	$(SLO)$/sch_objid.obj		\
	$(SLO)$/sch_objadj.obj		\
	$(SLO)$/sch_datarow.obj		\
	$(SLO)$/sch_datapoin.obj	\
	$(SLO)$/sch_schiocmp.obj	\
	$(SLO)$/sch_itempool.obj	\
	$(SLO)$/sch_datalog.obj		\
	$(SLO)$/sch_memchrt.obj 	\
	$(SLO)$/sch_chdescr.obj 	\
	$(SLO)$/sch_chaxis.obj		\
	$(SLO)$/sch_calculat.obj	\
	$(SLO)$/sch_axisobj.obj		\
	$(SLO)$/sch_SchAddInCollection.obj	\
	$(SLO)$/sch_ReBuildHint.obj

.IF "$(GUI)" == "WIN"
NOOPTFILES=\
	$(OBJ)$/sch_chtmode5.obj
.ENDIF

EXCEPTIONSFILES= \
	$(SLO)$/sch_chtmode4.obj   \
	$(SLO)$/sch_chtmodel.obj   \
	$(SLO)$/sch_SchAddInCollection.obj

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

