#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

# edit to match directory level 
PRJ		= ..$/..$/..$/..
# same for all makefiles in "helpcontent2"
PRJNAME = helpcontent2
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
