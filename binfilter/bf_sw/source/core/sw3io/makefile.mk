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
TARGET=sw_sw3io

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/sw.mk
INC+= -I$(PRJ)$/inc$/bf_sw

.IF "$(GUI)$(COM)" == "WINMSC"
LIBFLAGS=/NOI /NOE /PAGE:512
.ENDIF

# --- Files --------------------------------------------------------

CXXFILES = \
	sw_sw3io.cxx		\
	sw_sw3attr.cxx 	\
	sw_sw3block.cxx	\
	sw_sw3doc.cxx		\
	sw_sw3field.cxx	\
	sw_sw3fmts.cxx 	\
	sw_sw3imp.cxx		\
	sw_sw3misc.cxx 	\
	sw_sw3nodes.cxx	\
	sw_sw3npool.cxx	\
	sw_sw3num.cxx		\
	sw_sw3page.cxx 	\
	sw_sw3redln.cxx 	\
	sw_sw3sectn.cxx	\
	sw_sw3style.cxx	\
	sw_sw3table.cxx	\
	sw_crypter.cxx

SLOFILES = \
	$(SLO)$/sw_sw3io.obj	\
	$(SLO)$/sw_sw3attr.obj	\
	$(SLO)$/sw_sw3block.obj \
	$(SLO)$/sw_sw3doc.obj	\
	$(SLO)$/sw_sw3field.obj \
	$(SLO)$/sw_sw3fmts.obj	\
	$(SLO)$/sw_sw3imp.obj	\
	$(SLO)$/sw_sw3misc.obj	\
	$(SLO)$/sw_sw3nodes.obj \
	$(SLO)$/sw_sw3npool.obj \
	$(SLO)$/sw_sw3num.obj	\
	$(SLO)$/sw_sw3page.obj	\
	$(SLO)$/sw_sw3redln.obj	\
	$(SLO)$/sw_sw3sectn.obj \
	$(SLO)$/sw_sw3style.obj \
	$(SLO)$/sw_sw3table.obj \
	$(SLO)$/sw_crypter.obj

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

