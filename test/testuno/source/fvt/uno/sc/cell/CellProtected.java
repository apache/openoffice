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
import com.sun.star.table.XCell;
import com.sun.star.util.CellProtection;


/**
 *  Check the cell protection setting can be applied and saved
 * 
 */
@RunWith(value = Parameterized.class)
public class CellProtected {

	private Boolean[] expected;
	private String inputType;
	private CellProtection inputProtectProps;
	private String fileType;
	
	private static final UnoApp unoApp = new UnoApp();
	
	XComponent scComponent = null;
	XSpreadsheetDocument scDocument = null;
	
	@Parameters
	public static Collection<Object[]> data() throws Exception {
		Boolean[][] list = {
				{true, false, false, false}, //lock cell
				{false, true, false, false}, //hide formula
				{false, false, true, false}, //hide cell
				{false, false, false, true}, //hide cell from print
				
				{true, true, true, false}		
		};

		return Arrays.asList(new Object[][] {
			{list[0], "CellProtection", list[0], "ods"}, 
			{list[1], "CellProtection", list[1], "ods"},
			{list[4], "CellProtection", list[2], "ods"},
			{list[3], "CellProtection", list[3], "ods"}
		});
	}
	
	public CellProtected(Boolean[] expected, String inputType, Boolean[] inputData, String fileType) {
		
		CellProtection protection = new CellProtection();
		
		protection.IsLocked = inputData[0];
		protection.IsFormulaHidden = inputData[1];
		protection.IsHidden = inputData[2];
		protection.IsPrintHidden = inputData[3];
		
		this.expected = expected;
		this.inputType = inputType;
		this.inputProtectProps = protection;
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
	 * Check the cell protection settings
	 * 1. Create a spreadsheet file.
	 * 2. Input number, text, formula into many cell.
	 * 3. Set cell protection properties. (Clock, HiddenFormula, Hidden Cell, Hidden Cell from Printing)
	 * 4. Save file as ODF/MSBinary format.
	 * 5. Close and reopen file.  -> Check the cell protection setting.
	 * @throws Exception
	 */
	@Test
	public void testCellProtected() throws Exception {
		String fileName = "testCellProtected";
		
		int cellNum = 5;
		XCell[] cells = new XCell[cellNum];
		CellProtection[] results = new CellProtection[cellNum];
		CellInfo cInfo = TestUtil.randCell(10, 10);
		
		XSpreadsheet sheet = SCUtil.getCurrentSheet(scDocument);
		
		for (int i = 0; i < cellNum; i++) {
			cells[i] = sheet.getCellByPosition(cInfo.getCol() + i, cInfo.getRow());
		}
		
		cells[0].setValue(2134359.343223);
		SCUtil. setTextToCell(cells[1], inputType);
		cells[2].setFormula("=Average(A1:A10)");
		cells[3].setValue(-0.0003424);

		for (int i = 0; i < cellNum; i++) {
			SCUtil.setCellProperties(cells[i], inputType, inputProtectProps);
		}
		
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
		sheet = SCUtil.getCurrentSheet(scDocument);
		
		for (int i = 0; i < cellNum; i++) {
			cells[i] = sheet.getCellByPosition(cInfo.getCol() + i, cInfo.getRow());
			results[i] = (CellProtection) SCUtil.getCellProperties(cells[i], inputType);
		}
		
		SCUtil.closeFile(scDocument);
		
		for (int i = 0; i < cellNum; i++) {
			assertEquals("Incorrect cell protection (IsLocked) value got in ." + fileType + " file.", expected[0], results[i].IsLocked);
			assertEquals("Incorrect cell protection(IsFormulaHidden) value got in ." + fileType + " file.", expected[1], results[i].IsFormulaHidden);
			assertEquals("Incorrect cell protection(IsHidden) value got in ." + fileType + " file.", expected[2], results[i].IsHidden);
			assertEquals("Incorrect cell protection(IsPrintHidden) value got in ." + fileType + " file.", expected[3], results[i].IsPrintHidden);
		}	
	}	

}
