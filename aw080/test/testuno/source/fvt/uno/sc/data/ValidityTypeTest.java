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
package fvt.uno.sc.data;

import static org.junit.Assert.*;

import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SCUtil;
import testlib.uno.TestUtil;

import com.sun.star.beans.XPropertySet;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.ConditionOperator;
import com.sun.star.sheet.GeneralFunction;
import com.sun.star.sheet.ValidationAlertStyle;
import com.sun.star.sheet.ValidationType;
import com.sun.star.sheet.XSheetCondition;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.XCellRange;
import com.sun.star.uno.Enum;
import com.sun.star.uno.UnoRuntime;

@RunWith(value = Parameterized.class)
public class ValidityTypeTest {
	private static final UnoApp app = new UnoApp();

	UnoApp unoApp = new UnoApp();
	XSpreadsheetDocument scDocument = null;
	XComponent scComponent = null;

	private Enum validationtype;

	private boolean ignoreBlankCells;

	private Enum validationAlertStyle;

	private boolean showInputMessage;

	private boolean showErrorMessage;

	private Enum conditionOperator;

	private String formula1;

	private String formula2;

	private String extName;

	@Parameters
	public static Collection<Object[]> data() throws Exception {
		return Arrays
				.asList(new Object[][] {
						{ ValidationType.ANY, false, ValidationAlertStyle.STOP,
								false, false, ConditionOperator.EQUAL, "0",
								"5", "xls" },
								//Custom type is not exist in GUI so remove this 
						// { ValidationType.CUSTOM, true,
						// ValidationAlertStyle.INFO, false, true,
						// ConditionOperator.EQUAL, "0", "5", "ods"},
						{ ValidationType.DATE, false,
								ValidationAlertStyle.MACRO, false, false,
								ConditionOperator.EQUAL, "0", "0", "ods" },
						{ ValidationType.DECIMAL, true,
								ValidationAlertStyle.WARNING, false, true,
								ConditionOperator.BETWEEN, "0", "5", "ods" },
						{ ValidationType.LIST, false,
								ValidationAlertStyle.STOP, true, false,
								ConditionOperator.EQUAL, "\"Joe\";\"Jane\"",
								"0", "ods" },
						{ ValidationType.LIST, false,
								ValidationAlertStyle.STOP, true, false,
								ConditionOperator.EQUAL, "a;b;c", "0", "ods" },
						{ ValidationType.TEXT_LEN, true,
								ValidationAlertStyle.INFO, true, true,
								ConditionOperator.GREATER, "0", "5", "xls" },
						{ ValidationType.TIME, false,
								ValidationAlertStyle.STOP, true, false,
								ConditionOperator.GREATER_EQUAL, "0", "5",
								"xls" },
						{ ValidationType.WHOLE, true,
								ValidationAlertStyle.WARNING, true, true,
								ConditionOperator.LESS, "0", "5", "xls" },
						{ ValidationType.DECIMAL, false,
								ValidationAlertStyle.STOP, true, false,
								ConditionOperator.LESS_EQUAL, "0", "5", "xls" },
						{ ValidationType.WHOLE, true,
								ValidationAlertStyle.INFO, false, true,
								ConditionOperator.NOT_BETWEEN, "0", "5", "ods" },
						{ ValidationType.TEXT_LEN, false,
								ValidationAlertStyle.MACRO, true, false,
								ConditionOperator.NOT_EQUAL, "5", "0", "ods" }, });
	}

	@Before
	public void setUpDocument() throws Exception {
		unoApp.start();
	}

	@After
	public void tearDownDocument() {
		unoApp.close();
		unoApp.closeDocument(scComponent);

	}

	@BeforeClass
	public static void setUpConnection() throws Exception {

	}

	@AfterClass
	public static void tearDownConnection() throws InterruptedException,
			Exception {

	}

	public ValidityTypeTest(Enum validationtype, boolean ignoreBlankCells,
			Enum validationAlertStyle, boolean showInputMessage,
			boolean showErrorMessage, Enum conditionOperator, String formula1,
			String formula2, String extName) {
		this.validationtype = validationtype;
		this.ignoreBlankCells = ignoreBlankCells;
		this.validationAlertStyle = validationAlertStyle;
		this.showInputMessage = showInputMessage;
		this.showErrorMessage = showErrorMessage;
		this.conditionOperator = conditionOperator;
		this.formula1 = formula1;
		this.formula2 = formula2;
		this.extName = extName;
	}

