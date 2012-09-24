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
import org.openoffice.test.common.SystemUtil;


public class Benchmark {
	@Rule
	public Logger log = Logger.getLogger(this);
	
	@Rule
	public TestName testname = new TestName();
	
	private static DataSheet result;
	
	private static final double INTERVAL = 0.1;
	
	public Benchmark() {

	}
	
	@BeforeClass
	public static void beforeClass() throws Exception {
		result = new DataSheet(getFile("output/pvt_gui_benchmark.xml"), "benckmark");
		OpenOffice.killAll();
		result.addRow("Scenario", "No", "Consumed Time", "Memory(VSZ)", "Memory(RSS)", "Handles(Windows Only)");
	}
	
	@AfterClass
	public static void afterClass() throws Exception {
		app.close();
	}
	
	private HashMap<String, Object> getPerfData() {
		HashMap<String, Object> proccessInfo = SystemUtil.findProcess(".*(soffice\\.bin|soffice\\.exe .*-env).*");
		String pid = (String) proccessInfo.get("pid");
		return SystemUtil.getProcessPerfData(pid);
	}
	
	private void addRecord(int i, long start, long end) {
		HashMap<String, Object>  perf = getPerfData();
		result.addRow(testname.getMethodName(), i, (end - start), perf.get("vsz"), perf.get("rss"), perf.get("handles"));
	}
	
	@Test
	public void coolStartup() throws Exception {
		aoo.kill();
		aoo.cleanUserInstallation();
		aoo.start();
		long start = System.currentTimeMillis();
		startcenter.waitForExistence(120, INTERVAL);
		long end = System.currentTimeMillis();
		sleep(2);
		addRecord(0, start, end);
		app.close();
	}
	
	@Test
	public void warmStartup() throws Exception {
		for (int i = 0; i < 8; i++) {
			aoo.kill();//make sure app is closed
			aoo.start();
			long start = System.currentTimeMillis();
			startcenter.waitForExistence(120, INTERVAL);
			long end = System.currentTimeMillis();
			sleep(2);
			addRecord(i, start, end);
			app.close();
		}
	}

	@Test
	public void newTextDocument() {
		for (int i = 0; i < 8; i++) {
			aoo.kill();//make sure app is closed
			aoo.start();
			startCenterWriterButton.waitForExistence(120, 1);
			sleep(2);
			startCenterWriterButton.click(0.5, 0.5);
			long start = System.currentTimeMillis();
			writer.waitForExistence(60, INTERVAL);
			long end = System.currentTimeMillis();
			sleep(2);
			addRecord(i, start, end);
			app.close();
		}
	}

	@Test
	public void newSpreadsheet() {
		for (int i = 0; i < 8; i++) {
			aoo.kill();//make sure app is closed
			aoo.start();
			startCenterCalcButton.waitForExistence(120, 1);
			sleep(2);
			startCenterCalcButton.click(0.5, 0.5);
			long start = System.currentTimeMillis();
			calc.waitForExistence(60, INTERVAL);
			long end = System.currentTimeMillis();
			sleep(2);
			addRecord(i, start, end);
			app.close();
		}
	}
	
