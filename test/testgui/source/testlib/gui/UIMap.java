/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/

package testlib.gui;

import java.io.File;

import org.openoffice.test.OpenOffice;
import org.openoffice.test.vcl.IDList;
import org.openoffice.test.vcl.client.Constant;
import org.openoffice.test.vcl.widgets.VclApp;
import org.openoffice.test.vcl.widgets.VclButton;
import org.openoffice.test.vcl.widgets.VclComboBox;
import org.openoffice.test.vcl.widgets.VclControl;
import org.openoffice.test.vcl.widgets.VclDialog;
import org.openoffice.test.vcl.widgets.VclDockingWin;
import org.openoffice.test.vcl.widgets.VclEditBox;
import org.openoffice.test.vcl.widgets.VclField;
import org.openoffice.test.vcl.widgets.VclListBox;
import org.openoffice.test.vcl.widgets.VclMenuItem;
import org.openoffice.test.vcl.widgets.VclMessageBox;
import org.openoffice.test.vcl.widgets.VclStatusBar;
import org.openoffice.test.vcl.widgets.VclTabControl;
import org.openoffice.test.vcl.widgets.VclTabPage;
import org.openoffice.test.vcl.widgets.VclToolBox;
import org.openoffice.test.vcl.widgets.VclTreeListBox;
import org.openoffice.test.vcl.widgets.VclWindow;

/**
 * Define all UI controls in the class. One control should be defined once and
 * only once.
 * 
 */
public class UIMap {
	private static IDList idList = new IDList(new File("./hid"));
	
	public static final OpenOffice aoo = new OpenOffice();
	
	static {
		aoo.setAutomationPort(OpenOffice.DEFAULT_AUTOMATION_PORT);
		aoo.addArgs("-nofirststartwizard", "-norestore", "-quickstart=no");
		aoo.addRegistryModifications("<item oor:path=\"/org.openoffice.Office.Common/Misc\"><prop oor:name=\"UseSystemFileDialog\" oor:op=\"fuse\"><value>false</value></prop></item>",
				"<item oor:path=\"/org.openoffice.Office.Common/Security/Scripting\"><prop oor:name=\"MacroSecurityLevel\" oor:op=\"fuse\"><value>0</value></prop></item>");
	}
	
	public static final VclApp app = new VclApp(aoo);
	
	public static VclEditBox editbox(String id) {
		return new VclEditBox(app, idList.getId(id));
	}

	public static VclControl control(String id) {
		return new VclControl(app, idList.getId(id));
	}

	public static VclWindow window(String id) {
		return new VclWindow(app, idList.getId(id));
	}

	public static VclDialog dialog(String id) {
		return new VclDialog(app, idList.getId(id));
	}

	public static VclTabPage tabpage(String id) {
		return new VclTabPage(app, idList.getId(id));
	}

	public static VclListBox listbox(String id) {
		return new VclListBox(app, idList.getId(id));
	}

	public static VclComboBox combobox(String id) {
		return new VclComboBox(app, idList.getId(id));
	}

	public static VclToolBox toolbox(String id) {
		return new VclToolBox(app, idList.getId(id));
	}

	public static VclButton button(String id) {
		return new VclButton(app, idList.getId(id));
	}

	public static VclMessageBox msgbox(String text) {
		return new VclMessageBox(app, idList.getId("UID_ACTIVE"), text);
	}

	public static VclMenuItem menuItem(String text) {
		return new VclMenuItem(app, text);
	}

	public static VclStatusBar statusbar(String id) {
		return new VclStatusBar(app, idList.getId(id));
	}

	public static VclField field(String id) {
		return new VclField(app, idList.getId(id));
	}

	public static VclDockingWin dockingwin(String id) {
		return new VclDockingWin(app, idList.getId(id));
	}

	public static VclTreeListBox tree(String id) {
		return new VclTreeListBox(app, idList.getId(id));
	}

	public static final VclMessageBox activeMsgBox = new VclMessageBox(app, Constant.UID_ACTIVE);
	public static final VclMessageBox msgBox_AdditionalRowsNotSaved = new VclMessageBox(app, Constant.UID_ACTIVE, "Additional rows were not saved.");
	public static final VclTabControl activeTabControl = new VclTabControl(app, Constant.UID_ACTIVE);
	public static final VclWindow writer = window("SW_HID_EDIT_WIN");
	public static final VclWindow startcenter = window("FWK_HID_BACKINGWINDOW");
	public static final VclWindow calc = window("SC_HID_SC_WIN_GRIDWIN");
	public static final VclWindow impress = window("SD_HID_SDDRAWVIEWSHELL");
	public static final VclWindow draw = window("SD_HID_SDGRAPHICVIEWSHELL");
	public static final VclDialog filePicker = dialog("SVT_HID_EXPLORERDLG_FILE");
	public static final VclListBox filePickerFileType = listbox("fpicker:ListBox:DLG_SVT_EXPLORERFILE:LB_EXPLORERFILE_FILETYPE");
	public static final VclButton filePickerOpen = button("fpicker:PushButton:DLG_SVT_EXPLORERFILE:BTN_EXPLORERFILE_OPEN");
	public static final VclComboBox filePickerPath = combobox("SVT_HID_FILEDLG_AUTOCOMPLETEBOX");
	public static final VclDialog fileSave = dialog("SVT_HID_FILESAVE_DIALOG");
	public static final VclListBox fileSaveFileType = listbox("SVT_HID_FILESAVE_FILETYPE");
	public static final VclButton fileSaveSave = button("SVT_HID_FILESAVE_DOSAVE");
	public static final VclComboBox fileSavePath = combobox("SVT_HID_FILESAVE_FILEURL");
	public static final VclButton scInputBarFunctionWizard = button("SC_HID_INSWIN_CALC");
	public static final VclButton scInputBarSum = button("SC_HID_INSWIN_SUMME");
	public static final VclButton scInputBarFunction = button("SC_HID_INSWIN_FUNC");
	public static final VclToolBox scInputBar = toolbox("SC_HID_SC_INPUTWIN");
	public static final VclComboBox scInputBarPosition = combobox("SC_HID_INSWIN_POS");
	public static final VclWindow scInputBarInput = window("SC_HID_INSWIN_INPUT");
	public static final VclTabPage effectsPage = tabpage("CUI_HID_SVXPAGE_CHAR_EFFECTS");
	public static final VclListBox effectsPageColor = listbox("cui:ListBox:RID_SVXPAGE_CHAR_EFFECTS:LB_FONTCOLOR");
	public static final VclDialog alienFormatDlg = dialog("SFX2_HID_WARNING_ALIENFORMAT");
	public static final VclTabPage sortPage = tabpage("SC_HID_SCPAGE_SORT_FIELDS");
	public static final VclListBox sortPageBy1 = listbox("sc:ListBox:RID_SCPAGE_SORT_FIELDS:LB_SORT1");
	public static final VclButton sortPageAscending1 = button("sc:RadioButton:RID_SCPAGE_SORT_FIELDS:BTN_UP1");
	public static final VclButton sortPageDescending1 = button("sc:RadioButton:RID_SCPAGE_SORT_FIELDS:BTN_DOWN1");
	public static final VclListBox sortPageBy2 = listbox("sc:ListBox:RID_SCPAGE_SORT_FIELDS:LB_SORT2");
	public static final VclButton sortPageAscending2 = button("sc:RadioButton:RID_SCPAGE_SORT_FIELDS:BTN_UP2");
	public static final VclButton sortPageDescending2 = button("sc:RadioButton:RID_SCPAGE_SORT_FIELDS:BTN_DOWN2");
	public static final VclListBox sortPageBy3 = listbox("sc:ListBox:RID_SCPAGE_SORT_FIELDS:LB_SORT3");
	public static final VclButton sortPageAscending3 = button("sc:RadioButton:RID_SCPAGE_SORT_FIELDS:BTN_UP3");
	public static final VclButton sortPageDescending3 = button("sc:RadioButton:RID_SCPAGE_SORT_FIELDS:BTN_DOWN3");
	public static final VclTabPage pdfGeneralPage = tabpage("HID_FILTER_PDF_OPTIONS");
	public static final VclButton startCenterCalcButton = button(".HelpId:StartCenter:CalcButton");
	public static final VclButton startCenterImpressButton = button(".HelpId:StartCenter:ImpressButton");
	public static final VclButton startCenterWriterButton = button(".HelpId:StartCenter:WriterButton");
	public static final VclButton startCenterOpenButton = button(".HelpId:StartCenter:OpenButton");
	public static final VclButton startCenterDrawButton = button(".HelpId:StartCenter:DrawButton");
	public static final VclButton startCenterDBButton = button(".HelpId:StartCenter:DBButton");
	public static final VclButton startCenterMathButton = button(".HelpId:StartCenter:MathButton");
	public static final VclButton startCenterTemplateButton = button(".HelpId:StartCenter:TemplateButton");
	public static final VclButton startCenterTemplateRepository = button(".HelpId:StartCenter:TemplateRepository");
	public static final VclButton startCenterExtensions = button(".HelpId:StartCenter:Extensions");
	public static final VclButton startCenterInfo = button(".HelpId:StartCenter:Info");
	public static final VclWindow slideShow = window("SD_HID_SD_WIN_PRESENTATION");
	public static final VclDialog findDlg = dialog(".uno:SearchDialog");
	public static final VclComboBox findDlgFor = combobox("svx:ComboBox:RID_SVXDLG_SEARCH:ED_SEARCH");
	public static final VclComboBox findDlgReplaceWith = combobox("svx:ComboBox:RID_SVXDLG_SEARCH:ED_REPLACE");
	public static final VclButton findDlgFind = button("svx:PushButton:RID_SVXDLG_SEARCH:BTN_SEARCH");
	public static final VclButton findDlgFindAll = button("svx:PushButton:RID_SVXDLG_SEARCH:BTN_SEARCH_ALL");
	public static final VclButton findDlgReplace = button("svx:PushButton:RID_SVXDLG_SEARCH:BTN_REPLACE");
	public static final VclButton findDlgReplaceAll = button("svx:PushButton:RID_SVXDLG_SEARCH:BTN_REPLACE_ALL");
	public static final VclButton findDlgMatchCase = button("svx:CheckBox:RID_SVXDLG_SEARCH:CB_MATCH_CASE");
	public static final VclButton findDlgWholeWord = button("svx:CheckBox:RID_SVXDLG_SEARCH:CB_WHOLE_WORDS");
	public static final VclButton findDlgMore = button("svx:MoreButton:RID_SVXDLG_SEARCH:BTN_MORE");
	public static final VclWindow chart = window("CHART2_HID_SCH_WIN_DOCUMENT");
	public static final VclDialog chartWizard = dialog("CHART2_HID_SCH_CHART_AUTO_FORMAT");
	public static final VclDialog filePrintDlg = dialog(".HelpID:vcl:PrintDialog:Dialog");
	public static final VclWindow impressOutline = window("SD_HID_SDOUTLINEVIEWSHELL");
	public static final VclWindow impressSlideSorter = window("SD_HID_SD_WIN_DOCUMENT");
	public static final VclWindow impressHandout = window(".uno:HandoutMode");
	public static final VclDialog insertTable = dialog("cui:ModalDialog:RID_SVX_NEWTABLE_DLG");
	public static final VclToolBox tableToolbar = toolbox(".HelpId:tableobjectbar");
	public static final VclStatusBar statusBar = statusbar("FWK_HID_STATUSBAR");
	public static final VclDialog writerInsertTable = dialog(".uno:InsertTable");
	public static final VclWindow mathPreviewWindow = window("STARMATH_HID_SMA_WIN_DOCUMENT");
	public static final VclWindow mathEditWindow = window("STARMATH_HID_SMA_COMMAND_WIN_EDIT");
	public static final VclControl mathElementsWindow = control("STARMATH_HID_SMA_OPERATOR_WIN");
	public static final VclEditBox scFunctionWizardDlgMultiLineEdit = editbox("FORMULA_HID_FORMULA_FAP_FORMULA");
	public static final VclDialog scFunctionWizardDlg = dialog("FORMULA_HID_FORMULADLG_FORMULA");
	public static final VclListBox scFunctionWizardDlgFunctionList = listbox("FORMULA_HID_FORMULA_LB_FUNCTION");
	public static final VclEditBox scFunctionWizardDlgEdit1 = editbox("FORMULA_HID_FORMULA_FAP_EDIT1");
	public static final VclButton mathCatalogButton = button(".uno:SymbolCatalogue");
	public static final VclDialog mathSymbolsDlg = dialog("starmath:ModalDialog:RID_SYMBOLDIALOG");
	public static final VclListBox mathSymbolsDlgListbox = listbox("starmath:ListBox:RID_SYMBOLDIALOG:1");
	public static final VclButton mathSymbolsDlgEditButton = button("starmath:PushButton:RID_SYMBOLDIALOG:1");
	public static final VclButton mathSymbolsDlgInsertButton = button("starmath:PushButton:RID_SYMBOLDIALOG:2");
	public static final VclButton mathSymbolsDlgCloseButton = button("starmath:PushButton:RID_SYMBOLDIALOG:3");
	public static final VclDialog mathEditSymbolsDlg = dialog("starmath:ModalDialog:RID_SYMDEFINEDIALOG");
	public static final VclButton mathEditSymbolsDlgAdd = button("starmath:PushButton:RID_SYMDEFINEDIALOG:1");
	public static final VclButton mathEditSymbolsDlgModify = button("starmath:PushButton:RID_SYMDEFINEDIALOG:2");
	public static final VclButton mathEditSymbolsDlgDelete = button("starmath:PushButton:RID_SYMDEFINEDIALOG:3");
	public static final VclControl mathEditSymbolsDlgViewControl = control("STARMATH_HID_SMA_CONTROL_FONTCHAR_VIEW");
	public static final VclComboBox mathEditSymbolsDlgSymbol = combobox("starmath:ComboBox:RID_SYMDEFINEDIALOG:4");
	public static final VclListBox mathEditSymbolsDlgFont = listbox("starmath:ListBox:RID_SYMDEFINEDIALOG:1");
	public static final VclComboBox mathEditSymbolsDlgTypeface = combobox("starmath:ComboBox:RID_SYMDEFINEDIALOG:3");
	public static final VclDialog wizardsAgendaDialog = dialog("WIZARDS_HID_AGWIZ");
	public static final VclButton wizardsAgendaDialogFinishButton = button("WIZARDS_HID_AGWIZ_CREATE");
	public static final VclTabPage macroDialogsPage = tabpage("basctl:TabPage:RID_TP_DLGS");
	public static final VclDialog aboutDialog = dialog(".uno:About");
	public static final VclButton mathElementsRelations = button("STARMATH_HID_SMA_RELATIONS_CAT");
	public static final VclButton mathElementsRelationsNotEqual = button("STARMATH_HID_SMA_XNEQY");
	public static final VclButton mathElementsUnaryBinary = button("STARMATH_HID_SMA_UNBINOPS_CAT");
	public static final VclButton mathElementsUnaryBinaryPlus = button("STARMATH_HID_SMA_PLUSX");
	public static final VclControl macroWarningOK = control("Ok");
	public static final VclTreeListBox optionsDlgList = tree("CUI_HID_OFADLG_TREELISTBOX");
	// public static final VclListBox OptionsDlgList =
	// listbox("CUI_HID_OFADLG_TREELISTBOX");
	public static final VclDialog optionsDlg = dialog("CUI_HID_OFADLG_OPTIONS_TREE");
	public static final VclButton optionsDlgMacroSecurityButton = button("cui:PushButton:RID_SVXPAGE_INET_SECURITY:PB_SEC_MACROSEC");
	public static final VclControl macroSecurityDlg = control("XMLSECURITY_HID_XMLSEC_TP_SECLEVEL");
	public static final VclButton macroSecurityDlgLow = button("xmlsecurity:RadioButton:RID_XMLSECTP_SECLEVEL:RB_LOW");
	public static final VclWindow basicEditWindow = window("BASCTL_HID_BASICIDE_EDITORWINDOW");
	public static final VclDialog writerConvertTableToTextDlg = dialog(".uno:ConvertTableText");
	public static final VclWindow writerNavigator = window("SW_HID_NAVIGATION_PI");
	public static final VclListBox writerNavigatorList = listbox("SW_HID_NAVIGATOR_TREELIST");
	// public static final VclControl writer_FrameDlg =
	// control("SW_HID_FRM_STD");
	public static final VclDialog writerFrameDlg = dialog("SW_HID_FRM_STD");
	public static final VclField scNumberOfColumnsRowsToInsert = field("1502452023");
	public static final VclDialog scInsertColumnsRowsdialog = dialog("26166");
	public static final VclButton scInsertColumnsRowsAfterSelectColumn = button("1502446395");
	public static final VclButton wizardNextButton = button("SVT_HID_WIZARD_NEXT");
	public static final VclTabPage chartRangeChooseTabPage = tabpage("chart2:TabPage:TP_RANGECHOOSER");
	public static final VclButton wizardBackButton = button("SVT_HID_WIZARD_PREVIOUS");
	public static final VclTabPage chartTypeChooseTabPage = tabpage("CHART2_HID_DIAGRAM_TYPE");
	public static final VclControl chartWizardRoadMap = control("CHART2_HID_SCH_WIZARD_ROADMAP");
	public static final VclButton dataPilotTableToExistPlaceRadioButton = button("2569093658");
	public static final VclEditBox dataPilotTableToExistPlaceEditBox = editbox("2569095195");
	public static final VclDialog createDataPilotTableDialog = dialog("26151");
	public static final VclControl dataPilotFieldSelect = control("59030");
	public static final VclButton dataPilotAutomaticallyUpdateCheckBox = button("3643262003");
	public static final VclButton dataPilotTableUpdateButton = button("3643265586");
	public static final VclButton sdDateAndTimeFooterOnSlide = button("sd:CheckBox:RID_SD_TABPAGE_HEADERFOOTER:CB_DATETIME");
	public static final VclButton sdFixedDateAndTimeFooterOnSlide = button("sd:RadioButton:RID_SD_TABPAGE_HEADERFOOTER:RB_DATETIME_FIXED");
	public static final VclEditBox sdFixedDateAndTimeOnSlideInput = editbox("sd:Edit:RID_SD_TABPAGE_HEADERFOOTER:TB_DATETIME_FIXED");
	public static final VclButton sdFooterTextOnSlide = button("sd:CheckBox:RID_SD_TABPAGE_HEADERFOOTER:CB_FOOTER");
	public static final VclEditBox sdFooterTextOnSlideInput = editbox("sd:Edit:RID_SD_TABPAGE_HEADERFOOTER:TB_FOOTER_FIXED");
	public static final VclButton sdSlideNumAsFooterOnSlide = button("sd:CheckBox:RID_SD_TABPAGE_HEADERFOOTER:CB_SLIDENUMBER");
	public static final VclButton sdApplyToAllButtonOnSlideFooter = button("sd:PushButton:RID_SD_TABPAGE_HEADERFOOTER:BT_APPLYTOALL");
	public static final VclControl sdSlidelistInNaviagtor = control("HID_SD_NAVIGATOR_TLB");
	public static final VclControl sdHeaderAndFooterDialog = control("HID_SD_TABPAGE_HEADERFOOTER_SLIDE");
	public static final VclButton sdApplyButtonOnSlideFooter = button("sd:PushButton:RID_SD_TABPAGE_HEADERFOOTER:BT_APPLY");
	public static final VclButton sdInsertPageButtonOnToolbar = button(".uno:InsertPage");
	public static final VclControl dataPilotPaneRowArea = control("59036");
	public static final VclEditBox dataPilotFieldDisplayNameEditBox = editbox("971868166");
	public static final VclButton dataPilotFieldShowItemWithoutDataCheckBox = button("971867238");
	public static final VclTabPage dataPilotFieldOptionFieldTabPageDialog = tabpage("26550");
	public static final VclTabPage dataPilotFieldOptionSubTotalsTabPageDialog = tabpage("25760");
	public static final VclTabPage dataPilotFieldOptionSortTabPageDialog = tabpage("25763");
	public static final VclButton dataPilotFieldOptionSubTotalsManuallyRadioButton = button("958956137");
	public static final VclButton dataPilotFieldOptionSortDescendingRadioButton = button("958972620");
	public static final VclControl dataPiloPaneDataArea = control("59038");
	public static final VclControl dataPilotColumnArea = control("59034");
	public static final VclButton sdHeaderTextOnNotes = button("sd:CheckBox:RID_SD_TABPAGE_HEADERFOOTER:CB_HEADER");
	public static final VclEditBox sdHeaderTextOnNotesInput = editbox("sd:Edit:RID_SD_TABPAGE_HEADERFOOTER:TB_HEADER_FIXED");
	public static final VclTabPage sdHeaderAndFooterOnNotesTabPage = tabpage("SD_HID_SD_TABPAGE_HEADERFOOTER_NOTESHANDOUT");
	public static final VclButton sdFooterNotShowOn1stSlide = button("sd:CheckBox:RID_SD_TABPAGE_HEADERFOOTER:CB_NOTONTITLE");
	public static final VclTabPage sortOptionsPage = tabpage("SC_HID_SCPAGE_SORT_OPTIONS");
	public static final VclButton sortOptionsPageCaseSensitive = button("sc:CheckBox:RID_SCPAGE_SORT_OPTIONS:BTN_CASESENSITIVE");
	public static final VclButton sortOptionsPageCopyResultTo = button("sc:CheckBox:RID_SCPAGE_SORT_OPTIONS:BTN_COPYRESULT");
	public static final VclEditBox sortOptionsPageCopyResultToEdit = editbox("sc:Edit:RID_SCPAGE_SORT_OPTIONS:ED_OUTAREA");
	public static final VclDialog defineNamesDlg = dialog(".uno:DefineName");
	public static final VclEditBox defineNamesDlgNameEdit = editbox("sc:Edit:RID_SCDLG_NAMES:ED_ASSIGN");
	public static final VclListBox sortOptionsPageCopyResultToCellRange = listbox("sc:ListBox:RID_SCPAGE_SORT_OPTIONS:LB_OUTAREA");
	public static final VclWindow spreadSheetCells = window("58818");
	public static final VclButton dataPilotFieldOptionSubTotalsNeverRadioButton = button("958956135");
	public static final VclButton dataPilotFieldOptionSortAscendingRadioButton = button("958972619");
	public static final VclControl dataPilotPanePageArea = control("59032");
	public static final VclButton onlyDisplayTopNItemCheckBox = button("1495745540");
	public static final VclField numberOfItemShowInTopNEditBox = field("1495750661");
	public static final VclDialog fieldTopNSettingDialog = dialog("59039");
	public static final VclButton sdAutoUpdateTimeFooter = button("sd:RadioButton:RID_SD_TABPAGE_HEADERFOOTER:RB_DATETIME_AUTOMATIC");
	public static final VclListBox sdAutoUpdateTimeFooterType = listbox("sd:ListBox:RID_SD_TABPAGE_HEADERFOOTER:CB_DATETIME_FORMAT");
	public static final VclButton dataPilotButton = button("59270");
	public static final VclDockingWin dataPilotPanel = dockingwin("3643260928");
	public static final VclListBox scSubTotalsGroupByListBox = listbox("SC_HID_SC_SUBT_GROUP");
	public static final VclTreeListBox scCalcSubTotalForColumns = tree("SC_HID_SC_SUBT_COLS");
	public static final VclTabPage scSubTotalsGroup1Dialog = tabpage("SC_HID_SCPAGE_SUBT_GROUP1");
	public static final VclButton scSubtotalsInsertPageBreakCheckBox = button("sc:CheckBox:RID_SCPAGE_SUBT_OPTIONS:BTN_PAGEBREAK");
	public static final VclButton scSubtotalsCaseSensitiveCheckBox = button("sc:CheckBox:RID_SCPAGE_SUBT_OPTIONS:BTN_CASE");
	public static final VclButton scSubtotalsPreSortToGroupCheckBox = button("sc:CheckBox:RID_SCPAGE_SUBT_OPTIONS:BTN_SORT");
	public static final VclButton scSubtotalSortAscendingRadioButton = button("sc:RadioButton:RID_SCPAGE_SUBT_OPTIONS:BTN_ASCENDING");
	public static final VclButton scSubtotalSortDescendingRadioButton = button("sc:RadioButton:RID_SCPAGE_SUBT_OPTIONS:BTN_DESCENDING");
	public static final VclButton scSubtotalsIncludeFormatsCheckBox = button("sc:CheckBox:RID_SCPAGE_SUBT_OPTIONS:BTN_FORMATS");
	public static final VclButton scSubtotalsCustomSortOrderCheckBox = button("sc:CheckBox:RID_SCPAGE_SUBT_OPTIONS:BTN_USERDEF");
	public static final VclListBox scSubtotalsCustomSortListBox = listbox("sc:ListBox:RID_SCPAGE_SUBT_OPTIONS:LB_USERDEF");
	public static final VclListBox scValidityCriteriaAllowList = listbox("sc:ListBox:TP_VALIDATION_VALUES:LB_ALLOW");
	public static final VclTabPage scValidityCriteriaTabpage = tabpage("sc:TabPage:TP_VALIDATION_VALUES");
	public static final VclButton scValidityAllowBlankCells = button("sc:CheckBox:TP_VALIDATION_VALUES:TSB_ALLOW_BLANKS");
	public static final VclButton scValidityShowSelectionList = button("sc:CheckBox:TP_VALIDATION_VALUES:CB_SHOWLIST");
	public static final VclButton scValiditySortEntriesAscending = button("sc:CheckBox:TP_VALIDATION_VALUES:CB_SORTLIST");
	public static final VclButton scValiditySourcePicker = button("sc:ImageButton:TP_VALIDATION_VALUES:RB_VALIDITY_REF");
	public static final VclTabPage scValidityErrorAlertTabPage = tabpage("sc:TabPage:TP_VALIDATION_ERROR");
	public static final VclButton scValidityShowErrorMessage = button("sc:TriStateBox:TP_VALIDATION_ERROR:TSB_SHOW");
	public static final VclListBox scValidityErrorAlertActionList = listbox("sc:ListBox:TP_VALIDATION_ERROR:LB_ACTION");
	public static final VclEditBox scValidityErrorMessageTitle = editbox("sc:Edit:TP_VALIDATION_ERROR:EDT_TITLE");
	public static final VclEditBox scValidityErrorMessage = editbox("sc:MultiLineEdit:TP_VALIDATION_ERROR:EDT_ERROR");
	public static final VclButton scValidityErrorBrowseButton = button("sc:PushButton:TP_VALIDATION_ERROR:BTN_SEARCH");
	public static final VclTabPage scSubTotalsOptionsTabPage = tabpage("SC_HID_SCPAGE_SUBT_OPTIONS");
	public static final VclButton scDeleteCellsDeleteRowsRadioButton = button("sc:RadioButton:RID_SCDLG_DELCELL:BTN_DELROWS");
	public static final VclDialog scDeleteCellsDialog = dialog(".uno:DeleteCell");
	public static final VclTabPage scSubTotalsGroup2Dialog = tabpage("SC_HID_SCPAGE_SUBT_GROUP2");
	public static final VclTabPage scSubTotalsGroup3Dialog = tabpage("SC_HID_SCPAGE_SUBT_GROUP3");
	public static final VclListBox scCalcSubTotolsFuncionList = listbox("SC_HID_SC_SUBT_FUNC");
	public static final VclEditBox scValiditySourceInput = editbox("sc:Edit:TP_VALIDATION_VALUES:EDT_MIN");
	public static final VclListBox scValidityDecimalCompareOperator = listbox("sc:ListBox:TP_VALIDATION_VALUES:LB_VALUE");
	public static final VclEditBox scValidityMaxValueInput = editbox("sc:Edit:TP_VALIDATION_VALUES:EDT_MAX");
	public static final VclEditBox scValidityEntries = editbox("sc:MultiLineEdit:TP_VALIDATION_VALUES:EDT_LIST");
	public static final VclTabPage scValidityInputHelpTabPage = tabpage("sc:TabPage:TP_VALIDATION_INPUTHELP");
	public static final VclButton scValidityInputHelpCheckbox = button("sc:TriStateBox:TP_VALIDATION_INPUTHELP:TSB_HELP");
	public static final VclEditBox scValidityInputHelpTitle = editbox("sc:Edit:TP_VALIDATION_INPUTHELP:EDT_TITLE");
	public static final VclEditBox scValidityHelpMessage = editbox("sc:MultiLineEdit:TP_VALIDATION_INPUTHELP:EDT_INPUTHELP");
	public static final VclButton optionsDlgSortListsTabCopy = button("sc:PushButton:RID_SCPAGE_USERLISTS:BTN_COPY");
	public static final VclButton sortOptionsPageRangeContainsColumnLabels = button("sc:CheckBox:RID_SCPAGE_SORT_OPTIONS:BTN_LABEL");
	public static final VclButton sortOptionsPageCustomSortOrder = button("sc:CheckBox:RID_SCPAGE_SORT_OPTIONS:BTN_SORT_USER");
	public static final VclButton sortOptionsPageIncludeFormats = button("sc:CheckBox:RID_SCPAGE_SORT_OPTIONS:BTN_FORMATS");
	public static final VclListBox sortOptionsPageCustomSortOrderList = listbox("sc:ListBox:RID_SCPAGE_SORT_OPTIONS:LB_SORT_USER");
	public static final VclButton sortOptionsPageTopToBottom = button("sc:RadioButton:RID_SCPAGE_SORT_OPTIONS:BTN_TOP_DOWN");
	public static final VclButton sortOptionsPageLeftToRight = button("sc:RadioButton:RID_SCPAGE_SORT_OPTIONS:BTN_LEFT_RIGHT");
	public static final VclButton optionsDlgSortListsTabNew = button("sc:PushButton:RID_SCPAGE_USERLISTS:BTN_NEW");
	public static final VclButton optionsDlgSortListsTabAdd = button("sc:PushButton:RID_SCPAGE_USERLISTS:BTN_ADD");
	public static final VclTabPage formatCellsDlgNumbersPage = tabpage("CUI_HID_NUMBERFORMAT");
	public static final VclListBox formatCellsDlgNumbersPageCategory = listbox("cui:ListBox:RID_SVXPAGE_NUMBERFORMAT:LB_CATEGORY");
	public static final VclListBox formatCellsDlgNumbersPageCurrencyFormat = listbox("cui:ListBox:RID_SVXPAGE_NUMBERFORMAT:LB_CURRENCY");
	public static final VclTabPage dataPilotFieldOptionFiledSubtotalsPage = tabpage("25762");
	public static final VclControl scDataPilotOptionSubtotalFunctionList = control("59029");
	public static final VclDialog presentationWizard = dialog("SD_HID_SD_AUTOPILOT_PAGE1");
	public static final VclDialog fillSeriesDlg = dialog(".uno:FillSeries");
	public static final VclDialog sortWarningDlg = dialog("sc:ModalDialog:RID_SCDLG_SORT_WARNING");
	public static final VclButton sortWarningDlgExtend = button("sc:PushButton:RID_SCDLG_SORT_WARNING:BTN_EXTSORT");
	public static final VclButton sortWarningDlgCurrent = button("sc:PushButton:RID_SCDLG_SORT_WARNING:BTN_CURSORT");
	// Spreadsheet: "Sheet" dialog
	public static final VclDialog scInsertSheetDlg = dialog("sc:ModalDialog:RID_SCDLG_INSERT_TABLE");
	public static final VclButton scBeforeCurrentSheet = button("sc:RadioButton:RID_SCDLG_INSERT_TABLE:RB_BEFORE");
	public static final VclButton scAfterCurrentSheet = button("sc:RadioButton:RID_SCDLG_INSERT_TABLE:RB_BEHIND");
	public static final VclButton scNewSheet = button("sc:RadioButton:RID_SCDLG_INSERT_TABLE:RB_NEW");
	public static final VclButton scNewSheetFromFile = button("sc:RadioButton:RID_SCDLG_INSERT_TABLE:RB_FROMFILE");
	public static final VclEditBox scNewSheetName = editbox("sc:Edit:RID_SCDLG_INSERT_TABLE:ED_TABNAME");
	public static final VclField scSheetNumber = field("sc:NumericField:RID_SCDLG_INSERT_TABLE:NF_COUNT");
	public static final VclListBox scFromFileSheetList = listbox("sc:MultiListBox:RID_SCDLG_INSERT_TABLE:LB_TABLES");
	public static final VclButton scFromFileBrowse = button("sc:PushButton:RID_SCDLG_INSERT_TABLE:BTN_BROWSE");
	public static final VclButton scFromFileLink = button("sc:CheckBox:RID_SCDLG_INSERT_TABLE:CB_LINK");
	// Spreadsheet: "Select Sheets" dialog
	public static final VclDialog scSelectSheetsDlg = dialog(".uno:SelectTables");
	public static final VclListBox scSheetsList = listbox("SC_HID_SELECTTABLES");
	public static final VclWindow sdNavigatorDlg = window("SD_HID_SD_NAVIGATOR");
	public static final VclTabPage sdHeaderAndFooterDlgSlideTab = tabpage("SD_HID_SD_TABPAGE_HEADERFOOTER_SLIDE");
	public static final VclButton scFunctionWizardDlgNext = button("formula:PushButton:RID_FORMULADLG_FORMULA:BTN_FORWARD");
	public static final VclTreeListBox optionsDlgPathTabTree = tree("CUI_HID_OPTPATH_CTL_PATH");
	public static final VclTreeListBox optionsDlgMicorOfficeTree = tree("CUI_HID_OFAPAGE_MSFLTR2_CLB");
	public static final VclTabPage bulletPage = tabpage("CUI_HID_SVXPAGE_PICK_BULLET");
	// Writer: "Spell check" dialog
	public static final VclDialog spellcheckDlg = dialog("CUI_HID_SPELLCHECK");
	public static final VclListBox spellcheckDlgLanguageList = listbox("cui:ListBox:RID_SVXDLG_SPELLCHECK:LB_LANGUAGE");
	public static final VclEditBox spellcheckDlgMisspelledWords = editbox("cui:MultiLineEdit:RID_SVXDLG_SPELLCHECK:ED_NEWWORD");
	public static final VclListBox spellcheckDlgSuggestionList = listbox("cui:ListBox:RID_SVXDLG_SPELLCHECK:LB_SUGGESTION");
	public static final VclButton spellcheckDlgIgnoreOnce = button("CUI_HID_SPLDLG_BUTTON_IGNORE");
	public static final VclButton spellcheckDlgIgnoreAll = button("CUI_HID_SPLDLG_BUTTON_IGNOREALL");
	public static final VclButton spellcheckDlgAdd = button("cui:MenuButton:RID_SVXDLG_SPELLCHECK:MB_ADDTODICT");
	public static final VclButton spellcheckDlgChange = button("CUI_HID_SPLDLG_BUTTON_CHANGE");
	public static final VclButton spellcheckDlgChangeAll = button("CUI_HID_SPLDLG_BUTTON_CHANGEALL");
	public static final VclButton spellcheckDlgAutoCorrect = button("cui:PushButton:RID_SVXDLG_SPELLCHECK:PB_AUTOCORR");
	public static final VclButton spellcheckDlgClose = button("CUI_HID_SPLDLG_BUTTON_CLOSE");

