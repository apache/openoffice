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
PRJNAME=chart2
TARGET=none

LANGUAGE_FILELIST="\
gcontroller/dialogs/dlg_DataEditor.src \
gcontroller/dialogs/dlg_DataSource.src \
gcontroller/dialogs/dlg_InsertAxis_Grid.src \
gcontroller/dialogs/dlg_ShapeFont.src \
gcontroller/dialogs/dlg_ShapeParagraph.src \
gcontroller/dialogs/dlg_View3D.src \
gcontroller/dialogs/res_BarGeometry.src \
gcontroller/dialogs/res_DataLabel_tmpl.hrc \
gcontroller/dialogs/res_ErrorBar_tmpl.hrc \
gcontroller/dialogs/res_LegendPosition_tmpl.hrc \
gcontroller/dialogs/res_SecondaryAxisCheckBoxes_tmpl.hrc \
gcontroller/dialogs/res_TextSeparator.src \
gcontroller/dialogs/res_Titlesx_tmpl.hrc \
gcontroller/dialogs/res_Trendline_tmpl.hrc \
gcontroller/dialogs/Strings_AdditionalControls.src \
gcontroller/dialogs/Strings_ChartTypes.src \
gcontroller/dialogs/Strings_Scale.src \
gcontroller/dialogs/Strings.src \
gcontroller/dialogs/Strings_Statistic.src \
gcontroller/dialogs/tp_3D_SceneAppearance.src \
gcontroller/dialogs/tp_3D_SceneGeometry.src \
gcontroller/dialogs/tp_3D_SceneIllumination.src \
gcontroller/dialogs/tp_AxisLabel.src \
gcontroller/dialogs/tp_AxisPositions.src \
gcontroller/dialogs/tp_ChartType.src \
gcontroller/dialogs/tp_DataSource.src \
gcontroller/dialogs/tp_LegendPosition.src \
gcontroller/dialogs/tp_Location.src \
gcontroller/dialogs/tp_PolarOptions.src \
gcontroller/dialogs/tp_RangeChooser.src \
gcontroller/dialogs/tp_Scale.src \
gcontroller/dialogs/tp_SeriesToAxis.src \
gcontroller/dialogs/tp_TitleRotation.src \
gcontroller/dialogs/tp_Wizard_TitlesAndObjects.src\
"


.INCLUDE :	settings.mk

.INCLUDE :	target.mk
