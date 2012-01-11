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



PRJ=..$/..$/..$/..

PRJNAME=binfilter
TARGET=filter
LIBTARGET=NO

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_svtools

# --- Files --------------------------------------------------------

SLOFILES= $(SLO)$/svt_filter.obj				\
		  $(SLO)$/svt_filter2.obj				\
		  $(SLO)$/svt_sgfbram.obj				\
		  $(SLO)$/svt_sgvmain.obj				\
		  $(SLO)$/svt_sgvtext.obj				\
		  $(SLO)$/svt_sgvspln.obj				\
		  $(SLO)$/svt_FilterConfigItem.obj		\
		  $(SLO)$/svt_FilterConfigCache.obj

EXCEPTIONSNOOPTFILES=	$(SLO)$/svt_filter.obj				\
						$(SLO)$/svt_FilterConfigItem.obj	\
						$(SLO)$/svt_FilterConfigCache.obj

LIB1TARGET=		$(SLB)$/$(TARGET).lib
LIB1OBJFILES=	\
	$(SLO)$/svt_filter.obj				\
	$(SLO)$/svt_filter2.obj				\
		  $(SLO)$/svt_sgfbram.obj				\
		  $(SLO)$/svt_sgvmain.obj				\
		  $(SLO)$/svt_sgvtext.obj				\
		  $(SLO)$/svt_sgvspln.obj				\
		  $(SLO)$/svt_FilterConfigItem.obj		\
		  $(SLO)$/svt_FilterConfigCache.obj

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

