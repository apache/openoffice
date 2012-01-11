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
TARGET=xmloff_forms

ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
INC+= -I$(PRJ)$/inc$/bf_xmloff

# --- Files --------------------------------------------------------

SLOFILES =	\
		$(SLO)$/xmloff_formstyles.obj \
		$(SLO)$/xmloff_officeforms.obj \
		$(SLO)$/xmloff_formevents.obj \
		$(SLO)$/xmloff_eventimport.obj \
		$(SLO)$/xmloff_eventexport.obj \
		$(SLO)$/xmloff_controlpropertyhdl.obj \
		$(SLO)$/xmloff_controlpropertymap.obj \
		$(SLO)$/xmloff_valueproperties.obj \
		$(SLO)$/xmloff_attriblistmerge.obj \
		$(SLO)$/xmloff_controlelement.obj \
		$(SLO)$/xmloff_formlayerexport.obj \
		$(SLO)$/xmloff_layerexport.obj \
		$(SLO)$/xmloff_elementexport.obj \
		$(SLO)$/xmloff_propertyexport.obj \
		$(SLO)$/xmloff_elementimport.obj \
		$(SLO)$/xmloff_layerimport.obj \
		$(SLO)$/xmloff_propertyimport.obj \
		$(SLO)$/xmloff_formlayerimport.obj \
		$(SLO)$/xmloff_formattributes.obj \
		$(SLO)$/xmloff_formenums.obj \
		$(SLO)$/xmloff_strings.obj \
		$(SLO)$/xmloff_logging.obj \
		$(SLO)$/xmloff_formcellbinding.obj \

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk
