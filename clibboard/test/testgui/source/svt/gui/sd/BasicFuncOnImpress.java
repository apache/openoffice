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

package svt.gui.sd;

import static org.junit.Assert.assertEquals;
import static org.openoffice.test.common.Testspace.*;
import static org.openoffice.test.vcl.Tester.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import java.io.FileOutputStream;
import java.io.PrintStream;
import java.util.HashMap;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TestName;
import org.openoffice.test.OpenOffice;
import org.openoffice.test.common.DataSheet;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.SystemUtil;
import org.openoffice.test.common.Testspace;

import testlib.gui.SCTool;

public class BasicFuncOnImpress {
	@Rule
	public Logger log = Logger.getLogger(this);

	@Rule
	public TestName testname = new TestName();

	private static DataSheet xmlResult;

	private String pid = null;

	private static int iterator = 100;

	private int i = 0;

	/**
	 * @throws java.lang.Exception
	 */
	@BeforeClass
	public static void beforeClass() throws Exception {
		xmlResult = new DataSheet(getFile("output/svt_impress.xml"));
		xmlResult.addRow("Data", "Method", "No", "Consumed Time(MS)", "Memory(VSZ)", "Memory(RSS)", "Handles(Windows Only)");
	}

	@AfterClass
	public static void afterClass() throws Exception {
		app.stop();
	}

	@Before
	public void before() throws Exception{
		app.start(true);
	}

