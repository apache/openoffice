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
PRJNAME=dbaccess
TARGET=none

LANGUAGE_FILELIST="\
core/resource/strings.src,\
ext/adabas/AdabasNewDb.src,\
ext/macromigration/macromigration.src,\
sdbtools/resource/sdbt_strings.src,\
ui/app/app.src,\
ui/browser/bcommon.src,\
ui/browser/sbabrw.src,\
ui/browser/sbagrid.src,\
ui/control/TableGrantCtrl.src,\
ui/control/tabletree.src,\
ui/control/undosqledit.src,\
ui/dlg/AdabasStat.src,\
ui/dlg/admincontrols.src,\
ui/dlg/adtabdlg.src,\
ui/dlg/advancedsettings.src,\
ui/dlg/AutoControls.src,\
ui/dlg/AutoControls_tmpl.hrc,\
ui/dlg/CollectionView.src,\
ui/dlg/ConnectionPage.src,\
ui/dlg/dbadmin2.src,\
ui/dlg/dbadminsetup.src,\
ui/dlg/dbadmin.src,\
ui/dlg/dbfindex.src,\
ui/dlg/directsql.src,\
ui/dlg/dlgattr.src,\
ui/dlg/dlgsave.src,\
ui/dlg/dlgsize.src,\
ui/dlg/dsselect.src,\
ui/dlg/indexdialog.src,\
ui/dlg/paramdialog.src,\
ui/dlg/queryfilter.src,\
ui/dlg/queryorder.src,\
ui/dlg/RelationDlg.src,\
ui/dlg/sqlmessage.src,\
ui/dlg/textconnectionsettings.src,\
ui/dlg/UserAdminDlg.src,\
ui/dlg/UserAdmin.src,\
ui/inc/toolbox_tmpl.hrc,\
ui/misc/dbumiscres.src,\
ui/misc/WizardPages.src,\
ui/querydesign/querydlg.src,\
ui/querydesign/query.src,\
ui/relationdesign/relation.src,\
ui/tabledesign/table.src,\
ui/uno/copytablewizard.src,\
ui/uno/dbinteraction.src\
"

.INCLUDE :	settings.mk

.INCLUDE :	target.mk
