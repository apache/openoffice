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

package org.openoffice.sc.qa.vba;

import com.sun.star.beans.PropertyValue;
import com.sun.star.document.XEmbeddedScripts;
import com.sun.star.frame.XComponentLoader;
import com.sun.star.frame.XModel;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.script.provider.XScript;
import com.sun.star.script.provider.XScriptProvider;
import com.sun.star.script.provider.XScriptProviderSupplier;
import com.sun.star.script.vba.XVBACompatibility;
import com.sun.star.script.XStarBasicAccess;
import com.sun.star.sheet.XCell;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.table.XCellRange;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;

import org.openoffice.test.OfficeConnection;

import static org.junit.Assert.*;

/**
 * Base class for VBA compatibility tests.
 *
 * Provides infrastructure for:
 * - Connecting to a headless AOO instance with VBA support
 * - Creating a calc document with VBA compatibility mode enabled
 * - Inserting BASIC test modules with VBA code
 * - Executing BASIC macros via XScriptProvider
 * - Reading back cell values for verification
 */
public abstract class VbaCompatTest {

    protected final OfficeConnection connection = new OfficeConnection();

    protected XComponentContext context;
    protected XMultiServiceFactory serviceFactory;
    protected XSpreadsheetDocument document;
    protected XModel documentModel;

    @org.junit.Before
    public void setUp() throws Exception {
        connection.setUp();
        context = connection.getComponentContext();
        serviceFactory = UnoRuntime.queryInterface(
            XMultiServiceFactory.class, context.getServiceManager());
        createDocument();
    }

    @org.junit.After
    public void tearDown() throws Exception {
        if (documentModel != null) {
            try {
                com.sun.star.util.XCloseable closeable =
                    UnoRuntime.queryInterface(com.sun.star.util.XCloseable.class, documentModel);
                if (closeable != null) {
                    closeable.close(true);
                }
            } catch (Exception e) {
                // ignore close errors during teardown
            }
        }
        connection.tearDown();
    }

    /**
     * Creates a new Calc document with VBA compatibility mode enabled.
     */
    protected void createDocument() throws Exception {
        XComponentLoader loader = UnoRuntime.queryInterface(
            XComponentLoader.class,
            serviceFactory.createInstanceWithContext(
                "com.sun.star.frame.Desktop", context));
        assertNotNull("Failed to get XComponentLoader", loader);

        document = UnoRuntime.queryInterface(
            XSpreadsheetDocument.class,
            loader.loadComponentFromURL(
                "private:factory/scalc", "_blank", 0, new PropertyValue[]{}));
        assertNotNull("Failed to create spreadsheet document", document);
        documentModel = UnoRuntime.queryInterface(XModel.class, document);

        // Enable VBA compatibility mode
        XEmbeddedScripts embeddedScripts = UnoRuntime.queryInterface(
            XEmbeddedScripts.class, documentModel);
        assertNotNull("Document does not support XEmbeddedScripts", embeddedScripts);

        com.sun.star.script.XStorageBasedLibraryContainer basicLibs =
            embeddedScripts.getBasicLibraries();
        assertNotNull("Failed to get BasicLibraries", basicLibs);

        XVBACompatibility vbaCompat = UnoRuntime.queryInterface(
            XVBACompatibility.class, basicLibs);
        assertNotNull("BasicLibraries does not support XVBACompatibility", vbaCompat);
        vbaCompat.setVBACompatibilityMode(true);

        // Create the "Standard" library if it doesn't exist
        com.sun.star.script.XLibraryContainer libContainer = UnoRuntime.queryInterface(
            com.sun.star.script.XLibraryContainer.class, basicLibs);
        if (libContainer != null && !libContainer.hasByName("Standard")) {
            libContainer.createLibrary("Standard");
        }
    }

    /**
     * Inserts a BASIC module into the "Standard" library of the current document.
     *
     * @param moduleName the name of the module (e.g., "FormatConditionTests")
     * @param sourceCode the BASIC source code
     */
    protected void addBasicModule(String moduleName, String sourceCode) throws Exception {
        XStarBasicAccess basicAccess = UnoRuntime.queryInterface(
            XStarBasicAccess.class, documentModel);
        assertNotNull("Document model does not support XStarBasicAccess", basicAccess);

        // Create Standard library if needed
        try {
            basicAccess.createLibrary("Standard", "", "", "");
        } catch (com.sun.star.container.ElementExistException e) {
            // Library already exists
        }

        basicAccess.addModule("Standard", moduleName, "StarBasic", sourceCode);
    }

    /**
     * Executes a BASIC macro in the "Standard" library.
     *
     * @param moduleName the module name (e.g., "FormatConditionTests")
     * @param functionName the sub/function name to execute
     * @return the return value of the macro, or null for Subs
     */
    protected Object runMacro(String moduleName, String functionName) throws Exception {
        XScriptProviderSupplier scriptSupplier = UnoRuntime.queryInterface(
            XScriptProviderSupplier.class, documentModel);
        assertNotNull("Document does not support XScriptProviderSupplier", scriptSupplier);

        XScriptProvider provider = scriptSupplier.getScriptProvider();
        assertNotNull("Failed to get script provider", provider);

        String scriptURI = "vnd.sun.star.script:Standard." + moduleName + "." + functionName
                         + "?language=Basic&location=document";

        XScript script = provider.getScript(scriptURI);
        assertNotNull("Failed to get script for URI: " + scriptURI, script);

        // UNO Java bridge uses double-array holders for [out] sequence parameters
        short[][] outIndices = new short[1][];
        Object[][] outParams = new Object[1][];

        try {
            return script.invoke(new Object[]{}, outIndices, outParams);
        } catch (com.sun.star.reflection.InvocationTargetException e) {
            throw new RuntimeException("Script invocation failed", e);
        }
    }

    /**
     * Gets a cell from the first sheet by position (column, row).
     */
    protected XCell getCell(int column, int row) throws Exception {
        XSpreadsheets sheets = document.getSheets();
        XSpreadsheet sheet = UnoRuntime.queryInterface(
            XSpreadsheet.class, sheets.getByIndex(0));
        return sheet.getCellByPosition(column, row);
    }

    /**
     * Gets the active sheet's range as a string address (e.g., "A1:B10").
     */
    protected XCellRange getRangeByAddress(String address) throws Exception {
        XSpreadsheets sheets = document.getSheets();
        XSpreadsheet sheet = UnoRuntime.queryInterface(
            XSpreadsheet.class, sheets.getByIndex(0));
        return sheet.getCellRangeByName(address);
    }

    /**
     * Reads the value written to cell A1 by a BASIC test macro.
     * This is the standard result channel for BASIC tests.
     */
    protected double getResultAsDouble() throws Exception {
        return getCell(0, 0).getValue();
    }

    /**
     * Reads the string value written to cell A1 by a BASIC test macro.
     */
    protected String getResultAsString() throws Exception {
        return getCell(0, 0).getString();
    }

    /**
     * Reads the numeric result from cell B1.
     */
    protected double getNumericResultB1() throws Exception {
        return getCell(1, 0).getValue();
    }

    /**
     * Reads the string result from cell B1.
     */
    protected String getStringResultB1() throws Exception {
        return getCell(1, 0).getString();
    }

    /**
     * Writes a value to cell A1 (for communication to BASIC macros if needed).
     */
    protected void writeValue(int column, int row, double value) throws Exception {
        getCell(column, row).setValue(value);
    }

    /**
     * Writes a string to cell A1 (for communication to BASIC macros if needed).
     */
    protected void writeString(int column, int row, String value) throws Exception {
        getCell(column, row).setString(value);
    }

    /**
     * Clears the first row of the active sheet.
     */
    protected void clearResultRow() throws Exception {
        for (int c = 0; c < 10; c++) {
            getCell(c, 0).setString("");
            getCell(c, 0).setValue(0.0);
        }
    }
}
