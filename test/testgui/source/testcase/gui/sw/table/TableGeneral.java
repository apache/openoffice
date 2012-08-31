package testcase.gui.sw.table;

import static org.junit.Assert.*;
import static org.openoffice.test.vcl.Tester.*;
import static testlib.gui.AppUtil.*;
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.Logger;
import org.openoffice.test.common.SystemUtil;


public class TableGeneral {

	@Rule
	public Logger log = Logger.getLogger(this);

	@Before
	public void setUp() throws Exception {
		// Start OpenOffice
		app.start();

		// Create a new text document
		app.dispatch("private:factory/swriter");

		// Insert a table
		app.dispatch(".uno:InsertTable");
		assertTrue("Insert Table dialog pop up", writer_InsertTable.exists());
	}

	@After
	public void tearDown() throws Exception {
		app.close();
	}

	@Test
	// Test setting table size in text document
	public void testTableSize() throws Exception {

		SWTableSizeColBox.focus();
		typeKeys("<delete>");
		typeKeys("3");
		SWTableSizeRowBox.focus();
		typeKeys("<delete>");
		typeKeys("4");
		writer_InsertTable.ok();

		writer.focus(); // verify how many rows in the table
		assertNotNull(StatusBar.getItemTextById(8));
		for (int i = 0; i < 3; i++) {
			typeKeys("<down>");
			assertNotNull(StatusBar.getItemTextById(8));
		}
		typeKeys("<down>");
		sleep(1);
		assertFalse(Table_Toolbar.exists());
	}

	// Test setting table cell background in text document

	@Test
	@Ignore
	// bug120378
	public void testTableBackground() throws Exception {
		writer_InsertTable.ok();
		assertNotNull(StatusBar.getItemTextById(8));
		writer.focus();
		// set table cell background
		app.dispatch(".uno:TableDialog");
		SWTableBackground.select();
		assertTrue("Table background property dialog pop up",
				SWTableBackground.exists());
		;
		SWTableBackgroundColor.focus();
		SWTableBackgroundColor.click(50, 50);
		SWTableBackground.ok();
		// verify table cell background color
		writer.focus();
		// select the cell which is filled with color
		app.dispatch(".uno:EntireCell");

		typeKeys("<ctrl c>");
		app.dispatch("private:factory/simpress?slot=6686");
		PresentationWizard.ok();
		typeKeys("<ctrl v>");
		// enable table cell area format dialog
		app.dispatch(".uno:FormatArea");
		sleep(1);
		assertEquals("Light red", SDTable_BACGColorListbox.getSelText());
		// close table cell area format dialog
		SDTable_BACGColorArea.cancel();
	}

	// Test setting table border in text document

	@Test
	public void testTableBorder() throws Exception {
		writer_InsertTable.ok();
		assertNotNull(StatusBar.getItemTextById(8));
		// set table border as none
		writer.focus();
		app.dispatch(".uno:TableDialog");
		SWTableBorder.select();
		assertTrue("Table border property dialog pop up",
				SWTableBorder.exists());
		;
		SWTableBorderLineArrange.click(10, 10);
		SWTableBorder.ok();
	}

	// Test setting table border line style,line color,spacing to content in
	// text document

	@Test
	public void testTableBorderLineStyle() throws Exception {
		writer_InsertTable.ok();
		assertNotNull(StatusBar.getItemTextById(8));
		writer.focus();
		app.dispatch(".uno:TableDialog");
		SWTableBorder.select();
		assertTrue("Table border property dialog pop up",
				SWTableBorder.exists());
		;
		SWTableBorderLineStyle.select(8); // set line style
		SWTableBorderLineColor.select(5); // set line color
		SWTableSTCLeft.focus(); // set spacing to content
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("0.5"); // set spacing to content
		SWTableShadow.click(40, 10); // set table shadow
		SWTableShadowSize.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("2");
		SWTableShadowColor.select(5);
		SWTableBorder.ok();

		writer.focus(); // verify the setting property of table
		app.dispatch(".uno:TableDialog");
		SWTableBorder.select();
		assertEquals("2.60 pt", SWTableBorderLineStyle.getItemText(8));
		assertEquals("Magenta", SWTableBorderLineColor.getItemText(5));
		assertEquals("0.50\"", SWTableSTCLeft.getText());
		assertEquals("0.50\"", SWTableSTCRight.getText());
		assertEquals("0.50\"", SWTableSTCTop.getText());
		assertEquals("0.50\"", SWTableSTCBottom.getText());
		assertEquals("1.97\"", SWTableShadowSize.getText());
		assertEquals("Magenta", SWTableShadowColor.getItemText(5));
		assertTrue("SWTableSTC_SYNC", SWTableSTC_SYNC.isChecked());
		SWTableBorder.close();

		// uncheck Synchronize box and set spacing to content

		writer.focus();
		app.dispatch(".uno:TableDialog");
		SWTableBorder.select();
		SWTableSTC_SYNC.uncheck();
		SWTableSTCLeft.focus();// set left spacing to content
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("0.5");
		SWTableSTCRight.focus();// set right spacing to content
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("0.8");
		SWTableSTCTop.focus();// set top spacing to content
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("1.0");
		SWTableSTCBottom.focus();// set bottom spacing to content
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("2");
		SWTableBorder.ok();
		assertNotNull(StatusBar.getItemTextById(8));

		writer.focus(); // verify the setting value of spacing to content for
						// tabel
		app.dispatch(".uno:TableDialog");
		SWTableBorder.select();
		assertEquals("0.50\"", SWTableSTCLeft.getText());
		assertEquals("0.80\"", SWTableSTCRight.getText());
		assertEquals("1.00\"", SWTableSTCTop.getText());
		assertEquals("1.97\"", SWTableSTCBottom.getText());
		assertFalse("SWTableSTC_SYNC", SWTableSTC_SYNC.isChecked());
		SWTableBorder.close();
	}

