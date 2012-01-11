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
TARGET=sw_text

NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/sw.mk
INC+= -I$(PRJ)$/inc$/bf_sw
.IF "$(GUI)"!="OS2"
INCEXT=s:\solar\inc\hm
.ENDIF

.IF "$(mydebug)" != ""
CDEFS+=-Dmydebug
.ENDIF

.IF "$(GUI)$(COM)" == "WINMSC"
LIBFLAGS=/NOI /NOE /PAGE:512
.ENDIF

# --- Files --------------------------------------------------------

SLOFILES =  \
        $(SLO)$/sw_atrstck.obj \
		$(SLO)$/sw_frmcrsr.obj \
		$(SLO)$/sw_frmform.obj \
		$(SLO)$/sw_frmpaint.obj \
		$(SLO)$/sw_guess.obj \
		$(SLO)$/sw_inftxt.obj \
		$(SLO)$/sw_itradj.obj \
		$(SLO)$/sw_itratr.obj \
		$(SLO)$/sw_itrcrsr.obj \
		$(SLO)$/sw_itrform2.obj \
		$(SLO)$/sw_itrpaint.obj \
		$(SLO)$/sw_itrtxt.obj \
		$(SLO)$/sw_porexp.obj \
		$(SLO)$/sw_porfld.obj \
		$(SLO)$/sw_porfly.obj \
		$(SLO)$/sw_porglue.obj \
		$(SLO)$/sw_porlay.obj \
		$(SLO)$/sw_porlin.obj \
		$(SLO)$/sw_pormulti.obj \
		$(SLO)$/sw_porrst.obj \
		$(SLO)$/sw_portox.obj \
		$(SLO)$/sw_portxt.obj \
		$(SLO)$/sw_redlnitr.obj \
		$(SLO)$/sw_txtcache.obj \
		$(SLO)$/sw_txtdrop.obj \
		$(SLO)$/sw_txtfld.obj \
		$(SLO)$/sw_txtfly.obj \
		$(SLO)$/sw_txtfrm.obj \
		$(SLO)$/sw_txtftn.obj \
		$(SLO)$/sw_txthyph.obj \
		$(SLO)$/sw_txtinit.obj \
		$(SLO)$/sw_txttab.obj \
		$(SLO)$/sw_widorp.obj \
		$(SLO)$/sw_blink.obj \
		$(SLO)$/sw_noteurl.obj \
		$(SLO)$/sw_wrong.obj

.IF "$(dbgutil)" != ""
SLOFILES +=  \
		$(SLO)$/sw_txtio.obj
.ENDIF

.IF "$(CPUNAME)" == "SPARC"
.IF "$(OS)" == "NETBSD"
NOOPTFILES = \
	$(SLO)$/sw_txtftn.obj
.ENDIF
.ENDIF

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

