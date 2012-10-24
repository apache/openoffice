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

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import org.openoffice.test.uno.UnoApp;

import testlib.uno.SCUtil;
import testlib.uno.TestUtil;

import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.CellAddress;
import com.sun.star.table.XCell;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.sheet.XCellAddressable;
import com.sun.star.sheet.XSheetAnnotations;
import com.sun.star.sheet.XSheetAnnotationsSupplier;
import com.sun.star.sheet.XSheetAnnotation;
import com.sun.star.sheet.XSheetAnnotationAnchor;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.container.XIndexAccess;


/**
 * Test Create Show Hide Edit Delete Comments
 * @author BinGuo 9/5/2012
 *
 */

public class CreateShowHideEditDeleteComments {
	
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
	
	@BeforeClass
	public static void setUpConnection() throws Exception {
//		unoApp.start();
	}

	@AfterClass
	public static void tearDownConnection() throws InterruptedException, Exception {
//		unoApp.close();
		SCUtil.clearTempDir();	
	}
	
	/**
	 * New a spreadsheet
	 * Insert annotations for A1:A5
	 * Delete the 2nd annotations for A1:A5
	 * Edit text in annotation
	 */	
	
	@Test
	public void testCreateEditDeleteComments() throws Exception {
	
		scComponent = unoApp.newDocument("scalc");
		scDocument = (XSpreadsheetDocument) UnoRuntime.queryInterface(XSpreadsheetDocument.class, scComponent);
		XSpreadsheets xSpreadsheets = scDocument.getSheets();
			
		// Gets the first sheet in the document.			
	    XIndexAccess xSheetsIA = (XIndexAccess)UnoRuntime.queryInterface(XIndexAccess.class, xSpreadsheets);
		Object sheetObj = (XSpreadsheet)UnoRuntime.queryInterface(XSpreadsheet.class, xSheetsIA.getByIndex(0));
	    XSpreadsheet xSheet = (XSpreadsheet) UnoRuntime.queryInterface(XSpreadsheet.class, sheetObj);
				
		// Get current sheet
		xSheet = SCUtil.getCurrentSheet(scDocument);
		
		// Create cell range A2:A5 and Add annotation for cells
		int nRow = 1;
		
		for (int i = 1; i < 5; ++i) {
			XCell xCell = xSheet.getCellByPosition(0, nRow);
			xCell.setValue(nRow);
			
			// Create the CellAddress structure
	        XCellAddressable xCellAddr = (XCellAddressable)
	         UnoRuntime.queryInterface(XCellAddressable.class, xCell);
	        CellAddress aAddress = xCellAddr.getCellAddress();
	    
	        // Insert an annotation
	        XSheetAnnotationsSupplier xAnnotationsSupp =
	         (XSheetAnnotationsSupplier) UnoRuntime.queryInterface(
	             XSheetAnnotationsSupplier.class, xSheet);
	        XSheetAnnotations xAnnotations = xAnnotationsSupp.getAnnotations();
	        xAnnotations.insertNew(aAddress, "This is an annotation");
	   
	        nRow += 1;
	    }
	        
        XSheetAnnotationsSupplier xAnnotationsSupp =
		         (XSheetAnnotationsSupplier) UnoRuntime.queryInterface(
		             XSheetAnnotationsSupplier.class, xSheet);
		XSheetAnnotations xAnnotations = xAnnotationsSupp.getAnnotations();
		
        // Verify results after insert annotations for cell range A2:A5
     	assertEquals("Verify total number of annotations after execute insert annotations."
     	    		  ,4, xAnnotations.getCount());
   	
     	// Remove annotation
        xAnnotations.removeByIndex(1);
        
        // Verify results after delete annotations from cell range A2:A5
     	assertEquals("Verify number of annotations after execute delete annotations."
     	    		  ,3, xAnnotations.getCount());     	
     	    		    
    }
		
	/**
	 * New a spreadsheet
	 * Insert annotations for B2
	 * Show it
	 * Hide it
	 */	
	
	@Test
	public void testShowHideComments() throws Exception {
		
		scComponent = unoApp.newDocument("scalc");
		scDocument = (XSpreadsheetDocument) UnoRuntime.queryInterface(XSpreadsheetDocument.class, scComponent);
		XSpreadsheets xSpreadsheets = scDocument.getSheets();
			
		// Gets the first sheet in the document.			
	    XIndexAccess xSheetsIA = (XIndexAccess)UnoRuntime.queryInterface(XIndexAccess.class, xSpreadsheets);
		Object sheetObj = (XSpreadsheet)UnoRuntime.queryInterface(XSpreadsheet.class, xSheetsIA.getByIndex(0));
	    XSpreadsheet xSheet = (XSpreadsheet) UnoRuntime.queryInterface(XSpreadsheet.class, sheetObj);
				
		// Get current sheet
		xSheet = SCUtil.getCurrentSheet(scDocument);
		
		// Create the CellAddress structure
		
		// Get Cell B2
		int nColumn = 1;
		int nRow = 1;
		
	    XCell xCell = xSheet.getCellByPosition(nColumn, nRow);
	    XCellAddressable xCellAddr = (XCellAddressable)
	         UnoRuntime.queryInterface(XCellAddressable.class, xCell);
	    CellAddress aAddress = xCellAddr.getCellAddress();
	   
	    // Insert an annotation
	    XSheetAnnotationsSupplier xAnnotationsSupp =
	         (XSheetAnnotationsSupplier) UnoRuntime.queryInterface(
	             XSheetAnnotationsSupplier.class, xSheet);
	    XSheetAnnotations xAnnotations = xAnnotationsSupp.getAnnotations();
	    xAnnotations.insertNew(aAddress, "This is an annotation");
	    
	    XSheetAnnotationAnchor xAnnotAnchor =
                (XSheetAnnotationAnchor) UnoRuntime.queryInterface(XSheetAnnotationAnchor.class, xCell);
        XSheetAnnotation xAnnotation = xAnnotAnchor.getAnnotation();

        // Make the annotation visible
        xAnnotation.setIsVisible(true);
        ////////
        TestUtil.printPropertiesList(xAnnotAnchor);
        ////////////////
        // Verify annotation is visible.
        assertTrue("Verify annotation is visible in cell B2.",xAnnotation.getIsVisible());

        // Make the annotation invisible
        xAnnotation.setIsVisible(false);
  
        // Verify annotation is invisible.
        assertFalse("Verify annotation is invisible in cell B2.",xAnnotation.getIsVisible());
		
	}
	
}




