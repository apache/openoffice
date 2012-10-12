package fvt.uno.sc.cell;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import org.openoffice.test.uno.UnoApp;

import com.sun.star.container.XIndexAccess;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.CellDeleteMode;
import com.sun.star.sheet.CellInsertMode;
import com.sun.star.sheet.XCellRangeAddressable;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.table.XCell;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.table.XCellRange;
import com.sun.star.table.CellRangeAddress;
import com.sun.star.sheet.XCellRangeMovement;

/**
 * Test insert or delete cells
 * @author BinGuo 8/30/2012
 *
 */

public class InsertDeleteCells {
	
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
	
	/**
	 * New spreadsheet
	 * Create 3x3 cell range A2:C4
	 * Execute insert empty A2 & B2 cells shift other existing cells in Column A & B down
	 * Execute insert empty A2 & B2 cells shift other existing cells in row 2 move right
	 * Execute insert entire empty Row 2 make the whole existing cell range moves down
	 * Execute insert entire empty Columns A & B make the whole existing cell range moves right
	 * Verify results after insert cells
	 */	
	
	@Test
	public void testInsertCells() throws Exception {
	
		scComponent = unoApp.newDocument("scalc");
		scDocument = (XSpreadsheetDocument) UnoRuntime.queryInterface(XSpreadsheetDocument.class, scComponent);
		XSpreadsheets xSpreadsheets = scDocument.getSheets();
		
		// Gets the first sheet in the document.			
        XIndexAccess xSheetsIA = (XIndexAccess)UnoRuntime.queryInterface(XIndexAccess.class, xSpreadsheets);
        Object sheetObj = (XSpreadsheet)UnoRuntime.queryInterface(XSpreadsheet.class, xSheetsIA.getByIndex(0));
		XSpreadsheet xSheet = (XSpreadsheet) UnoRuntime.queryInterface(XSpreadsheet.class, sheetObj);

	    // Create a 3x3 cell range "A2:C4" with the values 0 ... 8.
	    int nCol = 0;
	    int nValue = 0;
	    
	    for (int n = 1; n < 4; ++n){
	    	int nRow = 1;
	    	for (int i = 1; i < 4; ++i) {
			   xSheet.getCellByPosition( nCol, nRow ).setValue( nValue );
		       nRow += 1;
		       nValue += 1;
		    }	    
	        nCol += 1; 
	    }   

	    //Insert 2 cells in A2:B2 and shift other existing cells in Column A & B down
	    
		// Get cell range A2:B2 by position - (column, row, column, row)
        XCellRange xCellRange = xSheet.getCellRangeByPosition( 0, 1, 1, 1 );
        XCellRangeMovement xCellRangeMovement = (XCellRangeMovement) 
        		UnoRuntime.queryInterface(XCellRangeMovement.class, xSheet); 

        // Gets the selected range's address/location.
 	    XCellRangeAddressable xCellRangeAddr = (XCellRangeAddressable)
	            UnoRuntime.queryInterface( XCellRangeAddressable.class, xCellRange );
	    CellRangeAddress address = xCellRangeAddr.getRangeAddress();
	    
	    //Execute Insert cells in A2:B2 and shift other existing cells in Column A & B down
	    xCellRangeMovement.insertCells(address, CellInsertMode.DOWN);
		
	    //Get value of cell A2, B2 and C2  
        XCell cellA2 = xSheet.getCellByPosition(0, 1);
        XCell cellB2 = xSheet.getCellByPosition(1, 1); 
        XCell cellC2 = xSheet.getCellByPosition(2, 1); 
        double expectValueA2 = 0.0;
        double expectValueB2 = 0.0;
        double expectValueC2 = 6;
      
        //Verify results after execute Insert cells in A2:B2 and shift other existing cells in Column A & B down
        assertEquals("Verify value of A2 after execute Insert cells in A2:B2 and shift cells down.",
        		expectValueA2, cellA2.getValue(),0);
        assertEquals("Verify value of B2 after execute Insert cells in A2:B2 and shift cells down.",
        		expectValueB2, cellB2.getValue(),0);
        assertEquals("Verify value of C2 after execute Insert cells in A2:B2 and shift cells down.",
        		expectValueC2, cellC2.getValue(),0);
        
        //Execute Insert cells in A2:B2 and shift other existing cells in row 2 move right
	    xCellRangeMovement.insertCells(address, CellInsertMode.RIGHT);
	    
	    //Get value of cell C2, D2, E2 and C3  
	    cellC2 = xSheet.getCellByPosition(2, 1);
        XCell cellD2 = xSheet.getCellByPosition(3, 1); 
        XCell cellE2 = xSheet.getCellByPosition(4, 1); 
        XCell cellC3 = xSheet.getCellByPosition(2, 2); 
        double expectValueC2right = 0.0;
        double expectValueD2 = 0.0;
        double expectValueE2 = 6;
        double expectValueC3 = 7;
      
        //Verify results after execute Insert cells in A2:B2 and shift other existing cells in row 2 move right
        assertEquals("Verify value of C2 after execute Insert cells in A2:B2 and shift cells Right.",
        		expectValueC2right, cellC2.getValue(),0);
        assertEquals("Verify value of D2 after execute Insert cells in A2:B2 and shift cells Right.",
        		expectValueD2, cellD2.getValue(),0);
        assertEquals("Verify value of E2 after execute Insert cells in A2:B2 and shift cells Right.",
        		expectValueE2, cellE2.getValue(),0);
        assertEquals("Verify value of C3 after execute Insert cells in A2:B2 and shift cells Right.",
        		expectValueC3, cellC3.getValue(),0);
	  
        //Execute Insert Entire Row 2 make the whole existing cell range moves down
	    xCellRangeMovement.insertCells(address, CellInsertMode.ROWS);
        
	    //Get value of cell E2, E3 and C3  
	    cellE2 = xSheet.getCellByPosition(4, 1); 
        XCell cellE3 = xSheet.getCellByPosition(4, 2); 
        cellC3 = xSheet.getCellByPosition(2, 2); 
        double expectValueE2rows = 0.0;
        double expectValueE3 = 6;
        double expectValueC3rows = 0.0;
     
        //Verify results after execute Insert Entire Row 2 make the whole existing cell range moves down
        assertEquals("Verify value of E2 after execute Insert Entire Row 2 make the whole existing cell range moves down.",
        		expectValueE2rows, cellE2.getValue(),0);
        assertEquals("Verify value of E3 after execute Insert Entire Row 2 make the whole existing cell range moves down.",
        		expectValueE3, cellE3.getValue(),0);
        assertEquals("Verify value of C3 after execute Insert Entire Row 2 make the whole existing cell range moves down.",
        		expectValueC3rows, cellC3.getValue(),0);
         
	    //Execute Insert Entire Columns make the whole existing cell range moves right
	    xCellRangeMovement.insertCells(address, CellInsertMode.COLUMNS);
       
	    //Get value of cell C4, C5 and C6  
	    XCell cellC4 = xSheet.getCellByPosition(2, 3); 
        XCell cellC5 = xSheet.getCellByPosition(2, 4); 
        XCell cellC6 = xSheet.getCellByPosition(2, 5); 
        double expectValueC4 = 0.0;
        double expectValueC5 = 1;
        double expectValueC6 = 2;
    
        //Verify results after execute Insert Entire Columns make the whole existing cell range moves right
        assertEquals("Verify value of E2 after execute Insert Entire Row 2 make the whole existing cell range moves down.",
    		   expectValueC4, cellC4.getValue(),0);
        assertEquals("Verify value of E3 after execute Insert Entire Row 2 make the whole existing cell range moves down.",
    		   expectValueC5, cellC5.getValue(),0);
        assertEquals("Verify value of C3 after execute Insert Entire Row 2 make the whole existing cell range moves down.",
    		   expectValueC6, cellC6.getValue(),0);
	    		
    }

