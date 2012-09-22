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

import static org.junit.Assert.assertArrayEquals;

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
import com.sun.star.chart.XDiagram;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.CellRangeAddress;

/**
 *  Check Grids in chart can be applied and saved
 * 
 */
@RunWith(value = Parameterized.class)
public class ChartGrid {

	private Boolean[] majorGrids;
	private Boolean[] minorGrids;
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
		Boolean[][] gridsList = {
				{false, false, false}, //[0] no grid
				{true, false, false}, // [1] X 
				{false, true, false}, // [2] Y
				{true, true, false}, // [3] X & Y
				{true, true, true}, // [4] X & Y & Z
				{false, false, true}, // [5] Z
				{false, true, true}, // [6] Y & Z
				{true, false, true} // [7] X & Z
		};

		return Arrays.asList(new Object[][] {
			{gridsList[0], null, "com.sun.star.chart.BarDiagram", numberData1, "ods"},
			{gridsList[2], gridsList[3], "com.sun.star.chart.BarDiagram", numberData1, "ods"},
			{gridsList[2], gridsList[4], "com.sun.star.chart.BarDiagram", numberData1, "ods"},
			{gridsList[6], gridsList[7], "com.sun.star.chart.BarDiagram", numberData1, "ods"},
			{gridsList[1], gridsList[5], "com.sun.star.chart.BarDiagram", numberData1, "ods"},
			{gridsList[4], gridsList[4], "com.sun.star.chart.BarDiagram", numberData1, "ods"},
			
			{gridsList[0], null, "com.sun.star.chart.BarDiagram", numberData1, "xls"},
			{gridsList[2], gridsList[3], "com.sun.star.chart.BarDiagram", numberData1, "xls"}
//			{null, gridsList[4], "com.sun.star.chart.BarDiagram", numberData1, "xls"},
//			{gridsList[6], gridsList[7], "com.sun.star.chart.BarDiagram", numberData1, "xls"},
//			{gridsList[1], gridsList[5], "com.sun.star.chart.BarDiagram", numberData1, "xls"},
//			{gridsList[4], gridsList[4], "com.sun.star.chart.BarDiagram", numberData1, "xls"}
		
		});
	}
	
	public ChartGrid(Boolean[] majorGrids, Boolean[] minorGrids, String inputType, double[][] numberData, String fileType) {
		this.majorGrids = majorGrids;
		this.minorGrids = minorGrids;
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
	 * Enable different types of grids in chart.
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range and create a 2D chart.
	 * 3. Enable grids of X/Y axis in chart.
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the grid setting.
	 * @throws Exception
	 */
	@Test
	public void testCreateXYGrid() throws Exception {
		String fileName = "testCreateXYGrid";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		Boolean[][] expected = {
				{false, true, false},
				{false, false, false}
		};
		Boolean[][] results = {
				{false, false, false},
				{false, false, false}
		};	
		
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
		
		if (majorGrids != null) {
			SCUtil.setProperties(xDiagram, "HasXAxisGrid", majorGrids[0]);
			SCUtil.setProperties(xDiagram, "HasYAxisGrid", majorGrids[1]);
			expected[0][0] = majorGrids[0];
			expected[0][1] = majorGrids[1];
		}
		if (minorGrids != null) {
			SCUtil.setProperties(xDiagram, "HasXAxisHelpGrid", minorGrids[0]);
			SCUtil.setProperties(xDiagram, "HasYAxisHelpGrid", minorGrids[1]);
			expected[1][0] = minorGrids[0];
			expected[1][1] = minorGrids[1];
		}
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		xDiagram = xChartDocument.getDiagram(); 
		results[0][0] = (Boolean) SCUtil.getProperties(xDiagram, "HasXAxisGrid");
		results[0][1] = (Boolean) SCUtil.getProperties(xDiagram, "HasYAxisGrid");
		results[1][0] = (Boolean) SCUtil.getProperties(xDiagram, "HasXAxisHelpGrid");
		results[1][1] = (Boolean) SCUtil.getProperties(xDiagram, "HasYAxisHelpGrid");
		
		SCUtil.closeFile(scDocument);
		
		assertArrayEquals("Incorrect chart grids got in ." + fileType + " file.", expected, results);

	}
	
	/** 
	 * Enable different types of grids in chart.
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range and create a 3D chart.
	 * 3. Enable grids of X/Y/Z axes in chart.
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the grid setting.
	 * @throws Exception
	 */
	@Test
	public void testCreateXYZGrid() throws Exception {
		String fileName = "testCreateXYZGrid";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		Boolean[][] expected = {
				{false, true, false},
				{false, false, false}
		};
		Boolean[][] results = {
				{false, false, false},
				{false, false, false}
		};	
		
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
		SCUtil.setProperties(xChartDocument.getDiagram(), "Dim3D", true);
		XDiagram xDiagram = xChartDocument.getDiagram();
		
		if (majorGrids != null) {
			SCUtil.setProperties(xDiagram, "HasXAxisGrid", majorGrids[0]);
			SCUtil.setProperties(xDiagram, "HasYAxisGrid", majorGrids[1]);
			SCUtil.setProperties(xDiagram, "HasZAxisGrid", majorGrids[2]);
			expected[0][0] = majorGrids[0];
			expected[0][1] = majorGrids[1];
			expected[0][2] = majorGrids[2];
		}
		if (minorGrids != null) {
			SCUtil.setProperties(xDiagram, "HasXAxisHelpGrid", minorGrids[0]);
			SCUtil.setProperties(xDiagram, "HasYAxisHelpGrid", minorGrids[1]);
			SCUtil.setProperties(xDiagram, "HasZAxisHelpGrid", minorGrids[2]);
			expected[1][0] = minorGrids[0];
			expected[1][1] = minorGrids[1];
			expected[1][2] = minorGrids[2];
		}

		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		xDiagram = xChartDocument.getDiagram(); 
		results[0][0] = (Boolean) SCUtil.getProperties(xDiagram, "HasXAxisGrid");
		results[0][1] = (Boolean) SCUtil.getProperties(xDiagram, "HasYAxisGrid");
		results[0][2] = (Boolean) SCUtil.getProperties(xDiagram, "HasZAxisGrid");
		results[1][0] = (Boolean) SCUtil.getProperties(xDiagram, "HasXAxisHelpGrid");
		results[1][1] = (Boolean) SCUtil.getProperties(xDiagram, "HasYAxisHelpGrid");
		results[1][2] = (Boolean) SCUtil.getProperties(xDiagram, "HasZAxisHelpGrid");

		SCUtil.closeFile(scDocument);
		
		assertArrayEquals("Incorrect chart grids got in ." + fileType + " file.", expected, results);

	}
	
}