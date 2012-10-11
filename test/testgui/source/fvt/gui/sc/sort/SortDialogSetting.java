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

/**
 * 
 */
package fvt.gui.sc.sort;

import static org.junit.Assert.*;
import static org.openoffice.test.common.Testspace.*;
import static org.openoffice.test.vcl.Tester.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Logger;

import testlib.gui.SCTool;

/**
 * Test Data->Sort dialog setting
 */
public class SortDialogSetting {

	@Rule
	public Logger log = Logger.getLogger(this);

	@Before
	public void setUp() throws Exception {
		app.start(true);

		// Create a new spreadsheet document
		app.dispatch("private:factory/scalc");
		calc.waitForExistence(10, 2);
	}

	@After
	public void tearDown() throws Exception {

	}

	/**
	 * Test sort with options setting: case sensitive
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSortOptionsCaseSensitive() throws Exception {

		// Input some data: A1~A6: 1ColumnName,D,C,B,A,a
		String[][] data = new String[][] { { "D" }, { "C" }, { "B" }, { "A" }, { "a" }, };
		String[][] expectedSortedResult = new String[][] { { "a" }, { "A" }, { "B" }, { "C" }, { "D" }, };
		SCTool.selectRange("A1");
		typeKeys("1ColumnName<down>D<down>C<down>B<down>A<down>a<down>");
		SCTool.selectRange("A6");
		app.dispatch(".uno:ChangeCaseToLower"); // In case SC capitalize first
												// letter automatically

		// "Data->Sort...", choose "Ascending", check "Case sensitive"
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageCaseSensitive.check();
		sortOptionsPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result", expectedSortedResult, SCTool.getCellTexts("A2:A6"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", data, SCTool.getCellTexts("A2:A6"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedSortedResult, SCTool.getCellTexts("A2:A6"));

		// Save and close document
		String saveTo = getPath("temp/" + "RowsSortWithOptionsCaseSensitive.ods");
		app.dispatch(".uno:SaveAs");
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		app.dispatch(".uno:CloseDoc");
		openStartcenter();

		// Reopen and verify sorted result
		app.dispatch(".uno:Open");
		submitOpenDlg(saveTo);
		calc.waitForExistence(10, 2);
		assertArrayEquals("Saved sorted result", expectedSortedResult, SCTool.getCellTexts("A2:A6"));
	}

	/**
	 * Test sort with options setting: copy sort result to
	 * 
	 * @throws Exception
	 */
	@Ignore("Bug #119035")
	public void testSortOptionsCopyResultTo() throws Exception {

		// Input some data
		// String[][] data = new String[][] {
		// {"3", "D"},
		// {"5", "FLK"},
		// {"4", "E"},
		// {"2", "BC"},
		// {"6", "GE"},
		// {"1", "AB"},
		// };
		String[][] expectedSortedResult = new String[][] { { "1", "AB" }, { "2", "BC" }, { "3", "D" }, { "4", "E" }, { "5", "FLK" }, { "6", "GE" }, };
		SCTool.selectRange("A1");
		typeKeys("3<down>5<down>4<down>2<down>6<down>1");
		SCTool.selectRange("B1");
		typeKeys("D<down>FLK<down>E<down>BC<down>GE<down>AB");

		// Data->Sort..., choose "Ascending", check "Copy sort results to:"
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageCopyResultTo.check();
		sortOptionsPageCopyResultToEdit.setText("$Sheet3.$A4");
		sortOptionsPage.ok();

		// Verify sorted result
		assertArrayEquals("Copy sorted result to", expectedSortedResult, SCTool.getCellTexts("$Sheet3.$A4:$B9"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertEquals("Undo sorted result", "", SCTool.getCellText("$Sheet3.$A4"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedSortedResult, SCTool.getCellTexts("$Sheet3.$A4:$B9"));

		// Move focus to sheet2 then select a cell range,
		// Insert->Names->Define...
		SCTool.selectRange("$Sheet2.$A1:$B3");
		app.dispatch(".uno:DefineName");
		defineNamesDlgNameEdit.setText("cellRange");
		defineNamesDlg.ok();

		// Set focus to the original data, Data->Sort...
		SCTool.selectRange("$Sheet1.$B1");
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageCopyResultTo.check();
		sortOptionsPageCopyResultToCellRange.select("cellRange");
		sortOptionsPage.ok();

		// Verify sorted result
		assertArrayEquals("Copy sorted result to cell range", expectedSortedResult, SCTool.getCellTexts("$Sheet2.$A1:$B6"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertEquals("Undo sorted result", "", SCTool.getCellText("$Sheet2.$A1"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedSortedResult, SCTool.getCellTexts("$Sheet2.$A1:$B6"));

		// Save and close document
		String saveTo = getPath("temp/" + "RowsSortWithOptionsCopyResultTo.ods");
		app.dispatch(".uno:SaveAs");
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		app.dispatch(".uno:CloseDoc");
		openStartcenter();

		// Reopen and verify sorted result
		app.dispatch(".uno:Open");
		submitOpenDlg(saveTo);
		calc.waitForExistence(10, 2);
		assertArrayEquals("Saved sorted result", expectedSortedResult, SCTool.getCellTexts("$Sheet3.$A4:$B9"));
		assertArrayEquals("Saved sorted result to cell range", expectedSortedResult, SCTool.getCellTexts("$Sheet2.$A1:$B6"));
	}

	/**
	 * Test sort criteria: sort first by
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSortCriteriaSortFirstBy() throws Exception {

		// Input some data
		String[][] data = new String[][] { { "3", "D" }, { "5", "FLK" }, { "4", "E" }, { "2", "BC" }, { "6", "GE" }, { "1", "AB" }, };
		String[][] expectedResultSortByColumnBAscending = new String[][] { { "1", "AB" }, { "2", "BC" }, { "3", "D" }, { "4", "E" }, { "5", "FLK" }, { "6", "GE" }, };
		String[][] expectedResultSortByColumnADescending = new String[][] { { "6", "GE" }, { "5", "FLK" }, { "4", "E" }, { "3", "D" }, { "2", "BC" }, { "1", "AB" }, };
		SCTool.selectRange("A1");
		typeKeys("3<down>5<down>4<down>2<down>6<down>1");
		SCTool.selectRange("B1");
		typeKeys("D<down>FLK<down>E<down>BC<down>GE<down>AB");

		// "Data->Sort...", choose "Ascending", sort first by Column B
		app.dispatch(".uno:DataSort");
		sortPageAscending1.check();
		sortPageBy1.select(2); // "Column B"
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result", expectedResultSortByColumnBAscending, SCTool.getCellTexts("A1:B6"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", data, SCTool.getCellTexts("A1:B6"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedResultSortByColumnBAscending, SCTool.getCellTexts("A1:B6"));
		app.dispatch(".uno:Undo");

		// Save and close document
		String saveTo = getPath("temp/" + "SortCriteriaSortFirstBy.ods");
		app.dispatch(".uno:SaveAs");
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		app.dispatch(".uno:CloseDoc");
		openStartcenter();

		// Reopen, "Data->Sort...", choose "Descending", sort first by Column A
		app.dispatch(".uno:Open");
		submitOpenDlg(saveTo);
		calc.waitForExistence(10, 2);
		app.dispatch(".uno:DataSort");
		sortPageDescending1.check();
		sortPageBy1.select(1); // "Column A"
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Saved sorted result", expectedResultSortByColumnADescending, SCTool.getCellTexts("A1:B6"));
	}

	/**
	 * Test sort criteria: sort second by
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSortCriteriaSortSecondBy() throws Exception {

		// Input some data
		String[][] data = new String[][] { { "3", "D" }, { "5", "FLK" }, { "4", "E" }, { "1", "AB" }, { "6", "GE" }, { "2", "AB" }, };
		String[][] expectedResultSortFirstByB = new String[][] { { "1", "AB" }, { "2", "AB" }, { "3", "D" }, { "4", "E" }, { "5", "FLK" }, { "6", "GE" }, };
		String[][] expectedResultSortSecondByA = new String[][] { { "2", "AB" }, { "1", "AB" }, { "3", "D" }, { "4", "E" }, { "5", "FLK" }, { "6", "GE" }, };
		SCTool.selectRange("A1");
		typeKeys("3<down>5<down>4<down>1<down>6<down>2");
		SCTool.selectRange("B1");
		typeKeys("D<down>FLK<down>E<down>AB<down>GE<down>AB");

		// "Data->Sort...", choose "Ascending", sort first by Column B
		app.dispatch(".uno:DataSort");
		sortPageAscending1.check();
		sortPageBy1.select(2); // "Column B"
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result", expectedResultSortFirstByB, SCTool.getCellTexts("A1:B6"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", data, SCTool.getCellTexts("A1:B6"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedResultSortFirstByB, SCTool.getCellTexts("A1:B6"));
		app.dispatch(".uno:Undo");

		// Save and close document
		String saveTo = getPath("temp/" + "SortCriteriaSortSecondBy.ods");
		app.dispatch(".uno:SaveAs");
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		app.dispatch(".uno:CloseDoc");
		openStartcenter();

		// Reopen, "Data->Sort...", sort first by Column B "Ascending", sort
		// second by Column A "Descending"
		app.dispatch(".uno:Open");
		submitOpenDlg(saveTo);
		calc.waitForExistence(10, 2);
		app.dispatch(".uno:DataSort");
		sortPageBy1.select(2); // "Column B"
		sortPageAscending1.check();
		sortPageBy2.select(1); // "Column A"
		sortPageDescending2.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Saved sorted result", expectedResultSortSecondByA, SCTool.getCellTexts("A1:B6"));
	}

	/**
	 * Test sort criteria: sort third by
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSortCriteriaSortThirdBy() throws Exception {

		// Input some data
		String[][] data = new String[][] { { "3", "AB", "2" }, { "8", "FLK", "5" }, { "6", "E", "4" }, { "1", "AB", "1" }, { "9", "GE", "6" }, { "2", "AB", "2" },
				{ "7", "EFYU", "7" }, { "5", "DS", "8" }, { "4", "AB", "1" }, };
		String[][] expectedResultSortFirstByB = new String[][] { { "3", "AB", "2" }, { "1", "AB", "1" }, { "2", "AB", "2" }, { "4", "AB", "1" }, { "5", "DS", "8" },
				{ "6", "E", "4" }, { "7", "EFYU", "7" }, { "8", "FLK", "5" }, { "9", "GE", "6" }, };
		String[][] expectedResultSortSecondByC = new String[][] { { "3", "AB", "2" }, { "2", "AB", "2" }, { "1", "AB", "1" }, { "4", "AB", "1" }, { "5", "DS", "8" },
				{ "6", "E", "4" }, { "7", "EFYU", "7" }, { "8", "FLK", "5" }, { "9", "GE", "6" }, };
		String[][] expectedResultSortThirdByA = new String[][] { { "3", "AB", "2" }, { "2", "AB", "2" }, { "4", "AB", "1" }, { "1", "AB", "1" }, { "5", "DS", "8" },
				{ "6", "E", "4" }, { "7", "EFYU", "7" }, { "8", "FLK", "5" }, { "9", "GE", "6" }, };
		SCTool.selectRange("A1");
		typeKeys("3<down>8<down>6<down>1<down>9<down>2<down>7<down>5<down>4");
		SCTool.selectRange("B1");
		typeKeys("AB<down>FLK<down>E<down>AB<down>GE<down>AB<down>EFYU<down>DS<down>AB");
		SCTool.selectRange("C1");
		typeKeys("2<down>5<down>4<down>1<down>6<down>2<down>7<down>8<down>1");

		// "Data->Sort...", choose "Ascending", sort first by Column B
		app.dispatch(".uno:DataSort");
		sortPageBy1.select(2); // "Column B"
		sortPageAscending1.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result", expectedResultSortFirstByB, SCTool.getCellTexts("A1:C9"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", data, SCTool.getCellTexts("A1:C9"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedResultSortFirstByB, SCTool.getCellTexts("A1:C9"));
		app.dispatch(".uno:Undo");

		// Save and close document
		String saveTo = getPath("temp/" + "SortCriteriaSortThirdBy.ods");
		app.dispatch(".uno:SaveAs");
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		app.dispatch(".uno:CloseDoc");
		openStartcenter();

		// Reopen, "Data->Sort...", sort first by Column B "Ascending", sort
		// second by Column C "Descending"
		app.dispatch(".uno:Open");
		submitOpenDlg(saveTo);
		calc.waitForExistence(10, 2);
		app.dispatch(".uno:DataSort");
		sortPageBy1.select(2); // "Column B"
		sortPageAscending1.check();
		sortPageBy2.select(3); // "Column C"
		sortPageDescending2.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result", expectedResultSortSecondByC, SCTool.getCellTexts("A1:C9"));

		// "Data->Sort...", sort first by Column B "Ascending", sort second by
		// Column C "Descending", sort third by Column A "Descending"
		app.dispatch(".uno:DataSort");
		sortPageBy1.select(2); // "Column B"
		sortPageAscending1.check();
		sortPageBy2.select(3); // "Column C"
		sortPageDescending2.check();
		sortPageBy3.select(1); // "Column A"
		sortPageDescending3.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result", expectedResultSortThirdByA, SCTool.getCellTexts("A1:C9"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", expectedResultSortSecondByC, SCTool.getCellTexts("A1:C9"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedResultSortThirdByA, SCTool.getCellTexts("A1:C9"));

		// Save and close document
		saveTo = getPath("temp/" + "SortCriteriaSortThirdBy1.ods");
		app.dispatch(".uno:SaveAs");
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		app.dispatch(".uno:CloseDoc");
		openStartcenter();

		// Reopen and verify data sort is not lost
		app.dispatch(".uno:Open");
		submitOpenDlg(saveTo);
		calc.waitForExistence(10, 2);
		assertArrayEquals("Saved sorted result", expectedResultSortThirdByA, SCTool.getCellTexts("A1:C9"));
	}

	/**
	 * Test sort options: custom sort order, predefined in preferences from copy
	 * list
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSortOptionsCustomSortOrderPredefineFromCopyList() throws Exception {

		// Dependencies start
		SCTool.selectRange("A1");
		typeKeys("red<down>yellow<down>blue<down>green<down>white<down>black");
		SCTool.selectRange("A1:A6");
		app.dispatch(".uno:ChangeCaseToLower"); // In case SC capitalize first
												// letter automatically

		// Select the cell range,
		// "Tools->Options...->OpenOffice.org Spreadsheets->Sort Lists"
		SCTool.selectRange("A1:A6");
		app.dispatch(".uno:OptionsTreeDialog");
		optionsDlgList.collapseAll();
		// // Select "Sort Lists": start. Shrink the tree list and select
		// OptionsDlgList.select(0);
		// typeKeys("<left>");
		// for (int i=0; i<6; i++) {
		// typeKeys("<down><left>");
		// }
		optionsDlgList.expand(3);
		// typeKeys("<right>");
		optionsDlgList.select(7);
		// // Select "Sort Lists": end

		// Click "Copy" button, "OK", close the document
		optionsDlgSortListsTabCopy.click();
		optionsDlg.ok();
		app.dispatch(".uno:CloseDoc");
		msgBox_AdditionalRowsNotSaved.no();
		// Dependencies end

		// Create a new spreadsheet document
		app.dispatch("private:factory/scalc");

		// Input some data
		String[][] data = new String[][] { { "Color" }, { "black" }, { "yellow" }, { "blue" }, { "black" }, { "white" }, { "red" }, };
		String[][] expectedResultNoCustomSortOrder = new String[][] { { "Color" }, { "black" }, { "black" }, { "blue" }, { "red" }, { "white" }, { "yellow" }, };
		String[][] expectedResultCustomSortOrder = new String[][] { { "Color" }, { "red" }, { "yellow" }, { "blue" }, { "white" }, { "black" }, { "black" }, };
		SCTool.selectRange("A1");
		typeKeys("Color<down>black<down>yellow<down>blue<down>black<down>white<down>red");
		SCTool.selectRange("A2:A7");
		app.dispatch(".uno:ChangeCaseToLower"); // In case SC capitalize first
												// letter automatically

		// "Data->Sort...", "Options" tab, check "Range contains column labels",
		// no custom sort order, "Ascending", sort first by Color
		SCTool.selectRange("A1:A7");
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageRangeContainsColumnLabels.check();
		sortOptionsPageCustomSortOrder.uncheck();
		sortPage.select();
		sortPageBy1.select(1); // "Color"
		sortPageAscending1.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result without custom sort order", expectedResultNoCustomSortOrder, SCTool.getCellTexts("A1:A7"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", data, SCTool.getCellTexts("A1:A7"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedResultNoCustomSortOrder, SCTool.getCellTexts("A1:A7"));
		app.dispatch(".uno:Undo");

		// Copy original data to sheet2
		SCTool.selectRange("A1:A7");
		app.dispatch(".uno:Copy");
		app.dispatch(".uno:SelectTables");
		scSheetsList.select(1);
		scSelectSheetsDlg.ok();
		app.dispatch(".uno:Paste");

		// "Data->Sort...", "Options" tab, check "Range contains column labels",
		// choose custom sort order, "Ascending", sort first by Color
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageRangeContainsColumnLabels.check();
		sortOptionsPageCustomSortOrder.check();
		sortOptionsPageCustomSortOrderList.select("red,yellow,blue,green,white,black");
		sortPage.select();
		sortPageBy1.select(1); // "Color"
		sortPageAscending1.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result with custom sort order", expectedResultCustomSortOrder, SCTool.getCellTexts("A1:A7"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", data, SCTool.getCellTexts("A1:A7"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedResultCustomSortOrder, SCTool.getCellTexts("A1:A7"));

		// Save and close document
		String saveTo = getPath("temp/" + "SortOptionsCustomSortOrderPredefineFromCopyList.ods");
		app.dispatch(".uno:SaveAs");
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		app.dispatch(".uno:CloseDoc");
		openStartcenter();

		// Reopen and verify sorted result
		app.dispatch(".uno:Open");
		submitOpenDlg(saveTo);
		calc.waitForExistence(10, 2);
		app.dispatch(".uno:SelectTables");
		scSheetsList.select(0); // Sheet 1
		scSelectSheetsDlg.ok();
		assertArrayEquals("Original data", data, SCTool.getCellTexts("$A1:$A7"));
		app.dispatch(".uno:SelectTables");
		scSheetsList.select(1); // Sheet 2
		scSelectSheetsDlg.ok();
		assertArrayEquals("Saved sorted result", expectedResultCustomSortOrder, SCTool.getCellTexts("$A1:$A7"));
	}

	/**
	 * Test sort options: custom sort order, predefined in preferences from new
	 * list
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSortOptionsCustomSortOrderPredefineFromNewList() throws Exception {

		// Dependencies start
		// "Tools->Options...->OpenOffice.org Spreadsheets->Sort Lists"
		app.dispatch(".uno:OptionsTreeDialog");
		// Select "Sort Lists": start. Shrink the tree list and select
		optionsDlgList.select(0);
		typeKeys("<left>");
		for (int i = 0; i < 6; i++) {
			typeKeys("<down><left>");
		}
		optionsDlgList.select(3);
		typeKeys("<right>");
		optionsDlgList.select(7);
		// Select "Sort Lists": end

		// Click "New" button, input "white,red,yellow,blue,green,black", press
		// "Add" and "OK", close the document
		optionsDlgSortListsTabNew.click();
		typeKeys("white,red,yellow,blue,green,black");
		optionsDlgSortListsTabAdd.click();
		optionsDlg.ok();
		app.dispatch(".uno:CloseDoc");
		// Dependencies end

		// Create a new spreadsheet document
		app.dispatch("private:factory/scalc");

		// Input some data
		String[][] data = new String[][] { { "Color" }, { "black" }, { "yellow" }, { "blue" }, { "black" }, { "white" }, { "red" }, };
		String[][] expectedResultNoCustomSortOrder = new String[][] { { "Color" }, { "black" }, { "black" }, { "blue" }, { "red" }, { "white" }, { "yellow" }, };
		String[][] expectedResultCustomSortOrder = new String[][] { { "Color" }, { "white" }, { "red" }, { "yellow" }, { "blue" }, { "black" }, { "black" }, };
		SCTool.selectRange("A1");
		typeKeys("Color<down>black<down>yellow<down>blue<down>black<down>white<down>red");
		SCTool.selectRange("A2:A7");
		app.dispatch(".uno:ChangeCaseToLower"); // In case SC capitalize first
												// letter automatically

		// "Data->Sort...", "Options" tab, check "Range contains column labels",
		// no custom sort order, "Ascending", sort first by Color
		SCTool.selectRange("A1:A7");
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageRangeContainsColumnLabels.check();
		sortOptionsPageCustomSortOrder.uncheck();
		sortPage.select();
		sortPageBy1.select(1); // "Color"
		sortPageAscending1.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result without custom sort order", expectedResultNoCustomSortOrder, SCTool.getCellTexts("A1:A7"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", data, SCTool.getCellTexts("A1:A7"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedResultNoCustomSortOrder, SCTool.getCellTexts("A1:A7"));
		app.dispatch(".uno:Undo");

		// Copy original data to sheet2
		SCTool.selectRange("A1:A7");
		app.dispatch(".uno:Copy");
		app.dispatch(".uno:SelectTables");
		scSheetsList.select(1); // Sheet 2
		scSelectSheetsDlg.ok();
		app.dispatch(".uno:Paste");

		// "Data->Sort...", "Options" tab, check "Range contains column labels",
		// choose custom sort order, "Ascending", sort first by Color
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageRangeContainsColumnLabels.check();
		sortOptionsPageCustomSortOrder.check();
		sortOptionsPageCustomSortOrderList.select("white,red,yellow,blue,green,black");
		sortPage.select();
		sortPageBy1.select(1); // "Color"
		sortPageAscending1.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result with custom sort order", expectedResultCustomSortOrder, SCTool.getCellTexts("A1:A7"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", data, SCTool.getCellTexts("A1:A7"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedResultCustomSortOrder, SCTool.getCellTexts("A1:A7"));

		// Save and close document
		String saveTo = getPath("temp/" + "SortOptionsCustomSortOrderPredefineFromNewList.ods");
		app.dispatch(".uno:SaveAs");
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		app.dispatch(".uno:CloseDoc");
		openStartcenter();

		// Reopen and verify sorted result
		app.dispatch(".uno:Open");
		submitOpenDlg(saveTo);
		calc.waitForExistence(10, 2);
		app.dispatch(".uno:SelectTables");
		scSheetsList.select(0); // Sheet 1
		scSelectSheetsDlg.ok();
		assertArrayEquals("Original data", data, SCTool.getCellTexts("$A1:$A7"));
		app.dispatch(".uno:SelectTables");
		scSheetsList.select(1); // Sheet 2
		scSelectSheetsDlg.ok();
		assertArrayEquals("Saved sorted result", expectedResultCustomSortOrder, SCTool.getCellTexts("$A1:$A7"));
	}

	/**
	 * Test sort options: sort columns, direction "Left to right"
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSortOptionsDirectionSortColumns() throws Exception {

		// Input some data
		String[][] data = new String[][] { { "Units", "7", "27", "4", "12", "3", "6" }, };
		String[][] expectedSortedResult = new String[][] { { "Units", "3", "4", "6", "7", "12", "27" }, };
		SCTool.selectRange("A1");
		typeKeys("Units<right>7<right>27<right>4<right>12<right>3<right>6");
		sleep(1); // If no sleep, some strings lost

		// "Data->Sort...", check "Range contains column labels",
		// "Left to right", sort first by"Units", "Ascending"
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageRangeContainsColumnLabels.check();
		sortOptionsPageLeftToRight.check();
		sortPage.select();
		sortPageBy1.select(1); // Choose "Units"
		sortPageAscending1.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result", expectedSortedResult, SCTool.getCellTexts("A1:G1"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", data, SCTool.getCellTexts("A1:G1"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedSortedResult, SCTool.getCellTexts("A1:G1"));

		// Save and close document
		String saveTo = getPath("temp/" + "SortOptionsDirectionSortColumns.ods");
		app.dispatch(".uno:SaveAs");
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		app.dispatch(".uno:CloseDoc");
		openStartcenter();

		// Reopen and verify sorted result
		app.dispatch(".uno:Open");
		submitOpenDlg(saveTo);
		calc.waitForExistence(10, 2);
		assertArrayEquals("Saved sorted result", expectedSortedResult, SCTool.getCellTexts("A1:G1"));
	}

	/**
	 * Test sort options: "Include formats"
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSortOptionsIncludeFormats() throws Exception {

		String[][] dataWithCurrencyFormats = new String[][] { { "Units" }, { "$32.00" }, { "57.00 \u20ac" }, { "\u20a4 74" }, { "R$ 50.00" }, { "\u062c.\u0645. 27" },
				{ "7.00 \u0440\u0443\u0431" }, };
		String[][] expectedSortedResultIncludeFormat = new String[][] { { "Units" }, { "7.00 \u0440\u0443\u0431" }, { "\u062c.\u0645. 27" }, { "$32.00" }, { "R$ 50.00" },
				{ "57.00 \u20ac" }, { "\u20a4 74" }, };
		String[][] expectedSortedResultExcludeFormat = new String[][] { { "Units" }, { "$7.00" }, { "27.00 \u20ac" }, { "\u20a4 32" }, { "R$ 50.00" }, { "\u062c.\u0645. 57" },
				{ "74.00 \u0440\u0443\u0431" }, };

		// Open sample file to get source data
		String file = prepareData("sc/SortOptionsIncludeFormats.ods");
		app.dispatch(".uno:Open", 3);
		submitOpenDlg(file);
		calc.waitForExistence(10, 2);

		assertArrayEquals("source", dataWithCurrencyFormats, SCTool.getCellTexts("A1:A7"));

		// "Data->Sort...", check "Range contains column labels", check
		// "Include formats", sort first by "Units", "Ascending"
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageRangeContainsColumnLabels.check();
		sortOptionsPageIncludeFormats.check();
		sortPage.select();
		sortPageBy1.select(1); // "Units"
		sortPageAscending1.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result include formats", expectedSortedResultIncludeFormat, SCTool.getCellTexts("A1:A7"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", dataWithCurrencyFormats, SCTool.getCellTexts("A1:A7"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedSortedResultIncludeFormat, SCTool.getCellTexts("A1:A7"));
		app.dispatch(".uno:Undo");

		// Copy the original data to sheet2
		SCTool.selectRange("A1:A7");
		app.dispatch(".uno:Copy");
		SCTool.selectRange("Sheet2.A1");
		app.dispatch(".uno:Paste");

		// "Data->Sort...", check "Range contains column labels", uncheck
		// "Include formats", sort first by "Units", "Ascending"
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageRangeContainsColumnLabels.check();
		sortOptionsPageIncludeFormats.uncheck();
		sortPage.select();
		sortPageBy1.select(1); // "Units"
		sortPageAscending1.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result exclude formats", expectedSortedResultExcludeFormat, SCTool.getCellTexts("A1:A7"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", dataWithCurrencyFormats, SCTool.getCellTexts("A1:A7"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedSortedResultExcludeFormat, SCTool.getCellTexts("A1:A7"));

		// Save and close document
		String saveTo = getPath("temp/" + "SortOptionsIncludeFormats.ods");
		app.dispatch(".uno:SaveAs");
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		app.dispatch(".uno:CloseDoc");
		openStartcenter();

		// Reopen and verify sorted result
		app.dispatch(".uno:Open");
		submitOpenDlg(saveTo);
		calc.waitForExistence(10, 2);
		assertArrayEquals("Original data", dataWithCurrencyFormats, SCTool.getCellTexts("$Sheet1.$A1:$A7"));
		assertArrayEquals("Saved sorted result exclude format", expectedSortedResultExcludeFormat, SCTool.getCellTexts("$Sheet2.$A1:$A7"));
	}

	/**
	 * Test sort options: multiple sort, data overlap
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSortOptionsMultipleSortDataOverlap() throws Exception {

		// Input some data
		String[][] data1 = new String[][] { { "D" }, { "C" }, { "B" }, { "A" }, { "E" }, };
		String[][] expectedSortedResult1 = new String[][] { { "A" }, { "B" }, { "C" }, { "D" }, { "E" }, };
		String[][] data2 = new String[][] { { "4" }, { "2" }, { "5" }, { "1" }, { "3" }, };
		String[][] expectedSortedResultDataOverlap = new String[][] { { "A" }, { "B" }, { "C" }, { "1" }, { "2" }, { "3" }, { "4" }, { "5" }, };
		SCTool.selectRange("A1");
		typeKeys("D<down>C<down>B<down>A<down>E");
		sleep(1); // If no sleep, some strings lost

		// "Data->Sort...", uncheck "Range contains column labels", sort first
		// by "Column A", "Ascending"
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageRangeContainsColumnLabels.uncheck();
		sortPage.select();
		sortPageBy1.select(1); // "Column A"
		sortPageAscending1.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result", expectedSortedResult1, SCTool.getCellTexts("A1:A5"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", data1, SCTool.getCellTexts("A1:A5"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedSortedResult1, SCTool.getCellTexts("A1:A5"));

		// Input data2 into same sheet, data1 and data2 are not overlapped
		SCTool.selectRange("G10");
		typeKeys("4<down>2<down>5<down>1<down>3");

		// Focus on data2, "Data->Sort...", "Copy result to" partially overlap
		// with data1, sort first by "Column G", "Ascending"
		SCTool.selectRange("G10");
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageRangeContainsColumnLabels.uncheck();
		sortOptionsPageCopyResultTo.check();
		sortOptionsPageCopyResultToEdit.setText("A4");
		sortPage.select();
		sortPageBy1.select(1); // "Column G"
		sortPageAscending1.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result data overlap", expectedSortedResultDataOverlap, SCTool.getCellTexts("A1:A8"));

		// Save and close document
		String saveTo = getPath("temp/" + "SortOptionsMultipleSortDataOverlap.ods");
		app.dispatch(".uno:SaveAs");
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		app.dispatch(".uno:CloseDoc");
		openStartcenter();

		// Reopen and verify sorted result
		app.dispatch(".uno:Open");
		submitOpenDlg(saveTo);
		calc.waitForExistence(10, 2);
		assertArrayEquals("Saved sorted result", expectedSortedResultDataOverlap, SCTool.getCellTexts("A1:A8"));
		assertArrayEquals("Original data2", data2, SCTool.getCellTexts("G10:G14"));
	}

	/**
	 * Test sort options: multiple sort, no data overlap, sort parameter saved
	 * correctly
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSortOptionsMultipleSortSortParameterSaved() throws Exception {

		// Input some data
		String[][] data1 = new String[][] { { "D" }, { "C" }, { "B" }, { "A" }, { "E" }, };
		String[][] expectedSortedResult1 = new String[][] { { "A" }, { "B" }, { "C" }, { "D" }, { "E" }, };
		String[][] data2 = new String[][] { { "Numbers" }, { "4" }, { "2" }, { "5" }, { "1" }, { "3" }, };
		String[][] expectedSortedResult2 = new String[][] { { "Numbers" }, { "1" }, { "2" }, { "3" }, { "4" }, { "5" }, };
		SCTool.selectRange("A1");
		typeKeys("D<down>C<down>B<down>A<down>E");
		sleep(1); // If no sleep, some strings lost

		// "Data->Sort...", uncheck "Range contains column labels", check
		// "Case sensitive" and "Include formats", sort first by "Column A",
		// "Ascending"
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageRangeContainsColumnLabels.uncheck();
		sortOptionsPageCaseSensitive.check();
		sortOptionsPageIncludeFormats.check();
		sortPage.select();
		sortPageBy1.select(1); // "Column A"
		sortPageAscending1.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result1", expectedSortedResult1, SCTool.getCellTexts("A1:A5"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", data1, SCTool.getCellTexts("A1:A5"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedSortedResult1, SCTool.getCellTexts("A1:A5"));

		// Input data2 into same sheet, data1 and data2 are not overlapped
		SCTool.selectRange("G10");
		typeKeys("Numbers<down>4<down>2<down>5<down>1<down>3");

		// Focus on data2, "Data->Sort...", check
		// "Range contains column labels", uncheck "Case sensitive" and
		// "Include formats", sort first by "Numbers", "Ascending"
		SCTool.selectRange("G10");
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		sortOptionsPageRangeContainsColumnLabels.check();
		sortOptionsPageCaseSensitive.uncheck();
		sortOptionsPageIncludeFormats.uncheck();
		sortPage.select();
		sortPageBy1.select(1); // "Numbers"
		sortPageAscending1.check();
		sortPage.ok();

		// Verify sorted result
		assertArrayEquals("Sorted result2", expectedSortedResult2, SCTool.getCellTexts("G10:G15"));

		// Uodo/redo
		app.dispatch(".uno:Undo");
		assertArrayEquals("Undo sorted result", data2, SCTool.getCellTexts("G10:G15"));
		app.dispatch(".uno:Redo");
		assertArrayEquals("Redo sorted result", expectedSortedResult2, SCTool.getCellTexts("G10:G15"));

		// Open sort dialog, check its setting
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		assertTrue("Range contains column labels should be checked", sortOptionsPageRangeContainsColumnLabels.isChecked());
		assertFalse("Case sensitive should not be checked", sortOptionsPageCaseSensitive.isChecked());
		assertFalse("Include formats should not be checked", sortOptionsPageIncludeFormats.isChecked());
		sortOptionsPage.ok();

		// Save and close document
		String saveTo = getPath("temp/" + "SortOptionsMultipleSortParameterSaved.ods");
		app.dispatch(".uno:SaveAs");
		FileUtil.deleteFile(saveTo);
		submitSaveDlg(saveTo);
		app.dispatch(".uno:CloseDoc");
		openStartcenter();

		// Reopen and verify sorted result and sort parameters
		app.dispatch(".uno:Open");
		submitOpenDlg(saveTo);
		calc.waitForExistence(10, 2);
		assertArrayEquals("Saved sorted result1", expectedSortedResult1, SCTool.getCellTexts("A1:A5"));
		assertArrayEquals("Saved sorted result2", expectedSortedResult2, SCTool.getCellTexts("G10:G15"));
		app.dispatch(".uno:DataSort");
		sortOptionsPage.select();
		assertTrue("Range contains column labels should be checked", sortOptionsPageRangeContainsColumnLabels.isChecked());
		assertFalse("Case sensitive should not be checked", sortOptionsPageCaseSensitive.isChecked());
		assertFalse("Include formats should not be checked", sortOptionsPageIncludeFormats.isChecked());
		sortOptionsPage.ok();
	}

}
