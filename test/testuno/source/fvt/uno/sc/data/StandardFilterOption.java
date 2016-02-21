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

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;
import testlib.uno.SCUtil;
import com.sun.star.beans.XPropertySet;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.FilterOperator;
import com.sun.star.sheet.TableFilterField;
import com.sun.star.sheet.XCellAddressable;
import com.sun.star.sheet.XSheetFilterDescriptor;
import com.sun.star.sheet.XSheetFilterable;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.CellAddress;
import com.sun.star.table.XCell;
import com.sun.star.table.XCellRange;
import com.sun.star.table.XColumnRowRange;
import com.sun.star.table.XTableRows;
import com.sun.star.uno.UnoRuntime;


public class StandardFilterOption {
	UnoApp unoApp = new UnoApp();
	XSpreadsheetDocument scDocument = null;
	XComponent scComponent = null;
	private String filename = "FilterTest.xls";

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

	/**
	 * test standard filter with string
	 */
	@Test
	public void testStandardFilterForString() throws Exception {
		// Prepare test data
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
		aFilterFields[0].Field = 0;
		aFilterFields[0].IsNumeric = false;
		aFilterFields[0].Operator = FilterOperator.EQUAL;
		aFilterFields[0].StringValue = "Tom";
		xFilterDesc.setFilterFields(aFilterFields);
		XPropertySet xFilterProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xFilterDesc);
		xFilterProp.setPropertyValue("ContainsHeader", new Boolean(true));
		xFilter.filter(xFilterDesc);

