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

LANGUAGE_FILELIST=controller/dialogs/dlg_DataEditor.src \
controller/dialogs/dlg_DataSource.src \
controller/dialogs/dlg_InsertAxis_Grid.src \
controller/dialogs/dlg_ShapeFont.src \
controller/dialogs/dlg_ShapeParagraph.src \
controller/dialogs/dlg_View3D.src \
controller/dialogs/res_BarGeometry.src \
controller/dialogs/res_DataLabel_tmpl.hrc \
controller/dialogs/res_ErrorBar_tmpl.hrc \
controller/dialogs/res_LegendPosition_tmpl.hrc \
controller/dialogs/res_SecondaryAxisCheckBoxes_tmpl.hrc \
controller/dialogs/res_TextSeparator.src \
controller/dialogs/res_Titlesx_tmpl.hrc \
controller/dialogs/res_Trendline_tmpl.hrc \
controller/dialogs/Strings_AdditionalControls.src \
controller/dialogs/Strings_ChartTypes.src \
controller/dialogs/Strings_Scale.src \
controller/dialogs/Strings.src \
controller/dialogs/Strings_Statistic.src \
controller/dialogs/tp_3D_SceneAppearance.src \
controller/dialogs/tp_3D_SceneGeometry.src \
controller/dialogs/tp_3D_SceneIllumination.src \
controller/dialogs/tp_AxisLabel.src \
controller/dialogs/tp_AxisPositions.src \
controller/dialogs/tp_ChartType.src \
controller/dialogs/tp_DataSource.src \
controller/dialogs/tp_LegendPosition.src \
controller/dialogs/tp_Location.src \
controller/dialogs/tp_PolarOptions.src \
controller/dialogs/tp_RangeChooser.src \
controller/dialogs/tp_Scale.src \
controller/dialogs/tp_SeriesToAxis.src \
controller/dialogs/tp_TitleRotation.src \
controller/dialogs/tp_Wizard_TitlesAndObjects.src



.INCLUDE :	settings.mk

.INCLUDE :	target.mk
