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
TARGET=sc_data

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  scpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sc.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_sc
# --- Files --------------------------------------------------------

SLOFILES =  \
	$(SLO)$/sc_bclist.obj \
	$(SLO)$/sc_bcaslot.obj \
	$(SLO)$/sc_docpool.obj \
	$(SLO)$/sc_poolhelp.obj \
	$(SLO)$/sc_column.obj \
    $(SLO)$/sc_column2.obj \
    $(SLO)$/sc_column3.obj \
	$(SLO)$/sc_document.obj \
	$(SLO)$/sc_documen2.obj \
	$(SLO)$/sc_documen3.obj \
	$(SLO)$/sc_documen4.obj \
    $(SLO)$/sc_documen5.obj \
    $(SLO)$/sc_documen6.obj \
    $(SLO)$/sc_documen7.obj \
    $(SLO)$/sc_documen8.obj \
    $(SLO)$/sc_documen9.obj \
    $(SLO)$/sc_fillinfo.obj \
	$(SLO)$/sc_table1.obj \
	$(SLO)$/sc_table2.obj \
	$(SLO)$/sc_table3.obj \
	$(SLO)$/sc_table4.obj \
	$(SLO)$/sc_table5.obj \
	$(SLO)$/sc_olinetab.obj \
	$(SLO)$/sc_patattr.obj \
	$(SLO)$/sc_cell.obj \
	$(SLO)$/sc_cell2.obj \
	$(SLO)$/sc_attarray.obj \
	$(SLO)$/sc_attrib.obj \
	$(SLO)$/sc_global.obj \
    $(SLO)$/sc_global2.obj \
	$(SLO)$/sc_globalx.obj \
	$(SLO)$/sc_markarr.obj \
	$(SLO)$/sc_markdata.obj \
	$(SLO)$/sc_dociter.obj \
	$(SLO)$/sc_drwlayer.obj \
	$(SLO)$/sc_userdat.obj \
    $(SLO)$/sc_drawpage.obj \
    $(SLO)$/sc_stlsheet.obj \
	$(SLO)$/sc_stlpool.obj \
    $(SLO)$/sc_dptabsrc.obj \
    $(SLO)$/sc_dptabdat.obj \
    $(SLO)$/sc_dpshttab.obj \
    $(SLO)$/sc_dpoutput.obj \
    $(SLO)$/sc_dpobject.obj \
    $(SLO)$/sc_dpsave.obj \
    $(SLO)$/sc_pivot.obj \
    $(SLO)$/sc_pivot2.obj \
    $(SLO)$/sc_dbdocutl.obj \
    $(SLO)$/sc_pagepar.obj \
    $(SLO)$/sc_conditio.obj \
    $(SLO)$/sc_validat.obj \
	$(SLO)$/sc_sortparam.obj

EXCEPTIONSFILES= \
	$(SLO)$/sc_documen6.obj \
	$(SLO)$/sc_dpobject.obj \
	$(SLO)$/sc_dpoutput.obj \
	$(SLO)$/sc_dpsave.obj	\
	$(SLO)$/sc_dbdocutl.obj \
    $(SLO)$/sc_dptabsrc.obj \
	$(SLO)$/sc_globalx.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

