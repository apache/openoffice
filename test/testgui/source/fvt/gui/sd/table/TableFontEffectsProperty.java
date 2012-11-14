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

package fvt.gui.sd.table;

import static org.junit.Assert.*;
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.Logger;

import testlib.gui.AppTool;

public class TableFontEffectsProperty {

	@Before
	public void setUp() throws Exception {
		app.start();
	}

	@After
	public void tearDown() throws Exception {
		app.stop();
	}

	/**
	 * Test the Font Effect property dialog in Presentation
	 * 
	 * @throws Exception
	 */

	@Test
	public void testTableFontEffectPropertyDialog() throws Exception {

		// Create a new presentation document
		AppTool.newPresentation();

		// Insert a table
		app.dispatch(".uno:InsertTable", 3);
		insertTable.ok();

		// Verify if the table toolbar is active
		assertTrue("Table Toolbar exist", tableToolbar.exists(3));

		// open Table Properties Dialog
		app.dispatch(".uno:TableDialog", 3);
		effectsPage.select();
		// select Font color
		effectsPageColor.select(1);
		assertEquals("EffectsPage_Color isn't black", effectsPageColor.getSelText(), "Black");

		// select Relief
		effectsPageRelief.select(0);
		assertEquals("EffectsPage_Relief isn't no", effectsPageRelief.getSelText(), "(Without)");
		effectsPageOutline.check();
		assertTrue("EffectsPage_Outline isn't checked", effectsPageOutline.isChecked());
		effectsPageShadow.check();
		assertTrue("EffectsPage_Shadow isn't checked", effectsPageShadow.isChecked());

		// select Overlining
		effectsPageOverline.select(1);
		assertEquals("EffectsPage_Overline isn't the first item", effectsPageOverline.getSelText(), effectsPageOverline.getItemText(1));
		effectsPageOverlineColor.select(1);
		assertEquals("EffectsPage_Overline_Color isn't the first item", effectsPageOverlineColor.getSelText(), effectsPageOverlineColor.getItemText(1));

		// select Strikethrough
		effectsPageStrikeout.select(1);
		assertEquals("EffectsPage_Strikeout isn't the first item", effectsPageStrikeout.getSelText(), effectsPageStrikeout.getItemText(1));

		// select Underlining
		effectsPageUnderline.select(1);
		assertEquals("EffectsPage_Underline isn't the first item", effectsPageUnderline.getSelText(), effectsPageUnderline.getItemText(1));
		effectsPageUnderlineColor.select(1);
		assertEquals("EffectsPage_Underline_Color isn't the first item", effectsPageUnderlineColor.getSelText(), effectsPageUnderlineColor.getItemText(1));

		// select individual workds
		effectsPageInvidiualWords.check();
		assertTrue("EffectsPage_InvidiualWords isn't checked", effectsPageInvidiualWords.isChecked());

		// select Emphasis Mark
		// EffectsPage_Emphasis.select(1);
		// assertEquals(EffectsPage_Emphasis.getSelText(), "Dot");
		// EffectsPage_Position.select(1);
		// assertEquals(EffectsPage_Position.getSelText(), "Below text");
		effectsPage.ok();

	}
}