	@Test
	public void newPresentation() {
		for (int i = 0; i < 8; i++) {
			aoo.kill();//make sure app is closed
			aoo.start();
			startCenterImpressButton.waitForExistence(120, 1);
			sleep(2);
			startCenterImpressButton.click(0.5, 0.5);
			sleep(1);
			presentationWizard.click(0.9, 0.95);
			long start = System.currentTimeMillis();
			impress.waitForExistence(60, INTERVAL);
			long end = System.currentTimeMillis();
			sleep(2);
			addRecord(i, start, end);
			app.close();
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
		loadFinish("pvt/plain_200p.docx", "Page 1 / 19[0-9]{1}");
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
	public void loadFinishComplexODT() {
		loadFinish("pvt/complex_800p.odt", "Page 1 / 8[0-9]{2}");
	}
	
	@Test
	public void loadFinishComplexXLS() {
		loadFinish("pvt/sc_complex_13sh_4kcell.xls", "Sheet 2 / 13");
	}
	
	@Test
	public void loadFinishComplexODS() {
		loadFinish("pvt/sc_complex_13sh_4kcell_new_odf1.2.ods", "Sheet 6 / 13");
	}

	
	@Test
	public void loadFinishComplexODP() {
		loadFinish("pvt/sd_complex_51p_odf1.2.odp", "Slide 1 / 51");
	}
	
	@Test
	public void loadFinishComplexPPT() {
		loadFinish("pvt/sd_complex_51p.ppt", "Slide 1 / 51");
	}
	
	public void loadFinish(String file, final String indicator) {
		final int openIndicatorIndex = file.matches(".*\\.(odp|ppt|pptx)$") ? 4 : 0;
		String path = prepareData(file);
		aoo.kill();
		app.start();
		startcenter.waitForExistence(10, 1);
		sleep(2); // Give some seconds to AOO relax. We want data more stable.
		for (int i = 0; i < 8; i++) {
			app.dispatch(".uno:Open");
			filePickerPath.setText(path);
			sleep(2);
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
			sleep(2);
			addRecord(i, start, end);
			discard();
		}
	
		app.close();
	}
	
	@Test
	public void savePlainDOC() {
		save("pvt/plain_50p.doc", "Page i / 5[0-9]{1}");
	}
	
	@Test
	public void savePlainODT() {
		save("pvt/plain_200p.odt", "Page 1 / 23[0-9]{1}");
	}
	
	@Test
	public void saveComplexDOC() {
		save("pvt/complex_300p.doc", "Page 1 / 3[0-9]{2}");
	}
	
	@Test
	public void saveComplexODT() {
		save("pvt/complex_800p.odt", "Page 1 / 8[0-9]{2}");
	}
	
	@Test
	public void savePlainXLS() {
		save("pvt/plain_11s.xls", "Sheet 1 / 11");
	}
	
	@Test
	public void savePlainODS() {
		save("pvt/plain_11s.ods", "Sheet 1 / 11");
	}
	
	@Test
	public void saveComplexXLS() {
		save("pvt/sc_complex_13sh_4kcell.xls", "Sheet 2 / 13");
	}
	
	@Test
	public void saveComplexODS() {
		save("pvt/sc_complex_13sh_4kcell_new_odf1.2.ods", "Sheet 6 / 13");
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
	public void saveComplexODP() {
		save("pvt/sd_complex_51p_odf1.2.odp", "Slide 1 / 51");
	}
	
	@Test
	public void saveComplexPPT() {
		save("pvt/sd_complex_51p.ppt", "Slide 1 / 51");
	}
	
	public void save(String file, final String openIndicator) {
		boolean alienFormat = file.matches(".*\\.(doc|xls|ppt|docx|xlsx|pptx)$");
		final int openIndicatorIndex = file.matches(".*\\.(odp|ppt|pptx)$") ? 4 : 0;
		final int saveIndicatorIndex = file.matches(".*\\.(odt|doc|docx)$") ? 5 : file.matches(".*\\.(ods|xls|xlsx)$") ? 4 : 2;
		aoo.kill();
		app.start();
		String picture = prepareData("image/red_64x64.bmp");
		for (int i = 0; i < 8; i++) {
			String dir = "temp/file" + i;
			getFile(dir).mkdirs();
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
			sleep(3);
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
			sleep(2);
			addRecord(i, start, end);
			close();
		}
	
		app.close();
	}

	@Test
	public void slideShow() {
		aoo.kill();
		app.start();
		String path = prepareData("pvt/sd_slideshow.odp");
		final Rectangle rect = GraphicsUtil.getScreenRectangle();
		// when slide show is running, top-center area will be filled with green
		rect.setRect(rect.getCenterX(), 2, 2, 2);
		for (int i = 0; i < 8; i++) {
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
			sleep(2);
			addRecord(i, start, end);
			slideShow.typeKeys("<esc>");
			sleep(2);
			close();
		}
		app.close();
	}
}
