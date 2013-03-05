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
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SCUtil;

import com.sun.star.beans.XPropertySet;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.FilterOperator;
import com.sun.star.sheet.TableFilterField;
import com.sun.star.sheet.XSheetFilterDescriptor;
import com.sun.star.sheet.XSheetFilterable;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.XCellRange;
import com.sun.star.table.XColumnRowRange;
import com.sun.star.table.XTableRows;
import com.sun.star.uno.Enum;
import com.sun.star.uno.UnoRuntime;

@RunWith(value = Parameterized.class)
public class StardarFilterTest {
	private static final UnoApp app = new UnoApp();
	UnoApp unoApp = new UnoApp();
	XSpreadsheetDocument scDocument = null;
	XComponent scComponent = null;
	private String filename = "FilterTest.xls";
	private Enum operator;
	private int column;
	private boolean[] rowshow;
	private int value;

	@Parameters
	public static Collection<Object[]> data() throws Exception {
		boolean[] rowshow1 = { true, true, false, true, false, false };
		boolean[] rowshow2 = { true, true, true, true, true, false };
		boolean[] rowshow3 = { true, false, false, true, false, false };
		boolean[] rowshow4 = { true, false, true, true, false, false };
		boolean[] rowshow5 = { true, false, false, false, false, true };
		boolean[] rowshow6 = { true, false, true, true, false, true };
		boolean[] rowshow7 = { true, false, true, false, false, false };
		boolean[] rowshow8 = { true, false, true, true, false, true };
		boolean[] rowshow9 = { true, true, true, false, true, true };
		boolean[] rowshow10 = { true, true, false, false, true, true };
		boolean[] rowshow11 = { true, true, false, false, false, false };
		boolean[] rowshow12 = { true, true, true, true, false, false };

		return Arrays.asList(new Object[][] { { FilterOperator.BOTTOM_PERCENT, 1, 30, rowshow1 }, { FilterOperator.BOTTOM_VALUES, 2, 4, rowshow2 },
				{ FilterOperator.EMPTY, 5, 1, rowshow3 }, { FilterOperator.EQUAL, 2, 44, rowshow4 }, { FilterOperator.GREATER, 2, 44, rowshow5 },
				{ FilterOperator.GREATER_EQUAL, 2, 44, rowshow6 }, { FilterOperator.LESS, 3, 155, rowshow7 }, { FilterOperator.LESS_EQUAL, 3, 155, rowshow8 },
				{ FilterOperator.NOT_EMPTY, 5, 44, rowshow9 }, { FilterOperator.NOT_EQUAL, 4, 85, rowshow10 }, { FilterOperator.TOP_PERCENT, 4, 30, rowshow11 },
				{ FilterOperator.TOP_VALUES, 4, 3, rowshow12 }, });
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
	public static void tearDownConnection() throws InterruptedException, Exception {

	}

	public StardarFilterTest(Enum operator, int column, int value, boolean[] rowshow) {
		this.operator = operator;
		this.column = column;
		this.rowshow = rowshow;
		this.value = value;
	}

	@Test
	public void testStandardFilter() throws Exception {
		String sample = Testspace.prepareData(filename);

		// Open document
		scDocument = SCUtil.openFile(sample, unoApp);

		// Get cell range
		XCellRange xdataRange = (XCellRange) UnoRuntime.queryInterface(XCellRange.class, SCUtil.getCurrentSheet(scDocument));

		// Set filter property and filter the cell range
		XSheetFilterable xFilter = (XSheetFilterable) UnoRuntime.queryInterface(XSheetFilterable.class, xdataRange.getCellRangeByName("A1:F6"));
		XSheetFilterDescriptor xFilterDesc = xFilter.createFilterDescriptor(true);
		TableFilterField[] aFilterFields = new TableFilterField[1];
		aFilterFields[0] = new TableFilterField();
		aFilterFields[0].Field = column;
		aFilterFields[0].IsNumeric = true;
		aFilterFields[0].Operator = (FilterOperator) operator;
		aFilterFields[0].NumericValue = value;
		xFilterDesc.setFilterFields(aFilterFields);
		XPropertySet xFilterProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xFilterDesc);
		xFilterProp.setPropertyValue("ContainsHeader", new Boolean(true));
		xFilter.filter(xFilterDesc);

		// Verify filter result
		XColumnRowRange ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:F6"));
		XTableRows Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount() - 1; i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			assertEquals(rowshow[i], (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

		// Save and reload the document
		SCUtil.save(scDocument);
		SCUtil.closeFile(scDocument);
		scDocument = SCUtil.openFile(sample, unoApp);

		// Verify the result agains
		xdataRange = (XCellRange) UnoRuntime.queryInterface(XCellRange.class, SCUtil.getCurrentSheet(scDocument));
		ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:F6"));
		Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount() - 1; i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			assertEquals(rowshow[i], (Boolean) PropSet.getPropertyValue("IsVisible"));
		}
		SCUtil.save(scDocument);
	}
}
