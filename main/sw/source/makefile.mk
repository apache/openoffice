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
PRJNAME=sw
TARGET=helper

LANGUAGE_FILELIST=core/undo/undo.src \
gcore/unocore/unocore.src \
gui/app/app.src \
gui/app/error.src \
gui/app/mn.src \
gui/chrdlg/ccoll.src \
gui/chrdlg/chardlg.src \
gui/chrdlg/chrdlg.src \
gui/chrdlg/drpcps.src \
gui/chrdlg/numpara.src \
gui/chrdlg/paradlg.src \
gui/chrdlg/swbreak.src \
gui/config/mailconfigpage.src \
gui/config/optcomp.src \
gui/config/optdlg.src \
gui/config/optload.src \
gui/config/redlopt.src \
gui/dbui/addresslistdialog.src \
gui/dbui/createaddresslistdialog.src \
gui/dbui/customizeaddresslistdialog.src \
gui/dbui/dbinsdlg.src \
gui/dbui/dbtablepreviewdialog.src \
gui/dbui/dbui.src \
gui/dbui/mailmergechildwindow.src \
gui/dbui/mailmergewizard.src \
gui/dbui/mmaddressblockpage.src \
gui/dbui/mmdocselectpage.src \
gui/dbui/mmgreetingspage.src \
gui/dbui/mmlayoutpage.src \
gui/dbui/mmmergepage.src \
gui/dbui/mmoutputpage.src \
gui/dbui/mmoutputtypepage.src \
gui/dbui/mmpreparemergepage.src \
gui/dbui/selectdbtabledialog.src \
gui/dialog/abstract.src \
gui/dialog/ascfldlg.src \
gui/dialog/dialog.src \
gui/dialog/docstdlg.src \
gui/dialog/regionsw.src \
gui/dialog/wordcountdialog.src \
gui/dochdl/dochdl.src \
gui/dochdl/selglos.src \
gui/docvw/access.src \
gui/docvw/annotation.src \
gui/docvw/docvw.src \
gui/envelp/envelp.src \
gui/envelp/envfmt.src \
gui/envelp/envlop.src \
gui/envelp/envprt.src \
gui/envelp/label.src \
gui/envelp/labfmt.src \
gui/envelp/labprt.src \
gui/envelp/mailmrge.src \
gui/fldui/changedb.src \
gui/fldui/DropDownFieldDialog.src \
gui/fldui/flddb.src \
gui/fldui/flddinf.src \
gui/fldui/flddok.src \
gui/fldui/fldfunc.src \
gui/fldui/fldref.src \
gui/fldui/fldtdlg.src \
gui/fldui/fldui.src \
gui/fldui/fldvar.src \
gui/fldui/inpdlg.src \
gui/fldui/javaedit.src \
gui/fmtui/tmpdlg.src \
gui/frmdlg/column.src \
gui/frmdlg/cption.src \
gui/frmdlg/frmpage.src \
gui/frmdlg/frmui.src \
gui/frmdlg/wrap.src \
gui/globdoc/globdoc.src \
gui/index/cnttab.src \
gui/index/idxmrk.src \
gui/index/multmrk.src \
gui/lingu/olmenu.src \
gui/misc/bookmark.src \
gui/misc/docfnote.src \
gui/misc/glosbib.src \
gui/misc/glossary.src \
gui/misc/insfnote.src \
gui/misc/insrule.src \
gui/misc/linenum.src \
gui/misc/numberingtypelistbox.src \
gui/misc/num.src \
gui/misc/outline.src \
gui/misc/pgfnote.src \
gui/misc/pggrid.src \
gui/misc/redlndlg.src \
gui/misc/srtdlg.src \
gui/ribbar/inputwin.src \
gui/ribbar/tbxanchr.src \
gui/ribbar/workctrl.src \
gui/shells/shells.src \
gui/smartmenu/stmenu.src \
gui/table/chartins.src \
gui/table/colwd.src \
gui/table/convert.src \
gui/table/instable.src \
gui/table/mergetbl.src \
gui/table/rowht.src \
gui/table/splittbl.src \
gui/table/tabledlg.src \
gui/table/table.src \
gui/table/tautofmt.src \
gui/uiview/pview.src \
gui/uiview/view.src \
gui/utlui/attrdesc.src \
gui/utlui/gloslst.src \
gui/utlui/initui.src \
gui/utlui/navipi.src \
gui/utlui/poolfmt.src \
gui/utlui/unotools.src \
gui/utlui/utlui.src \
gui/web/web.src \
gui/wrtsh/wrtsh.src


.INCLUDE :	settings.mk

.INCLUDE :	target.mk
