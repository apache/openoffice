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

package testcase.uno.sc.chart;

import static org.junit.Assert.assertEquals;

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
import com.sun.star.chart.ChartErrorCategory;
import com.sun.star.chart.ChartErrorIndicatorType;
import com.sun.star.chart.XChartDocument;
import com.sun.star.chart.XDiagram;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.CellRangeAddress;

/**
 *  Check Y error bar in chart can be applied and saved
 * 
 */
@RunWith(value = Parameterized.class)
public class ChartYErrorBar {

	private ChartErrorCategory expCategory;
	private ChartErrorIndicatorType expIndicator;
	private ChartErrorCategory inputCategory;
	private ChartErrorIndicatorType inputIndicator;
	private String inputType;
	private double[][] numberData;	
	private String fileType;
	
	private static final UnoApp unoApp = new UnoApp();
	
	XComponent scComponent = null;
	XSpreadsheetDocument scDocument = null;
	
	@Parameters
	public static Collection<Object[]> data() throws Exception {
		double[][] numberData1 = {
				{10, 20, 30, 40},
				{20, 40.3, 50, 80},
				{40, 20, 30, 10},
				{10, -10, 0, -30}
		};

		return Arrays.asList(new Object[][] {
			{ChartErrorCategory.NONE, ChartErrorIndicatorType.NONE, ChartErrorCategory.NONE, ChartErrorIndicatorType.NONE, "com.sun.star.chart.BarDiagram", numberData1, "ods"},
			{ChartErrorCategory.VARIANCE, ChartErrorIndicatorType.TOP_AND_BOTTOM, ChartErrorCategory.VARIANCE, ChartErrorIndicatorType.TOP_AND_BOTTOM, "com.sun.star.chart.LineDiagram", numberData1, "ods"},
			{ChartErrorCategory.STANDARD_DEVIATION, ChartErrorIndicatorType.UPPER, ChartErrorCategory.STANDARD_DEVIATION, ChartErrorIndicatorType.UPPER, "com.sun.star.chart.AreaDiagram", numberData1, "ods"},
			{ChartErrorCategory.PERCENT, ChartErrorIndicatorType.LOWER, ChartErrorCategory.PERCENT, ChartErrorIndicatorType.LOWER, "com.sun.star.chart.BarDiagram", numberData1, "ods"},
			{ChartErrorCategory.ERROR_MARGIN, ChartErrorIndicatorType.TOP_AND_BOTTOM, ChartErrorCategory.ERROR_MARGIN, ChartErrorIndicatorType.TOP_AND_BOTTOM, "com.sun.star.chart.LineDiagram", numberData1, "ods"},
			{ChartErrorCategory.CONSTANT_VALUE, ChartErrorIndicatorType.UPPER, ChartErrorCategory.CONSTANT_VALUE, ChartErrorIndicatorType.UPPER, "com.sun.star.chart.AreaDiagram", numberData1, "ods"},
			
			{ChartErrorCategory.NONE, ChartErrorIndicatorType.NONE, ChartErrorCategory.NONE, ChartErrorIndicatorType.NONE, "com.sun.star.chart.BarDiagram", numberData1, "xls"},
//			{ChartErrorCategory.VARIANCE, ChartErrorIndicatorType.TOP_AND_BOTTOM, ChartErrorCategory.VARIANCE, ChartErrorIndicatorType.TOP_AND_BOTTOM, "com.sun.star.chart.LineDiagram", numberData1, "xls"},
			{ChartErrorCategory.STANDARD_DEVIATION, ChartErrorIndicatorType.UPPER, ChartErrorCategory.STANDARD_DEVIATION, ChartErrorIndicatorType.UPPER, "com.sun.star.chart.AreaDiagram", numberData1, "xls"},
			{ChartErrorCategory.PERCENT, ChartErrorIndicatorType.LOWER, ChartErrorCategory.PERCENT, ChartErrorIndicatorType.LOWER, "com.sun.star.chart.BarDiagram", numberData1, "xls"},
//			{ChartErrorCategory.ERROR_MARGIN, ChartErrorIndicatorType.TOP_AND_BOTTOM, ChartErrorCategory.ERROR_MARGIN, ChartErrorIndicatorType.TOP_AND_BOTTOM, "com.sun.star.chart.AreaDiagram", numberData1, "xls"},
			{ChartErrorCategory.CONSTANT_VALUE, ChartErrorIndicatorType.UPPER, ChartErrorCategory.CONSTANT_VALUE, ChartErrorIndicatorType.UPPER, "com.sun.star.chart.LineDiagram", numberData1, "xls"}
		
		});
	}
	
	public ChartYErrorBar(ChartErrorCategory expCategory, ChartErrorIndicatorType expIndicator, ChartErrorCategory inputCategory, ChartErrorIndicatorType inputIndicator, String inputType, double[][] numberData, String fileType) {
		this.expCategory = expCategory;
		this.expIndicator = expIndicator;
		this.inputCategory = inputCategory;
		this.inputIndicator = inputIndicator;
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
	 * Enable different types of Y error bar in chart.
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range and create a chart.
	 * 3. Enable Y error bar in chart.
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the Y error bar setting.
	 * @throws Exception
	 */
	@Test
	public void testCreateYErrorBar() throws Exception {
		String fileName = "testCreateYErrorBar";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		ChartErrorCategory result1 = null;
		ChartErrorIndicatorType result2 = null;
		
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
		XDiagram xDiagram = xChartDocument.getDiagram(); 
		
		SCUtil.setProperties(xDiagram, "ErrorCategory", inputCategory);
		SCUtil.setProperties(xDiagram, "ErrorIndicator", inputIndicator);
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		xDiagram = xChartDocument.getDiagram(); 
		result1 = (ChartErrorCategory) SCUtil.getProperties(xDiagram, "ErrorCategory");
		result2 = (ChartErrorIndicatorType) SCUtil.getProperties(xDiagram, "ErrorIndicator");
		
		SCUtil.closeFile(scDocument);
		
		assertEquals("Incorrect chart Y error bar category got in ." + fileType + " file.", expCategory, result1);
		assertEquals("Incorrect chart Y error bar indicator got in ." + fileType + " file.", expIndicator, result2);

	}
	
}