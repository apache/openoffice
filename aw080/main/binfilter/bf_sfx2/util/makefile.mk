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

BFPRJ=..
PRJ=..$/..

ENABLE_EXCEPTIONS=TRUE

PRJNAME=binfilter
TARGET=bf_sfx

NO_HIDS=TRUE

.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

INC+= -I$(PRJ)$/inc$/bf_sfx2
# --- Allgemein ----------------------------------------------------

LIB1TARGET= $(SLB)$/$(TARGET).lib
LIB1ARCHIV=$(LB)$/lib$(TARGET).a
LIB1FILES=  $(SLB)$/sfx2_appl.lib		\
            $(SLB)$/sfx2_explorer.lib	\
            $(SLB)$/sfx2_doc.lib			\
            $(SLB)$/sfx2_view.lib		\
            $(SLB)$/sfx2_control.lib		\
            $(SLB)$/sfx2_notify.lib		\
            $(SLB)$/sfx2_bastyp.lib		\
            $(SLB)$/sfx2_config.lib

# --- Targets ------------------------------------------------------


.INCLUDE :  target.mk


## --- SFX-Filter-Datei ---
#
#$(MISC)$/$(SHL1TARGET).flt: makefile.mk
#    @echo ------------------------------
#    @echo Making: $@
#    @$(TYPE) sfxwin.flt > $@


