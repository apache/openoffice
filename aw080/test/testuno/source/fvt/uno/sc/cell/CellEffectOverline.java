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
import testlib.uno.TestUtil;
import testlib.uno.CellInfo;

import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.XCell;


/**
 *  Check the cell background color and font color setting can be applied and saved
 * 
 */
@RunWith(value = Parameterized.class)
public class CellEffectOverline {

	private int expectedLine;
	private int expectedColor;
	private String[] inputType;
	private int inputStyle;
	private int inputColor;
	private String fileType;
	
	private static final UnoApp unoApp = new UnoApp();
	
	XComponent scComponent = null;
	XSpreadsheetDocument scDocument = null;
	
	@Parameters
	public static Collection<Object[]> data() throws Exception {
		String[] typeList = {"CharOverline", "CharOverlineHasColor", "CharOverlineColor"};
		int[] list = TestUtil.randColorList(19);
		return Arrays.asList(new Object[][] {
			{0, list[0], typeList, 0, list[0], "ods"}, //NONE 0
			{1, list[1], typeList, 1, list[1], "ods"}, //SIGNLE 1
			{2, list[2], typeList, 2, list[2], "ods"}, //DOUBLE 2
			{3, list[3], typeList, 3, list[3], "ods"}, //DOTTED 3
			{0, list[4], typeList, 4, list[4], "ods"}, //DONTKNOW 4  can not set this setting via UI
			{5, list[5], typeList, 5, list[5], "ods"}, //DASH 5
			{6, list[6], typeList, 6, list[6], "ods"}, //LONGDASH 6
			{7, list[7], typeList, 7, list[7], "ods"}, //DASHDOT 7
			{8, list[8], typeList, 8, list[8], "ods"}, //DASHDOTDOT 8
			{9, list[9], typeList, 9, list[9], "ods"}, //SMALLWAVE 9  can not set this setting via UI
			{10, list[10], typeList, 10, list[10], "ods"}, //WAVE 10
			{11, list[11], typeList, 11, list[11], "ods"}, //DOUBLEWAVE 11
			{12, list[12], typeList, 12, list[12], "ods"}, //BOLD 12
			{13, list[13], typeList, 13, list[13], "ods"}, //BOLDDOTTED 13
			{14, list[14], typeList, 14, list[14], "ods"}, //BOLDDASH 14
			{15, list[15], typeList, 15, list[15], "ods"}, //BOLDLONGDASH 15
			{16, list[16], typeList, 16, list[16], "ods"}, //BOLDDASHDOT 16
			{17, list[17], typeList, 17, list[17], "ods"}, //BOLDDASHDOTDOT 17
			{18, list[18], typeList, 18, list[18], "ods"}, //BOLDWAVE = 18
			
			{0, list[0], typeList, 0, list[0], "xls"}, //NONE 0
			{0, list[1], typeList, 1, list[1], "xls"}, //SIGNLE 1
			{0, list[2], typeList, 2, list[2], "xls"}, //DOUBLE 2
			{0, list[3], typeList, 3, list[3], "xls"}, //DOTTED 3
			{0, list[4], typeList, 4, list[4], "xls"}, //DONTKNOW 4  can not set this setting via UI
			{0, list[5], typeList, 5, list[5], "xls"}, //DASH 5
			{0, list[6], typeList, 6, list[6], "xls"}, //LONGDASH 6
			{0, list[7], typeList, 7, list[7], "xls"}, //DASHDOT 7
			{0, list[8], typeList, 8, list[8], "xls"}, //DASHDOTDOT 8
			{0, list[9], typeList, 9, list[9], "xls"}, //SMALLWAVE 9  can not set this setting via UI
			{0, list[10], typeList, 10, list[10], "xls"}, //WAVE 10
			{0, list[11], typeList, 11, list[11], "xls"}, //DOUBLEWAVE 11
			{0, list[12], typeList, 12, list[12], "xls"}, //BOLD 12
			{0, list[13], typeList, 13, list[13], "xls"}, //BOLDDOTTED 13
			{0, list[14], typeList, 14, list[14], "xls"}, //BOLDDASH 14
			{0, list[15], typeList, 15, list[15], "xls"}, //BOLDLONGDASH 15
			{0, list[16], typeList, 16, list[16], "xls"}, //BOLDDASHDOT 16
			{0, list[17], typeList, 17, list[17], "xls"}, //BOLDDASHDOTDOT 17
			{0, list[18], typeList, 18, list[18], "xls"} //BOLDWAVE = 18
			
		});
	}
	
