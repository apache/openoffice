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

package pvt.gui;

import static org.junit.Assert.*;
import static org.openoffice.test.common.Testspace.*;
import static org.openoffice.test.vcl.Tester.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import java.awt.Rectangle;
import java.util.HashMap;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TestName;
import org.openoffice.test.OpenOffice;
import org.openoffice.test.common.Condition;
import org.openoffice.test.common.DataSheet;
import org.openoffice.test.common.GraphicsUtil;
import org.openoffice.test.common.Logger;


public class Benchmark {
	@Rule
	public Logger log = Logger.getLogger(this);
	
	@Rule
	public TestName testname = new TestName();
	
	private static DataSheet result;
	
	private static final double INTERVAL = 0.1; 
	
	private static int repeat = 8;
	
	private int i  = 0;
	
	public Benchmark() {

	}
	
	@BeforeClass
	public static void beforeClass() throws Exception {
		OpenOffice.killAll();
		result = new DataSheet(getFile("output/" + Benchmark.class.getName() + ".xml"));
		result.addRow("data", "Scenario", "No", "Consumed Time", "Memory(VSZ)", "Memory(RSS)", "Handles(Windows Only)");
	}
	
	@AfterClass
	public static void afterClass() throws Exception {
		app.stop();
	}
	
	@After
	public void after() {
		if (i < repeat)
			result.addRow("data", testname.getMethodName(), i, "err", "err", "err", "err", "err");
	}
	
	private void addRecord(long start, long end) {
		sleep(2);
		HashMap<String, Object>  perf = aoo.getPerfData();
		result.addRow("data", testname.getMethodName(), i, (end - start), perf.get("vsz"), perf.get("rss"), perf.get("handles"));
	}
	
	@Test
	public void coolStartup() throws Exception {
		app.stop();
		for (i = 0; i < repeat; i++) {
			aoo.cleanUserInstallation();
			assertFalse("User profile exists", aoo.getUserInstallation().exists());
			aoo.start();
			long start = System.currentTimeMillis();
			startcenter.waitForExistence(120, INTERVAL);
			long end = System.currentTimeMillis();
			addRecord(start, end);
			app.quit();
		}
		
	}
	
	@Test
	public void warmStartup() throws Exception {
		// Make sure we has generated user profile
		app.start(true);
		app.quit();
		
		for (i = 0; i < repeat; i++) {
			assertTrue("User profile exists", aoo.getUserInstallation().exists());
			aoo.start();
			long start = System.currentTimeMillis();
			startcenter.waitForExistence(120, INTERVAL);
			long end = System.currentTimeMillis();
			addRecord(start, end);
			app.quit();
		}
		
	}

	@Test
	public void newTextDocument() {
		app.start(true);
		app.quit();
		
		for (i = 0; i < repeat; i++) {
			app.start();
			startCenterWriterButton.click(0.5, 0.5);
			long start = System.currentTimeMillis();
			writer.waitForExistence(60, INTERVAL);
			long end = System.currentTimeMillis();
			addRecord(start, end);
			app.quit();
		}
		
	}

	@Test
	public void newSpreadsheet() {
		app.start(true);
		app.quit();
		for (i = 0; i < repeat; i++) {
			app.start();
			startCenterCalcButton.click(0.5, 0.5);
			long start = System.currentTimeMillis();
			calc.waitForExistence(60, INTERVAL);
			long end = System.currentTimeMillis();
			addRecord(start, end);
			app.quit();
		}
		
	}
	
	@Test
	public void newPresentation() {
		app.start(true);
		app.quit();
		for (i = 0; i < repeat; i++) {
			app.start();
			startCenterImpressButton.click(0.5, 0.5);
			presentationWizard.click(0.9, 0.95);
			long start = System.currentTimeMillis();
			impress.waitForExistence(60, INTERVAL);
			long end = System.currentTimeMillis();
			addRecord(start, end);
			app.quit();
		}
		
	}
	
