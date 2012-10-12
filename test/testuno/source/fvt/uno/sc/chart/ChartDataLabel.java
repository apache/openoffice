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
 *  Check different types of chart data labels can be applied and saved
 * 
 */
@RunWith(value = Parameterized.class)
public class ChartDataLabel {

	private int labelType;
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

		return Arrays.asList(new Object[][] {
			{0, "com.sun.star.chart.XYDiagram", numberData1, "ods"}, // no label
			{1, "com.sun.star.chart.PieDiagram", numberData1, "ods"}, // show number
			{2, "com.sun.star.chart.BarDiagram", numberData1, "ods"}, // show percentage
			{4, "com.sun.star.chart.LineDiagram", numberData1, "ods"}, // show category name
			{16, "com.sun.star.chart.BarDiagram", numberData1, "ods"}, // check legend symbol (won't shown on UI)
			{3, "com.sun.star.chart.DonutDiagram", numberData1, "ods"}, // show number & percentage
			{5, "com.sun.star.chart.BubbleDiagram", numberData1, "ods"}, // show number & category name
			{6, "com.sun.star.chart.NetDiagram", numberData1, "ods"}, // show percentage & category name
			{7, "com.sun.star.chart.BarDiagram", numberData1, "ods"}, // show number & percentage & category name
			{17, "com.sun.star.chart.LineDiagram", numberData1, "ods"}, // show number & legend symbol
			{18, "com.sun.star.chart.XYDiagram", numberData1, "ods"}, // show percentage & legend symbol
			{19, "com.sun.star.chart.BarDiagram", numberData1, "ods"}, // show number & percentage & legend symbol
			{20, "com.sun.star.chart.NetDiagram", numberData1, "ods"}, // show category name & legend symbol
			{21, "com.sun.star.chart.AreaDiagram", numberData1, "ods"}, // show number & Category name & legend symbol
			{22, "com.sun.star.chart.BarDiagram", numberData1, "ods"}, // show percentage & Category name & legend symbol
			{23, "com.sun.star.chart.BarDiagram", numberData1, "ods"}, // show number & percentage & & Category name & legend symbol
			{0, "com.sun.star.chart.BarDiagram", numberData1, "xls"}, 
			{1, "com.sun.star.chart.PieDiagram", numberData1, "xls"}, 
			{4, "com.sun.star.chart.LineDiagram", numberData1, "xls"},
			{5, "com.sun.star.chart.BarDiagram", numberData1, "xls"},
			{17, "com.sun.star.chart.BarDiagram", numberData1, "xls"},
			{20, "com.sun.star.chart.BubbleDiagram", numberData1, "xls"},
			{21, "com.sun.star.chart.BarDiagram", numberData1, "xls"}
		});
	}
	
	public ChartDataLabel(int labelType, String inputType, double[][] numberData, String fileType) {
		this.labelType = labelType;
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
	 * Enable different types of data labels in chart.
	 * 1. Create a spreadsheet file.
	 * 2. Input number in a cell range and create a chart.
	 * 3. Enable different types of data labels for all data series in chart.
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the chart data label setting.
	 * @throws Exception
	 */
	@Test
	public void testCreateDataLabel() throws Exception {
		String fileName = "testCreateDataLabel";
		String chartName = "testChart";
		String cellRangeName = "A1:D4";
		int result = 0;	
		
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
		
		SCUtil.setProperties(xChartDocument.getDiagram(), "DataCaption", labelType);

		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		xChartDocument = SCUtil.getChartByName(sheet, chartName);
		result = ((Integer) SCUtil.getProperties(xChartDocument.getDiagram(), "DataCaption")).intValue();
		SCUtil.closeFile(scDocument);
		
		assertEquals("Incorrect chart data label types got in ." + fileType + " file.", labelType, result, 0);

	}
	
}