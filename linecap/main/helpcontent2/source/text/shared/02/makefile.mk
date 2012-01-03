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
PACKAGE = text/shared/02
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_shared_02
# edit to match the current module
MODULE  = shared

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   01110000.xhp \
   01140000.xhp \
   01170000.xhp \
   01170001.xhp \
   01170002.xhp \
   01170003.xhp \
   01170004.xhp \
   01170100.xhp \
   01170101.xhp \
   01170102.xhp \
   01170103.xhp \
   01170200.xhp \
   01170201.xhp \
   01170202.xhp \
   01170203.xhp \
   01170300.xhp \
   01170400.xhp \
   01170500.xhp \
   01170600.xhp \
   01170700.xhp \
   01170800.xhp \
   01170801.xhp \
   01170802.xhp \
   01170900.xhp \
   01170901.xhp \
   01170902.xhp \
   01170903.xhp \
   01170904.xhp \
   01171000.xhp \
   01171100.xhp \
   01171200.xhp \
   01171300.xhp \
   01171400.xhp \
   01220000.xhp \
   01230000.xhp \
   02010000.xhp \
   02020000.xhp \
   02030000.xhp \
   02040000.xhp \
   02050000.xhp \
   02130000.xhp \
   02140000.xhp \
   02160000.xhp \
   02170000.xhp \
   03110000.xhp \
   03120000.xhp \
   03130000.xhp \
   03140000.xhp \
   03150000.xhp \
   03200000.xhp \
   04210000.xhp \
   05020000.xhp \
   05090000.xhp \
   05110000.xhp \
   06050000.xhp \
   06060000.xhp \
   06100000.xhp \
   06110000.xhp \
   06120000.xhp \
   07010000.xhp \
   07060000.xhp \
   07070000.xhp \
   07070100.xhp \
   07070200.xhp \
   07080000.xhp \
   07090000.xhp \
   08010000.xhp \
   08020000.xhp \
   09010000.xhp \
   09020000.xhp \
   09030000.xhp \
   09050000.xhp \
   09060000.xhp \
   09070000.xhp \
   09070100.xhp \
   09070200.xhp \
   09070300.xhp \
   09070400.xhp \
   10010000.xhp \
   10020000.xhp \
   10030000.xhp \
   10040000.xhp \
   10100000.xhp \
   12000000.xhp \
   12010000.xhp \
   12020000.xhp \
   12030000.xhp \
   12040000.xhp \
   12050000.xhp \
   12070000.xhp \
   12070100.xhp \
   12070200.xhp \
   12070300.xhp \
   12080000.xhp \
   12090000.xhp \
   12090100.xhp \
   12090101.xhp \
   12100000.xhp \
   12100100.xhp \
   12100200.xhp \
   12110000.xhp \
   12120000.xhp \
   12130000.xhp \
   12140000.xhp \
   13010000.xhp \
   13020000.xhp \
   14010000.xhp \
   14020000.xhp \
   14020100.xhp \
   14020200.xhp \
   14030000.xhp \
   14040000.xhp \
   14050000.xhp \
   14060000.xhp \
   14070000.xhp \
   18010000.xhp \
   18030000.xhp \
   19090000.xhp \
   20020000.xhp \
   20030000.xhp \
   20040000.xhp \
   20050000.xhp \
   20060000.xhp \
   20090000.xhp \
   20100000.xhp \
   24010000.xhp \
   24020000.xhp \
   24030000.xhp \
   24040000.xhp \
   24050000.xhp \
   24060000.xhp \
   24070000.xhp \
   24080000.xhp \
   24090000.xhp \
   24100000.xhp \
   basicshapes.xhp \
   blockarrows.xhp \
   callouts.xhp \
   colortoolbar.xhp \
   flowcharts.xhp \
   fontwork.xhp \
   more_controls.xhp \
   paintbrush.xhp \
   stars.xhp \
   symbolshapes.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
