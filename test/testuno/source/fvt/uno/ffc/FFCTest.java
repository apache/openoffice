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
 */
package fvt.uno.ffc;

import java.io.File;
import java.io.FilenameFilter;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import junit.framework.Assert;

import org.junit.After;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.openoffice.test.OpenOffice;
import org.openoffice.test.common.FileProvider;
import org.openoffice.test.common.FileProvider.FileRepos;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.SystemUtil;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import com.sun.star.beans.PropertyValue;
import com.sun.star.document.MacroExecMode;
import com.sun.star.frame.XComponentLoader;
import com.sun.star.frame.XStorable;
import com.sun.star.io.IOException;
import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.lang.XComponent;
import com.sun.star.uno.UnoRuntime;
/**
 * Pls place a suite file in directory "suite" which is same level with test uno. like bewlow
 * -suite
 * -testuno
 * The suite file content is like this format
 * ftp://user:password@192.168.0.1/public/sample/testsample.doc
 * ..
 * ..
 * ftp://user:password@192.168.0.1/public/sample/testsample2.doc
 *This script is used to test FFC by UNO API
 *It cover below scenario:
 *MS2003/2010 format->ODF format
 *New Saved ODF Format file -> MS 2003 Format
 *New Saved ODF Format file -> PDF
 *
 */
//@RunWith(FileProvider.class)
public class FFCTest {
	@Rule
	public Logger log = Logger.getLogger(this, false);
	
	
	private static  UnoApp app = null;
	private static Map<String, String> filterMap = new HashMap<String, String>();
	@FileRepos
//	public static String suiteDir = "../suite/";
	private String fileURL = "";
	private String operateFilePath = "";
	private static Map<String, String> formatMap = new HashMap<String, String>();
	private static File testSpaceFile = Testspace.getFile();
	private boolean isSucceed = false;
	private static String  tempFolder = testSpaceFile.getAbsolutePath() + File.separator + "temp";

	private static String failedFilesDir  = "output/failedSampleFiles/";;
//	@Parameters
//	public static Collection<String[]>  data() throws Exception{
//		initMap();
//		ArrayList<String[]> list = new ArrayList<String[]>();
//		List<String> suitePathList = new ArrayList<String>();
//		FileReader fileReader = null;
//		BufferedReader reader = null;
//		File suites = new File(suiteDir);
//		if (suites.exists() && suites.list().length > 0) {
//			isSuiteFileExist = true;
//			for(File file: suites.listFiles()){
//				if(FileUtil.getFileExtName(file.getName()).toLowerCase().equals("suite")){
//					suitePathList.add(file.getAbsolutePath());
//				}
//			}
//			try{
//				for (String suitePath : suitePathList) {
//					fileReader = new FileReader(suitePath);			
//					reader = new BufferedReader(fileReader);
//					String line = null;
//					while((line = reader.readLine()) != null){
//						if (!"".equals(line)) {
//							list.add(new String[]{line});
//						}
//					} 
//					if(reader != null){
//						reader.close();
//						reader = null;
//					}
//					if(fileReader != null){
//						fileReader.close();
//						fileReader = null;					
//					}
//				}
//			
//			}catch(Exception e){
//				throw new Exception("throw exception when read suite file. " + e.getMessage());			
//			}finally{
//				try{
//					if(reader != null){
//						reader.close();
//						reader = null;
//					}
//					if(fileReader != null){
//						fileReader.close();
//						fileReader = null;					
//					}
//				}catch(Exception io){
//				}
//			}
//		} else {// run files from ffc data directory
//			File ffcDataHome = new File("data\\ffc");
//			getFileList(ffcDataHome, list);
//		}
//	
//		return list;
//	}
//	
	public static void getFileList(File dir, List<String[]> list) {
		File[] files = dir.listFiles(new FilenameFilter() {
			@Override
			public boolean accept(File dir, String name) {
				File file =  new File(dir.getAbsolutePath() + File.separator + name);
				String filename = new File(name).getName().toLowerCase(); 
				boolean accept;
				if (file.isDirectory()) {
					accept = true;
				} else {
					accept =  filename.endsWith(".docx")
					|| filename.endsWith(".pptx")
					|| filename.endsWith(".xlsx")
					|| filename.endsWith(".ppt")
					|| filename.endsWith(".xls")
					|| 	filename.endsWith(".doc") ;
				}
				
				return accept;
			}
			
		});
		if (files == null)
			return;

		for (File file : files) {
			if (file.isDirectory()) {
				getFileList(file, list);
			} else {
				list.add(new String[] {file.getAbsolutePath().replace(dir.getParentFile().getAbsolutePath(), "").replace("\\", "/")});
			
			}
		}
	}
	
