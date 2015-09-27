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

package fvt.uno.sc.chart;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SCUtil;

import com.sun.star.awt.Rectangle;
import com.sun.star.chart.XChartDocument;
import com.sun.star.drawing.XShape;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.CellRangeAddress;

/**
 *  Check the chart title/subtitle can be created, modified and saved
 * 
 */
@RunWith(value = Parameterized.class)
public class ChartTitle {

	private String[] titles;
	private String inputType;
	private double[][] numberData;
	private String fileType;
	
	private static final UnoApp unoApp = new UnoApp();
	
	XComponent scComponent = null;
	XSpreadsheetDocument scDocument = null;
	
	@Parameters
	public static Collection<Object[]> data() throws Exception {
		double[][] numberData1 = {
				{1, 2, 3, 4},
				{2, 4.3, 5, 8},
				{4, 2, 3, 1},
				{1, -1, 0, -3}
		};
		String[][] titles = {
				{"MyMainTitle", "MySubTitle"},
				{"A Main Title With Space", "   Sub Title "},
				{"  ", "      "}			
		};

		return Arrays.asList(new Object[][] {
			{titles[0], "com.sun.star.chart.BarDiagram", numberData1, "ods"},
			{titles[1], "com.sun.star.chart.NetDiagram", numberData1, "ods"},
			{titles[2], "com.sun.star.chart.AreaDiagram", numberData1, "ods"},
			{titles[0], "com.sun.star.chart.PieDiagram", numberData1, "xls"},
			{titles[1], "com.sun.star.chart.NetDiagram", numberData1, "xls"},
			{titles[2], "com.sun.star.chart.XYDiagram", numberData1, "xls"}
		});
	}
	
	public ChartTitle(String[] titles, String inputType, double[][] numberData, String fileType) {
		this.titles = titles;
		this.inputType = inputType;
		this.numberData = numberData;
		this.fileType = fileType;
	}
	
	
	@Before
	public void setUp() throws Exception {
		scComponent = unoApp.newDocument("scalc");
		scDocument = SCUtil.getSCDocument(scComponent);
	}

	@After
	public void tearDown() throws Exception {
		unoApp.closeDocument(scComponent);
		
	}
	
	@BeforeClass
	public static void setUpConnection() throws Exception {
		unoApp.start();
	}

	@AfterClass
	public static void tearDownConnection() throws InterruptedException, Exception {
		unoApp.close();
		SCUtil.clearTempDir();	
	}
	
	/**
	 * Create main title in chart.
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range and create a chart.
	 * 3. Create main title in chart.
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the chart main title.
	 * @throws Exception
	 */
	@Test
	public void testCreateMainTitle() throws Exception {
		String fileName = "testCreateMainTitle";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		Boolean result = null;	
		String resultTitle = null;
		String defaultTitle = null;
		
		if (inputType.equals("com.sun.star.chart.StockDiagram")) {
			cellRangeName = "A1:C4";
		}	
		if (fileType.equalsIgnoreCase("xls")) {
			chartName = "Object 1";			
		}
		
		XSpreadsheet sheet = SCUtil.getCurrentSheet(scDocument);
		
		SCUtil.setValueToCellRange(sheet, 0, 0, numberData);

		CellRangeAddress[] cellAddress = new CellRangeAddress[1];
		cellAddress[0] = SCUtil.getChartDataRangeByName(sheet, cellRangeName);
		Rectangle rectangle = new Rectangle(1000, 1000, 15000, 9500);
		XChartDocument xChartDocument = null; 		
		xChartDocument = SCUtil.createChart(sheet, rectangle, cellAddress, chartName);
		SCUtil.setChartType(xChartDocument, inputType);
		
		result = (Boolean) SCUtil.getProperties(xChartDocument, "HasMainTitle");
		if (!result) {
			SCUtil.setProperties(xChartDocument, "HasMainTitle", true);
		}
		XShape aTitle = xChartDocument.getTitle();
		defaultTitle = (String) SCUtil.getProperties(aTitle, "String");
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		result = (Boolean) SCUtil.getProperties(xChartDocument, "HasMainTitle");
		resultTitle = (String) SCUtil.getProperties(xChartDocument.getTitle(), "String");		
		SCUtil.closeFile(scDocument);
		
		assertTrue("Chart title has not be created in ." + fileType + " file.", result);
		assertEquals("Incorrect chart title got in ." + fileType + " file.", defaultTitle, resultTitle);

	}
	
	/**
	 * Create sub title in chart.
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range and create a chart.
	 * 3. Create subtitle in chart.
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the chart main title.
	 * @throws Exception
	 */
	@Test
	public void testCreateSubTitle() throws Exception {
		String fileName = "testCreateSubTitle";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		Boolean result = null;	
		String resultTitle = null;
		String defaultTitle = null;
		
		if (inputType.equals("com.sun.star.chart.StockDiagram")) {
			cellRangeName = "A1:C4";
		}	
		if (fileType.equalsIgnoreCase("xls")) {
			chartName = "Object 1";			
		}
		
		XSpreadsheet sheet = SCUtil.getCurrentSheet(scDocument);
		
		SCUtil.setValueToCellRange(sheet, 0, 0, numberData);

		CellRangeAddress[] cellAddress = new CellRangeAddress[1];
		cellAddress[0] = SCUtil.getChartDataRangeByName(sheet, cellRangeName);
		Rectangle rectangle = new Rectangle(1000, 1000, 15000, 9500);
		XChartDocument xChartDocument = null; 		
		xChartDocument = SCUtil.createChart(sheet, rectangle, cellAddress, chartName);
		SCUtil.setChartType(xChartDocument, inputType);
		result = (Boolean) SCUtil.getProperties(xChartDocument, "HasSubTitle");
		if (!result) {
			SCUtil.setProperties(xChartDocument, "HasSubTitle", true);
		}
		XShape aSubTitle = xChartDocument.getSubTitle();
		defaultTitle = (String) SCUtil.getProperties(aSubTitle, "String");
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		result = (Boolean) SCUtil.getProperties(xChartDocument, "HasSubTitle");
		resultTitle = (String) SCUtil.getProperties(xChartDocument.getSubTitle(), "String");		
		SCUtil.closeFile(scDocument);
		
		
		if (fileType.equalsIgnoreCase("xls")) {
			assertFalse("Chart subtitle should not be saved in ." + fileType + " file.", result);
		}
		else {
			assertTrue("Chart subtitle has not be created in ." + fileType + " file.", result);
			assertEquals("Incorrect chart subtitle got in ." + fileType + " file.", defaultTitle, resultTitle);
		}	

	}
	
	/**
	 * Create titles in chart and change title string.
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range and create a chart.
	 * 3. Create main title and subtitle in chart, input customized string in title box.
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the chart titles.
	 * @throws Exception
	 */
	@Test
	public void testInputTitles() throws Exception {
		String fileName = "testInputTitles";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		Boolean[] result = new Boolean[2];	
		String[] resultTitle = new String[2];
		
		if (inputType.equals("com.sun.star.chart.StockDiagram")) {
			cellRangeName = "A1:C4";
		}	
		if (fileType.equalsIgnoreCase("xls")) {
			chartName = "Object 1";			
		}
		
		XSpreadsheet sheet = SCUtil.getCurrentSheet(scDocument);
		
		SCUtil.setValueToCellRange(sheet, 0, 0, numberData);

		CellRangeAddress[] cellAddress = new CellRangeAddress[1];
		cellAddress[0] = SCUtil.getChartDataRangeByName(sheet, cellRangeName);
		Rectangle rectangle = new Rectangle(1000, 1000, 15000, 9500);
		XChartDocument xChartDocument = null; 		
		xChartDocument = SCUtil.createChart(sheet, rectangle, cellAddress, chartName);
		SCUtil.setChartType(xChartDocument, inputType);
		SCUtil.setProperties(xChartDocument, "HasMainTitle", true);
		SCUtil.setProperties(xChartDocument, "HasSubTitle", true);
		
		SCUtil.setProperties(xChartDocument.getTitle(), "String", titles[0]);
		SCUtil.setProperties(xChartDocument.getSubTitle(), "String", titles[1]);
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		result[0] = (Boolean) SCUtil.getProperties(xChartDocument, "HasMainTitle");
		result[1] = (Boolean) SCUtil.getProperties(xChartDocument, "HasSubTitle");
		resultTitle[0] = (String) SCUtil.getProperties(xChartDocument.getTitle(), "String");	
		resultTitle[1] = (String) SCUtil.getProperties(xChartDocument.getSubTitle(), "String");		
		SCUtil.closeFile(scDocument);
		
		assertTrue("Chart main title has not be created in ." + fileType + " file.", result[0]);
		assertEquals("Incorrect chart title got in ." + fileType + " file.", titles[0], resultTitle[0]);
		if (fileType.equalsIgnoreCase("xls")) {
			assertFalse("Chart subtitle should not be saved in ." + fileType + " file.", result[1]);
		}
		else {
			assertTrue("Chart subtitle has not be created in ." + fileType + " file.", result[1]);
			assertEquals("Incorrect chart subtitle got in ." + fileType + " file.", titles[1], resultTitle[1]);
		}	

	}
	
}