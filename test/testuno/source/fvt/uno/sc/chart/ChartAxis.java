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
 *  Check Axis in chart can be applied and saved
 * 
 */
@RunWith(value = Parameterized.class)
public class ChartAxis {

	private Boolean[] axes;
	private Boolean[] secondaryAxes;
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
		Boolean[][] axesList = {
				{false, false, false}, //[0] no Axis
				{true, false, false}, // [1] X 
				{false, true, false}, // [2] Y
				{true, true, false}, // [3] X & Y
				{true, true, true}, // [4] X & Y & Z
				{false, false, true}, // [5] Z
				{false, true, true}, // [6] Y & Z
				{true, false, true} // [7] X & Z
		};

		return Arrays.asList(new Object[][] {
//			{axesList[0], null, "com.sun.star.chart.BarDiagram", numberData1, "ods"}, //Bug #121046
//			{axesList[2], axesList[3], "com.sun.star.chart.BarDiagram", numberData1, "ods"}, //Bug #121046
			{null, axesList[4], "com.sun.star.chart.BarDiagram", numberData1, "ods"},
//			{axesList[6], axesList[7], "com.sun.star.chart.BarDiagram", numberData1, "ods"}, //Bug #121046
			{axesList[1], axesList[5], "com.sun.star.chart.AreaDiagram", numberData1, "ods"},
			{axesList[4], axesList[4], "com.sun.star.chart.LineDiagram", numberData1, "ods"},
			
//			{axesList[0], axesList[0], "com.sun.star.chart.BarDiagram", numberData1, "xls"}, //Bug #121043
//			{axesList[2], axesList[3], "com.sun.star.chart.BarDiagram", numberData1, "xls"}, //Bug #121043
			{null, axesList[4], "com.sun.star.chart.BarDiagram", numberData1, "xls"},
			{axesList[6], axesList[7], "com.sun.star.chart.BarDiagram", numberData1, "xls"},
			{axesList[1], axesList[5], "com.sun.star.chart.AreaDiagram", numberData1, "xls"},
			{axesList[4], axesList[4], "com.sun.star.chart.LineDiagram", numberData1, "xls"}
		
		});
	}
	
	public ChartAxis(Boolean[] axes, Boolean[] secondaryAxes, String inputType, double[][] numberData, String fileType) {
		this.axes = axes;
		this.secondaryAxes = secondaryAxes;
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
	 * Enable different types of axes in chart.
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range and create a 2D chart.
	 * 3. Enable X/Y axis/secondaryAxis in chart.
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the axes setting.
	 * @throws Exception
	 */
	@Test
	public void testCreateXYAxes() throws Exception {
		String fileName = "testCreateXYAxes";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		Boolean[][] expected = {
				{true, true, false},
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
		
		if (axes != null) {
			SCUtil.setProperties(xDiagram, "HasXAxis", axes[0]);
			SCUtil.setProperties(xDiagram, "HasYAxis", axes[1]);
			expected[0][0] = axes[0];
			expected[0][1] = axes[1];
		}
		if (secondaryAxes != null) {
			SCUtil.setProperties(xDiagram, "HasSecondaryXAxis", secondaryAxes[0]);
			SCUtil.setProperties(xDiagram, "HasSecondaryYAxis", secondaryAxes[1]);
			expected[1][0] = secondaryAxes[0];
			expected[1][1] = secondaryAxes[1];
		}
		
		//Excel does not has secondary X axis, the value depends on the secondary Y axis setting
		if (fileType.equalsIgnoreCase("xls")) {
			expected[1][0] = false;			
			if (expected[1][1]) {
				expected[1][0] = true;
				SCUtil.setProperties(xDiagram.getDataRowProperties(1), "Axis", 4);
			}
		}
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		xDiagram = xChartDocument.getDiagram(); 
		results[0][0] = (Boolean) SCUtil.getProperties(xDiagram, "HasXAxis");
		results[0][1] = (Boolean) SCUtil.getProperties(xDiagram, "HasYAxis");
		results[1][0] = (Boolean) SCUtil.getProperties(xDiagram, "HasSecondaryXAxis");
		results[1][1] = (Boolean) SCUtil.getProperties(xDiagram, "HasSecondaryYAxis");

		SCUtil.closeFile(scDocument);
		
		assertArrayEquals("Incorrect chart grids got in ." + fileType + " file.", expected, results);

	}
	
	/**
	 * Enable different types of axes in 3D chart.
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range and create a 2D chart.
	 * 3. Enable X/Y/Z axis in chart.
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the axes setting.
	 * @throws Exception
	 */
	@Test
	public void testCreateXYZAxes() throws Exception {
		String fileName = "testCreateXYZAxes";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		Boolean[][] expected = {
				{true, true, true},
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
		
		if (axes != null) {
			SCUtil.setProperties(xDiagram, "HasXAxis", axes[0]);
			SCUtil.setProperties(xDiagram, "HasYAxis", axes[1]);
			SCUtil.setProperties(xDiagram, "HasZAxis", axes[2]);
			expected[0][0] = axes[0];
			expected[0][1] = axes[1];
			expected[0][2] = axes[2];
		}

		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		xDiagram = xChartDocument.getDiagram(); 
		results[0][0] = (Boolean) SCUtil.getProperties(xDiagram, "HasXAxis");
		results[0][1] = (Boolean) SCUtil.getProperties(xDiagram, "HasYAxis");
		results[0][2] = (Boolean) SCUtil.getProperties(xDiagram, "HasZAxis");
		
		SCUtil.closeFile(scDocument);

		assertArrayEquals("Incorrect chart Axes got in ." + fileType + " file.", expected, results);
		
	}
	
}