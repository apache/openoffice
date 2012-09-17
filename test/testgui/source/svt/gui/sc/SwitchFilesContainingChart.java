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
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.SystemUtil;
import org.openoffice.test.common.Testspace;

public class SwitchFilesContainingChart {
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
		result = new PrintStream(new FileOutputStream(Testspace.getFile("output/svt_sc_switchFiles.csv")));
		HashMap<String, Object> proccessInfo = SystemUtil.findProcess(".*(soffice\\.bin|soffice.*-env).*");
		pid = (String)proccessInfo.get("pid");
		result.println("Iterator,Time,Memory(KB),CPU(%)");
		log.info("Result will be saved to " + Testspace.getPath("output/svt_sc_switchFiles.csv"));
	}

	@After
	public void tearDown() throws Exception {
		app.close();
		result.close();
	}
	
	@Test
	public void SwitchFileContainingChart() throws Exception {
		String file1 = prepareData("svt/sc_chart edit mod close_1.ods");
		String file2 = prepareData("svt/sc_chart edit mod close_2.ods");
		String file3 = prepareData("svt/sc_chart edit mod close_3.ods");
		String file4 = prepareData("svt/sc_chart edit mod close_4.ods");
		String file5 = prepareData("svt/sc_chart edit mod close_5.ods");
		String file6 = prepareData("svt/sc_chart edit mod close_6.ods");
		
		for(int i = 0; i < 500; i++)
		{
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
			selectCertainFile(6);
			sleep(2);
			selectCertainFile(8);
			sleep(2);
			
			closeAlldocuments();
			sleep(5);
			
			HashMap<String, Object> perfData = SystemUtil.getProcessPerfData(pid);
			String record = i + "," + System.currentTimeMillis() + "," + perfData.get("rss") + "," + perfData.get("pcpu");
			log.info("Record: " + record);
			result.println(record);
			result.flush();
			
			sleep(3);
		}
		
	
	}
	
	public void selectCertainFile(int k){
		calc.menuItem("Window").select();
		k += 5;
		for(int i=0;i<k;i++){
		    typeKeys("<down>");   
		}
		typeKeys("<enter>");
		sleep(2);
//		DocWindow.scEdit.doubleClick(atPoint(438,131));
	}
	
	public void closeAlldocuments(){
		for(int index=3;index<9;index++){
			//SelectActivedocument(4);
			calc.menuItem("File->Close").select();
			sleep(2);
		}
		
	}

}
