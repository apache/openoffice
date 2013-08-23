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

LANGUAGE_FILELIST=acessibility/accessibility.src \
gdialog/bmpmask.src \
gdialog/contdlg.src \
gdialog/ctredlin.src \
gdialog/dlgctrl.src \
gdialog/docrecovery.src \
gdialog/fontwork.src \
gdialog/frmsel.src \
gdialog/hdft.src \
gdialog/hyprlink.src \
gdialog/imapdlg.src \
gdialog/language.src \
gdialog/optgrid.src \
gdialog/passwd.src \
gdialog/prtqry.src \
gdialog/rubydialog.src \
gdialog/ruler.src \
gdialog/sdstring.src \
gdialog/srchdlg.src \
gdialog/svxbmpnumvalueset.src \
gdialog/swframeposstrings.src \
gdialog/txenctab.src \
gdialog/ucsubset.src \
gengine3d/float3d.src \
gengine3d/string3d.src \
gfmcomp/gridctrl.src \
gform/datanavi.src \
gform/filtnav.src \
gform/fmexpl.src \
gform/fmstring.src \
gform/formshell.src \
ggallery2/gallery.src \
ggallery2/galtheme.src \
gitems/svxerr.src \
gitems/svxitems.src \
gsrc/app.src \
gsrc/errtxt.src \
gstbctrls/stbctrls.src \
gsvdraw/svdstr.src \
gtable/table.src \
gtbxctrls/colrctrl.src \
gtbxctrls/extrusioncontrols.src \
gtbxctrls/fontworkgallery.src \
gtbxctrls/grafctrl.src \
gtbxctrls/lboxctrl.src \
gtbxctrls/tbcontrl.src \
gtbxctrls/tbunosearchcontrollers.src \
gtoolbars/extrusionbar.src \
gtoolbars/fontworkbar.src \
gunodialogs/textconversiondlgs/chinese_dialogs.src \
gunodialogs/textconversiondlgs/chinese_dictionarydialog.src \
gunodialogs/textconversiondlgs/chinese_direction_tmpl.hrc \
gunodialogs/textconversiondlgs/chinese_translationdialog.src


.INCLUDE :	settings.mk

.INCLUDE :	target.mk
