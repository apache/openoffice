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

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;

import org.openoffice.test.common.Testspace;
import org.openoffice.test.common.Logger;

import org.openoffice.test.uno.UnoApp;

import testlib.uno.SCUtil;
import static testlib.uno.TestUtil.*;

import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.table.XCell;
import com.sun.star.uno.Any;
import com.sun.star.text.XText;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.XModifiable;

import java.util.logging.Level;


public class TestFormulaDocs {

	@Rule
	public Logger log = Logger.getLogger(this);

	UnoApp unoApp = new UnoApp();
	XComponent scComponent = null;

	@Before
	public void setUp() throws Exception {
		unoApp.start();
	}

	@After
	public void tearDown() throws Exception {
		unoApp.close();
	}

	/**
	 * Test evaluation of formulas in a sample document
	 * 
	 * @throws Exception
	 */

	@Test
	public void testFormulaDocs() throws Exception {
		testOneDoc( "uno/sc/fvt/FormulaTest1.ods");
	}

	public void testOneDoc( String filename) throws Exception {
		// open the spreadsheet document
		String sample = Testspace.prepareData( filename);
		XSpreadsheetDocument scDoc = SCUtil.openFile( sample, unoApp);
		XSpreadsheet xSheet = SCUtil.getCurrentSheet( scDoc);

		// find the "TestID" and "TestOK" markers
		int nTestIdCol = -1;
		int nTestOkCol = -1;
		int nTestRowStart = -1;
		for( int y = 0; y < 8; ++y) {
			for( int x = 0; x < 8; ++x) {
				XCell xCell = xSheet.getCellByPosition( x, y);
				XText xText = (XText)UnoRuntime.queryInterface( XText.class, xCell);
				String name = xText.getString();
				if( name.equals( "TestID")) {
					assertTrue( "Multiple rows with TestID marker!", nTestIdCol == -1);
					assertTrue( nTestRowStart == -1);
					nTestIdCol = x;
					nTestRowStart = y + 1;
				} else if( name.equals( "TestOK")) {
					assertTrue( "Multiple rows with TestOK marker!", nTestOkCol == -1);
					assertTrue( "TestID and TestOK marker not in same row!", nTestRowStart == y + 1);
					nTestOkCol = x;
				}
			}
		}
		assertTrue( "Column \"TestID\" not found!", nTestIdCol >= 0);
		assertTrue( "Column \"TestOK\" not found!", nTestOkCol >= 0);

		int nTestRowEnd = nTestRowStart + 100; // TODO: get the last row from the sheet
		int nTestCount = 0;
		int nFailCount = 0;
		for( int y = nTestRowStart; y < nTestRowEnd; ++y) {
			// get the test id
			XCell xCell = xSheet.getCellByPosition( nTestIdCol, y);
			XText xText = (XText)UnoRuntime.queryInterface( XText.class, xCell);
			String testId = xText.getString();
			// ignore rows without test ids
			if( testId.length() == 0)
				continue;
			++nTestCount;

			// get and check the test result
			xCell = xSheet.getCellByPosition( nTestOkCol, y);
			String testOk = ((XText)UnoRuntime.queryInterface( XText.class, xCell)).getString();
			assertTrue( "Test result must be TRUE or FALSE", testOk.equals("TRUE") || testOk.equals("FALSE"));
			boolean bOK = testOk.equals("TRUE");
			// mark evaluated test results
			SCUtil.setProperties( xCell, "CellBackColor", (Integer)(bOK ? 0x00FF00 : 0xFF0000));
			// handle failed test cases
			if( !bOK) {
				++nFailCount;
				log.log( Level.SEVERE, "\ttest \""+testId+" failed");
			}
		}

		assertTrue( (""+nFailCount+" of "+nTestCount+" tests failed"), nFailCount==0);

		XModifiable modified = (XModifiable)UnoRuntime.queryInterface( XModifiable.class, scDoc);
		modified.setModified( false);
		SCUtil.closeFile( scDoc);
	}

}

