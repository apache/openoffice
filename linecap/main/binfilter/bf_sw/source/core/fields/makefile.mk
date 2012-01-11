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
TARGET=sw_fields

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/sw.mk
INC+= -I$(PRJ)$/inc$/bf_sw
# --- Files --------------------------------------------------------

SLOFILES =	\
		$(SLO)$/sw_authfld.obj \
		$(SLO)$/sw_cellfml.obj \
		$(SLO)$/sw_chpfld.obj \
		$(SLO)$/sw_dbfld.obj \
		$(SLO)$/sw_ddefld.obj \
		$(SLO)$/sw_ddetbl.obj \
		$(SLO)$/sw_docufld.obj \
		$(SLO)$/sw_expfld.obj \
		$(SLO)$/sw_fldbas.obj \
		$(SLO)$/sw_flddat.obj \
		$(SLO)$/sw_flddropdown.obj \
		$(SLO)$/sw_scrptfld.obj \
		$(SLO)$/sw_macrofld.obj \
		$(SLO)$/sw_reffld.obj \
		$(SLO)$/sw_tblcalc.obj \
		$(SLO)$/sw_usrfld.obj


# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

