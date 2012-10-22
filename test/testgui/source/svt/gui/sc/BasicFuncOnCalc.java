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

/**
 * 
 */

package svt.gui.sc;

import static org.openoffice.test.common.Testspace.getFile;
import static org.openoffice.test.common.Testspace.getPath;
import static org.openoffice.test.common.Testspace.prepareData;
import static org.openoffice.test.vcl.Tester.sleep;
import static testlib.gui.AppTool.submitOpenDlg;
import static testlib.gui.AppTool.submitSaveDlg;
import static testlib.gui.AppTool.typeKeys;
import static testlib.gui.UIMap.activeMsgBox;
import static testlib.gui.UIMap.aoo;
import static testlib.gui.UIMap.app;
import static testlib.gui.UIMap.calc;
import static testlib.gui.UIMap.chartWizard;
import static testlib.gui.UIMap.filePickerOpen;
import static testlib.gui.UIMap.filePickerPath;
import static testlib.gui.UIMap.filterValue1;
import static testlib.gui.UIMap.fontworkGalleryDlg;
import static testlib.gui.UIMap.scAfterCurrentSheet;
import static testlib.gui.UIMap.scFormatCode;
import static testlib.gui.UIMap.scInsertSheetDlg;
import static testlib.gui.UIMap.scNewSheetName;
import static testlib.gui.UIMap.scPrintArea;
import static testlib.gui.UIMap.scPrintAreaType;
import static testlib.gui.UIMap.scValidityCriteriaAllowList;
import static testlib.gui.UIMap.scValidityCriteriaTabpage;
import static testlib.gui.UIMap.scValidityDecimalCompareOperator;
import static testlib.gui.UIMap.scValidityErrorAlertTabPage;
import static testlib.gui.UIMap.scValidityErrorMessage;
import static testlib.gui.UIMap.scValidityErrorMessageTitle;
import static testlib.gui.UIMap.scValidityShowErrorMessage;
import static testlib.gui.UIMap.scValiditySourceInput;
import static testlib.gui.UIMap.sortOptionsPage;
import static testlib.gui.UIMap.sortOptionsPageRangeContainsColumnLabels;
import static testlib.gui.UIMap.sortPage;
import static testlib.gui.UIMap.sortPageAscending1;
import static testlib.gui.UIMap.sortPageBy1;
import static testlib.gui.UIMap.standardFilterDlg;
import static testlib.gui.UIMap.startCenterOpenButton;

import java.util.HashMap;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TestName;
import org.openoffice.test.common.DataSheet;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Logger;

import testlib.gui.SCTool;

public class BasicFuncOnCalc {
	@Rule
	public Logger log = Logger.getLogger(this);

	@Rule
	public TestName testname = new TestName();

	private static DataSheet xmlResult;

	private static int iterator = 2;

	private int i = 0;

	/**
	 * @throws java.lang.Exception
	 */
	@BeforeClass
	public static void beforeClass() throws Exception {
		xmlResult = new DataSheet(getFile("output/svt.xml"));
		xmlResult.addRow("Data", "Method", "No", "Consumed Time(MS)", "Memory(VSZ)", "Memory(RSS)", "Handles(Windows Only)");
	}

	@AfterClass
	public static void afterClass() throws Exception {
		app.stop();
	}
	
	@Before 
	public void before()throws Exception{
		app.start(true);
	}
	