	@Test
	public void slideShow() {
		app.start(true);
		app.quit();
		
		String path = prepareData("pvt/slideshow.odp");
		final Rectangle rect = GraphicsUtil.getScreenRectangle();
		// when slide show is running, top-center area will be filled with green
		rect.setRect(rect.getCenterX(), 2, 2, 2);
		for (i = 0; i < repeat; i++) {
			app.start();
			open(path);
			impress.waitForExistence(60, 1);
			sleep(2);
			assertFalse("Slideshow control exists", slideShow.exists());
			assertFalse("Slideshow is not started", GraphicsUtil.isFilledWith(0xFF00FF00, rect));
			typeKeys("<F5>");
			long start = System.currentTimeMillis();
			new Condition() {
				@Override
				public boolean value() {
					return GraphicsUtil.isFilledWith(0xFF00FF00, rect);
				}
				
			}.waitForTrue("", 120, INTERVAL);
			long end = System.currentTimeMillis();
			addRecord(start, end);
			slideShow.typeKeys("<esc>");
			sleep(2);
			app.quit();
		}
		
	}
	
	@Test
	public void loadFinishPlainODT() {
		loadFinish("pvt/plain_200p.odt", "Page 1 / 23[0-9]{1}");
	}
	
	@Test
	public void loadFinishPlainDOC() {
		loadFinish("pvt/plain_50p.doc", "Page i / 5[0-9]{1}");
	}
	
	@Test
	public void loadFinishPlainDOCX() {
		loadFinish("pvt/plain_200p.docx", "Page 1 / 1[8-9]{1}[0-9]{1}");
	}
	
	@Test
	public void loadFinishPlainODS() {
		loadFinish("pvt/plain_11s.ods", "Sheet 1 / 11");
	}
	
	@Test
	public void loadFinishPlainXLS() {
		loadFinish("pvt/plain_11s.xls", "Sheet 1 / 11");
	}
	
	@Test
	public void loadFinishPlainXLSX() {
		loadFinish("pvt/plain_11s.xlsx", "Sheet 1 / 11");
	}
	
	@Test
	public void loadFinishPlainODP() {
		loadFinish("pvt/plain_200p.odp", "Slide 1 / 200");
	}
	
	@Test
	public void loadFinishPlainPPT() {
		loadFinish("pvt/plain_200p.ppt", "Slide 1 / 200");
	}
	
	@Test
	public void loadFinishPlainPPTX() {
		loadFinish("pvt/plain_200p.pptx", "Slide 1 / 200");
	}
	
	@Test
	public void loadFinishComplexDOC() {
		loadFinish("pvt/complex_300p.doc", "Page 1 / 3[0-9]{2}");
	}
	
	@Test
	public void loadFinishComplexDOCX() {
		loadFinish("pvt/complex_400p.docx", "Page 1 / 3[2-9]{2}");
	}
	
	@Test
	public void loadFinishComplexODT() {
		loadFinish("pvt/complex_800p.odt", "Page 1 / 8[0-9]{2}");
	}
	
	@Test
	public void loadFinishComplexODS() {
		loadFinish("pvt/complex_19s.ods", "Sheet 8 / 19");
	}
	
	@Test
	public void loadFinishComplexXLS() {
		loadFinish("pvt/complex_29s.xls", "Sheet 2 / 29");
	}
	
	@Test
	public void loadFinishComplexXLSX() {
		loadFinish("pvt/complex_29s.xlsx", "Sheet 29 / 29");
	}
	
	@Test
	public void loadFinishComplexODP() {
		loadFinish("pvt/complex_150p.odp", "Slide 1 / 150");
	}
	
	@Test
	public void loadFinishComplexPPT() {
		loadFinish("pvt/complex_100p.ppt", "Slide 1 / 100");
	}
	
	@Test
	public void loadFinishComplexPPTX() {
		loadFinish("pvt/complex_100p.pptx", "Slide 1 / 100");
	}
	
