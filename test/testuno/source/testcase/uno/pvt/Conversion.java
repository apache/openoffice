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

import static org.openoffice.test.common.Testspace.*;

import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintStream;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.openoffice.test.OpenOffice;
import org.openoffice.test.common.FileProvider;
import org.openoffice.test.common.FileProvider.FileFilter;
import org.openoffice.test.common.FileProvider.FileRepos;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import com.sun.star.beans.PropertyValue;
import com.sun.star.document.MacroExecMode;
import com.sun.star.lang.XComponent;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.XCloseable;


@RunWith(FileProvider.class)
public class Conversion {

	@Rule
	public Logger log = Logger.getLogger(this);

	@FileRepos
	public static String repos = getDataFile("pvt_conversion").getAbsolutePath();

	@FileFilter
	public static String filter = "-f .*\\.((doc)|(dot)|(odt)|(ott))$ writer_pdf_Export pdf " 
			+ "-f .*\\.((xls)|(xlt)|(ods)|(ots))$ calc_pdf_Export pdf "
			+ "-f .*\\.((ppt)|(ppt)|(odp)|(otp))$ impress_pdf_Export pdf " 
			+ "-f .*\\.((doc)|(dot)|(docx)|(docm)|(dotx)|(dotm))$ writer8 odt "
			+ "-f .*\\.((xls)|(xlt)|(xlsx)|(xltx)|(xlsm)|(xltm))$ calc8 ods " 
			+ "-f .*\\.((ppt)|(pot)|(pptx)|(pptm)|(potm)|(potx))$ impress8 odp "
			+ "-f .*\\.((odt)|(ott))$ 'MS Word 97' doc " 
			+ "-f .*\\.((ods)|(ots))$ 'MS Excel 97' xls " 
			+ "-f .*\\.((odp)|(otp))$ 'MS PowerPoint 97' ppt";

	private static UnoApp app = new UnoApp();
	
	private static PrintStream result;
	
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
	
	private String sourcePath = null;
	private String targetFilterName = null;
	private String targetExtName = null;
	
	private File sourceFile = null;
	private File targetFile = null;
	private String sourceFileUrl = null;
	private String targetFileUrl = null;

	private String scenario = null;
	private String sourceFileId = null;
	private long loadTime = -1;
	private long saveTime = -1;
	private long closeTime = -1;
	
	public Conversion(String sourcePath, String targetFilterName, String targetExtName) {
		super();
		this.sourcePath = sourcePath;
		this.targetFilterName = targetFilterName;
		this.targetExtName = targetExtName;
	}
	
	@Before
	public void before() throws Exception {
		sourceFile = prepareDataFile(sourcePath);
		sourceFileUrl = FileUtil.getUrl(this.sourceFile);
		targetFile = getFile("classtemp/" + sourceFile.getName()+ "." + targetExtName);
		targetFileUrl = FileUtil.getUrl(this.targetFile);
		
		scenario = FileUtil.getFileExtName(sourceFile.getName()).toLowerCase() + " to " + FileUtil.getFileExtName(targetFile.getName()).toLowerCase();
		sourceFileId = sourceFile.getAbsolutePath().replace(new File(repos).getAbsolutePath(), "").replace("\\", "/");
		
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
	
	@Test(timeout=10 * 60000)
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
