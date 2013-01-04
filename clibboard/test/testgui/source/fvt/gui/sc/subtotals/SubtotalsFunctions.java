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


package fvt.gui.sc.subtotals;

import static org.junit.Assert.*;
import static org.openoffice.test.common.Testspace.*;
import static org.openoffice.test.vcl.Tester.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.Logger;

import testlib.gui.SCTool;


public class SubtotalsFunctions {

	@Rule
	public Logger log = Logger.getLogger(this);
	
	@Before
	public void setUp() throws Exception {
		app.start(true);
		String file = prepareData("sc/SubtotalsSampleFile.ods");
		open(file);
		SCTool.selectRange("A1:E7");
		app.dispatch(".uno:DataSubTotals");
	}

	@After
	public void tearDown() throws Exception {
		app.stop();

	}

	/**
	 * Just support en-US language Verify Average function in SubTotals
	 */
	@Test
	public void testAverage() {
		scSubTotalsGroupByListBox.select(4); // "Team"
		// SCCalcSubTotalForColumns.click(10, 25); // In different platform, can
		// not focus on same checkbox
		scCalcSubTotalForColumns.select(1);
		scCalcSubTotalForColumns.check(1); // "Code"
		sleep(1);
		scCalcSubTotolsFuncionList.select(2); // "Average"
		scSubTotalsGroup1Dialog.ok();
		sleep(1);

		assertArrayEquals("Wrong Average function in Subtotal ", new String[][] { { "Level", "Code", "No.", "Team", "Name" }, { "BS", "20", "2", "A", "Chcomic" }, { "CS", "30", "5", "A", "Ally" },
				{ "MS", "10", "1", "A", "Joker" }, { "", "20", "", "A Average", "" }, { "BS", "20", "4", "B", "Elle" }, { "MS", "10", "3", "B", "Kevin" },
				{ "", "15", "", "B Average", "" }, { "BS", "20", "6", "C", "Sweet" }, { "", "20", "", "C Average", "" }, { "", "18.33333333", "", "Grand Total", "" } },
				SCTool.getCellTexts("A1:E11"));
	}

	/**
	 * Just support en-US language Verify Count Numbers only function in
	 * SubTotals
	 */
	@Test
	public void testCountNumbersOnly() {
		// SCCalcSubTotalForColumns.click(10, 45);
		scCalcSubTotalForColumns.select(2);
		scCalcSubTotalForColumns.check(2); // "No."
		scCalcSubTotolsFuncionList.select(6); // "Count (numbers only)"
		scSubTotalsGroup1Dialog.ok();
		sleep(1);

		assertArrayEquals("Wrong Count Numbers only function in Subtotal", new String[][] { { "Level", "Code", "No.", "Team", "Name" }, { "BS", "20", "4", "B", "Elle" }, { "BS", "20", "6", "C", "Sweet" },
				{ "BS", "20", "2", "A", "Chcomic" }, { "BS Count", "", "3", "", "" }, { "CS", "30", "5", "A", "Ally" }, { "CS Count", "", "1", "", "" },
				{ "MS", "10", "1", "A", "Joker" }, { "MS", "10", "3", "B", "Kevin" }, { "MS Count", "", "2", "", "" }, { "Grand Total", "", "6", "", "" } },
				SCTool.getCellTexts("A1:E11"));
	}

	/**
	 * Just support en-US language Verify Max Numbers function in SubTotals
	 */
	@Test
	public void testMax() {
		// SCCalcSubTotalForColumns.click(10, 45);
		scCalcSubTotalForColumns.select(2);
		scCalcSubTotalForColumns.check(2); // "No."
		scCalcSubTotolsFuncionList.select(3); // "Max"
		scSubTotalsGroup1Dialog.ok();
		sleep(1);

		assertArrayEquals("Wrong Max Numbers function in Subtotal", new String[][] { { "Level", "Code", "No.", "Team", "Name" }, { "BS", "20", "4", "B", "Elle" }, { "BS", "20", "6", "C", "Sweet" },
				{ "BS", "20", "2", "A", "Chcomic" }, { "BS Max", "", "6", "", "" }, { "CS", "30", "5", "A", "Ally" }, { "CS Max", "", "5", "", "" },
				{ "MS", "10", "1", "A", "Joker" }, { "MS", "10", "3", "B", "Kevin" }, { "MS Max", "", "3", "", "" }, { "Grand Total", "", "6", "", "" } },
				SCTool.getCellTexts("A1:E11"));
	}

