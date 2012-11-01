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

import com.sun.star.awt.Point;
import com.sun.star.awt.Rectangle;
import com.sun.star.chart.XChartDocument;
import com.sun.star.drawing.XShape;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.CellRangeAddress;

/**
 *  Check the chart legend and the position can be applied and saved
 * 
 */
@RunWith(value = Parameterized.class)
public class ChartLegend {

	private String inputType;
	private double[][] numberData;
	private int[] offset;
	private String fileType;
	
	private static final UnoApp unoApp = new UnoApp();
	
	XComponent scComponent = null;
	XSpreadsheetDocument scDocument = null;
	
	@Parameters
	public static Collection<Object[]> data() throws Exception {
		int[][] offsetList = {
				{-50, -2000},
				{-1000, 3000},
				{-4000, -1000}
		};
		
		double[][] numberData1 = {
				{1, 2, 3, 4},
				{2, 4.3, 5, 8},
				{4, 2, 3, 1},
				{1, -1, 0, 3}
		};
		return Arrays.asList(new Object[][] {
			{"com.sun.star.chart.BarDiagram", numberData1, offsetList[0], "ods"},
			{"com.sun.star.chart.BubbleDiagram", numberData1, offsetList[1], "ods"},
			{"com.sun.star.chart.NetDiagram", numberData1, offsetList[2], "ods"},
			{"com.sun.star.chart.BarDiagram", numberData1, offsetList[0], "xls"},
			{"com.sun.star.chart.BubbleDiagram", numberData1, offsetList[1], "xls"},
			{"com.sun.star.chart.NetDiagram", numberData1, offsetList[2], "xls"}
		});
	}
	
	public ChartLegend(String inputType, double[][] numberData, int[] offset, String fileType) {
		this.inputType = inputType;
		this.numberData = numberData;
		this.offset = offset;
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
	 * Check remove the legend of chart
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range.
	 * 3. Use the data to create a chart, set the chart type.
	 * 4. Remove the legend.
	 * 5. Save file as ODF/MSBinary format.
	 * 6. Close and reopen file.  -> Check the legend status.
	 * @throws Exception
	 */
	@Test
	public void testDisableLegend() throws Exception {
		String fileName = "testDisableLegend";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		Boolean result = true;		

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
		result = (Boolean) SCUtil.getProperties(xChartDocument, "HasLegend");
		if (result) {
			SCUtil.setProperties(xChartDocument, "HasLegend", false);
		}
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		result = (Boolean) SCUtil.getProperties(xChartDocument, "HasLegend");

		SCUtil.closeFile(scDocument);
		
		assertFalse("Chart legend has not been disabled in " + fileType + " file.", result);

	}
	
	/**
	 * Check change the position of legend in chart
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range.
	 * 3. Use the data to create a chart, set the chart type.
	 * 4. Change the position of legend in chart.
	 * 5. Save file as ODF/MSBinary format.
	 * 6. Close and reopen file.  -> Check the legend position.
	 * @throws Exception
	 */
	@Test
	public void testLegendPosition() throws Exception {
		String fileName = "testDisableLegend";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		Boolean result = true;
		int delta = 4;//Save as .xls file, the legend position may change a little, set acceptable range.

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
		
		XShape legend = xChartDocument.getLegend();
		Point aPoint = legend.getPosition();
		aPoint = new Point(aPoint.X + offset[0], aPoint.Y + offset[1]);
		legend.setPosition(aPoint);
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		result = (Boolean) SCUtil.getProperties(xChartDocument, "HasLegend");
		legend = xChartDocument.getLegend();
		Point resultPoint = legend.getPosition();

		SCUtil.closeFile(scDocument);
		
		assertTrue("Chart legend has not been enabled in ." + fileType + " file.", result);
		
		if (fileType.equalsIgnoreCase("xls")) {
			assertEquals("Incorrect chart legend position X got in ." + fileType + " file.", aPoint.X, resultPoint.X, delta);
			assertEquals("Incorrect chart legend position X got in ." + fileType + " file.", aPoint.Y, resultPoint.Y, delta);
		}
		else {
			assertEquals("Incorrect chart legend position X got in ." + fileType + " file.", aPoint.X, resultPoint.X);
			assertEquals("Incorrect chart legend position X got in ." + fileType + " file.", aPoint.Y, resultPoint.Y);
		}
		
	}

}