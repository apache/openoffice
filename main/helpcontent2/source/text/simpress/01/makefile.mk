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
PACKAGE = text/simpress/01
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_simpress_01
# edit to match the current module
MODULE  = simpress

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   01170000.xhp \
   01180000.xhp \
   01180001.xhp \
   01180002.xhp \
   02110000.xhp \
   02120000.xhp \
   02130000.xhp \
   02140000.xhp \
   02150000.xhp \
   02160000.xhp \
   03050000.xhp \
   03060000.xhp \
   03070000.xhp \
   03080000.xhp \
   03090000.xhp \
   03100000.xhp \
   03110000.xhp \
   03120000.xhp \
   03130000.xhp \
   03150000.xhp \
   03150100.xhp \
   03150300.xhp \
   03151000.xhp \
   03151100.xhp \
   03151200.xhp \
   03152000.xhp \
   03180000.xhp \
   04010000.xhp \
   04020000.xhp \
   04030000.xhp \
   04030000m.xhp \
   04030100.xhp \
   04040000m.xhp \
   04080100.xhp \
   04110000.xhp \
   04110100.xhp \
   04110200.xhp \
   04120000.xhp \
   04130000.xhp \
   04140000.xhp \
   04990000.xhp \
   04990100.xhp \
   04990200.xhp \
   04990300.xhp \
   04990400.xhp \
   04990500.xhp \
   04990600.xhp \
   04990700.xhp \
   05090000m.xhp \
   05100000.xhp \
   05110500m.xhp \
   05120000.xhp \
   05120100.xhp \
   05120500m.xhp \
   05130000.xhp \
   05140000.xhp \
   05150000.xhp \
   05170000.xhp \
   05250000.xhp \
   05250500.xhp \
   05250600.xhp \
   05250700.xhp \
   06030000.xhp \
   06040000.xhp \
   06050000.xhp \
   06060000.xhp \
   06070000.xhp \
   06080000.xhp \
   06100000.xhp \
   06100100.xhp \
   13050000.xhp \
   13050100.xhp \
   13050200.xhp \
   13050300.xhp \
   13050400.xhp \
   13050500.xhp \
   13050600.xhp \
   13050700.xhp \
   13140000.xhp \
   13150000.xhp \
   13160000.xhp \
   13170000.xhp \
   13180000.xhp \
   13180100.xhp \
   13180200.xhp \
   13180300.xhp \
   animationeffect.xhp \
   effectoptions.xhp \
   effectoptionseffect.xhp \
   effectoptionstext.xhp \
   effectoptionstiming.xhp \
   slidesorter.xhp \
   taskpanel.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
.ELSE
.INCLUDE :  target.mk
.ENDIF
