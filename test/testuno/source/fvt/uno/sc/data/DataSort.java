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
import org.openoffice.test.uno.UnoApp;
import testlib.uno.SCUtil;
import com.sun.star.beans.PropertyValue;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XCellAddressable;
import com.sun.star.sheet.XCellRangeData;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.CellAddress;
import com.sun.star.table.TableSortField;
import com.sun.star.table.XCell;
import com.sun.star.table.XCellRange;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.XSortable;

public class DataSort {
	UnoApp unoApp = new UnoApp();
	XSpreadsheetDocument scDocument = null;
	XComponent scComponent = null;

	@Before
	public void setUpDocument() throws Exception {
		unoApp.start();
		// New a SC document
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

	/**
	 * test single field sort
	 */
	@Test
	public void singleFieldSort() throws Exception {
		scDocument = SCUtil.getSCDocument(scComponent);
		XSpreadsheet currentsheet = SCUtil.getCurrentSheet(scDocument);
		XCellRange sourceRange = currentsheet.getCellRangeByName("A1:E8");
		XCellRangeData sourceData = (XCellRangeData) UnoRuntime.queryInterface(
				XCellRangeData.class, sourceRange);
		Object[][] Source = { { "Level", "Code", "No.", "Team", "Name" },
				{ "BS", 20, 4, "B", "Elle" }, { "BS", 20, 6, "C", "Sweet" },
				{ "BS", 20, 2, "A", "Chcomic" }, { "CS", 30, 5, "A", "Ally" },
				{ "MS", 10, 1, "A", "Joker" }, { "MS", 10, 3, "B", "Kevin" },
				{ "CS", 30, 7, "C", "Tom" } };
		sourceData.setDataArray(Source);

		// define the fields to sort
		TableSortField[] aSortFields = new TableSortField[1];
		aSortFields[0] = new TableSortField();
		aSortFields[0].Field = 1;
		aSortFields[0].IsAscending = true;
		aSortFields[0].IsCaseSensitive = false;

		// define the sort descriptor
		PropertyValue[] aSortDesc = new PropertyValue[2];
		aSortDesc[0] = new PropertyValue();
		aSortDesc[0].Name = "SortFields";
		aSortDesc[0].Value = aSortFields;
		aSortDesc[1] = new PropertyValue();
		aSortDesc[1].Name = "ContainsHeader";
		aSortDesc[1].Value = new Boolean(true);

		// perform the sorting
		XSortable xSort = (XSortable) UnoRuntime.queryInterface(
				XSortable.class, sourceRange);
		xSort.sort(aSortDesc);

		// Verify the sorting result
		String[][] expectResult = { { "Level", "Code", "No.", "Team", "Name" },
				{ "MS", "10", "1", "A", "Joker" },
				{ "MS", "10", "3", "B", "Kevin" },
				{ "BS", "20", "4", "B", "Elle" },
				{ "BS", "20", "6", "C", "Sweet" },
				{ "BS", "20", "2", "A", "Chcomic" },
				{ "CS", "30", "5", "A", "Ally" },
				{ "CS", "30", "7", "C", "Tom" } };
		String[][] actureResult = SCUtil.getTextFromCellRange(currentsheet, 0,
				0, 4, 7);
		assertArrayEquals(expectResult, actureResult);

		// Save and reload the document verify the sort result again
		SCUtil.saveFileAs(scComponent, "SortSingleFiled", "ods");
		XSpreadsheetDocument scDocumentTemp = SCUtil.reloadFile(unoApp,
				scDocument, "SortSingleFiled.ods");
		scDocument = scDocumentTemp;
		currentsheet = SCUtil.getCurrentSheet(scDocument);
		actureResult = SCUtil.getTextFromCellRange(currentsheet, 0, 0, 4, 7);
		assertArrayEquals(expectResult, actureResult);

	}

	/**
	 * test sort with two fields
	 */
	@Test
	public void sortByTwoFields() throws Exception {
		scDocument = SCUtil.getSCDocument(scComponent);
		XSpreadsheet currentsheet = SCUtil.getCurrentSheet(scDocument);
		XCellRange sourceRange = currentsheet.getCellRangeByName("A1:E8");
		XCellRangeData sourceData = (XCellRangeData) UnoRuntime.queryInterface(
				XCellRangeData.class, sourceRange);
		Object[][] Source = { { "Level", "Code", "No.", "Team", "Name" },
				{ "BS", 20, 4, "B", "Elle" }, { "BS", 20, 6, "C", "Sweet" },
				{ "BS", 20, 2, "A", "Chcomic" }, { "CS", 30, 5, "A", "Ally" },
				{ "MS", 10, 1, "A", "Joker" }, { "MS", 10, 3, "B", "Kevin" },
				{ "CS", 30, 7, "C", "Tom" } };
		sourceData.setDataArray(Source);

		// define the fields to sort
		TableSortField[] aSortFields = new TableSortField[2];
		aSortFields[0] = new TableSortField();
		aSortFields[0].Field = 1;
		aSortFields[0].IsAscending = true;
		aSortFields[0].IsCaseSensitive = false;
		aSortFields[1] = new TableSortField();
		aSortFields[1].Field = 2;
		aSortFields[1].IsAscending = false;
		aSortFields[1].IsCaseSensitive = false;

		// define the sort descriptor
		PropertyValue[] aSortDesc = new PropertyValue[2];
		aSortDesc[0] = new PropertyValue();
		aSortDesc[0].Name = "SortFields";
		aSortDesc[0].Value = aSortFields;
		aSortDesc[1] = new PropertyValue();
		aSortDesc[1].Name = "ContainsHeader";
		aSortDesc[1].Value = new Boolean(true);

		// perform the sorting
		XSortable xSort = (XSortable) UnoRuntime.queryInterface(
				XSortable.class, sourceRange);
		xSort.sort(aSortDesc);

		// Verify the sorting result
		String[][] expectResult = { { "Level", "Code", "No.", "Team", "Name" },
				{ "MS", "10", "3", "B", "Kevin" },
				{ "MS", "10", "1", "A", "Joker" },
				{ "BS", "20", "6", "C", "Sweet" },
				{ "BS", "20", "4", "B", "Elle" },
				{ "BS", "20", "2", "A", "Chcomic" },
				{ "CS", "30", "7", "C", "Tom" },
				{ "CS", "30", "5", "A", "Ally" } };
		String[][] actureResult = SCUtil.getTextFromCellRange(currentsheet, 0,
				0, 4, 7);
		assertArrayEquals(expectResult, actureResult);

		// Save and reload the document verify the sort result again
		SCUtil.saveFileAs(scComponent, "SortTwoFileds", "xls");
		XSpreadsheetDocument scDocumentTemp = SCUtil.reloadFile(unoApp,
				scDocument, "SortTwoFileds.xls");
		scDocument = scDocumentTemp;
		currentsheet = SCUtil.getCurrentSheet(scDocument);
		actureResult = SCUtil.getTextFromCellRange(currentsheet, 0, 0, 4, 7);
		assertArrayEquals(expectResult, actureResult);
	}

	
	/**
	 * test sort with three fields
	 */
	@Test
	public void sortByThreeField() throws Exception {
		scDocument = SCUtil.getSCDocument(scComponent);
		XSpreadsheet currentsheet = SCUtil.getCurrentSheet(scDocument);
		XCellRange sourceRange = currentsheet.getCellRangeByName("A1:E8");
		XCellRangeData sourceData = (XCellRangeData) UnoRuntime.queryInterface(
				XCellRangeData.class, sourceRange);
		Object[][] Source = { { "Level", "Code", "No.", "Team", "Name" },
				{ "BS", 20, 4, "B", "Elle" }, { "MS", 20, 6, "C", "Sweet" },
				{ "BS", 20, 2, "A", "Chcomic" }, { "CS", 30, 5, "A", "Ally" },
				{ "MS", 10, 1, "A", "Joker" }, { "MS", 10, 3, "B", "Kevin" },
				{ "CS", 30, 7, "C", "Tom" } };
		sourceData.setDataArray(Source);

		// --- sort by second column, ascending ---

		// define the fields to sort
		TableSortField[] aSortFields = new TableSortField[3];
		aSortFields[0] = new TableSortField();
		aSortFields[0].Field = 0;
		aSortFields[0].IsAscending = true;
		aSortFields[0].IsCaseSensitive = false;
		aSortFields[1] = new TableSortField();
		aSortFields[1].Field = 1;
		aSortFields[1].IsAscending = false;
		aSortFields[1].IsCaseSensitive = false;
		aSortFields[2] = new TableSortField();
		aSortFields[2].Field = 2;
		aSortFields[2].IsAscending = false;
		aSortFields[2].IsCaseSensitive = false;

		// define the sort descriptor
		PropertyValue[] aSortDesc = new PropertyValue[2];
		aSortDesc[0] = new PropertyValue();
		aSortDesc[0].Name = "SortFields";
		aSortDesc[0].Value = aSortFields;
		aSortDesc[1] = new PropertyValue();
		aSortDesc[1].Name = "ContainsHeader";
		aSortDesc[1].Value = new Boolean(true);

		// perform the sorting
		XSortable xSort = (XSortable) UnoRuntime.queryInterface(
				XSortable.class, sourceRange);
		xSort.sort(aSortDesc);

		// Verify the sorting result
		String[][] expectResult = { { "Level", "Code", "No.", "Team", "Name" },
				{ "BS", "20", "4", "B", "Elle" },
				{ "BS", "20", "2", "A", "Chcomic" },
				{ "CS", "30", "7", "C", "Tom" },
				{ "CS", "30", "5", "A", "Ally" },
				{ "MS", "20", "6", "C", "Sweet" },
				{ "MS", "10", "3", "B", "Kevin" },
				{ "MS", "10", "1", "A", "Joker" } };
		String[][] actureResult = SCUtil.getTextFromCellRange(currentsheet, 0,
				0, 4, 7);
		assertArrayEquals(expectResult, actureResult);

		// Save and reload the document verify the sort result again
		SCUtil.saveFileAs(scComponent, "SortThreeFileds", "ods");
		XSpreadsheetDocument scDocumentTemp = SCUtil.reloadFile(unoApp,
				scDocument, "SortThreeFileds.ods");
		scDocument = scDocumentTemp;
		currentsheet = SCUtil.getCurrentSheet(scDocument);
		actureResult = SCUtil.getTextFromCellRange(currentsheet, 0, 0, 4, 7);
		assertArrayEquals(expectResult, actureResult);
	}

	/**
	 * test sort options
	 */
	@Test
	public void sortOption() throws Exception {
		scDocument = SCUtil.getSCDocument(scComponent);
		XSpreadsheet currentsheet = SCUtil.getCurrentSheet(scDocument);
		XCellRange sourceRange = currentsheet.getCellRangeByName("A1:A8");
		XCellRangeData sourceData = (XCellRangeData) UnoRuntime.queryInterface(
				XCellRangeData.class, sourceRange);
		Object[][] source = { { "Fri" }, { "Mon" }, { "Sun" }, { "Wed" },
				{ "Thu" }, { "Sat" }, { "Tue" }, { "SUN" } };
		sourceData.setDataArray(source);

		XCell cell = currentsheet.getCellByPosition(1, 0);
		XCellAddressable xCellAddr = (XCellAddressable) UnoRuntime
				.queryInterface(XCellAddressable.class, cell);
		CellAddress copytoAddress = xCellAddr.getCellAddress();

		// define the fields to sort Sort by column 1and sort Ascending and not
		// case sensitive
		TableSortField[] aSortFields = new TableSortField[1];
		aSortFields[0] = new TableSortField();
		aSortFields[0].Field = 0;
		aSortFields[0].IsAscending = true;
		aSortFields[0].IsCaseSensitive = false;

		// define the sort descriptor
		// Range not contain label,Including formats,copy result to B1, and sort
		// with custom sort order
		PropertyValue[] aSortDesc = new PropertyValue[7];
		aSortDesc[0] = new PropertyValue();
		aSortDesc[0].Name = "SortFields";
		aSortDesc[0].Value = aSortFields;
		aSortDesc[1] = new PropertyValue();
		aSortDesc[1].Name = "ContainsHeader";
		aSortDesc[1].Value = new Boolean(false);
		aSortDesc[2] = new PropertyValue();
		aSortDesc[2].Name = "BindFormatsToContent";
		aSortDesc[2].Value = new Boolean(true);
		aSortDesc[3] = new PropertyValue();
		aSortDesc[3].Name = "IsUserListEnabled";
		aSortDesc[3].Value = new Boolean(true);
		aSortDesc[4] = new PropertyValue();
		aSortDesc[4].Name = "UserListIndex";
		aSortDesc[4].Value = 0;
		aSortDesc[5] = new PropertyValue();
		aSortDesc[5].Name = "CopyOutputData";
		aSortDesc[5].Value = new Boolean(true);
		aSortDesc[6] = new PropertyValue();
		aSortDesc[6].Name = "OutputPosition";
		aSortDesc[6].Value = copytoAddress;

		// perform the sorting
		XSortable xSort = (XSortable) UnoRuntime.queryInterface(
				XSortable.class, sourceRange);
		xSort.sort(aSortDesc);

		// Verify the sorting result
		String[][] expectResult = { { "Sun" }, { "SUN" }, { "Mon" }, { "Tue" },
				{ "Wed" }, { "Thu" }, { "Fri" }, { "Sat" } };
		String[][] actureSortResult = SCUtil.getTextFromCellRange(currentsheet,
				1, 0, 1, 7);
		String[][] sourceAfterSort = SCUtil.getTextFromCellRange(currentsheet,
				0, 0, 0, 7);
		assertArrayEquals(source, sourceAfterSort);
		assertArrayEquals(expectResult, actureSortResult);

		// Save and reload the document verify the sort result again
		SCUtil.saveFileAs(scComponent, "SortOption", "xls");
		XSpreadsheetDocument scDocumentTemp = SCUtil.reloadFile(unoApp,
				scDocument, "SortOption.xls");
		scDocument = scDocumentTemp;
		currentsheet = SCUtil.getCurrentSheet(scDocument);
		actureSortResult = SCUtil
				.getTextFromCellRange(currentsheet, 1, 0, 1, 7);
		sourceAfterSort = SCUtil.getTextFromCellRange(currentsheet, 0, 0, 0, 7);
		assertArrayEquals(source, sourceAfterSort);
		assertArrayEquals(expectResult, actureSortResult);
	}
}
