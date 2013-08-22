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
PRJNAME=officecfg
TARGET=helper

LANGUAGE_FILELIST="\
../registry/data/org/openoffice/Office/Common.xcu \
../registry/data/org/openoffice/Office/DataAccess.xcu \
../registry/data/org/openoffice/Office/Embedding.xcu \
../registry/data/org/openoffice/Office/FormWizard.xcu \
../registry/data/org/openoffice/Office/TableWizard.xcu \
../registry/data/org/openoffice/Office/UI/BaseWindowState.xcu \
../registry/data/org/openoffice/Office/UI/BasicIDEWindowState.xcu \
../registry/data/org/openoffice/Office/UI/BibliographyCommands.xcu \
../registry/data/org/openoffice/Office/UI/CalcCommands.xcu \
../registry/data/org/openoffice/Office/UI/CalcWindowState.xcu \
../registry/data/org/openoffice/Office/UI/ChartCommands.xcu \
../registry/data/org/openoffice/Office/UI/ChartWindowState.xcu \
../registry/data/org/openoffice/Office/UI/DbBrowserWindowState.xcu \
../registry/data/org/openoffice/Office/UI/DbQueryWindowState.xcu \
../registry/data/org/openoffice/Office/UI/DbRelationWindowState.xcu \
../registry/data/org/openoffice/Office/UI/DbTableDataWindowState.xcu \
../registry/data/org/openoffice/Office/UI/DbTableWindowState.xcu \
../registry/data/org/openoffice/Office/UI/DbuCommands.xcu \
../registry/data/org/openoffice/Office/UI/DrawImpressCommands.xcu \
../registry/data/org/openoffice/Office/UI/DrawWindowState.xcu \
../registry/data/org/openoffice/Office/UI/Effects.xcu \
../registry/data/org/openoffice/Office/UI/GenericCategories.xcu \
../registry/data/org/openoffice/Office/UI/GenericCommands.xcu \
../registry/data/org/openoffice/Office/UI/ImpressWindowState.xcu \
../registry/data/org/openoffice/Office/UI/MathCommands.xcu \
../registry/data/org/openoffice/Office/UI/MathWindowState.xcu \
../registry/data/org/openoffice/Office/UI/StartModuleWindowState.xcu \
../registry/data/org/openoffice/Office/UI/WriterCommands.xcu \
../registry/data/org/openoffice/Office/UI/WriterFormWindowState.xcu \
../registry/data/org/openoffice/Office/UI/WriterGlobalWindowState.xcu \
../registry/data/org/openoffice/Office/UI/WriterReportWindowState.xcu \
../registry/data/org/openoffice/Office/UI/WriterWebWindowState.xcu \
../registry/data/org/openoffice/Office/UI/WriterWindowState.xcu \
../registry/data/org/openoffice/Office/UI.xcu \
../registry/data/org/openoffice/Office/UI/XFormsWindowState.xcu \
../registry/data/org/openoffice/Office/WebWizard.xcu \
../registry/data/org/openoffice/Office/Writer.xcu"


.INCLUDE :	settings.mk

.INCLUDE :	target.mk
