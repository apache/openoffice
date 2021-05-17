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
package bvt.gui;

import static org.junit.Assert.*;
import static org.openoffice.test.common.Testspace.*;
import static org.openoffice.test.vcl.Tester.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import java.awt.Rectangle;
import java.io.File;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.GraphicsUtil;
import org.openoffice.test.common.Logger;

import testlib.gui.SCTool;

/**
 * 
 */
public class BasicFunctionTest {

	@Rule
	public Logger log = Logger.getLogger(this);

	@BeforeClass
	public static void beforeClass() {
		app.clean();
	}

	@AfterClass
	public static void afterClass() {
		app.stop();
	}
	
	@Before
	public void before() {
		app.stop();
		app.start();
	}
	

	@Test
	public void smokeTest() {
		File smoketestOutput = new File(aoo.getUserInstallation(), "user/temp");
		prepareData("TestExtension.oxt");
		// Open sample file smoketestdoc.sxw
		open(prepareData("smoketestdoc.sxw"));
		writer.waitForEnabled(10, 2);
		// Run test cases
		app.dispatch("vnd.sun.star.script:Standard.Global.StartTestWithDefaultOptions?language=Basic&location=document", 120);
		String smoketestlog = FileUtil.readFileAsString(new File(smoketestOutput, "smoketest.log"));
		String testclosurelog = FileUtil.readFileAsString(new File(smoketestOutput, "testclosure.log"));
		log.info(smoketestlog + "\n" + testclosurelog);
		assertTrue("No Error", !smoketestlog.contains("error") && !testclosurelog.contains("error"));

	}
	
	@Test
	public void testExportAsPDF() throws Exception {
		String file = prepareData("bvt/pdf.odt");
		String exportTo1 = getPath("temp/1.pdf");
		String exportTo2 = getPath("temp/2.pdf");
		deleteFile(exportTo1);
		deleteFile(exportTo2);
		open(file);
		writer.waitForExistence(10, 1);
		app.dispatch(".uno:ExportToPDF");
		pdfGeneralPage.ok();
		submitSaveDlg(exportTo1);
		sleep(1);
		String magic = FileUtil.readFileAsString(exportTo1).substring(0, 4);
		assertEquals("PDF is exported?", "%PDF", magic);
		
		button(".uno:ExportDirectToPDF").click();//Click via toolbar
		submitSaveDlg(exportTo2);
		sleep(1);
		magic = FileUtil.readFileAsString(exportTo2).substring(0, 4);
		assertEquals("PDF is exported directly?", "%PDF", magic);
	}

	/**
	 * Test the File -- Print Dialog show
	 * 
	 */
	@Test
	public void testPrinter() {
		// Create a new text document
		newTextDocument();
		app.dispatch(".uno:PrinterSetup");
		if (activeMsgBox.exists(2))
			activeMsgBox.ok();
		
//		PrintService[] ps = PrintServiceLookup.lookupPrintServices(null, null);
//		String[] names = new String[ps.length];
//		for (int i = 0; i < ps.length; i++) {
//			names[i] = ps[i].getName();
//		}
//		
//		assertArrayEquals("Printers Names", names, printerSetUpDlgPrinterNames.getItemsText());
		assertTrue("Printer Setup dialog appears", printerSetUpDlg.exists(3));
		printerSetUpDlg.cancel();
	}

	/**
	 * Test the File -- Java Dialog show
	 * 
	 */
//	@Test
//	public void testJavaDialog() {
//
//		// Create a new text document and launch a Wizards dialog which need JVM
//		// work correctly.
//		app.dispatch("private:factory/swriter");
//		File tempfile = new File(oo.getUserInstallation(), "user/template/myAgendaTemplate.ott");
//		FileUtil.deleteFile(tempfile);
//		sleep(3);
//		app.dispatch("service:com.sun.star.wizards.agenda.CallWizard?start");
//		sleep(5);
//		assertTrue(Wizards_AgendaDialog.exists(10));
//		Wizards_AgendaDialog_FinishButton.click();
//		sleep(10);
//		writer.focus();
//		sleep(1);
//		app.dispatch(".uno:SelectAll");
//		typeKeys("<$copy>");
//		// System.out.println("now txt:"+app.getClipboard());
//		// assertTrue(app.getClipboard().startsWith("<Name>"));
//		assertNotNull(app.getClipboard());
//	}

	/**
	 * Test the Tools / Macros / Organize Dialogs" show
	 * 
	 */
	@Test
	public void testRunMacro() {
		open(prepareData("bvt/macro.ods"));
		calc.waitForExistence(10, 2);
		app.dispatch(".uno:RunMacro");
		runMacroDlgCategories.expand("macro.ods");
		runMacroDlgCategories.expand("Standard");
		runMacroDlgCategories.select("Module1");
		runMacroDlgCommands.select(0);
		runMacroDlg.ok();
		assertEquals("A3 should be =1+3", "4", SCTool.getCellText("A3"));
		discard();
	}

	/**
	 * Test the About Dialog show
	 * 
	 */
	@Test
	public void testHelp() {
		app.dispatch(".uno:About");
		assertTrue(aboutDialog.exists(5));
		aboutDialog.ok();
		sleep(1);
		typeKeys("<F1>");
		assertTrue(helpWindow.exists(5));
		helpWindow.close();
	}

	/**
	 * Test inserting a picture in text document
	 * 
	 * @throws Exception
	 */

	@Test
	public void testInsertPictureInDocument() throws Exception {
		String bmp_green = prepareData("image/green_256x256.bmp");
		String bmp_red = prepareData("image/red_256x256.bmp");

		// Create a new text document
		newTextDocument();
		// Insert a picture fully filled with green
		app.dispatch(".uno:InsertGraphic");
		submitOpenDlg(bmp_green);
		writer.click(5,200);
		sleep(1);

		// Verify if the picture is inserted successfully
		Rectangle rectangle = GraphicsUtil.findRectangle(writer.getScreenRectangle(), 0xFF00FF00);
		assertTrue("Green Picture is inserted?" + rectangle, rectangle != null && rectangle.getWidth() > 10);
		// insert another picture
		app.dispatch(".uno:InsertGraphic");
		submitOpenDlg(bmp_red);
		writer.click(5, 200);
		sleep(1);
		// Verify if the picture is inserted successfully
		rectangle = GraphicsUtil.findRectangle(writer.getScreenRectangle(), 0xFFFF0000);
		assertTrue("Green Picture is inserted? " + rectangle, rectangle != null && rectangle.getWidth() > 10);
		discard();
	}

	@Test
	public void testInsertPictureInSpreadsheet() throws Exception {
		String bmp_green = prepareData("image/green_64x64.png");
		String bmp_red = prepareData("image/red_64x64.png");
		newSpreadsheet();
		// Insert a picture fully filled with green
		app.dispatch(".uno:InsertGraphic");
		submitOpenDlg(bmp_green);
		calc.click(5, 150);
		sleep(1);

		// Verify if the picture is inserted successfully
		Rectangle rectangle = GraphicsUtil.findRectangle(calc.getScreenRectangle(), 0xFF00FF00);
		assertTrue("Green Picture is inserted?" + rectangle, rectangle != null && rectangle.getWidth() > 10);
		
		SCTool.selectRange("C1");
		// insert another picture
		app.dispatch(".uno:InsertGraphic");
		submitOpenDlg(bmp_red);
		calc.click(5, 150);
		sleep(1);
		// Verify if the picture is inserted successfully
		rectangle = GraphicsUtil.findRectangle(calc.getScreenRectangle(), 0xFFFF0000);
		assertTrue("Red Picture is inserted? " + rectangle, rectangle != null && rectangle.getWidth() > 10);
		discard();
	}

	@Test
	public void testInsertPictureInPresentation() throws Exception {
		String bmp_green = prepareData("image/green_256x256.bmp");
		String bmp_red = prepareData("image/red_256x256.bmp");
		newPresentation();
		// Insert a picture fully filled with green
		app.dispatch(".uno:InsertGraphic");
		submitOpenDlg(bmp_green);
		impress.click(5, 5);
		sleep(1);

		// Verify if the picture is inserted successfully
		Rectangle rectangle = GraphicsUtil.findRectangle(impress.getScreenRectangle(), 0xFF00FF00);
		assertTrue("Green Picture is inserted?" + rectangle, rectangle != null && rectangle.getWidth() > 10);
		// insert another picture
		app.dispatch(".uno:InsertGraphic");
		submitOpenDlg(bmp_red);
		impress.click(1, 1);
		sleep(1);
		// Verify if the picture is inserted successfully
		rectangle = GraphicsUtil.findRectangle(impress.getScreenRectangle(), 0xFFFF0000);
		assertTrue("Red Picture is inserted? " + rectangle, rectangle != null && rectangle.getWidth() > 10);
		discard();
	}

	@Test
	public void testSlideShow() throws Exception {
		open(prepareData("bvt/slideshow.odp"));
		impress.waitForExistence(10, 2);
		sleep(1);
		impress.typeKeys("<F5>");
		sleep(3);
		Rectangle rectangle = GraphicsUtil.findRectangle(slideShow.getScreenRectangle(), 0xFFFF0000);
		assertNotNull("1st slide appears", rectangle);
		slideShow.click(0.5, 0.5);
		sleep(2);
		rectangle = GraphicsUtil.findRectangle(slideShow.getScreenRectangle(), 0xFF00FF00);
		assertNotNull("2nd slide appears", rectangle);
		typeKeys("<enter>");
		sleep(2);
		rectangle = GraphicsUtil.findRectangle(slideShow.getScreenRectangle(), 0xFF0000FF);
		assertNotNull("3rd slide appears", rectangle);
		slideShow.click(0.5, 0.5);
		sleep(2);
		rectangle = GraphicsUtil.findRectangle(slideShow.getScreenRectangle(), 0xFF0000FF);
		assertNull("The end", rectangle);
		slideShow.click(0.5, 0.5);
		sleep(3);
		assertFalse("Quit", slideShow.exists());
	}

	@Test
	public void testFind() {
		open(prepareData("bvt/find.odt"));
		writer.waitForExistence(10, 2);
		app.dispatch(".uno:SearchDialog");
		findDlgFor.setText("OpenOffice");
		findDlgFind.click();
		sleep(1);
		writer.typeKeys("<$copy>");
		assertEquals("OpenOffice", app.getClipboard());
		findDlgFindAll.click();
		sleep(1);
		writer.typeKeys("<$copy>");
		assertEquals("OpenOfficeOpenOfficeOpenOffice", app.getClipboard());
		findDlgReplaceWith.setText("Awesome OpenOffice");
		findDlgReplaceAll.click();
		sleep(1);
		msgbox("Search key replaced 3 times.").ok();
		findDlg.close();
		sleep(1);
		assertEquals(
				"Apache Awesome OpenOffice is comprised of six personal productivity applications: a word processor (and its web-authoring component), spreadsheet, presentation graphics, drawing, equation editor, and database. Awesome OpenOffice is released on Windows, Solaris, Linux and Macintosh operation systems, with more communities joining, including a mature FreeBSD port. Awesome OpenOffice is localized, supporting over 110 languages worldwide. ",
				copyAll());
		discard();
	}

	@Test
	public void testFillInSpreadsheet() {
		String[][] expected1 = new String[][] { { "1" }, { "1" }, { "1" }, { "1" }, { "1" }, { "1" }, };
		String[][] expected2 = new String[][] { { "2" }, { "2" }, { "2" }, { "2" }, { "2" }, { "2" }, };
		String[][] expected3 = new String[][] { { "Hi friends", "Hi friends", "Hi friends", "Hi friends" } };
		String[][] expected4 = new String[][] { { "99999.999", "99999.999", "99999.999", "99999.999" } };
		String[][] expected5 = new String[][] {
		{ "99999.999", "-10" }, { "100000.999", "-9" }, { "100001.999", "-8" }, { "100002.999", "-7" }, { "100003.999", "-6" }
		};
		newSpreadsheet();
		SCTool.selectRange("C5");
		typeKeys("1<enter>");
		SCTool.selectRange("C5:C10");
		app.dispatch(".uno:FillDown");
		assertArrayEquals("Fill Down:", expected1, SCTool.getCellTexts("C5:C10"));

		SCTool.selectRange("D10");
		typeKeys("2<enter>");
		SCTool.selectRange("D5:D10");
		app.dispatch(".uno:FillUp");
		assertArrayEquals("Fill Up:", expected2, SCTool.getCellTexts("D5:D10"));

		SCTool.selectRange("A1");
		typeKeys("Hi friends<enter>");
		SCTool.selectRange("A1:D1");
		app.dispatch(".uno:FillRight");
		assertArrayEquals("Fill Right:", expected3, SCTool.getCellTexts("A1:D1"));

		SCTool.selectRange("D2");
		typeKeys("99999.999<enter>");
		SCTool.selectRange("A2:D2");
		app.dispatch(".uno:FillLeft");
		assertArrayEquals("Fill left:", expected4, SCTool.getCellTexts("A2:D2"));

		SCTool.selectRange("E1");
		typeKeys("99999.999<tab>-10<enter>");

		SCTool.selectRange("E1:F5");
		app.dispatch(".uno:FillSeries");
		fillSeriesDlg.ok();
		sleep(1);
		assertArrayEquals("Fill series..", expected5, SCTool.getCellTexts("E1:F5"));
		discard();
	}

	@Test
	public void testSort() {
		String[][] expected1 = new String[][] { { "-9999999" }, { "-1.1" }, { "-1.1" }, { "0" }, { "0" }, { "0.1" }, { "10" }, { "12" }, { "9999999" }, { "9999999" },

		};
		String[][] expected2 = new String[][] { { "TRUE", "Oracle" }, { "TRUE", "OpenOffice" }, { "FALSE", "OpenOffice" }, { "TRUE", "IBM" }, { "FALSE", "IBM" },
				{ "TRUE", "Google" }, { "FALSE", "facebook " }, { "TRUE", "Apache" }, { "TRUE", "!yahoo" }, { "TRUE", "" },

		};

		String[][] expected3 = new String[][] { { "Sunday" }, { "Monday" }, { "Tuesday" }, { "Wednesday" }, { "Thursday" }, { "Friday" }, { "Saturday" },

		};

		String[][] expected4 = new String[][] { { "-$10.00" }, { "$0.00" }, { "$0.00" }, { "$1.00" }, { "$3.00" }, { "$9.00" }, { "$123.00" }, { "$200.00" }, { "$400.00" },
				{ "$10,000.00" },

		};
		open(prepareData("bvt/sort.ods"));
		calc.waitForExistence(10, 2);
		SCTool.selectRange("A1:A10");
		app.dispatch(".uno:DataSort");
		sortWarningDlgCurrent.click();
		assertEquals(1, sortPageBy1.getSelIndex());
		sortPage.ok();
		sleep(1);
		assertArrayEquals("Sorted Data", expected1, SCTool.getCellTexts("A1:A10"));
		SCTool.selectRange("B1:C10");
		app.dispatch(".uno:DataSort");

		sortPageBy1.select(2);
		sortPageDescending1.check();
		assertFalse(sortPageBy3.isEnabled());
		assertFalse(sortPageAscending3.isEnabled());
		assertFalse(sortPageDescending3.isEnabled());
		sortPageBy2.select(1);
		assertTrue(sortPageBy3.isEnabled());
		assertTrue(sortPageAscending3.isEnabled());
		assertTrue(sortPageDescending3.isEnabled());
		sortPageDescending2.check();
		sortPageBy2.select(0);
		assertFalse(sortPageBy3.isEnabled());
		assertFalse(sortPageAscending3.isEnabled());
		assertFalse(sortPageDescending3.isEnabled());
		sortPageBy2.select(1);
		sortPage.ok();
		sleep(1);

		assertArrayEquals("Sorted Data", expected2, SCTool.getCellTexts("B1:C10"));
		SCTool.selectRange("D1:D7");
		app.dispatch(".uno:DataSort");
		sortWarningDlgCurrent.click();
		sortOptionsPage.select();
		sortOptionsPageRangeContainsColumnLabels.uncheck();
		sortOptionsPageCustomSortOrder.check();
		sortOptionsPageCustomSortOrderList.select("Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday");
		sortOptionsPage.ok();
		sleep(1);
		assertArrayEquals("Sorted Data", expected3, SCTool.getCellTexts("D1:D7"));

		SCTool.selectRange("E1:E10");
		app.dispatch(".uno:DataSort");
		sortWarningDlgCurrent.click();
		sortPage.ok();
		sleep(1);
		assertArrayEquals("Sorted Data", expected4, SCTool.getCellTexts("E1:E10"));
		discard();
	}