	public void loadFinish(String file, final String indicator) {
		final int openIndicatorIndex = file.matches(".*\\.(odp|ppt|pptx)$") ? 4 : 0;
		String path = prepareData(file);
		app.stop();
		for (i = 0; i < repeat; i++) {
			app.start();
			app.dispatch(".uno:Open");
			filePickerPath.setText(path);
			sleep(1);
			filePickerOpen.click(0.5, 0.5);
			long start = System.currentTimeMillis();
			new Condition() {
				@Override
				public boolean value() {
					try {
						String text = statusBar.getItemText(openIndicatorIndex);
						return text.matches(indicator);
					} catch (Exception e) {
						return false;
					}
				}
				
			}.waitForTrue("", 120, INTERVAL);
			long end = System.currentTimeMillis();
			addRecord(start, end);
			discard();
			app.quit();
		}
		
	}
	
	@Test
	public void savePlainODT() {
		save("pvt/plain_200p.odt", "Page 1 / 23[0-9]{1}");
	}
	
	@Test
	public void savePlainDOC() {
		save("pvt/plain_50p.doc", "Page i / 5[0-9]{1}");
	}
	
	@Test
	public void savePlainODS() {
		save("pvt/plain_11s.ods", "Sheet 1 / 11");
	}
	
	@Test
	public void savePlainXLS() {
		save("pvt/plain_11s.xls", "Sheet 1 / 11");
	}
	
	@Test
	public void savePlainODP() {
		save("pvt/plain_200p.odp", "Slide 1 / 200");
	}
	
	@Test
	public void savePlainPPT() {
		save("pvt/plain_200p.ppt", "Slide 1 / 200");
	}
	
	@Test
	public void saveComplexODT() {
		save("pvt/complex_800p.odt", "Page 1 / 8[0-9]{2}");
	}
	
	@Test
	public void saveComplexDOC() {
		save("pvt/complex_300p.doc", "Page 1 / 3[0-9]{2}");
	}

	@Test
	public void saveComplexODS() {
		save("pvt/complex_19s.ods", "Sheet 8 / 19");
	}

	@Test
	public void saveComplexXLS() {
		save("pvt/complex_29s.xls", "Sheet 2 / 29");
	}
	
	@Test
	public void saveComplexODP() {
		save("pvt/complex_150p.odp", "Slide 1 / 150");
	}
	
	@Test
	public void saveComplexPPT() {
		save("pvt/complex_100p.ppt", "Slide 1 / 100");
	}
	
	public void save(String file, final String openIndicator) {
		boolean alienFormat = file.matches(".*\\.(doc|xls|ppt|docx|xlsx|pptx)$");
		final int openIndicatorIndex = file.matches(".*\\.(odp|ppt|pptx)$") ? 4 : 0;
		final int saveIndicatorIndex = file.matches(".*\\.(odt|doc|docx)$") ? 5 : file.matches(".*\\.(ods|xls|xlsx)$") ? 4 : 2;
		app.stop();
		String picture = prepareData("image/red_64x64.bmp");
		for (i = 0; i < repeat; i++) {
			String dir = "temp/file" + i;
			getFile(dir).mkdirs();
			app.start();
			open(prepareData(file, dir));
			new Condition() {
				@Override
				public boolean value() {
					try {
						String text = statusBar.getItemText(openIndicatorIndex);
						return text.matches(openIndicator);
					} catch (Exception e) {
						return false;
					}
				}
				
			}.waitForTrue("", 120, 1);
			sleep(2);
			insertPicture(picture);
			sleep(5);
			assertEquals("File is modified", "*", statusBar.getItemText(saveIndicatorIndex));
			app.dispatch(".uno:Save");
			if (alienFormat) {
				alienFormatDlg.waitForExistence(3, 1);
				sleep(1);
				typeKeys("<enter>");
			}
			
			long start = System.currentTimeMillis();
			new Condition() {
				@Override
				public boolean value() {
					try {
						String text = statusBar.getItemText(saveIndicatorIndex);
						return " ".equals(text);
					} catch (Exception e) {
						return false;
					}
				}
				
			}.waitForTrue("", 120, INTERVAL);
			long end = System.currentTimeMillis();
			addRecord(start, end);
			close();
			app.stop();
		}
		
	}
}
