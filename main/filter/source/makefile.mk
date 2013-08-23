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
PRJNAME=basic
TARGET=none

LANGUAGE_FILELIST=config/fragments/filters/calc8_template_ui.xcu \
config/fragments/filters/calc8_ui.xcu \
config/fragments/filters/calc_HTML_WebQuery_ui.xcu \
config/fragments/filters/calc_MS_Excel_2007_Binary_ui.xcu \
config/fragments/filters/calc_MS_Excel_2007_XML_Template_ui.xcu \
config/fragments/filters/calc_MS_Excel_2007_XML_ui.xcu \
config/fragments/filters/calc_StarOffice_XML_Calc_Template_ui.xcu \
config/fragments/filters/chart8_ui.xcu \
config/fragments/filters/draw8_template_ui.xcu \
config/fragments/filters/draw8_ui.xcu \
config/fragments/filters/draw_html_Export_ui.xcu \
config/fragments/filters/draw_StarOffice_XML_Draw_Template_ui.xcu \
config/fragments/filters/HTML_MasterDoc_ui.xcu \
config/fragments/filters/HTML__StarCalc__ui.xcu \
config/fragments/filters/HTML__StarWriter__ui.xcu \
config/fragments/filters/HTML_ui.xcu \
config/fragments/filters/impress8_draw_ui.xcu \
config/fragments/filters/impress8_template_ui.xcu \
config/fragments/filters/impress8_ui.xcu \
config/fragments/filters/impress_html_Export_ui.xcu \
config/fragments/filters/impress_MS_PowerPoint_2007_XML_Template_ui.xcu \
config/fragments/filters/impress_MS_PowerPoint_2007_XML_ui.xcu \
config/fragments/filters/impress_StarOffice_XML_Draw_ui.xcu \
config/fragments/filters/impress_StarOffice_XML_Impress_Template_ui.xcu \
config/fragments/filters/math8_ui.xcu \
config/fragments/filters/MS_Excel_2003_XML_ui.xcu \
config/fragments/filters/MS_Excel_4_0_Vorlage_Template_ui.xcu \
config/fragments/filters/MS_Excel_5_0_95_Vorlage_Template_ui.xcu \
config/fragments/filters/MS_Excel_95_Vorlage_Template_ui.xcu \
config/fragments/filters/MS_Excel_97_Vorlage_Template_ui.xcu \
config/fragments/filters/MS_PowerPoint_97_Vorlage_ui.xcu \
config/fragments/filters/MS_Word_2003_XML_ui.xcu \
config/fragments/filters/MS_Word_2007_XML_Template_ui.xcu \
config/fragments/filters/MS_Word_2007_XML_ui.xcu \
config/fragments/filters/MS_Word_95_Vorlage_ui.xcu \
config/fragments/filters/MS_Word_97_Vorlage_ui.xcu \
config/fragments/filters/NSO_Calc_UOF2_ui.xcu \
config/fragments/filters/NSO_Impress_UOF2_ui.xcu \
config/fragments/filters/NSO_Writer_UOF2_ui.xcu \
config/fragments/filters/StarCalc_3_0_Vorlage_Template_ui.xcu \
config/fragments/filters/StarCalc_4_0_Vorlage_Template_ui.xcu \
config/fragments/filters/StarCalc_5_0_Vorlage_Template_ui.xcu \
config/fragments/filters/StarDraw_3_0_Vorlage__StarImpress__ui.xcu \
config/fragments/filters/StarDraw_3_0_Vorlage_ui.xcu \
config/fragments/filters/StarDraw_5_0_Vorlage__StarImpress__ui.xcu \
config/fragments/filters/StarDraw_5_0_Vorlage_ui.xcu \
config/fragments/filters/StarImpress_4_0_Vorlage_ui.xcu \
config/fragments/filters/StarImpress_5_0__packed__ui.xcu \
config/fragments/filters/StarImpress_5_0_Vorlage_ui.xcu \
config/fragments/filters/StarOffice_XML__Base__ui.xcu \
config/fragments/filters/StarOffice_XML__Calc__ui.xcu \
config/fragments/filters/StarOffice_XML__Chart__ui.xcu \
config/fragments/filters/StarOffice_XML__Draw__ui.xcu \
config/fragments/filters/StarOffice_XML__Impress__ui.xcu \
config/fragments/filters/StarOffice_XML__Math__ui.xcu \
config/fragments/filters/StarOffice_XML__Writer__ui.xcu \
config/fragments/filters/StarWriter_3_0_Vorlage_Template_ui.xcu \
config/fragments/filters/StarWriter_4_0_GlobalDocument_ui.xcu \
config/fragments/filters/StarWriter_4_0_Vorlage_Template_ui.xcu \
config/fragments/filters/StarWriter_5_0_GlobalDocument_ui.xcu \
config/fragments/filters/StarWriter_5_0_Vorlage_Template_ui.xcu \
config/fragments/filters/StarWriter_Web_4_0_Vorlage_Template_ui.xcu \
config/fragments/filters/StarWriter_Web_5_0_Vorlage_Template_ui.xcu \
config/fragments/filters/Text__encoded___StarWriter_GlobalDocument__ui.xcu \
config/fragments/filters/Text__encoded___StarWriter_Web__ui.xcu \
config/fragments/filters/Text__encoded__ui.xcu \
config/fragments/filters/Text__StarWriter_Web__ui.xcu \
config/fragments/filters/Text___txt___csv__StarCalc__ui.xcu \
config/fragments/filters/Text_ui.xcu \
config/fragments/filters/UOF_presentation_ui.xcu \
config/fragments/filters/UOF_spreadsheet_ui.xcu \
config/fragments/filters/UOF_text_ui.xcu \
config/fragments/filters/writer8_template_ui.xcu \
config/fragments/filters/writer8_ui.xcu \
config/fragments/filters/writerglobal8_ui.xcu \
config/fragments/filters/writerglobal8_writer_ui.xcu \
config/fragments/filters/writer_globaldocument_StarOffice_XML_Writer_GlobalDocument_ui.xcu \
config/fragments/filters/writer_globaldocument_StarOffice_XML_Writer_ui.xcu \
config/fragments/filters/writer_StarOffice_XML_Writer_Template_ui.xcu \
config/fragments/filters/writerweb8_writer_template_ui.xcu \
config/fragments/filters/writerweb8_writer_ui.xcu \
config/fragments/filters/writer_web_StarOffice_XML_Writer_ui.xcu \
config/fragments/filters/writer_web_StarOffice_XML_Writer_Web_Template_ui.xcu \
config/fragments/internalgraphicfilters/bmp_Export.xcu \
config/fragments/internalgraphicfilters/bmp_Import.xcu \
config/fragments/internalgraphicfilters/dxf_Import.xcu \
config/fragments/internalgraphicfilters/emf_Export.xcu \
config/fragments/internalgraphicfilters/emf_Import.xcu \
config/fragments/internalgraphicfilters/eps_Export.xcu \
config/fragments/internalgraphicfilters/eps_Import.xcu \
config/fragments/internalgraphicfilters/gif_Export.xcu \
config/fragments/internalgraphicfilters/gif_Import.xcu \
config/fragments/internalgraphicfilters/jpg_Export.xcu \
config/fragments/internalgraphicfilters/jpg_Import.xcu \
config/fragments/internalgraphicfilters/met_Export.xcu \
config/fragments/internalgraphicfilters/met_Import.xcu \
config/fragments/internalgraphicfilters/pbm_Export.xcu \
config/fragments/internalgraphicfilters/pbm_Import.xcu \
config/fragments/internalgraphicfilters/pcd_Import_Base16.xcu \
config/fragments/internalgraphicfilters/pcd_Import_Base4.xcu \
config/fragments/internalgraphicfilters/pcd_Import_Base.xcu \
config/fragments/internalgraphicfilters/pct_Export.xcu \
config/fragments/internalgraphicfilters/pct_Import.xcu \
config/fragments/internalgraphicfilters/pcx_Import.xcu \
config/fragments/internalgraphicfilters/pgm_Export.xcu \
config/fragments/internalgraphicfilters/pgm_Import.xcu \
config/fragments/internalgraphicfilters/png_Export.xcu \
config/fragments/internalgraphicfilters/png_Import.xcu \
config/fragments/internalgraphicfilters/ppm_Export.xcu \
config/fragments/internalgraphicfilters/pgm_Import.xcu \
config/fragments/internalgraphicfilters/png_Export.xcu \
config/fragments/internalgraphicfilters/png_Import.xcu \
config/fragments/internalgraphicfilters/ppm_Export.xcu \
config/fragments/internalgraphicfilters/ppm_Import.xcu \
config/fragments/internalgraphicfilters/psd_Import.xcu \
config/fragments/internalgraphicfilters/ras_Export.xcu \
config/fragments/internalgraphicfilters/ras_Import.xcu \
config/fragments/internalgraphicfilters/sgf_Import.xcu \
config/fragments/internalgraphicfilters/sgv_Import.xcu \
config/fragments/internalgraphicfilters/svg_Export.xcu \
config/fragments/internalgraphicfilters/svg_Import.xcu \
config/fragments/internalgraphicfilters/svm_Export.xcu \
config/fragments/internalgraphicfilters/svm_Import.xcu \
config/fragments/internalgraphicfilters/tga_Import.xcu \
config/fragments/internalgraphicfilters/tif_Export.xcu \
config/fragments/internalgraphicfilters/tif_Import.xcu \
config/fragments/internalgraphicfilters/wmf_Export.xcu \
config/fragments/internalgraphicfilters/wmf_Import.xcu \
config/fragments/internalgraphicfilters/xbm_Import.xcu \
config/fragments/internalgraphicfilters/xpm_Export.xcu \
config/fragments/internalgraphicfilters/xpm_Import.xcu \
config/fragments/types/calc8_template.xcu \
config/fragments/types/calc8.xcu \
config/fragments/types/calc_MS_Excel_2003_XML.xcu \
config/fragments/types/calc_NSO_UOF2.xcu \
config/fragments/types/chart8.xcu \
config/fragments/types/draw8_template.xcu \
config/fragments/types/draw8.xcu \
config/fragments/types/impress8_template.xcu \
config/fragments/types/impress8.xcu \
config/fragments/types/impress_NSO_UOF2.xcu \
config/fragments/types/math8.xcu \
config/fragments/types/StarBase.xcu \
config/fragments/types/writer8_template.xcu \
config/fragments/types/writer8.xcu \
config/fragments/types/writerglobal8.xcu \
config/fragments/types/writer_MS_Word_2003_XML.xcu \
config/fragments/types/writer_NSO_UOF2.xcu \
config/fragments/types/writerweb8_writer_template.xcu \
flash/impswfdialog.src \
graphicfilter/eps/epsstr.src \
pdf/impdialog.src \
pdf/pdf.src \
t602/t602filter.src \
xsltdialog/xmlfileview.src \
xsltdialog/xmlfilterdialogstrings.src \
xsltdialog/xmlfiltersettingsdialog.src \
xsltdialog/xmlfiltertabdialog.src \
xsltdialog/xmlfiltertabpagebasic.src \
xsltdialog/xmlfiltertabpagexslt.src \
xsltdialog/xmlfiltertestdialog.src



.INCLUDE :	settings.mk

.INCLUDE :	target.mk
