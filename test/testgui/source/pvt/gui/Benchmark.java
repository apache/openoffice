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

import static org.openoffice.test.common.Testspace.*;
import static org.openoffice.test.vcl.Tester.*;
import static testlib.gui.UIMap.*;

import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.util.HashMap;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TestName;
import org.openoffice.test.OpenOffice;
import org.openoffice.test.common.Condition;
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.SystemUtil;
import org.openoffice.test.common.Testspace;


public class Benchmark {
	@Rule
	public Logger log = Logger.getLogger(this);
	
	@Rule
	public TestName testname = new TestName();
	
	private static PrintStream result;
	
	private static final double INTERVAL = 0.01;
	
	public Benchmark() {

	}
	
	@BeforeClass
	public static void beforeClass() throws Exception {
		File resultFile = Testspace.getFile("output/gui_benchmark.csv");
		resultFile.getParentFile().mkdirs();
		result = new PrintStream(new FileOutputStream(resultFile));
		OpenOffice.killAll();
		
		result.println("Scenario,Consumed Time,Memory(VSZ),Memory(RSS),Handles(Windows Only)");
	}
	
	@AfterClass
	public static void afterClass() throws Exception {
		result.close();
		app.close();
	}
	
	private HashMap<String, Object> getPerfData() {
		HashMap<String, Object> proccessInfo = SystemUtil.findProcess(".*(soffice\\.bin|soffice\\.exe .*-env).*");
		String pid = (String) proccessInfo.get("pid");
		return SystemUtil.getProcessPerfData(pid);
	}
	
	private void addRecord(int i, long start, long end) {
		HashMap<String, Object>  perf = getPerfData();
		result.println(testname.getMethodName() + i + "," + (end - start) + "," + perf.get("vsz") + "," + perf.get("rss") + "," + perf.get("handles"));
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
	public void loadFinishPlainDoc() {
		loadFinishTextDocument("pvt_benchmark/sw_plain_120p.doc", "Page 1 / ");
	}
	
	@Test
	public void loadFinishPlainOdt() {
		loadFinishTextDocument("pvt_benchmark/sw_plain_120p_odf1.2.odt", "Page 1 / ");
	}
	
	@Test
	public void loadFinishComplexDoc() {
		loadFinishTextDocument("pvt_benchmark/sw_complex_100p.doc", "Page 1 / ");
	}
	
	@Test
	public void loadFinishComplexOdt() {
		loadFinishTextDocument("pvt_benchmark/sw_complex_100p_odf1.2.odt", "Page 1 / ");
	}
	
	public void loadFinishTextDocument(String file, final String indicator) {
		String path = prepareData(file);
		aoo.kill();
		aoo.start();
		startcenter.waitForExistence(120, 1);
		for (int i = 0; i < 8; i++) {
			app.dispatch(".uno:Open");
			filePickerPath.setText(path);
			filePickerOpen.click();
			long start = System.currentTimeMillis();
			new Condition() {
	
				@Override
				public boolean value() {
					String text = statusBar.getItemText(0);
					if (text == null)
						return false;
					return text.startsWith(indicator);
				}
				
			}.waitForTrue("", 120, INTERVAL);
			long end = System.currentTimeMillis();
			sleep(2);
			addRecord(i, start, end);
			app.dispatch(".uno:CloseDoc");
		}
	
		app.close();
	}
	
	@Test
	public void loadFinishPlainXLS() {
		loadFinishTextDocument("pvt_benchmark/sc_plain_4sh_5kcell.xls", "Sheet 2 / 4");
	}
	
	@Test
	public void loadFinishPlainODS() {
		loadFinishTextDocument("pvt_benchmark/sc_plain_4sh_5kcell_new_odf1.2.ods", "Sheet 1 / 4");
	}
	
	@Test
	public void loadFinishComplexXLS() {
		loadFinishTextDocument("pvt_benchmark/sc_complex_13sh_4kcell.xls", "Sheet 2 / 13");
	}
	
	@Test
	public void loadFinishComplexODS() {
		loadFinishTextDocument("pvt_benchmark/sc_complex_13sh_4kcell_new_odf1.2.ods", "Sheet 6 / 13");
	}
	
	
	@Test
	public void loadFinishPlainODP() {
		loadFinishTextDocument("pvt_benchmark/sc_complex_13sh_4kcell_new_odf1.2.ods", "Sheet 6 / 13");
	}
	
	@Test
	public void loadFinishPlainPPT() {
		loadFinishTextDocument("pvt_benchmark/sc_complex_13sh_4kcell_new_odf1.2.ods", "Sheet 6 / 13");
	}
	
	@Test
	public void loadFinishComplexODP() {
		loadFinishTextDocument("pvt_benchmark/sc_complex_13sh_4kcell_new_odf1.2.ods", "Sheet 6 / 13");
	}
	
	@Test
	public void loadFinishComplexPPT() {
		loadFinishTextDocument("pvt_benchmark/sc_complex_13sh_4kcell_new_odf1.2.ods", "Sheet 6 / 13");
	}
	
	public void loadFinish(String file, final String indicator) {
		String path = prepareData(file);
		aoo.kill();
		aoo.start();
		startcenter.waitForExistence(120, 1);
		for (int i = 0; i < 8; i++) {
			app.dispatch(".uno:Open");
			filePickerPath.setText(path);
			filePickerOpen.click();
			long start = System.currentTimeMillis();
			new Condition() {
	
				@Override
				public boolean value() {
					String text = statusBar.getItemText(0);
					if (text == null)
						return false;
					return text.startsWith(indicator);
				}
				
			}.waitForTrue("", 120, INTERVAL);
			long end = System.currentTimeMillis();
			sleep(2);
			addRecord(i, start, end);
			app.dispatch(".uno:CloseDoc");
		}
	
		app.close();
	}

	
}
