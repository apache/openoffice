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
TARGET=svx_unodraw

NO_HIDS=TRUE

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_svx

# --- Files --------------------------------------------------------

SLOFILES =	\
		$(SLO)$/svx_UnoNamespaceMap.obj \
		$(SLO)$/svx_unopool.obj \
		$(SLO)$/svx_XPropertyTable.obj \
		$(SLO)$/svx_UnoNameItemTable.obj \
		$(SLO)$/svx_unoshape.obj	\
		$(SLO)$/svx_unoipset.obj	\
		$(SLO)$/svx_unoctabl.obj	\
		$(SLO)$/svx_unoshap2.obj	\
		$(SLO)$/svx_unoshap3.obj	\
		$(SLO)$/svx_unoshap4.obj	\
		$(SLO)$/svx_unopage.obj		\
		$(SLO)$/svx_unoshtxt.obj	\
		$(SLO)$/svx_unoshcol.obj	\
		$(SLO)$/svx_unoprov.obj		\
		$(SLO)$/svx_unomod.obj      \
		$(SLO)$/svx_unonrule.obj	\
		$(SLO)$/svx_unofdesc.obj	\
		$(SLO)$/svx_unomlstr.obj	\
		$(SLO)$/svx_unogtabl.obj	\
		$(SLO)$/svx_unohtabl.obj	\
		$(SLO)$/svx_unobtabl.obj	\
		$(SLO)$/svx_unottabl.obj	\
		$(SLO)$/svx_unomtabl.obj	\
		$(SLO)$/svx_unodtabl.obj	\
		$(SLO)$/svx_gluepts.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

