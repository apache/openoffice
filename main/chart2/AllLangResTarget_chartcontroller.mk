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



$(eval $(call gb_AllLangResTarget_AllLangResTarget,chartcontroller))

$(eval $(call gb_AllLangResTarget_set_reslocation,chartcontroller,chart2))

$(eval $(call gb_AllLangResTarget_add_srs,chartcontroller,\
	chartcontroller/controller \
))

$(eval $(call gb_SrsTarget_SrsTarget,chartcontroller/controller))

$(eval $(call gb_SrsTarget_set_include,chartcontroller/controller,\
	$$(INCLUDE) \
	-I$(OUTDIR)/inc \
	-I$(SRCDIR)/chart2/source/inc \
	-I$(SRCDIR)/chart2/source/controller/inc \
	-I$(SRCDIR)/chart2/source/controller/dialogs \
	-I$(WORKDIR)/inc/chart2 \
))

$(eval $(call gb_SrsTarget_add_files,chartcontroller/controller,\
	chart2/source/controller/dialogs/res_TextSeparator.src \
	chart2/source/controller/dialogs/res_BarGeometry.src \
	chart2/source/controller/dialogs/tp_ChartType.src \
	chart2/source/controller/dialogs/tp_RangeChooser.src \
	chart2/source/controller/dialogs/tp_Wizard_TitlesAndObjects.src \
	chart2/source/controller/dialogs/tp_Location.src \
	chart2/source/controller/dialogs/tp_AxisLabel.src \
	chart2/source/controller/dialogs/tp_AxisPositions.src \
	chart2/source/controller/dialogs/tp_DataLabel.src \
	chart2/source/controller/dialogs/tp_DataSource.src \
	chart2/source/controller/dialogs/tp_LegendPosition.src \
	chart2/source/controller/dialogs/tp_PointGeometry.src \
	chart2/source/controller/dialogs/tp_Scale.src \
	chart2/source/controller/dialogs/tp_Trendline.src \
	chart2/source/controller/dialogs/tp_ErrorBars.src \
	chart2/source/controller/dialogs/tp_SeriesToAxis.src \
	chart2/source/controller/dialogs/tp_TitleRotation.src \
	chart2/source/controller/dialogs/tp_3D_SceneGeometry.src \
	chart2/source/controller/dialogs/tp_3D_SceneAppearance.src \
	chart2/source/controller/dialogs/tp_3D_SceneIllumination.src \
	chart2/source/controller/dialogs/tp_PolarOptions.src \
	chart2/source/controller/dialogs/dlg_ObjectProperties.src \
	chart2/source/controller/dialogs/Strings_Statistic.src \
	chart2/source/controller/dialogs/Strings_Scale.src \
	chart2/source/controller/dialogs/Strings_ChartTypes.src \
	chart2/source/controller/dialogs/Strings_AdditionalControls.src \
	chart2/source/controller/dialogs/Strings.src \
	chart2/source/controller/dialogs/Bitmaps.src \
	chart2/source/controller/dialogs/Bitmaps_HC.src \
	chart2/source/controller/dialogs/dlg_DataEditor.src \
	chart2/source/controller/dialogs/dlg_DataSource.src \
	chart2/source/controller/dialogs/dlg_View3D.src \
	chart2/source/controller/dialogs/dlg_InsertAxis_Grid.src \
	chart2/source/controller/dialogs/dlg_InsertDataLabel.src \
	chart2/source/controller/dialogs/dlg_InsertLegend.src \
	chart2/source/controller/dialogs/dlg_InsertTrendline.src \
	chart2/source/controller/dialogs/dlg_InsertErrorBars.src \
	chart2/source/controller/dialogs/dlg_InsertTitle.src \
	chart2/source/controller/dialogs/dlg_ChartType.src \
	chart2/source/controller/dialogs/dlg_CreationWizard.src \
	chart2/source/controller/dialogs/dlg_ShapeFont.src \
	chart2/source/controller/dialogs/dlg_ShapeParagraph.src \
	chart2/source/controller/menus/ShapeContextMenu.src \
	chart2/source/controller/menus/ShapeEditContextMenu.src \
))


$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/tp_DataLabel.src) : $(WORKDIR)/inc/chart2/res_DataLabel.hrc
$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/dlg_InsertDataLabel.src) : $(WORKDIR)/inc/chart2/res_DataLabel.hrc
$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/dlg_InsertLegend.src) : $(WORKDIR)/inc/chart2/res_LegendPosition.hrc
$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/tp_Wizard_TitlesAndObjects.src) : $(WORKDIR)/inc/chart2/res_LegendPosition.hrc
$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/tp_LegendPosition.src) : $(WORKDIR)/inc/chart2/res_LegendPosition.hrc
$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/tp_Trendline.src) : $(WORKDIR)/inc/chart2/res_Trendline.hrc
$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/dlg_InsertTrendline.src) : $(WORKDIR)/inc/chart2/res_Trendline.hrc
$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/tp_ErrorBars.src) : $(WORKDIR)/inc/chart2/res_ErrorBar.hrc
$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/dlg_InsertErrorBars.src) : $(WORKDIR)/inc/chart2/res_ErrorBar.hrc
$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/tp_Wizard_TitlesAndObjects.src) : $(WORKDIR)/inc/chart2/res_Titlesx.hrc
$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/dlg_InsertTitle.src) : $(WORKDIR)/inc/chart2/res_Titlesx.hrc
$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/dlg_InsertAxis_Grid.src) : $(WORKDIR)/inc/chart2/res_SecondaryAxisCheckBoxes.hrc
$(call gb_SrsPartTarget_get_target,chart2/source/controller/dialogs/tp_Wizard_TitlesAndObjects.src) : $(WORKDIR)/inc/chart2/res_SecondaryAxisCheckBoxes.hrc


ifeq ($(strip $(WITH_LANG)),)

$(WORKDIR)/inc/chart2/res_DataLabel.hrc : $(SRCDIR)/chart2/source/controller/dialogs/res_DataLabel_tmpl.hrc
	echo copying $@
	-mkdir -p $(WORKDIR)/inc/chart2
	cp $< $@

$(WORKDIR)/inc/chart2/res_LegendPosition.hrc : $(SRCDIR)/chart2/source/controller/dialogs/res_LegendPosition_tmpl.hrc
	echo copying $@
	-mkdir -p $(WORKDIR)/inc/chart2
	cp $< $@

$(WORKDIR)/inc/chart2/res_Trendline.hrc : $(SRCDIR)/chart2/source/controller/dialogs/res_Trendline_tmpl.hrc
	echo copying $@
	-mkdir -p $(WORKDIR)/inc/chart2
	cp $< $@

$(WORKDIR)/inc/chart2/res_ErrorBar.hrc : $(SRCDIR)/chart2/source/controller/dialogs/res_ErrorBar_tmpl.hrc
	echo copying $@
	-mkdir -p $(WORKDIR)/inc/chart2
	cp $< $@

$(WORKDIR)/inc/chart2/res_Titlesx.hrc : $(SRCDIR)/chart2/source/controller/dialogs/res_Titlesx_tmpl.hrc
	echo copying $@
	-mkdir -p $(WORKDIR)/inc/chart2
	cp $< $@

$(WORKDIR)/inc/chart2/res_SecondaryAxisCheckBoxes.hrc : $(SRCDIR)/chart2/source/controller/dialogs/res_SecondaryAxisCheckBoxes_tmpl.hrc
	echo copying $@
	-mkdir -p $(WORKDIR)/inc/chart2
	cp $< $@

else

