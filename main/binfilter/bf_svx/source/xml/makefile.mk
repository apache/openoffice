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
TARGET=svx_xml

NO_HIDS=TRUE

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
INC+= -I$(PRJ)$/inc$/bf_svx


# --- Files --------------------------------------------------------
SLOFILES = \
		$(SLO)$/svx_xmltxtimp.obj		\
		$(SLO)$/svx_xmltxtexp.obj		\
		$(SLO)$/svx_xmlxtimp.obj		\
		$(SLO)$/svx_xmlxtexp.obj		\
		$(SLO)$/svx_xmlgrhlp.obj		\
		$(SLO)$/svx_xmleohlp.obj		

# --- Targets --------------------------------------------------------------

.INCLUDE : target.mk