	/**
	 * Just support en-US language Verify Min Numbers function in SubTotals
	 */
	@Test
	public void testMin() {
		// SCCalcSubTotalForColumns.click(10, 45);
		scCalcSubTotalForColumns.select(2);
		scCalcSubTotalForColumns.check(2); // "No."
		scCalcSubTotolsFuncionList.select(4); // "Min"
		scSubTotalsGroup1Dialog.ok();
		sleep(1);

		assertArrayEquals("Wrong Min Numbers function in Subtotal", new String[][] { { "Level", "Code", "No.", "Team", "Name" }, { "BS", "20", "4", "B", "Elle" }, { "BS", "20", "6", "C", "Sweet" },
				{ "BS", "20", "2", "A", "Chcomic" }, { "BS Min", "", "2", "", "" }, { "CS", "30", "5", "A", "Ally" }, { "CS Min", "", "5", "", "" },
				{ "MS", "10", "1", "A", "Joker" }, { "MS", "10", "3", "B", "Kevin" }, { "MS Min", "", "1", "", "" }, { "Grand Total", "", "1", "", "" } },
				SCTool.getCellTexts("A1:E11"));
	}

	/**
	 * Just support en-US language Verify Product function in SubTotals
	 */
	@Test
	public void testProduct() {
		// SCCalcSubTotalForColumns.click(10, 45);
		scCalcSubTotalForColumns.select(2);
		scCalcSubTotalForColumns.check(2); // "No."
		scCalcSubTotolsFuncionList.select(5); // "Product"
		scSubTotalsGroup1Dialog.ok();
		sleep(1);

		assertArrayEquals("Wrong Product function in Subtotal", new String[][] { { "Level", "Code", "No.", "Team", "Name" }, { "BS", "20", "4", "B", "Elle" }, { "BS", "20", "6", "C", "Sweet" },
				{ "BS", "20", "2", "A", "Chcomic" }, { "BS Product", "", "48", "", "" }, { "CS", "30", "5", "A", "Ally" }, { "CS Product", "", "5", "", "" },
				{ "MS", "10", "1", "A", "Joker" }, { "MS", "10", "3", "B", "Kevin" }, { "MS Product", "", "3", "", "" }, { "Grand Total", "", "720", "", "" } },
				SCTool.getCellTexts("A1:E11"));
	}

	/**
	 * Just support en-US language Verify StDevP (Population) function in
	 * SubTotals
	 */
	@Test
	public void testStDevPPopulation() {
		// SCCalcSubTotalForColumns.click(10, 45);
		scCalcSubTotalForColumns.select(2);
		scCalcSubTotalForColumns.check(2); // "No."
		scCalcSubTotolsFuncionList.select(8); // "StDevP (Population)"
		scSubTotalsGroup1Dialog.ok();
		sleep(1);

		assertArrayEquals("Wrong StdevP function in Subtotal", new String[][] { { "Level", "Code", "No.", "Team", "Name" }, { "BS", "20", "4", "B", "Elle" }, { "BS", "20", "6", "C", "Sweet" },
				{ "BS", "20", "2", "A", "Chcomic" }, { "BS StDev", "", "1.63299316", "", "" }, { "CS", "30", "5", "A", "Ally" }, { "CS StDev", "", "0", "", "" },
				{ "MS", "10", "1", "A", "Joker" }, { "MS", "10", "3", "B", "Kevin" }, { "MS StDev", "", "1", "", "" }, { "Grand Total", "", "1.70782513", "", "" } },
				SCTool.getCellTexts("A1:E11"));
	}

	/**
	 * Just support en-US language Verify sum function in SubTotals
	 */
	@Test
	public void testSum() {
		// SCCalcSubTotalForColumns.click(10, 45);
		scCalcSubTotalForColumns.select(2);
		scCalcSubTotalForColumns.check(2); // "No."
		scCalcSubTotolsFuncionList.select(0); // "Sum"
		scSubTotalsGroup1Dialog.ok();
		sleep(1);

		assertArrayEquals("Wrong Sum Function in Subtotal", new String[][] { { "Level", "Code", "No.", "Team", "Name" }, { "BS", "20", "4", "B", "Elle" }, { "BS", "20", "6", "C", "Sweet" },
				{ "BS", "20", "2", "A", "Chcomic" }, { "BS Sum", "", "12", "", "" }, { "CS", "30", "5", "A", "Ally" }, { "CS Sum", "", "5", "", "" },
				{ "MS", "10", "1", "A", "Joker" }, { "MS", "10", "3", "B", "Kevin" }, { "MS Sum", "", "4", "", "" }, { "Grand Total", "", "21", "", "" } },
				SCTool.getCellTexts("A1:E11"));
	}

