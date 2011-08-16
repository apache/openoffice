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
PACKAGE = text/sbasic/shared
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_sbasic_shared
# edit to match the current module
MODULE  = sbasic

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   00000002.xhp \
   00000003.xhp \
   01000000.xhp \
   01010210.xhp \
   01020000.xhp \
   01020100.xhp \
   01020200.xhp \
   01020300.xhp \
   01020500.xhp \
   01030000.xhp \
   01030100.xhp \
   01030200.xhp \
   01030300.xhp \
   01030400.xhp \
   01040000.xhp \
   01050000.xhp \
   01050100.xhp \
   01050200.xhp \
   01050300.xhp \
   01170100.xhp \
   01170101.xhp \
   01170103.xhp \
   03000000.xhp \
   03010000.xhp \
   03010100.xhp \
   03010101.xhp \
   03010102.xhp \
   03010103.xhp \
   03010200.xhp \
   03010201.xhp \
   03010300.xhp \
   03010301.xhp \
   03010302.xhp \
   03010303.xhp \
   03010304.xhp \
   03010305.xhp \
   03020000.xhp \
   03020100.xhp \
   03020101.xhp \
   03020102.xhp \
   03020103.xhp \
   03020104.xhp \
   03020200.xhp \
   03020201.xhp \
   03020202.xhp \
   03020203.xhp \
   03020204.xhp \
   03020205.xhp \
   03020301.xhp \
   03020302.xhp \
   03020303.xhp \
   03020304.xhp \
   03020305.xhp \
   03020400.xhp \
   03020401.xhp \
   03020402.xhp \
   03020403.xhp \
   03020404.xhp \
   03020405.xhp \
   03020406.xhp \
   03020407.xhp \
   03020408.xhp \
   03020409.xhp \
   03020410.xhp \
   03020411.xhp \
   03020412.xhp \
   03020413.xhp \
   03020414.xhp \
   03020415.xhp \
   03030000.xhp \
   03030100.xhp \
   03030101.xhp \
   03030102.xhp \
   03030103.xhp \
   03030104.xhp \
   03030105.xhp \
   03030106.xhp \
   03030107.xhp \
   03030108.xhp \
   03030110.xhp \
   03030120.xhp \
   03030130.xhp \
   03030200.xhp \
   03030201.xhp \
   03030202.xhp \
   03030203.xhp \
   03030204.xhp \
   03030205.xhp \
   03030206.xhp \
   03030300.xhp \
   03030301.xhp \
   03030302.xhp \
   03030303.xhp \
   03050000.xhp \
   03050100.xhp \
   03050200.xhp \
   03050300.xhp \
   03050500.xhp \
   03060000.xhp \
   03060100.xhp \
   03060200.xhp \
   03060300.xhp \
   03060400.xhp \
   03060500.xhp \
   03060600.xhp \
   03070000.xhp \
   03070100.xhp \
   03070200.xhp \
   03070300.xhp \
   03070400.xhp \
   03070500.xhp \
   03070600.xhp \
   03080000.xhp \
   03080100.xhp \
   03080101.xhp \
   03080102.xhp \
   03080103.xhp \
   03080104.xhp \
   03080200.xhp \
   03080201.xhp \
   03080202.xhp \
   03080300.xhp \
   03080301.xhp \
   03080302.xhp \
   03080400.xhp \
   03080401.xhp \
   03080500.xhp \
   03080501.xhp \
   03080502.xhp \
   03080600.xhp \
   03080601.xhp \
   03080700.xhp \
   03080701.xhp \
   03080800.xhp \
   03080801.xhp \
   03080802.xhp \
   03090000.xhp \
   03090100.xhp \
   03090101.xhp \
   03090102.xhp \
   03090103.xhp \
   03090200.xhp \
   03090201.xhp \
   03090202.xhp \
   03090203.xhp \
   03090300.xhp \
   03090301.xhp \
   03090302.xhp \
   03090303.xhp \
   03090400.xhp \
   03090401.xhp \
   03090402.xhp \
   03090403.xhp \
   03090404.xhp \
   03090405.xhp \
   03090406.xhp \
   03090407.xhp \
   03090408.xhp \
   03090409.xhp \
   03090410.xhp \
   03090411.xhp \
   03090412.xhp \
   03100000.xhp \
   03100050.xhp \
   03100060.xhp \
   03100070.xhp \
   03100080.xhp \
   03100100.xhp \
   03100300.xhp \
   03100400.xhp \
   03100500.xhp \
   03100600.xhp \
   03100700.xhp \
   03100900.xhp \
   03101000.xhp \
   03101100.xhp \
   03101110.xhp \
   03101120.xhp \
   03101130.xhp \
   03101140.xhp \
   03101300.xhp \
   03101400.xhp \
   03101500.xhp \
   03101600.xhp \
   03101700.xhp \
   03102000.xhp \
   03102100.xhp \
   03102101.xhp \
   03102200.xhp \
   03102300.xhp \
   03102400.xhp \
   03102450.xhp \
   03102600.xhp \
   03102700.xhp \
   03102800.xhp \
   03102900.xhp \
   03103000.xhp \
   03103100.xhp \
   03103200.xhp \
   03103300.xhp \
   03103400.xhp \
   03103450.xhp \
   03103500.xhp \
   03103600.xhp \
   03103700.xhp \
   03103800.xhp \
   03103900.xhp \
   03104000.xhp \
   03104100.xhp \
   03104200.xhp \
   03104300.xhp \
   03104400.xhp \
   03104500.xhp \
   03104600.xhp \
   03104700.xhp \
   03110000.xhp \
   03110100.xhp \
   03120000.xhp \
   03120100.xhp \
   03120101.xhp \
   03120102.xhp \
   03120103.xhp \
   03120104.xhp \
   03120105.xhp \
   03120200.xhp \
   03120201.xhp \
   03120202.xhp \
   03120300.xhp \
   03120301.xhp \
   03120302.xhp \
   03120303.xhp \
   03120304.xhp \
   03120305.xhp \
   03120306.xhp \
   03120307.xhp \
   03120308.xhp \
   03120309.xhp \
   03120310.xhp \
   03120311.xhp \
   03120312.xhp \
   03120313.xhp \
   03120314.xhp \
   03120315.xhp \
   03120400.xhp \
   03120401.xhp \
   03120402.xhp \
   03120403.xhp \
   03130000.xhp \
   03130100.xhp \
   03130500.xhp \
   03130600.xhp \
   03130700.xhp \
   03130800.xhp \
   03131000.xhp \
   03131300.xhp \
   03131400.xhp \
   03131500.xhp \
   03131600.xhp \
   03131700.xhp \
   03131800.xhp \
   03131900.xhp \
   03132000.xhp \
   03132100.xhp \
   03132200.xhp \
   03132300.xhp \
   03132400.xhp \
   03132500.xhp \
   05060700.xhp \
   keys.xhp \
   main0211.xhp \
   main0601.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
