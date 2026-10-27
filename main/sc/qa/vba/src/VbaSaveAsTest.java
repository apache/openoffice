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

import org.junit.Test;
import org.junit.Before;

import java.io.File;

import static org.junit.Assert.*;

/**
 * P2: Tests for VBA Workbook.SaveAs API.
 *
 * Tests the following VBA object model features:
 * - ThisWorkbook.SaveAs with xlCSV format
 * - ThisWorkbook.SaveAs with xlExcel9795 format
 * - File creation verification after SaveAs
 * - SaveAs with overwrite behavior (second save to same path)
 * - SaveAs with default format (no explicit format parameter)
 *
 * Reference Excel constants:
 * - xlCSV = 6
 * - xlExcel9795 = 43
 *
 * Test methodology: BASIC macros call SaveAs to save the document to a
 * temp path, then Java verifies the file exists on disk. Uses temp
 * directory created by Java for deterministic file operations.
 *
 * Result columns:
 * - Z column (index 25): numeric results (1 = success)
 * - AA column (index 26): string results (file paths)
 */
public class VbaSaveAsTest extends VbaCompatTest {

    private static final String MODULE_NAME = "SaveAsTests";

    // xlFileFormat constants
    private static final int XL_CSV = 6;
    private static final int XL_EXCEL_97_95 = 43;

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +

        "'@TestMethod\n" +
        "Sub TestSaveAsCSV(tempPath As String)\n" +
        "    ' Arrange\n" +
        "    Dim wb As Object\n" +
        "    Set wb = ThisWorkbook\n" +
        "\n" +
        "    ' Act\n" +
        "    wb.SaveAs tempPath, " + XL_CSV + "\n" +
        "\n" +
        "    ' Assert - write result\n" +
        "    Range(\"Z1\").Value = 1\n" +
        "    Range(\"AA1\").Value = tempPath\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestSaveAsExcel9795(tempPath As String)\n" +
        "    ' Arrange\n" +
        "    Dim wb As Object\n" +
        "    Set wb = ThisWorkbook\n" +
        "\n" +
        "    ' Act\n" +
        "    wb.SaveAs tempPath, " + XL_EXCEL_97_95 + "\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = 1\n" +
        "    Range(\"AA1\").Value = tempPath\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestSaveAsDefault(tempPath As String)\n" +
        "    ' Arrange\n" +
        "    Dim wb As Object\n" +
        "    Set wb = ThisWorkbook\n" +
        "\n" +
        "    ' Act\n" +
        "    wb.SaveAs tempPath\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = 1\n" +
        "    Range(\"AA1\").Value = tempPath\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestSaveAsOverwrite(tempPath As String)\n" +
        "    ' Arrange\n" +
        "    Dim wb As Object\n" +
        "    Set wb = ThisWorkbook\n" +
        "\n" +
        "    ' Act - first save\n" +
        "    wb.SaveAs tempPath\n" +
        "    Range(\"Z1\").Value = 1\n" +
        "\n" +
        "    ' Act - second save (overwrite)\n" +
        "    wb.SaveAs tempPath\n" +
        "    Range(\"Z2\").Value = 1\n" +
        "End Sub\n";

    private String tempFileDir;

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
        tempFileDir = getTempDirPath();
    }

    @Test
    public void testSaveAsCSV() throws Exception {
        clearResultRow();
        String tempPath = tempFileDir + File.separator + "test_saveas_csv.xls";
        runMacroWithArg(MODULE_NAME, "TestSaveAsCSV", tempPath);

        double result = getNumericResult(0);
        assertEquals("SaveAs CSV should complete and set result to 1", 1.0, result, 0.001);

        File savedFile = new File(tempPath);
        assertTrue("Saved file should exist on disk", savedFile.exists());
        assertTrue("Saved file should not be empty", savedFile.length() > 0);

        savedFile.delete();
    }

    @Test
    public void testSaveAsExcel9795() throws Exception {
        clearResultRow();
        String tempPath = tempFileDir + File.separator + "test_saveas_9795.xls";
        runMacroWithArg(MODULE_NAME, "TestSaveAsExcel9795", tempPath);

        double result = getNumericResult(0);
        assertEquals("SaveAs Excel9795 should complete and set result to 1", 1.0, result, 0.001);

        File savedFile = new File(tempPath);
        assertTrue("Saved file should exist on disk", savedFile.exists());
        savedFile.delete();
    }

    @Test
    public void testSaveAsDefault() throws Exception {
        clearResultRow();
        String tempPath = tempFileDir + File.separator + "test_saveas_default.xls";
        runMacroWithArg(MODULE_NAME, "TestSaveAsDefault", tempPath);

        double result = getNumericResult(0);
        assertEquals("SaveAs default format should complete", 1.0, result, 0.001);

        File savedFile = new File(tempPath);
        assertTrue("Saved file should exist on disk", savedFile.exists());
        savedFile.delete();
    }

    @Test
    public void testSaveAsOverwrite() throws Exception {
        clearResultRow();
        String tempPath = tempFileDir + File.separator + "test_saveas_overwrite.ods";
        runMacroWithArg(MODULE_NAME, "TestSaveAsOverwrite", tempPath);

        double firstSave = getNumericResult(0);
        assertEquals("First SaveAs should succeed", 1.0, firstSave, 0.001);

        double secondSave = getNumericResult(1);
        assertEquals("Second SaveAs (overwrite) should succeed", 1.0, secondSave, 0.001);

        File savedFile = new File(tempPath);
        assertTrue("Saved file should exist on disk", savedFile.exists());
        savedFile.delete();
    }

    private void runMacroWithArg(String moduleName, String functionName, String arg) throws Exception {
        com.sun.star.script.provider.XScriptProviderSupplier scriptSupplier =
            com.sun.star.uno.UnoRuntime.queryInterface(
                com.sun.star.script.provider.XScriptProviderSupplier.class, documentModel);
        com.sun.star.script.provider.XScriptProvider provider = scriptSupplier.getScriptProvider();

        String scriptURI = "vnd.sun.star.script:Standard." + moduleName + "." + functionName
                         + "?language=Basic&location=document";

        com.sun.star.script.provider.XScript script = provider.getScript(scriptURI);
        assertNotNull(script);

        short[][] outIndices = new short[1][];
        Object[][] outParams = new Object[1][];

        script.invoke(new Object[]{arg}, outIndices, outParams);
    }
}