	/**
	 * Just support en-US language Verify Var Sample function in SubTotals
	 */
	@Test
	public void testVarSample() {
		// SCCalcSubTotalForColumns.click(10, 45);
		scCalcSubTotalForColumns.select(2);
		scCalcSubTotalForColumns.check(2); // "No."
		scCalcSubTotolsFuncionList.select(9); // "Var (Sample)"
		scSubTotalsGroup1Dialog.ok();
		sleep(1);

		assertArrayEquals("Wrong Var Sample function in Subtotal", new String[][] { { "Level", "Code", "No.", "Team", "Name" }, { "BS", "20", "4", "B", "Elle" }, { "BS", "20", "6", "C", "Sweet" },
				{ "BS", "20", "2", "A", "Chcomic" }, { "BS Var", "", "4", "", "" }, { "CS", "30", "5", "A", "Ally" }, { "CS Var", "", "#DIV/0!", "", "" },
				{ "MS", "10", "1", "A", "Joker" }, { "MS", "10", "3", "B", "Kevin" }, { "MS Var", "", "2", "", "" }, { "Grand Total", "", "3.5", "", "" } },
				SCTool.getCellTexts("A1:E11"));
	}

	/**
	 * Just support en-US language Verify Don't sort in Sub totals
	 */
	@Test
	public void testDoNotSortOption() {
		scSubTotalsGroupByListBox.select(4); // "Team"
		// SCCalcSubTotalForColumns.click(10, 25);
		scCalcSubTotalForColumns.select(1);
		scCalcSubTotalForColumns.check(1); // "Code"
		scSubTotalsOptionsTabPage.select();
		scSubtotalsPreSortToGroupCheckBox.uncheck();
		scSubTotalsOptionsTabPage.ok();
		sleep(1);

		assertArrayEquals("Wrong Not Sort option in Subtotal", new String[][] { { "Level", "Code", "No.", "Team", "Name" }, { "BS", "20", "4", "B", "Elle" }, { "", "20", "", "B Sum", "" },
				{ "BS", "20", "6", "C", "Sweet" }, { "", "20", "", "C Sum", "" }, { "BS", "20", "2", "A", "Chcomic" }, { "CS", "30", "5", "A", "Ally" },
				{ "MS", "10", "1", "A", "Joker" }, { "", "60", "", "A Sum", "" }, { "MS", "10", "3", "B", "Kevin" }, { "", "10", "", "B Sum", "" },
				{ "", "110", "", "Grand Total", "" } }, SCTool.getCellTexts("A1:E12"));
	}

	/**
	 * Just support en-US language Verify SubTotals Options default UI
	 */
	@Test
	public void testDefaultUI() {
		assertArrayEquals("Group List: ", new String[] { "- none -", "Level", "Code", "No.", "Team", "Name" }, scSubTotalsGroupByListBox.getItemsText());

		scSubTotalsOptionsTabPage.select();
		assertFalse("Wrong default value of InsertPageBreak checkbox",scSubtotalsInsertPageBreakCheckBox.isChecked());
		assertFalse("Wrong default value of CaseSensitive checkbox",scSubtotalsCaseSensitiveCheckBox.isChecked());
		assertTrue("Worng default value of PreSortToGroup checkbox",scSubtotalsPreSortToGroupCheckBox.isChecked());
		assertTrue("Wrong default value of SortAscending Radio button",scSubtotalSortAscendingRadioButton.isChecked());
		assertFalse("Wrong default value of SortDescending Radio button",scSubtotalSortDescendingRadioButton.isChecked());
		assertFalse("Wrong default value of InludeFormat checkbox",scSubtotalsIncludeFormatsCheckBox.isChecked());
		assertFalse("Wrong default value of CustomSortOrder checkbox",scSubtotalsCustomSortOrderCheckBox.isChecked());
		assertFalse("Wrong default value of CustomSort listbox",scSubtotalsCustomSortListBox.isEnabled());
		scSubTotalsOptionsTabPage.ok();
	}

