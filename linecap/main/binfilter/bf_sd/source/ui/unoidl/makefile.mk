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

PROJECTPCH=sd
PROJECTPCHSOURCE=$(BFPRJ)$/util$/sd_sd

PRJNAME=binfilter
TARGET=sd_unoidl

#GEN_HID=TRUE
#GEN_HID_OTHER=TRUE
NO_HIDS=TRUE

ENABLE_EXCEPTIONS=TRUE
# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_sd
# --- Files --------------------------------------------------------

.IF "$(COM)"=="GCC"
NOOPTFILES= $(SLO)$/sd_unowcntr.obj
.ENDIF

SLOFILES =      \
		$(SLO)$/sd_unopool.obj \
		$(SLO)$/sd_UnoDocumentSettings.obj \
		$(SLO)$/sd_facreg.obj \
		$(SLO)$/sd_unomodel.obj    \
		$(SLO)$/sd_unopage.obj     \
		$(SLO)$/sd_unolayer.obj    \
		$(SLO)$/sd_unopres.obj     \
        $(SLO)$/sd_unocpres.obj    \
        $(SLO)$/sd_unoobj.obj		\
		$(SLO)$/sd_unowcntr.obj	\
		$(SLO)$/sd_unogsfm.obj		\
		$(SLO)$/sd_unogstyl.obj	\
		$(SLO)$/sd_unopsfm.obj		\
		$(SLO)$/sd_unopstyl.obj	\
		$(SLO)$/sd_unokywds.obj	\
		$(SLO)$/sd_unostyls.obj	\
		$(SLO)$/sd_unopback.obj \
		$(SLO)$/sd_unodoc.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

