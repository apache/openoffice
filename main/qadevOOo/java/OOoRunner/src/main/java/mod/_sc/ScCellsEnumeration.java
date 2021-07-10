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


package mod._sc;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.SOfficeFactory;

import com.sun.star.container.XEnumerationAccess;
import com.sun.star.container.XIndexAccess;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sheet.XCellRangesQuery;
import com.sun.star.sheet.XSheetCellRanges;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.table.XCell;
import com.sun.star.table.XCellRange;
import com.sun.star.text.XTextRange;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
* Test for object which is represented by service
* <code>com.sun.star.sheet.CellsEnumeration</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::container::XEnumeration</code></li>
* </ul>
* @see com.sun.star.sheet.CellsEnumeration
* @see com.sun.star.container.XEnumeration
* @see ifc.container._XEnumeration
*/
public class ScCellsEnumeration extends TestCase {
    static XSpreadsheetDocument xSheetDoc = null;

    /**
    * Creates Spreadsheet document.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {

        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );

        try {
            log.println( "creating a Spreadsheet document" );
            xSheetDoc = SOF.createCalcDoc(null);
        } catch ( com.sun.star.uno.Exception e ) {
            // Some exception occured.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
    * Disposes Spreadsheet document.
    */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xSheetDoc " );
        XComponent oComp = (XComponent)
            UnoRuntime.queryInterface (XComponent.class, xSheetDoc) ;
        util.DesktopTools.closeDoc(oComp);
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Retrieves a collection of spreadsheets from a document
    * and takes one of them. Replaces text of some cells.
    * Retrieves a cell range of the visible cells using the interface
    * <code>XCellRangesQuery</code>. Retrieves a collection of cells from
    * this cell range and creates it's enumeration using the interface
    * <code>XEnumerationAccess</code>.The created enumeration is the instance
    * of the service <code>com.sun.star.sheet.CellsEnumeration</code>.
    * Object relations created :
    * <ul>
    *  <li> <code>'ENUM'</code> for
    *      {@link ifc.container._XEnumeration} (type of
    *      <code>XEnumerationAccess</code> that was retrieved from the
    *      collection of visible cells)</li>
    * </ul>
    * @see com.sun.star.sheet.XCellRangesQuery
    * @see com.sun.star.container.XEnumerationAccess
    */
    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {

        XInterface oObj = null;
        Object cellArr[] = new Object[3] ;
        XEnumerationAccess oEnum = null;

        // creation of testobject here
        XSpreadsheets oSheets = (XSpreadsheets)xSheetDoc.getSheets();
        XIndexAccess oIndexAccess = (XIndexAccess)
            UnoRuntime.queryInterface(XIndexAccess.class, oSheets);
        XCellRange oSheet = null;
        try {
            oSheet = (XCellRange) AnyConverter.toObject(
                    new Type(XCellRange.class),oIndexAccess.getByIndex(0));

            XCell oCell_1 = (XCell)oSheet.getCellByPosition(0, 0);
            XTextRange oTextRange = (XTextRange)
                UnoRuntime.queryInterface(XTextRange.class, oCell_1);
            oTextRange.setString("Test string 1");

            XCell oCell_2 = (XCell)oSheet.getCellByPosition(5, 1);
            oCell_2.setValue(15);

            XCell oCell_3 = (XCell)oSheet.getCellByPosition(3, 9);
            oTextRange = (XTextRange)
                UnoRuntime.queryInterface(XTextRange.class, oCell_3);
            oTextRange.setString("test 2");
            cellArr[0] = oCell_1;
            cellArr[1] = oCell_2;
            cellArr[2] = oCell_3;
        } catch(com.sun.star.lang.WrappedTargetException e) {
            log.println ("Exception occurred while creating test Object.");
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        } catch(com.sun.star.lang.IndexOutOfBoundsException e) {
            log.println ("Exception occurred while creating test Object.");
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        } catch(com.sun.star.lang.IllegalArgumentException e) {
            log.println ("Exception occurred while creating test Object.");
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        }

        XCellRangesQuery oCellRangesQuery = (XCellRangesQuery)
            UnoRuntime.queryInterface(XCellRangesQuery.class, oSheet);
        XSheetCellRanges oSheetCellRanges = oCellRangesQuery.queryVisibleCells();
        oEnum = (XEnumerationAccess) oSheetCellRanges.getCells();
        oObj = oSheetCellRanges.getCells().createEnumeration();

        TestEnvironment tEnv = new TestEnvironment(oObj);

        // ENUM: XEnumeration
        tEnv.addObjRelation("ENUM", oEnum);

        return tEnv;
    }

}    // finish class ScCellsEnumeration