	// create table with auto format

	@Test
	public void testTableAutoFormat() throws Exception {
		// create table with auto format
		button("sw:PushButton:DLG_INSERT_TABLE:BT_AUTOFORMAT").click();
		assertTrue("Table auto format dialog pop up", SWTableAutoFMT.exists());
		SWTableAutoFormat_Listbox.select(3);
		SWTableAutoFMT.ok();
		// verify the auto format is that just selected
		button("sw:PushButton:DLG_INSERT_TABLE:BT_AUTOFORMAT").click();
		assertEquals("Blue", SWTableAutoFormat_Listbox.getSelText());
		SWTableAutoFMT.close();
		writer_InsertTable.ok();
		assertNotNull(StatusBar.getItemTextById(8));

	}

	// set row height and select row,insert/delete row

	@Test
	public void testTableRowHeight() throws Exception {
		writer_InsertTable.ok();
		assertNotNull(StatusBar.getItemTextById(8));

		// set row height
		writer.focus();
		writer.openContextMenu();
		SWTable_RowHeightMenu.select();
		assertTrue(SWTable_SetRowHeight_Dialog.exists());
		SWTable_SetRowHeight.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("0.5");
		SWTable_SetRowHeight_Dialog.ok();

		// verify row height
		writer.focus();
		writer.openContextMenu();
		SWTable_RowHeightMenu.select();
		assertTrue(SWTable_SetRowHeight_Dialog.exists());
		assertEquals("0.50\"", SWTable_SetRowHeight.getText());
		SWTable_SetRowHeight_Dialog.close();
	}

	// select row
	@Test
	public void testTableSelectRow() throws Exception {
		writer_InsertTable.ok();
		assertNotNull(StatusBar.getItemTextById(8));
		// select row
		writer.focus();
		writer.openContextMenu();
		SWTable_SelectRowMenu.select();

		// verify select one row successfully
		typeKeys("<ctrl c>");
		typeKeys("<down>");
		typeKeys("<down>");
		typeKeys("<enter>");
		typeKeys("<ctrl v>");
		typeKeys("<up>");
		assertTrue(Table_Toolbar.exists());

	}

	// insert row and verify how many row inserted
	@Test
	public void testTableInsertRow() throws Exception {
		writer_InsertTable.ok();
		assertNotNull(StatusBar.getItemTextById(8));
		writer.focus();
		writer.openContextMenu();
		SWTable_InsertRowMenu.select();
		assertTrue("SWTable_InsertRow Dialog pop up",
				SWTable_InsertRow.exists());
		SWTable_InsertRow_Column_SetNumber.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("3");
		SWTable_InsertRow.ok();

		writer.focus(); // verify how many rows in the table
		assertNotNull(StatusBar.getItemTextById(8));
		for (int i = 0; i < 4; i++) {
			typeKeys("<down>");
			assertNotNull(StatusBar.getItemTextById(8));
		}
		typeKeys("<down>");
		sleep(1);
		assertFalse(Table_Toolbar.exists());
	}

	// delete row and verify row
	@Test
	public void testTableRowDelete() throws Exception {
		writer_InsertTable.ok();
		assertNotNull(StatusBar.getItemTextById(8));
		// delete row
		writer.focus();
		writer.openContextMenu();
		SWTable_RowDleteMenu.select();
		// verify whether delete row
		writer.focus();
		assertNotNull(StatusBar.getItemTextById(8));
		typeKeys("<down>");
		sleep(1);
		assertFalse(Table_Toolbar.exists());

	}

