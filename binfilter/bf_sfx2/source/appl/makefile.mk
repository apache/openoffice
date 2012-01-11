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
TARGET=sfx2_appl

NO_HIDS=TRUE

ENABLE_EXCEPTIONS=TRUE
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

INC+= -I$(PRJ)$/inc$/bf_sfx2

# w.g. compilerbugs
.IF "$(GUI)"=="WNT"
.IF "$(COM)"!="GCC"
CFLAGS+=-Od
.ENDIF
.ENDIF

# --- Files --------------------------------------------------------

SLOFILES =  \
	$(SLO)$/sfx2_appuno.obj \
	$(SLO)$/sfx2_appmain.obj \
	$(SLO)$/sfx2_appopen.obj \
	$(SLO)$/sfx2_appinit.obj \
	$(SLO)$/sfx2_appmisc.obj \
	$(SLO)$/sfx2_appcfg.obj \
	$(SLO)$/sfx2_appquit.obj \
	$(SLO)$/sfx2_appdata.obj \
	$(SLO)$/sfx2_app.obj \
	$(SLO)$/sfx2_appbas.obj \
	$(SLO)$/sfx2_appdde.obj \
	$(SLO)$/sfx2_sfxdll.obj \
	$(SLO)$/sfx2_module.obj \
	$(SLO)$/sfx2_dlgcont.obj \
	$(SLO)$/sfx2_namecont.obj \
	$(SLO)$/sfx2_scriptcont.obj \
    $(SLO)$/sfx2_imestatuswindow.obj 

EXCEPTIONSFILES=\
	$(SLO)$/sfx2_appopen.obj \
	$(SLO)$/sfx2_appmain.obj			\
	$(SLO)$/sfx2_appmisc.obj			\
	$(SLO)$/sfx2_appinit.obj			\
	$(SLO)$/sfx2_appcfg.obj			

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

