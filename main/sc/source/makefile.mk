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
PRJNAME=sc
TARGET=helper

LANGUAGE_FILELIST=../util/hidother.src,\
../addin/rot13/rot13.src,\
../addin/datefunc/dfa.src,\
ui/formdlg/formdlgs.src,\
ui/formdlg/dwfunctr.src,\
ui/styleui/styledlg.src,\
ui/styleui/scstyles.src,\
ui/dbgui/imoptdlg.src,\
ui/dbgui/pivot.src,\
ui/dbgui/dapitype.src,\
ui/dbgui/textimportoptions.src,\
ui/dbgui/consdlg.src,\
ui/dbgui/dpgroupdlg.src \
ui/dbgui/outline.src \
ui/dbgui/validate.src \
ui/dbgui/pvfundlg.src \
ui/dbgui/asciiopt.src \
ui/dbgui/scendlg.src \
ui/cctrl/dpcontrol.src \
ui/docshell/tpstat.src \
ui/navipi/navipi.src \
ui/pagedlg/pagedlg.src \
ui/pagedlg/hfedtdlg.src \
ui/pagedlg/tphf.src \
ui/drawfunc/objdraw.src \
ui/drawfunc/drformsh.src \
ui/src/optdlg.src \
ui/src/pseudo.src \
ui/src/subtdlg.src \
ui/src/attrdlg.src \
ui/src/solveroptions.src \
ui/src/sortdlg.src \
ui/src/globstr.src \
ui/src/textdlgs.src \
ui/src/autofmt.src \
ui/src/filter.src \
ui/src/scfuncs.src \
ui/src/scstring.src \
ui/src/toolbox.src \
ui/src/tabopdlg.src \
ui/src/namedlg.src \
ui/src/dbnamdlg.src \
ui/src/optsolver.src \
ui/src/popup.src \
ui/src/sc.src \
ui/src/scwarngs.src \
ui/src/miscdlgs.src \
ui/src/opredlin.src \
ui/src/crnrdlg.src \
ui/src/simpref.src \
ui/src/condfrmt.src \
ui/src/solvrdlg.src \
ui/src/scerrors.src \
ui/src/hdrcont.src \
ui/sidebar/AlignmentPropertyPanel.src \
ui/sidebar/CellAppearancePropertyPanel.src \
ui/sidebar/NumberFormatPropertyPanel.src \
ui/miscdlgs/acredlin.src \
ui/miscdlgs/conflictsdlg.src \
ui/miscdlgs/retypepassdlg.src \
ui/miscdlgs/sharedocdlg.src \
ui/miscdlgs/instbdlg.src \
ui/miscdlgs/highred.src \
ui/miscdlgs/protectiondlg.src \
ui/miscdlgs/linkarea.src \
core/src/compiler.src

.INCLUDE :	settings.mk

.INCLUDE :	target.mk
