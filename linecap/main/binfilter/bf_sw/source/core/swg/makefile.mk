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
TARGET=sw_swg

NO_HIDS=TRUE

AUTOSEG=true
#ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/sw.mk
INC+= -I$(PRJ)$/inc$/bf_sw
.IF "$(GUI)$(COM)" == "WINMSC"
LIBFLAGS=/NOI /NOE /PAGE:512
.ENDIF

# --- Files --------------------------------------------------------

SLOFILES =  \
		$(SLO)$/sw_rdcont.obj \
		$(SLO)$/sw_rdflds.obj \
		$(SLO)$/sw_rdfmts.obj \
        $(SLO)$/sw_rdhnt.obj \
		$(SLO)$/sw_rdmisc.obj \
		$(SLO)$/sw_rdnds.obj \
		$(SLO)$/sw_rdnum.obj \
		$(SLO)$/sw_rdpage.obj \
		$(SLO)$/sw_rdswg.obj \
		$(SLO)$/sw_rdtox.obj \
		$(SLO)$/sw_swgpar.obj \
		$(SLO)$/sw_swgstr.obj \
		$(SLO)$/sw_SwXMLSectionList.obj

EXCEPTIONSFILES = \
		$(SLO)$/sw_SwXMLSectionList.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk
