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
PRJNAME=svx
TARGET=helper

LANGUAGE_FILELIST=accessibility/accessibility.src \
dialog/bmpmask.src \
dialog/contdlg.src \
dialog/ctredlin.src \
dialog/dlgctrl.src \
dialog/docrecovery.src \
dialog/fontwork.src \
dialog/frmsel.src \
dialog/hdft.src \
dialog/hyprlink.src \
dialog/imapdlg.src \
dialog/language.src \
dialog/optgrid.src \
dialog/passwd.src \
dialog/prtqry.src \
dialog/rubydialog.src \
dialog/ruler.src \
dialog/sdstring.src \
dialog/srchdlg.src \
dialog/svxbmpnumvalueset.src \
dialog/swframeposstrings.src \
dialog/txenctab.src \
dialog/ucsubset.src \
engine3d/float3d.src \
engine3d/string3d.src \
fmcomp/gridctrl.src \
form/datanavi.src \
form/filtnav.src \
form/fmexpl.src \
form/fmstring.src \
form/formshell.src \
gallery2/gallery.src \
gallery2/galtheme.src \
items/svxerr.src \
items/svxitems.src \
src/app.src \
src/errtxt.src \
stbctrls/stbctrls.src \
svdraw/svdstr.src \
table/table.src \
tbxctrls/colrctrl.src \
tbxctrls/extrusioncontrols.src \
tbxctrls/fontworkgallery.src \
tbxctrls/grafctrl.src \
tbxctrls/lboxctrl.src \
tbxctrls/tbcontrl.src \
tbxctrls/tbunosearchcontrollers.src \
toolbars/extrusionbar.src \
toolbars/fontworkbar.src \
unodialogs/textconversiondlgs/chinese_dialogs.src \
unodialogs/textconversiondlgs/chinese_dictionarydialog.src \
unodialogs/textconversiondlgs/chinese_direction_tmpl.hrc \
unodialogs/textconversiondlgs/chinese_translationdialog.src \
sidebar/graphic/GraphicPropertyPanel.src \
sidebar/line/LinePropertyPanel.src \
sidebar/EmptyPanel.src \
sidebar/text/TextPropertyPanel.src \
sidebar/possize/PosSizePropertyPanel.src \
sidebar/area/AreaPropertyPanel.src \
sidebar/paragraph/ParaPropertyPanel.src \
sidebar/insert/InsertPropertyPanel.src \
../inc/globlmn_tmpl.hrc


.INCLUDE :	settings.mk

.INCLUDE :	target.mk