	/**
	 * Just support en-US language Verify Subtotals using all group
	 */
	@Test
	public void testUsingAllGroup() {
		scSubTotalsGroup1Dialog.select();
		scSubTotalsGroupByListBox.select(1); // "Level"
		// SCCalcSubTotalForColumns.click(10, 45);
		scCalcSubTotalForColumns.select(2);
		scCalcSubTotalForColumns.check(2); // "No."
		scSubTotalsGroup2Dialog.select();
		scSubTotalsGroupByListBox.select(4); // "Team"
		// SCCalcSubTotalForColumns.click(10, 25);
		scCalcSubTotalForColumns.select(1);
		scCalcSubTotalForColumns.check(1); // "Code"
		scCalcSubTotolsFuncionList.select(3); // "Max"
		scSubTotalsGroup3Dialog.select();
		scSubTotalsGroupByListBox.select(5); // "Name"
		// SCCalcSubTotalForColumns.click(10, 25);
		scCalcSubTotalForColumns.select(1);
		scCalcSubTotalForColumns.check(1); // "Code"
		scSubTotalsGroup1Dialog.select();
		scSubTotalsGroup1Dialog.ok();
		sleep(1);

		assertArrayEquals("Not all group in Subtotal", new String[][] { { "Level", "Code", "No.", "Team", "Name" }, { "BS", "20", "2", "A", "Chcomic" }, { "", "20", "", "", "Chcomic Sum" },
				{ "", "20", "", "A Max", "" }, { "BS", "20", "4", "B", "Elle" }, { "", "20", "", "", "Elle Sum" }, { "", "20", "", "B Max", "" },
				{ "BS", "20", "6", "C", "Sweet" }, { "", "20", "", "", "Sweet Sum" }, { "", "20", "", "C Max", "" }, { "BS Sum", "", "12", "", "" },
				{ "CS", "30", "5", "A", "Ally" }, { "", "30", "", "", "Ally Sum" }, { "", "30", "", "A Max", "" }, { "CS Sum", "", "5", "", "" },
				{ "MS", "10", "1", "A", "Joker" }, { "", "10", "", "", "Joker Sum" }, { "", "10", "", "A Max", "" }, { "MS", "10", "3", "B", "Kevin" },
				{ "", "10", "", "", "Kevin Sum" }, { "", "10", "", "B Max", "" }, { "MS Sum", "", "4", "", "" }, { "Grand Total", "", "21", "", "" } },
				SCTool.getCellTexts("A1:E23"));
	}

	/**
	 * Just support en-US language Verify Subtotals_Recalculate and refresh
	 * results when data rows deleted.
	 */
	@Test
	public void testRecalculateAfterRowDeleted() {
		// SCCalcSubTotalForColumns.click(10,45);
		scCalcSubTotalForColumns.select(2);
		scCalcSubTotalForColumns.check(2); // "No."
		scSubTotalsGroup1Dialog.ok();
		sleep(1);

		assertArrayEquals("Subtotal table not equal before deleting row", new String[][] { { "Level", "Code", "No.", "Team", "Name" }, { "BS", "20", "4", "B", "Elle" },
				{ "BS", "20", "6", "C", "Sweet" }, { "BS", "20", "2", "A", "Chcomic" }, { "BS Sum", "", "12", "", "" }, { "CS", "30", "5", "A", "Ally" },
				{ "CS Sum", "", "5", "", "" }, { "MS", "10", "1", "A", "Joker" }, { "MS", "10", "3", "B", "Kevin" }, { "MS Sum", "", "4", "", "" },
				{ "Grand Total", "", "21", "", "" } }, SCTool.getCellTexts("A1:E11"));

		SCTool.selectRange("A3:E3");
		app.dispatch(".uno:DeleteCell");
		scDeleteCellsDeleteRowsRadioButton.check();
		scDeleteCellsDialog.ok();
		sleep(1);

		assertArrayEquals("Subtotal table not equal after deleting row", new String[][] { { "Level", "Code", "No.", "Team", "Name" }, { "BS", "20", "4", "B", "Elle" },
				{ "BS", "20", "2", "A", "Chcomic" }, { "BS Sum", "", "6", "", "" }, { "CS", "30", "5", "A", "Ally" }, { "CS Sum", "", "5", "", "" },
				{ "MS", "10", "1", "A", "Joker" }, { "MS", "10", "3", "B", "Kevin" }, { "MS Sum", "", "4", "", "" }, { "Grand Total", "", "15", "", "" }, { "", "", "", "", "" } },
				SCTool.getCellTexts("A1:E11"));

	}

	/**
	 * Verify Recalculates when source data changed with Average function in
	 * SubTotals
	 */
	@Test
	public void testRecalculateWhenDataChanged() {
		scSubTotalsGroupByListBox.select(4); // "Team"
		// SCCalcSubTotalForColumns.click(10,25);
		scCalcSubTotalForColumns.select(1);
		scCalcSubTotalForColumns.check(1); // "Code"
		scCalcSubTotolsFuncionList.select(2); // "Average"
		scSubTotalsGroup1Dialog.ok();
		sleep(1);

		SCTool.selectRange("B4");
		typeKeys("40" + "<enter>");
		sleep(1);
		assertEquals("B5's cell text is not 30","30", SCTool.getCellText("B5"));
		assertEquals("B11's cell text is not 23.33333333","23.33333333", SCTool.getCellText("B11"));

		SCTool.selectRange("B7");
		typeKeys("50" + "<enter>");
		sleep(1);
		assertEquals("B8's cell text is not 35","35", SCTool.getCellText("B8"));
		assertEquals("B11's cell text is not 30","30", SCTool.getCellText("B11"));

		SCTool.selectRange("B9");
		typeKeys("30" + "<enter>");
		sleep(1);
		assertEquals("B10's cell text is not 30","30", SCTool.getCellText("B10"));
		assertEquals("B11's cell text is not 31.66666667","31.66666667", SCTool.getCellText("B11"));
	}
}