	public CellEffectOverline(int expectedStyle, int expectedColor, String[] inputType, int inputStyle, int inputColor, String fileType) {
		this.expectedLine = expectedStyle;
		this.expectedColor = expectedColor;
		this.inputType = inputType;
		this.inputStyle = inputStyle;
		this.inputColor = inputColor;
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
	 * Check the cell overline style and overline color
	 * 1. Create a spreadsheet file.
	 * 2. Input number, text, formula into many cell.
	 * 3. Set cell overline style.
	 * 4. Set cell overline color, if overline style is not NONE.
	 * 4. Save file as ODF/MSBinary format. (MSBinary file can not support overline, save as .xls, the overline setting will be lost)
	 * 5. Close and reopen file.  -> Check the overline style and overline color setting. 
	 * @throws Exception
	 */
	@Test
	public void testCharOverline() throws Exception {
		String fileName = "testCharOverline";
		int cellNum = 5;
		XCell[] cells = new XCell[cellNum];
		int[] styleResults = new int[cellNum];
		boolean[] hasColor = new boolean[cellNum];
		int[] colorResults = new int[cellNum];
		CellInfo cInfo = TestUtil.randCell(100, 100);
		
		XSpreadsheet sheet = SCUtil.getCurrentSheet(scDocument);
		
		for (int i = 0; i < cellNum; i++) {
			cells[i] = sheet.getCellByPosition(cInfo.getCol(), cInfo.getRow() + i);
		}
		
		cells[0].setValue(inputColor);
		SCUtil.setTextToCell(cells[1], inputType[0]);
		cells[2].setFormula("=10/0");
		cells[3].setValue(-0.0000001);

		for (int i = 0; i < cellNum; i++) {
			SCUtil.setCellProperties(cells[i], inputType[0], inputStyle);
			if (inputStyle > 0) {
				SCUtil.setCellProperties(cells[i], inputType[1], true);
				SCUtil.setCellProperties(cells[i], inputType[2], inputColor);
			}						
		}
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		for (int i = 0; i < cellNum; i++) {
			cells[i] = sheet.getCellByPosition(cInfo.getCol(), cInfo.getRow() + i);
			styleResults[i] = ((Short) SCUtil.getCellProperties(cells[i], inputType[0])).shortValue();
			
			if (inputStyle > 0) {
				hasColor[i] = ((Boolean) SCUtil.getCellProperties(cells[i], inputType[1])).booleanValue();
				colorResults[i] = ((Integer) SCUtil.getCellProperties(cells[i], inputType[2])).intValue();
			}
		}
		
		SCUtil.closeFile(scDocument);

		for (int i = 0; i < cellNum; i++) {
			assertEquals("Incorrect cell overline style(" + inputType[0] + ") value got in ." + fileType + " file.", expectedLine, styleResults[i], 0);

			if (inputStyle > 0) {
				
				if( fileType.equalsIgnoreCase("xls") || fileType.equalsIgnoreCase("xlt")) {
					assertFalse("Incorrect cell overline has color setting(" + inputType[1] + ") value got in ." + fileType + " file.", hasColor[i]);
				}
				
				else {
					assertTrue("Incorrect cell overline has color setting(" + inputType[1] + ") value got in ." + fileType + " file.", hasColor[i]);
					assertEquals("Incorrect cell overline color(" + inputType[2] + ") value got in ." + fileType + " file.", expectedColor, colorResults[i], 0);
				}
				
			}
		}
			
	}

}
