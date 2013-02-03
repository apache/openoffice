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
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.Logger;

import testlib.gui.AppTool;

public class ValidityDialogDefaultValue {

	@Rule
	public Logger log = Logger.getLogger(this);
	
	@Before
	public void setUp() throws Exception {
		app.start(true);
		AppTool.newSpreadsheet();
		app.dispatch(".uno:Validation");
	}

	@After
	public void tearDown() throws Exception {
		app.stop();
	}

	/**
	 * test Data -> Validity - UI(Criteria: Cell range).
	 */
	@Test
	public void testValidityUICellRange() {

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(5); 

		assertEquals("Wrong default value of AllowBlankCells in Validity",true, scValidityAllowBlankCells.isChecked());
		assertEquals("Wrong default value of ShowSelectionList in Validity",true, scValidityShowSelectionList.isChecked());
		assertEquals("Wrong default value of Sort Entries Ascending in Validity",false, scValiditySortEntriesAscending.isChecked());
		assertEquals("Wrong default value of AllowBlankCells in Validity",true, scValidityAllowBlankCells.isEnabled());
		assertEquals("Wrong default value of Sort Entries Ascending in Validity",true, scValiditySortEntriesAscending.isEnabled());
		assertEquals("Wrong default value of SourcePicker in Validity",true, scValiditySourcePicker.isEnabled());
	}

	/**
	 * test Data -> Validity - UI(Error Alert: Default status)
	 */
	@Test
	public void testValidityUIErrorAlert() {

		scValidityErrorAlertTabPage.select();
		assertEquals("Error Alert Action list is disable",true, scValidityErrorAlertActionList.isEnabled());
		// assertEquals("Stop",SC_ValidityErrorAlertActionList.getSelText()); 
		// Do not test this. Do it in GVT.
		assertEquals("Error Message Title is disable",true, scValidityErrorMessageTitle.isEnabled());
		assertEquals("Have default error message title","", scValidityErrorMessageTitle.getText());
		assertEquals("Error message is disable",true, scValidityErrorMessage.isEnabled());
		assertEquals("Have default error message","", scValidityErrorMessage.getText());
		scValidityErrorAlertActionList.select(3); // "Macro"
		assertEquals("error browse button is disable",true, scValidityErrorBrowseButton.isEnabled());
	}

}
