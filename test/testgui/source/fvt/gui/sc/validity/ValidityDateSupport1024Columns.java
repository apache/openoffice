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

package fvt.gui.sc.validity;

import static org.junit.Assert.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.Logger;

import testlib.gui.AppTool;
import testlib.gui.SCTool;

public class ValidityDateSupport1024Columns {


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
	 * test Allow Greater than or equal to Date type in Validity, support 1024
	 * columns.
	 */
	@Test
	public void testValidityDateSupport1024Columns() {
		SCTool.selectRange("ALM1000:ALO1005");
		app.dispatch(".uno:Validation");

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(3); // "Date"
		scValidityDecimalCompareOperator.select(2); // "greater than"
		scValiditySourceInput.setText("01/01/08");
		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.check();
		scValidityErrorMessageTitle.setText("Stop to enter");
		scValidityErrorMessage.setText("Invalid value");
		scValidityErrorAlertTabPage.ok();

		SCTool.selectRange("ALM1001");
		scInputBarInput.activate();
		typeKeys("02/01/08<enter>");
		assertEquals("Cell text in ALM1001 is not 02/01/08","02/01/08", SCTool.getCellText("ALM1001"));

		SCTool.selectRange("ALM1002");
		scInputBarInput.activate();
		typeKeys("01/02/08<enter>");
		assertEquals("Cell text in ALM1002 is not 01/02/08","01/02/08", SCTool.getCellText("ALM1002"));

		SCTool.selectRange("ALM1003");
		scInputBarInput.activate();
		typeKeys("01/01/08<enter>");
		assertEquals("Have no alert message","Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("Validity have not filter invalid value","", SCTool.getCellText("ALM1003"));

		SCTool.selectRange("AML1003");
		scInputBarInput.activate();
		typeKeys("12/31/07<enter>");
		assertEquals("Have no alert message","Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("Validity have not filter invalid value","", SCTool.getCellText("AML1003"));
	}
}
