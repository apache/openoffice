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
