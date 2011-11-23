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
PACKAGE = text/scalc/01
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_scalc_01
# edit to match the current module
MODULE  = scalc

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   01120000.xhp \
   02110000.xhp \
   02120000.xhp \
   02120100.xhp \
   02140000.xhp \
   02140100.xhp \
   02140200.xhp \
   02140300.xhp \
   02140400.xhp \
   02140500.xhp \
   02140600.xhp \
   02150000.xhp \
   02160000.xhp \
   02170000.xhp \
   02180000.xhp \
   02190000.xhp \
   02190100.xhp \
   02190200.xhp \
   02200000.xhp \
   02210000.xhp \
   03070000.xhp \
   03080000.xhp \
   03090000.xhp \
   03100000.xhp \
   04010000.xhp \
   04010100.xhp \
   04010200.xhp \
   04020000.xhp \
   04030000.xhp \
   04040000.xhp \
   04050000.xhp \
   04050100.xhp \
   04060000.xhp \
   04060100.xhp \
   04060101.xhp \
   04060102.xhp \
   04060103.xhp \
   04060104.xhp \
   04060105.xhp \
   04060106.xhp \
   04060107.xhp \
   04060108.xhp \
   04060109.xhp \
   04060110.xhp \
   04060111.xhp \
   04060112.xhp \
   04060115.xhp \
   04060116.xhp \
   04060118.xhp \
   04060119.xhp \
   04060181.xhp \
   04060182.xhp \
   04060183.xhp \
   04060184.xhp \
   04060185.xhp \
   04060199.xhp \
   04070000.xhp \
   04070100.xhp \
   04070200.xhp \
   04070300.xhp \
   04070400.xhp \
   04080000.xhp \
   04090000.xhp \
   05020000.xhp \
   05020600.xhp \
   05030000.xhp \
   05030200.xhp \
   05030300.xhp \
   05030400.xhp \
   05040000.xhp \
   05040200.xhp \
   05050000.xhp \
   05050100.xhp \
   05050300.xhp \
   05060000.xhp \
   05070000.xhp \
   05070500.xhp \
   05080000.xhp \
   05080100.xhp \
   05080200.xhp \
   05080300.xhp \
   05080400.xhp \
   05100000.xhp \
   05110000.xhp \
   05120000.xhp \
   06020000.xhp \
   06030000.xhp \
   06030100.xhp \
   06030200.xhp \
   06030300.xhp \
   06030400.xhp \
   06030500.xhp \
   06030600.xhp \
   06030700.xhp \
   06030800.xhp \
   06030900.xhp \
   06031000.xhp \
   06040000.xhp \
   06050000.xhp \
   06060000.xhp \
   06060100.xhp \
   06060200.xhp \
   06070000.xhp \
   06080000.xhp \
   06130000.xhp \
   06990000.xhp \
   07080000.xhp \
   07090000.xhp \
   12010000.xhp \
   12010100.xhp \
   12020000.xhp \
   12030000.xhp \
   12030100.xhp \
   12030200.xhp \
   12040000.xhp \
   12040100.xhp \
   12040201.xhp \
   12040300.xhp \
   12040400.xhp \
   12040500.xhp \
   12050000.xhp \
   12050100.xhp \
   12050200.xhp \
   12060000.xhp \
   12070000.xhp \
   12070100.xhp \
   12080000.xhp \
   12080100.xhp \
   12080200.xhp \
   12080300.xhp \
   12080400.xhp \
   12080500.xhp \
   12080600.xhp \
   12080700.xhp \
   12090000.xhp \
   12090100.xhp \
   12090101.xhp \
   12090102.xhp \
   12090103.xhp \
   12090104.xhp \
   12090105.xhp \
   12090106.xhp \
   12090200.xhp \
   12090300.xhp \
   12090400.xhp \
   12100000.xhp \
   12120000.xhp \
   12120100.xhp \
   12120200.xhp \
   12120300.xhp \
   format_graphic.xhp \
   func_date.xhp \
   func_datevalue.xhp \
   func_day.xhp \
   func_days.xhp \
   func_days360.xhp \
   func_eastersunday.xhp \
   func_edate.xhp \
   func_eomonth.xhp \
   func_hour.xhp \
   func_minute.xhp \
   func_month.xhp \
   func_networkdays.xhp \
   func_now.xhp \
   func_second.xhp \
   func_time.xhp \
   func_timevalue.xhp \
   func_today.xhp \
   func_weekday.xhp \
   func_weeknum.xhp \
   func_weeknumadd.xhp \
   func_workday.xhp \
   func_year.xhp \
   func_yearfrac.xhp \
   solver.xhp \
   solver_options.xhp \
   text2columns.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
