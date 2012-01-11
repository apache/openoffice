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
TARGET=sw_w4w

#GEN_HID=TRUE
#GEN_HID_OTHER=TRUE
NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

#.INCLUDE :  $(PRJ)$/inc$/swpre.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :  settings.mk
#.INCLUDE :  $(PRJ)$/inc$/sw.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/sw.mk

INC+= -I$(PRJ)$/inc$/bf_sw
.IF "$(mydebug)" != ""
CDEFS=$(CDEFS) -Dmydebug
.ENDIF

# --- Files --------------------------------------------------------

CXXFILES = \
		sw_w4watr.cxx \
		sw_w4wgraf.cxx \
		sw_w4wpar1.cxx \
		sw_w4wpar2.cxx \
		sw_w4wpar3.cxx \
		sw_w4wstk.cxx \
		sw_wrtgraf.cxx \
		sw_wrtw4w.cxx



SLOFILES =  \
		$(SLO)$/sw_w4watr.obj \
		$(SLO)$/sw_w4wgraf.obj \
		$(SLO)$/sw_w4wpar1.obj \
		$(SLO)$/sw_w4wpar2.obj \
		$(SLO)$/sw_w4wpar3.obj \
		$(SLO)$/sw_w4wstk.obj \
		$(SLO)$/sw_wrtgraf.obj \
		$(SLO)$/sw_wrtw4w.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

