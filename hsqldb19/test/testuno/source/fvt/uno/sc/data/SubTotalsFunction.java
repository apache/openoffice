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
package fvt.uno.sc.data;

import static org.junit.Assert.*;

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
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.GeneralFunction;
import com.sun.star.sheet.SubTotalColumn;
import com.sun.star.sheet.XCellRangeData;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSubTotalCalculatable;
import com.sun.star.sheet.XSubTotalDescriptor;
import com.sun.star.table.XCellRange;
import com.sun.star.uno.Enum;
import com.sun.star.uno.UnoRuntime;

@RunWith(value = Parameterized.class)
public class SubTotalsFunction {
	private static final UnoApp app = new UnoApp();

	UnoApp unoApp = new UnoApp();
	XSpreadsheetDocument scDocument = null;
	XComponent scComponent = null;

	private GeneralFunction operator;

	private String operatorString;

	private int operatorvalue;

	private double bssubtotalresult;

	private double cssubtotalresult;

	private double mssubtotalresult;

	private double grandtotal;

	@Parameters
	public static Collection<Object[]> data() throws Exception {
		// Remove GeneralFunction.Auto,GeneralFunction.NONE
		return Arrays.asList(new Object[][] {
				{ GeneralFunction.SUM, "Sum", 9, 12, 12, 4, 28 },
				{ GeneralFunction.AVERAGE, "Average", 1, 4, 6, 2, 4 },
				{ GeneralFunction.COUNT, "Count", 3, 3, 2, 2, 7 },
				{ GeneralFunction.COUNTNUMS, "Count", 2, 3, 2, 2, 7 },
				{ GeneralFunction.MAX, "Max", 4, 6, 7, 3, 7 },
				{ GeneralFunction.MIN, "Min", 5, 2, 5, 1, 1 },
				{ GeneralFunction.VAR, "Var", 10, 4, 2, 2, 4.666666667 },
				{ GeneralFunction.PRODUCT, "Product", 6, 48, 35, 3, 5040 },
				{ GeneralFunction.STDEVP, "StDev", 8, 1.6329931619, 1, 1, 2 },
				{ GeneralFunction.STDEV, "StDev", 7, 2, 1.4142135624,
						1.4142135624, 2.1602468995 },
				{ GeneralFunction.VARP, "Var", 11, 2.6666666667, 1, 1, 4 }, });
	}

