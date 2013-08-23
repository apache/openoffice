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
PRJNAME=sfx2
TARGET=helper

LANGUAGE_FILELIST=../util/hidother.src \
menu/menu.src \
doc/doctempl.src \
doc/doctdlg.src \
doc/graphhelp.src \
doc/doc.src \
doc/docvor.src \
doc/new.src \
bastyp/fltfnc.src \
bastyp/bastyp.src \
dialog/printopt.src \
dialog/templdlg.src \
dialog/securitypage.src \
dialog/versdlg.src \
dialog/mgetempl.src \
dialog/filedlghelper.src \
dialog/srchdlg.src \
dialog/passwd.src \
dialog/dinfdlg.src \
dialog/dinfedt.src \
dialog/newstyle.src \
dialog/recfloat.src \
dialog/titledockwin.src \
dialog/mailwindow.src \
dialog/dialog.src \
dialog/taskpane.src \
dialog/alienwarn.src \
view/view.src \
sidebar/Sidebar.src \
appl/dde.src \
appl/sfx.src \
appl/app.src \
appl/newhelp.src

.INCLUDE :	settings.mk

.INCLUDE :	target.mk
