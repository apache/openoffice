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
# edit to match the current package
PACKAGE = text/scalc/02
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_scalc_02
# edit to match the current module
MODULE  = scalc

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   02130000.xhp \
   02140000.xhp \
   02150000.xhp \
   02160000.xhp \
   02170000.xhp \
   06010000.xhp \
   06030000.xhp \
   06040000.xhp \
   06050000.xhp \
   06060000.xhp \
   06070000.xhp \
   06080000.xhp \
   08010000.xhp \
   08080000.xhp \
   10050000.xhp \
   10060000.xhp \
   18010000.xhp \
   18020000.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
