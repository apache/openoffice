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

/**
 *
 */
public class AddDeleteSymbolsFromCatalog {

	@Rule
	public Logger log = Logger.getLogger(this);

	@Before
	public void setUp() throws Exception {
		app.start(true);

		// New a formula document
		app.dispatch("private:factory/smath");
		mathEditWindow.waitForExistence(10, 2);

		// Click catalog button
		mathCatalogButton.click();

		// Select "Special", click "Edit" button
		mathSymbolsDlgListbox.select(2);
		mathSymbolsDlgEditButton.click();
	}

	@After
	public void tearDown() throws Exception {

	}

	/**
	 * Test add custom symbols from Catalog->Symbols
	 * 
	 * @throws Exception
	 */
	@Test
	public void testAddSymbolFromCatalog() throws Exception {

		// Choose a symbol which is not in the list, click "Add" and "OK"
		String selectedSymbol;
		boolean bSelectSymbolNotInList;
		int nListCount;
		int nIndex = 1;
		do {
			mathEditSymbolsDlgViewControl.click(100 * nIndex, 10); // risk:
																	// after 6
																	// or 7
																	// circles,
																	// this will
																	// click out
																	// of
																	// ViewControl
			selectedSymbol = mathEditSymbolsDlgSymbol.getText();
			nIndex++;
			// Find if the selected symbol is already in the list
			bSelectSymbolNotInList = false;
			nListCount = mathEditSymbolsDlgSymbol.getItemCount();
			for (int i = 0; i < nListCount; i++) {
				if (selectedSymbol.equals(mathEditSymbolsDlgSymbol.getItemText(i))) {
					bSelectSymbolNotInList = true;
					break;
				}
			}
		} while (bSelectSymbolNotInList);
		mathEditSymbolsDlgAdd.click();
		mathEditSymbolsDlg.ok();

		// Verify if the symbol is added to Symbol set
		mathSymbolsDlgEditButton.click();
		bSelectSymbolNotInList = false;
		nListCount = mathEditSymbolsDlgSymbol.getItemCount();
		for (int i = 0; i < nListCount; i++) {
			if (selectedSymbol.equals(mathEditSymbolsDlgSymbol.getItemText(i))) {
				bSelectSymbolNotInList = true;
				break;
			}
		}
		assertTrue("Symbol is not added to Symbol set", bSelectSymbolNotInList);

		// Close all dialogs
		mathEditSymbolsDlg.cancel();
		mathSymbolsDlg.close();
		app.dispatch(".uno:CloseDoc");
	}

	/**
	 * Test modify font of custom symbols from Catalog->Symbols
	 * 
	 * @throws Exception
	 */
	@Test
	public void testModifySymbolFontFromCatalog() throws Exception {

		// Modify the font of selected symbol
		// String selectedSymbol = math_EditSymbolsDlgSymbol.getText();
		int oldSymbolFontIndex = mathEditSymbolsDlgFont.getSelIndex();
		int modifiedSymbolFondIndex = (oldSymbolFontIndex + 1 == mathEditSymbolsDlgFont.getItemCount()) ? 0 : (oldSymbolFontIndex + 1);
		mathEditSymbolsDlgFont.select(modifiedSymbolFondIndex); // select the
																	// next font
																	// of old
																	// font
		String modifiedSymbolFont = mathEditSymbolsDlgFont.getSelText();
		String selectedSymbol = mathEditSymbolsDlgSymbol.getText();
		mathEditSymbolsDlgModify.click();
		mathEditSymbolsDlg.ok();

		// Verify if the font of symbol is modified successfully
		mathSymbolsDlgEditButton.click();
		mathEditSymbolsDlgSymbol.select(selectedSymbol);
		assertEquals("Font of symbol is not modified successfully", modifiedSymbolFont, mathEditSymbolsDlgFont.getSelText());

		// Close all dialogs
		mathEditSymbolsDlg.cancel();
		mathSymbolsDlg.close();
		app.dispatch(".uno:CloseDoc");
	}

	/**
	 * Test modify typeface of custom symbols from Catalog->Symbols
	 * 
	 * @throws Exception
	 */
	@Test
	public void testModifySymbolTypefaceFromCatalog() throws Exception {

		// Modify the typeface of selected symbol
		// String selectedSymbol = math_EditSymbolsDlgSymbol.getText();
		int oldSymbolTypefaceIndex = mathEditSymbolsDlgTypeface.getSelIndex();
		int modifiedSymbolTypefaceIndex = (oldSymbolTypefaceIndex + 1 == mathEditSymbolsDlgTypeface.getItemCount()) ? 0 : (oldSymbolTypefaceIndex + 1);
		mathEditSymbolsDlgTypeface.select(modifiedSymbolTypefaceIndex); // select
																			// the
																			// next
																			// typeface
																			// of
																			// old
																			// typeface
		String modifiedSymbolTypeface = mathEditSymbolsDlgTypeface.getSelText();
		String selectedSymbol = mathEditSymbolsDlgSymbol.getText();
		mathEditSymbolsDlgModify.click();
		mathEditSymbolsDlg.ok();

		// Verify if the typeface of symbol is modified successfully
		mathSymbolsDlgEditButton.click();
		mathEditSymbolsDlgSymbol.select(selectedSymbol);
		assertEquals("Typeface of symbol is not modified successfully", modifiedSymbolTypeface, mathEditSymbolsDlgTypeface.getSelText());

		// Close all dialogs
		mathEditSymbolsDlg.cancel();
		mathSymbolsDlg.close();
		app.dispatch(".uno:CloseDoc");
	}

	/**
	 * Test delete custom symbols from Catalog->Symbols
	 * 
	 * @throws Exception
	 */
	@Test
	public void testDeleteSymbolFromCatalog() throws Exception {

		// Delete the selected symbol
		String selectedSymbol = mathEditSymbolsDlgSymbol.getText();
		mathEditSymbolsDlgDelete.click();
		mathEditSymbolsDlg.ok();

		// Verify if the selected symbol is deleted successfully
		mathSymbolsDlgEditButton.click();
		boolean isDeleted = true;
		for (int i = 0; i < mathEditSymbolsDlgSymbol.getItemCount(); i++) {
			if (selectedSymbol.equals(mathEditSymbolsDlgSymbol.getItemText(i))) {
				isDeleted = false;
				break;
			}
		}
		assertTrue("Symbol is not deleted successfully", isDeleted);

		// Close all dialogs
		mathEditSymbolsDlg.cancel();
		mathSymbolsDlg.close();
		app.dispatch(".uno:CloseDoc");
	}
}
