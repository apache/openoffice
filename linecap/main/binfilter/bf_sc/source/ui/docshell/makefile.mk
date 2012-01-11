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
TARGET=sc_docshell

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  scpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sc.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_sc
# --- Files --------------------------------------------------------

SLOFILES =  \
        $(SLO)$/sc_docsh.obj	\
        $(SLO)$/sc_docsh2.obj	\
        $(SLO)$/sc_docsh3.obj	\
        $(SLO)$/sc_docsh4.obj   	\
        $(SLO)$/sc_docsh5.obj   	\
        $(SLO)$/sc_docsh6.obj   	\
        $(SLO)$/sc_tablink.obj   	\
        $(SLO)$/sc_arealink.obj   	\
        $(SLO)$/sc_dbdocfun.obj 	\
        $(SLO)$/sc_dbdocimp.obj 	\
        $(SLO)$/sc_impex.obj	\
        $(SLO)$/sc_docfunc.obj	\
        $(SLO)$/sc_olinefun.obj	\
        $(SLO)$/sc_pntlock.obj \
        $(SLO)$/sc_sizedev.obj \
        $(SLO)$/sc_editable.obj


EXCEPTIONSFILES= \
        $(SLO)$/sc_dbdocimp.obj


# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

