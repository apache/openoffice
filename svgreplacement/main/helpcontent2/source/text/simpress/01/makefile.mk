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
PACKAGE = text/simpress/01
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_simpress_01
# edit to match the current module
MODULE  = simpress

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   01170000.xhp \
   01180000.xhp \
   01180001.xhp \
   01180002.xhp \
   02110000.xhp \
   02120000.xhp \
   02130000.xhp \
   02140000.xhp \
   02150000.xhp \
   02160000.xhp \
   03050000.xhp \
   03060000.xhp \
   03070000.xhp \
   03080000.xhp \
   03090000.xhp \
   03100000.xhp \
   03110000.xhp \
   03120000.xhp \
   03130000.xhp \
   03150000.xhp \
   03150100.xhp \
   03150300.xhp \
   03151000.xhp \
   03151100.xhp \
   03151200.xhp \
   03152000.xhp \
   03180000.xhp \
   04010000.xhp \
   04020000.xhp \
   04030000.xhp \
   04030000m.xhp \
   04030100.xhp \
   04040000m.xhp \
   04080100.xhp \
   04110000.xhp \
   04110100.xhp \
   04110200.xhp \
   04120000.xhp \
   04130000.xhp \
   04140000.xhp \
   04990000.xhp \
   04990100.xhp \
   04990200.xhp \
   04990300.xhp \
   04990400.xhp \
   04990500.xhp \
   04990600.xhp \
   04990700.xhp \
   05090000m.xhp \
   05100000.xhp \
   05110500m.xhp \
   05120000.xhp \
   05120100.xhp \
   05120500m.xhp \
   05130000.xhp \
   05140000.xhp \
   05150000.xhp \
   05170000.xhp \
   05250000.xhp \
   05250500.xhp \
   05250600.xhp \
   05250700.xhp \
   06030000.xhp \
   06040000.xhp \
   06050000.xhp \
   06060000.xhp \
   06070000.xhp \
   06080000.xhp \
   06100000.xhp \
   06100100.xhp \
   13050000.xhp \
   13050100.xhp \
   13050200.xhp \
   13050300.xhp \
   13050400.xhp \
   13050500.xhp \
   13050600.xhp \
   13050700.xhp \
   13140000.xhp \
   13150000.xhp \
   13160000.xhp \
   13170000.xhp \
   13180000.xhp \
   13180100.xhp \
   13180200.xhp \
   13180300.xhp \
   animationeffect.xhp \
   effectoptions.xhp \
   effectoptionseffect.xhp \
   effectoptionstext.xhp \
   effectoptionstiming.xhp \
   slidesorter.xhp \
   taskpanel.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
