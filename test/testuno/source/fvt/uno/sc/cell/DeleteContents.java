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

package fvt.uno.sc.cell;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SCUtil;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.table.XCell;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.table.XCellRange;
import com.sun.star.sheet.XSheetAnnotations;
import com.sun.star.sheet.XSheetAnnotationsSupplier;
import com.sun.star.sheet.XSheetOperation;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XDrawPageSupplier;
import com.sun.star.drawing.XShapes;

/**
 * Test delete contents
 * @author BinGuo 9/4/2012
 *
 */

public class DeleteContents {
	
	UnoApp unoApp = new UnoApp();
	XSpreadsheetDocument scDocument = null;
	XComponent scComponent = null;
	private String filename = "uno/sc/cell/TestDeleteContents.ods";
	XDrawPage xpage = null;
	XShapes xShapes = null;
	
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
	 * Open existing ODS file with contents in cell range B5:C15
	 * Execute delete Text, Verify results after delete text
	 * Execute delete Number, Verify results after delete number
	 * Execute delete Formula, Verify results after delete formula
	 * Execute delete Comment, Verify results after delete comment
	 * Execute delete Format, Verify results after delete format
	 * Execute delete Drawing Object, Verify results after delete object
	 */	
	
	@Test
	public void testDeleteContents() throws Exception {
	
		// Prepare test data
		String sample = Testspace.prepareData(filename);
		
		// Open document
		scDocument = SCUtil.openFile(sample, unoApp);
		
		// Get current sheet
		XSpreadsheet xSheet = SCUtil.getCurrentSheet(scDocument);
	    
		// Get cell range B5:C15 by position - (column, row, column, row)
        XCellRange xCellRange = xSheet.getCellRangeByPosition( 1, 4, 2, 14 );
        XSheetOperation xSheetOp = (XSheetOperation) UnoRuntime.queryInterface(XSheetOperation.class, xCellRange);
        
        //Get Cell B5
        XCell cellB5 = xSheet.getCellByPosition(1, 4);
        
        //Delete Text from Cell Range B5:C15
        xSheetOp.clearContents(4);
             
        //Verify results after execute delete 'Text' contents.
        double NullCellValue = 0.0;
        assertEquals("Verify after execute delete 'Text' contents.",NullCellValue, cellB5.getValue(),0);
               
        //Get Cell B7
        XCell cellB7 = xSheet.getCellByPosition(1, 6);
        
        //Delete Date & Time from Cell Range B5:C15
        xSheetOp.clearContents(2);

        //Verify results after execute delete 'Date & Time' contents.
        assertEquals("Verify after execute delete 'Date & Time' contents.",NullCellValue, cellB7.getValue(),0);
        
        //Get Cell B8
        XCell cellB8 = xSheet.getCellByPosition(1, 7);
        
        //Delete Formula from Cell Range B5:C15
        xSheetOp.clearContents(16);
        
        //Verify results after execute delete 'Formula' contents.
        assertEquals("Verify after execute delete 'Formula' contents.",NullCellValue, cellB8.getValue(),0);
        
        //Delete Comment from Cell Range B5:C15
        xSheetOp.clearContents(8);

        XSheetAnnotationsSupplier xAnnotationsSupp =
		         (XSheetAnnotationsSupplier) UnoRuntime.queryInterface(
		             XSheetAnnotationsSupplier.class, xSheet);
		XSheetAnnotations xAnnotations = xAnnotationsSupp.getAnnotations();

        //Verify comment is deleted.
		assertEquals("Verify total number of annotations after execute delete annotations."
	    		  ,0, xAnnotations.getCount());      
        //Delete contents with Formatted from Cell Range B5:C15
//        xSheetOp.clearContents(512); @Ignore("#BUGID 120816 - BUG TITLE Method clearContents() clears 'Formatted' contents of the cells used does not work, If constants of com.sun.star.sheet.CellFlags is '512'. ")
        
        //Get Cell B6 and B10
        XCell cellB6 = xSheet.getCellByPosition(1, 5);
        XCell cellB10 = xSheet.getCellByPosition(1, 9);
        
        xSheetOp.clearContents(1);
        
        //Verify results after execute delete 'Number' contents.
        assertEquals("Verify after execute delete 'Number' contents in B6.",NullCellValue, cellB6.getValue(),0);
        assertEquals("Verify after execute delete 'Number' contents in B10.",NullCellValue, cellB10.getValue(),0);

        //Get Draw page
        XDrawPageSupplier xDrawPageSupplier =
        		(XDrawPageSupplier)UnoRuntime.queryInterface(XDrawPageSupplier.class, xSheet);
        XDrawPage xDrawPage = xDrawPageSupplier.getDrawPage();
        
        XShapes xShapes = (XShapes) UnoRuntime.queryInterface(XShapes.class, xDrawPage);
        
        //Verify number of shape in sheet.
        assertEquals("Verify number of shape in sheet.",1, xShapes.getCount());
        
        //Delete drawing object from Cell Range B5:C15
        xSheetOp.clearContents(128);
      
        xShapes = (XShapes) UnoRuntime.queryInterface(XShapes.class, xDrawPage);
  
        //Verify results after execute delete 'Objects' contents in cell range. 
        assertEquals("Verify shape is deleted in sheet after execute delete 'Objects' contents in cell range."
    		  ,0, xShapes.getCount());

    }
	
}


