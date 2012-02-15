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



PRJ=..$/..$/..

PRJNAME=binfilter
TARGET=basic_runtime

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------------

.INCLUDE :  settings.mk

INC+= -I$(PRJ)$/inc$/bf_basic

# --- Allgemein -----------------------------------------------------------

SLOFILES=	\
	$(SLO)$/basrdll.obj	\

#	$(SLO)$/inputbox.obj	\
	$(SLO)$/runtime.obj	\
	$(SLO)$/step0.obj	\
	$(SLO)$/step1.obj	\
	$(SLO)$/step2.obj	\
	$(SLO)$/iosys.obj	\
	$(SLO)$/stdobj.obj	\
	$(SLO)$/stdobj1.obj	\
	$(SLO)$/methods.obj	\
	$(SLO)$/methods1.obj	\
	$(SLO)$/props.obj	\
	$(SLO)$/ddectrl.obj	\
	$(SLO)$/dllmgr.obj

.IF "$(GUI)$(CPU)" == "WINI"
#SLOFILES+=	$(SLO)$/win.obj
.ENDIF

.IF "$(GUI)$(CPU)" == "WNTI"
#SLOFILES+=	$(SLO)$/wnt.obj
.ENDIF

.IF "$(GUI)$(CPU)" == "OS2I"
#SLOFILES+= $(SLO)$/os2.obj
.ENDIF

#EXCEPTIONSFILES=$(SLO)$/step0.obj	\
		$(SLO)$/step2.obj	\
		$(SLO)$/methods.obj	\
		$(SLO)$/methods1.obj	\
		$(SLO)$/iosys.obj	\
		$(SLO)$/runtime.obj

# --- Targets -------------------------------------------------------------

.INCLUDE :  target.mk