	/**
	 * New spreadsheet
	 * Create 3x3 cell range A2:C4
	 * Execute delete cells A2 & B2 shift other existing cells in column A & B move up
	 * Execute delete cells A2 & B2 shift other existing cells in row 2 move left
	 * Execute delete entire Row 2 make the whole existing cell range moves up
	 * Execute delete entire Columns A & B make the whole existing cell range moves left
	 * Verify results after delete cells
	 */	
	
    @Test
    public void testDeleteCells() throws Exception {

	    scComponent = unoApp.newDocument("scalc");
	    scDocument = (XSpreadsheetDocument) UnoRuntime.queryInterface(XSpreadsheetDocument.class, scComponent);
	    XSpreadsheets xSpreadsheets = scDocument.getSheets();
	
	    // Gets the first sheet in the document.			
        XIndexAccess xSheetsIA = (XIndexAccess)UnoRuntime.queryInterface(XIndexAccess.class, xSpreadsheets);
        Object sheetObj = (XSpreadsheet)UnoRuntime.queryInterface(XSpreadsheet.class, xSheetsIA.getByIndex(0));
	    XSpreadsheet xSheet = (XSpreadsheet) UnoRuntime.queryInterface(XSpreadsheet.class, sheetObj);

        // Create a 3x3 cell range "A2:C4" with the values 0 ... 8.
        int nCol = 0;
        int nValue = 0;
    
        for (int n = 1; n < 4; ++n){
    	    int nRow = 1;
    	    for (int i = 1; i < 4; ++i) {
		        xSheet.getCellByPosition( nCol, nRow ).setValue( nValue );
	            nRow += 1;
	            nValue += 1;
	        }	    
            nCol += 1; 
       }   

        //Insert 2 cells in A2:B2 and shift cells up
    
	    // Get cell range A2:B2 by position - (column, row, column, row)
        XCellRange xCellRange = xSheet.getCellRangeByPosition( 0, 1, 1, 1 );
        XCellRangeMovement xCellRangeMovement = (XCellRangeMovement) 
    		UnoRuntime.queryInterface(XCellRangeMovement.class, xSheet); 

        // Gets the selected range's address/location.
	    XCellRangeAddressable xCellRangeAddr = (XCellRangeAddressable)
            UnoRuntime.queryInterface( XCellRangeAddressable.class, xCellRange );
        CellRangeAddress address = xCellRangeAddr.getRangeAddress();
    
        //Execute delete cells in A2:B2 and shift cells in column A & B move up
        xCellRangeMovement.removeRange(address,CellDeleteMode.UP);
	
        //Get value of cell A2, B2 and C2   
        XCell cellA2 = xSheet.getCellByPosition(0, 1);
        XCell cellB2 = xSheet.getCellByPosition(1, 1); 
        XCell cellC2 = xSheet.getCellByPosition(2, 1); 
        double expectValueA2up = 1;
        double expectValueB2up = 4;
        double expectValueC2up = 6;
  
        //Verify results after execute delete cells in A2:B2 and shift cells in column A & B move up
        assertEquals("Verify value of A2 after execute delete cells in A2:B2 and shift cells up.",
    		expectValueA2up, cellA2.getValue(),0);
        assertEquals("Verify value of B2 after execute delete cells in A2:B2 and shift cells up.",
    		expectValueB2up, cellB2.getValue(),0);
        assertEquals("Verify value of C2 after execute delete cells in A2:B2 and shift cells up.",
    		expectValueC2up, cellC2.getValue(),0);
    
        //Execute delete cells in A2:B2 and shift other existing cells in row 2 move left
        xCellRangeMovement.removeRange(address,CellDeleteMode.LEFT);
    
        //Get value of cell A2, B2 and C2  
        cellA2 = xSheet.getCellByPosition(0, 1);
        cellB2 = xSheet.getCellByPosition(1, 1); 
        cellC2 = xSheet.getCellByPosition(2, 1); 
        double expectValueA2left = 6;
        double expectValueB2left = 0.0;
        double expectValueC2left = 0.0;
  
        //Verify results after execute delete cells in A2:B2 and shift other existing cells in row 2 move left
        assertEquals("Verify value of A2 after execute delete cells in A2:B2 and shift cells left.",
    		expectValueA2left, cellA2.getValue(),0);
        assertEquals("Verify value of B2 after execute delete cells in A2:B2 and shift cells left.",
    		expectValueB2left, cellB2.getValue(),0);
        assertEquals("Verify value of C2 after execute delete cells in A2:B2 and shift cells left.",
    		expectValueC2left, cellC2.getValue(),0);
        
        //Execute delete Entire Row 2 make the whole existing cell range moves up
        xCellRangeMovement.removeRange(address,CellDeleteMode.ROWS);
    
        //Get value of cell A2, B2 and C2  
        cellA2 = xSheet.getCellByPosition(0, 1);
        cellB2 = xSheet.getCellByPosition(1, 1); 
        cellC2 = xSheet.getCellByPosition(2, 1); 
        double expectValueA2rows = 2;
        double expectValueB2rows = 5;
        double expectValueC2rows = 7;
    
        //Verify results after delete Entire Row 2 make the whole existing cell range moves up
        assertEquals("Verify value of A2 after delete Entire Row 2 make the whole existing cell range moves up.",
    		expectValueA2rows, cellA2.getValue(),0);
        assertEquals("Verify value of B2 after delete Entire Row 2 make the whole existing cell range moves up.",
    		expectValueB2rows, cellB2.getValue(),0);
        assertEquals("Verify value of C2 after delete Entire Row 2 make the whole existing cell range moves up.",
    		expectValueC2rows, cellC2.getValue(),0);
    
        //Execute delete Entire Columns make the whole existing cell range moves left
        xCellRangeMovement.removeRange(address,CellDeleteMode.COLUMNS);
    
        //Get value of cell A2, B2 and C2  
        cellA2 = xSheet.getCellByPosition(0, 1);
        cellB2 = xSheet.getCellByPosition(1, 1); 
        cellC2 = xSheet.getCellByPosition(2, 1); 
        double expectValueA2columns = 7;
        double expectValueB2columns = 0.0;
        double expectValueC2columns = 0.0;
    
        //Verify results after execute delete Entire Columns make the whole existing cell range moves left
        assertEquals("Verify value of A2 after delete Entire Columns make the whole existing cell range moves left.",
    		expectValueA2columns, cellA2.getValue(),0);
        assertEquals("Verify value of B2 after delete Entire Columns make the whole existing cell range moves left.",
    		expectValueB2columns, cellB2.getValue(),0);
        assertEquals("Verify value of C2 after delete Entire Columns make the whole existing cell range moves left.",
    		expectValueC2columns, cellC2.getValue(),0);
    		
    }

}


