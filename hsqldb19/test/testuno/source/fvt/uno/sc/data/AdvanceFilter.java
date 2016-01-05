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
import testlib.uno.TestUtil;
import com.sun.star.beans.XPropertySet;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.FilterOperator;
import com.sun.star.sheet.TableFilterField;
import com.sun.star.sheet.XCellRangeData;
import com.sun.star.sheet.XSheetFilterDescriptor;
import com.sun.star.sheet.XSheetFilterable;
import com.sun.star.sheet.XSheetFilterableEx;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.XCellRange;
import com.sun.star.table.XColumnRowRange;
import com.sun.star.table.XTableRows;
import com.sun.star.uno.UnoRuntime;

public class AdvanceFilter {
	private static final UnoApp app = new UnoApp();

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

	@Test
	public void testStandardFilterForString() throws Exception {
		// Prepare test data
		String sample = Testspace.prepareData(filename);
		// Open document
		scDocument = SCUtil.openFile(sample, unoApp);
		// Get cell range
		XCellRange xdataRange = (XCellRange) UnoRuntime.queryInterface(XCellRange.class, SCUtil.getCurrentSheet(scDocument));
		XSpreadsheet currentsheet = SCUtil.getCurrentSheet(scDocument);

		// Get the FilterCrit range and set the filter Critvalue
		XCellRange FilterCritRange = currentsheet.getCellRangeByName("A15:F16");
		XCellRangeData FilterCritData = (XCellRangeData) UnoRuntime.queryInterface(XCellRangeData.class, FilterCritRange);
		Object[][] aCritValues = { { "Name", "Age", "Weight", "Height", "Score", "Graduate" }, { "", "", ">= 44", "", "", "" } };
		FilterCritData.setDataArray(aCritValues);

		// Filter the date
		XSheetFilterable xFilter = (XSheetFilterable) UnoRuntime.queryInterface(XSheetFilterable.class, xdataRange.getCellRangeByName("A1:F6"));

		XSheetFilterableEx xCriteria = (XSheetFilterableEx) UnoRuntime.queryInterface(XSheetFilterableEx.class, xdataRange.getCellRangeByName("A15:F16"));
		XSheetFilterDescriptor xFilterDesc = xCriteria.createFilterDescriptorByObject(xFilter);
		if (xFilterDesc != null)
			xFilter.filter(xFilterDesc);

		// Verify filter result
		XColumnRowRange ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:F6"));
		XTableRows Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount(); i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 1 | i == 4) {
				assertFalse("Expect should be false", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertTrue("Expect should be true", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

		// Save and reload the document
		SCUtil.save(scDocument);
		SCUtil.closeFile(scDocument);
		scDocument = SCUtil.openFile(sample, unoApp);

		// Verify the result agains
		xdataRange = (XCellRange) UnoRuntime.queryInterface(XCellRange.class, SCUtil.getCurrentSheet(scDocument));
		ColRowRange = (XColumnRowRange) UnoRuntime.queryInterface(XColumnRowRange.class, xdataRange.getCellRangeByName("A1:F6"));
		Rows = ColRowRange.getRows();
		for (int i = 0; i < Rows.getCount(); i++) {
			Object aRowObj = Rows.getByIndex(i);
			XPropertySet PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);
			if (i == 1 | i == 4) {
				assertFalse("Expect should be false", (Boolean) PropSet.getPropertyValue("IsVisible"));
			} else
				assertTrue("Expect should be true", (Boolean) PropSet.getPropertyValue("IsVisible"));
		}

	}
}
