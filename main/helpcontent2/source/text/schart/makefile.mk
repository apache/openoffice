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
PRJNAME = help_schart

# edit to match the current package
PACKAGE = text/schart
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_schart
# edit to match the current module
MODULE  = schart

# --- Settings -----------------------------------------------------
LANGUAGE_FILELIST= 00/00000004.xhp \
01/03010000.xhp \
01/04010000.xhp \
01/04020000.xhp \
01/04030000.xhp \
01/04040000.xhp \
01/04050000.xhp \
01/04050100.xhp \
01/04060000.xhp \
01/04070000.xhp \
01/05010000.xhp \
01/05010100.xhp \
01/05010200.xhp \
01/05020000.xhp \
01/05020100.xhp \
01/05020101.xhp \
01/05020200.xhp \
01/05020201.xhp \
01/05030000.xhp \
01/05040000.xhp \
01/05040100.xhp \
01/05040200.xhp \
01/05040201.xhp \
01/05040202.xhp \
01/05050000.xhp \
01/05050100.xhp \
01/05060000.xhp \
01/05070000.xhp \
01/05080000.xhp \
01/05120000.xhp \
01/choose_chart_type.xhp \
01/smooth_line_properties.xhp \
01/three_d_view.xhp \
01/type_area.xhp \
01/type_bubble.xhp \
01/type_column_bar.xhp \
01/type_column_line.xhp \
01/type_line.xhp \
01/type_net.xhp \
01/type_pie.xhp \
01/type_stock.xhp \
01/type_xy.xhp \
01/wiz_chart_elements.xhp \
01/wiz_chart_type.xhp \
01/wiz_data_range.xhp \
01/wiz_data_series.xhp \
02/01190000.xhp \
02/01200000.xhp \
02/01210000.xhp \
02/01220000.xhp \
02/02020000.xhp \
04/01020000.xhp \
main0000.xhp \
main0202.xhp \
main0503.xhp

.IF "$(MAKETARGETS)"!="genPO"
.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   main0000.xhp \
   main0202.xhp \
   main0503.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
.ELSE
.INCLUDE :  settings.mk
.INCLUDE :  target.mk
.ENDIF
