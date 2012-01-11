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
TARGET=sw_txtnode

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/sw.mk
INC+= -I$(PRJ)$/inc$/bf_sw
.IF "$(GUI)"!="OS2"
INCEXT=s:$/solar$/inc$/hm
.ENDIF

.IF "$(mydebug)" != ""
CDEFS+=-Dmydebug
.ENDIF

.IF "$(GUI)$(COM)" == "WINMSC"
LIBFLAGS=/NOI /NOE /PAGE:512
.ENDIF

# --- Files --------------------------------------------------------

SLOFILES =  \
		$(SLO)$/sw_atrfld.obj \
		$(SLO)$/sw_atrflyin.obj \
		$(SLO)$/sw_atrftn.obj \
		$(SLO)$/sw_atrref.obj \
		$(SLO)$/sw_atrtox.obj \
		$(SLO)$/sw_fmtatr1.obj \
		$(SLO)$/sw_fmtatr2.obj \
		$(SLO)$/sw_fntcap.obj \
		$(SLO)$/sw_fntcache.obj \
		$(SLO)$/sw_swfntcch.obj \
		$(SLO)$/sw_ndhints.obj \
		$(SLO)$/sw_ndtxt.obj \
		$(SLO)$/sw_swfont.obj \
		$(SLO)$/sw_thints.obj \
		$(SLO)$/sw_txatbase.obj \
		$(SLO)$/sw_txtatr2.obj \
		$(SLO)$/sw_txtedt.obj

.IF "$(dbutil)" != ""
OFILES+=$(SLO)$/sw_dbchratr.$(QBJX)
.ENDIF

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

