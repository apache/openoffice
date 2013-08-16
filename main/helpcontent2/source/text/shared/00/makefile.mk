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
PACKAGE = text/shared/00
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_shared_00
# edit to match the current module
MODULE  = shared

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   00000001.xhp \
   00000002.xhp \
   00000003.xhp \
   00000004.xhp \
   00000005.xhp \
   00000007.xhp \
   00000010.xhp \
   00000011.xhp \
   00000020.xhp \
   00000021.xhp \
   00000099.xhp \
   00000200.xhp \
   00000206.xhp \
   00000207.xhp \
   00000208.xhp \
   00000210.xhp \
   00000215.xhp \
   00000401.xhp \
   00000402.xhp \
   00000403.xhp \
   00000404.xhp \
   00000406.xhp \
   00000407.xhp \
   00000408.xhp \
   00000409.xhp \
   00000450.xhp \
   00040500.xhp \
   00040501.xhp \
   00040502.xhp \
   00040503.xhp \
   01000000.xhp \
   01010000.xhp \
   01020000.xhp \
   01050000.xhp \
   icon_alt.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
.ELSE
.INCLUDE :  target.mk
.ENDIF