	@Test
	public void saveNewSCWithNumberFormat() {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSC();
			SCTool.selectRange("A1");
			typeKeys("0.3");
			sleep(1);
			app.dispatch(".uno:FormatCellDialog");
			sleep(2);
			scFormatCode.setText("0%");
			typeKeys("<enter>");
			sleep(1);
			saveAndReopenNewSC(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void saveNewSCWithPrintRange() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSC();
			app.dispatch(".uno:EditPrintArea");
			sleep(1);
			scPrintAreaType.select("- user defined -");
			scPrintArea.setText("$A$1:$C$4");
			typeKeys("<enter>");
			SCTool.selectRange("A1");
			typeKeys("test");
			saveAndReopenNewSC(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void saveNewSCWithText() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSC();
			SCTool.selectRange("A1");
			typeKeys("test");
			saveAndReopenNewSC(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void saveNewSCWithDataSort() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSC();
			SCTool.selectRange("A1");
			typeKeys("3<down>2<down>5<down>1<down>6<down>4<down>10<down>8<down>9<down>7");
			sleep(1);
			app.dispatch(".uno:DataSort");
			sortOptionsPage.select();
			sortOptionsPageRangeContainsColumnLabels.uncheck();
			sortPage.select();
			sortPageBy1.select(1); // "Column A"
			sortPageAscending1.check();
			sortPage.ok();
			saveAndReopenNewSC(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void saveNewSCWithInsertSheets() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSC();
			app.dispatch(".uno:Insert");
			scAfterCurrentSheet.check();
			scNewSheetName.setText("Instant Filter");
			scInsertSheetDlg.ok();
			sleep(5);
			saveAndReopenNewSC(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void saveNewSCWithStandardFilter() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSC();
			SCTool.selectRange("A1");
			typeKeys("A<down>1<down>2<down>3<down>1<down>2<down>3<down>1<down>2<down>3");
			sleep(1);
			SCTool.selectRange("A1");
			app.dispatch(".uno:DataFilterStandardFilter");
			sleep(2);
			filterValue1.setText("1");
			standardFilterDlg.ok();
			saveAndReopenNewSC(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void saveNewSCWithInsertPic() throws Exception {
		String pic = prepareData("image/blue_256x256.jpg");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSC();
			calc.menuItem("Insert->Picture->From File...").select();
			sleep(2);
			filePickerPath.setText(pic);
			sleep(1);
			filePickerOpen.click();
			sleep(2);
			typeKeys("<esc>");
			sleep(2);
			saveAndReopenNewSC(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void saveNewSCWithValidaty() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSC();
			app.dispatch(".uno:Insert");
			scAfterCurrentSheet.check();
			scNewSheetName.setText("Data Validate");
			scInsertSheetDlg.ok();
			sleep(5);
			SCTool.selectRange("B2:E5");
			sleep(2);

			app.dispatch(".uno:Validation");
			scValidityCriteriaTabpage.select();
			scValidityCriteriaAllowList.select("Whole Numbers");
			scValidityDecimalCompareOperator.select("greater than");
			scValiditySourceInput.setText("1");
			scValidityErrorAlertTabPage.select();
			scValidityShowErrorMessage.check();
			scValidityErrorMessageTitle.setText("Error");
			scValidityErrorMessage.setText("Must greater than 1");
			scValidityErrorAlertTabPage.ok();

			SCTool.selectRange("B2");
			typeKeys("0<enter>");
			activeMsgBox.ok();
			sleep(1);

			SCTool.selectRange("E5");
			typeKeys("1<enter>");
			activeMsgBox.ok();
			sleep(1);

			SCTool.selectRange("E2");
			typeKeys("2<enter>");
			sleep(5);
			saveAndReopenNewSC(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void insertChartToOpenedSC() throws Exception {
		String file = prepareData("pvt/plain_11s.ods");
		String[][] inputStr = { { "Area", "Item", "Count" }, { "1", "2", "3" },
				{ "4", "5", "6" }, { "7", "8", "9" }, { "10", "11", "12" } };
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			calc.waitForExistence(20, 2);
			sleep(2);

			// Insert Chart
			app.dispatch(".uno:Insert");
			scAfterCurrentSheet.check();
			scNewSheetName.setText("Instant Chart");
			scInsertSheetDlg.ok();
			sleep(5);
			inputCells(inputStr);
			sleep(2);
			SCTool.selectRange("A1:C5");
			calc.menuItem("Insert->Chart...").select();
			sleep(1);
			chartWizard.ok();
			sleep(5);
			calc.typeKeys("<esc>");
			sleep(5);
			calc.typeKeys("<esc>");
			sleep(5);
			calc.typeKeys("<esc>");
			sleep(5);
			closeWithoutSaveSC();
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void insertPicToOpenedSC() throws Exception {
		String file = prepareData("pvt/plain_11s.ods");
		String pic = prepareData("image/blue_256x256.jpg");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			calc.waitForExistence(20, 2);
			sleep(2);

			// Insert Chart
			app.dispatch(".uno:Insert");
			scAfterCurrentSheet.check();
			scNewSheetName.setText("Instant Graphic");
			scInsertSheetDlg.ok();
			sleep(5);
			calc.menuItem("Insert->Picture->From File...").select();
			sleep(2);
			filePickerPath.setText(pic);
			sleep(1);
			filePickerOpen.click();
			sleep(5);
			calc.typeKeys("<esc>");
			sleep(2);
			closeWithoutSaveSC();
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void insertFontWorkToOpenedSC() throws Exception {
		String file = prepareData("pvt/plain_11s.ods");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			calc.waitForExistence(20, 2);
			sleep(2);

			calc.menuItem("View->Toolbars->Fontwork").select();
			sleep(2);
			app.dispatch(".uno:FontworkGalleryFloater");
			sleep(2);
			typeKeys("<right>");
			fontworkGalleryDlg.ok();
			sleep(2);
			calc.typeKeys("<esc>");
			sleep(2);

			closeWithoutSaveSC();
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void switchFileContainingChart() throws Exception {
		String file1 = prepareData("svt/64K_Chart1.ods");
		String file2 = prepareData("svt/64K_Chart2.ods");
		String file3 = prepareData("svt/64K_Chart3.ods");
		String file4 = prepareData("svt/64K_Chart4.ods");
		String file5 = prepareData("svt/64K_Chart5.ods");
		String file6 = prepareData("svt/64K_Chart6.ods");

		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file1);
			calc.waitForExistence(20, 2);
			sleep(2);

			app.dispatch(".uno:Open");
			submitOpenDlg(file2);
			calc.waitForExistence(20, 2);
			sleep(2);

			app.dispatch(".uno:Open");
			submitOpenDlg(file3);
			calc.waitForExistence(20, 2);
			sleep(2);

			app.dispatch(".uno:Open");
			submitOpenDlg(file4);
			calc.waitForExistence(20, 2);
			sleep(2);

			app.dispatch(".uno:Open");
			submitOpenDlg(file5);
			calc.waitForExistence(20, 2);
			sleep(2);

			app.dispatch(".uno:Open");
			submitOpenDlg(file6);
			calc.waitForExistence(20, 2);

			sleep(2);

			selectCertainFile(2);
			sleep(2);
			selectCertainFile(3);
			sleep(2);
			selectCertainFile(5);
			sleep(2);

			closeAlldocuments();
			sleep(3);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	public void selectCertainFile(int k) {
		calc.menuItem("Window").select();
		k += 5;
		for (int i = 0; i < k; i++) {
			typeKeys("<down>");
		}
		typeKeys("<enter>");
		sleep(2);
	}

	public void closeAlldocuments() {
		for (int index = 3; index < 9; index++) {
			calc.menuItem("File->Close").select();
			sleep(2);
		}

	}

	public static void inputCells(String[][] inputs) {
		String back = "";
		for (int i = 0; i < inputs.length; i++) {
			calc.typeKeys(back);
			sleep(1);
			back = "";
			for (int j = 0; j < inputs[i].length; j++) {
				typeKeys(inputs[i][j]);
				typeKeys("<Right>");
				sleep(0.5);
				back += "<Left>";
			}
			back += "<enter>";
		}
		sleep(2);
	}

	private void createNewSC() {
		app.dispatch("private:factory/scalc");
	}

	private void saveAndReopenNewSC(int iterator) {
		String saveTo = getPath("temp/" + "tempSC_New" + iterator + ".ods");
		calc.menuItem("File->Save As...").select();
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		if (activeMsgBox.exists()) {
			activeMsgBox.yes();
			sleep(2);
		}
		app.dispatch(".uno:CloseDoc");
		// sleep(2);
		startCenterOpenButton.waitForExistence(30, 2);
		startCenterOpenButton.click();
		submitOpenDlg(saveTo);
		sleep(1);
		app.dispatch(".uno:CloseDoc");
	}

	private void closeWithoutSaveSC() {
		calc.menuItem("File->Close").select();
		activeMsgBox.no();
		sleep(2);
	}

	private void addRecord(int i, long start, long end) {
		HashMap<String, Object>  perf = aoo.getPerfData();
		xmlResult.addRow("Data",testname.getMethodName(), i, (end - start),
				perf.get("vsz"), perf.get("rss"), perf.get("handles"));
	}
}
