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
TARGET=sw_crsr

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :	$(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :	settings.mk
.INCLUDE :	$(PRJ)$/inc$/bf_sw$/sw.mk
INC+= -I$(PRJ)$/inc$/bf_sw
.IF "$(mydebug)" != ""
CDEFS+=-Dmydebug
.ENDIF

# --- Files --------------------------------------------------------

SLOFILES =	\
		$(SLO)$/sw_bookmrk.obj \
		$(SLO)$/sw_callnk.obj \
		$(SLO)$/sw_crbm.obj \
		$(SLO)$/sw_crsrsh.obj \
		$(SLO)$/sw_findattr.obj \
		$(SLO)$/sw_findcoll.obj \
		$(SLO)$/sw_findtxt.obj \
		$(SLO)$/sw_pam.obj \
		$(SLO)$/sw_paminit.obj \
		$(SLO)$/sw_swcrsr.obj \
		$(SLO)$/sw_trvlreg.obj \
		$(SLO)$/sw_trvltbl.obj \
		$(SLO)$/sw_unocrsr.obj \
		$(SLO)$/sw_viscrs.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :	target.mk
