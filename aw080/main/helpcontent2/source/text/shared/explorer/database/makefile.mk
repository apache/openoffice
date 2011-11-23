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
PRJ		= ..$/..$/..$/..$/..
# same for all makefiles in "helpcontent2"
PRJNAME = helpcontent2
# edit to match the current package
PACKAGE = text/shared/explorer/database
# uniqe name (module wide);
# using a modified form of package should do here
TARGET  = text_shared_explorer_database
# edit to match the current module
MODULE  = shared

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/settings.pmk

# this list matches the *.xhp files to process
XHPFILES = \
   02000000.xhp \
   02000002.xhp \
   02010100.xhp \
   02010101.xhp \
   04000000.xhp \
   04030000.xhp \
   05000000.xhp \
   05000001.xhp \
   05000002.xhp \
   05000003.xhp \
   05010000.xhp \
   05010100.xhp \
   05020000.xhp \
   05020100.xhp \
   05030000.xhp \
   05030100.xhp \
   05030200.xhp \
   05030300.xhp \
   05030400.xhp \
   05040000.xhp \
   05040100.xhp \
   05040200.xhp \
   11000002.xhp \
   11020000.xhp \
   11030000.xhp \
   11030100.xhp \
   11080000.xhp \
   11090000.xhp \
   11150200.xhp \
   11170000.xhp \
   11170100.xhp \
   30000000.xhp \
   30100000.xhp \
   dabaadvprop.xhp \
   dabaadvpropdat.xhp \
   dabaadvpropgen.xhp \
   dabadoc.xhp \
   dabaprop.xhp \
   dabapropadd.xhp \
   dabapropcon.xhp \
   dabapropgen.xhp \
   dabawiz00.xhp \
   dabawiz01.xhp \
   dabawiz02.xhp \
   dabawiz02access.xhp \
   dabawiz02adabas.xhp \
   dabawiz02ado.xhp \
   dabawiz02dbase.xhp \
   dabawiz02jdbc.xhp \
   dabawiz02ldap.xhp \
   dabawiz02mysql.xhp \
   dabawiz02odbc.xhp \
   dabawiz02oracle.xhp \
   dabawiz02spreadsheet.xhp \
   dabawiz02text.xhp \
   dabawiz03auth.xhp \
   main.xhp \
   menubar.xhp \
   menuedit.xhp \
   menufile.xhp \
   menufilesave.xhp \
   menuinsert.xhp \
   menutools.xhp \
   menuview.xhp \
   migrate_macros.xhp \
   password.xhp \
   querywizard00.xhp \
   querywizard01.xhp \
   querywizard02.xhp \
   querywizard03.xhp \
   querywizard04.xhp \
   querywizard05.xhp \
   querywizard06.xhp \
   querywizard07.xhp \
   querywizard08.xhp \
   rep_datetime.xhp \
   rep_main.xhp \
   rep_navigator.xhp \
   rep_pagenumbers.xhp \
   rep_prop.xhp \
   rep_sort.xhp \
   tablewizard00.xhp \
   tablewizard01.xhp \
   tablewizard02.xhp \
   tablewizard03.xhp \
   tablewizard04.xhp \
   toolbars.xhp 
	
# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE : tg_help.mk