	// Presentation:"Table Property" dialog
	public static final VclButton impressWizardDlgNext = button("sd:PushButton:DLG_ASS:BUT_NEXT");
	public static final VclListBox areaFillColor = listbox("cui:ListBox:RID_SVXPAGE_AREA:LB_COLOR");
	public static final VclTabPage areaAreapage = tabpage("CUI_HID_AREA_AREA");
	public static final VclListBox areaFillType = listbox("cui:ListBox:RID_SVXPAGE_AREA:LB_AREA_TYPE");
	public static final VclListBox areaFillGradient = listbox("cui:ListBox:RID_SVXPAGE_AREA:LB_GRADIENT");
	public static final VclListBox areaFillHatching = listbox("cui:ListBox:RID_SVXPAGE_AREA:LB_HATCHING");
	public static final VclListBox areaFillBitmap = listbox("cui:ListBox:RID_SVXPAGE_AREA:LB_BITMAP");
	public static final VclTabPage areaBorderpage = tabpage("CUI_HID_BORDER");
	public static final VclTabPage areaFontpage = tabpage("CUI_HID_SVXPAGE_CHAR_NAME");
	public static final VclComboBox areaFontWesternFontName = combobox("cui:ComboBox:RID_SVXPAGE_CHAR_NAME:LB_WEST_NAME");
	public static final VclComboBox areaFontWesternStyle = combobox("cui:ComboBox:RID_SVXPAGE_CHAR_NAME:LB_WEST_STYLE");
	public static final VclListBox areaFontWesternSize = listbox("cui:MetricBox:RID_SVXPAGE_CHAR_NAME:LB_WEST_SIZE");
	public static final VclListBox areaFontWesternLang = listbox("cui:ListBox:RID_SVXPAGE_CHAR_NAME:LB_WEST_LANG");
	public static final VclComboBox areaFontEasternFontName = combobox("cui:ComboBox:RID_SVXPAGE_CHAR_NAME:LB_EAST_NAME");
	public static final VclComboBox areaFontEasternStyle = combobox("cui:ComboBox:RID_SVXPAGE_CHAR_NAME:LB_EAST_STYLE");
	public static final VclListBox areaFontEasternSize = listbox("cui:MetricBox:RID_SVXPAGE_CHAR_NAME:LB_EAST_SIZE");
	public static final VclListBox areaFontEasternLang = listbox("cui:ListBox:RID_SVXPAGE_CHAR_NAME:LB_EAST_LANG");
	public static final VclListBox effectsPageRelief = listbox("cui:ListBox:RID_SVXPAGE_CHAR_EFFECTS:LB_RELIEF");
	public static final VclButton effectsPageOutline = button("cui:TriStateBox:RID_SVXPAGE_CHAR_EFFECTS:CB_OUTLINE");
	public static final VclButton effectsPageShadow = button("cui:TriStateBox:RID_SVXPAGE_CHAR_EFFECTS:CB_SHADOW");
	public static final VclListBox effectsPageOverline = listbox("cui:ListBox:RID_SVXPAGE_CHAR_EFFECTS:LB_OVERLINE");
	public static final VclListBox effectsPageOverlineColor = listbox("cui:ListBox:RID_SVXPAGE_CHAR_EFFECTS:LB_OVERLINE_COLOR");
	public static final VclListBox effectsPageStrikeout = listbox("cui:ListBox:RID_SVXPAGE_CHAR_EFFECTS:LB_STRIKEOUT");
	public static final VclListBox effectsPageUnderline = listbox("cui:ListBox:RID_SVXPAGE_CHAR_EFFECTS:LB_UNDERLINE");
	public static final VclListBox effectsPageUnderlineColor = listbox("cui:ListBox:RID_SVXPAGE_CHAR_EFFECTS:LB_UNDERLINE_COLOR");
	public static final VclButton effectsPageInvidiualWords = button("cui:CheckBox:RID_SVXPAGE_CHAR_EFFECTS:CB_INDIVIDUALWORDS");
	public static final VclListBox effectsPageEmphasis = listbox("cui:ListBox:RID_SVXPAGE_CHAR_EFFECTS:LB_EMPHASIS");
	public static final VclListBox effectsPagePosition = listbox("cui:ListBox:RID_SVXPAGE_CHAR_EFFECTS:LB_POSITION");
	public static final VclControl areaBorderPresets = control("CUI_HID_BORDER_CTL_PRESETS");
	public static final VclListBox areaBorderLineStyle = listbox("cui:ListBox:RID_SVXPAGE_BORDER:LB_LINESTYLE");
	public static final VclListBox areaBorderLineColor = listbox("cui:ListBox:RID_SVXPAGE_BORDER:LB_LINECOLOR");
	public static final VclField areaBorderMfLeft = field("cui:MetricField:RID_SVXPAGE_BORDER:MF_LEFT");
	public static final VclField areaBorderMfRight = field("cui:MetricField:RID_SVXPAGE_BORDER:MF_RIGHT");
	public static final VclField areaBorderMfTop = field("cui:MetricField:RID_SVXPAGE_BORDER:MF_TOP");
	public static final VclField areaBorderMfBottom = field("cui:MetricField:RID_SVXPAGE_BORDER:MF_BOTTOM");
	public static final VclButton areaBorderSync = button("cui:CheckBox:RID_SVXPAGE_BORDER:CB_SYNC");
	// Presentation:"Textbox Property" dialog
	public static final VclToolBox textboxToolbar = toolbox(".HelpId:textbar");
	public static final VclButton sdInsertTextboxButtonOnToolbar = button(".uno:Text");
	public static final VclButton sdFormatArea = button(".uno:FormatArea");

