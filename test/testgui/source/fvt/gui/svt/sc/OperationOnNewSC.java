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

package testcase.gui.svt.sc;

import static org.openoffice.test.common.Testspace.*;
import static org.openoffice.test.vcl.Tester.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import java.io.FileOutputStream;
import java.io.PrintStream;
import java.util.HashMap;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.OpenOffice;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.SystemUtil;
import org.openoffice.test.common.Testspace;

import testlib.gui.SCTool;

public class OperationOnNewSC {
	@Rule
	public Logger log = Logger.getLogger(this);

	private PrintStream result = null;

	private String pid = null;

	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		OpenOffice.killAll();
		app.start();
		result = new PrintStream(new FileOutputStream(Testspace.getFile("output/svt_sc_new.csv")));
		HashMap<String, Object> proccessInfo = SystemUtil.findProcess(".*(soffice\\.bin|soffice.*-env).*");
		pid = (String) proccessInfo.get("pid");
		result.println("Iterator,Time,Memory(KB),CPU(%)");
		log.info("Result will be saved to " + Testspace.getPath("output/svt_sc_new.csv"));
	}

	@After
	public void tearDown() throws Exception {
		app.close();
		result.close();
	}

	@Test
	public void operationOnNewSC() throws Exception {
		String[][] inputStr = { { "3" }, { "2" }, { "5" }, { "1" }, { "6" }, { "4" }, { "10" }, { "8" }, { "9" }, { "7" } };
		String[][] inputStr_InstantFilter = { { "A" }, { "1" }, { "2" }, { "3" }, { "1" }, { "2" }, { "3" }, { "1" }, { "2" }, { "3" } };
		String pic = prepareData("svt/Sunset.jpg");

		for (int i = 0; i < 1000; i++) {
			// Data Sort
			app.dispatch("private:factory/scalc");
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
			sleep(5);

			// Insert Sheet
			app.dispatch(".uno:Insert");
			scAfterCurrentSheet.check();
			scNewSheetName.setText("Instant Filter");
			scInsertSheetDlg.ok();
			sleep(5);

			// Standard Filter
			SCTool.selectRange("A1");
			typeKeys("A<down>1<down>2<down>3<down>1<down>2<down>3<down>1<down>2<down>3");
			sleep(1);
			SCTool.selectRange("A1");
			app.dispatch(".uno:DataFilterStandardFilter");
			sleep(2);
			filterValue1.setText("1");
			standardFilterDlg.ok();
			sleep(5);

			// Data Validate
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

			// Input cells, insert pics and chart
			app.dispatch(".uno:Insert");
			scBeforeCurrentSheet.check();
			scNewSheetName.setText("InsertObjects");
			scInsertSheetDlg.ok();
			sleep(5);
			calc.menuItem("Insert->Picture->From File...").select();
			sleep(2);
			filePickerPath.setText(pic);
			sleep(1);
			filePickerOpen.click();
			sleep(5);
			typeKeys("<esc>");
			sleep(5);

			// Save file and close
			String saveTo = "tempSC_New" + i + ".ods";
			calc.menuItem("File->Save As...").select();
			FileUtil.deleteFile(saveTo);
			submitSaveDlg(saveTo);
			if (activeMsgBox.exists()) {
				activeMsgBox.yes();
				sleep(2);
			}
			calc.menuItem("File->Close").select();

			HashMap<String, Object> perfData = SystemUtil.getProcessPerfData(pid);
			String record = i + "," + System.currentTimeMillis() + "," + perfData.get("rss") + "," + perfData.get("pcpu");
			log.info("Record: " + record);
			result.println(record);
			result.flush();

			sleep(3);
		}
	}

}
