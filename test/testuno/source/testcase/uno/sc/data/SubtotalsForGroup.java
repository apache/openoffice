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
package testcase.uno.sc.data;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SCUtil;

import com.sun.star.lang.XComponent;
import com.sun.star.sheet.GeneralFunction;
import com.sun.star.sheet.SubTotalColumn;
import com.sun.star.sheet.XCellRangeData;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSubTotalCalculatable;
import com.sun.star.sheet.XSubTotalDescriptor;
import com.sun.star.table.XCellRange;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;

public class SubtotalsForGroup {
	private static final UnoApp app = new UnoApp();

	UnoApp unoApp = new UnoApp();
	XSpreadsheetDocument scDocument = null;
	XComponent scComponent = null;

	@Before
	public void setUpDocument() throws Exception {
		unoApp.start();
		scComponent = unoApp.newDocument("scalc");
	}

	@After
	public void tearDownDocument() {
		 unoApp.close();
		 unoApp.closeDocument(scComponent);

	}

	@BeforeClass
	public static void setUpConnection() throws Exception {

	}

	@AfterClass
	public static void tearDownConnection() throws InterruptedException,
			Exception {

	}

	@Test
	public void testForSecondGroup() throws Exception {
		scComponent = unoApp.newDocument("scalc");
		scDocument = SCUtil.getSCDocument(scComponent);
		XSpreadsheet currentsheet = SCUtil.getCurrentSheet(scDocument);
		XCellRange xdataRange = (XCellRange) UnoRuntime.queryInterface(
				XCellRange.class, currentsheet);
		XCellRange sourceRange = currentsheet.getCellRangeByName("A1:E8");
		XCellRangeData sourceData = (XCellRangeData) UnoRuntime.queryInterface(
				XCellRangeData.class, sourceRange);
		Object[][] Source = { { "Level", "Code", "No.", "Team", "Name" },
				{ "BS", 20, 4, "B", "Elle" }, { "BS", 20, 6, "C", "Sweet" },
				{ "BS", 20, 2, "A", "Chcomic" }, { "CS", 30, 5, "A", "Ally" },
				{ "MS", 10, 1, "A", "Joker" }, { "MS", 10, 3, "B", "Kevin" },
				{ "CS", 30, 7, "C", "Tom" } };
		sourceData.setDataArray(Source);

		// Create SubTotals
		XSubTotalCalculatable xSub = (XSubTotalCalculatable) UnoRuntime
				.queryInterface(XSubTotalCalculatable.class, sourceRange);
		XSubTotalDescriptor xSubDesc = xSub.createSubTotalDescriptor(true);
		SubTotalColumn[] aColumns = new SubTotalColumn[1];
		SubTotalColumn[] bColumns = new SubTotalColumn[1];
		// calculate sum of Second column
		aColumns[0] = new SubTotalColumn();
		aColumns[0].Column = 1;
		aColumns[0].Function = GeneralFunction.SUM;
		// group by 4th column
		xSubDesc.addNew(aColumns, 3);

		// calculate sum of third column
		bColumns[0] = new SubTotalColumn();
		bColumns[0].Column = 2;
		bColumns[0].Function = GeneralFunction.SUM;
		// group by second column
		xSubDesc.addNew(bColumns, 1);
		xSub.applySubTotals(xSubDesc, true);

		// Verify the result on line 3
		assertEquals("10 Sum", SCUtil.getTextFromCell(currentsheet, 1, 2));

		assertEquals("=SUBTOTAL(9;$C$2:$C$2)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 2));

		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 2, 2),
				0.000000001);

		// Verify the result on line 5
		assertEquals("20 Sum", SCUtil.getTextFromCell(currentsheet, 1, 4));

