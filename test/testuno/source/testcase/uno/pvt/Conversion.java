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

package testcase.uno.pvt;

import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TestRule;
import org.junit.rules.Timeout;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;
import org.openoffice.test.OpenOffice;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import com.sun.star.beans.PropertyValue;
import com.sun.star.document.MacroExecMode;
import com.sun.star.lang.XComponent;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.XCloseable;


@RunWith(Parameterized.class)
public class Conversion {
	@Rule
    public TestRule timeout = new Timeout(10 * 60000); 
	
	@Rule
	public Logger log = Logger.getLogger(this);

	public static File repos = Testspace.getFile("pvt_conversion_data");

	public static String[] params = {
		".*\\.((doc)|(dot)|(odt)|(ott))$", "writer_pdf_Export", "pdf", 
		".*\\.((xls)|(xlt)|(ods)|(ots))$", "calc_pdf_Export", "pdf", 
		".*\\.((ppt)|(ppt)|(odp)|(otp))$", "impress_pdf_Export", "pdf", 
		".*\\.((doc)|(dot)|(docx)|(docm)|(dotx)|(dotm))$", "writer8", "odt", 
		".*\\.((xls)|(xlt)|(xlsx)|(xltx)|(xlsm)|(xltm))$", "calc8", "ods", 
		".*\\.((ppt)|(pot)|(pptx)|(pptm)|(potm)|(potx))$", "impress8", "odp", 
		".*\\.((odt)|(ott))$", "MS Word 97", "doc", 
		".*\\.((ods)|(ots))$", "MS Excel 97", "xls", 
		".*\\.((odp)|(otp))$", "MS PowerPoint 97", "ppt", 
		};

	private static UnoApp app = new UnoApp();
	private static PrintStream result;
	@Parameters
	public static Collection<Object[]> data() {
		ArrayList<Object[]> list = new ArrayList<Object[]>();
		collect(repos, list);
		return list;
	}

	/**
	 * @param dir
	 * @param list
	 */
	public static void collect(File dir, ArrayList<Object[]> list) {
		File[] files = dir.listFiles();
		if (files == null)
			return;
		Arrays.sort(files);
		for (File file : files) {
			if (file.isDirectory()) {
				collect(file, list);
			} else {
				String fileName = file.getName().toLowerCase();
				for (int i = 0; i < params.length;) {
					String pattern = params[i++];
					String targetFilterName = params[i++];
					String targetExtName = params[i++];
					if (pattern != null && fileName.matches(pattern)) {
						Object[] data = {file, Testspace.getFile("classtemp/" + file.getName()+ "." + targetExtName), targetFilterName};
						list.add(data);
					}
				}
			}
		}
	}
	
	@BeforeClass
	public static void beforeClass() throws Exception {
		//Disable automation
		OpenOffice.getDefault().setAutomationPort(-1);
		OpenOffice.getDefault().addArgs("-invisible", "-conversionmode", "-headless", "-hidemenu");

		File resultFile = Testspace.getFile("output/conversion.csv");
		resultFile.getParentFile().mkdirs();
		result = new PrintStream(new FileOutputStream(resultFile));
		result.println("File,Scenario,After Close,After Save,After Load");
	}
	
	@AfterClass
	public static void afterClass() throws Exception {
		result.close();
		app.close();
	}
	
	private File sourceFile = null;
	private File targetFile = null;
	private String sourceFileUrl = null;
	private String targetFileUrl = null;
	private String targetFilterName = null;
	private String scenario = null;
	private String sourceFileId = null;
	private long loadTime = -1;
	private long saveTime = -1;
	private long closeTime = -1;
	
	public Conversion(File sourceFile, File targetFile, String targetFilterName) {
		super();
		this.sourceFile = sourceFile;
		this.targetFile = targetFile;
		this.targetFilterName = targetFilterName;
		sourceFileUrl = FileUtil.getUrl(this.sourceFile);
		targetFileUrl = FileUtil.getUrl(this.targetFile);
		scenario = FileUtil.getFileExtName(sourceFile.getName()).toLowerCase() + " to " + FileUtil.getFileExtName(targetFile.getName()).toLowerCase();
		sourceFileId = sourceFile.getAbsolutePath().replace(repos.getAbsolutePath(), "").replace("\\", "/");
	}
	
	@Before
	public void before() throws Exception {
		log.info("Start [File: " + sourceFileId + "] [Size: " + (sourceFile.length() / 1024) + "KB] [Scenario: " + scenario + "]");
		app.start();
	}
	
	@After
	public void after() throws Exception{
		result.println(sourceFileId + "," + scenario + "," + closeTime + "," + saveTime + "," + loadTime);
		log.info("Result [After Closing: " + closeTime + "] [After Saving: " + saveTime + "] [After Loading: " + loadTime + "]");
		if (closeTime < 0) {
			app.close();
		}
	}
	
	private PropertyValue propertyValue(String name, Object value) {
		PropertyValue p = new PropertyValue();
		p.Name = name;
		p.Value= value;
		return p;
	}
	
	@Test
	public void testConversion() throws Exception {
		// convert
		long start = System.currentTimeMillis();
		XComponent doc = app.loadDocumentFromURL(sourceFileUrl, 
				propertyValue("Hidden", true),
				propertyValue("ReadOnly", true),
				propertyValue("AsyncMode", false),
				propertyValue("MacroExecutionMode", MacroExecMode.NEVER_EXECUTE));

		loadTime = System.currentTimeMillis() - start;
		app.saveDocumentToURL(doc, targetFileUrl, 
				propertyValue( "FilterName", targetFilterName),
				propertyValue( "Overwrite", true));
		saveTime = System.currentTimeMillis() - start;
		XCloseable xCloseable = (XCloseable) UnoRuntime.queryInterface(XCloseable.class, doc);
		xCloseable.close(true);
		closeTime = System.currentTimeMillis() - start;
	}
}