	@Test
	public void test() throws Exception {
		// New a SC document
		scComponent = unoApp.newDocument("scalc");
		scDocument = SCUtil.getSCDocument(scComponent);
		XSpreadsheet currentsheet = SCUtil.getCurrentSheet(scDocument);
		// --- Data validation ---
		XCellRange xCellRange = currentsheet.getCellRangeByName("A7:C7");
		XPropertySet xCellPropSet = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xCellRange);

		// validation properties
		XPropertySet xValidPropSet = (XPropertySet) UnoRuntime
				.queryInterface(XPropertySet.class,
						xCellPropSet.getPropertyValue("Validation"));

		String errorTitle = "This is an Error title";
		String inputHelp = "This is an input help";
		String inputTitle = "This is an input title";
		String errorMessage = "This is an invalid value!";
		xValidPropSet.setPropertyValue("IgnoreBlankCells", ignoreBlankCells);
		xValidPropSet.setPropertyValue("Type", validationtype);
		xValidPropSet.setPropertyValue("ErrorTitle", errorTitle);
		xValidPropSet.setPropertyValue("ShowErrorMessage", showErrorMessage);
		xValidPropSet.setPropertyValue("ErrorMessage", errorMessage);
		xValidPropSet.setPropertyValue("ErrorAlertStyle", validationAlertStyle);
		xValidPropSet.setPropertyValue("ShowInputMessage", showInputMessage);
		xValidPropSet.setPropertyValue("InputMessage", inputHelp);
		xValidPropSet.setPropertyValue("InputTitle", inputTitle);

		// condition
		XSheetCondition xCondition = (XSheetCondition) UnoRuntime
				.queryInterface(XSheetCondition.class, xValidPropSet);
		xCondition.setOperator((ConditionOperator) conditionOperator);
		xCondition.setFormula1(formula1);
		xCondition.setFormula2(formula2);

		// apply on cell range
		xCellPropSet.setPropertyValue("Validation", xValidPropSet);

		// Save and reload the file
		SCUtil.saveFileAs(scComponent, "Validationtest", extName);
		XSpreadsheetDocument scDocumentTemp = SCUtil.reloadFile(unoApp,
				scDocument, "Validationtest." + extName);
		scDocument = scDocumentTemp;
		currentsheet = SCUtil.getCurrentSheet(scDocument);

		// --- Data validation ---
		xCellRange = currentsheet.getCellRangeByName("A7:C7");
		xCellPropSet = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xCellRange);
		// validation properties
		xValidPropSet = (XPropertySet) UnoRuntime
				.queryInterface(XPropertySet.class,
						xCellPropSet.getPropertyValue("Validation"));

		// Verify the validation property
		//If validation Alert Style is Macro, after save the error message will be clean
		if (validationAlertStyle == ValidationAlertStyle.MACRO) {
			assertEquals("", xValidPropSet.getPropertyValue("ErrorMessage"));
		} else {
			assertEquals(errorMessage,
					xValidPropSet.getPropertyValue("ErrorMessage"));
		}
		assertEquals(showErrorMessage,
				xValidPropSet.getPropertyValue("ShowErrorMessage"));
		assertEquals(showInputMessage,
				xValidPropSet.getPropertyValue("ShowInputMessage"));
		assertEquals(inputHelp, xValidPropSet.getPropertyValue("InputMessage"));
		assertEquals(ignoreBlankCells,
				xValidPropSet.getPropertyValue("IgnoreBlankCells"));
		assertEquals(validationtype, xValidPropSet.getPropertyValue("Type"));
		assertEquals(errorTitle, xValidPropSet.getPropertyValue("ErrorTitle"));
		assertEquals(validationAlertStyle,
				xValidPropSet.getPropertyValue("ErrorAlertStyle"));
		assertEquals(inputTitle, xValidPropSet.getPropertyValue("InputTitle"));

		xCondition = (XSheetCondition) UnoRuntime.queryInterface(
				XSheetCondition.class, xValidPropSet);
		assertEquals(formula1, xCondition.getFormula1());
		assertEquals(formula2, xCondition.getFormula2());
		assertEquals(conditionOperator, xCondition.getOperator());

	}
}
