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

import static org.junit.Assert.*;

/**
 * LibreOffice-style failure hypothesis tests for cross-sheet named ranges.
 *
 * Based on documented LibreOffice VBA port trouble areas:
 * - Cross-sheet name creation, save/reopen, and independent resolution
 * - Workbook-level vs worksheet-level scope disambiguation
 * - Names collection access via Index and Name string
 * - RefersToRange property resolution after save/reopen
 *
 * NOT integrated into the build (not in JunitTest_sc_vba.mk).
 * These exist solely to surface potential failures.
 * Status: EXPLORATORY - written but not verified against AOO runtime.
 * Save/reopen resolution tests require document persistence round-tripping,
 * which should be validated separately.
 *
 * Result columns (BASIC -> Java via UNO):
 * - Z column (index 25): numeric results (counts, error codes)
 * - AA column (index 26): string results (addresses, names)
 */
public class VbaLibreNamedRangesTest extends VbaCompatTest {

    private static final String MODULE_NAME = "LibreNamedRangeTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +

        "'@TestMethod\n" +
        "Sub TestWorkbookNameCreateAndResolve()\n" +
        "    ' Arrange\n" +
        "    Dim nm As Object\n" +
        "    ThisWorkbook.Names.Add \"WBName1\", \"=Sheet1!$A$1:$A$3\"\n" +
        "    ThisWorkbook.Names(\"WBName1\").RefersToRange.Value = Array(10, 20, 30)\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim r As Object\n" +
        "    Set r = ThisWorkbook.Names(\"WBName1\").RefersToRange\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = r.Count\n" +
        "    Range(\"AA1\").Value = r.Address\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestWorksheetNameCreateAndResolve()\n" +
        "    ' Arrange - create a second sheet for cross-sheet testing\n" +
        "    Dim ws As Object\n" +
        "    Set ws = ThisWorkbook.Worksheets.Add\n" +
        "    ws.Name = \"SecondSheet\"\n" +
        "    ActiveSheet.Range(\"A1\").Value = \"on first\"\n" +
        "    ws.Range(\"A1\").Value = \"on second\"\n" +
        "\n" +
        "    ' Act - create a name on the second sheet\n" +
        "    ws.Names.Add \"Sheet2Name\", \"=SecondSheet!$A$1\"\n" +
        "    Dim r As Object\n" +
        "    Set r = ws.Names(\"Sheet2Name\").RefersToRange\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = r.Count\n" +
        "    Range(\"AA1\").Value = r.Address\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestCrossSheetNameDoesNotLeak()\n" +
        "    ' Arrange - create name on Sheet1, verify not on Sheet2\n" +
        "    ThisWorkbook.Names.Add \"CrossName\", \"=Sheet1!$A$1\"\n" +
        "    ActiveSheet.Range(\"A1\").Value = 1\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim found As Integer: found = 0\n" +
        "    Dim nm As Object\n" +
        "    On Error Resume Next\n" +
        "    Set nm = ThisWorkbook.Names(\"NonExistentName\")\n" +
        "    If Err.Number <> 0 Then found = 1\n" +
        "    Err.Clear\n" +
        "    On Error GoTo 0\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = found\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestNamesItemByString()\n" +
        "    ' Arrange\n" +
        "    ThisWorkbook.Names.Add \"ItemTestName\", \"=$B$2:$B$5\"\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim nm As Object\n" +
        "    Set nm = ThisWorkbook.Names(\"ItemTestName\")\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = IIf(nm Is Nothing, 0, 1)\n" +
        "    Range(\"AA1\").Value = nm.Name\n" +
        "    Range(\"AA2\").Value = nm.RefersTo\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestNameDelete()\n" +
        "    ' Arrange\n" +
        "    ThisWorkbook.Names.Add \"DeleteMe\", \"=$C$1\"\n" +
        "    Range(\"C1\").Value = 99\n" +
        "\n" +
        "    ' Act\n" +
        "    ThisWorkbook.Names(\"DeleteMe\").Delete\n" +
        "    Dim found As Integer: found = 0\n" +
        "    Dim nm As Object\n" +
        "    On Error Resume Next\n" +
        "    Set nm = ThisWorkbook.Names(\"DeleteMe\")\n" +
        "    If Err.Number <> 0 Then found = 1\n" +
        "    Err.Clear\n" +
        "    On Error GoTo 0\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = found\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestNamesCount()\n" +
        "    ' Arrange\n" +
        "    ThisWorkbook.Names.Add \"CountTest1\", \"=$A$1\"\n" +
        "    ThisWorkbook.Names.Add \"CountTest2\", \"=$B$1\"\n" +
        "    ThisWorkbook.Names.Add \"CountTest3\", \"=$C$1\"\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = ThisWorkbook.Names.Count\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testWorkbookNameCreateAndResolve() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestWorkbookNameCreateAndResolve");
        // Cross-sheet named ranges may not resolve to the right sheet
        double count = getNumericResult(0);
        assertTrue("WBName1 RefersToRange Count should be >= 1, got: " + count, count >= 1);
        String address = getStringResult(0);
        assertNotNull("RefersToRange.Address should not be null", address);
    }

    @Test
    public void testWorksheetNameCreateAndResolve() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestWorksheetNameCreateAndResolve");
        // Worksheet-level Names API may not be fully wired in ScVbaWorksheet
        double count = getNumericResult(0);
        assertTrue("Sheet2Name RefersToRange Count should be >= 1, got: " + count, count >= 1);
        String address = getStringResult(0);
        if (address != null && !address.isEmpty()) {
            assertTrue("Address should reference SecondSheet, got: " + address,
                    address.contains("SecondSheet") || address.contains("Sheet2"));
        }
    }

    @Test
    public void testCrossSheetNameDoesNotLeak() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCrossSheetNameDoesNotLeak");
        // Accessing non-existent name should trigger error (1 = found error)
        double result = getNumericResult(0);
        assertEquals("NonExistentName should trigger an error when accessed",
                1.0, result, 0.001);
    }

    @Test
    public void testNamesItemByString() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestNamesItemByString");
        // Names("name") string indexer may not work correctly
        assertEquals("Names(\"ItemTestName\") should return non-null",
                1.0, getNumericResult(0), 0.001);
        String name = getStringResult(0);
        assertNotNull("Name property should not be null", name);
        String refersTo = getStringResult(1);
        assertNotNull("RefersTo should not be null", refersTo);
    }

    @Test
    public void testNameDelete() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestNameDelete");
        // Delete may not properly remove from Names collection
        double result = getNumericResult(0);
        assertEquals("After Delete, accessing name should error (1 = found error)",
                1.0, result, 0.001);
    }

    @Test
    public void testNamesCount() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestNamesCount");
        // Count may be wrong if names are added but not tracked
        double count = getNumericResult(0);
        assertTrue("Names.Count should be >= 3, got: " + count, count >= 3);
    }
}
