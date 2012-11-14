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

package fvt.gui.sw.table;

import static org.junit.Assert.*;
import static org.openoffice.test.vcl.Tester.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;
import org.openoffice.test.common.SystemUtil;

import testlib.gui.AppTool;

public class TableGeneral {

	@Before
	public void setUp() throws Exception {
		// Start OpenOffice
		app.start();

		AppTool.newTextDocument();
		// Insert a table
		app.dispatch(".uno:InsertTable");
	}

	@After
	public void tearDown() throws Exception {
		app.stop();
	}

	/**
	 * Test setting table size in text document
	 * 
	 * @throws Exception
	 */
	@Test
	public void testTableSize() throws Exception {

		swTableSizeColBox.focus();
		typeKeys("<delete>");
		typeKeys("3");
		swTableSizeRowBox.focus();
		typeKeys("<delete>");
		typeKeys("4");
		writerInsertTable.ok();

		writer.focus();
		// verify the rows in the table
		assertNotNull(statusBar.getItemTextById(8));
		for (int i = 0; i < 3; i++) {
			typeKeys("<down>");
			assertNotNull(statusBar.getItemTextById(8));
		}
		typeKeys("<down>");
		sleep(1);
		assertFalse(tableToolbar.exists());
	}

	/**
	 * Test setting table cell background in text document
	 * 
	 * @throws Exception
	 */
	@Test
	@Ignore("Bug #120378- the table cell fill color change when copy one table cell in word processor to presentation")
	public void testTableBackground() throws Exception {
		writerInsertTable.ok();
		assertNotNull(statusBar.getItemTextById(8));
		writer.focus();
		// set table cell background
		app.dispatch(".uno:TableDialog");
		swTableBackground.select();
		assertTrue("Table background property dialog pop up",
				swTableBackground.exists());
		swTableBackgroundColor.focus();
		swTableBackgroundColor.click(50, 50);
		swTableBackground.ok();
		// verify table cell background color
		writer.focus();
		// select the cell which is filled with color
		app.dispatch(".uno:EntireCell");

		typeKeys("<ctrl c>");
		AppTool.newPresentation();
		typeKeys("<ctrl v>");
		// enable table cell area format dialog
		app.dispatch(".uno:FormatArea");
		sleep(1);
		assertEquals("Light red", sdTableBACGColorListbox.getSelText());
		// close table cell area format dialog
		sdTableBACGColorArea.cancel();
	}

	/**
	 * Test setting table border in text document
	 * 
	 * @throws Exception
	 */
	@Test
	public void testTableBorder() throws Exception {
		writerInsertTable.ok();
		assertNotNull(statusBar.getItemTextById(8));
		// set table border as none
		writer.focus();
		app.dispatch(".uno:TableDialog");
		swTableBorder.select();
		assertTrue("Table border property dialog pop up",
				swTableBorder.exists());
		swTableBorderLineArrange.click(10, 10);
		swTableBorder.ok();
	}

	/**
	 * Test setting table border line style,line color,spacing to content in
	 * text document
	 * 
	 * @throws Exception
	 */
	@Test
	public void testTableBorderLineStyle() throws Exception {
		writerInsertTable.ok();
		assertNotNull(statusBar.getItemTextById(8));
		writer.focus();
		app.dispatch(".uno:TableDialog");
		swTableBorder.select();
		assertTrue("Table border property dialog pop up",
				swTableBorder.exists());
		// set line style
		swTableBorderLineStyle.select(8);
		// set line color
		swTableBorderLineColor.select(5);
		// set spacing to content
		swTableSTCLeft.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		// set spacing to content
		typeKeys("0.5");
		// set table shadow
		swTableShadow.click(40, 10);
		swTableShadowSize.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("2");
		swTableShadowColor.select(5);
		swTableBorder.ok();
		// verify the setting property of table
		writer.focus();
		app.dispatch(".uno:TableDialog");
		swTableBorder.select();
		assertEquals("2.60 pt", swTableBorderLineStyle.getItemText(8));
		assertEquals("Magenta", swTableBorderLineColor.getItemText(5));
		assertEquals("0.50\"", swTableSTCLeft.getText());
		assertEquals("0.50\"", swTableSTCRight.getText());
		assertEquals("0.50\"", swTableSTCTop.getText());
		assertEquals("0.50\"", swTableSTCBottom.getText());
		assertEquals("1.97\"", swTableShadowSize.getText());
		assertEquals("Magenta", swTableShadowColor.getItemText(5));
		assertTrue("SWTableSTC_SYNC", swTableSTCSYNC.isChecked());
		swTableBorder.close();

		// uncheck Synchronize box and set spacing to content
		writer.focus();
		app.dispatch(".uno:TableDialog");
		swTableBorder.select();
		swTableSTCSYNC.uncheck();
		// set left spacing to content
		swTableSTCLeft.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("0.5");
		// set right spacing to content
		swTableSTCRight.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("0.8");
		// set top spacing to content
		swTableSTCTop.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("1.0");
		// set bottom spacing to content
		swTableSTCBottom.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("2");
		swTableBorder.ok();
		assertNotNull(statusBar.getItemTextById(8));

		writer.focus();
		// verify the setting value of spacing to content for table
		app.dispatch(".uno:TableDialog");
		swTableBorder.select();
		assertEquals("0.50\"", swTableSTCLeft.getText());
		assertEquals("0.80\"", swTableSTCRight.getText());
		assertEquals("1.00\"", swTableSTCTop.getText());
		assertEquals("1.97\"", swTableSTCBottom.getText());
		assertFalse("SWTableSTC_SYNC", swTableSTCSYNC.isChecked());
		swTableBorder.close();
	}

