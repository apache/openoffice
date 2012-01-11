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
TARGET=svx_form

NO_HIDS=TRUE

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_svx

# --- Files --------------------------------------------------------

BMP_IN=$(BFPRJ)$/win$/res

SRS1NAME=svx_form
SRC1FILES= \
	svx_fmstring.src 	\
	svx_tbxform.src		\
	svx_fmsearch.src


SLOFILES = \
	$(SLO)$/svx_fmpage.obj      \
	$(SLO)$/svx_fmmodel.obj		\
	$(SLO)$/svx_fmdpage.obj		\
	$(SLO)$/svx_fmobj.obj		\
	$(SLO)$/svx_fmobjfac.obj \
	$(SLO)$/svx_fmimplids.obj \
	$(SLO)$/svx_fmdmod.obj \
	$(SLO)$/svx_ParseContext.obj \
	$(SLO)$/svx_siimport.obj \
	$(SLO)$/svx_fmtools.obj \
	$(SLO)$/svx_fmpgeimp.obj

.IF "$(OS)$(CPU)"=="SOLARISI" 
NOOPTFILES=$(SLO)$/svx_fmprop.obj
.ENDIF

.INCLUDE :  target.mk
