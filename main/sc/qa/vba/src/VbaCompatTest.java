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
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.script.provider.XScript;
import com.sun.star.script.provider.XScriptProvider;
import com.sun.star.script.provider.XScriptProviderSupplier;
import com.sun.star.script.vba.XVBACompatibility;
import com.sun.star.script.XStarBasicAccess;
import com.sun.star.table.XCell;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.container.XIndexAccess;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.table.XCellRange;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;

import org.openoffice.test.OfficeConnection;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.*;

/**
 * Base class for VBA compatibility tests.
 *
 * Provides infrastructure for:
 * - Connecting to a headless AOO instance with VBA support
 * - Creating a calc document with VBA compatibility mode enabled
 * - Inserting BASIC test modules with VBA code (following Arrange-Act-Assert
 *   convention, with '@TestMethod' markers in comments)
 * - Executing BASIC macros via XScriptProvider
 * - Creating temp files for deterministic FileSearch/FileDialog tests
 *   (using Fakes pattern for intercepting environment variables)
 * - Reading back cell values for verification
 *
 * Result channel convention:
 * - Cell Z1 (column 25, row 0): primary numeric result
 * - Cell Z2 (column 25, row 1): secondary numeric result
 * - Cell AA1 (column 26, row 0): primary string result
 * - Cell AA2 (column 26, row 1): secondary string result
 */
public abstract class VbaCompatTest {

    protected final OfficeConnection connection = new OfficeConnection();

    protected XComponentContext context;
    protected XMultiComponentFactory serviceFactory;
    protected XSpreadsheetDocument document;
    protected XModel documentModel;

    private final List<File> tempFiles = new ArrayList<>();
    private Path tempDir;

    @org.junit.Before
    public void setUp() throws Exception {
        connection.setUp();
        context = connection.getComponentContext();
        serviceFactory = UnoRuntime.queryInterface(
            XMultiComponentFactory.class, context.getServiceManager());
        tempDir = Files.createTempDirectory("aoo_vba_test");
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
        cleanupTempFiles();
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
     * BASIC modules should follow these conventions:
     * - Start with a '@TestModule' comment marker
     * - Each test should have AAA structure (Arrange-Act-Assert)
     * - Use 'Option VBASupport 1' for VBA compatibility
     *
     * @param moduleName the name of the module
     * @param sourceCode the BASIC source code
     */
    protected void addBasicModule(String moduleName, String sourceCode) throws Exception {
        XStarBasicAccess basicAccess = UnoRuntime.queryInterface(
            XStarBasicAccess.class, documentModel);
        assertNotNull("Document model does not support XStarBasicAccess", basicAccess);

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
     * @param functionName the sub name to execute
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
     * Creates a test file in the temp directory and returns its path.
     * Used to set up deterministic FileSearch test fixtures.
     */
    protected File createTempFile(String filename, String content) throws IOException {
        File file = new File(tempDir.toFile(), filename);
        Files.write(file.toPath(), content.getBytes());
        tempFiles.add(file);
        return file;
    }

    /**
     * Creates a subdirectory in the temp directory for FileSearch subfolder tests.
     */
    protected File createTempSubdir(String name) throws IOException {
        File subdir = new File(tempDir.toFile(), name);
        Files.createDirectories(subdir.toPath());
        tempFiles.add(subdir);
        return subdir;
    }

    /**
     * Returns the temp directory path as a string suitable for passing to VBA.
     * On Windows, backslashes are converted to forward slashes or file:// URLs.
     */
    protected String getTempDirPath() {
        return tempDir.toAbsolutePath().toString();
    }

    /**
     * Gets a cell from the first sheet by position (column, row).
     */
    protected XCell getCell(int column, int row) throws Exception {
        XSpreadsheets sheets = document.getSheets();
        XIndexAccess indexAccess = UnoRuntime.queryInterface(
            XIndexAccess.class, sheets);
        XSpreadsheet sheet = UnoRuntime.queryInterface(
            XSpreadsheet.class, indexAccess.getByIndex(0));
        return sheet.getCellByPosition(column, row);
    }

    /**
     * Gets the active sheet's range by name (e.g., "A1:B10").
     */
    protected XCellRange getRangeByAddress(String address) throws Exception {
        XSpreadsheets sheets = document.getSheets();
        XIndexAccess indexAccess = UnoRuntime.queryInterface(
            XIndexAccess.class, sheets);
        XSpreadsheet sheet = UnoRuntime.queryInterface(
            XSpreadsheet.class, indexAccess.getByIndex(0));
        return sheet.getCellRangeByName(address);
    }

    /**
     * Clears the result row (row 0, columns 0-10).
     * Called before each test to ensure clean results.
     */
    protected void clearResultRow() throws Exception {
        for (int c = 0; c < 10; c++) {
            getCell(c, 0).setFormula("");
            getCell(c, 0).setValue(0.0);
        }
        for (int c = 24; c < 30; c++) {
            getCell(c, 0).setFormula("");
            getCell(c, 0).setValue(0.0);
            getCell(c, 1).setFormula("");
            getCell(c, 1).setValue(0.0);
        }
    }

    /**
     * Reads a numeric result from the Z column (column 25).
     */
    protected double getNumericResult(int row) throws Exception {
        return getCell(25, row).getValue();
    }

    /**
     * Reads a string result from the AA column (column 26).
     */
    protected String getStringResult(int row) throws Exception {
        return getCell(26, row).getFormula();
    }

    /**
     * Reads the numeric result from cell Z1 (column 25, row 0).
     */
    protected double getResultAsDouble() throws Exception {
        return getCell(25, 0).getValue();
    }

    /**
     * Reads the string result from cell AA1 (column 26, row 0).
     */
    protected String getResultAsString() throws Exception {
        return getCell(26, 0).getFormula();
    }

    private void cleanupTempFiles() {
        for (File f : tempFiles) {
            try {
                if (f.isDirectory()) {
                    deleteDirectory(f);
                } else {
                    f.delete();
                }
            } catch (Exception e) {
                // ignore cleanup errors
            }
        }
        try {
            if (tempDir != null) {
                deleteDirectory(tempDir.toFile());
            }
        } catch (Exception e) {
            // ignore
        }
    }

    private static void deleteDirectory(File dir) {
        File[] files = dir.listFiles();
        if (files != null) {
            for (File f : files) {
                if (f.isDirectory()) {
                    deleteDirectory(f);
                } else {
                    f.delete();
                }
            }
        }
        dir.delete();
    }
}
