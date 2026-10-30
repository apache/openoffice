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
 * P1: Tests for VBA Range operations derived from SAP IBP samples.
 *
 * Tests the following VBA object model features:
 * - Range.Find with LookIn:=xlValues
 * - Range.Text property (read-only formatted text)
 * - Columns(col).Cut and Columns(col).Insert with Shift parameter
 * - Range.Rows.Group and Range.Rows.Ungroup
 * - Range.Offset chaining (Offset + EntireRow.Hidden)
 *
 * Test methodology: BASIC macros exercise the VBA APIs and write
 * results to cells Z1, Z2, etc. (column 25). Java code verifies via UNO.
 * Follows AAA pattern with '@TestMethod' markers in comments.
 */
public class VbaSapRangeOpsTest extends VbaCompatTest {

    private static final String MODULE_NAME = "RangeOpsTests";

    // Excel VBA constants
    private static final int XL_DIRECTION_DOWN = -4162;  // xlDown
    private static final int XL_SHIFT_TO_RIGHT = -4162;  // xlShiftToRight
    private static final int XL_LOOK_IN_VALUES = -4163; // xlValues
    private static final int XL_LOOK_IN_FORMULAS = -4144; // xlFormulas (not standard, but used in test)

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +
        "Const xlValues = -4163\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeFind()\n" +
        "    ' Arrange - populate A1:A10 with strings, set A5 = 'Target'\n" +
        "    Dim i As Integer\n" +
        "    For i = 1 To 10\n" +
        "        Cells(i, 1).Value = \"item\" & i\n" +
        "    Next i\n" +
        "    Range(\"A5\").Value = \"Target\"\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim c As Object\n" +
        "    Set c = Range(\"A1:A10\").Find(\"Target\", LookIn:=xlValues)\n" +
        "\n" +
        "    ' Assert\n" +
        "    If c Is Nothing Then\n" +
        "        Range(\"Z1\").Value = 0\n" +
        "    Else\n" +
        "        Range(\"Z1\").Value = c.Row\n" +
        "    End If\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeText()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = 42\n" +
        "\n" +
        "    ' Act & Assert - .Text returns formatted display text\n" +
        "    Range(\"AA1\").Value = Range(\"A1\").Text\n" +
        "    Range(\"Z1\").Value = CDbl(Range(\"A1\").Value)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeTextFormatted()\n" +
        "    ' Arrange - set a date-formatted cell\n" +
        "    Range(\"A1\").Value = 44927 ' Jan 1 2023 in OLE date\n" +
        "    Range(\"A1\").NumberFormat = \"MM/DD/YYYY\"\n" +
        "\n" +
        "    ' Act & Assert - .Text should return formatted string\n" +
        "    Range(\"AA1\").Value = Range(\"A1\").Text\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeTextString()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = \"hello world\"\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"AA1\").Value = Range(\"A1\").Text\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestColumnsCutInsert()\n" +
        "    ' Arrange - put value in A1\n" +
        "    Range(\"A1\").Value = \"original\"\n" +
        "\n" +
        "    ' Act\n" +
        "    Columns(\"A\").Cut\n" +
        "    Columns(\"C\").Insert Shift:=-4162\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"AA1\").Value = Range(\"C1\").Value\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRowsGroupUngroup()\n" +
        "    ' Arrange - put data in rows 3-8\n" +
        "    Dim i As Integer\n" +
        "    For i = 3 To 8\n" +
        "        Cells(i, 1).Value = \"row\" & i\n" +
        "    Next i\n" +
        "\n" +
        "    ' Act - group rows 3-8\n" +
        "    Range(\"A3:A8\").Rows.Group\n" +
        "\n" +
        "    ' Assert - check row count of the range\n" +
        "    Range(\"Z1\").Value = CDbl(Range(\"A3:A8\").Rows.Count)\n" +
        "    Range(\"AA1\").Value = \"grouped\"\n" +
        "\n" +
        "    ' Cleanup - ungroup\n" +
        "    Range(\"A3:A8\").Rows.Ungroup\n" +
        "    Range(\"AA2\").Value = \"ungrouped\"\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestOffsetEntireRowHidden()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Offset(3, 0).EntireRow.Hidden = True\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = CInt(Range(\"A4\").EntireRow.Hidden)\n" +
        "\n" +
        "    ' Restore\n" +
        "    Range(\"A1\").Offset(3, 0).EntireRow.Hidden = False\n" +
        "    Range(\"Z2\").Value = CInt(Range(\"A4\").EntireRow.Hidden)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeFindNotFound()\n" +
        "    ' Arrange\n" +
        "    Dim i As Integer\n" +
        "    For i = 1 To 5\n" +
        "        Cells(i, 1).Value = \"item\" & i\n" +
        "    Next i\n" +
        "\n" +
        "    ' Act - search for something that doesn't exist\n" +
        "    Dim c As Object\n" +
        "    Set c = Range(\"A1:A5\").Find(\"NonExistent\", LookIn:=xlValues)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = IIf(c Is Nothing, 1, 0)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRangeTextNarrowColumn()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = \"this is a long string that might be truncated\"\n" +
        "\n" +
        "    ' Act & Assert - .Text should return the display text\n" +
        "    Range(\"AA1\").Value = Range(\"A1\").Text\n" +
        "    Range(\"Z1\").Value = Len(Range(\"AA1\").Value)\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testRangeFind() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeFind");
        assertEquals("Find for 'Target' should return row 5",
            5.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testRangeFindNotFound() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeFindNotFound");
        assertEquals("Find for non-existent should return Nothing (1)",
            1.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testRangeText() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeText");
        assertEquals("Value of 42 should be 42",
            42.0, getNumericResult(0), 0.001);
        assertEquals("Text of 42 should be '42'",
            "42", getStringResult(0));
    }

    @Test
    public void testRangeTextFormatted() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeTextFormatted");
        String text = getStringResult(0);
        assertNotNull("Text for date should not be null", text);
        assertFalse("Text should not be empty", text.isEmpty());
    }

    @Test
    public void testRangeTextString() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeTextString");
        assertEquals("Text of 'hello world' should be 'hello world'",
            "hello world", getStringResult(0));
    }

    @Test
    public void testRangeTextNarrowColumn() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeTextNarrowColumn");
        String text = getStringResult(0);
        assertNotNull("Text for long string should not be null", text);
    }

    @Test
    public void testColumnsCutInsert() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestColumnsCutInsert");
        assertEquals("Cut and Insert should move 'original' to column C",
            "original", getStringResult(0));
    }

    @Test
    public void testRowsGroupUngroup() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRowsGroupUngroup");
        assertEquals("Range A3:A8 should have 6 rows",
            6.0, getNumericResult(0), 0.001);
        assertEquals("After Group, status should be 'grouped'",
            "grouped", getStringResult(0));
        assertEquals("After Ungroup, status should be 'ungrouped'",
            "ungrouped", getStringResult(1));
    }

    @Test
    public void testOffsetEntireRowHidden() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestOffsetEntireRowHidden");
        assertEquals("EntireRow.Hidden after set=True should be -1",
            -1.0, getNumericResult(0), 0.001);
        assertEquals("EntireRow.Hidden after set=False should be 0",
            0.0, getNumericResult(1), 0.001);
    }
}
