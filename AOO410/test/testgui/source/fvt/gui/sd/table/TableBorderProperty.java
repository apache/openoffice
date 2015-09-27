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
import static org.openoffice.test.vcl.Tester.*;
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.Logger;

import testlib.gui.AppTool;

public class TableBorderProperty {
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
	 * Test the table border property in Presentation
	 * 
	 * @throws Exception
	 */

	@Test
	public void testTableBorderPropertyDialog() throws Exception {

		// Create a new presentation document
		AppTool.newPresentation();

		// Insert a table
		app.dispatch(".uno:InsertTable", 3);
		insertTable.ok();
		sleep(3);

		// Verify if the table toolbar is active
		assertTrue(tableToolbar.exists(3));

		// open Table Properties Dialog
		app.dispatch(".uno:TableDialog", 3);
		areaBorderpage.select();

		areaBorderPresets.click(0.01, 0.01);

		// select Line
		areaBorderLineStyle.select(1);
		assertEquals("Area_Border_LineStyle is not the first item", areaBorderLineStyle.getSelText(), areaBorderLineStyle.getItemText(1));

		areaBorderLineColor.select(1);
		assertEquals("Area_Border_LineColor is not the first item", areaBorderLineColor.getSelText(), areaBorderLineColor.getItemText(1));

		// select Spacing to Contents
		areaBorderSync.check();
		assertTrue("Area_Border_Sync isn't checked", areaBorderSync.isChecked());

		areaBorderMfLeft.setText("0.5");
		assertTrue("Area_Border_Mf_Left isn't 0.5", areaBorderMfLeft.getText().startsWith("0.5"));

		areaBorderMfRight.setText("0.5");
		assertTrue("Area_Border_Mf_Right isn't 0.5", areaBorderMfRight.getText().startsWith("0.5"));

		areaBorderMfTop.setText("0.5");
		assertTrue("Area_Border_Mf_Top isn't 0.5", areaBorderMfTop.getText().startsWith("0.5"));

		areaBorderMfBottom.setText("0.5");
		assertTrue("Area_Border_Mf_Bottom isn't 0.5", areaBorderMfBottom.getText().startsWith("0.5"));

		areaBorderpage.ok();

	}
}
