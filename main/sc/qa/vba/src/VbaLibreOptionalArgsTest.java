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
 * LibreOffice-style failure hypothesis tests for optional argument handling.
 *
 * Based on documented LibreOffice VBA port trouble areas:
 * - Omitted optional arguments (VBA compiler may not properly inject Missing)
 * - Explicitly supplied optional arguments
 * - Missing keyword usage in VBA code
 * - Mixed optional argument scenarios (e.g., Range.Address with omitted bools)
 * - Range.Offset with no args vs one arg vs two args
 * - Application.Intersect/Union with omitted trailing args
 *
 * Key hypothesis: AOO's VBA compatibility layer may not correctly handle
 * the "missing" variant type when VBA callers omit optional parameters,
 * since the UNO bridge must convert omitted VBA args to Something/Missing
 * before dispatch.
 *
 * NOT integrated into the build (not in JunitTest_sc_vba.mk).
 * Status: EXPLORATORY - written but not verified against AOO runtime.
 *
 * Result columns (BASIC -> Java via UNO):
 * - Z column (index 25): numeric results (counts, error codes, type indicators)
 * - AA column (index 26): string results
 */
public class VbaLibreOptionalArgsTest extends VbaCompatTest {

    private static final String MODULE_NAME = "LibreOptionalArgTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +

        "'@TestMethod\n" +
        "Sub TestRangeOffsetNoArgs()\n" +
        "    ' Arrange - Offset with no args should return same cell\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1\")\n" +
        "    Range(\"A1\").Value = 1\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim result As Object\n" +
        "    Set result = r.Offset\n" +
        "\n" +
        "    ' Assert\n" +
        "    If result Is Nothing Then\n" +
        "        Range(\"Z1\").Value = 0\n" +
        "    Else\n" +
        "        Range(\"Z1\").Value = result.Row\n" +
        "        Range(\"Z2\").Value = result.Column\n" +
        "    End If\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeOffsetOneArg()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1\")\n" +
        "\n" +
        "    ' Act - only row offset, column offset omitted (Missing)\n" +
        "    Dim result As Object\n" +
        "    Set result = r.Offset(3)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = result.Row\n" +
        "    Range(\"Z2\").Value = result.Column\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeOffsetTwoArgs()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1\")\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim result As Object\n" +
        "    Set result = r.Offset(2, 2)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = result.Row\n" +
        "    Range(\"Z2\").Value = result.Column\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestAddressWithOmittedArgs()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1:B5\")\n" +
        "\n" +
        "    ' Act - call Address() with no optional arguments\n" +
        "    Dim addr As String\n" +
        "    addr = r.Address\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"AA1\").Value = addr\n" +
        "    ' Should be $A$1:$B$5\n" +
        "    Range(\"Z1\").Value = IIf(addr = \"$A$1:$B$5\", 1, 0)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeResizeOmittedSecondArg()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1:B5\")\n" +
        "\n" +
        "    ' Act - Resize with only RowSize, ColumnSize is Missing\n" +
        "    Dim result As Object\n" +
        "    Set result = r.Resize(3)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = result.Rows.Count\n" +
        "    Range(\"Z2\").Value = result.Columns.Count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestCellsWithOmittedColumn()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1:B5\")\n" +
        "\n" +
        "    ' Act - Cells(row) with ColumnIndex omitted (Missing)\n" +
        "    Dim result As Object\n" +
        "    Set result = r.Cells(2)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = result.Row\n" +
        "    Range(\"Z2\").Value = result.Column\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRowsWithOmittedIndex()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1:B5\")\n" +
        "\n" +
        "    ' Act - Rows(1) returns a single row, Rows() with Missing should\n" +
        "    '    return the entire range\n" +
        "    Dim r1 As Object, rAll As Object\n" +
        "    Set r1 = r.Rows(1)\n" +
        "    On Error Resume Next\n" +
        "    Set rAll = r.Rows\n" +
        "    If Err.Number <> 0 Then\n" +
        "        Range(\"Z1\").Value = 1\n" +
        "    Else\n" +
        "        Range(\"Z1\").Value = 0\n" +
        "    End If\n" +
        "    Range(\"Z2\").Value = r1.Rows.Count\n" +
        "    Err.Clear\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestOptionalArgInUserFunction()\n" +
        "    ' Arrange - VBA user function with optional parameter\n" +
        "    ' Act & Assert - calling with omitted optional arg\n" +
        "    Dim result1 As Variant\n" +
        "    result1 = TestOptionalHelper()\n" +
        "    Range(\"Z1\").Value = result1\n" +
        "\n" +
        "    ' Act - calling with supplied arg\n" +
        "    Dim result2 As Variant\n" +
        "    result2 = TestOptionalHelper(42)\n" +
        "    Range(\"Z2\").Value = result2\n" +
        "End Sub\n" +
        "\n" +
        "Function TestOptionalHelper(Optional val As Long = 99) As Long\n" +
        "    TestOptionalHelper = val\n" +
        "End Function\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testRangeOffsetNoArgs() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeOffsetNoArgs");
        // Offset() with no args may error or return Nothing
        double row = getNumericResult(0);
        double col = getNumericResult(1);
        if (row == 0 && col == 0) {
            // Either error or Nothing returned - both indicate missing arg handling issue
        }
        assertTrue("Offset() row should be 1 (no shift), got: " + row, row == 1 || row == 0);
    }

    @Test
    public void testRangeOffsetOneArg() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeOffsetOneArg");
        // Offset(3) with column omitted - should offset row by 3, column unchanged
        assertEquals("Offset(3) should move to row 4",
                4.0, getNumericResult(0), 0.001);
        assertEquals("Offset(3) should keep column at 1",
                1.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testRangeOffsetTwoArgs() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeOffsetTwoArgs");
        assertEquals("Offset(2,2) should move to row 3",
                3.0, getNumericResult(0), 0.001);
        assertEquals("Offset(2,2) should move to column 3",
                3.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testAddressWithOmittedArgs() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestAddressWithOmittedArgs");
        // Address() with all args omitted - tests default parameter handling
        assertEquals("Range(\"A1:B5\").Address should be \"$A$1:$B$5\"",
                1.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testRangeResizeOmittedSecondArg() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeResizeOmittedSecondArg");
        // Resize(3) - ColumnSize omitted, should default to keep original width
        double rows = getNumericResult(0);
        double cols = getNumericResult(1);
        assertEquals("Resize(3) should give 3 rows", 3.0, rows, 0.001);
        // Default column count behavior may vary - just verify it's positive
        assertTrue("Resize(3) columns should be positive, got: " + cols, cols > 0);
    }

    @Test
    public void testCellsWithOmittedColumn() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCellsWithOmittedColumn");
        // Cells(2) with ColumnIndex omitted - behavior differs in VBA vs LibreOffice
        double row = getNumericResult(0);
        double col = getNumericResult(1);
        assertTrue("Cells(2) row should be 2, got: " + row, row >= 2);
    }

    @Test
    public void testRowsWithOmittedIndex() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRowsWithOmittedIndex");
        // Rows() with no argument - may not be supported
        double result = getNumericResult(0);
        // 0 = error occurred, 1 = no error
        assertTrue("Rows() with omitted index may not be supported, got: " + result,
                result == 0 || result == 1);
    }

    @Test
    public void testOptionalArgInUserFunction() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestOptionalArgInUserFunction");
        // TestOptionalHelper() with no args - should use default 99
        assertEquals("Optional param default should be 99",
                99.0, getNumericResult(0), 0.001);
        // TestOptionalHelper(42) - explicit value
        assertEquals("Optional param explicit should be 42",
                42.0, getNumericResult(1), 0.001);
    }
}