$(WORKDIR)/inc/chart2/res_DataLabel.hrc : $(SRCDIR)/chart2/source/controller/dialogs/res_DataLabel_tmpl.hrc
	echo merging $@
	-mkdir -p $(WORKDIR)/inc/chart2
	$(call gb_Helper_abbreviate_dirs_native, \
		$(gb_SrsPartMergeTarget_TRANSEXCOMMAND) \
		-p chart2 \
		-i $< -o $@ -m $(gb_SrsPartMergeTarget_SDFLOCATION)/chart2/source/controller/dialogs/localize.sdf -l all)

$(WORKDIR)/inc/chart2/res_LegendPosition.hrc : $(SRCDIR)/chart2/source/controller/dialogs/res_LegendPosition_tmpl.hrc
	echo merging $@
	-mkdir -p $(WORKDIR)/inc/chart2
	$(call gb_Helper_abbreviate_dirs_native, \
		$(gb_SrsPartMergeTarget_TRANSEXCOMMAND) \
		-p chart2 \
		-i $< -o $@ -m $(gb_SrsPartMergeTarget_SDFLOCATION)/chart2/source/controller/dialogs/localize.sdf -l all)

$(WORKDIR)/inc/chart2/res_Trendline.hrc : $(SRCDIR)/chart2/source/controller/dialogs/res_Trendline_tmpl.hrc
	echo merging $@
	-mkdir -p $(WORKDIR)/inc/chart2
	$(call gb_Helper_abbreviate_dirs_native, \
		$(gb_SrsPartMergeTarget_TRANSEXCOMMAND) \
		-p chart2 \
		-i $< -o $@ -m $(gb_SrsPartMergeTarget_SDFLOCATION)/chart2/source/controller/dialogs/localize.sdf -l all)

$(WORKDIR)/inc/chart2/res_ErrorBar.hrc : $(SRCDIR)/chart2/source/controller/dialogs/res_ErrorBar_tmpl.hrc
	echo merging $@
	-mkdir -p $(WORKDIR)/inc/chart2
	$(call gb_Helper_abbreviate_dirs_native, \
		$(gb_SrsPartMergeTarget_TRANSEXCOMMAND) \
		-p chart2 \
		-i $< -o $@ -m $(gb_SrsPartMergeTarget_SDFLOCATION)/chart2/source/controller/dialogs/localize.sdf -l all)

$(WORKDIR)/inc/chart2/res_Titlesx.hrc : $(SRCDIR)/chart2/source/controller/dialogs/res_Titlesx_tmpl.hrc
	echo merging $@
	-mkdir -p $(WORKDIR)/inc/chart2
	$(call gb_Helper_abbreviate_dirs_native, \
		$(gb_SrsPartMergeTarget_TRANSEXCOMMAND) \
		-p chart2 \
		-i $< -o $@ -m $(gb_SrsPartMergeTarget_SDFLOCATION)/chart2/source/controller/dialogs/localize.sdf -l all)

$(WORKDIR)/inc/chart2/res_SecondaryAxisCheckBoxes.hrc : $(SRCDIR)/chart2/source/controller/dialogs/res_SecondaryAxisCheckBoxes_tmpl.hrc
	echo merging $@
	-mkdir -p $(WORKDIR)/inc/chart2
	$(call gb_Helper_abbreviate_dirs_native, \
		$(gb_SrsPartMergeTarget_TRANSEXCOMMAND) \
		-p chart2 \
		-i $< -o $@ -m $(gb_SrsPartMergeTarget_SDFLOCATION)/chart2/source/controller/dialogs/localize.sdf -l all)

endif


.PHONY : $(WORKDIR)/inc/chart2/hrc_clean
	rm -f \
		$(WORKDIR)/inc/chart2/res_DataLabel.hrc \
		$(WORKDIR)/inc/chart2/res_LegendPosition.hrc \
		$(WORKDIR)/inc/chart2/res_Trendline.hrc \
		$(WORKDIR)/inc/chart2/res_ErrorBar.hrc \
		$(WORKDIR)/inc/chart2/res_Titlesx.hrc \
		$(WORKDIR)/inc/chart2/res_SecondaryAxisCheckBoxes.hrc

# vim: set noet sw=4 ts=4:
