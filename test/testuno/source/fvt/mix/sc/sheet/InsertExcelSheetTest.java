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

package fvt.mix.sc.sheet;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;

import org.openoffice.test.common.Logger;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XIndexAccess;
import com.sun.star.frame.XDesktop;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.SheetLinkMode;
import com.sun.star.sheet.XSheetLinkable;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.XRefreshable;

import org.openoffice.test.OpenOffice;
import org.openoffice.test.vcl.widgets.VclApp;
import org.openoffice.test.vcl.widgets.VclMessageBox;
import org.openoffice.test.vcl.widgets.VclButton;
import org.openoffice.test.vcl.widgets.VclComboBox;
import org.openoffice.test.vcl.widgets.VclListBox;
import org.openoffice.test.vcl.widgets.VclDialog;
import org.openoffice.test.vcl.client.Constant;
import static org.openoffice.test.vcl.Tester.sleep;

import org.openoffice.test.vcl.client.VclHookException;

import testlib.uno.SCUtil;

/**
 * test inserting linked and non-linked sheets from an XLS spreadsheet.
 */
public class InsertExcelSheetTest {

    OpenOffice aoo;
    UnoApp unoApp;
    VclApp vclApp;
    VclMessageBox activeMsgBox;
    VclButton scAfterCurrentSheet;
    VclButton scNewSheetFromFile;
    VclButton scFromFileLink;
    VclButton scFromFileBrowse;
    VclComboBox filePickerPath;
    VclButton filePickerOpen;
    VclListBox scFromFileSheetList;
    VclDialog scInsertSheetDlg;
    VclDialog scSelectSheetsDlg;
    VclListBox scSheetsList;

    XSpreadsheetDocument scDocument = null;
    XComponent scComponent = null;

    @Rule
    public Logger log = Logger.getLogger(this);


    @BeforeClass
    public static void setUpBeforeClass() throws Exception {

    }

    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {

        OpenOffice aoo = OpenOffice.getDefault();
        unoApp = new UnoApp(aoo);
        vclApp = new VclApp(aoo);
        scAfterCurrentSheet = new VclButton(vclApp, "sc:RadioButton:RID_SCDLG_INSERT_TABLE:RB_BEHIND");
        scNewSheetFromFile = new VclButton(vclApp, "sc:RadioButton:RID_SCDLG_INSERT_TABLE:RB_FROMFILE");
        scFromFileLink = new VclButton(vclApp, "sc:CheckBox:RID_SCDLG_INSERT_TABLE:CB_LINK");
        scFromFileBrowse = new VclButton(vclApp, "sc:PushButton:RID_SCDLG_INSERT_TABLE:BTN_BROWSE");
        filePickerPath = new VclComboBox(vclApp, "SVT_HID_FILEDLG_AUTOCOMPLETEBOX");
        filePickerOpen = new VclButton(vclApp, "fpicker:PushButton:DLG_SVT_EXPLORERFILE:BTN_EXPLORERFILE_OPEN");
        scFromFileSheetList = new VclListBox(vclApp, "sc:MultiListBox:RID_SCDLG_INSERT_TABLE:LB_TABLES");
        scInsertSheetDlg = new VclDialog(vclApp, "sc:ModalDialog:RID_SCDLG_INSERT_TABLE");
        scSelectSheetsDlg = new VclDialog(vclApp, ".uno:SelectTables");
        scSheetsList = new VclListBox(vclApp, "SC_HID_SELECTTABLES");

        // existing setup
        unoApp.start();
        // New a SC document
        scComponent = unoApp.newDocument("scalc");

        // New a document source.xls, add value to 3 sheet
        scDocument = SCUtil.getSCDocument(scComponent);
        XSpreadsheets spreadsheets = scDocument.getSheets();
        XSpreadsheet firstSheet = SCUtil.getSCSheetByIndex(scDocument,
                (short) 0);
        XSpreadsheet secondSheet = SCUtil.getSCSheetByIndex(scDocument,
                (short) 1);
        XSpreadsheet thirdSheet = SCUtil.getSCSheetByIndex(scDocument,
                (short) 2);
        SCUtil.setFormulaToCell(firstSheet, 1, 2, "=1*2"); // cell B3
        SCUtil.setFormulaToCell(secondSheet, 1, 2, "=2*2");
        SCUtil.setFormulaToCell(thirdSheet, 1, 2, "=3*2");

        // Save and close this document
        SCUtil.saveFileAs(scComponent, "source", "xls");
        SCUtil.closeFile(scDocument);

    }

    @After
    public void tearDown() throws Exception {
        unoApp.closeDocument(scComponent);
        unoApp.close();
    }

    /**
     * test insert sheet from other file with a normal link meaning the cell
     * will return the formula and text values like the source and the link
     * will update the cell.
     */
    @Test()
    public void insertSheetFromFileLinkedNormal() throws Exception {

        // get source document URL
        String SourcestoreUrl = Testspace.getUrl("output/sc/" + "source" + "."
                + "xls");

        // New a document
        scComponent = unoApp.newDocument("scalc");
        scDocument = SCUtil.getSCDocument(scComponent);
        XSpreadsheets spreadsheets = scDocument.getSheets();

        // Insert firstexternalsheet sheet, link with Sheet1 in source document
        // and the link mode is NORMAL

        // using VCL
        boolean retry = false;
        int tryCount = 0;
        int maxTries = 5;
        do {
            tryCount++; // starts at 1
            retry = false;
            try {
                vclApp.dispatch(".uno:SelectTables");
            } catch (java.lang.Exception e) {
                // e.printStackTrace();
                log.info("Exception " + tryCount + "Caught !");
                retry = true;
                if (tryCount == maxTries) {
                    throw e; // throw the exception
                }
                sleep(1);
            }
        } while ((retry) && (tryCount < maxTries));

        // select sheet3
        scSheetsList.select(2);

        scSelectSheetsDlg.ok();

        // Open Insert Sheet dialog via main menu Insert-> Sheet
        vclApp.dispatch(".uno:Insert");

        // set after current sheet
        scAfterCurrentSheet.setChecked(true);

        // enable new sheet from file
        scNewSheetFromFile.setChecked(true);

        // check Link
        scFromFileLink.setChecked(true);

        // browse
        scFromFileBrowse.click();

        // opens Insert dialog

        // add filename
        filePickerPath.setText(SourcestoreUrl);

        // Insert button
        filePickerOpen.click();

        // back to Insert Sheet dialog

        // might have to select from
        scFromFileSheetList.select(0);

        // select Insert button
        scInsertSheetDlg.ok();

        log.info("sheet inserted...");

        activeMsgBox = new VclMessageBox(vclApp, Constant.UID_ACTIVE);
        boolean msgExists = activeMsgBox.exists(1); // wait 1 second for the dialog
        if (msgExists) {
            log.info("got the update links dialog and accepting");
            activeMsgBox.yes(); // yes dialog
        } else {
            log.info("missed the update links dialog");
        }
        XSpreadsheet firstexternalsheet = SCUtil.getSCSheetByIndex(scDocument,
                (short) 3);
        XSheetLinkable xfirstSheetLinkable = (XSheetLinkable) UnoRuntime
                .queryInterface(XSheetLinkable.class, firstexternalsheet);

        assertEquals("Expect first sheet link is NORMAL",
                SheetLinkMode.NORMAL, xfirstSheetLinkable.getLinkMode());

        // Verify firstexternalsheet
        assertEquals("Expect first sheet formula should be =1*2", "=1*2",
                SCUtil.getFormulaFromCell(firstexternalsheet, 1, 2));
        assertEquals("Expect first sheet text should be 2", "2",
                SCUtil.getTextFromCell(firstexternalsheet, 1, 2));

        log.info("passed first round of assertions");
        log.info("saving document as linked-normal.ods");

        // save document and verify the linked sheet again
        SCUtil.saveFileAs(scComponent, "linked-normal", "ods");
        log.info("reloading linked-normal.ods...");

        // this will hang on "update links" dialog with no way out - leaving here for others that follow...
        // XSpreadsheetDocument tempscDocument = SCUtil.reloadFile(unoApp, scDocument, "linked.ods");

        // open it with VCL instead
        vclApp.dispatch(".uno:Open");
        // add filename
        filePickerPath.setText("linked-normal.ods");
        
        // Insert button
        filePickerOpen.click();
        sleep(1);

        // get the current document as tempscdocument
        XDesktop xDesktop = unoApp.getDesktop();
        // xDesktop.getCurrentComponent returns an XComponent (XModel)
        XComponent xComponent = xDesktop.getCurrentComponent();
        XSpreadsheetDocument tempscDocument = (XSpreadsheetDocument)UnoRuntime.queryInterface(
                XSpreadsheetDocument.class, xComponent);

        scDocument = tempscDocument;
        firstexternalsheet = SCUtil.getSCSheetByIndex(scDocument, (short) 3);

        // Verify firstexternalsheet
        assertEquals("Expect first sheet formula should be =1*2", "=1*2",
                SCUtil.getFormulaFromCell(firstexternalsheet, 1, 2));
        assertEquals("Expect first sheet text should be 2", "2",
                SCUtil.getTextFromCell(firstexternalsheet, 1, 2));

        log.info("passed second round of assertions");

        log.info("saving the document...");
        // save and close document
        SCUtil.save(scDocument);
        SCUtil.closeFile(scDocument);

        log.info("opening source.xls to modify first sheet...");
        // Open source document and change the value in source document
        XSpreadsheetDocument sourcescDocument = SCUtil.reloadFile(unoApp,
                scDocument, "source.xls");
        XSpreadsheet firstSheet = SCUtil.getSCSheetByIndex(sourcescDocument, (short) 0);
        SCUtil.setFormulaToCell(firstSheet, 1, 2, "=1*3");
        log.info("saving and closing source.xls...");
        SCUtil.save(sourcescDocument);
        SCUtil.closeFile(sourcescDocument);

        log.info("opening linked-normal.ods...");
        // open test document
        vclApp.dispatch(".uno:Open");
        // add filename
        filePickerPath.setText("linked-normal.ods");
        
        // Insert button
        filePickerOpen.click();

        msgExists = activeMsgBox.exists(1); // wait 1 second for the dialog
        if (msgExists) {
            log.info("got the update links dialog and accepting");
            activeMsgBox.yes(); // yes dialog
        } else {
            log.info("missed the update links dialog");
        }

        // get the current document as tempscdocument
        xDesktop = unoApp.getDesktop();
        xComponent = xDesktop.getCurrentComponent();
        tempscDocument = (XSpreadsheetDocument)UnoRuntime.queryInterface(
                XSpreadsheetDocument.class, xComponent);

        scDocument = tempscDocument;
        spreadsheets = scDocument.getSheets();

        firstexternalsheet = SCUtil.getSCSheetByIndex(scDocument, (short) 3);

        // get Object SheetLinks for document
        XPropertySet sheetpropertyset = (XPropertySet) UnoRuntime
                .queryInterface(XPropertySet.class, scDocument);
        Object sheetLinks = sheetpropertyset.getPropertyValue("SheetLinks");

        XIndexAccess xsheetlinks = (XIndexAccess) UnoRuntime.queryInterface(
                XIndexAccess.class, sheetLinks);

        // Refresh all links
        log.info("refreshing all links...");
        for (int i = 0; i < xsheetlinks.getCount(); i++) {
            Object sheetlink = xsheetlinks.getByIndex(i);
            XRefreshable xsheetRefreshable = (XRefreshable) UnoRuntime
                    .queryInterface(XRefreshable.class, sheetlink);
            xsheetRefreshable.refresh();
            log.info("in refresh loop " + i);
            msgExists = activeMsgBox.exists(1); // wait 1 second for the dialog
            if (msgExists) {
                log.info("got the update links dialog and accepting");
                activeMsgBox.yes(); // yes dialog
            } else {
                log.info("missed the update links dialog");
            }
        }

        // Verify firstexternalsheet did change
        assertEquals("Expect first sheet formula should be =1*3", "=1*3",
                SCUtil.getFormulaFromCell(firstexternalsheet, 1, 2));
        assertEquals("Expect first sheet text should be 3", "3",
                SCUtil.getTextFromCell(firstexternalsheet, 1, 2));

        // Save the document before close
        SCUtil.save(scDocument);
    }

    /**
     * test insert sheet from other file with a value link, meaning the cell
     * will return the text value for both the formula and the text values and
     * the link will update the cell.
     */
    @Test()
    public void insertSheetFromFileLinkedValue() throws Exception {

        // get source document URL
        String SourcestoreUrl = Testspace.getUrl("output/sc/" + "source" + "."
                + "xls");

        // New a document
        scComponent = unoApp.newDocument("scalc");
        scDocument = SCUtil.getSCDocument(scComponent);
        XSpreadsheets spreadsheets = scDocument.getSheets();

        // Insert firstexternalsheet sheet, link with Sheet2 in source document
        // and the link mode is VALUE

        // using VCL
        boolean retry = false;
        int tryCount = 0;
        int maxTries = 5;
        do {
            tryCount++; // starts at 1
            retry = false;
            try {
                vclApp.dispatch(".uno:SelectTables");
            } catch (java.lang.Exception e) {
                // e.printStackTrace();
                log.info("Exception " + tryCount + "Caught !");
                retry = true;
                if (tryCount == maxTries) {
                    throw e; // throw the exception
                }
                sleep(1);
            }
        } while ((retry) && (tryCount < maxTries));
        
        // select sheet3
        scSheetsList.select(2);

        scSelectSheetsDlg.ok();

        // Open Insert Sheet dialog via main menu Insert-> Sheet
        vclApp.dispatch(".uno:Insert");

        // set after current sheet
        scAfterCurrentSheet.setChecked(true);

        // enable new sheet from file
        scNewSheetFromFile.setChecked(true);

        // check Link
        scFromFileLink.setChecked(true);

        // browse
        scFromFileBrowse.click();

        // opens Insert dialog

        // add filename
        filePickerPath.setText(SourcestoreUrl);

        // Insert button
        filePickerOpen.click();

        // back to Insert Sheet dialog

        // select sheet 2
        scFromFileSheetList.select(1);

        // select Insert button
        scInsertSheetDlg.ok();

        log.info("sheet inserted...");
        
        // check for message box
        activeMsgBox = new VclMessageBox(vclApp, Constant.UID_ACTIVE);
        boolean msgExists = activeMsgBox.exists(1); // wait 1 second for the dialog
        if (msgExists) {
            log.info("got the update links dialog and accepting");
            activeMsgBox.yes(); // yes dialog
        } else {
            log.info("missed the update links dialog");
        }
        XSpreadsheet firstexternalsheet = SCUtil.getSCSheetByIndex(scDocument,
                (short) 3);
        XSheetLinkable xfirstSheetLinkable = (XSheetLinkable) UnoRuntime
                .queryInterface(XSheetLinkable.class, firstexternalsheet);

        // this seem the only way to set this as it's not in the UI dialog
        xfirstSheetLinkable.setLinkMode(SheetLinkMode.VALUE);

        assertEquals("Expect first sheet link is NORMAL",
                SheetLinkMode.VALUE, xfirstSheetLinkable.getLinkMode());

        // Verify firstexternalsheet
        assertEquals("Expect second sheet formula should be 4", "4",
                SCUtil.getFormulaFromCell(firstexternalsheet, 1, 2));
        assertEquals("Expect second sheet text should be 4", "4",
                SCUtil.getTextFromCell(firstexternalsheet, 1, 2));

        log.info("passed first round of assertions...");

        log.info("saving document as linked-value.ods");
        // save document and verify the linked sheet again
        SCUtil.saveFileAs(scComponent, "linked-value", "ods");

        log.info("reloading linked-value.ods...");

        // this will hang on "update links" dialog with no way out - leaving here for others that follow...
        // XSpreadsheetDocument tempscDocument = SCUtil.reloadFile(unoApp, scDocument, "linked.ods");

        // open it with VCL instead
        vclApp.dispatch(".uno:Open");
        // add filename
        filePickerPath.setText("linked-value.ods");
        
        // Insert button
        filePickerOpen.click();
        sleep(1);

        // get the current document as tempscdocument
        XDesktop xDesktop = unoApp.getDesktop();
        XComponent xComponent = xDesktop.getCurrentComponent();
        XSpreadsheetDocument tempscDocument = (XSpreadsheetDocument)UnoRuntime.queryInterface(
                XSpreadsheetDocument.class, xComponent);

        scDocument = tempscDocument;
        firstexternalsheet = SCUtil.getSCSheetByIndex(scDocument, (short) 3);

        // Verify firstexternalsheet
        assertEquals("Expect second sheet formula should be 4", "4",
                SCUtil.getFormulaFromCell(firstexternalsheet, 1, 2));
        assertEquals("Expect second sheet text should be 4", "4",
                SCUtil.getTextFromCell(firstexternalsheet, 1, 2));

        log.info("passed second round of assertions");
        log.info("saving the document...");

        // save and close document
        SCUtil.save(scDocument);
        SCUtil.closeFile(scDocument);

        // Open source document and change the value in source document
        XSpreadsheetDocument sourcescDocument = SCUtil.reloadFile(unoApp,
                scDocument, "source.xls");
        XSpreadsheet secondSheet = SCUtil.getSCSheetByIndex(sourcescDocument, (short) 1);
        SCUtil.setFormulaToCell(secondSheet, 1, 2, "=2*3");
        SCUtil.save(sourcescDocument);
        SCUtil.closeFile(sourcescDocument);

        log.info("opening linked-value.ods...");
        // open test document
        vclApp.dispatch(".uno:Open");
        // add filename
        filePickerPath.setText("linked-value.ods");
        
        // Insert button
        filePickerOpen.click();

        msgExists = activeMsgBox.exists(1); // wait 1 second for the dialog
        if (msgExists) {
            log.info("got the update links dialog and accepting");
            activeMsgBox.yes(); // yes dialog
        } else {
            log.info("missed the dialog");
        }

        // get the current document as tempscdocument
        xDesktop = unoApp.getDesktop();
        xComponent = xDesktop.getCurrentComponent();
        tempscDocument = (XSpreadsheetDocument)UnoRuntime.queryInterface(
                XSpreadsheetDocument.class, xComponent);

        scDocument = tempscDocument;
        spreadsheets = scDocument.getSheets();

        firstexternalsheet = SCUtil.getSCSheetByIndex(scDocument, (short) 3);

        // get Object SheetLinks for document
        XPropertySet sheetpropertyset = (XPropertySet) UnoRuntime
                .queryInterface(XPropertySet.class, scDocument);
        Object sheetLinks = sheetpropertyset.getPropertyValue("SheetLinks");

        XIndexAccess xsheetlinks = (XIndexAccess) UnoRuntime.queryInterface(
                XIndexAccess.class, sheetLinks);

        log.info("refreshing all links...");
        // Refresh all links
        for (int i = 0; i < xsheetlinks.getCount(); i++) {
            Object sheetlink = xsheetlinks.getByIndex(i);
            XRefreshable xsheetRefreshable = (XRefreshable) UnoRuntime
                    .queryInterface(XRefreshable.class, sheetlink);
            xsheetRefreshable.refresh();
            log.info("in refresh loop " + i);
            msgExists = activeMsgBox.exists(1); // wait 1 second for the dialog
            if (msgExists) {
                log.info("got the update links dialog and accepting");
                activeMsgBox.yes(); // yes dialog
            } else {
                log.info("missed the dialog");
            }
        }

        // Verify firstexternalsheet did change
        assertEquals("Expect first sheet formula should be =6", "6",
                SCUtil.getFormulaFromCell(firstexternalsheet, 1, 2));
        assertEquals("Expect first sheet text should be 6", "6",
                SCUtil.getTextFromCell(firstexternalsheet, 1, 2));

        // Save the document before close
        SCUtil.save(scDocument);

    }

    /**
     * test insert sheet from other file with no link meaning the cell will
     * return the formula and text values like the source but the cell will
     * not update.
     */
    @Test()
    public void insertSheetFromFileLinkedNone() throws Exception {

        // get source document URL
        String SourcestoreUrl = Testspace.getUrl("output/sc/" + "source" + "."
                + "xls");

        // New a document
        scComponent = unoApp.newDocument("scalc");
        scDocument = SCUtil.getSCDocument(scComponent);
        XSpreadsheets spreadsheets = scDocument.getSheets();

        // Insert firstexternalsheet sheet, link with Sheet1 in source document
        // and the link mode is NORMAL

        // try with VCL
        boolean retry = false;
        int tryCount = 0;
        int maxTries = 5;
        do {
            tryCount++; // starts at 1
            retry = false;
            try {
                vclApp.dispatch(".uno:SelectTables");
            } catch (java.lang.Exception e) {
                // e.printStackTrace();
                log.info("Exception " + tryCount + "Caught !");
                retry = true;
                if (tryCount == maxTries) {
                    throw e; // throw the exception
                }
                sleep(1);
            }
        } while ((retry) && (tryCount < maxTries));
        
        // select sheet3
        scSheetsList.select(2);

        scSelectSheetsDlg.ok();

        // Open Insert Sheet dialog via main menu Insert-> Sheet
        vclApp.dispatch(".uno:Insert");

        // set after current sheet
        scAfterCurrentSheet.setChecked(true);

        // enable new sheet from file
        scNewSheetFromFile.setChecked(true);

        // check Link
        scFromFileLink.setChecked(false); // no link

        // browse
        scFromFileBrowse.click();

        // opens Insert dialog

        // add filename
        filePickerPath.setText(SourcestoreUrl);

        // Insert button
        filePickerOpen.click();

        // back to Insert Sheet dialog

        // select sheet 3
        scFromFileSheetList.select(2);

        // select Insert button
        scInsertSheetDlg.ok();

        log.info("sheet inserted......");

        XSpreadsheet firstexternalsheet = SCUtil.getSCSheetByIndex(scDocument,
                (short) 3);
        XSheetLinkable xfirstSheetLinkable = (XSheetLinkable) UnoRuntime
                .queryInterface(XSheetLinkable.class, firstexternalsheet);

        assertEquals("Expect first sheet link is NONE",
                SheetLinkMode.NONE, xfirstSheetLinkable.getLinkMode());

        // Verify thirdexternalsheet
        assertEquals("Expect third sheet formula should be =3*2", "=3*2",
                SCUtil.getFormulaFromCell(firstexternalsheet, 1, 2));
        assertEquals("Expect third sheet text should be 6", "6",
                SCUtil.getTextFromCell(firstexternalsheet, 1, 2));

        log.info("passed first round of assertions...");
        log.info("");

        log.info("saving document as linked-none.ods");

        // save document and verify the linked sheet again
        SCUtil.saveFileAs(scComponent, "linked-none", "ods");
        log.info("reloading linked-none.ods...");

        // this will hang on "update links" dialog with no way out - leaving here for others that follow...
        // XSpreadsheetDocument tempscDocument = SCUtil.reloadFile(unoApp, scDocument, "linked.ods");

        // open it with VCL instead
        vclApp.dispatch(".uno:Open");
        // add filename
        filePickerPath.setText("linked-none.ods");
        
        // Insert button
        filePickerOpen.click();
        sleep(1);

        // need to get the current document as tempscdocument
        XDesktop xDesktop = unoApp.getDesktop();
        // xDesktop.getCurrentComponent returns an XComponent (XModel)
        XComponent xComponent = xDesktop.getCurrentComponent();
        XSpreadsheetDocument tempscDocument = (XSpreadsheetDocument)UnoRuntime.queryInterface(
                XSpreadsheetDocument.class, xComponent);

        scDocument = tempscDocument;
        firstexternalsheet = SCUtil.getSCSheetByIndex(scDocument, (short) 3);

        // Verify firstexternalsheet
        assertEquals("Expect first sheet formula should be =3*2", "=3*2",
                SCUtil.getFormulaFromCell(firstexternalsheet, 1, 2));
        assertEquals("Expect first sheet text should be 6", "6",
                SCUtil.getTextFromCell(firstexternalsheet, 1, 2));

        log.info("passed second round of assertions");
        log.info("");

        // save and close document
        SCUtil.save(scDocument);
        SCUtil.closeFile(scDocument);

        // Open source document and change the value in source document
        XSpreadsheetDocument sourcescDocument = SCUtil.reloadFile(unoApp,
                scDocument, "source.xls");
        XSpreadsheet thirdSheet = SCUtil.getSCSheetByIndex(sourcescDocument, (short) 2);
        SCUtil.setFormulaToCell(thirdSheet, 1, 2, "=3*3");
        SCUtil.save(sourcescDocument);
        SCUtil.closeFile(sourcescDocument);

        // open test document
        vclApp.dispatch(".uno:Open");
        // add filename
        filePickerPath.setText("linked-none.ods");
        
        // Insert button
        filePickerOpen.click();
        sleep(1);

        // get the current document as tempscdocument
        xDesktop = unoApp.getDesktop();
        xComponent = xDesktop.getCurrentComponent();
        tempscDocument = (XSpreadsheetDocument)UnoRuntime.queryInterface(
                XSpreadsheetDocument.class, xComponent);

        scDocument = tempscDocument;
        spreadsheets = scDocument.getSheets();

        firstexternalsheet = SCUtil.getSCSheetByIndex(scDocument, (short) 3);

        // get Object SheetLinks for document
        XPropertySet sheetpropertyset = (XPropertySet) UnoRuntime
                .queryInterface(XPropertySet.class, scDocument);
        Object sheetLinks = sheetpropertyset.getPropertyValue("SheetLinks");

        XIndexAccess xsheetlinks = (XIndexAccess) UnoRuntime.queryInterface(
                XIndexAccess.class, sheetLinks);

        log.info("Number of Links should be 0");
        assertEquals("Number of Links should be 0", 0, xsheetlinks.getCount());

        // Verify firstexternalsheet did not change
        assertEquals("Expect first sheet formula should be =3*2", "=3*2",
                SCUtil.getFormulaFromCell(firstexternalsheet, 1, 2));
        assertEquals("Expect first sheet text should be 6", "6",
                SCUtil.getTextFromCell(firstexternalsheet, 1, 2));

        // Save the document before close
        SCUtil.save(scDocument);

    }

} // end class