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
PRJNAME=sd
TARGET=helper

LANGUAGE_FILELIST="\
../util/hidother.src \
filter/html/pubdlg.src \
ui/dlg/sdpreslt.src \
ui/dlg/inspagob.src \
ui/dlg/dlgsnap.src \
ui/dlg/dlgfield.src \
ui/dlg/vectdlg.src \
ui/dlg/tabtempl.src \
ui/dlg/custsdlg.src \
ui/dlg/dlgass.src \
ui/dlg/PaneDockingWindow.src \
ui/dlg/headerfooterdlg.src \
ui/dlg/present.src \
ui/dlg/dlgolbul.src \
ui/dlg/prltempl.src \
ui/dlg/copydlg.src \
ui/dlg/paragr.src \
ui/dlg/dlg_char.src \
ui/dlg/dlgpage.src \
ui/dlg/tpaction.src \
ui/dlg/brkdlg.src \
ui/dlg/masterlayoutdlg.src \
ui/dlg/tpoption.src \
ui/dlg/morphdlg.src \
ui/dlg/navigatr.src \
ui/dlg/layeroptionsdlg.src \
ui/dlg/ins_paste.src \
ui/dlg/LayerDialog.src \
ui/dlg/animobjs.src \
ui/dlg/prntopts.src \
ui/annotations/annotations.src \
ui/accessibility/accessibility.src \
ui/slideshow/slideshow.src \
ui/app/menuids4_tmpl.src \
ui/app/app.src \
ui/app/popup2_tmpl.src \
ui/app/strings.src \
ui/app/res_bmp.src \
ui/app/menuids_tmpl.src \
ui/app/toolbox2_tmpl.src \
ui/app/menuids3_tmpl.src \
ui/app/menuportal_tmpl.src \
ui/app/toolbox.src \
ui/app/accelids_tmpl.src \
ui/app/menu_tmpl.src \
ui/app/popup.src \
ui/app/sdstring.src \
ui/app/menuids2_tmpl.src \
ui/app/tbxids_tmpl.src \
ui/animations/CustomAnimationPane.src \
ui/animations/CustomAnimationCreateDialog.src \
ui/animations/CustomAnimationDialog.src \
ui/animations/CustomAnimation.src \
ui/animations/SlideTransitionPane.src \
ui/slidesorter/shell/SlideSorterChildWindow.src \
ui/slidesorter/view/SlsResource.src \
ui/view/DocumentRenderer.src \
ui/table/TableDesignPane.src \
ui/notes/NotesChildWindow.src \
core/glob.src"

.INCLUDE :	settings.mk

.INCLUDE :	target.mk
