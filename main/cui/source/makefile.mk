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
PRJNAME=cui
TARGET=none

LANGUAGE_FILELIST="\
customize/acccfg.src \
customize/cfg.src \
customize/eventdlg.src \
customize/macropg.src \
customize/selector.src \
dialogs/about.src \
dialogs/charmap.src \
dialogs/colorpicker.src \
dialogs/commonlingui.src \
dialogs/cuiimapdlg.src \
dialogs/dlgname.src \
dialogs/fmsearch.src \
dialogs/gallery.src \
dialogs/grfflt.src \
dialogs/hangulhanjadlg.src \
dialogs/hlmarkwn.src \
dialogs/hyperdlg.src \
dialogs/hyphen.src \
dialogs/iconcdlg.src \
dialogs/insrc.src \
dialogs/multipat.src \
dialogs/newtabledlg.src \
dialogs/passwdomdlg.src \
dialogs/postdlg.src \
dialogs/scriptdlg.src \
dialogs/sdrcelldlg.src \
dialogs/showcols.src \
dialogs/SpellDialog.src \
dialogs/splitcelldlg.src \
dialogs/srchxtra.src \
dialogs/svuidlg.src \
dialogs/tbxform.src \
dialogs/thesdlg.src \
dialogs/zoom.src \
options/connpooloptions.src \
options/dbregister.src \
options/doclinkdialog.src \
options/fontsubs.src \
options/internationaloptions.src \
options/optaccessibility.src \
options/optasian.src \
options/optchart.src \
options/optcolor.src \
options/optctl.src \
options/optdict.src \
options/optfltr.src \
options/optgdlg.src \
options/optgenrl.src \
options/opthtml.src \
options/optimprove.src \
options/optinet2.src \
options/optjava.src \
options/optjsearch.src \
options/optlingu.src \
options/optmemory.src \
options/optpath.src \
options/optsave.src \
options/optspell.src \
options/optupdt.src \
options/readonlyimage.src \
options/securityoptions.src \
options/treeopt.src \
options/webconninfo.src \
tabpages/align.src \
tabpages/autocdlg.src \
tabpages/backgrnd.src \
tabpages/bbdlg.src \
tabpages/border.src \
tabpages/chardlg.src \
tabpages/connect.src \
tabpages/dstribut.src \
tabpages/frmdirlbox.src \
tabpages/grfpage.src \
tabpages/labdlg.src \
tabpages/macroass.src \
tabpages/measure.src \
tabpages/numfmt.src \
tabpages/numpages.src \
tabpages/page.src \
tabpages/paragrph.src \
tabpages/strings.src \
tabpages/swpossizetabpage.src \
tabpages/tabarea.src \
tabpages/tabline.src \
tabpages/tabstpge.src \
tabpages/textanim.src \
tabpages/textattr.src \
tabpages/transfrm.src\
"


.INCLUDE :	settings.mk

.INCLUDE :	target.mk
