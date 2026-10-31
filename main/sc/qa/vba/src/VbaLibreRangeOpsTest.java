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
 * LibreOffice-style failure hypothesis tests for Range operations.
 *
 * Based on documented LibreOffice VBA port trouble areas:
 * - Application.Intersect with overlapping and non-overlapping ranges
 * - Application.Union with adjacent and disjoint ranges
 * - Overlap removal after Intersect/Union operations
 * - Merged-cell handling in Range operations
 * - Range.Count, Rows.Count, Columns.Count after merges
 * - Range.Value on merged cells vs individual cells
 *
 * Key hypothesis: AOO's interop layer may not properly handle
 * merged cell ranges in VBA Range operations, since Calc's
 * merged cells and VBA's merge semantics differ. Intersect/Union
 * on merged ranges may produce unexpected results.
 *
 * NOT integrated into the build (not in JunitTest_sc_vba.mk).
 * Status: EXPLORATORY - written but not verified against AOO runtime.
 *
 * Result columns (BASIC -> Java via UNO):
 * - Z column (index 25): numeric results (counts, row/column numbers)
 * - AA column (index 26): string results (addresses)
 */
public class VbaLibreRangeOpsTest extends VbaCompatTest {

    private static final String MODULE_NAME = "LibreRangeOpsTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +

        "'@TestMethod\n" +
        "Sub TestIntersectOverlapRemoval()\n" +
        "    ' Arrange - partially overlapping ranges\n" +
        "    Dim r1 As Object, r2 As Object, result As Object\n" +
        "    Set r1 = ActiveSheet.Range(\"A1:A10\")\n" +
        "    Set r2 = ActiveSheet.Range(\"A5:A15\")\n" +
        "\n" +
        "    ' Act\n" +
        "    Set result = Application.Intersect(r1, r2)\n" +
        "\n" +
        "    ' Assert - overlap should be A5:A10 (6 cells)\n" +
        "    Range(\"Z1\").Value = result.Count\n" +
        "    Range(\"AA1\").Value = result.Address\n" +
        "    Range(\"Z2\").Value = result.Row\n" +
        "    Range(\"Z3\").Value = result.Rows.Count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestUnionDisjointRanges()\n" +
        "    ' Arrange - truly disjoint ranges\n" +
        "    Dim r1 As Object, r2 As Object, result As Object\n" +
        "    Set r1 = ActiveSheet.Range(\"A1:B3\")\n" +
        "    Set r2 = ActiveSheet.Range(\"D5:E7\")\n" +
        "\n" +
        "    ' Act\n" +
        "    Set result = Application.Union(r1, r2)\n" +
        "\n" +
        "    ' Assert - union should be 12 cells (6 + 6)\n" +
        "    Range(\"Z1\").Value = result.Count\n" +
        "    Range(\"AA1\").Value = result.Address\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestIntersectNonContiguous()\n" +
        "    ' Arrange\n" +
        "    Dim r1 As Object, r2 As Object, result As Object\n" +
        "    Set r1 = ActiveSheet.Range(\"A1:A3,C1:C3\")\n" +
        "    Set r2 = ActiveSheet.Range(\"A2:A4,C2:C4\")\n" +
        "\n" +
        "    ' Act\n" +
        "    Set result = Application.Intersect(r1, r2)\n" +
        "\n" +
        "    ' Assert - intersect should be A2:A3,C2:C3 (4 cells)\n" +
        "    Range(\"Z1\").Value = result.Count\n" +
        "    Range(\"AA1\").Value = result.Address\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestMergedCellMergeArea()\n" +
        "    ' Arrange - merge cells A1:C3\n" +
        "    ActiveSheet.Range(\"A1:C3\").Merge\n" +
        "\n" +
        "    ' Act - MergeArea should return the full merged range\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1\").MergeArea\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = r.Count\n" +
        "    Range(\"AA1\").Value = r.Address\n" +
        "    Range(\"Z2\").Value = r.Rows.Count\n" +
        "    Range(\"Z3\").Value = r.Columns.Count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestMergedCellValue()\n" +
        "    ' Arrange - merge cells and set value on merged range\n" +
        "    ActiveSheet.Range(\"A1:C3\").Merge\n" +
        "    Range(\"A1\").Value = \"merged content\"\n" +
        "\n" +
        "    ' Act - reading individual cells within merge should return\n" +
        "    '       the value of the top-left cell\n" +
        "    Dim v1 As Variant, v2 As Variant\n" +
        "    v1 = ActiveSheet.Range(\"A1\").Value\n" +
        "    v2 = ActiveSheet.Range(\"B2\").Value\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = IIf(v1 = \"merged content\", 1, 0)\n" +
        "    Range(\"Z2\").Value = IIf(v2 = \"merged content\", 1, 0)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestIntersectWithMergedRange()\n" +
        "    ' Arrange - merged A1:C3, intersect with A2:B4\n" +
        "    ActiveSheet.Range(\"A1:C3\").Merge\n" +
        "    Dim merged As Object, other As Object, result As Object\n" +
        "    Set merged = ActiveSheet.Range(\"A1\")\n" +
        "    Set other = ActiveSheet.Range(\"A2:B4\")\n" +
        "\n" +
        "    ' Act\n" +
        "    Set result = Application.Intersect(merged, other)\n" +
        "\n" +
        "    ' Assert - intersect should be A2:B3 (the overlap)\n" +
        "    Range(\"Z1\").Value = IIf(result Is Nothing, 0, result.Count)\n" +
        "    If Not result Is Nothing Then\n" +
        "        Range(\"AA1\").Value = result.Address\n" +
        "    End If\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeCellsIndexing()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1:B5\")\n" +
        "\n" +
        "    ' Act - Cells(row, col) on a VBA Range\n" +
        "    ' Cells(1,1) should be A1, Cells(5,2) should be B5\n" +
        "    Dim c1 As Object: Set c1 = r.Cells(1, 1)\n" +
        "    Dim c2 As Object: Set c2 = r.Cells(5, 2)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = c1.Row\n" +
        "    Range(\"Z2\").Value = c1.Column\n" +
        "    Range(\"Z3\").Value = c2.Row\n" +
        "    Range(\"Z4\").Value = c2.Column\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeRowsColumnsIndexing()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1:B5\")\n" +
        "\n" +
        "    ' Act - Rows(2).Cells(1,1) should be A2\n" +
        "    Dim row2 As Object: Set row2 = r.Rows(2)\n" +
        "    Dim col1 As Object: Set col1 = r.Columns(1)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = row2.Row\n" +
        "    Range(\"Z2\").Value = row2.Cells.Count\n" +
        "    Range(\"Z3\").Value = col1.Column\n" +
        "    Range(\"Z4\").Value = col1.Rows.Count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeEntireRowEntireColumn()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"C3\")\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim er As Object: Set er = r.EntireRow\n" +
        "    Dim ec As Object: Set ec = r.EntireColumn\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = er.Row\n" +
        "    Range(\"Z2\").Value = er.Columns.Count\n" +
        "    Range(\"Z3\").Value = ec.Column\n" +
        "    Range(\"Z4\").Value = ec.Rows.Count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestUnionThreeRanges()\n" +
        "    ' Arrange\n" +
        "    Dim r1 As Object, r2 As Object, r3 As Object\n" +
        "    Set r1 = ActiveSheet.Range(\"A1:A3\")\n" +
        "    Set r2 = ActiveSheet.Range(\"C1:C3\")\n" +
        "    Set r3 = ActiveSheet.Range(\"E1:E3\")\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim result As Object\n" +
        "    Set result = Application.Union(r1, r2, r3)\n" +
        "\n" +
        "    ' Assert - should be 9 cells\n" +
        "    Range(\"Z1\").Value = IIf(result Is Nothing, -1, result.Count)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestIntersectAdjacentRanges()\n" +
        "    ' Arrange\n" +
        "    Dim r1 As Object, r2 As Object\n" +
        "    Set r1 = ActiveSheet.Range(\"A1:A5\")\n" +
        "    Set r2 = ActiveSheet.Range(\"A5:A10\")\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim result As Object\n" +
        "    Set result = Application.Intersect(r1, r2)\n" +
        "\n" +
        "    ' Assert - should be just A5 (1 cell)\n" +
        "    Range(\"Z1\").Value = IIf(result Is Nothing, 0, result.Count)\n" +
        "    If Not result Is Nothing Then\n" +
        "        Range(\"Z2\").Value = result.Row\n" +
        "    End If\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testIntersectOverlapRemoval() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestIntersectOverlapRemoval");
        assertEquals("Intersect of A1:A10 and A5:A15 should have 6 cells",
                6.0, getNumericResult(0), 0.001);
        String address = getStringResult(0);
        assertNotNull("Address should not be null", address);
        assertEquals("Intersect row should start at 5", 5.0, getNumericResult(1), 0.001);
        assertEquals("Intersect rows count should be 6", 6.0, getNumericResult(2), 0.001);
    }

    @Test
    public void testUnionDisjointRanges() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestUnionDisjointRanges");
        assertEquals("Union of A1:B3 and D5:E7 should have 12 cells",
                12.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testIntersectNonContiguous() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestIntersectNonContiguous");
        // A1:A3,C1:C3 intersect A2:A4,C2:C4 = A2:A3,C2:C3 (4 cells)
        assertEquals("Intersect of non-contiguous ranges should have 4 cells",
                4.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testMergedCellMergeArea() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestMergedCellMergeArea");
        assertEquals("Merged A1:C3 MergeArea Count should be 9",
                9.0, getNumericResult(0), 0.001);
        assertEquals("MergeArea Rows should be 3", 3.0, getNumericResult(1), 0.001);
        assertEquals("MergeArea Columns should be 3", 3.0, getNumericResult(2), 0.001);
    }

    @Test
    public void testMergedCellValue() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestMergedCellValue");
        // On some implementations, only the top-left cell holds the value
        assertEquals("Merged cell A1 should have value",
                1.0, getNumericResult(0), 0.001);
        assertEquals("Merged cell B2 should also have value (VBA semantics)",
                1.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testIntersectWithMergedRange() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestIntersectWithMergedRange");
        // Intersect of merged range and A2:B4
        double result = getNumericResult(0);
        // Should be non-zero (the overlap area count)
        assertTrue("Intersect with merged range should return non-zero count, got: " + result,
                result > 0);
    }

    @Test
    public void testRangeCellsIndexing() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeCellsIndexing");
        assertEquals("Cells(1,1) of A1:B5 should be row 1",
                1.0, getNumericResult(0), 0.001);
        assertEquals("Cells(1,1) of A1:B5 should be column 1",
                1.0, getNumericResult(1), 0.001);
        assertEquals("Cells(5,2) of A1:B5 should be row 5",
                5.0, getNumericResult(2), 0.001);
        assertEquals("Cells(5,2) of A1:B5 should be column 2",
                2.0, getNumericResult(3), 0.001);
    }

    @Test
    public void testRangeRowsColumnsIndexing() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeRowsColumnsIndexing");
        assertEquals("Rows(2) of A1:B5 should be row 2",
                2.0, getNumericResult(0), 0.001);
        assertEquals("Rows(2) should have 2 cells (2 columns)",
                2.0, getNumericResult(1), 0.001);
        assertEquals("Columns(1) of A1:B5 should be column 1",
                1.0, getNumericResult(2), 0.001);
        assertEquals("Columns(1) should have 5 rows",
                5.0, getNumericResult(3), 0.001);
    }

    @Test
    public void testRangeEntireRowEntireColumn() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeEntireRowEntireColumn");
        assertEquals("EntireRow of C3 should be row 3",
                3.0, getNumericResult(0), 0.001);
        double colCount = getNumericResult(1);
        assertTrue("EntireRow columns > 0, got: " + colCount, colCount > 0);
        assertEquals("EntireColumn of C3 should be column 3",
                3.0, getNumericResult(2), 0.001);
        double rowCount = getNumericResult(3);
        assertTrue("EntireColumn rows > 0, got: " + rowCount, rowCount > 0);
    }

    @Test
    public void testUnionThreeRanges() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestUnionThreeRanges");
        // Union with 3 args tests optional parameter handling
        assertEquals("Union of 3 single-column ranges (3x3 each) should have 9 cells",
                9.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testIntersectAdjacentRanges() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestIntersectAdjacentRanges");
        // Adjacent ranges sharing only one row
        assertEquals("Intersect of adjacent ranges should have 1 cell",
                1.0, getNumericResult(0), 0.001);
        assertEquals("Intersect row should be 5",
                5.0, getNumericResult(1), 0.001);
    }
}
