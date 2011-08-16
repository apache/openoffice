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
PACKAGE = text/swriter/02
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_swriter_02
# edit to match the current module
MODULE  = swriter

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   02110000.xhp \
   03210000.xhp \
   03220000.xhp \
   04090000.xhp \
   04100000.xhp \
   04220000.xhp \
   04230000.xhp \
   04240000.xhp \
   04250000.xhp \
   06040000.xhp \
   06070000.xhp \
   06080000.xhp \
   06090000.xhp \
   06120000.xhp \
   06130000.xhp \
   06140000.xhp \
   08010000.xhp \
   08080000.xhp \
   10010000.xhp \
   10020000.xhp \
   10030000.xhp \
   10050000.xhp \
   10070000.xhp \
   10080000.xhp \
   10090000.xhp \
   10110000.xhp \
   14010000.xhp \
   14020000.xhp \
   14030000.xhp \
   14040000.xhp \
   14050000.xhp \
   18010000.xhp \
   18030000.xhp \
   18030100.xhp \
   18030200.xhp \
   18030300.xhp \
   18030400.xhp \
   18030500.xhp \
   18030600.xhp \
   18030700.xhp \
   18120000.xhp \
   18130000.xhp \
   19010000.xhp \
   19020000.xhp \
   19030000.xhp \
   19040000.xhp \
   19050000.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
