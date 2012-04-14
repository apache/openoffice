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
TARGET=xmloff_draw
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk

INC+= -I$(PRJ)$/inc$/bf_xmloff

# --- Files --------------------------------------------------------

SLOFILES =	\
		$(SLO)$/xmloff_XMLNumberStyles.obj\
		$(SLO)$/xmloff_XMLGraphicsDefaultStyle.obj\
		$(SLO)$/xmloff_viewcontext.obj\
		$(SLO)$/xmloff_eventimp.obj\
		$(SLO)$/xmloff_layerexp.obj\
		$(SLO)$/xmloff_layerimp.obj\
		$(SLO)$/xmloff_ximpshow.obj\
		$(SLO)$/xmloff_animimp.obj\
		$(SLO)$/xmloff_animexp.obj\
		$(SLO)$/xmloff_numithdl.obj\
		$(SLO)$/xmloff_sdxmlimp.obj \
		$(SLO)$/xmloff_sdxmlexp.obj \
		$(SLO)$/xmloff_ximpstyl.obj \
		$(SLO)$/xmloff_ximpbody.obj \
		$(SLO)$/xmloff_ximpshap.obj \
		$(SLO)$/xmloff_ximpgrp.obj \
		$(SLO)$/xmloff_ximp3dscene.obj \
		$(SLO)$/xmloff_ximp3dobject.obj \
		$(SLO)$/xmloff_ximpnote.obj \
		$(SLO)$/xmloff_sdpropls.obj \
		$(SLO)$/xmloff_propimp0.obj \
		$(SLO)$/xmloff_xexptran.obj \
		$(SLO)$/xmloff_shapeexport.obj \
		$(SLO)$/xmloff_shapeexport2.obj \
		$(SLO)$/xmloff_shapeexport3.obj \
		$(SLO)$/xmloff_shapeimport.obj \
		$(SLO)$/xmloff_XMLImageMapExport.obj \
		$(SLO)$/xmloff_XMLImageMapContext.obj \
		$(SLO)$/xmloff_XMLShapePropertySetContext.obj \
		$(SLO)$/xmloff_XMLShapeStyleContext.obj \
		$(SLO)$/xmloff_ximppage.obj

# --- Targets --------------------------------------------------------------

.INCLUDE : target.mk

