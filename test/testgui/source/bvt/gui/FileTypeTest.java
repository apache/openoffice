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

import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.GraphicsUtil;
import org.openoffice.test.common.Logger;

import testlib.gui.SCTool;

/**
 * 
 */
public class FileTypeTest {

	@Rule
	public Logger log = Logger.getLogger(this);

	@BeforeClass
	public static void beforeClass() throws Exception {
		app.clean();
	}
	
	@AfterClass
	public static void afterClass() throws Exception {
		app.close();
	}
	
	@Before
	public void before() {
		app.close();
		app.start();
	}
	
	
	/**
	 * Test New/Save a text document
	 * 
	 * @throws Exception
	 */
	@Test
	public void testSaveNewODT() throws Exception {
		saveNewDocument("helloworld_saveas.odt");
	}

	@Test
	public void testSaveNewOTT() throws Exception {
		saveNewDocument("helloworld_saveas.ott");
	}

	@Test
	public void testSaveNewSXW() throws Exception {
		saveNewDocument("helloworld_saveas.sxw");
	}

	@Test
	public void testSaveNewSTW() throws Exception {
		saveNewDocument("helloworld_saveas.stw");
	}

	@Test
	public void testSaveNewDOC() throws Exception {
		saveNewDocument("helloworld_saveas.doc");
	}

	@Test
	public void testSaveNewTXT() throws Exception {
		saveNewDocument("helloworld_saveas.txt");
	}

	private void saveNewDocument(String file) {
		String saveTo = "temp/" + file;
		String text = "@AOO";
		newTextDocument();
		writer.typeKeys(text);
		// Verify the text via system clip board
		Assert.assertEquals("The typed text into writer", text, copyAll());

		// menuItem("Text Properties...").select();
		app.dispatch(".uno:FontDialog");
		effectsPage.select();
		effectsPageColor.select(6);
		effectsPage.ok();
		sleep(1);

		// Save the text document
		deleteFile(saveTo);
		saveAs(saveTo);
		close();
		open(saveTo);
		// Reopen the saved file
		writer.waitForExistence(10, 2);
		sleep(1);
		// Verify if the text still exists in the file
		Assert.assertEquals("The typed text into writer is saved!", text, copyAll());
	}

	@Test
	public void testSaveNewODS() throws Exception {
		saveNewSpreadsheet("helloworld_saveas.ods");
	}

	@Test
	public void testSaveNewOTS() throws Exception {
		saveNewSpreadsheet("helloworld_saveas.ots");
	}

	@Test
	public void testSaveNewSXC() throws Exception {
		saveNewSpreadsheet("helloworld_saveas.sxc");
	}

	@Test
	public void testSaveNewSTC() throws Exception {
		saveNewSpreadsheet("helloworld_saveas.stc");
	}

	// @Test
	// public void testSaveNewCSV() throws Exception {
	// saveNewSpreadsheet("helloworld_saveas.csv");
	// }

	@Test
	public void testSaveNewXLS() throws Exception {
		saveNewSpreadsheet("helloworld_saveas.xls");
	}

	private void saveNewSpreadsheet(String file) {
		String saveTo = "temp/" + file;
		String text = "@AOO";
		newSpreadsheet();
		SCTool.selectRange("A65536");
		calc.typeKeys(text);
		deleteFile(saveTo);
		saveAs(saveTo);
		close();
		open(saveTo);
		calc.waitForExistence(10, 2);
		sleep(1);
		Assert.assertEquals("The typed text is saved!", text, SCTool.getCellText("A65536"));
	}

	@Test
	public void testSaveNewODP() throws Exception {
		saveNewPresentation("helloworld_saveas.odp");
	}

	@Test
	public void testSaveNewOTP() throws Exception {
		saveNewPresentation("helloworld_saveas.otp");
	}

	@Test
	public void testSaveNewPPT() throws Exception {
		saveNewPresentation("helloworld_saveas.ppt");
	}

	@Test
	public void testSaveNewPOT() throws Exception {
		saveNewPresentation("helloworld_saveas.pot");
	}

	@Test
	public void testSaveNewSXI() throws Exception {
		saveNewPresentation("helloworld_saveas.sxi");
	}

	@Test
	public void testSaveNewSTI() throws Exception {
		saveNewPresentation("helloworld_saveas.sti");
	}

	private void saveNewPresentation(String file) {
		String saveTo = "temp/" + file;
		String text = "@AOO";
		newPresentation();
		impress.typeKeys(text);
		impress.doubleClick(0.1, 0.5);
		deleteFile(saveTo);
		saveAs(saveTo);
		close();
		open(saveTo);
		impress.waitForExistence(10, 2);
		sleep(1);
		impress.typeKeys("<tab><enter>");
		Assert.assertEquals("The typed text is saved!", text, copyAll().trim());
	}

	// drawing

	/**
	 * Test save a new drawing as .odg
	 */
	@Test
	public void testSaveNewODG() throws Exception {
		saveNewDrawing("draw_saveas.odg");
	}

	/**
	 * Test save a new drawing as .otg
	 */
	@Test
	public void testSaveNewOTG() throws Exception {
		saveNewDrawing("draw_saveas.otg");
	}

	/**
	 * Test save a new drawing as .sxd
	 */
	@Test
	public void testSaveNewSXD() throws Exception {
		saveNewDrawing("draw_saveas.sxd");
	}

	/**
	 * Test save a new drawing as .std
	 */
	@Test
	public void testSaveNewSTD() throws Exception {
		saveNewDrawing("draw_saveas.std");
	}

	/**
	 * New/Save a draw document 1. New a draw document 2. Insert a picture 3.
	 * Save it as the input filename 4. Reopen the saved file 5. Check if the
	 * picture is still there
	 * 
	 * @param filename
	 *            : filename to be saved
	 * @throws Exception
	 */
	public void saveNewDrawing(String filename) {
		String saveTo = "temp/" + filename;
		String bmp_green = prepareData("image/green_256x256.bmp");
		// Create a new drawing document
		newDrawing();
		// Insert a picture fully filled with green
		app.dispatch(".uno:InsertGraphic");
		submitOpenDlg(bmp_green);
		// Focus on edit pane
		draw.click(5, 5);
		sleep(1);
		// Verify if the picture is inserted successfully
		Rectangle rectangle = GraphicsUtil.findRectangle(draw.getScreenRectangle(), 0xFF00FF00);
		assertNotNull("Green rectangle: " + rectangle, rectangle);
		deleteFile(saveTo);
		saveAs(saveTo);
		close();
		open(saveTo);
		draw.waitForExistence(10, 2);
		sleep(1);
		// Verify if the picture still exists in the file
		Rectangle rectangle1 = GraphicsUtil.findRectangle(draw.getScreenRectangle(), 0xFF00FF00);
		assertNotNull("Green rectangle: " + rectangle1, rectangle1);
	}

	// math
	/**
	 * Test save a new math as .odf
	 */
	@Test
	public void testSaveNewODF() throws Exception {
		saveNewMath("math_saveas.odf");
	}

	/**
	 * Test save a new math as .sxm
	 */
	@Test
	public void testSaveNewSXM() throws Exception {
		saveNewMath("math_saveas.sxm");
	}

	/**
	 * Test save a new math as .mml
	 */
	@Test
	public void testSaveNewMML() throws Exception {
		saveNewMath("math_saveas.mml");
	}

	/**
	 * New/Save a math 1. New a math 2. Insert a formula 3. Save it as the input
	 * filename 4. Reopen the saved file 5. Check if the formula is still there
	 * 
	 * @param filename
	 *            : filename to be saved
	 * @throws Exception
	 */
	public void saveNewMath(String filename) {
		String saveTo = "temp/" + filename;
		String text = "5 times 3 = 15";
		// Create a new math
		newFormula();
		// Insert a formula
		mathEditWindow.typeKeys(text);
		// Verify the text via system clip board
		assertEquals("The typed formula into math", text, copyAll());

		// Save the formula
		deleteFile(saveTo);
		saveAs(saveTo);
		close();
		open(saveTo);
		mathEditWindow.waitForExistence(10, 2);
		sleep(1);
		mathEditWindow.focus();
		assertEquals("The typed formula into math is saved", text, copyAll());
	}
}
