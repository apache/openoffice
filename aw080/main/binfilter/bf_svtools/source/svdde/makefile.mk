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
TARGET=svdde

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_svtools

# --- Files --------------------------------------------------------

.IF "$(GUIBASE)"=="WIN"

SLOFILES=	$(SLO)$/svt_ddecli.obj	\
			$(SLO)$/svt_ddesvr.obj	\
			$(SLO)$/svt_ddedata.obj	\
			$(SLO)$/svt_ddestrg.obj	\
			$(SLO)$/svt_ddewrap.obj \
			$(SLO)$/svt_ddeinf.obj
.ELSE

SLOFILES=	$(SLO)$/svt_ddedummy.obj	\

.ENDIF

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk


