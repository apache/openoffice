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
TARGET=sw_layout

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/sw.mk
INC+= -I$(PRJ)$/inc$/bf_sw

.IF "$(mydebug)" != ""
CDEFS+=-Dmydebug
.ENDIF

.IF "$(madebug)" != ""
CDEFS+=-DDEBUG
.ENDIF

#		"Querdarstellung des Dokumentes"
#		CDEFS=$(CDEFS) -DQUER

# 		CDEFS=$(CDEFS) -DPAGE

.IF "$(GUI)$(COM)" == "WINMSC"
LIBFLAGS=/NOI /NOE /PAGE:512
.ENDIF


# --- Files --------------------------------------------------------

CXXFILES = \
		sw_atrfrm.cxx \
		sw_calcmove.cxx \
		sw_colfrm.cxx \
		sw_findfrm.cxx \
		sw_flowfrm.cxx \
		sw_fly.cxx \
		sw_flycnt.cxx \
		sw_flyincnt.cxx \
		sw_flylay.cxx \
		sw_flypos.cxx \
		sw_frmtool.cxx \
		sw_ftnfrm.cxx \
		sw_hffrm.cxx \
		sw_layact.cxx \
                sw_laycache.cxx \
		sw_layouter.cxx \
		sw_newfrm.cxx \
		sw_pagechg.cxx \
		sw_pagedesc.cxx \
		sw_pageiter.cxx \
		sw_paintfrm.cxx \
		sw_sectfrm.cxx \
		sw_ssfrm.cxx \
		sw_tabfrm.cxx \
		sw_trvlfrm.cxx \
		sw_unusedf.cxx \
		sw_wsfrm.cxx



SLOFILES =  \
		$(SLO)$/sw_atrfrm.obj \
		$(SLO)$/sw_calcmove.obj \
		$(SLO)$/sw_colfrm.obj \
		$(SLO)$/sw_findfrm.obj \
		$(SLO)$/sw_flowfrm.obj \
		$(SLO)$/sw_fly.obj \
		$(SLO)$/sw_flycnt.obj \
		$(SLO)$/sw_flyincnt.obj \
		$(SLO)$/sw_flylay.obj \
		$(SLO)$/sw_flypos.obj \
		$(SLO)$/sw_frmtool.obj \
		$(SLO)$/sw_ftnfrm.obj \
		$(SLO)$/sw_hffrm.obj \
		$(SLO)$/sw_layact.obj \
                $(SLO)$/sw_laycache.obj \
		$(SLO)$/sw_layouter.obj \
		$(SLO)$/sw_newfrm.obj \
		$(SLO)$/sw_pagechg.obj \
		$(SLO)$/sw_pagedesc.obj \
		$(SLO)$/sw_pageiter.obj \
		$(SLO)$/sw_paintfrm.obj \
		$(SLO)$/sw_sectfrm.obj \
		$(SLO)$/sw_ssfrm.obj \
		$(SLO)$/sw_tabfrm.obj \
		$(SLO)$/sw_trvlfrm.obj \
		$(SLO)$/sw_unusedf.obj \
		$(SLO)$/sw_wsfrm.obj

.IF "$(dbgutil)"!=""
CXXFILES += \
		sw_dbg_lay.cxx
SLOFILES +=  \
		$(SLO)$/sw_dbg_lay.obj
.ENDIF

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

