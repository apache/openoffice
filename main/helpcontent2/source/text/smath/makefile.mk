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
PRJ		= ..$/..$/..
PRJNAME = help_smath
PACKAGE = text/smath
TARGET  = text_smath
MODULE  = smath

LANGUAGE_FILELIST= main0103.xhp \
04/01020000.xhp \
02/03010000.xhp \
main0102.xhp \
main0100.xhp \
main0000.xhp \
main0107.xhp \
main0200.xhp \
main0106.xhp \
00/00000004.xhp \
01/03091500.xhp \
01/03091502.xhp \
01/03091508.xhp \
01/05050000.xhp \
01/03091506.xhp \
01/03091400.xhp \
01/03070000.xhp \
01/03090903.xhp \
01/03090800.xhp \
01/03080000.xhp \
01/05010000.xhp \
01/03090200.xhp \
01/03090904.xhp \
01/03090910.xhp \
01/03090908.xhp \
01/03090907.xhp \
01/06010000.xhp \
01/03050000.xhp \
01/03091600.xhp \
01/03090100.xhp \
01/03091200.xhp \
01/03091100.xhp \
01/03090600.xhp \
01/05010100.xhp \
01/06010100.xhp \
01/02090000.xhp \
01/03090902.xhp \
01/03090905.xhp \
01/03091507.xhp \
01/02080000.xhp \
01/03091300.xhp \
01/03091503.xhp \
01/05020000.xhp \
01/03090000.xhp \
01/02110000.xhp \
01/02100000.xhp \
01/03091505.xhp \
01/06020000.xhp \
01/03090400.xhp \
01/03060000.xhp \
01/03090500.xhp \
01/03040000.xhp \
01/03090901.xhp \
01/05040000.xhp \
01/03090906.xhp \
01/03091509.xhp \
01/03090909.xhp \
01/03090300.xhp \
01/03090700.xhp \
01/03091504.xhp \
01/03090900.xhp \
01/03091501.xhp \
01/05030000.xhp \
main0203.xhp \
guide/parentheses.xhp \
guide/main.xhp \
guide/newline.xhp \
guide/attributes.xhp \
guide/brackets.xhp \
guide/limits.xhp \
guide/text.xhp \
guide/comment.xhp \
guide/align.xhp \
guide/keyboard.xhp \
main0101.xhp \
main0105.xhp \
main0202.xhp \
main0503.xhp

.IF "$(MAKETARGETS)"!="genPO"

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
.ELSE
.INCLUDE :  settings.mk
.INCLUDE :  target.mk
.ENDIF
