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
core/unocore/unocore.src \
ui/app/app.src \
ui/app/error.src \
ui/app/mn.src \
ui/chrdlg/ccoll.src \
ui/chrdlg/chardlg.src \
ui/chrdlg/chrdlg.src \
ui/chrdlg/drpcps.src \
ui/chrdlg/numpara.src \
ui/chrdlg/paradlg.src \
ui/chrdlg/swbreak.src \
ui/config/mailconfigpage.src \
ui/config/optcomp.src \
ui/config/optdlg.src \
ui/config/optload.src \
ui/config/redlopt.src \
ui/dbui/addresslistdialog.src \
ui/dbui/createaddresslistdialog.src \
ui/dbui/customizeaddresslistdialog.src \
ui/dbui/dbinsdlg.src \
ui/dbui/dbtablepreviewdialog.src \
ui/dbui/dbui.src \
ui/dbui/mailmergechildwindow.src \
ui/dbui/mailmergewizard.src \
ui/dbui/mmaddressblockpage.src \
ui/dbui/mmdocselectpage.src \
ui/dbui/mmgreetingspage.src \
ui/dbui/mmlayoutpage.src \
ui/dbui/mmmergepage.src \
ui/dbui/mmoutputpage.src \
ui/dbui/mmoutputtypepage.src \
ui/dbui/mmpreparemergepage.src \
ui/dbui/selectdbtabledialog.src \
ui/dialog/abstract.src \
ui/dialog/ascfldlg.src \
ui/dialog/dialog.src \
ui/dialog/docstdlg.src \
ui/dialog/regionsw.src \
ui/dialog/wordcountdialog.src \
ui/dochdl/dochdl.src \
ui/dochdl/selglos.src \
ui/docvw/access.src \
ui/docvw/annotation.src \
ui/docvw/docvw.src \
ui/envelp/envelp.src \
ui/envelp/envfmt.src \
ui/envelp/envlop.src \
ui/envelp/envprt.src \
ui/envelp/label.src \
ui/envelp/labfmt.src \
ui/envelp/labprt.src \
ui/envelp/mailmrge.src \
ui/fldui/changedb.src \
ui/fldui/DropDownFieldDialog.src \
ui/fldui/flddb.src \
ui/fldui/flddinf.src \
ui/fldui/flddok.src \
ui/fldui/fldfunc.src \
ui/fldui/fldref.src \
ui/fldui/fldtdlg.src \
ui/fldui/fldui.src \
ui/fldui/fldvar.src \
ui/fldui/inpdlg.src \
ui/fldui/javaedit.src \
ui/fmtui/tmpdlg.src \
ui/frmdlg/column.src \
ui/frmdlg/cption.src \
ui/frmdlg/frmpage.src \
ui/frmdlg/frmui.src \
ui/frmdlg/wrap.src \
ui/globdoc/globdoc.src \
ui/index/cnttab.src \
ui/index/idxmrk.src \
ui/index/multmrk.src \
ui/lingu/olmenu.src \
ui/misc/bookmark.src \
ui/misc/docfnote.src \
ui/misc/glosbib.src \
ui/misc/glossary.src \
ui/misc/insfnote.src \
ui/misc/insrule.src \
ui/misc/linenum.src \
ui/misc/numberingtypelistbox.src \
ui/misc/num.src \
ui/misc/outline.src \
ui/misc/pgfnote.src \
ui/misc/pggrid.src \
ui/misc/redlndlg.src \
ui/misc/srtdlg.src \
ui/ribbar/inputwin.src \
ui/ribbar/tbxanchr.src \
ui/ribbar/workctrl.src \
ui/shells/shells.src \
ui/smartmenu/stmenu.src \
ui/table/chartins.src \
ui/table/colwd.src \
ui/table/convert.src \
ui/table/instable.src \
ui/table/mergetbl.src \
ui/table/rowht.src \
ui/table/splittbl.src \
ui/table/tabledlg.src \
ui/table/table.src \
ui/table/tautofmt.src \
ui/uiview/pview.src \
ui/uiview/view.src \
ui/utlui/attrdesc.src \
ui/utlui/gloslst.src \
ui/utlui/initui.src \
ui/utlui/navipi.src \
ui/utlui/poolfmt.src \
ui/utlui/unotools.src \
ui/utlui/utlui.src \
ui/web/web.src \
ui/wrtsh/wrtsh.src \
ui/sidebar/WrapPropertyPanel.src \
ui/sidebar/PagePropertyPanel.src



.INCLUDE :	settings.mk

.INCLUDE :	target.mk