		// Verify filter result
		XColumnRowRange ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:E6"));
		XTableRows Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount() - 1; i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 0 | i == 5) {
				assertTrue("Verify row is invisible.", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertFalse("Verify row is invisible.", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

		// Save and reload the document
		SCUtil.save(scDocument);
		SCUtil.closeFile(scDocument);
		scDocument = SCUtil.openFile(sample, unoApp);

		// Verify the result again
		xdataRange = (XCellRange) UnoRuntime.queryInterface(XCellRange.class, SCUtil.getCurrentSheet(scDocument));
		ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:E6"));
		Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount() - 1; i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 0 | i == 5) {
				assertTrue("Verify row is invisible.", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertFalse("Verify row is invisible.", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

	}

	/**
	 * test standard filter with case sensitive options
	 */
	@Test
	public void testStandardFilterOptionCaseSensitive() throws Exception {
		// Prepare test data
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
		aFilterFields[0].Field = 5;
		aFilterFields[0].IsNumeric = false;
		aFilterFields[0].Operator = FilterOperator.EQUAL;
		aFilterFields[0].StringValue = "No";
		xFilterDesc.setFilterFields(aFilterFields);
		XPropertySet xFilterProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xFilterDesc);
		xFilterProp.setPropertyValue("ContainsHeader", new Boolean(true));
		xFilterProp.setPropertyValue("IsCaseSensitive", false);
		xFilter.filter(xFilterDesc);

		// Verify filter result
		XColumnRowRange ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:F6"));
		XTableRows Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount() - 1; i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 0 | i == 1 | i == 5) {
				assertTrue("Expect should be True", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertFalse("Expect should be false", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

		// Change to CaseSenstive
		xFilterProp.setPropertyValue("IsCaseSensitive", true);
		xFilter.filter(xFilterDesc);

		// Verify result
		for (int i = 0; i < Rows.getCount() - 1; i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 0 | i == 5) {
				assertTrue("Expect should be True", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertFalse("Expect should be false", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

		// Save and reload the document
		SCUtil.save(scDocument);
		SCUtil.closeFile(scDocument);
		scDocument = SCUtil.openFile(sample, unoApp);

		// Verify the result again
		xdataRange = (XCellRange) UnoRuntime.queryInterface(XCellRange.class, SCUtil.getCurrentSheet(scDocument));
		ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:F6"));
		Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount() - 1; i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 0 | i == 5) {
				assertTrue("Expect should be True", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertFalse("Expect should be false", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}
	}

	/**
	 * test standard filter with contain header options
	 */
	@Test
	public void testStandardFilterOptionContainsHeader() throws Exception {
		// Prepare test data
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
		aFilterFields[0].Field = 2;
		aFilterFields[0].IsNumeric = true;
		aFilterFields[0].Operator = FilterOperator.LESS;
		aFilterFields[0].NumericValue = 44;
		xFilterDesc.setFilterFields(aFilterFields);
		XPropertySet xFilterProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xFilterDesc);
		xFilterProp.setPropertyValue("ContainsHeader", new Boolean(true));
		xFilter.filter(xFilterDesc);

		// Verify filter result
		XColumnRowRange ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:E6"));
		XTableRows Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount() - 1; i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 0 | i == 1 | i == 4) {
				assertTrue("Expect result should be True", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertFalse("Expect result should be false", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

		// Change to not contain header
		xFilterProp.setPropertyValue("ContainsHeader", new Boolean(false));
		xFilter.filter(xFilterDesc);

		// Verify result
		for (int i = 0; i < Rows.getCount() - 1; i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 1 | i == 4) {
				assertTrue("Expect result should be True", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertFalse("Expect result should be false", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

		// Save the document
		SCUtil.save(scDocument);
		SCUtil.closeFile(scDocument);
		scDocument = SCUtil.openFile(sample, unoApp);

		// Verify filter result again
		xdataRange = (XCellRange) UnoRuntime.queryInterface(XCellRange.class, SCUtil.getCurrentSheet(scDocument));
		ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:F6"));
		Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount() - 1; i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 1 | i == 4) {
				assertTrue("Expect result should be True", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertFalse("Expect result should be false", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

	}

	/**
	 * test standard filter with copy out put after filter in options
	 */
	@Test
	public void testStandardFilterOptionCopyOutput() throws Exception {
		// Prepare test data
		String sample = Testspace.prepareData(filename);
		// Open document
		scDocument = SCUtil.openFile(sample, unoApp);
		// Get cell range
		XCellRange xdataRange = (XCellRange) UnoRuntime.queryInterface(XCellRange.class, SCUtil.getCurrentSheet(scDocument));
		XSpreadsheet currentSheet = SCUtil.getCurrentSheet(scDocument);
		// Get the value before filter
		String[][] souce = SCUtil.getTextFromCellRange(currentSheet, 0, 0, 5, 5);

		// Copy to cell position
		XCell cell = currentSheet.getCellByPosition(7, 7);
		XCellAddressable xCellAddr = (XCellAddressable) UnoRuntime.queryInterface(XCellAddressable.class, cell);
		CellAddress copytoAddress = xCellAddr.getCellAddress();

		// Set filter property and filter the cell range
		XSheetFilterable xFilter = (XSheetFilterable) UnoRuntime.queryInterface(XSheetFilterable.class, xdataRange.getCellRangeByName("A1:F6"));
		XSheetFilterDescriptor xFilterDesc = xFilter.createFilterDescriptor(true);
		TableFilterField[] aFilterFields = new TableFilterField[1];
		aFilterFields[0] = new TableFilterField();
		aFilterFields[0].Field = 3;
		aFilterFields[0].IsNumeric = true;
		aFilterFields[0].Operator = FilterOperator.GREATER;
		aFilterFields[0].NumericValue = 155;
		xFilterDesc.setFilterFields(aFilterFields);
		XPropertySet xFilterProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xFilterDesc);
		xFilterProp.setPropertyValue("ContainsHeader", new Boolean(true));
		xFilterProp.setPropertyValue("CopyOutputData", new Boolean(true));
		xFilterProp.setPropertyValue("OutputPosition", copytoAddress);
		xFilter.filter(xFilterDesc);

		// Verify source range not changed
		XColumnRowRange ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:F6"));
		XTableRows Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount(); i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			assertTrue("Expect should be True", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

		// Get the data after filter
		String[][] dataafterFilter = SCUtil.getTextFromCellRange(currentSheet, 0, 0, 5, 5);
		assertArrayEquals(souce, dataafterFilter);

		// Get the copyto filter result, verify it
		ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("H8:M10"));
		for (int i = 0; i < Rows.getCount(); i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			assertTrue("Expect should be True", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}
		// Verify the first filter line data
		assertArrayEquals(SCUtil.getTextFromCellRange(currentSheet, 0, 0, 5, 0), SCUtil.getTextFromCellRange(currentSheet, 7, 7, 12, 7));

		// Verify the Second filter line data
		assertArrayEquals(SCUtil.getTextFromCellRange(currentSheet, 0, 1, 5, 1), SCUtil.getTextFromCellRange(currentSheet, 7, 8, 12, 8));

		// Verify the Last filter line data
		assertArrayEquals(SCUtil.getTextFromCellRange(currentSheet, 0, 4, 5, 4), SCUtil.getTextFromCellRange(currentSheet, 7, 9, 12, 9));

		// Save the document
		SCUtil.save(scDocument);
		SCUtil.closeFile(scDocument);
		scDocument = SCUtil.openFile(sample, unoApp);

		// Verify filter result again
		xdataRange = (XCellRange) UnoRuntime.queryInterface(XCellRange.class, SCUtil.getCurrentSheet(scDocument));
		ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:F6"));
		Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount(); i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			assertTrue("Expect should be true", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

		// Get the data after filter
		currentSheet = SCUtil.getCurrentSheet(scDocument);
		dataafterFilter = SCUtil.getTextFromCellRange(currentSheet, 0, 0, 5, 5);
		assertArrayEquals(souce, dataafterFilter);

		// Get the copyto filter result, verify it
		ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("H8:M10"));
		for (int i = 0; i < Rows.getCount(); i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			assertTrue("Expect should be True", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}
		// Verify the first filter line data
		assertArrayEquals(SCUtil.getTextFromCellRange(currentSheet, 0, 0, 5, 0), SCUtil.getTextFromCellRange(currentSheet, 7, 7, 12, 7));

		// Verify the Second filter line data
		assertArrayEquals(SCUtil.getTextFromCellRange(currentSheet, 0, 1, 5, 1), SCUtil.getTextFromCellRange(currentSheet, 7, 8, 12, 8));

		// Verify the Last filter line data
		assertArrayEquals(SCUtil.getTextFromCellRange(currentSheet, 0, 4, 5, 4), SCUtil.getTextFromCellRange(currentSheet, 7, 9, 12, 9));
	}

	/**
	 * test standard filter with skip duplicates in options
	 */
	@Test
	public void testStandardFilterOptionSkipDuplicates() throws Exception {
		// Prepare test data
		String sample = Testspace.prepareData(filename);
		// Open document
		scDocument = SCUtil.openFile(sample, unoApp);
		// Get cell range
		XCellRange xdataRange = (XCellRange) UnoRuntime.queryInterface(XCellRange.class, SCUtil.getCurrentSheet(scDocument));

		// Set filter property and filter the cell range
		XSheetFilterable xFilter = (XSheetFilterable) UnoRuntime.queryInterface(XSheetFilterable.class, xdataRange.getCellRangeByName("A1:E6"));
		XSheetFilterDescriptor xFilterDesc = xFilter.createFilterDescriptor(true);
		TableFilterField[] aFilterFields = new TableFilterField[1];
		aFilterFields[0] = new TableFilterField();
		aFilterFields[0].Field = 3;
		aFilterFields[0].IsNumeric = true;
		aFilterFields[0].Operator = FilterOperator.GREATER_EQUAL;
		aFilterFields[0].NumericValue = 155;
		xFilterDesc.setFilterFields(aFilterFields);
		XPropertySet xFilterProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xFilterDesc);
		xFilterProp.setPropertyValue("ContainsHeader", new Boolean(true));
		xFilterProp.setPropertyValue("SkipDuplicates", new Boolean(true));
		xFilter.filter(xFilterDesc);

		// Verify filter result
		XColumnRowRange ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:E6"));
		XTableRows Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount(); i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 2) {
				assertFalse("Verify row is invisible.", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertTrue("Verify row is invisible.", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

		// Change to skip Dulicates
		xFilterProp.setPropertyValue("SkipDuplicates", new Boolean(false));
		xFilter.filter(xFilterDesc);

		// Verify filter result
		ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:E6"));
		Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount(); i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 2 | i == 6) {
				assertFalse("Expect should be false", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertTrue("Expect should be True", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

		// Save the document
		SCUtil.save(scDocument);
		SCUtil.closeFile(scDocument);
		scDocument = SCUtil.openFile(sample, unoApp);

		// Verify filter result again
		xdataRange = (XCellRange) UnoRuntime.queryInterface(XCellRange.class, SCUtil.getCurrentSheet(scDocument));
		ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:E6"));
		Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount() - 1; i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 2 | i == 6) {
				assertFalse("Expect should be false", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertTrue("Expect should be true", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

	}

	/**
	 * test standard filter with regular expressions in options
	 */
	@Test
	public void testStandardFilterOptionUseRegularExpressions() throws Exception {
		// Prepare test data
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
		aFilterFields[0].Field = 0;
		aFilterFields[0].IsNumeric = false;
		aFilterFields[0].Operator = FilterOperator.EQUAL;
		aFilterFields[0].StringValue = "^.{3}$";
		xFilterDesc.setFilterFields(aFilterFields);
		XPropertySet xFilterProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xFilterDesc);
		xFilterProp.setPropertyValue("ContainsHeader", new Boolean(true));
		xFilterProp.setPropertyValue("UseRegularExpressions", new Boolean(true));
		xFilter.filter(xFilterDesc);

		// Verify filter result
		XColumnRowRange ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:F6"));
		XTableRows Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount(); i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 2 | i == 4) {
				assertFalse("Expect should be false", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertTrue("Expect should be true", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

		// Save the document
		SCUtil.save(scDocument);
		SCUtil.closeFile(scDocument);
		scDocument = SCUtil.openFile(sample, unoApp);

		// Verify filter result again
		xdataRange = (XCellRange) UnoRuntime.queryInterface(XCellRange.class, SCUtil.getCurrentSheet(scDocument));
		ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:F6"));
		Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount(); i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 2 | i == 4) {
				assertFalse("Expect should be false", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertTrue("Expect should be true", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

	}

}
