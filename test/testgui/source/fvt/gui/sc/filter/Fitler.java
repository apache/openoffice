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
package fvt.gui.sc.filter;

import static org.junit.Assert.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.Logger;

import testlib.gui.AppTool;
import testlib.gui.SCTool;

/**
 * Test cases about Data->Filter in spreadsheet
 */
public class Fitler {

	@Before
	public void setUp() throws Exception {
		app.start(true);
		AppTool.newSpreadsheet();
	}

	@After
	public void tearDown() throws Exception {
		app.stop();

	}

	/**
	 * 
	 * Verify 2+ won't be treated as 2
	 */
	@Ignore("Bug #120076 2+ and 2 will be filter out at the same time")
	@Test
	public void testAutoFilterWithPlusSign() {
		String expect = "2+";
		SCTool.selectRange("A1");
		typeKeys(expect + "<enter>");
		assertEquals("expect 2+ not display correctly",expect, SCTool.getCellText("A1"));
	}
}
