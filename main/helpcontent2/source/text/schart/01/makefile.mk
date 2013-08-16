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
PACKAGE = text/schart/01
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_schart_01
# edit to match the current module
MODULE  = schart

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   03010000.xhp \
   04010000.xhp \
   04020000.xhp \
   04030000.xhp \
   04040000.xhp \
   04050000.xhp \
   04050100.xhp \
   04060000.xhp \
   04070000.xhp \
   05010000.xhp \
   05010100.xhp \
   05010200.xhp \
   05020000.xhp \
   05020100.xhp \
   05020101.xhp \
   05020200.xhp \
   05020201.xhp \
   05030000.xhp \
   05040000.xhp \
   05040100.xhp \
   05040200.xhp \
   05040201.xhp \
   05040202.xhp \
   05050000.xhp \
   05050100.xhp \
   05060000.xhp \
   05070000.xhp \
   05080000.xhp \
   05120000.xhp \
   choose_chart_type.xhp \
   smooth_line_properties.xhp \
   three_d_view.xhp \
   type_area.xhp \
   type_bubble.xhp \
   type_column_bar.xhp \
   type_column_line.xhp \
   type_line.xhp \
   type_net.xhp \
   type_pie.xhp \
   type_stock.xhp \
   type_xy.xhp \
   wiz_chart_elements.xhp \
   wiz_chart_type.xhp \
   wiz_data_range.xhp \
   wiz_data_series.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
.ELSE
.INCLUDE :  target.mk
.ENDIF