		assertEquals("=SUBTOTAL(9;$C$4:$C$4)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 4));

		assertEquals(2, SCUtil.getValueFromCell(currentsheet, 2, 4),
				0.000000001);

		// Verify the result on line 7
		assertEquals("30 Sum", SCUtil.getTextFromCell(currentsheet, 1, 6));

		assertEquals("=SUBTOTAL(9;$C$6:$C$6)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 6));

		assertEquals(5, SCUtil.getValueFromCell(currentsheet, 2, 6),
				0.000000001);

		// Verify the result on line 8
		assertEquals("A Sum", SCUtil.getTextFromCell(currentsheet, 3, 7));

		assertEquals("=SUBTOTAL(9;$B$2:$B$7)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 7));

		assertEquals(60, SCUtil.getValueFromCell(currentsheet, 1, 7),
				0.000000001);

		// Verify the result on line 10
		assertEquals("10 Sum", SCUtil.getTextFromCell(currentsheet, 1, 9));

		assertEquals("=SUBTOTAL(9;$C$9:$C$9)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 9));

		assertEquals(3, SCUtil.getValueFromCell(currentsheet, 2, 9),
				0.000000001);

		// Verify the result on line 12
		assertEquals("20 Sum", SCUtil.getTextFromCell(currentsheet, 1, 11));

		assertEquals("=SUBTOTAL(9;$C$11:$C$11)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 11));

		assertEquals(4, SCUtil.getValueFromCell(currentsheet, 2, 11),
				0.000000001);

		// Verify the result on line 13
		assertEquals("B Sum", SCUtil.getTextFromCell(currentsheet, 3, 12));

		assertEquals("=SUBTOTAL(9;$B$9:$B$12)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 12));

		assertEquals(30, SCUtil.getValueFromCell(currentsheet, 1, 12),
				0.000000001);

		// Verify the result on line 15
		assertEquals("20 Sum", SCUtil.getTextFromCell(currentsheet, 1, 14));

		assertEquals("=SUBTOTAL(9;$C$14:$C$14)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 14));

		assertEquals(6, SCUtil.getValueFromCell(currentsheet, 2, 14),
				0.000000001);

		// Verify the result on line 17
		assertEquals("30 Sum", SCUtil.getTextFromCell(currentsheet, 1, 16));

		assertEquals("=SUBTOTAL(9;$C$16:$C$16)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 16));

		assertEquals(7, SCUtil.getValueFromCell(currentsheet, 2, 16),
				0.000000001);

		// Verify the result on line 18
		assertEquals("C Sum", SCUtil.getTextFromCell(currentsheet, 3, 17));

		assertEquals("=SUBTOTAL(9;$B$14:$B$17)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 17));

		assertEquals(50, SCUtil.getValueFromCell(currentsheet, 1, 17),
				0.000000001);

		// Verify the result on line 19
		assertEquals("Grand Total", SCUtil.getTextFromCell(currentsheet, 3, 18));

		assertEquals("=SUBTOTAL(9;$B$2:$B$18)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 18));

		assertEquals(140, SCUtil.getValueFromCell(currentsheet, 1, 18),
				0.000000001);

		// Save the file and reload it
		SCUtil.saveFileAs(scComponent, "SubTotalsForGroup", "ods");
		XSpreadsheetDocument scDocumentTemp = SCUtil.reloadFile(unoApp,
				scDocument, "SubTotalsForGroup.ods");
		scDocument = scDocumentTemp;
		currentsheet = SCUtil.getCurrentSheet(scDocument);

		// verify it again
		// Verify the result on line 3
		assertEquals("10 Sum", SCUtil.getTextFromCell(currentsheet, 1, 2));

		assertEquals("=SUBTOTAL(9;$C$2:$C$2)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 2));

		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 2, 2),
				0.000000001);

		// Verify the result on line 5
		assertEquals("20 Sum", SCUtil.getTextFromCell(currentsheet, 1, 4));

		assertEquals("=SUBTOTAL(9;$C$4:$C$4)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 4));

		assertEquals(2, SCUtil.getValueFromCell(currentsheet, 2, 4),
				0.000000001);

		// Verify the result on line 7
		assertEquals("30 Sum", SCUtil.getTextFromCell(currentsheet, 1, 6));

		assertEquals("=SUBTOTAL(9;$C$6:$C$6)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 6));

		assertEquals(5, SCUtil.getValueFromCell(currentsheet, 2, 6),
				0.000000001);

		// Verify the result on line 8
		assertEquals("A Sum", SCUtil.getTextFromCell(currentsheet, 3, 7));

		assertEquals("=SUBTOTAL(9;$B$2:$B$7)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 7));

		assertEquals(60, SCUtil.getValueFromCell(currentsheet, 1, 7),
				0.000000001);

		// Verify the result on line 10
		assertEquals("10 Sum", SCUtil.getTextFromCell(currentsheet, 1, 9));

		assertEquals("=SUBTOTAL(9;$C$9:$C$9)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 9));

		assertEquals(3, SCUtil.getValueFromCell(currentsheet, 2, 9),
				0.000000001);

		// Verify the result on line 12
		assertEquals("20 Sum", SCUtil.getTextFromCell(currentsheet, 1, 11));

		assertEquals("=SUBTOTAL(9;$C$11:$C$11)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 11));

		assertEquals(4, SCUtil.getValueFromCell(currentsheet, 2, 11),
				0.000000001);

		// Verify the result on line 13
		assertEquals("B Sum", SCUtil.getTextFromCell(currentsheet, 3, 12));

		assertEquals("=SUBTOTAL(9;$B$9:$B$12)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 12));

		assertEquals(30, SCUtil.getValueFromCell(currentsheet, 1, 12),
				0.000000001);

		// Verify the result on line 15
		assertEquals("20 Sum", SCUtil.getTextFromCell(currentsheet, 1, 14));

		assertEquals("=SUBTOTAL(9;$C$14:$C$14)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 14));

		assertEquals(6, SCUtil.getValueFromCell(currentsheet, 2, 14),
				0.000000001);

		// Verify the result on line 17
		assertEquals("30 Sum", SCUtil.getTextFromCell(currentsheet, 1, 16));

		assertEquals("=SUBTOTAL(9;$C$16:$C$16)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 16));

		assertEquals(7, SCUtil.getValueFromCell(currentsheet, 2, 16),
				0.000000001);

		// Verify the result on line 18
		assertEquals("C Sum", SCUtil.getTextFromCell(currentsheet, 3, 17));

		assertEquals("=SUBTOTAL(9;$B$14:$B$17)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 17));

		assertEquals(50, SCUtil.getValueFromCell(currentsheet, 1, 17),
				0.000000001);

		// Verify the result on line 19
		assertEquals("Grand Total", SCUtil.getTextFromCell(currentsheet, 3, 18));

		assertEquals("=SUBTOTAL(9;$B$2:$B$18)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 18));

		assertEquals(140, SCUtil.getValueFromCell(currentsheet, 1, 18),
				0.000000001);

	}

	@Test
	public void testForThirdGroup() throws Exception {
		scComponent = unoApp.newDocument("scalc");
		scDocument = SCUtil.getSCDocument(scComponent);
		XSpreadsheet currentsheet = SCUtil.getCurrentSheet(scDocument);
		XCellRange xdataRange = (XCellRange) UnoRuntime.queryInterface(
				XCellRange.class, currentsheet);
		XCellRange sourceRange = currentsheet.getCellRangeByName("A1:E8");
		XCellRangeData sourceData = (XCellRangeData) UnoRuntime.queryInterface(
				XCellRangeData.class, sourceRange);
		Object[][] Source = { { "Level", "Code", "No.", "Team", "Name" },
				{ "BS", 20, 4, "B", "Elle" }, { "BS", 20, 6, "C", "Sweet" },
				{ "BS", 20, 2, "A", "Chcomic" }, { "CS", 30, 5, "A", "Ally" },
				{ "MS", 10, 1, "A", "Joker" }, { "MS", 10, 3, "B", "Kevin" },
				{ "CS", 30, 7, "C", "Tom" } };
		sourceData.setDataArray(Source);

		// Create SubTotals
		XSubTotalCalculatable xSub = (XSubTotalCalculatable) UnoRuntime
				.queryInterface(XSubTotalCalculatable.class, sourceRange);
		XSubTotalDescriptor xSubDesc = xSub.createSubTotalDescriptor(true);
		SubTotalColumn[] aColumns = new SubTotalColumn[1];
		SubTotalColumn[] bColumns = new SubTotalColumn[1];
		SubTotalColumn[] cColumns = new SubTotalColumn[1];
		// calculate sum of Second column
		aColumns[0] = new SubTotalColumn();
		aColumns[0].Column = 1;
		aColumns[0].Function = GeneralFunction.SUM;
		// group by 4th column
		xSubDesc.addNew(aColumns, 3);

		// calculate sum of third column
		bColumns[0] = new SubTotalColumn();
		bColumns[0].Column = 2;
		bColumns[0].Function = GeneralFunction.SUM;
		// group by second column
		xSubDesc.addNew(bColumns, 1);

		// calculate sum of third column
		cColumns[0] = new SubTotalColumn();
		cColumns[0].Column = 4;
		cColumns[0].Function = GeneralFunction.COUNT;
		// group by first column
		xSubDesc.addNew(cColumns, 0);
		xSub.applySubTotals(xSubDesc, true);

		// Verify the result on line 3
		assertEquals("MS Count", SCUtil.getTextFromCell(currentsheet, 0, 2));

		assertEquals("=SUBTOTAL(3;$E$2:$E$2)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 2));

		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 2),
				0.000000001);

		// Verify the result on line 4
		assertEquals("10 Sum", SCUtil.getTextFromCell(currentsheet, 1, 3));

		assertEquals("=SUBTOTAL(9;$C$2:$C$3)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 3));

		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 2, 3),
				0.000000001);

		// Verify the result on line 6
		assertEquals("BS Count", SCUtil.getTextFromCell(currentsheet, 0, 5));
		assertEquals("=SUBTOTAL(3;$E$5:$E$5)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 5));
		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 5),
				0.000000001);

		// Verify the result on line7
		assertEquals("20 Sum", SCUtil.getTextFromCell(currentsheet, 1, 6));
		assertEquals("=SUBTOTAL(9;$C$5:$C$6)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 6));
		assertEquals(2, SCUtil.getValueFromCell(currentsheet, 2, 6),
				0.000000001);

		// Verify the result on line 9
		assertEquals("CS Count", SCUtil.getTextFromCell(currentsheet, 0, 8));
		assertEquals("=SUBTOTAL(3;$E$8:$E$8)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 8));
		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 8),
				0.000000001);

		// Verify the result on line 10
		assertEquals("30 Sum", SCUtil.getTextFromCell(currentsheet, 1, 9));
		assertEquals("=SUBTOTAL(9;$C$8:$C$9)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 9));
		assertEquals(5, SCUtil.getValueFromCell(currentsheet, 2, 9),
				0.000000001);

		// Verify the result on line 11
		assertEquals("A Sum", SCUtil.getTextFromCell(currentsheet, 3, 10));
		assertEquals("=SUBTOTAL(9;$B$2:$B$10)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 10));
		assertEquals(60, SCUtil.getValueFromCell(currentsheet, 1, 10),
				0.000000001);

		// Verify the result on line 13
		assertEquals("MS Count", SCUtil.getTextFromCell(currentsheet, 0, 12));
		assertEquals("=SUBTOTAL(3;$E$12:$E$12)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 12));
		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 12),
				0.000000001);

		// Verify the result on line 14
		assertEquals("10 Sum", SCUtil.getTextFromCell(currentsheet, 1, 13));
		assertEquals("=SUBTOTAL(9;$C$12:$C$13)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 13));
		assertEquals(3, SCUtil.getValueFromCell(currentsheet, 2, 13),
				0.000000001);

		// Verify the result on line 16
		assertEquals("BS Count", SCUtil.getTextFromCell(currentsheet, 0, 15));
		assertEquals("=SUBTOTAL(3;$E$15:$E$15)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 15));
		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 15),
				0.000000001);

		// Verify the result on line 17
		assertEquals("20 Sum", SCUtil.getTextFromCell(currentsheet, 1, 16));

		assertEquals("=SUBTOTAL(9;$C$15:$C$16)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 16));

		assertEquals(4, SCUtil.getValueFromCell(currentsheet, 2, 16),
				0.000000001);

		// Verify the result on line 18
		assertEquals("B Sum", SCUtil.getTextFromCell(currentsheet, 3, 17));

		assertEquals("=SUBTOTAL(9;$B$12:$B$17)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 17));

		assertEquals(30, SCUtil.getValueFromCell(currentsheet, 1, 17),
				0.000000001);

		// Verify the result on line 20
		assertEquals("BS Count", SCUtil.getTextFromCell(currentsheet, 0, 19));
		assertEquals("=SUBTOTAL(3;$E$19:$E$19)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 19));
		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 19),
				0.000000001);

		// Verify the result on line 21
		assertEquals("20 Sum", SCUtil.getTextFromCell(currentsheet, 1, 20));
		assertEquals("=SUBTOTAL(9;$C$19:$C$20)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 20));
		assertEquals(6, SCUtil.getValueFromCell(currentsheet, 2, 20),
				0.000000001);

		// Verify the result on line 23
		assertEquals("CS Count", SCUtil.getTextFromCell(currentsheet, 0, 22));
		assertEquals("=SUBTOTAL(3;$E$22:$E$22)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 22));
		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 22),
				0.000000001);

		// Verify the result on line 24
		assertEquals("30 Sum", SCUtil.getTextFromCell(currentsheet, 1, 23));

		assertEquals("=SUBTOTAL(9;$C$22:$C$23)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 23));
		assertEquals(7, SCUtil.getValueFromCell(currentsheet, 2, 23),
				0.000000001);

		// Verify the result on line 25
		assertEquals("C Sum", SCUtil.getTextFromCell(currentsheet, 3, 24));

		assertEquals("=SUBTOTAL(9;$B$19:$B$24)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 24));
		assertEquals(50, SCUtil.getValueFromCell(currentsheet, 1, 24),
				0.000000001);

		// Verify the result on line 26
		assertEquals("Grand Total", SCUtil.getTextFromCell(currentsheet, 3, 25));
		assertEquals("=SUBTOTAL(9;$B$2:$B$25)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 25));
		assertEquals(140, SCUtil.getValueFromCell(currentsheet, 1, 25),
				0.000000001);

		// Save the file and reload it
		SCUtil.saveFileAs(scComponent, "SubTotalsForGroup", "ods");
		XSpreadsheetDocument scDocumentTemp = SCUtil.reloadFile(unoApp,
				scDocument, "SubTotalsForGroup.ods");
		scDocument = scDocumentTemp;
		currentsheet = SCUtil.getCurrentSheet(scDocument);

		// verify it again
		// Verify the result on line 3
		assertEquals("MS Count", SCUtil.getTextFromCell(currentsheet, 0, 2));

		assertEquals("=SUBTOTAL(3;$E$2:$E$2)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 2));

		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 2),
				0.000000001);

		// Verify the result on line 4
		assertEquals("10 Sum", SCUtil.getTextFromCell(currentsheet, 1, 3));

		assertEquals("=SUBTOTAL(9;$C$2:$C$3)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 3));

		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 2, 3),
				0.000000001);

		// Verify the result on line 6
		assertEquals("BS Count", SCUtil.getTextFromCell(currentsheet, 0, 5));
		assertEquals("=SUBTOTAL(3;$E$5:$E$5)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 5));
		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 5),
				0.000000001);

		// Verify the result on line7
		assertEquals("20 Sum", SCUtil.getTextFromCell(currentsheet, 1, 6));
		assertEquals("=SUBTOTAL(9;$C$5:$C$6)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 6));
		assertEquals(2, SCUtil.getValueFromCell(currentsheet, 2, 6),
				0.000000001);

		// Verify the result on line 9
		assertEquals("CS Count", SCUtil.getTextFromCell(currentsheet, 0, 8));
		assertEquals("=SUBTOTAL(3;$E$8:$E$8)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 8));
		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 8),
				0.000000001);

		// Verify the result on line 10
		assertEquals("30 Sum", SCUtil.getTextFromCell(currentsheet, 1, 9));
		assertEquals("=SUBTOTAL(9;$C$8:$C$9)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 9));
		assertEquals(5, SCUtil.getValueFromCell(currentsheet, 2, 9),
				0.000000001);

		// Verify the result on line 11
		assertEquals("A Sum", SCUtil.getTextFromCell(currentsheet, 3, 10));
		assertEquals("=SUBTOTAL(9;$B$2:$B$10)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 10));
		assertEquals(60, SCUtil.getValueFromCell(currentsheet, 1, 10),
				0.000000001);

		// Verify the result on line 13
		assertEquals("MS Count", SCUtil.getTextFromCell(currentsheet, 0, 12));
		assertEquals("=SUBTOTAL(3;$E$12:$E$12)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 12));
		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 12),
				0.000000001);

		// Verify the result on line 14
		assertEquals("10 Sum", SCUtil.getTextFromCell(currentsheet, 1, 13));
		assertEquals("=SUBTOTAL(9;$C$12:$C$13)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 13));
		assertEquals(3, SCUtil.getValueFromCell(currentsheet, 2, 13),
				0.000000001);

		// Verify the result on line 16
		assertEquals("BS Count", SCUtil.getTextFromCell(currentsheet, 0, 15));
		assertEquals("=SUBTOTAL(3;$E$15:$E$15)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 15));
		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 15),
				0.000000001);

		// Verify the result on line 17
		assertEquals("20 Sum", SCUtil.getTextFromCell(currentsheet, 1, 16));

		assertEquals("=SUBTOTAL(9;$C$15:$C$16)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 16));

		assertEquals(4, SCUtil.getValueFromCell(currentsheet, 2, 16),
				0.000000001);

		// Verify the result on line 18
		assertEquals("B Sum", SCUtil.getTextFromCell(currentsheet, 3, 17));

		assertEquals("=SUBTOTAL(9;$B$12:$B$17)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 17));

		assertEquals(30, SCUtil.getValueFromCell(currentsheet, 1, 17),
				0.000000001);

		// Verify the result on line 20
		assertEquals("BS Count", SCUtil.getTextFromCell(currentsheet, 0, 19));
		assertEquals("=SUBTOTAL(3;$E$19:$E$19)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 19));
		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 19),
				0.000000001);

		// Verify the result on line 21
		assertEquals("20 Sum", SCUtil.getTextFromCell(currentsheet, 1, 20));
		assertEquals("=SUBTOTAL(9;$C$19:$C$20)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 20));
		assertEquals(6, SCUtil.getValueFromCell(currentsheet, 2, 20),
				0.000000001);

		// Verify the result on line 23
		assertEquals("CS Count", SCUtil.getTextFromCell(currentsheet, 0, 22));
		assertEquals("=SUBTOTAL(3;$E$22:$E$22)",
				SCUtil.getFormulaFromCell(currentsheet, 4, 22));
		assertEquals(1, SCUtil.getValueFromCell(currentsheet, 4, 22),
				0.000000001);

		// Verify the result on line 24
		assertEquals("30 Sum", SCUtil.getTextFromCell(currentsheet, 1, 23));

		assertEquals("=SUBTOTAL(9;$C$22:$C$23)",
				SCUtil.getFormulaFromCell(currentsheet, 2, 23));
		assertEquals(7, SCUtil.getValueFromCell(currentsheet, 2, 23),
				0.000000001);

		// Verify the result on line 25
		assertEquals("C Sum", SCUtil.getTextFromCell(currentsheet, 3, 24));

		assertEquals("=SUBTOTAL(9;$B$19:$B$24)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 24));
		assertEquals(50, SCUtil.getValueFromCell(currentsheet, 1, 24),
				0.000000001);

		// Verify the result on line 26
		assertEquals("Grand Total", SCUtil.getTextFromCell(currentsheet, 3, 25));
		assertEquals("=SUBTOTAL(9;$B$2:$B$25)",
				SCUtil.getFormulaFromCell(currentsheet, 1, 25));
		assertEquals(140, SCUtil.getValueFromCell(currentsheet, 1, 25),
				0.000000001);

	}
}