	// Writer:Style and Formatting List
	public static final VclControl styleAndFormattingList = control("SFX2_HID_TEMPLATE_FMT");
	public static final VclListBox styleFilterList = listbox("SFX2_HID_TEMPLATE_FILTER");
	public static final VclDockingWin styleAndFormattingDlg = dockingwin(".uno:DesignerDialog");

	// Writer:Insert Index and Table Dialog
	public static final VclDialog insertIndexDlg = dialog("SW_HID_TP_TOX_SELECT");

	// Writer:Split Cell Dialog
	public static final VclDialog writerSplitCellDlg = dialog("cui:ModalDialog:RID_SVX_SPLITCELLDLG");

	// Writer:Insert Rows/Columns Dialog
	public static final VclDialog writerInsertRowsDlg = dialog(".uno:InsertRowDialog");
	public static final VclField writerInsertNum = field("cui:NumericField:DLG_INS_ROW_COL:ED_COUNT");
	public static final VclButton writerInsertBefore = button("cui:RadioButton:DLG_INS_ROW_COL:CB_POS_BEFORE");
	public static final VclButton writerInsertAfter = button("cui:RadioButton:DLG_INS_ROW_COL:CB_POS_AFTER");
	public static final VclDialog writerInsertColumnsDlg = dialog(".uno:InsertColumnDialog");

	// Writer:Alignment TabPage of Paragraph format
	public static final VclTabPage alignPage = tabpage("CUI_HID_FORMAT_PARAGRAPH_ALIGN");
	public static final VclButton alignPageLeft = button("cui:RadioButton:RID_SVXPAGE_ALIGN_PARAGRAPH:BTN_LEFTALIGN");
	public static final VclButton alignPageRight = button("cui:RadioButton:RID_SVXPAGE_ALIGN_PARAGRAPH:BTN_RIGHTALIGN");
	public static final VclButton alignPageCenter = button("cui:RadioButton:RID_SVXPAGE_ALIGN_PARAGRAPH:BTN_CENTERALIGN");
	public static final VclButton alignPageJustified = button("cui:RadioButton:RID_SVXPAGE_ALIGN_PARAGRAPH:BTN_JUSTIFYALIGN");
	public static final VclListBox alignPageLastline = listbox("cui:ListBox:RID_SVXPAGE_ALIGN_PARAGRAPH:LB_LASTLINE");
	public static final VclButton alignPageExpandSingleWord = button("cui:CheckBox:RID_SVXPAGE_ALIGN_PARAGRAPH:CB_EXPAND");
	public static final VclButton alignPageSnapToTextGrid = button("cui:CheckBox:RID_SVXPAGE_ALIGN_PARAGRAPH:CB_SNAP");
	public static final VclListBox alignPageVerticalAlign = listbox("cui:ListBox:RID_SVXPAGE_ALIGN_PARAGRAPH:LB_VERTALIGN");

	// Writer: Insert Object Dialog
	public static final VclDialog insertObject = dialog(".uno:InsertObject");
	public static final VclButton newObject = button("cui:RadioButton:MD_INSERT_OLEOBJECT:RB_NEW_OBJECT");
	public static final VclButton objectFromFile = button("cui:RadioButton:MD_INSERT_OLEOBJECT:RB_OBJECT_FROMFILE");
	public static final VclListBox objectType = listbox("cui:ListBox:MD_INSERT_OLEOBJECT:LB_OBJECTTYPE");
	public static final VclEditBox objectFilePath = editbox("cui:Edit:MD_INSERT_OLEOBJECT:ED_FILEPATH");
	public static final VclButton objectFileSearch = button("cui:PushButton:MD_INSERT_OLEOBJECT:BTN_FILEPATH");
	public static final VclButton objectFileLink = button("cui:CheckBox:MD_INSERT_OLEOBJECT:CB_FILELINK");
	public static final VclField numberofCol = field("cui:NumericField:RID_SVX_NEWTABLE_DLG:NF_COLUMNS");

	// Presentation: Insert Table
	public static final VclField numberofRow = field("cui:NumericField:RID_SVX_NEWTABLE_DLG:NF_ROWS");
	public static final VclDialog insertSlideObjects = dialog("SD_HID_DLG_INSERT_PAGES_OBJS");

