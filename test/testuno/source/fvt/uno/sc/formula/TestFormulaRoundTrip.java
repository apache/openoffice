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


package fvt.uno.sc.formula;

import static org.junit.Assert.assertEquals;

import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SCUtil;
import static testlib.uno.TestUtil.*;

import com.sun.star.beans.PropertyValue;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.frame.XStorable;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.table.XCell;

public class TestFormulaRoundTrip {
	private UnoApp unoApp = new UnoApp();

	private XSpreadsheetDocument scDocument = null;
	private XComponent scComponent = null;

	@Before
	public void setUp() throws Exception {
		unoApp.start();
		scComponent = unoApp.newDocument("scalc");
		scDocument = SCUtil.getSCDocument(scComponent);
	}

	@After
	public void tearDown() throws Exception {
		unoApp.closeDocument(scComponent);
		unoApp.close();
	}

	private XSpreadsheet generateTestDocument(XSpreadsheetDocument scDocument) throws Exception {
		XSpreadsheet sheet = SCUtil.getCurrentSheet(scDocument);
		SCUtil.setTextToCell(sheet, 0, 0, "Hello world");
		SCUtil.setFormulaToCell(sheet, 0, 1, "=A1");
		return sheet;
	}

	@Test
	public void testMSExcel2003XMLFormulaRoundTrip() throws Exception {
		XSpreadsheet sheet = generateTestDocument(scDocument);
		String formulaValue = SCUtil.getTextFromCell(sheet, 0, 1);
		assertEquals("Hello world", formulaValue);
		String path = "output/sc/temp.xml";
		saveFormatTo("MS Excel 2003 XML", Testspace.getUrl(path));
		unoApp.closeDocument(scComponent);

		scDocument = (XSpreadsheetDocument) UnoRuntime.queryInterface(
			XSpreadsheetDocument.class, unoApp.loadDocument(Testspace.getPath(path)));
		sheet = SCUtil.getCurrentSheet(scDocument);
		String formulaValue2 = SCUtil.getTextFromCell(sheet, 0, 1);
		assertEquals("Hello world", formulaValue2);
		String formula2 = SCUtil.getFormulaFromCell(sheet, 0, 1);
		assertEquals("=A1", formula2);
	}

	@Test
	public void testStarOfficeXMLFormulaRoundTrip() throws Exception {
		XSpreadsheet sheet = generateTestDocument(scDocument);
		String formulaValue = SCUtil.getTextFromCell(sheet, 0, 1);
		assertEquals("Hello world", formulaValue);
		String path = "output/sc/temp.sxc";
		saveFormatTo("StarOffice XML (Calc)", Testspace.getUrl(path));
		unoApp.closeDocument(scComponent);

		scDocument = (XSpreadsheetDocument) UnoRuntime.queryInterface(
			XSpreadsheetDocument.class, unoApp.loadDocument(Testspace.getPath(path)));
		sheet = SCUtil.getCurrentSheet(scDocument);
		String formulaValue2 = SCUtil.getTextFromCell(sheet, 0, 1);
		assertEquals("Hello world", formulaValue2);
		String formula2 = SCUtil.getFormulaFromCell(sheet, 0, 1);
		assertEquals("=A1", formula2);
	}

	private void saveFormatTo(String filterName, String storeUrl) throws Exception {
		PropertyValue[] storeProps = new PropertyValue[2];
		storeProps[0] = new PropertyValue();
		storeProps[0].Name = "FilterName";
		storeProps[0].Value = filterName;
		storeProps[1] = new PropertyValue();
		storeProps[1].Name = "Overwrite";
		storeProps[1].Value = new Boolean(true);
		XStorable scStorable =
			(XStorable) UnoRuntime.queryInterface(XStorable.class, scComponent);
		scStorable.storeAsURL(storeUrl, storeProps);
	}
}
