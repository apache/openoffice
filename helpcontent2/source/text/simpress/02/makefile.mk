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
PACKAGE = text/simpress/02
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_simpress_02
# edit to match the current module
MODULE  = simpress

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   04010000.xhp \
   04020000.xhp \
   04030000.xhp \
   04040000.xhp \
   04060000.xhp \
   04070000.xhp \
   08020000.xhp \
   08060000.xhp \
   10020000.xhp \
   10030000.xhp \
   10030200.xhp \
   10050000.xhp \
   10060000.xhp \
   10070000.xhp \
   10080000.xhp \
   10090000.xhp \
   10100000.xhp \
   10110000.xhp \
   10120000.xhp \
   10130000.xhp \
   11060000.xhp \
   11070000.xhp \
   11080000.xhp \
   11090000.xhp \
   11100000.xhp \
   11110000.xhp \
   13010000.xhp \
   13020000.xhp \
   13030000.xhp \
   13040000.xhp \
   13050000.xhp \
   13060000.xhp \
   13070000.xhp \
   13080000.xhp \
   13090000.xhp \
   13100000.xhp \
   13140000.xhp \
   13150000.xhp \
   13160000.xhp \
   13170000.xhp \
   13180000.xhp \
   13190000.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
