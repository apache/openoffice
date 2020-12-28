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
package fvt.gui.formula.importexport;

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

import testlib.gui.AppTool;


public class FormulaInDifferentWays {
	
	@Rule
	public Logger log = Logger.getLogger(this);

	@Before
	public void setUp() throws Exception {
		app.start(true);
		newFormula();
	}

	@After
	public void tearDown() throws Exception {
		discard();
		app.stop();
	}

	/**
	 * Test elements window active and inactive
	 * 
	 * @throws Exception
	 */
	@Test
	public void testElementsWindowActive() throws Exception {
		// Check if the "View->Elements" menu is selected
		boolean viewElements = mathElementsWindow.exists();
		// Active or inactive the Elements window
		app.dispatch(".uno:ToolBox");
		assertNotSame("Elements window active/inactive failed", viewElements, mathElementsWindow.exists());
	}

	/**
	 * Test create a formula from Elements window
	 * 
	 * @throws Exception
	 */
	@Test
	public void testCreateFormulaFromElementsWindow() throws Exception {
		String saveTo = getPath("temp/" + "FormulaFromElements.odf");

		// Make Elements window pop up
		if (!mathElementsWindow.exists()) {
			app.dispatch(".uno:ToolBox");
		}
		// Click a formula in Elements window and edit the formula in the
		// commands window
		mathElementsRelations.click();
		mathElementsRelationsNotEqual.click();
		sleep(1);
		typeKeys("a");
		app.dispatch(".uno:NextMark");
		typeKeys("b");
		String insertedFormula = "a <> b";

		// Verify if the formula is correct
		app.dispatch(".uno:Select");
		app.dispatch(".uno:Copy");
		assertEquals("The inserted formula into math", insertedFormula.concat(" "), app.getClipboard()); 
		
		// Save and reopen the formula
		FileUtil.deleteFile(saveTo);
		saveAndReopen(saveTo);
		mathEditWindow.waitForExistence(10, 2);

		// Verify if the formula still exists in the file, and correct
		app.dispatch(".uno:Select");
		app.dispatch(".uno:Copy");
		assertEquals("The inserted formula into math", insertedFormula.concat(" "), app.getClipboard());
	
	}

	/**
	 * Test create a formula from right click menu in equation editor
	 * 
	 * @throws Exception
	 */
	@Test
	public void testCreateFormulaFromRightClickMenu() throws Exception {
		String saveTo = getPath("temp/" + "FormulaFromRightClickMenu.odf");

		// Right click in equation editor, choose "Functions->More->arcsin(x)",
		mathEditWindow.rightClick(5, 5);
		typeKeys("<down>");
		typeKeys("<down>");
		typeKeys("<down>");
		typeKeys("<down>");
		typeKeys("<enter>");
		typeKeys("<up>");
		typeKeys("<enter>");
		typeKeys("<enter>");
		typeKeys("a");
		sleep(2); 
		String insertedFormula = "arcsin(a)";

		// Verify if the formula is correct
		app.dispatch(".uno:Select");
		app.dispatch(".uno:Copy");

		assertEquals("The inserted formula into math", insertedFormula.concat(" "), app.getClipboard()); 
		// Save and reopen the formula
		FileUtil.deleteFile(saveTo);
		saveAndReopen(saveTo);
		
		mathEditWindow.waitForExistence(10, 2);
		// Verify if the formula still exists in the file, and correct
		app.dispatch(".uno:Select");
		app.dispatch(".uno:Copy");
		assertEquals("The inserted formula into math", insertedFormula.concat(" "), app.getClipboard());																								
	}

	/**
	 * Test undo/redo in math
	 * 
	 * @throws Exception
	 */
	@Test
	public void testUndoRedoInMath() throws Exception {

		// Make Elements window pop up
		if (!mathElementsWindow.exists()) {
			app.dispatch(".uno:ToolBox");
		}

		// Click a formula in Elements window and edit the formula in the
		// commands window
		mathElementsUnaryBinary.click();
		mathElementsUnaryBinaryPlus.click();
		sleep(1);
		typeKeys("a"); // "+a";

		// Undo and verify if it works fine
		app.dispatch(".uno:Undo");
		app.dispatch(".uno:Select");
		app.dispatch(".uno:Copy");
		assertEquals("The inserted formula into math", "+<?> ", app.getClipboard()); 																					

		// Redo and verify if it works fine
		app.dispatch(".uno:Redo");
		app.dispatch(".uno:Select");
		app.dispatch(".uno:Copy");
		assertEquals("The inserted formula into math", "+a ", app.getClipboard()); 
	}
}
