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



# edit to match directory level 
PRJ		= ..$/..$/..$/..
# same for all makefiles in "helpcontent2"
PRJNAME = helpcontent2
.IF "$(MAKETARGETS)"!="genPO"

# edit to match the current package
PACKAGE = text/shared/autopi
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_shared_autopi
# edit to match the current module
MODULE  = shared

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   01000000.xhp \
   01010000.xhp \
   01010100.xhp \
   01010200.xhp \
   01010300.xhp \
   01010400.xhp \
   01010500.xhp \
   01010600.xhp \
   01020000.xhp \
   01020100.xhp \
   01020200.xhp \
   01020300.xhp \
   01020400.xhp \
   01020500.xhp \
   01040000.xhp \
   01040100.xhp \
   01040200.xhp \
   01040300.xhp \
   01040400.xhp \
   01040500.xhp \
   01040600.xhp \
   01050000.xhp \
   01050100.xhp \
   01050200.xhp \
   01050300.xhp \
   01050400.xhp \
   01050500.xhp \
   01090000.xhp \
   01090100.xhp \
   01090200.xhp \
   01090210.xhp \
   01090220.xhp \
   01090300.xhp \
   01090400.xhp \
   01090500.xhp \
   01090600.xhp \
   01100000.xhp \
   01100100.xhp \
   01100150.xhp \
   01100200.xhp \
   01100300.xhp \
   01100400.xhp \
   01100500.xhp \
   01110000.xhp \
   01110100.xhp \
   01110200.xhp \
   01110300.xhp \
   01110400.xhp \
   01110500.xhp \
   01110600.xhp \
   01120000.xhp \
   01120100.xhp \
   01120200.xhp \
   01120300.xhp \
   01120400.xhp \
   01120500.xhp \
   01130000.xhp \
   01130100.xhp \
   01130200.xhp \
   01150000.xhp \
   01170000.xhp \
   01170200.xhp \
   01170300.xhp \
   01170400.xhp \
   01170500.xhp \
   purchase.xhp \
   startup.xhp \
   webwizard00.xhp \
   webwizard01.xhp \
   webwizard02.xhp \
   webwizard03.xhp \
   webwizard04.xhp \
   webwizard05.xhp \
   webwizard05bi.xhp \
   webwizard05is.xhp \
   webwizard06.xhp \
   webwizard07.xhp \
   webwizard07fc.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
.ELSE
.INCLUDE :  target.mk
.ENDIF
