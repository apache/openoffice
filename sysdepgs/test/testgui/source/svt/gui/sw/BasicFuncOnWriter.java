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
package svt.gui.sw;

import static org.openoffice.test.common.Testspace.getFile;
import static org.openoffice.test.common.Testspace.getPath;
import static org.openoffice.test.common.Testspace.prepareData;
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
import org.openoffice.test.common.DataSheet;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.SystemUtil;
import org.openoffice.test.common.Testspace;

public class BasicFuncOnWriter {
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
		xmlResult = new DataSheet(getFile("output/svt_writer.xml"));
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
	public void saveNewSWWithPic() throws Exception {
		String pic = prepareData("image/blue_256x256.jpg");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSW();
			writer.menuItem("Insert->Picture->From File...").select();
			sleep(2);
			filePickerPath.setText(pic);
			sleep(1);
			filePickerOpen.click();
			sleep(5);
			typeKeys("<esc>");
			sleep(2);
			saveAndReopenNewSW(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	
	@Test
	public void saveNewSWWithBullet() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSW();
			writer.typeKeys("The first");
			writer.typeKeys("<enter>");
			sleep(3);
			writer.typeKeys("The second");
			app.dispatch(".uno:SelectAll");
			writer.menuItem("Format->Bullets and Numbering...").select();
			sleep(3);
			bulletPage.ok();
			writer.typeKeys("<ctrl z");
			sleep(1);
			writer.typeKeys("<ctrl y>");
			sleep(1);
			writer.typeKeys("<ctrl end>");
			writer.typeKeys("<enter>");
			sleep(2);
			
			writer.focus();
			saveAndReopenNewSW(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void saveNewSWWithUnDoDelete()throws Exception{
		if(!sdDrawingToolbar.exists()){
			app.dispatch(".uno:AvailableToolbars?Toolbar:string=toolbar");
		}
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSW();
			app.dispatch(".uno:Ellipse");
			writer.focus();
			writer.drag(200, 200, 300, 300);
			typeKeys("<Delete>");
			typeKeys("<ctrl z>");
			typeKeys("<ctrl y>");

			saveAndReopenNewSW(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	
	@Test
	public void saveNewSWWithSpellCheck() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSW();
			writer.typeKeys("goood");
			sleep(2);
			writer.menuItem("Tools->Spelling and Grammar...").select();
			sleep(2);
			spellcheckDlgSuggestionList.select(0);
			spellcheckDlgIgnoreOnce.click();
			activeMsgBox.ok();
			sleep(2);
			saveAndReopenNewSW(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void saveNewSWWithHeaderFooter() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSW();
			writer.menuItem("Insert->Header->Default").select();
			sleep(2);
			writer.typeKeys("Header");
			sleep(2);
			writer.menuItem("Insert->Footer->Default").select();
			sleep(2);
			writer.typeKeys("Footer");
			sleep(2);
			writer.typeKeys("<ctrl end>");
			sleep(2);
			writer.typeKeys("<enter>");
			sleep(2);
			saveAndReopenNewSW(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void saveNewSWWithFrameAnchor() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSW();
			writer.menuItem("Insert->Frame...").select();
			sleep(2);
			writerFrameDlg.ok();
			writer.menuItem("Format->Anchor->To Page").select();
			sleep(2);
			writer.menuItem("Format->Anchor->To Paragraph").select();
			sleep(2);
			writer.menuItem("Format->Anchor->To Character").select();
			sleep(2);
			writer.menuItem("Format->Anchor->As Character").select();
			sleep(2);
			writer.typeKeys("<esc>");
			sleep(2);
			writer.typeKeys("<enter>");
			sleep(2);
			saveAndReopenNewSW(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void saveNewSWWithSDAsOLE()throws Exception{
		String linkFile = prepareData("pvt/plain_200p.odp");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSW();
			app.dispatch(".uno:InsertObject");
			sleep(2);
			insertObject.waitForExistence(60, 2);
			objectFromFile.check();
			objectFilePath.setText(linkFile);
			objectFileLink.check();
			insertObject.ok();
			sleep(10);
			
			writer.focus();
			writer.doubleClick(200, 300);
			sleep(15);
			writer.activate();
			writer.click(10, 10);
			sleep(5);
			
			saveAndReopenNewSW(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void saveNewSWWithTOC() throws Exception {
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSW();
			for (int j = 0; j < 6; j++) {
				writer.typeKeys(String.valueOf(j + 1));
				writer.typeKeys("<enter>");
			}
			writer.typeKeys("<ctrl home>");
			for (int k = 0; k < 2; k++) {
				writer.typeKeys("<shift down>");
			}
			if (!styleAndFormattingDlg.exists()) {
				app.dispatch(".uno:DesignerDialog");
				sleep(2);
			}

			styleAndFormattingList.doubleClick(0.5, 0.25);
			sleep(2);
			writer.typeKeys("<down>");
			writer.typeKeys("<right>");
			writer.typeKeys("<left>");
			for (int k = 0; k < 2; k++) {
				writer.typeKeys("<shift down>");
			}
			styleAndFormattingList.doubleClick(0.5, 0.3);
			sleep(2);
			writer.typeKeys("<ctrl home>");
			sleep(2);
			writer.menuItem("Insert->Indexes and Tables->Indexes and Tables...").select();
			sleep(2);
			insertIndexDlg.ok();
			sleep(2);
			writer.typeKeys("<ctrl end>");
			saveAndReopenNewSW(i);
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void splitMergeTableOnOpenedSW() throws Exception {
		String file = prepareData("svt/swdesign.odt");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			writer.waitForExistence(10, 2);
			writer.typeKeys("<down>");
			writer.typeKeys("<down>");
			sleep(2);

			writer.menuItem("Table->Split Cells").select();
			sleep(2);
			writerSplitCellDlg.ok();
			sleep(2);

			writer.typeKeys("<shift down>");
			writer.typeKeys("<shift down>");
			writer.typeKeys("<shift down>");
			writer.typeKeys("<shift down>");
			writer.typeKeys("<shift down>");
			sleep(2);
			writer.menuItem("Table->Merge Cells").select();
			sleep(2);

			writer.typeKeys("<enter>");
			closeWithoutSaveSW();
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void addDelColumnsRowsInTableOnOpenedSW() throws Exception {
		String file = prepareData("svt/swdesign.odt");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			writer.waitForExistence(10, 2);
			writer.typeKeys("<down>");
			writer.typeKeys("<down>");
			sleep(2);

			app.dispatch(".uno:InsertRowDialog");
			writerInsertRowsDlg.ok();
			sleep(2);

			writer.menuItem("Table->Delete->Rows").select();
			sleep(2);

			app.dispatch(".uno:InsertColumnDialog");
			writerInsertColumnsDlg.ok();
			sleep(2);

			writer.menuItem("Table->Delete->Columns").select();
			sleep(2);
			
			closeWithoutSaveSW();
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void setParaPropertoesOnOpenedSW() throws Exception {
		String file = prepareData("pvt/plain_200p.odt");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			writer.waitForExistence(10, 2);
			writer.focus();
			app.dispatch(".uno:ParagraphDialog");
			alignPage.select();
			alignPageRight.check();
			sleep(2);
			alignPageVerticalAlign.select("Bottom");
			sleep(2);
			alignPage.ok();
			sleep(2);
			writer.typeKeys("<ctrl end>");
			sleep(5);
			
			closeWithoutSaveSW();
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void InsertPicToOpenedSW() throws Exception {
		String file = prepareData("pvt/plain_200p.odt");
		String pic = prepareData("image/blue_256x256.jpg");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			writer.waitForExistence(10, 2);
			writer.focus();
			writer.menuItem("Insert->Picture->From File...").select();
			sleep(2);
			filePickerPath.setText(pic);
			sleep(1);
			filePickerOpen.click();
			sleep(5);
			writer.typeKeys("<esc>");
			sleep(2);			
			closeWithoutSaveSW();
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void InsertOLEToOpenedSW() throws Exception {
		String file = prepareData("pvt/plain_200p.odt");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			writer.waitForExistence(10, 2);
			writer.focus();
			app.dispatch(".uno:InsertObject");
			sleep(2);
			insertObject.ok();
			sleep(10);
			typeKeys("<esc>");
			sleep(5);
			typeKeys("<esc>");
			sleep(5);		
			closeWithoutSaveSW();
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void InsertChartToOpenedSW() throws Exception {
		String file = prepareData("pvt/plain_200p.odt");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			writer.waitForExistence(10, 2);
			writer.focus();
			app.dispatch(".uno:InsertObjectChart");
//			writer.menuItem("Insert->Object->Chart...").select();
			sleep(5);
			typeKeys("<esc>");
			sleep(5);
			typeKeys("<esc>");
			sleep(5);
			typeKeys("<esc>");
			sleep(5);
			closeWithoutSaveSW();
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	@Test
	public void openSWWithXForm()throws Exception{
		String file = prepareData("svt/sickness_self-certification_meory issue.odt");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			writer.waitForExistence(10, 2);
			app.dispatch(".uno:CloseDoc");
			
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void openSWWithSecuritySignature()throws Exception{
		String file = prepareData("svt/howtouse.odt");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			app.dispatch(".uno:Open");
			submitOpenDlg(file);
			writer.waitForExistence(10, 2);
			app.dispatch(".uno:CloseDoc");
			
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	
	private void createNewSW() {
		app.dispatch("private:factory/swriter");
	}
	
	private void saveAndReopenNewSW(int iterator) {
		String saveTo = getPath("temp/" + "tempSW_New" + iterator + ".odt");
		writer.menuItem("File->Save As...").select();
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		if (activeMsgBox.exists()) {
			activeMsgBox.yes();
			sleep(2);
		}
		app.dispatch(".uno:CloseDoc");	
		startCenterOpenButton.waitForExistence(30, 2);
		startCenterOpenButton.click();
		submitOpenDlg(saveTo);
		sleep(1);		
		app.dispatch(".uno:CloseDoc");
	}

	private void closeWithoutSaveSW() {
		writer.menuItem("File->Close").select();
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
