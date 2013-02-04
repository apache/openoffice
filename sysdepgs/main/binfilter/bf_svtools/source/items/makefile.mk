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
TARGET=items
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_svtools

# --- Files --------------------------------------------------------

SLOFILES=\
	$(SLO)$/svt_aeitem.obj	\
	$(SLO)$/svt_eitem.obj	\
	$(SLO)$/svt_flagitem.obj	\
	$(SLO)$/svt_globalnameitem.obj	\
	$(SLO)$/svt_intitem.obj	\
	$(SLO)$/svt_macitem.obj	\
	$(SLO)$/svt_poolcach.obj	\
	$(SLO)$/svt_ptitem.obj	\
	$(SLO)$/svt_rectitem.obj	\
	$(SLO)$/svt_rngitem.obj	\
	$(SLO)$/svt_stritem.obj	\
	$(SLO)$/svt_style.obj	\
	$(SLO)$/svt_szitem.obj

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk

