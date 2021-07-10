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
import util.utils;

import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XIndexAccess;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
* Test for object which is represented by service
* <code>com.sun.star.sheet.DDELinks</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::container::XNameAccess</code></li>
*  <li> <code>com::sun::star::container::XElementAccess</code></li>
* </ul>
* The following files used by this test :
* <ul>
*  <li><b> ScDDELinksObj.sdc </b> : the predefined testdocument </li>
* </ul> <p>
* @see com.sun.star.sheet.DDELinks
* @see com.sun.star.container.XNameAccess
* @see com.sun.star.container.XElementAccess
* @see ifc.container._XNameAccess
* @see ifc.container._XElementAccess
*/
public class ScDDELinksObj extends TestCase {
    static XSpreadsheetDocument xSheetDoc = null;
    static XComponent oDoc = null;

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
    * Disposes Spreadsheet document and testdocument.
    */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xSheetDoc " );
        XComponent oComp = (XComponent) UnoRuntime.
                                queryInterface(XComponent.class, xSheetDoc) ;
        util.DesktopTools.closeDoc(oComp);
        util.DesktopTools.closeDoc(oDoc);
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Disposes the testdocument if it was loaded already.
    * Creates an instance of the <code>com.sun.star.frame.Desktop</code>
    * and loads the predefined testdocument. Retrieves a collection of
    * spreadsheets from a document and takes one of them. Sets specific formula
    * to some cells in the spreadsheet(the formula specify DDE links to the
    * testdocument). Retrieves the collection of DDE links in the document.
    * The retrieved collection of DDE link is the instance of the service
    * <code>com.sun.star.sheet.DDELinks</code>.
    * @see com.sun.star.frame.Desktop
    * @see com.sun.star.sheet.DDELinks
    */
    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {

        XInterface oObj = null;

        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "Creating a test environment" );

        // create testobject here

        XMultiServiceFactory oMSF = (XMultiServiceFactory)Param.getMSF();

        // load the predefined testdocument
        String testdoc = utils.getFullTestURL("ScDDELinksObj.sdc");
        try {
            oDoc = SOfficeFactory.getFactory(oMSF).loadDocument(testdoc);
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log);
            throw new StatusException("Can't load test document", e);
        } catch (com.sun.star.io.IOException e) {
            e.printStackTrace(log);
            throw new StatusException("Can't load test document", e);
        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace(log);
            throw new StatusException("Can't load test document", e);
        }

        log.println("getting sheets");
        XSpreadsheets xSpreadsheets = (XSpreadsheets)xSheetDoc.getSheets();

        log.println("getting a sheet");
        XSpreadsheet oSheet = null;
        XIndexAccess oIndexAccess = (XIndexAccess)
            UnoRuntime.queryInterface(XIndexAccess.class, xSpreadsheets);
        try {
            oSheet = (XSpreadsheet) AnyConverter.toObject(
                    new Type(XSpreadsheet.class),oIndexAccess.getByIndex(0));
        } catch (com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get a spreadsheet", e);
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get a spreadsheet", e);
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't get a spreadsheet", e);
        }

        testdoc = utils.getFullTestDocName("ScDDELinksObj.sdc");
        log.println("filling some cells");
        try {
            oSheet.getCellByPosition(5, 5).setFormula(
                "=DDE(\"soffice\";\""+testdoc+"\";\"Sheet1.A1\"");
            oSheet.getCellByPosition(1, 4).setFormula(
                "=DDE(\"soffice\";\""+testdoc+"\";\"Sheet1.A1\"");
            oSheet.getCellByPosition(2, 0).setFormula(
                "=DDE(\"soffice\";\""+testdoc+"\";\"Sheet1.A1\"");
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            e.printStackTrace(log);
            throw new StatusException(
                "Exception occurred while filling cells", e);
        }

        try {
            log.println("Getting test object ") ;

            // Getting named ranges.
            XPropertySet docProps = (XPropertySet)
                UnoRuntime.queryInterface(XPropertySet.class, xSheetDoc);
            oObj = (XInterface)AnyConverter.toObject(
                new Type(XInterface.class),docProps.getPropertyValue("DDELinks"));
            log.println("Creating object - " +
                                        ((oObj == null) ? "FAILED" : "OK"));
        } catch (com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log) ;
            throw new StatusException(
                "Error getting test object from spreadsheet document", e) ;
        } catch (com.sun.star.beans.UnknownPropertyException e) {
            e.printStackTrace(log) ;
            throw new StatusException(
                "Error getting test object from spreadsheet document", e) ;
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log) ;
            throw new StatusException(
                "Error getting test object from spreadsheet document", e) ;
        }

        TestEnvironment tEnv = new TestEnvironment( oObj );

        return tEnv;
    }

}


