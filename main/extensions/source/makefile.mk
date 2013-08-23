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

PRJ=..
PRJNAME=extensions
TARGET=none

LANGUAGE_FILELIST=abpilot/abspilot.src \
bibliography/bib.src \
bibliography/datman.src \
bibliography/sections.src \
bibliography/toolbar.src \
dbpilots/commonpagesdbp.src \
dbpilots/dbpilots.src \
dbpilots/gridpages.src \
dbpilots/groupboxpages.src \
dbpilots/listcombopages.src \
preload/preload.src \
propctrlr/fontdialog.src \
propctrlr/formlinkdialog.src \
propctrlr/formres.src \
propctrlr/newdatatype.src \
propctrlr/pcrmiscres.src \
propctrlr/propres.src \
propctrlr/selectlabeldialog.src \
propctrlr/taborder.src \
scanner/grid.src \
scanner/sanedlg.src \
update/check/Addons.xcu \
update/check/updatehdl.src


.INCLUDE :	settings.mk

.INCLUDE :	target.mk
