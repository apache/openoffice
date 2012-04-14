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



EXTERNAL_WARNINGS_NOT_ERRORS := TRUE
PRJ=..$/..$/..$/..
BFPRJ=..$/..$/..

PRJNAME=binfilter
TARGET=sc_xml

NO_HIDS=TRUE

ENABLE_EXCEPTIONS=TRUE

PROJECTPCH4DLL=TRUE
PROJECTPCH=filt_pch
PROJECTPCHSOURCE=..\pch\sc_filt_pch

# --- Settings -----------------------------------------------------

.INCLUDE :  scpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sc.mk
.INCLUDE :  $(BFPRJ)$/util$/makefile.pmk
INC+= -I$(PRJ)$/inc$/bf_sc
# --- Files --------------------------------------------------------

CXXFILES = \
		sc_xmlwrap.cxx \
		sc_xmlimprt.cxx \
		sc_xmlexprt.cxx \
		sc_xmlbodyi.cxx \
		sc_xmltabi.cxx \
		sc_xmlrowi.cxx \
		sc_xmlcelli.cxx \
		sc_xmlconti.cxx \
		sc_xmlcoli.cxx \
		sc_xmlsubti.cxx \
		sc_xmlnexpi.cxx \
		sc_xmldrani.cxx \
		sc_xmlfilti.cxx \
		sc_xmlsorti.cxx \
		sc_xmlstyle.cxx \
		sc_xmlstyli.cxx \
		sc_xmldpimp.cxx \
		sc_xmlannoi.cxx \
		sc_xmlsceni.cxx \
		sc_xmlcvali.cxx \
		sc_XMLTableMasterPageExport.cxx \
		sc_xmllabri.cxx \
		sc_XMLTableHeaderFooterContext.cxx \
		sc_XMLDetectiveContext.cxx \
		sc_XMLCellRangeSourceContext.cxx \
		sc_XMLConsolidationContext.cxx \
		sc_XMLConverter.cxx \
		sc_XMLExportIterator.cxx \
		sc_XMLColumnRowGroupExport.cxx \
		sc_XMLStylesExportHelper.cxx \
		sc_XMLStylesImportHelper.cxx \
		sc_XMLExportDataPilot.cxx \
		sc_XMLExportDatabaseRanges.cxx \
		sc_XMLTableShapeImportHelper.cxx \
		sc_XMLTableShapesContext.cxx \
		sc_XMLExportDDELinks.cxx \
		sc_XMLDDELinksContext.cxx \
		sc_XMLCalculationSettingsContext.cxx \
		sc_XMLTableSourceContext.cxx \
		sc_XMLTextPContext.cxx \
		sc_XMLTableShapeResizer.cxx \
		sc_XMLChangeTrackingExportHelper.cxx \
		sc_xmlfonte.cxx \
		sc_XMLChangeTrackingImportHelper.cxx \
		sc_XMLTrackedChangesContext.cxx \
		sc_XMLExportSharedData.cxx \
        sc_XMLEmptyContext.cxx

SLOFILES =  \
		$(SLO)$/sc_xmlwrap.obj \
		$(SLO)$/sc_xmlimprt.obj \
		$(SLO)$/sc_xmlexprt.obj \
		$(SLO)$/sc_xmlbodyi.obj \
		$(SLO)$/sc_xmltabi.obj \
		$(SLO)$/sc_xmlrowi.obj \
		$(SLO)$/sc_xmlcelli.obj \
		$(SLO)$/sc_xmlconti.obj \
		$(SLO)$/sc_xmlcoli.obj \
		$(SLO)$/sc_xmlsubti.obj \
		$(SLO)$/sc_xmlnexpi.obj \
		$(SLO)$/sc_xmldrani.obj \
		$(SLO)$/sc_xmlfilti.obj \
		$(SLO)$/sc_xmlsorti.obj \
		$(SLO)$/sc_xmlstyle.obj \
		$(SLO)$/sc_xmlstyli.obj \
		$(SLO)$/sc_xmldpimp.obj \
		$(SLO)$/sc_xmlannoi.obj \
		$(SLO)$/sc_xmlsceni.obj \
		$(SLO)$/sc_xmlcvali.obj \
		$(SLO)$/sc_XMLTableMasterPageExport.obj \
		$(SLO)$/sc_xmllabri.obj \
		$(SLO)$/sc_XMLTableHeaderFooterContext.obj \
		$(SLO)$/sc_XMLDetectiveContext.obj \
		$(SLO)$/sc_XMLCellRangeSourceContext.obj \
		$(SLO)$/sc_XMLConsolidationContext.obj \
		$(SLO)$/sc_XMLConverter.obj \
		$(SLO)$/sc_XMLExportIterator.obj \
		$(SLO)$/sc_XMLColumnRowGroupExport.obj \
		$(SLO)$/sc_XMLStylesExportHelper.obj \
		$(SLO)$/sc_XMLStylesImportHelper.obj \
		$(SLO)$/sc_XMLExportDataPilot.obj \
		$(SLO)$/sc_XMLExportDatabaseRanges.obj \
		$(SLO)$/sc_XMLTableShapeImportHelper.obj \
		$(SLO)$/sc_XMLTableShapesContext.obj \
		$(SLO)$/sc_XMLExportDDELinks.obj \
		$(SLO)$/sc_XMLDDELinksContext.obj \
		$(SLO)$/sc_XMLCalculationSettingsContext.obj \
		$(SLO)$/sc_XMLTableSourceContext.obj \
		$(SLO)$/sc_XMLTextPContext.obj \
		$(SLO)$/sc_XMLTableShapeResizer.obj \
		$(SLO)$/sc_XMLChangeTrackingExportHelper.obj \
		$(SLO)$/sc_xmlfonte.obj \
		$(SLO)$/sc_XMLChangeTrackingImportHelper.obj \
		$(SLO)$/sc_XMLTrackedChangesContext.obj \
		$(SLO)$/sc_XMLExportSharedData.obj \
        $(SLO)$/sc_XMLEmptyContext.obj


# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

