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

public class TableBackgroundProperty {
	@Rule
	public Logger log = Logger.getLogger(this);

	@Before
	public void setUp() throws Exception {
		app.start();
	}

	@After
	public void tearDown() throws Exception {
		app.stop();
	}

	/**
	 * Test the Table Background property dialog in Presentation
	 * 
	 * @throws Exception
	 */

	@Test
	public void testTableBackgroundPropertyDialog() throws Exception {
		AppTool.newPresentation();

		// Insert a table
		app.dispatch(".uno:InsertTable", 3);
		insertTable.ok();

		// Verify if the table toolbar is active
		assertTrue(tableToolbar.exists(3));

		// open Table Properties Dialog
		app.dispatch(".uno:TableDialog", 3);
		areaAreapage.select();
		// select Color
		areaFillType.select(1);
		assertEquals("Area_Fill_Type is not Color", areaFillType.getSelText(), areaFillType.getItemText(1));
		areaFillColor.select(2);
		// Area_Fill_Color.getItemCount();
		assertEquals("Area_Fill_Color is not the second item", areaFillColor.getSelText(), areaFillColor.getItemText(2));
		areaAreapage.ok();

		// open Table Properties Dialog
		app.dispatch(".uno:TableDialog", 3);
		areaAreapage.select();
		// select Gradient
		areaFillType.select(2);
		assertEquals("Area_Fill_Type is not Gradient", areaFillType.getSelText(), areaFillType.getItemText(2));
		areaFillGradient.select(2);
		assertEquals("Area_Fill_Gradient is not the second item", areaFillGradient.getSelText(), areaFillGradient.getItemText(2));
		areaAreapage.ok();

		// open Table Properties Dialog
		app.dispatch(".uno:TableDialog", 3);
		areaAreapage.select();
		// select Hatching
		areaFillType.select(3);
		assertEquals("Area_Fill_Type is not Hatching", areaFillType.getSelText(), areaFillType.getItemText(3));
		areaFillHatching.select(2);
		assertEquals("Area_Fill_Hatching is not the second item", areaFillHatching.getSelText(), areaFillHatching.getItemText(2));
		areaAreapage.ok();

		// open Table Properties Dialog
		app.dispatch(".uno:TableDialog", 3);
		areaAreapage.select();
		// select Bitmap
		areaFillType.select(4);
		assertEquals("Area_Fill_Type is not Bitmap", areaFillType.getSelText(), areaFillType.getItemText(4));
		areaFillBitmap.select(2);
		assertEquals("Area_Fill_Bitmap is not the second item", areaFillBitmap.getSelText(), areaFillBitmap.getItemText(2));
		areaAreapage.ok();

	}

}
