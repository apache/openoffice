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

package fvt.uno.sc.rowcolumn;

import static org.junit.Assert.*;
import static testlib.uno.SCUtil.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.openoffice.test.uno.UnoApp;

import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.table.XTableColumns;
import com.sun.star.table.XTableRows;
import com.sun.star.table.XColumnRowRange;
import com.sun.star.beans.XPropertySet;
import com.sun.star.table.XCellRange;
import com.sun.star.frame.XModel;
import com.sun.star.frame.XController;
import com.sun.star.sheet.XSpreadsheetView;


public class ResizeHideShowRowColumn {

	UnoApp unoApp = new UnoApp();
	XSpreadsheetDocument scDocument = null;
	XComponent scComponent = null;
	
	@Before
	public void setUp() throws Exception {
		unoApp.start();
	}

	@After
	public void tearDown() throws Exception {
		unoApp.closeDocument(scComponent);
		unoApp.close();
		}

@Test
public void testResizeColumn() throws Exception {
	String sheetname = "AddTest";
  	XPropertySet PropSet = null;
	
	//Create Spreadsheet file.
	scComponent = unoApp.newDocument("scalc");
	scDocument = (XSpreadsheetDocument) UnoRuntime.queryInterface(XSpreadsheetDocument.class, scComponent);
	
	//Create a sheet at the first place.
	XSpreadsheets spreadsheets = scDocument.getSheets();
	spreadsheets.insertNewByName(sheetname, (short) 0);
	Object sheetObj = spreadsheets.getByName(sheetname);

	XSpreadsheet sheet = (XSpreadsheet) UnoRuntime.queryInterface(XSpreadsheet.class, sheetObj);
	
	//Active the new sheet.
	XModel scModel = (XModel) UnoRuntime.queryInterface(XModel.class, scDocument); 
    XController scController = scModel.getCurrentController();
    XSpreadsheetView sheetview = (XSpreadsheetView) UnoRuntime.queryInterface(XSpreadsheetView.class, scController);
    sheetview.setActiveSheet(sheet);
    
    //Set cell range to A1:B1
    XCellRange CellRange = sheet.getCellRangeByPosition(0, 0, 1, 0);
    
    //Get column A1 by index
    XColumnRowRange ColRowRange = (XColumnRowRange)UnoRuntime.queryInterface( XColumnRowRange.class, CellRange );
    XTableColumns Columns = ColRowRange.getColumns();
    Object aColumnObj = Columns.getByIndex( 0 );
 
    PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aColumnObj);
    
    //Verify the default values of specified column A1
    assertTrue("Verify column is visible as default.",  (Boolean) PropSet.getPropertyValue("IsVisible"));
    
    //Resize width of column A1 to "6001"
    PropSet.setPropertyValue( "Width", new Integer( 6001 ));

    //Save and reload document
    saveFileAs(scComponent, "TestColumn", "ods");
    XSpreadsheetDocument TempSCDocument = reloadFile(unoApp, scDocument, "TestColumn.ods");
    scDocument = TempSCDocument;
    
    spreadsheets = scDocument.getSheets();
   	sheetObj = spreadsheets.getByName(sheetname);
	sheet = (XSpreadsheet) UnoRuntime.queryInterface(XSpreadsheet.class, sheetObj);
	
    //Set cell range to A1:B1
    CellRange = sheet.getCellRangeByPosition(0, 0, 1, 0);
    ColRowRange = (XColumnRowRange)UnoRuntime.queryInterface( XColumnRowRange.class, CellRange );
    Columns = ColRowRange.getColumns();
    
    //Get column A1 by index
    aColumnObj = Columns.getByIndex( 0 );
 
    PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aColumnObj);

    //Verify the  values of specified column A1 after resize
       int expectedWidth = 6001;

    assertEquals("Verify current width value is 6001.", expectedWidth, PropSet.getPropertyValue("Width"));
    assertTrue("Verify column is visible as default.",  (Boolean) PropSet.getPropertyValue("IsVisible"));

    //Set column is invisible
    PropSet.setPropertyValue("IsVisible", new Boolean(false));
   
    //Save and reload document
    //Save the modified spreadsheet first
    save(TempSCDocument);
    //close it and reload it
    TempSCDocument = reloadFile(unoApp, scDocument, "TestColumn.ods");
    scDocument = TempSCDocument;
   
    spreadsheets = scDocument.getSheets();
	sheetObj = spreadsheets.getByName(sheetname);
	sheet = (XSpreadsheet) UnoRuntime.queryInterface(XSpreadsheet.class, sheetObj);
    
    //Set cell range to A1:B1
    CellRange = sheet.getCellRangeByPosition(0, 0, 1, 0);
    ColRowRange = (XColumnRowRange)UnoRuntime.queryInterface( XColumnRowRange.class, CellRange );
    Columns = ColRowRange.getColumns();
    
    //Get column A1 by index
    aColumnObj = Columns.getByIndex( 0 );
 
    PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aColumnObj);
    
    //Verify the values of specified column A1 after save
    assertFalse("Verify column A1 is invisible", (Boolean) PropSet.getPropertyValue("IsVisible"));
    assertEquals("Verify current width value is 6001 after hide it.", expectedWidth, PropSet.getPropertyValue("Width"));
    }

@Test
public void testResizeRow() throws Exception {
	String sheetname = "AddTest";
	XPropertySet PropSet = null;
	
	//Create Spreadsheet file.
	scComponent = unoApp.newDocument("scalc");
	scDocument = (XSpreadsheetDocument) UnoRuntime.queryInterface(XSpreadsheetDocument.class, scComponent);
	
	//Create a sheet at the first place.
	XSpreadsheets spreadsheets = scDocument.getSheets();
	spreadsheets.insertNewByName(sheetname, (short) 0);
	Object sheetObj = spreadsheets.getByName(sheetname);
	XSpreadsheet sheet = (XSpreadsheet) UnoRuntime.queryInterface(XSpreadsheet.class, sheetObj);
	
	//Active the new sheet.
	XModel scModel = (XModel) UnoRuntime.queryInterface(XModel.class, scDocument); 
    XController scController = scModel.getCurrentController();
    XSpreadsheetView sheetview = (XSpreadsheetView) UnoRuntime.queryInterface(XSpreadsheetView.class, scController);
    sheetview.setActiveSheet(sheet);
    
    //Set cell range to A1:A2
    XCellRange CellRange = sheet.getCellRangeByPosition(0, 0, 0, 1);
    //XCell cell = sheet.getCellByPosition(1, 0);
    XColumnRowRange ColRowRange = (XColumnRowRange)UnoRuntime.queryInterface( XColumnRowRange.class, CellRange );
    XTableRows Rows = ColRowRange.getRows();
    
    //Get Row 1 by index
    Object aRowObj = Rows.getByIndex( 0 );

    PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj );
    
    //Verify the default values of specified Row 1
    assertTrue("Verify column is visible as default.",  (Boolean) PropSet.getPropertyValue("IsVisible"));

    //Resize Height of Row 1 to "5001"
    PropSet.setPropertyValue( "Height", new Integer( 5001 ) );

    // Save and reload document
    saveFileAs(scComponent, "TestRow", "xls");
    XSpreadsheetDocument TempSCDocument = reloadFile(unoApp, scDocument, "TestRow.xls");
    scDocument = TempSCDocument;
    
    spreadsheets = scDocument.getSheets();
	sheetObj = spreadsheets.getByName(sheetname);
	sheet = (XSpreadsheet) UnoRuntime.queryInterface(XSpreadsheet.class, sheetObj);
	
    //Set cell range to A1:A2
    CellRange = sheet.getCellRangeByPosition(0, 0, 0, 1);
    ColRowRange = (XColumnRowRange)UnoRuntime.queryInterface( XColumnRowRange.class, CellRange );
    Rows = ColRowRange.getRows();
 
    //Get Row 1 by index
    aRowObj = Rows.getByIndex( 0 );

    PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);

    //Verify the  values of specified Row 1 after resize
    int expectedHeight = 5001;

    assertEquals("Verify current width value is 5001.", expectedHeight, PropSet.getPropertyValue("Height"));
    assertTrue("Verify column is visible as default.", (Boolean) PropSet.getPropertyValue("IsVisible"));

    //Set Row is invisible
    PropSet.setPropertyValue("IsVisible", new Boolean(false));

    //Save and reload document
    //Save the modified spreadsheet first
    save(TempSCDocument);
    //Close and reload it
    TempSCDocument = reloadFile(unoApp, scDocument, "TestRow.xls");
    scDocument = TempSCDocument;
    
    spreadsheets = scDocument.getSheets();
 	sheetObj = spreadsheets.getByName(sheetname);
 	sheet = (XSpreadsheet) UnoRuntime.queryInterface(XSpreadsheet.class, sheetObj);
 	
    //Set cell range to A1:A2
    CellRange = sheet.getCellRangeByPosition(0, 0, 0, 1);
    ColRowRange = (XColumnRowRange)UnoRuntime.queryInterface( XColumnRowRange.class, CellRange );
    Rows = ColRowRange.getRows();
  
    //Get Row 1 by index
    aRowObj = Rows.getByIndex( 0 );

    PropSet = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, aRowObj);

    //Verify the values of specified Row 1 after resize
    assertEquals("Verify current height value is 5001 after hide it.", expectedHeight, PropSet.getPropertyValue("Height"));
    assertFalse("Verify column is invisible.",  (Boolean) PropSet.getPropertyValue("IsVisible"));

    }

}