	/**
	 * create table with auto format
	 * 
	 * @throws Exception
	 */
	@Test
	public void testTableAutoFormat() throws Exception {
		// create table with auto format
		button("sw:PushButton:DLG_INSERT_TABLE:BT_AUTOFORMAT").click();
		assertTrue("Table auto format dialog pop up", swTableAutoFMT.exists());
		swTableAutoFormatListbox.select(3);
		swTableAutoFMT.ok();
		// verify the auto format is that just selected
		button("sw:PushButton:DLG_INSERT_TABLE:BT_AUTOFORMAT").click();
		assertEquals("Blue", swTableAutoFormatListbox.getSelText());
		swTableAutoFMT.close();
		writerInsertTable.ok();
		assertNotNull(statusBar.getItemTextById(8));

	}

	/**
	 * set row height and select row,insert/delete row
	 * 
	 * @throws Exception
	 */
	@Test
	public void testTableRowHeight() throws Exception {
		writerInsertTable.ok();
		assertNotNull(statusBar.getItemTextById(8));

		// set row height
		writer.focus();
		writer.openContextMenu();
		swTableRowHeightMenu.select();
		assertTrue(swTableSetRowHeightDialog.exists());
		swTableSetRowHeight.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("0.5");
		swTableSetRowHeightDialog.ok();

		// verify row height
		writer.focus();
		writer.openContextMenu();
		swTableRowHeightMenu.select();
		assertTrue(swTableSetRowHeightDialog.exists());
		assertEquals("0.50\"", swTableSetRowHeight.getText());
		swTableSetRowHeightDialog.close();
	}

	/**
	 * test select row
	 * 
	 * @throws Exception
	 */
	@Test
	public void testTableSelectRow() throws Exception {
		writerInsertTable.ok();
		assertNotNull(statusBar.getItemTextById(8));
		// select row
		writer.focus();
		writer.openContextMenu();
		swTableSelectRowMenu.select();

		// verify select one row successfully
		typeKeys("<ctrl c>");
		typeKeys("<down>");
		typeKeys("<down>");
		typeKeys("<enter>");
		typeKeys("<ctrl v>");
		typeKeys("<up>");
		assertTrue(tableToolbar.exists());

	}

	/**
	 * insert row and verify how many row inserted
	 * 
	 * @throws Exception
	 */
	@Test
	public void testTableInsertRow() throws Exception {
		writerInsertTable.ok();
		assertNotNull(statusBar.getItemTextById(8));
		writer.focus();
		writer.openContextMenu();
		swTableInsertRowMenu.select();
		assertTrue("SWTable_InsertRow Dialog pop up", swTableInsertRow.exists());
		swTableInsertRowColumnSetNumber.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("3");
		swTableInsertRow.ok();

		writer.focus();
		// verify how many rows in the table
		assertNotNull(statusBar.getItemTextById(8));
		for (int i = 0; i < 4; i++) {
			typeKeys("<down>");
			assertNotNull(statusBar.getItemTextById(8));
		}
		typeKeys("<down>");
		sleep(1);
		assertFalse(tableToolbar.exists());
	}

	/**
	 * delete row and verify row
	 * 
	 * @throws Exception
	 */
	@Test
	public void testTableRowDelete() throws Exception {
		writerInsertTable.ok();
		assertNotNull(statusBar.getItemTextById(8));
		// delete row
		writer.focus();
		writer.openContextMenu();
		swTableRowDleteMenu.select();
		// verify whether delete row
		writer.focus();
		assertNotNull(statusBar.getItemTextById(8));
		typeKeys("<down>");
		sleep(1);
		assertFalse(tableToolbar.exists());

	}