	// set column width and verify
	@Test
	public void testTableColumnWidth() throws Exception {
		writer_InsertTable.ok();
		assertNotNull(StatusBar.getItemTextById(8));
		// set column width
		writer.focus();
		writer.openContextMenu();
		SWTable_ColumnWidthMenu.select();
		SWTable_SetColumnWidth.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("2");
		SWTable_SetColumnDialog.ok();
		// verify column width
		writer.focus();
		writer.openContextMenu();
		SWTable_ColumnWidthMenu.select();
		assertEquals("2.00\"", SWTable_SetColumnWidth.getText());

	}

	// select column and verify
	@Test
	public void testTableColumnSelect() throws Exception {
		writer_InsertTable.ok();
		assertNotNull(StatusBar.getItemTextById(8));
		writer.focus();
		writer.openContextMenu();
		SWTable_ColumnSelectMenu.select();

		// verify select one column
		typeKeys("<ctrl c>");
		typeKeys("<down>");
		typeKeys("<down>");
		typeKeys("<enter>");
		typeKeys("<ctrl v>");
		typeKeys("<up>");
		assertTrue(Table_Toolbar.exists());

	}

	// insert column and verify
	@Test
	public void testTableColumnInsert() throws Exception {
		writer_InsertTable.ok();
		assertNotNull(StatusBar.getItemTextById(8));
		// insert column
		writer.focus();
		writer.openContextMenu();
		SWTable_ColumnInsertMenu.select();
		SWTable_InsertRow_Column_SetNumber.focus();
		typeKeys("<ctrl a>");
		typeKeys("<delete>");
		typeKeys("3");
		SWTable_InsertColumn.ok();
		// verify insert column successfully
		writer.focus();
		assertTrue(Table_Toolbar.exists());
		for (int i = 0; i < 9; i++) {
			typeKeys("<right>");
			sleep(1);
			assertTrue(Table_Toolbar.exists());
		}
		typeKeys("<right>");
		sleep(1);
		assertFalse(Table_Toolbar.exists());
	}

	// delete column and verify whether delete or not
	public void testTableColumnDelete() throws Exception {
		writer_InsertTable.ok();
		assertNotNull(StatusBar.getItemTextById(8));
		// delete column
		writer.focus();
		writer.openContextMenu();
		SWTable_ColumnDeleteMenu.select();
		// verify delete column
		writer.focus();
		assertTrue(Table_Toolbar.exists());
		for (int i = 0; i < 7; i++) {
			typeKeys("<right>");
			assertTrue(Table_Toolbar.exists());
		}
		sleep(1);
		assertFalse(Table_Toolbar.exists());
	}

	// split cell
	@Test
	public void testTableCellSplit() throws Exception {
		writer_InsertTable.ok();
		assertTrue(Table_Toolbar.exists());
		for (int k = 0; k < 2; k++) {
			writer.focus();
			writer.openContextMenu();
			SWTable_CellSplitMenu.select();
			SWTable_CellSplitNumber.focus();
			typeKeys("<ctrl a>");
			typeKeys("<delete>");
			typeKeys("2");
			if (k == 0) {
				SWTable_CellSplitDialog.ok(); // split table cell horizontally
			} else {
				SWTable_CellSplitVERT_Button.check(); // split table cell
				// vertically
				SWTable_CellSplitDialog.ok();
			}
		}
		// verify cell split successfully
		writer.focus();
		assertTrue(Table_Toolbar.exists());
		for (int i = 0; i < 7; i++) {
			typeKeys("<right>");
			assertTrue(Table_Toolbar.exists());
		}
		sleep(1);
		assertFalse(Table_Toolbar.exists());
	}
	
	/**
	 * Test convert table to text in text document
	 * 
	 * @throws Exception
	 */
	@Test
	public void testConvertTableToText() throws Exception {
		writer_InsertTable.ok();
		writer.focus();
		typeKeys("1<right>2<right>3<right>4");
		sleep(1);

		// Convert table to text
		app.dispatch(".uno:ConvertTableToText");
		assertTrue("Convert Table to Text dialog pop up", writer_ConvertTableToTextDlg.exists());
		// typeKeys("<enter>");
		writer_ConvertTableToTextDlg.ok(); // "Enter" does not work on linux

		// Verify if text is converted successfully
		app.dispatch(".uno:SelectAll");
		app.dispatch(".uno:Copy");
		if (SystemUtil.isWindows())
			assertEquals("Converted text", "1\t2\r\n3\t4\r\n", app.getClipboard()); // in
																					// windows,
																					// \n
																					// is
																					// \r\n
		else
			assertEquals("Converted text", "1\t2\n3\t4\n", app.getClipboard());
	}
}