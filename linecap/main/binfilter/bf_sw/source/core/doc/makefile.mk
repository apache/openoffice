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
TARGET=sw_doc

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :	$(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :	settings.mk
.INCLUDE :	$(PRJ)$/inc$/bf_sw$/sw.mk
INC+= -I$(PRJ)$/inc$/bf_sw
.IF "$(GUI)$(COM)" == "WINMSC"
LIBFLAGS=/NOI /NOE /PAGE:512
.ENDIF

# --- Files --------------------------------------------------------

SLOFILES =	\
		$(SLO)$/sw_SwStyleNameMapper.obj \
		$(SLO)$/sw_doc.obj \
		$(SLO)$/sw_docbm.obj \
		$(SLO)$/sw_docchart.obj \
		$(SLO)$/sw_doccorr.obj \
		$(SLO)$/sw_docdde.obj \
		$(SLO)$/sw_docdesc.obj \
		$(SLO)$/sw_docdraw.obj \
		$(SLO)$/sw_docedt.obj \
		$(SLO)$/sw_docfld.obj \
		$(SLO)$/sw_docfly.obj \
		$(SLO)$/sw_docfmt.obj \
		$(SLO)$/sw_docftn.obj \
		$(SLO)$/sw_doclay.obj \
		$(SLO)$/sw_docnew.obj \
		$(SLO)$/sw_docnum.obj \
		$(SLO)$/sw_docredln.obj \
		$(SLO)$/sw_docruby.obj \
		$(SLO)$/sw_docsort.obj \
		$(SLO)$/sw_docstat.obj \
		$(SLO)$/sw_doctxm.obj \
		$(SLO)$/sw_extinput.obj \
		$(SLO)$/sw_fmtcol.obj \
		$(SLO)$/sw_ftnidx.obj \
		$(SLO)$/sw_gctable.obj \
		$(SLO)$/sw_lineinfo.obj \
		$(SLO)$/sw_notxtfrm.obj \
		$(SLO)$/sw_number.obj \
		$(SLO)$/sw_poolfmt.obj \
		$(SLO)$/sw_sortopt.obj \
		$(SLO)$/sw_swserv.obj \
		$(SLO)$/sw_swtable.obj \
		$(SLO)$/sw_tblafmt.obj \
		$(SLO)$/sw_tblrwcl.obj \
		$(SLO)$/sw_visiturl.obj \
		$(SLO)$/sw_htmltbl.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :	target.mk