	@Before
	public void setUpDocument() throws Exception {
		unoApp.start();
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

	public SubTotalsFunction(Enum operator, String operatorString,
			int operatorvalue, double bssubtotalresult,
			double cssubtotalresult, double mssubtotalresult, double grandtotal) {
		this.operator = (GeneralFunction) operator;
		this.operatorString = operatorString;
		this.operatorvalue = operatorvalue;
		this.bssubtotalresult = bssubtotalresult;
		this.cssubtotalresult = cssubtotalresult;
		this.mssubtotalresult = mssubtotalresult;
		this.grandtotal = grandtotal;
	}

	@Test
	public void test() throws Exception {
		// New document and input data in document
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
		// calculate sum of third column
		aColumns[0] = new SubTotalColumn();
		aColumns[0].Column = 2;
		aColumns[0].Function = operator;
		// group by first column
		xSubDesc.addNew(aColumns, 0);
		xSub.applySubTotals(xSubDesc, true);

		// Verify BS SubTotals result
		String BSsubtotals = "=SUBTOTAL(" + operatorvalue + ";$C$2:$C$4)";
		String BSsubtotalsString = "BS " + operatorString;

		assertEquals(bssubtotalresult,
				SCUtil.getValueFromCell(currentsheet, 2, 4), 0.000000001);
		assertEquals(BSsubtotals, SCUtil.getFormulaFromCell(currentsheet, 2, 4));
		assertEquals(BSsubtotalsString,
				SCUtil.getTextFromCell(currentsheet, 0, 4));

		// Verify CS SubTotals result
		String CSsubtotals = "=SUBTOTAL(" + operatorvalue + ";$C$6:$C$7)";
		String CSsubtotalsString = "CS " + operatorString;

		assertEquals(cssubtotalresult,
				SCUtil.getValueFromCell(currentsheet, 2, 7), 0.000000001);
		assertEquals(CSsubtotals, SCUtil.getFormulaFromCell(currentsheet, 2, 7));
		assertEquals(CSsubtotalsString,
				SCUtil.getTextFromCell(currentsheet, 0, 7));

		// Verify MS SubTotals result
		String MSsubtotals = "=SUBTOTAL(" + operatorvalue + ";$C$9:$C$10)";
		String MSsubtotalsString = "MS " + operatorString;

		assertEquals(mssubtotalresult,
				SCUtil.getValueFromCell(currentsheet, 2, 10), 0.000000001);
		assertEquals(MSsubtotals,
				SCUtil.getFormulaFromCell(currentsheet, 2, 10));
		assertEquals(MSsubtotalsString,
				SCUtil.getTextFromCell(currentsheet, 0, 10));

		// Verify GrandTotal result
		String GTsubtotals = "=SUBTOTAL(" + operatorvalue + ";$C$2:$C$11)";
		String GTsubtotalsString = "Grand Total";

		assertEquals(grandtotal, SCUtil.getValueFromCell(currentsheet, 2, 11),
				0.000000001);
		assertEquals(GTsubtotals,
				SCUtil.getFormulaFromCell(currentsheet, 2, 11));
		assertEquals(GTsubtotalsString,
				SCUtil.getTextFromCell(currentsheet, 0, 11));

		// Save the file and reload it
		SCUtil.saveFileAs(scComponent, "Subtotals", "ods");
		XSpreadsheetDocument scDocumentTemp = SCUtil.reloadFile(unoApp,
				scDocument, "Subtotals.ods");
		scDocument = scDocumentTemp;
		currentsheet = SCUtil.getCurrentSheet(scDocument);

		// verify it again
		// Verify BS SubTotals result
		BSsubtotals = "=SUBTOTAL(" + operatorvalue + ";$C$2:$C$4)";
		BSsubtotalsString = "BS " + operatorString;

		assertEquals(bssubtotalresult,
				SCUtil.getValueFromCell(currentsheet, 2, 4), 0.000000001);
		assertEquals(BSsubtotals, SCUtil.getFormulaFromCell(currentsheet, 2, 4));
		assertEquals(BSsubtotalsString,
				SCUtil.getTextFromCell(currentsheet, 0, 4));

		// Verify CS SubTotals result
		CSsubtotals = "=SUBTOTAL(" + operatorvalue + ";$C$6:$C$7)";
		CSsubtotalsString = "CS " + operatorString;

		assertEquals(cssubtotalresult,
				SCUtil.getValueFromCell(currentsheet, 2, 7), 0.000000001);
		assertEquals(CSsubtotals, SCUtil.getFormulaFromCell(currentsheet, 2, 7));
		assertEquals(CSsubtotalsString,
				SCUtil.getTextFromCell(currentsheet, 0, 7));

		// Verify MS SubTotals result
		MSsubtotals = "=SUBTOTAL(" + operatorvalue + ";$C$9:$C$10)";
		MSsubtotalsString = "MS " + operatorString;

		assertEquals(mssubtotalresult,
				SCUtil.getValueFromCell(currentsheet, 2, 10), 0.000000001);
		assertEquals(MSsubtotals,
				SCUtil.getFormulaFromCell(currentsheet, 2, 10));
		assertEquals(MSsubtotalsString,
				SCUtil.getTextFromCell(currentsheet, 0, 10));

		// Verify GrandTotal result
		GTsubtotals = "=SUBTOTAL(" + operatorvalue + ";$C$2:$C$11)";
		GTsubtotalsString = "Grand Total";

		assertEquals(grandtotal, SCUtil.getValueFromCell(currentsheet, 2, 11),
				0.000000001);
		assertEquals(GTsubtotals,
				SCUtil.getFormulaFromCell(currentsheet, 2, 11));
		assertEquals(GTsubtotalsString,
				SCUtil.getTextFromCell(currentsheet, 0, 11));
	}
}
