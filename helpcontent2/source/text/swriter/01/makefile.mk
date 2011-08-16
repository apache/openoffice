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
PACKAGE = text/swriter/01
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_swriter_01
# edit to match the current module
MODULE  = swriter

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   01120000.xhp \
   01150000.xhp \
   01160100.xhp \
   01160200.xhp \
   01160300.xhp \
   01160400.xhp \
   01160500.xhp \
   02110000.xhp \
   02110100.xhp \
   02120000.xhp \
   02120100.xhp \
   02130000.xhp \
   02140000.xhp \
   02150000.xhp \
   02160000.xhp \
   02170000.xhp \
   03050000.xhp \
   03070000.xhp \
   03080000.xhp \
   03090000.xhp \
   03100000.xhp \
   03120000.xhp \
   03130000.xhp \
   03140000.xhp \
   04010000.xhp \
   04020000.xhp \
   04020100.xhp \
   04020200.xhp \
   04030000.xhp \
   04040000.xhp \
   04060000.xhp \
   04060100.xhp \
   04070000.xhp \
   04070100.xhp \
   04070200.xhp \
   04070300.xhp \
   04090000.xhp \
   04090001.xhp \
   04090002.xhp \
   04090003.xhp \
   04090004.xhp \
   04090005.xhp \
   04090006.xhp \
   04090007.xhp \
   04090100.xhp \
   04090200.xhp \
   04120000.xhp \
   04120100.xhp \
   04120200.xhp \
   04120201.xhp \
   04120210.xhp \
   04120211.xhp \
   04120212.xhp \
   04120213.xhp \
   04120214.xhp \
   04120215.xhp \
   04120216.xhp \
   04120217.xhp \
   04120219.xhp \
   04120220.xhp \
   04120221.xhp \
   04120222.xhp \
   04120223.xhp \
   04120224.xhp \
   04120225.xhp \
   04120226.xhp \
   04120227.xhp \
   04120229.xhp \
   04120250.xhp \
   04120300.xhp \
   04130000.xhp \
   04130100.xhp \
   04150000.xhp \
   04180400.xhp \
   04190000.xhp \
   04200000.xhp \
   04210000.xhp \
   04220000.xhp \
   04230000.xhp \
   04990000.xhp \
   05030200.xhp \
   05030400.xhp \
   05030800.xhp \
   05040000.xhp \
   05040500.xhp \
   05040501.xhp \
   05040600.xhp \
   05040700.xhp \
   05040800.xhp \
   05060000.xhp \
   05060100.xhp \
   05060200.xhp \
   05060201.xhp \
   05060300.xhp \
   05060700.xhp \
   05060800.xhp \
   05060900.xhp \
   05080000.xhp \
   05090000.xhp \
   05090100.xhp \
   05090200.xhp \
   05090201.xhp \
   05090300.xhp \
   05100000.xhp \
   05100300.xhp \
   05100400.xhp \
   05110000.xhp \
   05110100.xhp \
   05110200.xhp \
   05110300.xhp \
   05110500.xhp \
   05120000.xhp \
   05120100.xhp \
   05120200.xhp \
   05120300.xhp \
   05120400.xhp \
   05120500.xhp \
   05130000.xhp \
   05130002.xhp \
   05130004.xhp \
   05130100.xhp \
   05140000.xhp \
   05150000.xhp \
   05150100.xhp \
   05150101.xhp \
   05150104.xhp \
   05150200.xhp \
   05150300.xhp \
   05170000.xhp \
   05180000.xhp \
   05190000.xhp \
   05200000.xhp \
   05990000.xhp \
   06030000.xhp \
   06040000.xhp \
   06060000.xhp \
   06060100.xhp \
   06080000.xhp \
   06080100.xhp \
   06080200.xhp \
   06090000.xhp \
   06100000.xhp \
   06110000.xhp \
   06120000.xhp \
   06160000.xhp \
   06170000.xhp \
   06180000.xhp \
   06190000.xhp \
   06200000.xhp \
   06210000.xhp \
   06220000.xhp \
   06990000.xhp \
   format_object.xhp \
   mailmerge00.xhp \
   mailmerge01.xhp \
   mailmerge02.xhp \
   mailmerge03.xhp \
   mailmerge04.xhp \
   mailmerge05.xhp \
   mailmerge06.xhp \
   mailmerge07.xhp \
   mailmerge08.xhp \
   mm_copyto.xhp \
   mm_cusaddfie.xhp \
   mm_cusaddlis.xhp \
   mm_cusgrelin.xhp \
   mm_emabod.xhp \
   mm_finent.xhp \
   mm_matfie.xhp \
   mm_newaddblo.xhp \
   mm_newaddlis.xhp \
   mm_seladdblo.xhp \
   mm_seladdlis.xhp \
   mm_seltab.xhp \
   selection_mode.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
