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
TARGET=svx_unoedit

NO_HIDS=TRUE

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_svx

# --- Files --------------------------------------------------------

SLOFILES =	\
		$(SLO)$/svx_UnoForbiddenCharsTable.obj \
		$(SLO)$/svx_unoedsrc.obj	\
		$(SLO)$/svx_unoedhlp.obj	\
		$(SLO)$/svx_unoedprx.obj	\
		$(SLO)$/svx_unoviwed.obj	\
		$(SLO)$/svx_unoviwou.obj	\
		$(SLO)$/svx_unofored.obj	\
		$(SLO)$/svx_unoforou.obj	\
		$(SLO)$/svx_unotext.obj		\
		$(SLO)$/svx_unotext2.obj	\
		$(SLO)$/svx_unofield.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

