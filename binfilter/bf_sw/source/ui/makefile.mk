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
TARGET=sw_ui

#GEN_HID=TRUE
#GEN_HID_OTHER=TRUE
NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

#.INCLUDE :  $(PRJ)$/inc$/swpre.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :  settings.mk
#.INCLUDE :  $(PRJ)$/inc$/sw.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/sw.mk
INC+= -I$(PRJ)$/inc$/bf_sw

.IF "$(GUI)" != "OS2"
.IF "$(GUI)" != "WNT"
.IF "$(GUI)" != "MAC"
.IF "$(COM)" != "BLC"
.IF "$(COM)" != "WTC"
LIBFLAGS=/NOI /NOE /PAGE:256
.ENDIF
.ENDIF
.ENDIF
.ENDIF
.ENDIF

################################################################

# andere Label als Verzeichnisnamen fuer VC++/NT

LIB1TARGET=$(SLB)$/sw_ui.lib
LIB1FILES= \
		$(SLB)$/sw_app.lib      \
		$(SLB)$/sw_config.lib   \
		$(SLB)$/sw_dbui.lib     \
		$(SLB)$/sw_dochdl.lib   \
        $(SLB)$/sw_utlui.lib    \
		$(SLB)$/sw_fldui.lib    \
        $(SLB)$/sw_globdoc.lib  \
		$(SLB)$/sw_index.lib    \
		$(SLB)$/sw_unoidl.lib   \
        $(SLB)$/sw_web.lib      \
		$(SLB)$/sw_wrtsh.lib

.INCLUDE :  target.mk

################################################################


kill:
	@$(RM) $(SLB)$/ui.lst
	@$(RM) $(SLB)$/ui.lib

