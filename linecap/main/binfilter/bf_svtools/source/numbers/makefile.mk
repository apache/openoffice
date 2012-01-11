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



PRJ=..$/..$/..

PRJNAME=binfilter
TARGET=numbers
LIBTARGET=NO

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_svtools

# --- Files --------------------------------------------------------

EXCEPTIONSFILES= \
		$(SLO)$/svt_numuno.obj \
		$(SLO)$/svt_numfmuno.obj \
		$(SLO)$/svt_supservs.obj

SLOFILES =  \
		$(EXCEPTIONSFILES) \
		$(SLO)$/svt_zforfind.obj	\
		$(SLO)$/svt_zforlist.obj \
		$(SLO)$/svt_zformat.obj 	\
		$(SLO)$/svt_zforscan.obj \
		$(SLO)$/svt_numhead.obj

LIB1TARGET=	$(SLB)$/$(TARGET).uno.lib
LIB1OBJFILES=	\
		$(SLO)$/svt_numfmuno.obj \
		$(SLO)$/svt_supservs.obj

LIB2TARGET=	$(SLB)$/$(TARGET).lib
LIB2OBJFILES=	\
		$(SLO)$/svt_zforfind.obj	\
		$(SLO)$/svt_zforlist.obj \
		$(SLO)$/svt_zformat.obj 	\
		$(SLO)$/svt_zforscan.obj \
		$(SLO)$/svt_numuno.obj \
		$(SLO)$/svt_numhead.obj

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

