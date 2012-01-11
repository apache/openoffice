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
TARGET=sw_uiview

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/sw.mk

INC+= -I$(PRJ)$/inc$/bf_sw
# --- Files --------------------------------------------------------

SRS1NAME=$(TARGET)
SRC1FILES =  \
        sw_view.src \
        sw_pview.src

SLOFILES =  \
		$(SLO)$/sw_view0.obj \
		$(SLO)$/sw_view.obj \
		$(SLO)$/sw_view1.obj \
		$(SLO)$/sw_view2.obj \
		$(SLO)$/sw_viewdraw.obj \
		$(SLO)$/sw_viewmdi.obj \
        $(SLO)$/sw_pview.obj \
        $(SLO)$/sw_viewport.obj \
        $(SLO)$/sw_viewstat.obj \
        $(SLO)$/sw_viewtab.obj \
        $(SLO)$/sw_viewprt.obj \
        $(SLO)$/sw_scroll.obj \
        $(SLO)$/sw_swcli.obj  \
        $(SLO)$/sw_srcview.obj \
        $(SLO)$/sw_uivwimp.obj

EXCEPTIONSFILES= \
        $(SLO)$/sw_srcview.obj \
        $(SLO)$/sw_uivwimp.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

$(SRS)$/sw_uiview.srs: $(PRJ)$/inc$/bf_svx$/globlmn.hrc