	public FFCTest(String url) {
		this.fileURL = url;
	}
	
	@BeforeClass
	public static void init() {
		initMap();
		
		//Disable automation
		
		OpenOffice defaultOpenOffice = new OpenOffice();
		defaultOpenOffice.addArgs("-nofirststartwizard", "-norestore", "-quickstart=no");
		defaultOpenOffice.setUnoUrl(OpenOffice.DEFAULT_UNO_URL);
		defaultOpenOffice.addArgs("-invisible", "-conversionmode", "-headless", "-hidemenu");
		app = new UnoApp(defaultOpenOffice);
		
		File failedDirec = Testspace.getFile(failedFilesDir);
		failedDirec.mkdirs();
	}
	
	@Before
	public void setUp() throws Exception {
		operateFilePath =  Testspace.prepareData(fileURL);
				
	
		app.start();	
	}
	@After
	public void tearDown() throws Exception {			
		if (!isSucceed) {
			FileUtil.copyFile(operateFilePath, Testspace.getFile(failedFilesDir).getAbsolutePath());
			FileUtil.appendStringToFile( Testspace.getFile(failedFilesDir + File.separator + "failedFiles.files").getAbsolutePath(), fileURL +"\r\n");
			app.close();
			SystemUtil.killProcess("WerFault.*");
			SystemUtil.sleep(2);
			SystemUtil.killProcess("EQNEDT32.*");
			//WerFault.exe
			//EQNEDT32.EXE
		}
	}
	
	
	
//	@Test(timeout=1000*60*10)
//	public void exportTest() throws Exception {
//		//MS Office Format ->ODF
//		boolean flag = false;
//		
//		String saveAsODF = exportAsODF(operateFilePath);
//		System.out.println("MS ->ODF finished");
//		//ODF->MS
//		String savedMSFilePath = exportAsODF(saveAsODF); 
//		File savedMSFile = new File(savedMSFilePath);
//		Assert.assertTrue("FFC Test for file : "+ savedMSFilePath, savedMSFile.exists());
//		System.out.println("ODF->MS Finished");
//		
//
//		//Export ODF->PDF
//		exportAsPDF(saveAsODF);
//		System.out.println("ODF->PDF Finished");
//		flag = true;
//		Assert.assertTrue("FFC Test for file : "+ operateFilePath, flag);
//		isSucceed = true;
//	}
	private String getSuffix(String file) {
		String lowerCaseName = file.toLowerCase();
		String suffix = lowerCaseName.substring(lowerCaseName.lastIndexOf("."));
		return suffix;
	}
	/**
	 * return the Export ODF file path
	 * @throws IOException
	 * @throws IllegalArgumentException
	 */
	private String exportAsODF(String testFile) throws IOException, IllegalArgumentException {
		XComponent document = loadSampleFile(testFile);
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		String suffix = getSuffix(testFile);
		String filterName = filterMap.get(suffix);
		PropertyValue[] lProperties = null;
		lProperties = new PropertyValue[3];
		lProperties[0] = new PropertyValue();
		lProperties[0].Name = "FilterName";
		lProperties[0].Value = filterName;
		lProperties[1] = new PropertyValue();
		lProperties[1].Name = "Overwrite";
		lProperties[1].Value = Boolean.TRUE;
		lProperties[2] = new PropertyValue();
		lProperties[2].Name = "AsyncMode";
		lProperties[2].Value = new Boolean(false);
		
		XStorable store = (XStorable) UnoRuntime.queryInterface(XStorable.class, document);
		File file = new File(testFile);
		String fileName = file.getName();
		String saveAsFilePath = file.getParentFile().getAbsolutePath() + File.separator + fileName + "." + formatMap.get(suffix);//TODO
		store.storeAsURL(Testspace.getUrl(saveAsFilePath), lProperties);
		try {
			Thread.sleep(3000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		app.closeDocument(document);
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		return saveAsFilePath;
	}
	
	private void exportAsPDF(String testFilePath) throws Exception {
		XComponent xComponent = loadSampleFile(testFilePath);
		XStorable xStorable = (XStorable) UnoRuntime.queryInterface(
				XStorable.class, xComponent);
	
		PropertyValue[] aMediaDescriptor = new PropertyValue[1];
		aMediaDescriptor[0] = new PropertyValue();
		aMediaDescriptor[0].Name = "FilterName";
		aMediaDescriptor[0].Value = "writer_pdf_Export";
		File file = new File(testFilePath); 
		String fileName = file.getName();
		String saveAsFilePath =  file.getParentFile().getAbsolutePath() + File.separator + fileName + ".pdf" ;
		// export to pdf
		xStorable.storeToURL(Testspace.getUrl(saveAsFilePath), aMediaDescriptor);
		try {
			Thread.sleep(5000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		// close this document
		app.closeDocument(xComponent);
		File pdfFile = new File(saveAsFilePath);
		Assert.assertTrue("Verify sampe file " + testFilePath + " exprot to pdf!", pdfFile.exists());
	}
	
	public static void initMap() {
		filterMap.put(".doc", "writer8");
		filterMap.put(".docx", "writer8");
		filterMap.put(".odt", "MS Word 97");
		filterMap.put(".ppt", "impress8");
		filterMap.put(".pptx", "impress8");
		filterMap.put(".odp", "MS PowerPoint 97");
		filterMap.put(".xls", "calc8");
		filterMap.put(".xlsx", "calc8");
		filterMap.put(".ods", "MS Excel 97");
		
		formatMap.put(".doc", "odt");
		formatMap.put(".docx", "odt");
		formatMap.put(".odt", "doc");
		
		formatMap.put(".ppt", "odp");
		formatMap.put(".pptx", "odp");
		formatMap.put(".odp", "ppt");
		
		formatMap.put(".xls", "ods");
		formatMap.put(".xlsx", "ods");
		formatMap.put(".ods", "xls");
	}
	private XComponent loadSampleFile(String filePath) throws IOException, IllegalArgumentException {
		if (!"".equals(filePath)) {
			PropertyValue[] loadProps = null;
			if (filePath.endsWith("x")) {//ooxml sample file
				loadProps = new PropertyValue[4];
				loadProps[0] = new PropertyValue();
				loadProps[0].Name = "Hidden";
				loadProps[0].Value = Boolean.TRUE;
				loadProps[1] = new PropertyValue();
				loadProps[1].Name = "FilterName";
				String filePathLowCase = filePath.toLowerCase();
				if(filePathLowCase.endsWith("docx")) {	
					loadProps[1].Value = "MS Word 2007 XML";
				}
				if(filePathLowCase.endsWith("pptx")){
					loadProps[1].Value = "MS PowerPoint 2007 XML";
				}
				if(filePathLowCase.endsWith("xlsx")) {
					loadProps[1].Value = "MS Excel 2007 XML";
				}
				loadProps[2] = new PropertyValue();
				loadProps[2].Name = "ReadOnly";
				loadProps[2].Value = true;
				loadProps[3] = new PropertyValue();
				loadProps[3].Name = "MacroExecutionMode";
				loadProps[3].Value = MacroExecMode.NEVER_EXECUTE;
			} else {
				loadProps = new PropertyValue[3];
				loadProps[0] = new PropertyValue();
				loadProps[0].Name = "Hidden";
				loadProps[0].Value = Boolean.TRUE;
				loadProps[1] = new PropertyValue();
				loadProps[1].Name = "ReadOnly";
				loadProps[1].Value = Boolean.TRUE;
				loadProps[2] = new PropertyValue();
				loadProps[2].Name = "AsyncMode";
				loadProps[2].Value = new Boolean(false);
			}
			 
			String urlPath = Testspace.getUrl(filePath);
			XComponentLoader componentLoader = (XComponentLoader) UnoRuntime.queryInterface(XComponentLoader.class, app.getDesktop());
			return componentLoader.loadComponentFromURL(urlPath, "_blank", 0, loadProps);
		}
		return null;
	}
	/**
	 * the url is like this format:
	 * ftp://user:password@192.168.0.1/public/sample/testsample.doc
	 * @param url
	 * @return
	 */
	public String downloadFile(String url) {
		File urlFile = new File( new File(url.replaceAll("%20", " ")).getName());
		
		File tempFolderFile = new File(tempFolder);
		if (!tempFolderFile.exists()) {
			tempFolderFile.mkdir();
		}
		String testFile = testSpaceFile.getAbsolutePath() + File.separator + "temp" + File.separator + urlFile.getName();
		FileUtil.download(url, new File(testFile));
		return testFile;
	}
	
	
	
}
