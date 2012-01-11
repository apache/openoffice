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

PROJECTPCH=svxpch
PROJECTPCHSOURCE=$(BFPRJ)$/util$/svx_svxpch

PRJNAME=binfilter
TARGET=svx_svxlink

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_svx

# --- Files --------------------------------------------------------

.IF "$(header)" == ""

CXXFILES=\
	svx_linkmgr.cxx		\
	svx_fileobj.cxx

SLOFILES=\
        $(SLO)$/svx_linkmgr.obj	\
        $(SLO)$/svx_fileobj.obj

SRS1NAME=$(TARGET)
SRC1FILES=	\
        svx_linkmgr.src

.IF "$(GUI)" == "WNT"
NOOPTFILES=\
	$(SLO)$/svx_fileobj.obj
.ENDIF

.ENDIF

# ==========================================================================

.INCLUDE :  target.mk


