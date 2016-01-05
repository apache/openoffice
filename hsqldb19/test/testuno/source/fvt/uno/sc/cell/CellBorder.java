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


package fvt.uno.sc.cell;

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
import testlib.uno.TestUtil;
import testlib.uno.CellInfo;

import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.BorderLine;
import com.sun.star.table.XCell;


/**
 *  Check the cell border setting can be applied and saved
 * 
 */
@RunWith(value = Parameterized.class)
public class CellBorder {
	//create a class to implement Equals method for BorderLine
	private class BorderLineWithEqualsFunction{
		private int Color;
		private short InnerLineWidth;
		private short LineDistance;
		private short OuterLineWidth;
		
		public BorderLineWithEqualsFunction(BorderLine borderLine) {
			this.Color = borderLine.Color;
			this.InnerLineWidth = borderLine.InnerLineWidth;
			this.LineDistance = borderLine.LineDistance;
			this.OuterLineWidth = borderLine.OuterLineWidth;
		}
		
		public boolean equals(Object obj) {
			if (!(obj instanceof BorderLineWithEqualsFunction)) {
				return false;
			}
			BorderLineWithEqualsFunction borderLine = (BorderLineWithEqualsFunction) obj;
			return this.Color == borderLine.Color
					&& this.InnerLineWidth == borderLine.InnerLineWidth
					&& this.LineDistance == borderLine.LineDistance 
					&& this.OuterLineWidth == borderLine.OuterLineWidth;
		}
		
		public int hashCode() {
	       int result = 17;
	       result = 37 * result + (int) this.Color;
	       result = 37 * result + (short) this.InnerLineWidth;
	       result = 37 * result + (short) this.LineDistance;
	       result = 37 * result + (short) this.OuterLineWidth;
	       return result;
	    } 
	}
	
	private BorderLine expected;
	private BorderLine borderLine;
	private String fileType;
	
	private static final UnoApp unoApp = new UnoApp();
	
	XComponent scComponent = null;
	XSpreadsheetDocument scDocument = null;
	
	@Parameters
	public static Collection<Object[]> data() throws Exception {
		int[] colorList = TestUtil.randColorList(3);

		return Arrays.asList(new Object[][] {
			//{inner line (pt), distance (pt), outer line (pt), color number, inner line (pt), distance (pt), outer line (pt), file type} 
			{0, 0, 1, 0xFF0000, 0, 0, 1, "ods"},   
			{0, 0, 0.5, 0x00FF00, 0, 0, 0.5, "ods"},
			{0, 0, 2.5, 0x0000FF, 0, 0, 2.5, "ods"},
			{0, 0, 5, 0x0000FF, 0, 0, 5, "ods"},
			{0.05, 0.05, 0.05, colorList[0], 0.05, 0.05, 0.05, "ods"},   
			{1.0, 0.5, 1.0, colorList[1], 1.0, 0.5, 1.0, "ods"},
			{5, 2, 5, colorList[2], 5, 2, 5, "ods"},
			{0, 0, 4, 0xFF0000, 0, 0, 5, "xls"}, 
			{0, 0, 2.5, 0xFFFF00, 0, 0, 2, "xls"}, 
			{0, 0, 1, 0x00FF00, 0, 0, 0.5, "xls"},
			{1, 1, 1, 0x0000FF, 0.5, 1.0, 0.5, "xls"}

		});
	}
	
	public CellBorder(double expInnerLineWidth, double expLineDistance, double expOuterLineWidth, int color, double innerLineWidth, double lineDistance, double outerLineWidth, String fileType) {
		BorderLine eBorderLine = new BorderLine();
		BorderLine aBorderLine = new BorderLine();
		
		eBorderLine.Color = color;
		eBorderLine.InnerLineWidth = (short) Math.round(2540 / 72.0 * expInnerLineWidth);
		eBorderLine.LineDistance = (short) Math.round(2540 / 72.0 * expLineDistance);
		eBorderLine.OuterLineWidth = (short) Math.round(2540 / 72.0 * expOuterLineWidth);
		
		aBorderLine.Color = color;
		aBorderLine.InnerLineWidth = (short) Math.round(2540 / 72.0 * innerLineWidth);
		aBorderLine.LineDistance = (short) Math.round(2540 / 72.0 * lineDistance);
		aBorderLine.OuterLineWidth = (short) Math.round(2540 / 72.0 * outerLineWidth);
		
		this.expected = eBorderLine;
		this.borderLine = aBorderLine;
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
	 * Check the cell border settings
	 * 1. Create a spreadsheet file.
	 * 2. Input number, text, formula into many cell.
	 * 3. Set cell border properties.
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the border setting.
	 * @throws Exception
	 */
	@Test
	public void testCellBorder() throws Exception {
		String fileName = "testCellBorder";
		String[] borderType = {"LeftBorder", "RightBorder", "TopBorder", "BottomBorder"};
		int borderNum = borderType.length; 
		int cellNum = 10;
		XCell[] cells = new XCell[cellNum];
		BorderLine[][] results = new BorderLine[cellNum][borderNum];
		CellInfo cInfo = TestUtil.randCell(10, 10);

		XSpreadsheet sheet = SCUtil.getCurrentSheet(scDocument);

		for (int i = 0; i < cellNum; i++) {
			cells[i] = sheet.getCellByPosition(cInfo.getCol(), cInfo.getRow() + i);
		}

		cells[0].setValue(borderLine.Color);
		SCUtil. setTextToCell(cells[1], "all border");
		cells[2].setFormula("=2^6");
		cells[3].setValue(-0.1);

		for (int i = 0; i < cellNum; i++) {
			for (int j = 0; j < borderNum; j++) {
				SCUtil.setCellProperties(cells[i], borderType[j], borderLine);
			}
		}
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		for (int i = 0; i < cellNum; i++) {
			cells[i] = sheet.getCellByPosition(cInfo.getCol(), cInfo.getRow() + i);
			for (int j = 0; j < borderNum; j++) {
				results[i][j] = (BorderLine) SCUtil.getCellProperties(cells[i], borderType[j]);
			}
		}
		
		SCUtil.closeFile(scDocument);
			
		for (int i = 0; i< cellNum; i++){
			for (int j = 0; j<borderNum; j++) {
				assertEquals("Incorrect cell border(" + borderType[j] + ") value got in ." + fileType + " file.", 
					new BorderLineWithEqualsFunction(expected), new BorderLineWithEqualsFunction(results[i][j]));
			}
		}
			
	}	

}
