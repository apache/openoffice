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
package fvt.gui.formula.catalog;

import static org.junit.Assert.*;
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.Logger;

import testlib.gui.AppTool;

public class CharacterFromCatalogDlg {
	
	@Rule
	public Logger log = Logger.getLogger(this);

	@Before
	public void setUp() throws Exception {
		app.start(true);
		AppTool.newFormula();
		// Click catalog button
		mathCatalogButton.click();
	}

	@After
	public void tearDown() throws Exception {
		AppTool.close();
		app.stop();
	}

	/**
	 * Test input Greek character from Catalog->Symbols
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInputGreekFromCatalog() throws Exception {

		// Select "Greek", click "Edit" button to get the selected Symbol
		mathSymbolsDlgListbox.select(0);
		mathSymbolsDlgEditButton.click();
		String selectedSymbol = mathEditSymbolsDlgSymbol.getText();
		mathEditSymbolsDlg.ok();

		// Insert the selected symbol
		mathSymbolsDlgInsertButton.click();
		mathSymbolsDlgCloseButton.click();

		// Verify if the symbol is inserted successfully
		mathEditWindow.click(5, 5);
		app.dispatch(".uno:Select");
		app.dispatch(".uno:Copy");
		assertEquals("Symbol is not inserted succcessfully",
				"%".concat(selectedSymbol).concat(" "), app.getClipboard());
		// add "%" in the font, add " " in the end

	}

	/**
	 * Test input iGreek character from Catalog->Symbols
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInputIGreekFromCatalog() throws Exception {

		// Select "iGreek", click "Edit" button to get the selected Symbol
		mathSymbolsDlgListbox.select(1);
		mathSymbolsDlgEditButton.click();
		String selectedSymbol = mathEditSymbolsDlgSymbol.getText();
		mathEditSymbolsDlg.ok();

		// Insert the selected symbol
		mathSymbolsDlgInsertButton.click();
		mathSymbolsDlgCloseButton.click();

		// Verify if the symbol is inserted successfully
		mathEditWindow.click(5, 5);
		app.dispatch(".uno:Select");
		app.dispatch(".uno:Copy");
		assertEquals("Symbol is not inserted succcessfully",
				"%".concat(selectedSymbol).concat(" "), app.getClipboard());
		// add "%" in the font, add " " in the end
	}

	/**
	 * Test input Special character from Catalog->Symbols
	 * 
	 * @throws Exception
	 */
	@Test
	public void testInputSpecialFromCatalog() throws Exception {

		// Select "Special", "Insert" the default first symbol
		mathSymbolsDlgListbox.select(2);
		mathSymbolsDlgInsertButton.click();

		// Click "Edit" button to get the selected Symbol
		mathSymbolsDlgEditButton.click();
		String selectedSymbol = mathEditSymbolsDlgSymbol.getText();
		mathEditSymbolsDlg.ok();
		mathSymbolsDlgCloseButton.click();

		// Verify if the symbol is inserted successfully
		mathEditWindow.click(5, 5);
		app.dispatch(".uno:Select");
		app.dispatch(".uno:Copy");
		assertEquals("Symbol is not inserted succcessfully",
				"%".concat(selectedSymbol).concat(" "), app.getClipboard());
		// add "%" in the font, add " " in the end
	}
}