	/**
	 * Test insert a chart in a draw document 1. New a draw document 2. Insert a
	 * chart 3. Check if the chart is inserted successfully
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertChartInDraw() throws Exception {
		// Create a new drawing document
		newDrawing();
		// Insert a chart
		app.dispatch(".uno:InsertObjectChart");
		sleep(3);

		// Verify if the chart is inserted successfully
		assertTrue("Chart Editor appears", chart.exists(3));
		// Focus on edit pane
		draw.click(5, 5);
		sleep(1);
		assertFalse("Chart Editor appears", chart.exists());
		discard();
	}

	/**
	 * Test insert a chart in a text document 1. New a text document 2. Insert a
	 * chart 3. Check if the chart is inserted successfully
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertChartInDocument() throws Exception {
		// Create a new text document
		newTextDocument();
		// Insert a chart
		app.dispatch(".uno:InsertObjectChart");
		sleep(3);

		// Verify if the chart is inserted successfully
		assertTrue("Chart Editor appears", chart.exists(3));
		// Focus on edit pane
		writer.click(5, 5);
		sleep(1);
		assertFalse("Chart Editor appears", chart.exists());
		discard();
	}

	/**
	 * Test insert a chart in a spreadsheet document 1. New a spreadsheet
	 * document 2. Insert a chart 3. Check if the chart is inserted successfully
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertChartInSpreadsheet() throws Exception {
		// Create a new spreadsheet document
		newSpreadsheet();
		// Insert a chart
		app.dispatch(".uno:InsertObjectChart");
		sleep(3);
		chartWizard.ok();

		// Verify if the chart is inserted successfully
		assertTrue("Chart Editor appears", chart.exists(3));
		// Focus on edit pane
		calc.click(5, 5);
		sleep(1);
		assertFalse("Chart Editor appears", chart.exists());
		discard();
	}

	/**
	 * Test insert a chart in a presentation document 1. New a presentation
	 * document 2. Insert a chart 3. Check if the chart is inserted successfully
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertChartInPresentation() throws Exception {
		// Create a new presentation document
		newPresentation();
		// Insert a chart
		app.dispatch(".uno:InsertObjectChart");
		sleep(3);
		// Verify if the chart is inserted successfully
		assertTrue("Chart Editor appears", chart.exists(3));
		// Focus on edit pane
		impress.click(5, 5);
		sleep(1);
		assertFalse("Chart Editor appears", chart.exists());
		discard();
	}

	/**
	 * Test insert a table in a draw document 1. New a draw document 2. Insert a
	 * default table 3. Check if the table is inserted successfully
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertTableInDraw() throws Exception {
		// Create a new drawing document
		newDrawing();
		// Insert a table
		app.dispatch(".uno:InsertTable");
		insertTable.ok();
		sleep(1);
		draw.typeKeys("3");
		assertTrue("Table Toolbar appears", tableToolbar.exists(3));
//		assertEquals("The cell content", "3", copyAll());
		discard();
	}

	/**
	 * Test insert a table in a text document 1. New a text document 2. Insert a
	 * default table 3. Check if the table is inserted successfully
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertTableInDocument() throws Exception {
		// Create a new text document
		newTextDocument();
		// Insert a table
		app.dispatch(".uno:InsertTable");
		writerInsertTable.ok();
		sleep(1);
		writer.typeKeys("3");
		// Verify if the table toolbar is active
		assertTrue("Table Toolbar appears", tableToolbar.exists(3));
//		assertEquals("The cell content", "3", copyAll());
		discard();
	}

	/**
	 * Test insert a table in a presentation document 1. New a presentation
	 * document 2. Insert a default table 3. Check if the table is inserted
	 * successfully
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertTableInPresentation() throws Exception {
		// Create a new presentation document
		newPresentation();

		// Insert a table
		app.dispatch(".uno:InsertTable");
		insertTable.ok();
		sleep(1);
		impress.typeKeys("3");
		assertTrue("Table Toolbar appears", tableToolbar.exists(3));
//		assertEquals("The cell content", "3", copyAll());
		discard();
	}

	/**
	 * Test insert a function in a spreadsheet document via Sum button 1. New a
	 * spreadsheet document 2. Insert a function via Sum button 3. Check if the
	 * result is correct
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSumInFormulaBar() throws Exception {
		// Create a new spreadsheet document
		newSpreadsheet();
		// Insert source numbers
		String sourceNumber1 = "5";
		String sourceNumber2 = "3";
		String expectedResult = "8";
		SCTool.selectRange("A1");
		typeKeys(sourceNumber1);
		SCTool.selectRange("B1");
		typeKeys(sourceNumber2);
		// Insert a function via Sum button
		SCTool.selectRange("C1");
		scInputBarSum.click();
		typeKeys("<enter>");
		// Verify if the calculated result is equal to the expected result
		assertEquals("The calculated result", expectedResult, SCTool.getCellText("C1"));
		discard();
	}

	/**
	 * Test insert a function in a spreadsheet document via inputbar 1. New a
	 * spreadsheet document 2. Insert a function via inputbar: COS 3. Check if
	 * the result is correct
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInsertFunctionViaFormulaBar() throws Exception {
		// Create a new spreadsheet document
		newSpreadsheet();
		// Insert source numbers and expected result
		String sourceData = "0";
		String expectedResult = "1";
		SCTool.selectRange("A1");
		typeKeys(sourceData);

		// Insert a function via inputbar: COS
		SCTool.selectRange("D1");
		scInputBarInput.inputKeys("=COS(A1)");
		typeKeys("<enter>");

		// Verify if the calculated result is equal to the expected result
		assertEquals("The calculated result", expectedResult, SCTool.getCellText("D1"));
		discard();
	}

	/**
	 * Test insert a function in a spreadsheet document via Function Wizard
	 * Dialog 1. New a spreadsheet document 2. Insert a function via Function
	 * Wizard Dialog: ABS 3. Check if the result is correct
	 * 
	 * @throws Exception
	 */
	@Test
	public void testFunctionWizardInFormulaBar() throws Exception {
		// Create a new spreadsheet document
		newSpreadsheet();
		// Insert source number
		String sourceNumber = "-5";
		String expectedResult = "5";
		SCTool.selectRange("A1");
		typeKeys(sourceNumber);
		typeKeys("<enter>");
		// Insert a function via Function Wizard Dialog: ABS
		SCTool.selectRange("B1");
		app.dispatch(".uno:FunctionDialog");
		// SC_FunctionWizardDlg_FunctionList.doubleClick(5, 5);
		scFunctionWizardDlgFunctionList.select("ABS");
		scFunctionWizardDlgNext.click(); // Use "Next" button
		scFunctionWizardDlgEdit1.typeKeys("A1");
		scFunctionWizardDlg.ok();
		// Verify if the calculated result is equal to the expected result
		assertEquals("The calculated result", expectedResult, SCTool.getCellText("B1"));
		discard();
	}

