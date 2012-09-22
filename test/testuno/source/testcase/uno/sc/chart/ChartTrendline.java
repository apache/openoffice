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
import com.sun.star.chart.ChartRegressionCurveType;
import com.sun.star.chart.XChartDocument;
import com.sun.star.chart.XDiagram;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.CellRangeAddress;

/**
 *  Check trend line in chart can be applied and saved
 * 
 */
@RunWith(value = Parameterized.class)
public class ChartTrendline {

	private ChartRegressionCurveType expected;
	private ChartRegressionCurveType trendlineType;
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
			{ChartRegressionCurveType.NONE, ChartRegressionCurveType.NONE, "com.sun.star.chart.LineDiagram", numberData1, "ods"},
			{ChartRegressionCurveType.LINEAR, ChartRegressionCurveType.LINEAR, "com.sun.star.chart.LineDiagram", numberData1, "ods"},
			{ChartRegressionCurveType.LOGARITHM, ChartRegressionCurveType.LOGARITHM, "com.sun.star.chart.AreaDiagram", numberData1, "ods"},
			{ChartRegressionCurveType.EXPONENTIAL, ChartRegressionCurveType.EXPONENTIAL, "com.sun.star.chart.XYDiagram", numberData1, "ods"},
			{ChartRegressionCurveType.POWER, ChartRegressionCurveType.POWER, "com.sun.star.chart.BarDiagram", numberData1, "ods"},
			
			{ChartRegressionCurveType.NONE, ChartRegressionCurveType.NONE, "com.sun.star.chart.BarDiagram", numberData1, "xls"},
			{ChartRegressionCurveType.LINEAR, ChartRegressionCurveType.LINEAR, "com.sun.star.chart.XYDiagram", numberData1, "xls"},
			{ChartRegressionCurveType.LOGARITHM, ChartRegressionCurveType.LOGARITHM, "com.sun.star.chart.LineDiagram", numberData1, "xls"},
			{ChartRegressionCurveType.EXPONENTIAL, ChartRegressionCurveType.EXPONENTIAL, "com.sun.star.chart.AreaDiagram", numberData1, "xls"},
			{ChartRegressionCurveType.POWER, ChartRegressionCurveType.POWER, "com.sun.star.chart.BarDiagram", numberData1, "xls"}
		
		});
	}
	
	public ChartTrendline(ChartRegressionCurveType expected, ChartRegressionCurveType trendlineType, String inputType, double[][] numberData, String fileType) {
		this.expected = expected;
		this.trendlineType = trendlineType;
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
	 * Enable different types of trend line in chart.
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range and create a 2D chart.
	 * 3. Enable trend line in chart.
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the trend line setting.
	 * @throws Exception
	 */
	@Test
	public void testCreateTrendline() throws Exception {
		String fileName = "testCreateTrendline";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		ChartRegressionCurveType result = null;
		
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
		
		SCUtil.setProperties(xDiagram, "RegressionCurves", trendlineType);
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		xDiagram = xChartDocument.getDiagram(); 
		result = (ChartRegressionCurveType) SCUtil.getProperties(xDiagram, "RegressionCurves");

		SCUtil.closeFile(scDocument);
		
		assertEquals("Incorrect chart trendline got in ." + fileType + " file.", expected, result);

	}
	
}