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
 * P4: Tests for VBA Range object model APIs.
 *
 * Tests the following VBA object model features:
 * - Application.Intersect returns the intersection of two ranges
 * - Application.Union returns the union of two ranges
 * - Range.Value2 property (variant array for multi-cell ranges)
 * - Range.Precedents property (cells referenced by formula)
 * - Range.Dependents property (cells that reference this cell)
 * - Range.Offset property (returns shifted range)
 * - Range.Rows/Columns indexing
 * - Range.Count property
 *
 * Test methodology: BASIC macros exercise Range VBA APIs and write
 * results to cells Z1, Z2, etc. (column 25). Java code verifies via UNO.
 *
 * Result columns:
 * - Z column (index 25): numeric results
 * - AA column (index 26): string results (cell addresses, etc.)
 */
public class VbaRangeTest extends VbaCompatTest {

    private static final String MODULE_NAME = "RangeTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +

        "'@TestMethod\n" +
        "Sub TestIntersectOverlap()\n" +
        "    ' Arrange\n" +
        "    Dim r1 As Object, r2 As Object, result As Object\n" +
        "\n" +
        "    ' Act\n" +
        "    Set r1 = ActiveSheet.Range(\"A1:A10\")\n" +
        "    Set r2 = ActiveSheet.Range(\"A5:A15\")\n" +
        "    Set result = Application.Intersect(r1, r2)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = result.Count\n" +
        "    Range(\"AA1\").Value = result.Address\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestIntersectNoOverlap()\n" +
        "    ' Arrange\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim r1 As Object, r2 As Object, result As Object\n" +
        "    Set r1 = ActiveSheet.Range(\"A1:A3\")\n" +
        "    Set r2 = ActiveSheet.Range(\"B5:B10\")\n" +
        "    Set result = Application.Intersect(r1, r2)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = IIf(result Is Nothing, 1, 0)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestUnion()\n" +
        "    ' Arrange\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim r1 As Object, r2 As Object, result As Object\n" +
        "    Set r1 = ActiveSheet.Range(\"A1:A5\")\n" +
        "    Set r2 = ActiveSheet.Range(\"C1:C5\")\n" +
        "    Set result = Application.Union(r1, r2)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = result.Count\n" +
        "    Range(\"AA1\").Value = result.Address\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestValue2SingleCell()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = 42\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim val As Variant\n" +
        "    val = ActiveSheet.Range(\"A1\").Value2\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = val\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestValue2StringCell()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = \"hello\"\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim val As Variant\n" +
        "    val = ActiveSheet.Range(\"A1\").Value2\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = IIf(val = \"hello\", 1, 0)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestPrecedents()\n" +
        "    ' Arrange - set up formula chain: A1 -> B1 -> C1\n" +
        "    Range(\"A1\").Value = 10\n" +
        "    Range(\"B1\").Formula = \"=A1*2\"\n" +
        "    Range(\"C1\").Formula = \"=B1+5\"\n" +
        "\n" +
        "    ' Act - Precedents of C1 should include B1 (and transitively A1)\n" +
        "    Dim prec As Object\n" +
        "    Set prec = ActiveSheet.Range(\"C1\").Precedents\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = prec.Count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestDependents()\n" +
        "    ' Arrange - set up formula chain: A1 -> B1 -> C1\n" +
        "    Range(\"A1\").Value = 10\n" +
        "    Range(\"B1\").Formula = \"=A1*2\"\n" +
        "    Range(\"C1\").Formula = \"=B1+5\"\n" +
        "\n" +
        "    ' Act - Dependents of A1 should include B1 (and transitively C1)\n" +
        "    Dim dep As Object\n" +
        "    Set dep = ActiveSheet.Range(\"A1\").Dependents\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = dep.Count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeCount()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "\n" +
        "    ' Act\n" +
        "    Set r = ActiveSheet.Range(\"A1:A10\")\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = r.Count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeRowsColumns()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1:C3\")\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = r.Rows.Count\n" +
        "    Range(\"Z2\").Value = r.Columns.Count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeOffset()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1\")\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim offset As Object\n" +
        "    Set offset = r.Offset(2, 1)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = offset.Row\n" +
        "    Range(\"Z2\").Value = offset.Column\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeAddress()\n" +
        "    ' Arrange\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1:B5\")\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"AA1\").Value = r.Address\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestValue2MultiCell()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = 10\n" +
        "    Range(\"A2\").Value = 20\n" +
        "    Range(\"A3\").Value = 30\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim arr As Variant\n" +
        "    arr = ActiveSheet.Range(\"A1:A3\").Value2\n" +
        "\n" +
        "    ' Assert - write each element to Z column\n" +
        "    Range(\"Z1\").Value = arr(1, 1)\n" +
        "    Range(\"Z2\").Value = arr(2, 1)\n" +
        "    Range(\"Z3\").Value = arr(3, 1)\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testIntersectOverlap() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestIntersectOverlap");
        assertEquals("Intersect of A1:A10 and A5:A15 should have 6 cells",
            6.0, getNumericResult(0), 0.001);
        String address = getStringResult(0);
        assertEquals("Intersection range address should be A5:A10", "$A$5:$A$10", address);
    }

    @Test
    public void testIntersectNoOverlap() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestIntersectNoOverlap");
        assertEquals("Non-overlapping ranges should return Nothing (1)",
            1.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testUnion() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestUnion");
        assertEquals("Union of A1:A5 and C1:C5 should have 10 cells",
            10.0, getNumericResult(0), 0.001);
        String address = getStringResult(0);
        assertNotNull("Union range address should not be null", address);
    }

    @Test
    public void testValue2SingleCell() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestValue2SingleCell");
        assertEquals("Value2 of A1 should be 42", 42.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testValue2StringCell() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestValue2StringCell");
        assertEquals("Value2 of string cell should be 1 (match)",
            1.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testPrecedents() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestPrecedents");
        double count = getNumericResult(0);
        assertTrue("Precedents count should be > 0, got: " + count, count > 0);
    }

    @Test
    public void testDependents() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestDependents");
        double count = getNumericResult(0);
        assertTrue("Dependents count should be > 0, got: " + count, count > 0);
    }

    @Test
    public void testRangeCount() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeCount");
        assertEquals("Range A1:A10 should have Count=10",
            10.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testRangeRowsColumns() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeRowsColumns");
        assertEquals("Range A1:C3 should have 3 rows",
            3.0, getNumericResult(0), 0.001);
        assertEquals("Range A1:C3 should have 3 columns",
            3.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testRangeOffset() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeOffset");
        assertEquals("Offset(2,1) from A1 should be row 3",
            3.0, getNumericResult(0), 0.001);
        assertEquals("Offset(2,1) from A1 should be column 2 (B)",
            2.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testRangeAddress() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeAddress");
        String address = getStringResult(0);
        assertEquals("Address of A1:B5 should be $A$1:$B$5", "$A$1:$B$5", address);
    }

    @Test
    public void testValue2MultiCell() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestValue2MultiCell");
        assertEquals("First element of Value2 should be 10",
            10.0, getNumericResult(0), 0.001);
        assertEquals("Second element of Value2 should be 20",
            20.0, getNumericResult(1), 0.001);
        assertEquals("Third element of Value2 should be 30",
            30.0, getNumericResult(2), 0.001);
    }
}