	/**
	 * Test open a non-http(s) type hyperlink (with host only) in a text document.
	 * (coverage included in fvt.gui.sw.hyperlink.WarningDialog
	 * testHyperlinkDisplaysWarning() and included here for build verification)
	 * 1. New a text document
	 * 2. Insert a dav type hyperlink
	 * 3. Open hyperlink
	 * 4. Verify security warning dialog is displayed
	 *
	 * @throws Exception
	 */
	@Test
	public void testNonHttpHyperlinkWithHostOnly() throws Exception {
		// Create a new text document
		newTextDocument();
		writer.waitForExistence(10, 2);
		// open the hyperlink dialog
		writer.typeKeys("<alt i>"); // insert menu
		writer.typeKeys("h"); // hyperlink
		hyperlinkInetPathComboBox.setText("dav://nonexistant.url.com"); //target
		hyperlinkInetText.setText("dav://nonexistant.url.com"); // displayed text
		hyperlinkDialogOkBtn.click(); // apply
		hyperlinkDialogCancelBtn.click(); // close
		sleep(1); // give the dialog time to close
		typeKeys("<shift F10>"); // context menu
		typeKeys("o"); // open hyperlink
		// we can't be sure of the language so just check for the dialog
		boolean msgExists = activeMsgBox.exists(1); // wait 1 second for the dialog
		if (msgExists) {
			activeMsgBox.no(); // close dialog
		}
		assertTrue("security warning not displayed", msgExists);
		discard();
	}

}