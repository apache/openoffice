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
PACKAGE = text/smath/01
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_smath_01
# edit to match the current module
MODULE  = smath

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   02080000.xhp \
   02090000.xhp \
   02100000.xhp \
   02110000.xhp \
   03040000.xhp \
   03050000.xhp \
   03060000.xhp \
   03070000.xhp \
   03080000.xhp \
   03090000.xhp \
   03090100.xhp \
   03090200.xhp \
   03090300.xhp \
   03090400.xhp \
   03090500.xhp \
   03090600.xhp \
   03090700.xhp \
   03090800.xhp \
   03090900.xhp \
   03090901.xhp \
   03090902.xhp \
   03090903.xhp \
   03090904.xhp \
   03090905.xhp \
   03090906.xhp \
   03090907.xhp \
   03090908.xhp \
   03090909.xhp \
   03090910.xhp \
   03091100.xhp \
   03091200.xhp \
   03091300.xhp \
   03091400.xhp \
   03091500.xhp \
   03091501.xhp \
   03091502.xhp \
   03091503.xhp \
   03091504.xhp \
   03091505.xhp \
   03091506.xhp \
   03091507.xhp \
   03091508.xhp \
   03091509.xhp \
   03091600.xhp \
   05010000.xhp \
   05010100.xhp \
   05020000.xhp \
   05030000.xhp \
   05040000.xhp \
   05050000.xhp \
   06010000.xhp \
   06010100.xhp \
   06020000.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
.ELSE
.INCLUDE :  target.mk
.ENDIF
