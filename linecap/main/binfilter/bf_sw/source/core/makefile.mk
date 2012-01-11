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

PRJNAME=binfilter
TARGET=sw_core

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/sw.mk

INC+= -I$(PRJ)$/inc$/bf_sw
.IF "$(CALLTARGETS)"=="core"
RC_SUBDIRS=
.ENDIF

# --- Files --------------------------------------------------------

SUBLIBS1= \
		$(SLB)$/sw_graphic.lib \
		$(SLB)$/sw_para.lib \
		$(SLB)$/sw_attr.lib \
		$(SLB)$/sw_edit.lib \
		$(SLB)$/sw_crsr.lib \
		$(SLB)$/sw_view.lib \
		$(SLB)$/sw_frmedt.lib \
		$(SLB)$/sw_ole.lib \
		$(SLB)$/sw_fields.lib \
		$(SLB)$/sw_tox.lib \
		$(SLB)$/sw_bastyp.lib

SUBLIBS2= \
		$(SLB)$/sw_draw.lib \
		$(SLB)$/sw_sw3io.lib \
		$(SLB)$/sw_swg.lib \
		$(SLB)$/sw_layout.lib \
		$(SLB)$/sw_text.lib \
		$(SLB)$/sw_txtnode.lib \
		$(SLB)$/sw_doc.lib \
		$(SLB)$/sw_docnode.lib \
		$(SLB)$/sw_unocore.lib

.IF "$(dbgutil)" != ""
SUBLIBS2+= \
		$(SLB)$/sw_except.lib
.ENDIF

#-------------------------------------------------------------------------

################################################################

LIB1TARGET=$(SLB)$/sw_core1.lib
LIB1FILES= \
		$(SUBLIBS1)

LIB2TARGET=$(SLB)$/sw_core2.lib
LIB2FILES= \
		$(SUBLIBS2)

.INCLUDE :  target.mk

################################################################

#-------------------------------------------------------------------------


