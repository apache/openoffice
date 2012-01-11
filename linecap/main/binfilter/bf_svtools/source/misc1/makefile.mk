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

PRJNAME=binfilter
TARGET=misc1

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_svtools

# --- Files --------------------------------------------------------

EXCEPTIONSFILES=\
	$(SLO)$/svt_fstathelper.obj \
	$(SLO)$/svt_strmadpt.obj \
	$(SLO)$/svt_svtdata.obj

SLOFILES=\
	$(EXCEPTIONSFILES) \
	$(SLO)$/svt_adrparse.obj \
	$(SLO)$/svt_docpasswdrequest.obj \
	$(SLO)$/svt_inethist.obj \
	$(SLO)$/svt_inettype.obj \
	$(SLO)$/svt_loginerr.obj \
	$(SLO)$/svt_PasswordHelper.obj

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk




