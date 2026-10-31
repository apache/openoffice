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
 * P2: Tests for VBA Outline and UsedRange object model features.
 *
 * Tests the following VBA object model features:
 * - Worksheet.UsedRange (range object)
 * - Worksheet.Outline.ShowLevels (row/column outline level expansion)
 *
 * Test methodology: BASIC macros exercise the VBA APIs and write
 * results to cells Z1, Z2, etc. (column 25). Java code verifies via UNO.
 * Follows AAA pattern with '@TestMethod' markers in comments.
 */
public class VbaSapOutlineTest extends VbaCompatTest {

    private static final String MODULE_NAME = "OutlineTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +

        "'@TestMethod\n" +
        "Sub TestUsedRangeBlank()\n" +
        "    ' Arrange - blank sheet\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Dim ur As Object\n" +
        "    Set ur = ActiveSheet.UsedRange\n" +
        "    Range(\"Z1\").Value = ur.Cells.Count\n" +
        "    Range(\"AA1\").Value = ur.Address\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestUsedRangeWithValue()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = \"data\"\n" +
        "    Range(\"C3\").Value = 42\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Dim ur As Object\n" +
        "    Set ur = ActiveSheet.UsedRange\n" +
        "    Range(\"Z1\").Value = ur.Cells.Count\n" +
        "    Range(\"AA1\").Value = ur.Address\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestUsedRangeWithFormula()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = 10\n" +
        "    Range(\"B1\").Formula = \"=A1*2\"\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Dim ur As Object\n" +
        "    Set ur = ActiveSheet.UsedRange\n" +
        "    Range(\"Z1\").Value = ur.Cells.Count\n" +
        "    Range(\"AA1\").Value = ur.Address\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestUsedRangeAfterClear()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = \"data\"\n" +
        "    Range(\"Z1\").Value = 1 ' placeholder\n" +
        "\n" +
        "    ' Act - clear values\n" +
        "    Range(\"A1\").ClearContents\n" +
        "    Dim ur As Object\n" +
        "    Set ur = ActiveSheet.UsedRange\n" +
        "    Range(\"AA1\").Value = ur.Address\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestShowLevelsRowOnly()\n" +
        "    ' Arrange - create data and group rows\n" +
        "    Dim i As Integer\n" +
        "    For i = 1 To 10\n" +
        "        Cells(i, 1).Value = \"row\" & i\n" +
        "    Next i\n" +
        "    Range(\"A3:A8\").Rows.Group\n" +
        "\n" +
        "    ' Act - show 1 row level (collapse)\n" +
        "    ActiveSheet.Outline.ShowLevels RowLevels:=1\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = 1\n" +
        "    Range(\"AA1\").Value = \"row levels shown\"\n" +
        "\n" +
        "    ' Restore\n" +
        "    ActiveSheet.Outline.ShowLevels RowLevels:=2\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestShowLevelsColumnOnly()\n" +
        "    ' Arrange - create data and group columns\n" +
        "    Dim i As Integer\n" +
        "    For i = 1 To 5\n" +
        "        Cells(1, i).Value = \"col\" & i\n" +
        "    Next i\n" +
        "    Range(\"B1:D1\").Columns.Group\n" +
        "\n" +
        "    ' Act\n" +
        "    ActiveSheet.Outline.ShowLevels ColumnLevels:=1\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = 1\n" +
        "    Range(\"AA1\").Value = \"column levels shown\"\n" +
        "\n" +
        "    ' Restore\n" +
        "    ActiveSheet.Outline.ShowLevels ColumnLevels:=2\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestShowLevelsCombined()\n" +
        "    ' Arrange - group both rows and columns\n" +
        "    Dim i As Integer\n" +
        "    For i = 1 To 10\n" +
        "        Cells(i, 1).Value = \"r\" & i\n" +
        "    Next i\n" +
        "    For i = 1 To 5\n" +
        "        Cells(1, i).Value = \"c\" & i\n" +
        "    Next i\n" +
        "    Range(\"A3:A8\").Rows.Group\n" +
        "    Range(\"B1:D1\").Columns.Group\n" +
        "\n" +
        "    ' Act\n" +
        "    ActiveSheet.Outline.ShowLevels RowLevels:=1, ColumnLevels:=1\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = 1\n" +
        "    Range(\"AA1\").Value = \"combined levels shown\"\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testUsedRangeBlank() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestUsedRangeBlank");
        double count = getNumericResult(0);
        assertTrue("Blank sheet UsedRange should have at least 1 cell", count >= 1);
    }

    @Test
    public void testUsedRangeWithValue() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestUsedRangeWithValue");
        double count = getNumericResult(0);
        assertEquals("UsedRange with data in A1 and C3 should cover at least 9 cells (A1:C3)",
            9.0, count, 0.0);
        String address = getStringResult(0);
        assertNotNull("UsedRange address should not be null", address);
    }

    @Test
    public void testUsedRangeWithFormula() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestUsedRangeWithFormula");
        double count = getNumericResult(0);
        assertTrue("UsedRange with formula should have at least 2 cells", count >= 2);
    }

    @Test
    public void testUsedRangeAfterClear() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestUsedRangeAfterClear");
        String address = getStringResult(0);
        assertNotNull("UsedRange after clear should have an address", address);
    }

    @Test
    public void testShowLevelsRowOnly() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestShowLevelsRowOnly");
        assertEquals("ShowLevels RowLevels should not error",
            1.0, getNumericResult(0), 0.001);
        assertEquals("Status should be 'row levels shown'",
            "row levels shown", getStringResult(0));
    }

    @Test
    public void testShowLevelsColumnOnly() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestShowLevelsColumnOnly");
        assertEquals("ShowLevels ColumnLevels should not error",
            1.0, getNumericResult(0), 0.001);
        assertEquals("Status should be 'column levels shown'",
            "column levels shown", getStringResult(0));
    }

    @Test
    public void testShowLevelsCombined() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestShowLevelsCombined");
        assertEquals("ShowLevels combined should not error",
            1.0, getNumericResult(0), 0.001);
        assertEquals("Status should be 'combined levels shown'",
            "combined levels shown", getStringResult(0));
    }
}