	/**
	 * set column width and verify
	 * 
	 * @throws Exception
	 */
	@Test
	public void testTableColumnWidth() throws Exception {
		writerInsertTable.ok();
		assertNotNull(statusBar.getItemTextById(8));
		// set column width
		writer.focus();
		writer.openContextMenu();
		swTableColumnWidthMenu.select();
		swTableSetColumnWidth.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("2");
		swTableSetColumnDialog.ok();
		// verify column width
		writer.focus();
		writer.openContextMenu();
		swTableColumnWidthMenu.select();
		assertEquals("2.00\"", swTableSetColumnWidth.getText());

	}

	/**
	 * select column and verify
	 * 
	 * @throws Exception
	 */
	@Test
	public void testTableColumnSelect() throws Exception {
		writerInsertTable.ok();
		assertNotNull(statusBar.getItemTextById(8));
		writer.focus();
		writer.openContextMenu();
		swTableColumnSelectMenu.select();

		// verify select one column
		typeKeys("<ctrl c>");
		typeKeys("<down>");
		typeKeys("<down>");
		typeKeys("<enter>");
		typeKeys("<ctrl v>");
		typeKeys("<up>");
		assertTrue(tableToolbar.exists());

	}

	/**
	 * insert column and verify
	 * 
	 * @throws Exception
	 */
	@Test
	public void testTableColumnInsert() throws Exception {
		writerInsertTable.ok();
		assertNotNull(statusBar.getItemTextById(8));
		// insert column
		writer.focus();
		writer.openContextMenu();
		swTableColumnInsertMenu.select();
		swTableInsertRowColumnSetNumber.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("3");
		swTableInsertColumn.ok();
		// verify insert column successfully
		writer.focus();
		assertTrue(tableToolbar.exists());
		for (int i = 0; i < 9; i++) {
			typeKeys("<right>");
			sleep(1);
			assertTrue(tableToolbar.exists());
		}
		typeKeys("<right>");
		sleep(1);
		assertFalse(tableToolbar.exists());
	}

	/**
	 * delete column and verify whether delete or not
	 * 
	 * @throws Exception
	 */
	public void testTableColumnDelete() throws Exception {
		writerInsertTable.ok();
		assertNotNull(statusBar.getItemTextById(8));
		// delete column
		writer.focus();
		writer.openContextMenu();
		swTableColumnDeleteMenu.select();
		// verify delete column
		writer.focus();
		assertTrue(tableToolbar.exists());
		for (int i = 0; i < 7; i++) {
			typeKeys("<right>");
			assertTrue(tableToolbar.exists());
		}
		sleep(1);
		assertFalse(tableToolbar.exists());
	}

	/**
	 * split cell
	 * 
	 * @throws Exception
	 */
	@Test
	public void testTableCellSplit() throws Exception {
		writerInsertTable.ok();
		assertTrue(tableToolbar.exists());
		for (int k = 0; k < 2; k++) {
			writer.focus();
			writer.openContextMenu();
			swTableCellSplitMenu.select();
			swTableCellSplitNumber.focus();
			typeKeys("<ctrl a>");
			typeKeys("<delete>");
			typeKeys("2");
			if (k == 0) {
				// split table cell horizontally
				swTableCellSplitDialog.ok();
			} else {
				// split table cell
				swTableCellSplitVERTButton.check();
				// vertically
				swTableCellSplitDialog.ok();
			}
		}
		// verify cell split successfully
		writer.focus();
		assertTrue(tableToolbar.exists());
		for (int i = 0; i < 7; i++) {
			typeKeys("<right>");
			assertTrue(tableToolbar.exists());
		}
		sleep(1);
		assertFalse(tableToolbar.exists());
	}

	/**
	 * Test convert table to text in text document
	 * 
	 * @throws Exception
	 */
	@Test
	public void testConvertTableToText() throws Exception {
		writerInsertTable.ok();
		writer.focus();
		typeKeys("1<right>2<right>3<right>4");
		sleep(1);

		// Convert table to text
		app.dispatch(".uno:ConvertTableToText");
		assertTrue("Convert Table to Text dialog pop up",
				writerConvertTableToTextDlg.exists());
		// typeKeys("<enter>");
		writerConvertTableToTextDlg.ok();

		// Verify if text is converted successfully
		app.dispatch(".uno:SelectAll");
		app.dispatch(".uno:Copy");
		if (SystemUtil.isWindows())
			assertEquals("Converted text", "1\t2\r\n3\t4\r\n",
					app.getClipboard());
		else
			assertEquals("Converted text", "1\t2\n3\t4\n", app.getClipboard());
	}
}
