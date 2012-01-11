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
TARGET=sc_unoobj

NO_HIDS=TRUE

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  scpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sc.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_sc
# --- Files --------------------------------------------------------

SLOFILES =  \
        $(SLO)$/sc_docuno.obj \
        $(SLO)$/sc_servuno.obj \
        $(SLO)$/sc_defltuno.obj \
        $(SLO)$/sc_drdefuno.obj \
        $(SLO)$/sc_cellsuno.obj \
        $(SLO)$/sc_textuno.obj \
        $(SLO)$/sc_notesuno.obj \
        $(SLO)$/sc_cursuno.obj \
        $(SLO)$/sc_fielduno.obj \
        $(SLO)$/sc_miscuno.obj \
        $(SLO)$/sc_optuno.obj \
        $(SLO)$/sc_appluno.obj \
        $(SLO)$/sc_funcuno.obj \
        $(SLO)$/sc_nameuno.obj \
        $(SLO)$/sc_datauno.obj \
        $(SLO)$/sc_dapiuno.obj \
        $(SLO)$/sc_chartuno.obj \
        $(SLO)$/sc_shapeuno.obj \
        $(SLO)$/sc_pageuno.obj \
        $(SLO)$/sc_forbiuno.obj \
        $(SLO)$/sc_styleuno.obj \
        $(SLO)$/sc_afmtuno.obj \
        $(SLO)$/sc_fmtuno.obj \
        $(SLO)$/sc_linkuno.obj \
        $(SLO)$/sc_targuno.obj \
        $(SLO)$/sc_convuno.obj \
        $(SLO)$/sc_editsrc.obj \
        $(SLO)$/sc_unoguard.obj \
        $(SLO)$/sc_confuno.obj \
        $(SLO)$/sc_filtuno.obj \
        $(SLO)$/sc_unodoc.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

