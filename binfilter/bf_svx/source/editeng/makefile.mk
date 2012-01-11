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
TARGET=svx_editeng

NO_HIDS=TRUE

PROJECTPCH4DLL=TRUE
PROJECTPCH=eeng_pch
PROJECTPCHSOURCE=svx_eeng_pch

ENABLE_EXCEPTIONS=TRUE


# --- Settings -----------------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_svx

# --- Allgemein ----------------------------------------------------------

.IF "$(editdebug)" != "" || "$(EDITDEBUG)" != ""
CDEFS+=-DEDITDEBUG
.ENDIF

SLOFILES =	\
			$(SLO)$/svx_txtrange.obj \
			$(SLO)$/svx_forbiddencharacterstable.obj \
			$(SLO)$/svx_swafopt.obj \
			$(SLO)$/svx_editattr.obj \
			$(SLO)$/svx_editdbg.obj	\
			$(SLO)$/svx_editdoc.obj	\
			$(SLO)$/svx_editdoc2.obj \
			$(SLO)$/svx_editeng.obj	\
			$(SLO)$/svx_editobj.obj	\
			$(SLO)$/svx_editsel.obj	\
			$(SLO)$/svx_editundo.obj \
			$(SLO)$/svx_editview.obj  \
			$(SLO)$/svx_eerdll.obj	\
			$(SLO)$/svx_impedit.obj	\
			$(SLO)$/svx_impedit2.obj \
			$(SLO)$/svx_impedit3.obj \
			$(SLO)$/svx_impedit4.obj \
			$(SLO)$/svx_impedit5.obj \
			$(SLO)$/svx_unolingu.obj

SRS1NAME=$(TARGET)
SRC1FILES=	svx_editeng.src

EXCEPTIONSFILES= \
	$(SLO)$/svx_unolingu.obj

.INCLUDE :  target.mk

