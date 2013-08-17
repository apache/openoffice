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
PRJNAME=svtools
TARGET=helper

LANGUAGE_FILELIST="\
contnr/fileview.src,\
contnr/svcontnr.src,\
contnr/templwin.src,\
control/calendar.src,\
control/ctrlbox.src,\
control/ctrltool.src,\
control/filectrl.src,\
dialogs/addresstemplate.src,\
dialogs/filedlg2.src,\
dialogs/formats.src,\
dialogs/prnsetup.src,\
dialogs/so3res.src,\
dialogs/wizardmachine.src,\
filter/exportdialog.src,\
java/javaerror.src,\
misc/imagemgr.src,\
misc/langtab.src,\
misc/undo.src,\
plugapp/testtool.src,\
toolpanel/toolpanel.src,\
../workben/unodialog/roadmapskeleton.src"



.INCLUDE :	settings.mk

.INCLUDE :	target.mk