	// Presentation: Slide Transition Settings
	public static final VclListBox impressSlideTransitions = listbox("SD_HID_SD_SLIDETRANSITIONPANE_LB_SLIDE_TRANSITIONS");
	public static final VclListBox slideShowSpeed = listbox("SD_HID_SD_SLIDETRANSITIONPANE_LB_SPEED");
	public static final VclButton slideAutoAfter = button("SD_HID_SD_SLIDETRANSITIONPANE_RB_ADVANCE_AUTO");
	public static final VclButton applyToAllSlides = button("SD_HID_SD_SLIDETRANSITIONPANE_PB_APPLY_TO_ALL");

	// Spreadsheet: Standard Filter Dialog
	public static final VclComboBox filterValue1 = combobox("sc:ComboBox:RID_SCDLG_FILTER:ED_VAL1");
	public static final VclDialog standardFilterDlg = dialog(".uno:DataFilterStandardFilter");

	// Fontwork
	public static final VclDialog fontworkGalleryDlg = dialog("svx:ModalDialog:RID_SVX_MDLG_FONTWORK_GALLERY");
	
	public static final VclEditBox swTableSizeColBox=editbox("sw:NumericField:DLG_INSERT_TABLE:ED_COL");
	public static final VclEditBox swTableSizeRowBox=editbox("sw:NumericField:DLG_INSERT_TABLE:ED_ROW");
	public static final VclControl swTableBackgroundColor=control("CUI_HID_BACKGROUND_CTL_BGDCOLORSET");
	public static final VclTabPage swTableBackground = tabpage("CUI_HID_BACKGROUND");
	public static final VclTabPage swTableBorder = tabpage("CUI_HID_BORDER");
	public static final VclControl swTableBorderLineArrange = control("CUI_HID_BORDER_CTL_PRESETS");
	public static final VclControl swTableProperties = control("SW_HID_FORMAT_TABLE");
	public static final VclListBox swTableBorderLineStyle = listbox("cui:ListBox:RID_SVXPAGE_BORDER:LB_LINESTYLE");
	public static final VclListBox swTableBorderLineColor = listbox("cui:ListBox:RID_SVXPAGE_BORDER:LB_LINECOLOR");
	public static final VclEditBox swTableSTCLeft = editbox("cui:MetricField:RID_SVXPAGE_BORDER:MF_LEFT");
	public static final VclEditBox swTableSTCRight = editbox("cui:MetricField:RID_SVXPAGE_BORDER:MF_RIGHT");
	public static final VclEditBox swTableSTCTop = editbox("cui:MetricField:RID_SVXPAGE_BORDER:MF_TOP");
	public static final VclEditBox swTableSTCBottom = editbox("cui:MetricField:RID_SVXPAGE_BORDER:MF_BOTTOM");
	public static final VclButton swTableSTCSYNC = button("cui:CheckBox:RID_SVXPAGE_BORDER:CB_SYNC");
	public static final VclControl swTableShadow = control("CUI_HID_BORDER_CTL_SHADOWS");
	public static final VclEditBox swTableShadowSize = editbox("cui:MetricField:RID_SVXPAGE_BORDER:ED_SHADOWSIZE");
	public static final VclListBox swTableShadowColor = listbox("cui:ListBox:RID_SVXPAGE_BORDER:LB_SHADOWCOLOR");
	public static final VclButton swInsertTableAutoFormat = button("sw:PushButton:DLG_INSERT_TABLE:BT_AUTOFORMAT");
	public static final VclListBox swTableAutoFormatListbox = listbox("sw:ListBox:DLG_AUTOFMT_TABLE:LB_FORMAT");
	public static final VclDialog swTableAutoFMT = dialog("SW_HID_AUTOFMT_TABLE");
	public static final VclDialog swTableSetRowHeightDialog = dialog(".uno:SetRowHeight");
	public static final VclField swTableSetRowHeight = field("sw:MetricField:DLG_ROW_HEIGHT:ED_HEIGHT");
	public static final VclDialog swTableInsertRow = dialog(".uno:InsertRowDialog");
	public static final VclEditBox swTableInsertRowColumnSetNumber = editbox("cui:NumericField:DLG_INS_ROW_COL:ED_COUNT");
	public static final VclField swTableSetColumnNumber = field("sw:NumericField:DLG_COL_WIDTH:ED_COL");
	public static final VclField swTableSetColumnWidth = field("sw:MetricField:DLG_COL_WIDTH:ED_WIDTH");
	public static final VclDialog swTableInsertColumn = dialog(".uno:InsertColumnDialog");
	public static final VclDialog swTableSetColumnDialog = dialog(".uno:SetColumnWidth");
	public static final VclDialog swTableCellSplitDialog = dialog("cui:ModalDialog:RID_SVX_SPLITCELLDLG");
	public static final VclField swTableCellSplitNumber = field("cui:NumericField:RID_SVX_SPLITCELLDLG:ED_COUNT");
	public static final VclButton swTableCellSplitHORZButton = button("cui:ImageRadioButton:RID_SVX_SPLITCELLDLG:RB_HORZ");
	public static final VclButton swTableCellSplitVERTButton = button("cui:ImageRadioButton:RID_SVX_SPLITCELLDLG:RB_VERT");
	public static final VclMenuItem swTableSelectRowMenu=menuItem("Row->Select");
	public static final VclMenuItem swTableInsertRowMenu=menuItem("Row->Insert...");
	public static final VclMenuItem swTableRowHeightMenu=menuItem("Row->Height...");
	public static final VclMenuItem swTableRowDleteMenu=menuItem("Row->Delete");
	public static final VclMenuItem swTableColumnWidthMenu=menuItem("Column->Width...");
	public static final VclMenuItem swTableColumnSelectMenu=menuItem("Column->Select");
	public static final VclMenuItem swTableColumnDeleteMenu=menuItem("Column->Delete");
	public static final VclMenuItem swTableColumnInsertMenu=menuItem("Column->Insert...");
	public static final VclMenuItem swTableCellSplitMenu=menuItem("Cell->Split...");
	public static final VclListBox sdTableBACGColorListbox = listbox("cui:ListBox:RID_SVXPAGE_AREA:LB_COLOR");
	public static final VclTabPage sdTableBACGColorArea = tabpage("CUI_HID_AREA_AREA");
	public static final VclTabPage sdTableBACGColorsOption = tabpage("CUI_HID_OPTIONS_COLOR");
	public static final VclEditBox sdTableBACGColorsOptionName = editbox("cui:Edit:RID_SVXPAGE_COLOR:EDT_NAME");
	
