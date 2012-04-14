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

PRJ=..$/..$/..

PRJNAME=binfilter
TARGET=xmloff_core

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
CDEFS+=-DCONV_STAR_FONTS

INC+= -I$(PRJ)$/inc$/bf_xmloff

# --- to build xmlkywd.obj in obj, too -----------------------------

OBJFILES =  $(OBJ)$/xmloff_xmlkywd.obj
LIB2TARGET =$(LB)$/xmloff_xmlkywd.lib
LIB2OBJFILES  =$(OBJFILES)

# --- Files --------------------------------------------------------

SLOFILES =	\
		$(SLO)$/xmloff_facreg.obj		\
		$(SLO)$/xmloff_xmlcnitm.obj	\
		$(SLO)$/xmloff_attrlist.obj	\
		$(SLO)$/xmloff_i18nmap.obj		\
		$(SLO)$/xmloff_nmspmap.obj		\
		$(SLO)$/xmloff_unoatrcn.obj	\
		$(SLO)$/xmloff_xmlehelp.obj	\
		$(SLO)$/xmloff_xmlerror.obj	\
		$(SLO)$/xmloff_xmlexp.obj		\
		$(SLO)$/xmloff_xmlictxt.obj	\
		$(SLO)$/xmloff_xmlimp.obj		\
		$(SLO)$/xmloff_xmlkywd.obj		\
		$(SLO)$/xmloff_xmltkmap.obj	\
		$(SLO)$/xmloff_xmltoken.obj	\
		$(SLO)$/xmloff_xmluconv.obj	\
		$(SLO)$/xmloff_ProgressBarHelper.obj	\
		$(SLO)$/xmloff_PropertySetMerger.obj	\
		$(SLO)$/xmloff_SettingsExportHelper.obj	\
		$(SLO)$/xmloff_DocumentSettingsContext.obj \
		$(SLO)$/xmloff_XMLEmbeddedObjectExportFilter.obj \
		$(SLO)$/xmloff_XMLEmbeddedObjectImportContext.obj \
		$(SLO)$/xmloff_XMLBase64Export.obj \
		$(SLO)$/xmloff_XMLBase64ImportContext.obj \
		$(SLO)$/xmloff_XMLBasicExportFilter.obj

CPPUMAKERFLAGS=

# --- Targets -------------------------------------------------------

.INCLUDE :  target.mk