	@Test
	public void saveNewSDWithNewSlides() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSD();
			impress.menuItem("Insert->Slide").select();
			sleep(2);
			saveAndReopenNewSD(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void saveNewSDWithTable() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSD();
			app.dispatch(".uno:InsertTable", 3);
			sleep(2);
			numberofCol.setText("5");
			numberofRow.setText("4");
			sleep(2);
			insertTable.ok();
			impress.typeKeys("<enter>");
			impress.typeKeys("2");
			impress.typeKeys("<right>");
			impress.typeKeys("2");
			impress.typeKeys("<right>");
			impress.typeKeys("<ctrl end>");
			impress.typeKeys("2");
			sleep(2);
			saveAndReopenNewSD(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void saveNewSDWithPic() throws Exception {
		String pic = prepareData("image/blue_256x256.jpg");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSD();
			app.dispatch(".uno:InsertGraphic");
//			impress.menuItem("Insert->Picture->From File...").select();
			sleep(2);
			filePickerPath.setText(pic);
			sleep(1);
			filePickerOpen.click();
			sleep(5);
			impress.typeKeys("<esc>");
			sleep(2);
			saveAndReopenNewSD(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void saveNewSDWithTextbox()throws Exception{
		if(!sdDrawingToolbar.exists()){
			app.dispatch(".uno:AvailableToolbars?Toolbar:string=toolbar");
		}
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSD();
			sdInsertTextboxButtonOnToolbar.click();
			impress.focus();
			impress.drag(100, 100, 200, 200);
			typeKeys("test");
			typeKeys("<esc>");
			sleep(2);
			saveAndReopenNewSD(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void saveNewSDWithExternalSlides() throws Exception {
		String externalFile = prepareData("pvt/plain_200p.odp");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSD();
			app.dispatch(".uno:ImportFromFile");
			sleep(2);
			filePickerPath.setText(externalFile);
			filePickerOpen.click();
			sleep(2);
			insertSlideObjects.ok();
			sleep(5);
			saveAndReopenNewSD(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void saveNewSDWithAnimation()throws Exception{
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSD();			
			//Insert a Shape and select it via Navigator
			sdCalloutShapes.click();
			impress.focus();
			impress.drag(100, 100, 200, 200);
			app.dispatch(".uno:CustomAnimation");
			sdAddAnimation.click();
			sdEntranceAnimationTab.waitForExistence(60, 2);
			sdEntranceAnimationTab.select("Circle");
			typeKeys("<enter>");
			sdAnimationList.openContextMenu();
			typeKeys("<down><down><enter>");
			impress.menuItem("Slide Show->Slide Show").select();
			sleep(10);
			impress.typeKeys("<esc>");
			sleep(5);
			saveAndReopenNewSD(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void saveNewSDWithShowSettings() throws Exception {
		String externalFile = prepareData("pvt/plain_200p.odp");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSD();
			// Insert Slides from External Sample files
			impress.menuItem("Insert->File...").select();
			sleep(2);
			filePickerPath.setText(externalFile);
			filePickerOpen.click();
			sleep(2);
			insertSlideObjects.ok();
			sleep(20);

			// Slide Screen Show Settings
			impress.menuItem("Slide Show->Slide Transition...").select();
			sleep(2);
			impressSlideTransitions.select("Uncover Up");
			sleep(2);
			slideShowSpeed.select("Slow");
			sleep(5);
			slideAutoAfter.check();
			sleep(2);
			applyToAllSlides.click();
			sleep(2);
			impress.menuItem("Slide Show->Slide Show").select();
			sleep(70);
			impress.typeKeys("<esc>");
			sleep(5);
			saveAndReopenNewSD(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void saveSDTemplate()throws Exception{
		String templateFile = prepareData("svt/DarkBlue.otp");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(templateFile);
			impress.waitForExistence(10, 2);
			sleep(5);	
			saveAndReopenNewSD(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	@Ignore
	public void opChartOnOpenedSD() throws Exception {
		String file = prepareData("svt/OLEinODP.odp");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			impress.waitForExistence(10, 2);
			sleep(5);
			if (!sdNavigatorDlg.exists()) {
				app.dispatch(".uno:Navigator");
			}
			impress.focus();
			sdNavigator.focus();
			sdNavigator.collapseAll();
			sdNavigator.expand(0);
			sdNavigator.select(1);
			typeKeys("<enter>");
			impress.menuItem("Edit->Copy").select();
			sleep(2);
			impress.menuItem("Edit->Paste").select();
			sleep(2);

			sdNavigator.focus();
			sdNavigator.collapseAll();
			sdNavigator.expand(0);
			sdNavigator.select(2);
			typeKeys("<enter>");
			impress.menuItem("Edit->Copy").select();
			sleep(2);
			impress.menuItem("Edit->Paste").select();
			sleep(2);

			sdNavigator.focus();
			sdNavigator.collapseAll();
			sdNavigator.expand(0);
			sdNavigator.select(3);
			typeKeys("<enter>");
			impress.menuItem("Edit->Copy").select();
			sleep(2);
			impress.menuItem("Edit->Paste").select();
			sleep(2);
			closeWithoutSaveSD();
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void switchViewOfOpenedSD() throws Exception {
		String file = prepareData("pvt/plain_200p.odp");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			impress.waitForExistence(10, 2);
			sleep(5);

			impress.menuItem("View->Outline").select();
			sleep(2);
			impressOutline.menuItem("View->Slide Sorter").select();
			sleep(10);
			impressSlideSorter.menuItem("View->Notes Page").select();
			sleep(2);
			impressHandoutView.menuItem("View->Master->Slide Master").select();
			sleep(2);
			impress.menuItem("View->Master->Notes Master").select();
			sleep(2);

			impressHandoutView.menuItem("View->Normal").select();
			sleep(2);
			closeWithoutSaveSD();
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	private void createNewSD() {
		app.dispatch("private:factory/simpress?slot=6686");
		presentationWizard.ok();
	}

	private void saveAndReopenNewSD(int iterator) {
		String saveTo = getPath("temp/" + "tempSD_New" + iterator + ".odp");
		impress.menuItem("File->Save As...").select();
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		if (activeMsgBox.exists()) {
			activeMsgBox.yes();
			sleep(2);
		}
		sleep(5);
		app.dispatch(".uno:CloseDoc");	
		startCenterOpenButton.waitForExistence(30, 2);
		startCenterOpenButton.click();
		submitOpenDlg(saveTo);
		sleep(2);		
		app.dispatch(".uno:CloseDoc");
	}
	
	private void closeWithoutSaveSD() {
		impress.menuItem("File->Close").select();
		if (activeMsgBox.exists()) {
			activeMsgBox.no();
			sleep(2);
		}
	}

	private void addRecord(int i, long start, long end) {
		HashMap<String, Object>  perf = aoo.getPerfData();
		xmlResult.addRow("Data",testname.getMethodName(), i, (end - start),
				perf.get("vsz"), perf.get("rss"), perf.get("handles"));
	}
}
