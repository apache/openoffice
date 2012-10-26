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
package fvt.gui.sc.datapilot;

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
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.SystemUtil;

import testlib.gui.SCTool;

/**
 * Test Data->Pivot Table Test cases in this class are unavailable. Because
 * these test cases are based on symphony. DataPilot of symphony is totally
 * different from PivotTable of AOO. Keep this class to track the test point.
 * TODO: Add test cases about Pivot Table.
 */
public class DataPivotTable {

	@Rule
	public Logger log = Logger.getLogger(this);

	@Before
	public void setUp() throws Exception {
		app.start(true);
	}

	@After
	public void tearDown() throws Exception {

	}

	/**
	 * 
	 * Verify copy and paste Data Pilot Table
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testCopyPasteDataPilotTable() {
		// Open the sample file
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);

		// Create DataPilotTable and verify the content
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));

		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();
		}

		// Add field into every area
		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Page").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A4"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		// Copy and paste the data pilot table and verify the content
		SCTool.selectRange("A1:J24");
		typeKeys("<$copy>");

		SCTool.selectRange("A26");
		typeKeys("<$paste>");
		assertEquals("Locale", SCTool.getCellText("A26"));
		assertEquals("Sum - Order Number", SCTool.getCellText("A28"));
		assertEquals("Name", SCTool.getCellText("B28"));
		assertEquals("Date", SCTool.getCellText("A29"));
		assertEquals("Total Result", SCTool.getCellText("A48"));
		assertEquals("Total Result", SCTool.getCellText("J29"));
		assertEquals("266773", SCTool.getCellText("J48"));
		sleep(1);

		SCTool.selectRange("A1:J24");
		typeKeys("<$copy>");
		SCTool.selectRange("$A.$A29");
		typeKeys("<$paste>");
		assertEquals("Locale", SCTool.getCellText("A29"));
		assertEquals("Sum - Order Number", SCTool.getCellText("A31"));
		assertEquals("Name", SCTool.getCellText("B31"));
		assertEquals("Date", SCTool.getCellText("A32"));
		assertEquals("Total Result", SCTool.getCellText("A51"));
		assertEquals("Total Result", SCTool.getCellText("J32"));
		assertEquals("266773", SCTool.getCellText("J51"));
	}

	/**
	 * 
	 * Verify that DP panel will be synchronized with table while add fields on
	 * panel.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testDataPilotAddFieldsAutoUpdate() {
		// open the sample file Create DataPilotTable and verify the content
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));

		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();

		}
		sleep(1);

		// Add field into every area
		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A2"));
		sleep(1);

		dataPilotFieldSelect.drag(1, 1, 184, 80);
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		typeKeys("<$undo>");
		assertEquals("", SCTool.getCellText("A1"));
		sleep(1);

		typeKeys("<$redo>");
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		typeKeys("<$undo>");
		assertEquals("", SCTool.getCellText("A3"));
		sleep(1);

		typeKeys("<$redo>");
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);
	}

	/**
	 * 
	 * Verify that DP panel will be synchronized with table while add fields on
	 * panel.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testDataPilotAddFieldsManualUpdate() {
		// open the sample file Create DataPilotTable and verify the content
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));

		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();

		}

		dataPilotAutomaticallyUpdateCheckBox.uncheck();

		sleep(1);
		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));
		sleep(1);

		SCTool.selectRange("B20");
		activeMsgBox.yes();
		sleep(1);
		assertEquals("Name", SCTool.getCellText("B1"));
		assertEquals("Date", SCTool.getCellText("A2"));

		dataPilotFieldSelect.drag(1, 1, 184, 80);
		assertEquals("Name", SCTool.getCellText("B1"));
		assertEquals("Date", SCTool.getCellText("A2"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Name", SCTool.getCellText("B1"));
		assertEquals("Date", SCTool.getCellText("A2"));
		sleep(1);

		dataPilotTableUpdateButton.click();
		assertEquals("Locale", SCTool.getCellText("A1"));
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		typeKeys("<$undo>");
		assertEquals("Name", SCTool.getCellText("B1"));
		assertEquals("Date", SCTool.getCellText("A2"));
		sleep(1);

		typeKeys("<$redo>");
		assertEquals("Locale", SCTool.getCellText("A1"));
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		dataPilotAutomaticallyUpdateCheckBox.check();
		assertTrue(dataPilotAutomaticallyUpdateCheckBox.isChecked());
	}

	/**
	 * 
	 * Verify that DP panel will be synchronized with table while add fields on
	 * panel.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testDataPilotChangeDataFieldOptionAutoUpdate() {
		// open the sample file Create DataPilotTable and verify the content
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));
		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();

		}
		sleep(1);

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("B1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Order Number", SCTool.getCellText("B3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 90);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Amount", SCTool.getCellText("A1"));
		assertEquals("32779.17", SCTool.getCellInput("K30"));
		sleep(1);

		// Change Locale option settings, and verify it
		String target = "New Locale";
		dataPilotColumnArea.click(1, 1);
		dataPilotColumnArea.openContextMenu();
		menuItem("Field Option").select();
		dataPilotFieldOptionFieldTabPageDialog.select();
		dataPilotFieldDisplayNameEditBox.setText(target);
		sleep(1);

		dataPilotFieldOptionFiledSubtotalsPage.select();
		sleep(1);
		dataPilotFieldOptionSubTotalsManuallyRadioButton.check();
		sleep(1);
		scDataPilotOptionSubtotalFunctionList.click(9, 24);
		sleep(1);

		dataPilotFieldOptionSortTabPageDialog.select();

		dataPilotFieldOptionSortDescendingRadioButton.check();
		dataPilotFieldOptionFieldTabPageDialog.select();
		dataPilotFieldOptionFieldTabPageDialog.ok();
		sleep(1);

		assertEquals("New Locale", SCTool.getCellText("C1"));
		assertEquals("USA", SCTool.getCellText("C2"));
		assertEquals("USA Count - Amount", SCTool.getCellInput("H2"));
		assertEquals("17", SCTool.getCellInput("H30"));
		assertEquals("China Count - Amount", SCTool.getCellInput("L2"));
		assertEquals("9", SCTool.getCellInput("L30"));
		assertEquals("32779.17", SCTool.getCellInput("M30"));

		dataPilotColumnArea.click(1, 30);
		dataPilotColumnArea.openContextMenu();
		menuItem("Field Option").select();
		dataPilotFieldOptionFieldTabPageDialog.select();
		dataPilotFieldShowItemWithoutDataCheckBox.check();
		dataPilotFieldOptionFieldTabPageDialog.ok();
		assertEquals("Emmy Ma", SCTool.getCellText("F3"));
		assertEquals("32779.17", SCTool.getCellInput("U30"));

		SCTool.selectRange("C1");
		spreadSheetCells.openContextMenu();
		menuItem("Field Option...").select();
		dataPilotFieldOptionFieldTabPageDialog.select();
		dataPilotFieldDisplayNameEditBox.setText("Locale");
		dataPilotFieldOptionFiledSubtotalsPage.select();
		dataPilotFieldOptionSubTotalsNeverRadioButton.check();
		sleep(1);
		dataPilotFieldOptionSortTabPageDialog.select();
		dataPilotFieldOptionSortAscendingRadioButton.check();
		dataPilotFieldOptionFieldTabPageDialog.select();
		dataPilotFieldOptionFieldTabPageDialog.ok();

		assertEquals("Locale", SCTool.getCellText("C1"));
		assertEquals("China", SCTool.getCellText("C2"));
		assertEquals("USA", SCTool.getCellText("K2"));
		assertEquals("32779.17", SCTool.getCellInput("S30"));

		SCTool.selectRange("D1");
		spreadSheetCells.openContextMenu();
		menuItem("Field Option...").select();
		dataPilotFieldOptionFieldTabPageDialog.select();
		dataPilotFieldShowItemWithoutDataCheckBox.uncheck();
		dataPilotFieldOptionFieldTabPageDialog.ok();
		assertEquals("32779.17", SCTool.getCellInput("K30"));
	}

	/**
	 * 
	 * Verify that Verify the Drag/Move function of Grouped field
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testDragMoveFunctionOfGroupedField() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));
		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();

		}

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Page").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A4"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		SCTool.selectRange("B4:C4");
		spreadSheetCells.openContextMenu();
		menuItem("Group->Group Selected Items").select();
		assertEquals("Name2", SCTool.getCellText("B3"));
		assertEquals("Name", SCTool.getCellText("C3"));
		assertEquals("Group1", SCTool.getCellText("D4"));
		assertEquals("266773", SCTool.getCellText("J24"));
		sleep(1);

		dataPilotColumnArea.click(1, 1);
		dataPilotColumnArea.openContextMenu();
		menuItem("Move to Row").select();
		assertEquals("Name2", SCTool.getCellText("B4"));
		assertEquals("Group1", SCTool.getCellText("B6"));
		assertEquals("266773", SCTool.getCellText("K31"));
		sleep(1);

		typeKeys("<$undo>");
		assertEquals("Name2", SCTool.getCellText("B3"));
		assertEquals("Name", SCTool.getCellText("C3"));
		assertEquals("Group1", SCTool.getCellText("D4"));
		assertEquals("266773", SCTool.getCellText("J24"));
		sleep(1);

		typeKeys("<$redo>");
		assertEquals("Name2", SCTool.getCellText("B4"));
		assertEquals("Group1", SCTool.getCellText("B6"));
		assertEquals("266773", SCTool.getCellText("K31"));
	}

	/**
	 * 
	 * Verify that DP panel while switch focus among tables with same/different
	 * source range.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testHideShowPanel() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));

		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();
		}

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Page").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A4"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		SCTool.selectRange("A1:J24");
		typeKeys("<$copy>");

		SCTool.selectRange("A26");
		typeKeys("<$paste>");
		assertEquals("Locale", SCTool.getCellText("A26"));
		assertEquals("Sum - Order Number", SCTool.getCellText("A28"));
		assertEquals("Name", SCTool.getCellText("B28"));
		assertEquals("Date", SCTool.getCellText("A29"));
		assertEquals("Total Result", SCTool.getCellText("A48"));
		assertEquals("Total Result", SCTool.getCellText("J29"));
		assertEquals("266773", SCTool.getCellText("J48"));
		sleep(1);

		SCTool.selectRange("A1");
		spreadSheetCells.openContextMenu();
		menuItem("Show DataPilot Panel").select();
		assertFalse(dataPilotPanel.exists());
		sleep(1);

		SCTool.selectRange("A1");
		spreadSheetCells.openContextMenu();
		menuItem("Show DataPilot Panel").select();
		assertTrue(dataPilotPanel.exists());
		sleep(1);

		dataPilotPanel.close();
		assertFalse(dataPilotPanel.exists());
		sleep(1);

		SCTool.selectRange("A26");
		dataPilotButton.click();
		sleep(1);
		assertTrue(dataPilotPanel.exists());
		assertEquals("Locale", SCTool.getCellText("A26"));
		assertEquals("Sum - Order Number", SCTool.getCellText("A28"));
		assertEquals("Name", SCTool.getCellText("B28"));
		assertEquals("Date", SCTool.getCellText("A29"));
		assertEquals("Total Result", SCTool.getCellText("A48"));
		assertEquals("Total Result", SCTool.getCellText("J29"));
		assertEquals("266773", SCTool.getCellText("J48"));
	}

	/**
	 * 
	 * Verify that DP panel will be synchronized with table while add fields on
	 * panel.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testMoveFieldInSameAreaManualUpdate() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();

		}
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));
		sleep(1);

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("B1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Order Number", SCTool.getCellText("B3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 90);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Amount", SCTool.getCellText("A1"));
		assertEquals("32779.17", SCTool.getCellInput("K30"));
		sleep(1);

		dataPilotAutomaticallyUpdateCheckBox.uncheck();
		dataPilotColumnArea.drag(1, 1, 1, 60);
		assertEquals("Name", SCTool.getCellText("D1"));
		assertEquals("Locale", SCTool.getCellText("C1"));
		sleep(1);

		dataPilotPaneRowArea.drag(1, 1, 1, 60);
		assertEquals("Order Number", SCTool.getCellText("B3"));
		assertEquals("Date", SCTool.getCellText("A3"));
		sleep(1);

		SCTool.selectRange("A33");
		activeMsgBox.yes();
		assertEquals("Name", SCTool.getCellText("C1"));
		assertEquals("Locale", SCTool.getCellText("D1"));
		assertEquals("Order Number", SCTool.getCellText("A3"));
		assertEquals("Date", SCTool.getCellText("B3"));
		sleep(1);

		dataPilotColumnArea.click(1, 1);
		dataPilotColumnArea.openContextMenu();
		menuItem("Move Down").select();
		assertEquals("Name", SCTool.getCellText("C1"));
		assertEquals("Locale", SCTool.getCellText("D1"));
		sleep(1);

		dataPilotPaneRowArea.click(1, 30);
		dataPilotPaneRowArea.openContextMenu();
		menuItem("Move Up").select();
		assertEquals("Order Number", SCTool.getCellText("A3"));
		assertEquals("Date", SCTool.getCellText("B3"));
		sleep(1);

		SCTool.selectRange("A33");
		activeMsgBox.yes();
		assertEquals("Name", SCTool.getCellText("D1"));
		assertEquals("Locale", SCTool.getCellText("C1"));
		assertEquals("Order Number", SCTool.getCellText("B3"));
		assertEquals("Date", SCTool.getCellText("A3"));
		assertEquals("Sum - Amount", SCTool.getCellText("A1"));
		assertEquals("32779.17", SCTool.getCellInput("K30"));
		dataPilotAutomaticallyUpdateCheckBox.check();
		sleep(1);
	}

	/**
	 * 
	 * Verify that DP panel will be synchronized with table while move fields on
	 * panel.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testMoveFieldToOtherAreaAutoUpdate() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();

		}
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));
		sleep(1);

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("B1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Order Number", SCTool.getCellText("B3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 90);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Amount", SCTool.getCellText("A1"));
		assertEquals("32779.17", SCTool.getCellInput("K30"));
		sleep(1);

		dataPilotPaneRowArea.drag(1, 1, 1, -50);
		assertEquals("Date", SCTool.getCellText("D1"));
		assertEquals("32779.17", SCTool.getCellInput("AB31"));
		sleep(1);

		dataPilotColumnArea.click(1, 50);
		dataPilotColumnArea.openContextMenu();
		menuItem("Move to Row").select();
		assertEquals("Date", SCTool.getCellText("B3"));
		assertEquals("32779.17", SCTool.getCellInput("K30"));
		sleep(1);

		typeKeys("<$undo>");
		assertEquals("Date", SCTool.getCellText("D1"));
		assertEquals("32779.17", SCTool.getCellInput("AB31"));
		sleep(1);

		typeKeys("<$redo>");
		assertEquals("Date", SCTool.getCellText("B3"));
		assertEquals("32779.17", SCTool.getCellInput("K30"));
	}

	/**
	 * 
	 * Verify that DP panel will be synchronized with table while move fields on
	 * panel.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testMoveFieldToOtherAreaDiscardChange() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();

		}
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));
		sleep(1);

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("B1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Order Number", SCTool.getCellText("B3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 90);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Amount", SCTool.getCellText("A1"));
		assertEquals("32779.17", SCTool.getCellInput("K30"));
		sleep(1);

		dataPilotAutomaticallyUpdateCheckBox.uncheck();

		dataPilotPaneRowArea.drag(1, 1, 1, -50);
		assertEquals("Date", SCTool.getCellText("A3"));
		assertEquals("32779.17", SCTool.getCellInput("K30"));
		sleep(1);

		SCTool.selectRange("B33");
		activeMsgBox.yes();
		assertEquals("Date", SCTool.getCellText("D1"));
		assertEquals("32779.17", SCTool.getCellInput("AB31"));
		sleep(1);

		dataPilotColumnArea.click(1, 50);
		dataPilotColumnArea.openContextMenu();
		menuItem("Move to Row").select();
		assertEquals("Date", SCTool.getCellText("D1"));
		assertEquals("32779.17", SCTool.getCellInput("AB31"));

		SCTool.selectRange("B33");
		activeMsgBox.yes();
		assertEquals("Date", SCTool.getCellText("B3"));
		assertEquals("32779.17", SCTool.getCellInput("K30"));
		sleep(1);

		dataPilotAutomaticallyUpdateCheckBox.check();
	}

	/**
	 * 
	 * Verify that DP panel will be synchronized with table while move table.
	 * After the field name get changed, when you refresh the data, the
	 * DataPilot table will change to empty table if you create table small than
	 * the empty table, the table size will get larger if there is another
	 * DataPilot table exist in the larger range, there will pop a warning.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testMoveTableNarrowSpaceToShowTheTable() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));

		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();
		}

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Page").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		SCTool.selectRange("A1:B4");
		typeKeys("<$copy>");
		SCTool.selectRange("C1");
		typeKeys("<$paste>");
		sleep(1);

		SCTool.selectRange("$A.A1");
		typeKeys("New Locale" + "<enter>");

		SCTool.selectRange("$DataPilot_A_1.A1");
		spreadSheetCells.openContextMenu();
		menuItem("Refresh Data").select();
		activeMsgBox.ok();

		SCTool.selectRange("A1:B4");
		spreadSheetCells.drag(80, 45, 80, 350);

		assertEquals("New DataPilot Table", SCTool.getCellText("B20"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B22"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B23"));
		assertEquals("Page Area", SCTool.getCellText("B25"));
		assertEquals("Row Area", SCTool.getCellText("B28"));
		assertEquals("Column Area", SCTool.getCellText("D27"));
		assertEquals("Data Area", SCTool.getCellText("D29"));
		dataPilotPanel.close();
		sleep(1);

		SCTool.selectRange("C1:D4");
		spreadSheetCells.drag(210, 50, 10, 50);
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));
	}

	/**
	 * 
	 * Verify the data pilot result table refresh after source data changed
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testOneMRowDataPilotRefresh() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		calc.maximize();
		SCTool.selectRange("A1:E27");
		typeKeys("<$copy>");
		SCTool.selectRange("A1048540");
		typeKeys("<$paste>");
		calc.menuItem("Data->DataPilot->Start...").select();
		dataPilotTableToExistPlaceRadioButton.check();
		assertTrue(dataPilotTableToExistPlaceRadioButton.isChecked());
		dataPilotTableToExistPlaceEditBox.setText("$A.$F$1048540");
		createDataPilotTableDialog.ok();
		sleep(1);
		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();

		}
		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Page").select();
		assertEquals("Locale", SCTool.getCellText("F1048540"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("F1048542"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("F1048543"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Order Number", SCTool.getCellText("F1048542"));
		assertEquals("266773", SCTool.getCellText("O1048562"));
		sleep(1);

		SCTool.selectRange("D1048541");
		typeKeys("10000<enter>");
		sleep(1);

		SCTool.selectRange("O1048562");
		calc.menuItem("Data->DataPilot->Refresh").select();
		assertEquals("266525", SCTool.getCellText("O1048562"));
	}

	/**
	 * 
	 * Verify that DP panel will be synchronized with table while remove fields
	 * on panel.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testRemoveFieldsDiscardChange() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));

		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();
		}

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Page").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A4"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		dataPilotAutomaticallyUpdateCheckBox.uncheck();

		dataPilotColumnArea.drag(1, 1, -30, 1);
		assertEquals("Name", SCTool.getCellText("B3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		dataPilotPaneRowArea.click(1, 1);
		dataPilotPaneRowArea.openContextMenu();
		menuItem("Remove").select();
		assertEquals("Date", SCTool.getCellText("A4"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		dataPilotPanePageArea.drag(1, 1, -30, 1);
		assertEquals("Locale", SCTool.getCellText("A1"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		dataPiloPaneDataArea.openContextMenu();
		menuItem("Remove").select();
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));

		SCTool.selectRange("B25");
		activeMsgBox.yes();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));

		dataPilotAutomaticallyUpdateCheckBox.check();
	}

	/**
	 * 
	 * Verify the Remove function of Grouped field
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testRemoveFunctionOfGroupedField() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));

		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();
		}

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Page").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A4"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		SCTool.selectRange("B4:C4");
		spreadSheetCells.openContextMenu();
		menuItem("Group->Group Selected Items").select();
		assertEquals("Name2", SCTool.getCellText("B3"));
		assertEquals("Name", SCTool.getCellText("C3"));
		assertEquals("Group1", SCTool.getCellText("D4"));
		assertEquals("266773", SCTool.getCellText("J24"));
		sleep(1);

		SCTool.selectRange("D4");
		spreadSheetCells.openContextMenu();
		menuItem("Group->Ungroup").select();
		assertEquals("Name", SCTool.getCellText("B3"));
		assertEquals("", SCTool.getCellText("C3"));
		assertEquals("Bill Zhang", SCTool.getCellText("D4"));
		assertEquals("266773", SCTool.getCellText("J23"));
	}

	/**
	 * 
	 * Verify that DP panel will be synchronized with table while rename fields
	 * on panel.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testRenameFieldAutoUpdate() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));

		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();
		}

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Page").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A4"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		dataPilotPanePageArea.click(1, 1);
		dataPilotPanePageArea.openContextMenu();
		menuItem("Field Option").select();
		dataPilotFieldDisplayNameEditBox.setText("New Locale");
		dataPilotFieldOptionFieldTabPageDialog.ok();
		assertEquals("New Locale", SCTool.getCellText("A1"));
		sleep(1);

		typeKeys("<$undo>");
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		typeKeys("<$redo>");
		assertEquals("New Locale", SCTool.getCellText("A1"));
		sleep(1);

		SCTool.selectRange("B3");
		spreadSheetCells.openContextMenu();
		menuItem("Field Option...").select();
		dataPilotFieldDisplayNameEditBox.setText("New Name");
		dataPilotFieldOptionFieldTabPageDialog.ok();
		assertEquals("New Name", SCTool.getCellText("B3"));
		sleep(1);

		typeKeys("<$undo>");
		assertEquals("Name", SCTool.getCellText("B3"));
		sleep(1);

		typeKeys("<$redo>");
		assertEquals("New Name", SCTool.getCellText("B3"));
		sleep(1);

		SCTool.selectRange("A4");
		spreadSheetCells.openContextMenu();
		menuItem("Field Option...").select();
		dataPilotFieldDisplayNameEditBox.setText("New Date");
		dataPilotFieldOptionFieldTabPageDialog.ok();
		assertEquals("New Date", SCTool.getCellText("A4"));
		sleep(1);

		typeKeys("<$undo>");
		assertEquals("Date", SCTool.getCellText("A4"));
		sleep(1);

		typeKeys("<$redo>");
		assertEquals("New Date", SCTool.getCellText("A4"));
		sleep(1);

		dataPiloPaneDataArea.openContextMenu();
		menuItem("Field Option").select();
		dataPilotFieldDisplayNameEditBox.setText("New Sum - Order Number");
		dataPilotFieldOptionFieldTabPageDialog.ok();
		assertEquals("New Sum - Order Number", SCTool.getCellText("A3"));
		sleep(1);

		typeKeys("<$undo>");
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		sleep(1);

		typeKeys("<$redo>");
		assertEquals("New Sum - Order Number", SCTool.getCellText("A3"));
		sleep(1);
	}

	/**
	 * 
	 * Verify that DP panel will be synchronized with table while add fields on
	 * panel.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testSortFunctionInGroupedField() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));

		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();
		}

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Page").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A4"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		SCTool.selectRange("B4:C4");
		spreadSheetCells.openContextMenu();
		menuItem("Group->Group Selected Items").select();
		assertEquals("Name2", SCTool.getCellText("B3"));
		assertEquals("Name", SCTool.getCellInput("C3"));
		assertEquals("Group1", SCTool.getCellText("D4"));
		assertEquals("266773", SCTool.getCellText("J24"));
		sleep(1);

		SCTool.selectRange("B3");
		spreadSheetCells.openContextMenu();
		menuItem("Field Option...").select();
		dataPilotFieldOptionSortTabPageDialog.select();

		dataPilotFieldOptionSortDescendingRadioButton.check();
		dataPilotFieldOptionFieldTabPageDialog.select();
		dataPilotFieldOptionFieldTabPageDialog.ok();
		sleep(1);

		assertEquals("Kevin Wang", SCTool.getCellText("B4"));
		assertEquals("Group1", SCTool.getCellText("F4"));
		assertEquals("Bill Zhang", SCTool.getCellText("I4"));
		assertEquals("266773", SCTool.getCellText("J24"));

		typeKeys("<$undo>");
		assertEquals("Bill Zhang", SCTool.getCellText("B4"));
		assertEquals("Group1", SCTool.getCellText("D4"));
		assertEquals("Kevin Wang", SCTool.getCellText("I4"));
		assertEquals("266773", SCTool.getCellText("J24"));
		sleep(1);

		typeKeys("<$redo>");
		assertEquals("Kevin Wang", SCTool.getCellText("B4"));
		assertEquals("Group1", SCTool.getCellText("F4"));
		assertEquals("Bill Zhang", SCTool.getCellText("I4"));
		assertEquals("266773", SCTool.getCellText("J24"));
	}

	/**
	 * 
	 * Verify that DP panel will be synchronized with table while add fields on
	 * panel.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testSubtotalsFunctionInGroupedField() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));

		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();
		}

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Page").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A4"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Order Number", SCTool.getCellText("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		SCTool.selectRange("B4:C4");
		spreadSheetCells.openContextMenu();
		menuItem("Group->Group Selected Items").select();
		assertEquals("Name2", SCTool.getCellText("B3"));
		assertEquals("Name", SCTool.getCellText("C3"));
		assertEquals("Group1", SCTool.getCellText("D4"));
		assertEquals("266773", SCTool.getCellText("J24"));
		sleep(1);

		SCTool.selectRange("B3");
		spreadSheetCells.openContextMenu();
		menuItem("Field Option...").select();
		dataPilotFieldOptionFiledSubtotalsPage.select();
		sleep(1);

		dataPilotFieldOptionSubTotalsManuallyRadioButton.check();

		scDataPilotOptionSubtotalFunctionList.click(9, 24);
		sleep(1);
		dataPilotFieldOptionFieldTabPageDialog.select();
		dataPilotFieldOptionFieldTabPageDialog.ok();

		assertEquals("Bill Zhang Count - Order Number", SCTool.getCellText("C4"));
		assertEquals("5", SCTool.getCellText("C24"));

		assertEquals("Emmy Ma Count - Order Number", SCTool.getCellText("E4"));
		assertEquals("2", SCTool.getCellText("E24"));

		assertEquals("Group1 Count - Order Number", SCTool.getCellText("H4"));
		assertEquals("9", SCTool.getCellText("H24"));

		assertEquals("Harry Wu Count - Order Number", SCTool.getCellText("J4"));
		assertEquals("1", SCTool.getCellText("J24"));

		assertEquals("Jerry Lu Count - Order Number", SCTool.getCellText("L4"));
		assertEquals("2", SCTool.getCellText("L24"));

		assertEquals("Joe Liu Count - Order Number", SCTool.getCellText("N4"));
		assertEquals("4", SCTool.getCellText("N24"));

		assertEquals("Kevin Wang Count - Order Number", SCTool.getCellText("P4"));
		assertEquals("3", SCTool.getCellText("P24"));
		assertEquals("266773", SCTool.getCellText("Q24"));
		sleep(1);

		typeKeys("<$undo>");
		assertEquals("Emmy Ma", SCTool.getCellText("C4"));
		assertEquals("20518", SCTool.getCellText("C24"));

		assertEquals("", SCTool.getCellText("E4"));
		assertEquals("20528", SCTool.getCellText("E24"));

		assertEquals("Joe Liu", SCTool.getCellText("H4"));
		assertEquals("41056", SCTool.getCellText("H24"));

		assertEquals("Total Result", SCTool.getCellText("J4"));
		assertEquals("266773", SCTool.getCellText("J24"));

		assertEquals("", SCTool.getCellText("L4"));
		assertEquals("", SCTool.getCellText("L24"));

		assertEquals("", SCTool.getCellText("N4"));
		assertEquals("", SCTool.getCellText("N24"));

		assertEquals("", SCTool.getCellText("P4"));
		assertEquals("", SCTool.getCellText("P24"));
		assertEquals("", SCTool.getCellText("Q24"));
		sleep(1);

		typeKeys("<$redo>");
		assertEquals("Bill Zhang Count - Order Number", SCTool.getCellText("C4"));
		assertEquals("5", SCTool.getCellText("C24"));

		assertEquals("Emmy Ma Count - Order Number", SCTool.getCellText("E4"));
		assertEquals("2", SCTool.getCellText("E24"));

		assertEquals("Group1 Count - Order Number", SCTool.getCellText("H4"));
		assertEquals("9", SCTool.getCellText("H24"));

		assertEquals("Harry Wu Count - Order Number", SCTool.getCellText("J4"));
		assertEquals("1", SCTool.getCellText("J24"));

		assertEquals("Jerry Lu Count - Order Number", SCTool.getCellText("L4"));
		assertEquals("2", SCTool.getCellText("L24"));

		assertEquals("Joe Liu Count - Order Number", SCTool.getCellText("N4"));
		assertEquals("4", SCTool.getCellText("N24"));

		assertEquals("Kevin Wang Count - Order Number", SCTool.getCellText("P4"));
		assertEquals("3", SCTool.getCellText("P24"));
		assertEquals("266773", SCTool.getCellText("Q24"));
	}

	/**
	 * 
	 * Verify that DP panel will be synchronized with table while add fields on
	 * panel.
	 */
	@Ignore("Case Obsolete in AOO")
	@Test
	public void testTopNFunctionInGroupedField() {
		String file = prepareData("source_data01.ods");
		startcenter.menuItem("File->Open...").select();
		submitOpenDlg(file);
		sleep(2);
		SCTool.selectRange("A1:E27");
		calc.menuItem("Data->DataPilot->Start...").select();
		createDataPilotTableDialog.ok();
		assertEquals("New DataPilot Table", SCTool.getCellText("B2"));
		assertEquals("Use the DataPilot panel to assign fields to areas in the DataPilot table.", SCTool.getCellText("B4"));
		assertEquals("The DataPilot panel automatically displays when the DataPilot table has focus.", SCTool.getCellText("B5"));
		assertEquals("Page Area", SCTool.getCellText("B7"));
		assertEquals("Row Area", SCTool.getCellText("B10"));
		assertEquals("Column Area", SCTool.getCellText("D9"));
		assertEquals("Data Area", SCTool.getCellText("D11"));

		if (dataPilotAutomaticallyUpdateCheckBox.isChecked() == false) {
			dataPilotAutomaticallyUpdateCheckBox.check();
		}

		dataPilotFieldSelect.click(1, 1);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Page").select();
		assertEquals("Locale", SCTool.getCellText("A1"));
		sleep(1);

		dataPilotFieldSelect.click(1, 30);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Column").select();
		assertEquals("Name", SCTool.getCellText("A3"));
		sleep(1);

		dataPilotFieldSelect.click(1, 50);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Row").select();
		assertEquals("Date", SCTool.getCellText("A4"));
		sleep(1);

		dataPilotFieldSelect.click(1, 70);
		dataPilotFieldSelect.openContextMenu();
		menuItem("Add to Data by->Sum").select();
		assertEquals("Sum - Order Number", SCTool.getCellInput("A3"));
		assertEquals("266773", SCTool.getCellText("J23"));
		sleep(1);

		SCTool.selectRange("B4:C4");
		spreadSheetCells.openContextMenu();
		menuItem("Group->Group Selected Items").select();
		assertEquals("Name2", SCTool.getCellText("B3"));
		assertEquals("Name", SCTool.getCellText("C3"));
		assertEquals("Group1", SCTool.getCellText("D4"));
		assertEquals("266773", SCTool.getCellText("J24"));
		sleep(1);

		SCTool.selectRange("A1");
		sleep(1);

		// the Display resolution is 1280 * 1050 on windows and Linux and 1920 *
		// 1080 on Mac
		if (SystemUtil.isWindows()) {
			spreadSheetCells.click(238, 43);
		} else if (SystemUtil.isLinux()) {
			spreadSheetCells.click(267, 43);
		} else if (SystemUtil.isMac()) {
			spreadSheetCells.click(238, 43);
		}

		typeKeys("<tab>");
		typeKeys("<tab>");
		typeKeys("<tab>");
		typeKeys("<tab>");
		typeKeys("<tab>");
		typeKeys("<tab>");
		typeKeys("<enter>");
		onlyDisplayTopNItemCheckBox.check();
		numberOfItemShowInTopNEditBox.setText("4");
		fieldTopNSettingDialog.ok();
		sleep(1);

		assertEquals("Bill Zhang", SCTool.getCellText("B4"));
		assertEquals("51299", SCTool.getCellText("B24"));

		assertEquals("Group1", SCTool.getCellText("C4"));
		assertEquals("71806", SCTool.getCellText("C24"));
		assertEquals("", SCTool.getCellText("D4"));
		assertEquals("Amy Zhao", SCTool.getCellText("C5"));
		assertEquals("Anne Lee", SCTool.getCellText("D5"));
		assertEquals("20528", SCTool.getCellText("D24"));

		assertEquals("Joe Liu", SCTool.getCellText("E4"));
		assertEquals("41056", SCTool.getCellText("E24"));

		assertEquals("Kevin Wang", SCTool.getCellText("F4"));
		assertEquals("30771", SCTool.getCellText("F24"));

		assertEquals("Total Result", SCTool.getCellText("G4"));
		assertEquals("215460", SCTool.getCellText("G24"));
		sleep(1);

		typeKeys("<$undo>");
		assertEquals("Name2", SCTool.getCellText("B3"));
		assertEquals("Name", SCTool.getCellText("C3"));
		assertEquals("Group1", SCTool.getCellText("D4"));
		assertEquals("266773", SCTool.getCellText("J24"));

		assertEquals("Bill Zhang", SCTool.getCellText("B4"));
		assertEquals("51299", SCTool.getCellText("B24"));

		assertEquals("Emmy Ma", SCTool.getCellText("C4"));
		assertEquals("20518", SCTool.getCellText("C24"));
		assertEquals("Group1", SCTool.getCellText("D4"));
		assertEquals("Emmy Ma", SCTool.getCellText("C5"));
		assertEquals("Amy Zhao", SCTool.getCellText("D5"));
		assertEquals("71806", SCTool.getCellText("D24"));

		assertEquals("", SCTool.getCellText("E4"));
		assertEquals("20528", SCTool.getCellText("E24"));

		assertEquals("Harry Wu", SCTool.getCellText("F4"));
		assertEquals("10265", SCTool.getCellText("F24"));

		assertEquals("Jerry Lu", SCTool.getCellText("G4"));
		assertEquals("20530", SCTool.getCellText("G24"));

		assertEquals("Total Result", SCTool.getCellText("J4"));
		assertEquals("266773", SCTool.getCellText("J24"));
		sleep(1);

		typeKeys("<$redo>");
		assertEquals("Bill Zhang", SCTool.getCellText("B4"));
		assertEquals("51299", SCTool.getCellText("B24"));

		assertEquals("Group1", SCTool.getCellText("C4"));
		assertEquals("71806", SCTool.getCellText("C24"));
		assertEquals("", SCTool.getCellText("D4"));
		assertEquals("Amy Zhao", SCTool.getCellText("C5"));
		assertEquals("Anne Lee", SCTool.getCellText("D5"));
		assertEquals("20528", SCTool.getCellText("D24"));

		assertEquals("Joe Liu", SCTool.getCellText("E4"));
		assertEquals("41056", SCTool.getCellText("E24"));

		assertEquals("Kevin Wang", SCTool.getCellText("F4"));
		assertEquals("30771", SCTool.getCellText("F24"));

		assertEquals("Total Result", SCTool.getCellText("G4"));
		assertEquals("215460", SCTool.getCellText("G24"));
	}
}
