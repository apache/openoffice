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
TARGET=sc_ui

#GEN_HID=TRUE
#GEN_HID_OTHER=TRUE
NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  scpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sc.mk
INC+= -I$(PRJ)$/inc$/bf_sc
#IMGLST_SRS=$(SRS)$/sc_ui.srs
#BMP_IN=$(BFPRJ)$/res/imglst/apptbx
# --- Update-Version -----------------------------------------------

RSCUPDVER=$(RSCREVISION)(SV$(UPD)$(UPDMINOR))

# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES = \
	sc_sc.src		 \
	sc_pseudo.src	 \
	sc_scstring.src \
	sc_globstr.src  \
	sc_scerrors.src

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

$(SRS)$/sc_ui.srs: $(PRJ)$/inc$/bf_svx$/globlmn.hrc

