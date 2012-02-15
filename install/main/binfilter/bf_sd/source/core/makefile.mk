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

PROJECTPCH=sd
PROJECTPCHSOURCE=$(BFPRJ)$/util$/sd_sd

PRJNAME=binfilter
TARGET=sd_core

#GEN_HID=TRUE
#GEN_HID_OTHER=TRUE
NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_sd
IENV+=-I..\ui\inc

# --- Files --------------------------------------------------------


SRS1NAME=$(TARGET)
SRC1FILES = sd_glob.src

SLOFILES = $(SLO)$/sd_stlsheet.obj  \
		   $(SLO)$/sd_stlpool.obj	\
		   $(SLO)$/sd_drawdoc.obj \
		   $(SLO)$/sd_drawdoc2.obj \
		   $(SLO)$/sd_drawdoc3.obj \
		   $(SLO)$/sd_drawdoc4.obj \
		   $(SLO)$/sd_sdpage.obj \
		   $(SLO)$/sd_sdattr.obj \
		   $(SLO)$/sd_sdobjfac.obj \
		   $(SLO)$/sd_anminfo.obj	\
		   $(SLO)$/sd_sdiocmpt.obj	\
		   $(SLO)$/sd_sdpage2.obj	\
           $(SLO)$/sd_cusshow.obj

EXCEPTIONSFILES= \
		   $(SLO)$/sd_drawdoc.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

