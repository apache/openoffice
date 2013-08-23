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
# same for all makefiles in "helpcontent2"
PRJNAME = helpcontent2

# edit to match the current package
PACKAGE = text/sdraw
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_sdraw
# edit to match the current module
MODULE  = sdraw

# --- Settings -----------------------------------------------------
LANGUAGE_FILELIST=text/sdraw/00/00000004.xhp,\
text/sdraw/01/04010000.xhp,\
text/sdraw/04/01020000.xhp,\
text/sdraw/guide/align_arrange.xhp,\
text/sdraw/guide/color_define.xhp,\
text/sdraw/guide/combine_etc.xhp,\
text/sdraw/guide/cross_fading.xhp,\
text/sdraw/guide/draw_sector.xhp,\
text/sdraw/guide/duplicate_object.xhp,\
text/sdraw/guide/eyedropper.xhp,\
text/sdraw/guide/gradient.xhp,\
text/sdraw/guide/graphic_insert.xhp,\
text/sdraw/guide/groups.xhp,\
text/sdraw/guide/join_objects3d.xhp,\
text/sdraw/guide/join_objects.xhp,\
text/sdraw/guide/keyboard.xhp,\
text/sdraw/guide/main.xhp,\
text/sdraw/guide/rotate_object.xhp,\
text/sdraw/guide/text_enter.xhp,\
text/sdraw/main0000.xhp,\
text/sdraw/main0100.xhp,\
text/sdraw/main0101.xhp,\
text/sdraw/main0102.xhp,\
text/sdraw/main0103.xhp,\
text/sdraw/main0104.xhp,\
text/sdraw/main0105.xhp,\
text/sdraw/main0106.xhp,\
text/sdraw/main0200.xhp,\
text/sdraw/main0202.xhp,\
text/sdraw/main0210.xhp,\
text/sdraw/main0213.xhp,\
text/sdraw/main0503.xhp"

.IF "$(MAKETARGETS)"!="genPO"
.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   main0000.xhp \
   main0100.xhp \
   main0101.xhp \
   main0102.xhp \
   main0103.xhp \
   main0104.xhp \
   main0105.xhp \
   main0106.xhp \
   main0200.xhp \
   main0202.xhp \
   main0210.xhp \
   main0213.xhp \
   main0503.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
.ELSE
.INCLUDE :  target.mk
.ENDIF
