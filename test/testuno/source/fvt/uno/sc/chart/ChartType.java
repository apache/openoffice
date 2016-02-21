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
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.CellRangeAddress;

/**
 *  Check the chart type setting can be applied and saved
 * 
 */
@RunWith(value = Parameterized.class)
public class ChartType {

	private String expected;
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
				{1, -1, 0, 3}
		};
		double[][] numberData2 = {
				{22, 12, 15},
				{20, 11, 20},
				{40, 37, 38},
				{3.01, 2.2, 2.2}
		};

		return Arrays.asList(new Object[][] {
			{"com.sun.star.chart.BarDiagram", "com.sun.star.chart.BarDiagram", numberData1, "ods"},
			{"com.sun.star.chart.AreaDiagram", "com.sun.star.chart.AreaDiagram", numberData1, "ods"},
			{"com.sun.star.chart.LineDiagram", "com.sun.star.chart.LineDiagram", numberData1, "ods"},
			{"com.sun.star.chart.PieDiagram", "com.sun.star.chart.PieDiagram", numberData1, "ods"},
			{"com.sun.star.chart.DonutDiagram", "com.sun.star.chart.DonutDiagram", numberData1, "ods"},
			{"com.sun.star.chart.NetDiagram", "com.sun.star.chart.NetDiagram", numberData1, "ods"},
			{"com.sun.star.chart.XYDiagram", "com.sun.star.chart.XYDiagram", numberData1, "ods"},
			{"com.sun.star.chart.StockDiagram", "com.sun.star.chart.StockDiagram", numberData2, "ods"},
			{"com.sun.star.chart.BubbleDiagram", "com.sun.star.chart.BubbleDiagram", numberData1, "ods"},
			
			{"com.sun.star.chart.BarDiagram", "com.sun.star.chart.BarDiagram", numberData1, "xls"},
			{"com.sun.star.chart.AreaDiagram", "com.sun.star.chart.AreaDiagram", numberData1, "xls"},
			{"com.sun.star.chart.LineDiagram", "com.sun.star.chart.LineDiagram", numberData1, "xls"},
			{"com.sun.star.chart.PieDiagram", "com.sun.star.chart.PieDiagram", numberData1, "xls"},
			{"com.sun.star.chart.DonutDiagram", "com.sun.star.chart.DonutDiagram", numberData1, "xls"},
			{"com.sun.star.chart.NetDiagram", "com.sun.star.chart.NetDiagram", numberData1, "xls"},
			{"com.sun.star.chart.XYDiagram", "com.sun.star.chart.XYDiagram", numberData1, "xls"},
			{"com.sun.star.chart.StockDiagram", "com.sun.star.chart.StockDiagram", numberData2, "xls"},
			{"com.sun.star.chart.BubbleDiagram", "com.sun.star.chart.BubbleDiagram", numberData1, "xls"}		
		});
	}
	
	public ChartType(String expected, String inputType, double[][] numberData, String fileType) {
		this.expected = expected;
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
	 * Check the basic types of chart
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range.
	 * 3. Use the data to create a chart, change the chart type.
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the chart type setting.
	 * @throws Exception
	 */
	@Test
	public void testCreateChart() throws Exception {
		String fileName = "testCreateChart";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		String result = null;		
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
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		result = xChartDocument.getDiagram().getDiagramType();
		
		SCUtil.closeFile(scDocument);
		
		assertEquals("Incorrect chart type string got in ." + fileType + " file.", expected, result);

	}

}