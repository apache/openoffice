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
PACKAGE = text/simpress/02
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_simpress_02
# edit to match the current module
MODULE  = simpress

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   04010000.xhp \
   04020000.xhp \
   04030000.xhp \
   04040000.xhp \
   04060000.xhp \
   04070000.xhp \
   08020000.xhp \
   08060000.xhp \
   10020000.xhp \
   10030000.xhp \
   10030200.xhp \
   10050000.xhp \
   10060000.xhp \
   10070000.xhp \
   10080000.xhp \
   10090000.xhp \
   10100000.xhp \
   10110000.xhp \
   10120000.xhp \
   10130000.xhp \
   11060000.xhp \
   11070000.xhp \
   11080000.xhp \
   11090000.xhp \
   11100000.xhp \
   11110000.xhp \
   13010000.xhp \
   13020000.xhp \
   13030000.xhp \
   13040000.xhp \
   13050000.xhp \
   13060000.xhp \
   13070000.xhp \
   13080000.xhp \
   13090000.xhp \
   13100000.xhp \
   13140000.xhp \
   13150000.xhp \
   13160000.xhp \
   13170000.xhp \
   13180000.xhp \
   13190000.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
.ELSE
.INCLUDE :  target.mk
.ENDIF
