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
TARGET=svx_dialogs

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_svx

.IF "$(GUI)" != "MAC"
CFLAGS+=-DDG_DLL
.ELSE
CFLAGS+=-D DG_DLL
.ENDIF

# --- Files --------------------------------------------------------

BMP_IN=$(BFPRJ)$/win/res

SRS1NAME=svx_dialogs
SRC1FILES =  \
		svx_impgrf.src \
		svx_lingu.src

SRS2NAME=svx_drawdlgs
SRC2FILES =  \
        svx_sdstring.src

SLOFILES=\
        $(SLO)$/svx_dlgutil.obj  \
        $(SLO)$/svx_impgrf.obj   

.IF "$(COM)" == "C50"
EXCEPTIONSNOOPTFILES=$(SLO)$/svx_impgrf.obj
.ELSE
EXCEPTIONSFILES+=$(SLO)$/svx_impgrf.obj
.ENDIF

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