	public static final VclTreeListBox sdNavigator = tree("SD_HID_SD_NAVIGATOR_TLB");
	public static final VclButton sdNavigatorShapeFilter = button("SD_HID_SD_NAVIGATOR_TBI_SHAPE_FILTER");
	public static final VclButton sdCalloutShapes = button(".uno:CalloutShapes");
	public static final VclButton sdStarShapes = button(".uno:StarShapes");
	public static final VclToolBox sdDrawingToolbar = toolbox(".HelpId:toolbar");
	
	public static final VclWindow impressHandoutView = window(".uno:NotesMode");
	public static final VclWindow helpWindow = window("SFX2_HID_HELP_ONHELP");
	public static final VclDialog printerSetUpDlg = dialog("svtools:ModalDialog:DLG_SVT_PRNDLG_PRNSETUPDLG");
	public static final VclListBox printerSetUpDlgPrinterNames = listbox("svtools:ListBox:DLG_SVT_PRNDLG_PRNSETUPDLG:LB_NAMES");
	public static final VclDialog runMacroDlg = dialog("CUI_HID_SELECTOR_DIALOG");
	public static final VclTreeListBox runMacroDlgCategories = tree("CUI_HID_SELECTOR_CATEGORIES");
	public static final VclTreeListBox runMacroDlgCommands = tree("CUI_HID_SELECTOR_COMMANDS");
	public static final VclButton optionUseOOdialog = button("cui:CheckBox:OFA_TP_MISC:CB_FILEDLG");
	
	public static final VclButton standardBarSave = button(".uno:Save");

	public static final VclControl swFormatParagragh = control("CUI_HID_FORMAT_PARAGRAPH_STD");

	public static final VclButton sdAddAnimation = button("SD_HID_SD_CUSTOMANIMATIONPANE_PB_ADD_EFFECT");

	public static final VclListBox sdEntranceAnimationTab = listbox("sd:ListBox:RID_TP_CUSTOMANIMATION_ENTRANCE:LB_EFFECTS");

	public static final VclControl sdAnimationList = control("SD_HID_SD_CUSTOMANIMATIONPANE_CT_CUSTOM_ANIMATION_LIST");

	public static final VclWindow sdCurrentMastersList = window("SD_HID_SD_CURRENT_MASTERS");

	public static final VclButton swInsertLineButtonOnToolbar = button(".uno:Line");

	public static final VclToolBox swDrawingToolbar = toolbox(".HelpId:drawbar");

	public static final VclListBox scPrintAreaType = listbox("sc:ListBox:RID_SCDLG_AREAS:LB_PRINTAREA");

	public static final VclEditBox scPrintArea = editbox("sc:Edit:RID_SCDLG_AREAS:ED_PRINTAREA");

	public static final VclEditBox scFormatCode = editbox("cui:Edit:RID_SVXPAGE_NUMBERFORMAT:ED_FORMAT");

	// hyperlink dialog
	public static final VclComboBox hyperlinkInetPathComboBox = combobox("CUI_HID_HYPERDLG_INET_PATH"); // target
	public static final VclEditBox hyperlinkInetName = editbox("cui:Edit:RID_SVXPAGE_HYPERLINK_INTERNET:ED_TEXT"); // text
	public static final VclEditBox hyperlinkInetText = editbox("cui:Edit:RID_SVXPAGE_HYPERLINK_INTERNET:ED_INDICATION"); // name
	public static final VclControl hyperlinkDialogOkBtn = control("CUI_HID_ICCDIALOG_OK_BTN"); // apply
	public static final VclControl hyperlinkDialogCancelBtn = control("CUI_HID_ICCDIALOG_CANCEL_BTN"); // close
}
