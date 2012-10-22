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

public class FileTypeAboutWriter {
	@Rule
	public Logger log = Logger.getLogger(this);

	@Rule
	public TestName testname = new TestName();

	private static DataSheet xmlResult;

	private String pid = null;

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
	public void saveNewSWWithShapeToHTML()throws Exception{
		for ( i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSW();
			if(!swDrawingToolbar.exists()){
				app.dispatch(".uno:AvailableToolbars?Toolbar:string=drawbar");
			}
			sleep(2);
			app.dispatch(".uno:Rect");
			writer.focus();
			writer.drag(200, 200, 300, 300);
			sleep(2);

			typeKeys("<esc>");
			sleep(2);
			saveToOther(i,".html");
			//reopen
			String saveTo = getPath("temp/" + "tempDOC" + i + ".html");
			startCenterOpenButton.click();
			submitOpenDlg(saveTo);
			sleep(1);
			app.dispatch(".uno:CloseDoc");	
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
    public void saveNewSWWithLineToDoc()throws Exception{
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSW();
			if(!swDrawingToolbar.exists()){
				app.dispatch(".uno:AvailableToolbars?Toolbar:string=drawbar");
			}
			sleep(2);
			swInsertLineButtonOnToolbar.click();
			writer.focus();
			for(int j=0;j<10;j++){
			writer.drag(150+j, 150+j*10, 200+j, 200+j*10);
			sleep(2);
			}
			typeKeys("<esc>");
			sleep(2);
			saveToOther(i,".doc");
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}
	
	@Test
	public void saveNewSWWithPicToDoc() throws Exception {
		String pic = prepareData("image/blue_256x256.jpg");
		for (i = 1; i <= iterator; i++) {
			long start = System.currentTimeMillis();
			createNewSW();
			writer.menuItem("Insert->Picture->From File...").select();
			sleep(2);
			filePickerPath.setText(pic);
			sleep(1);
			filePickerOpen.click();
			sleep(3);
			typeKeys("<esc>");
			sleep(2);
			saveToOther(i,".doc");
			long end = System.currentTimeMillis();
			addRecord(i, start, end);
		}
	}

	private void createNewSW() {
		app.dispatch("private:factory/swriter");
	}

	private void saveToOther(int iterator, String type){
		String saveTo = getPath("temp/" + "tempDOC" + iterator + type);
		writer.menuItem("File->Save As...").select();
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		if (activeMsgBox.exists()) {
			activeMsgBox.yes();
			sleep(2);
		}		
		if (alienFormatDlg.exists(3))
			alienFormatDlg.ok();
		app.dispatch(".uno:CloseDoc");	
		startCenterOpenButton.waitForExistence(30, 2);
		startCenterOpenButton.click();
		submitOpenDlg(saveTo);
		sleep(2);		
		app.dispatch(".uno:CloseDoc");
		
	}
	
	private void addRecord(int i, long start, long end) {
		HashMap<String, Object>  perf = aoo.getPerfData();
		xmlResult.addRow("Data",testname.getMethodName(), i, (end - start),
				perf.get("vsz"), perf.get("rss"), perf.get("handles"));
	}

	
}
