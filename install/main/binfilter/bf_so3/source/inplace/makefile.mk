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
SUBPRJ=..$/..

PRJINC=$(SUBPRJ)

PRJNAME=binfilter
TARGET=so3_inplace

# --- Settings -----------------------------------------------------
.INCLUDE :  settings.mk
.INCLUDE :  $(SUBPRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------

SLOFILES= \
	$(SLO)$/applet2.obj		\
	$(SLO)$/outplace.obj	\
	$(SLO)$/embobj.obj                           \
	$(SLO)$/client.obj                           \
	$(SLO)$/ipobj.obj                            \
	$(SLO)$/ipclient.obj                         \
	$(SLO)$/protocol.obj                         \
	$(SLO)$/soconv.obj		\
	$(SLO)$/ipenv.obj		\
	$(SLO)$/plugin.obj		


EXCEPTIONSFILES= \
	$(SLO)$/applet2.obj \
    $(SLO)$/plugin.obj


# --- Tagets -------------------------------------------------------


.INCLUDE :  target.mk

.INCLUDE :  $(SUBPRJ)$/util$/target.pmk
