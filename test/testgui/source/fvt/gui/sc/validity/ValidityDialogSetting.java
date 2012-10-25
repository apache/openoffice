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
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.Logger;

import testlib.gui.SCTool;

public class ValidityDialogSetting {

	@Rule
	public Logger log = Logger.getLogger(this);

	@Before
	public void setUp() throws Exception {
		app.start(true);

		// New a spreadsheet, select cell range, open Validity dialog
		app.dispatch("private:factory/scalc");
		calc.waitForExistence(10, 2);
		SCTool.selectRange("A1:C5");
		app.dispatch(".uno:Validation");
	}

	@After
	public void tearDown() throws Exception {
		app.stop();
	}

	/**
	 * test Allow not between Date type in Validity.
	 */
	@Test
	public void testAllowDateNotBetween() {
		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(3); // "Date"
		scValidityDecimalCompareOperator.select(7); // "not between"
		scValiditySourceInput.setText("01/01/08");
		scValidityMaxValueInput.setText("03/01/08");
		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.check();
		scValidityErrorMessageTitle.setText("Stop to enter");
		scValidityErrorMessage.setText("Invalid value");
		scValidityErrorAlertTabPage.ok();

		SCTool.selectRange("A1");
		scInputBarInput.activate();
		typeKeys("12/31/07");
		typeKeys("<enter>");
		assertEquals("12/31/07", SCTool.getCellText("A1"));

		SCTool.selectRange("A2");
		scInputBarInput.activate();
		typeKeys("03/02/08");
		typeKeys("<enter>");
		assertEquals("03/02/08", SCTool.getCellText("A2"));

		SCTool.selectRange("A3");
		scInputBarInput.activate();
		typeKeys("01/01/08");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("A3"));

		SCTool.selectRange("A4");
		scInputBarInput.activate();
		typeKeys("03/01/08");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("A4"));

		SCTool.selectRange("A5");
		scInputBarInput.activate();
		typeKeys("01/02/08");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("A5"));

		SCTool.selectRange("B1");
		scInputBarInput.activate();
		typeKeys("02/29/08");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("B1"));

		SCTool.selectRange("B2");
		scInputBarInput.activate();
		typeKeys("test");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("B2"));

		SCTool.selectRange("B3");
		scInputBarInput.activate();
		typeKeys("39448");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("B3"));
	}

	/**
	 * test Allow Decimal equal in Validity.
	 */
	@Test
	public void testAllowDecimalEqual() {

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(2); // "Decimal"
		scValidityDecimalCompareOperator.select(0); // "equal"
		scValiditySourceInput.setText("0.33333333");
		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.check();
		scValidityErrorMessageTitle.setText("Stop to enter");
		scValidityErrorMessage.setText("Invalid value");
		scValidityErrorAlertTabPage.ok();

		SCTool.selectRange("A1");
		scInputBarInput.activate();
		typeKeys("0.33333333");
		typeKeys("<enter>");
		assertEquals("0.33333333", SCTool.getCellText("A1"));

		SCTool.selectRange("A2");
		scInputBarInput.activate();
		typeKeys("=1/3");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("A2"));

		SCTool.selectRange("A3");
		scInputBarInput.activate();
		typeKeys("0.3");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("A3"));

		SCTool.selectRange("A4");
		scInputBarInput.activate();
		typeKeys("0.333333333");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("A4"));

		SCTool.selectRange("B2");
		scInputBarInput.activate();
		typeKeys("test");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("B2"));
	}

	/**
	 * test Allow Text length, greater than or equal to in Validity.
	 */
	@Test
	public void testAllowGreaterTextLength() {

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(7); // "Text length"
		scValidityDecimalCompareOperator.select(4); // "greater than or equal to"
		scValiditySourceInput.setText("10");
		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.check();
		scValidityErrorMessageTitle.setText("Stop to enter");
		scValidityErrorMessage.setText("Invalid value");
		scValidityErrorAlertTabPage.ok();

		SCTool.selectRange("A1");
		scInputBarInput.activate();
		typeKeys("testtesttesttest");
		typeKeys("<enter>");
		assertEquals("testtesttesttest", SCTool.getCellText("A1"));

		SCTool.selectRange("A2");
		scInputBarInput.activate();
		typeKeys("test test ");
		typeKeys("<enter>");
		assertEquals("test test ", SCTool.getCellText("A2"));

		SCTool.selectRange("A4");
		scInputBarInput.activate();
		typeKeys(" ");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("A4"));

		SCTool.selectRange("A3");
		scInputBarInput.activate();
		typeKeys("Testatest");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("A3"));
	}

	/**
	 * test Allow Text length, less than in Validity.
	 */
	@Test
	public void testAllowLessThanTextLength() {

//		app.dispatch(".uno:Validation");

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(7); // "Text length"
		scValidityDecimalCompareOperator.select(1); // "less than"
		scValiditySourceInput.setText("10");

		scValidityInputHelpTabPage.select();
		scValidityInputHelpCheckbox.check();
		scValidityInputHelpTitle.setText("Help Info Title");
		scValidityHelpMessage.setText("help info");

		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.check();
		scValidityErrorAlertActionList.select("Information");
		scValidityErrorMessageTitle.setText("Notes to enter");
		scValidityErrorMessage.setText("Invalid value");
		scValidityErrorAlertTabPage.ok();

		SCTool.selectRange("A1");
		scInputBarInput.activate();
		typeKeys("testtesttesttest<enter>");
		activeMsgBox.ok();
		assertEquals("testtesttesttest", SCTool.getCellText("A1"));

		SCTool.selectRange("A2");
		scInputBarInput.activate();
		typeKeys("sfsafsddddddd<enter>");
		activeMsgBox.cancel();
		assertEquals("", SCTool.getCellText("A2"));

		SCTool.selectRange("A2");
		scInputBarInput.activate();
		typeKeys("10<enter>");
		assertEquals("10", SCTool.getCellText("A2"));

		SCTool.selectRange("A3");
		scInputBarInput.activate();
		typeKeys("ok<enter>");
		assertEquals("ok", SCTool.getCellText("A3"));
	}

	/**
	 * test Allow list.
	 */
	@Test
	public void testAllowListSpecialChar() {

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(6); // "List"
		scValidityEntries.focus();
		typeKeys("a");
		typeKeys("<enter>");
		typeKeys("b");
		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.check();
		scValidityErrorMessageTitle.setText("Stop to enter");
		scValidityErrorMessage.setText("Invalid value");
		scValidityCriteriaTabpage.select();
		scValidityCriteriaTabpage.ok();

		// These codes are not stable: start
		// calc.rightClick(1, 1);
		// typeKeys("<shift s>");
		// typeKeys("<down><enter>"); // Choose a
		// sleep(2); // if no sleep, error occur
		// These codes are not stable: end
		SCTool.selectRange("A1");
		scInputBarInput.activate();
		typeKeys("a<enter>");
		assertEquals("a", SCTool.getCellText("A1"));

		SCTool.selectRange("B2");
		scInputBarInput.activate();
		typeKeys("test");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("B2"));
	}

	/**
	 * test Allow time between in Validity.
	 */
	@Test
	public void testAllowTimeBetween() {

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(4); // "Time"
		scValidityDecimalCompareOperator.select(6); // "between"
		scValiditySourceInput.setText("27:00");
		scValidityMaxValueInput.setText("21:00");
		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.check();
		scValidityErrorMessageTitle.setText("Stop to enter");
		scValidityErrorMessage.setText("Invalid value");
		scValidityErrorAlertTabPage.ok();

		SCTool.selectRange("A1");
		scInputBarInput.activate();
		typeKeys("21:00");
		typeKeys("<enter>");
		assertEquals("09:00:00 PM", SCTool.getCellText("A1"));

		SCTool.selectRange("A2");
		scInputBarInput.activate();
		typeKeys("27:00");
		typeKeys("<enter>");
		assertEquals("27:00:00", SCTool.getCellText("A2"));

		SCTool.selectRange("A3");
		scInputBarInput.activate();
		typeKeys("1.125");
		typeKeys("<enter>");
		assertEquals("1.125", SCTool.getCellText("A3"));

		SCTool.selectRange("A4");
		scInputBarInput.activate();
		typeKeys("0.875");
		typeKeys("<enter>");
		assertEquals("0.875", SCTool.getCellText("A4"));

		SCTool.selectRange("B1");
		scInputBarInput.activate();
		typeKeys("03:00:01");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("B1"));

		SCTool.selectRange("B2");
		scInputBarInput.activate();
		typeKeys("20:59:59");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("B2"));

		SCTool.selectRange("B3");
		scInputBarInput.activate();
		typeKeys("1.126");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("B3"));

		SCTool.selectRange("B4");
		scInputBarInput.activate();
		typeKeys("0.874");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("B4"));

		SCTool.selectRange("C1");
		scInputBarInput.activate();
		typeKeys("test");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("C1"));

		SCTool.selectRange("C2");
		scInputBarInput.activate();
		typeKeys("24:00");
		typeKeys("<enter>");
		assertEquals("24:00:00", SCTool.getCellText("C2"));

		SCTool.selectRange("C3");
		scInputBarInput.activate();
		typeKeys("12:00");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("C3"));
	}

	/**
	 * test Allow time Greater than and equal to in Validity.
	 */
	@Test
	public void testAllowTimeGreaterThan() {

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(4); // "Time"
		scValidityDecimalCompareOperator.select(4); // "greater than or equal to"
		scValiditySourceInput.setText("8:00");

		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.check();
		scValidityErrorAlertActionList.select(1); // "Warning"

		scValidityErrorMessageTitle.setText("warning to enter");
		scValidityErrorMessage.setText("Invalid value");
		scValidityErrorAlertTabPage.ok();

		SCTool.selectRange("A1");
		scInputBarInput.activate();
		typeKeys("7:30");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("07:30:00 AM", SCTool.getCellText("A1"));

		SCTool.selectRange("A2");
		scInputBarInput.activate();
		typeKeys("6:00");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.cancel();
		assertEquals("", SCTool.getCellText("A2"));

		SCTool.selectRange("A3");
		scInputBarInput.activate();
		typeKeys("8:00");
		typeKeys("<enter>");
		assertEquals("08:00:00 AM", SCTool.getCellText("A3"));
	}

	/**
	 * test Allow whole number, less than or equal to in Validity.
	 */
	@Test
	public void testAllowWholeNumLessThan() {

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(1); // "Whole Numbers"
		scValidityDecimalCompareOperator.select(3); // "less than or equal"
		scValiditySourceInput.setText("100");
		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.check();
		scValidityErrorMessageTitle.setText("Stop to enter");
		scValidityErrorMessage.setText("Invalid value");
		scValidityErrorAlertTabPage.ok();

		SCTool.selectRange("A1");
		scInputBarInput.activate();
		typeKeys("99");
		typeKeys("<enter>");
		assertEquals("99", SCTool.getCellText("A1"));

		SCTool.selectRange("A2");
		scInputBarInput.activate();
		typeKeys("100");
		typeKeys("<enter>");
		assertEquals("100", SCTool.getCellText("A2"));

		SCTool.selectRange("B1");
		scInputBarInput.activate();
		typeKeys("101");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("B1"));

		SCTool.selectRange("B2");
		scInputBarInput.activate();
		typeKeys("45.5");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("B2"));

		SCTool.selectRange("C1");
		scInputBarInput.activate();
		typeKeys("test");
		typeKeys("<enter>");
		assertEquals("Invalid value", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("C1"));
	}

	/**
	 * test default message of Error Alert in Validity.
	 */
	@Test
	public void testDefaultErrorAlertMessage() {

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(2); // "Decimal"
		scValidityDecimalCompareOperator.select(0); // "equal"
		scValiditySourceInput.setText("1");

		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.check();
		scValidityErrorAlertActionList.select(0); // "Stop"
		scValidityErrorAlertTabPage.ok();

		SCTool.selectRange("A1");
		scInputBarInput.activate();
		typeKeys("13");
		typeKeys("<enter>");
		assertEquals("OpenOffice.org Calc", activeMsgBox.getCaption());
		// assertEquals("Invalid value.",ActiveMsgBox.getMessage()); // Can not
		// verify in multi-language
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("A1"));
	}

	/**
	 * test uncheck Error Alert in Validity.
	 */
	@Test
	public void testUncheckErrorAlert() {

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(2); // "Decimal"
		scValidityDecimalCompareOperator.select(0); // "equal"
		scValiditySourceInput.setText("1");

		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.uncheck();
		scValidityErrorAlertActionList.select(0); // "Stop"
		scValidityErrorAlertTabPage.ok();

		SCTool.selectRange("A1");
		typeKeys("13");
		typeKeys("<enter>");
		assertEquals("13", SCTool.getCellText("A1"));
	}

	/**
	 * test Cell range source picker in Validity. Input from Edit Box.
	 */
	@Test
	public void testValidityCellRangeSourcePicker() {

		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(5); // "Cell range"
		scValiditySourcePicker.click();
		assertEquals(false, scValidityCriteriaAllowList.exists());
		scValiditySourceInput.setText("$E$2:$G$5");
		scValiditySourcePicker.click();
		assertEquals(true, scValidityCriteriaAllowList.exists());

		scValidityErrorAlertTabPage.select();
		scValidityShowErrorMessage.check();
		scValidityErrorAlertActionList.select(0); // "Stop"
		scValidityErrorMessageTitle.setText("Stop to enter");
		scValidityErrorMessage.setText("Invalid value.");
		scValidityErrorAlertTabPage.ok();

		// calc.focus();
		SCTool.selectRange("E2");
		scInputBarInput.activate();
		typeKeys("test");
		typeKeys("<enter>");

		SCTool.selectRange("A1");
		scInputBarInput.activate();
		typeKeys("test32");
		typeKeys("<enter>");
		assertEquals("Invalid value.", activeMsgBox.getMessage());
		activeMsgBox.ok();
		assertEquals("", SCTool.getCellText("A1"));

		SCTool.selectRange("B1");
		scInputBarInput.activate();
		typeKeys("test");
		typeKeys("<enter>");
		assertEquals("test", SCTool.getCellText("B1"));
	}

	/**
	 * test Allow Blank cell Checkbox in Validity.
	 */
	@Test
	public void testAllowBlankCells() {
		scValidityCriteriaTabpage.select();
		scValidityCriteriaAllowList.select(5); // "Cell range"
		scValiditySourceInput.setText("$E$1:$E$5");
		scValidityAllowBlankCells.check();
		scValidityCriteriaTabpage.ok();

		SCTool.selectRange("E1");
		typeKeys("A<enter>A<enter>A<enter>A<enter>A<enter>");

		SCTool.selectRange("A1");
		typeKeys("A<enter>");
		SCTool.selectRange("D1");
		scInputBarInput.activate();
		typeKeys("<backspace><enter>");
		assertEquals("", SCTool.getCellText("D1"));

		SCTool.selectRange("B1");
		app.dispatch(".uno:Validation");
		scValidityCriteriaTabpage.select();
		scValidityAllowBlankCells.uncheck();
		typeKeys("<enter>");

		SCTool.selectRange("B1");
		scInputBarInput.activate();
		typeKeys("<backspace><enter>");
		typeKeys("<enter>");
	}
}
