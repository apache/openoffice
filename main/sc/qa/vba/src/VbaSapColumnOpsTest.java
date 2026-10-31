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
 * P1: Tests for VBA ColumnWidth and column visibility APIs.
 *
 * Tests the following VBA object model features:
 * - Range.ColumnWidth get/set
 * - EntireColumn.ColumnWidth get/set
 * - Iterating ActiveSheet.Columns collection
 * - Range.Column property (get column number)
 * - Columns.Hidden get/set (column visibility toggle)
 *
 * Test methodology: BASIC macros exercise the VBA APIs and write
 * results to cells Z1, Z2, etc. (column 25). Java code verifies via UNO.
 * Follows AAA pattern with '@TestMethod' markers in comments.
 */
public class VbaSapColumnOpsTest extends VbaCompatTest {

    private static final String MODULE_NAME = "ColumnOpsTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +

        "'@TestMethod\n" +
        "Sub TestColumnWidthGetSet()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").ColumnWidth = 15\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = CDbl(Range(\"A1\").ColumnWidth)\n" +
        "    Range(\"Z2\").Value = CDbl(Range(\"A1\").EntireColumn.ColumnWidth)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestColumnWidthEntireColumn()\n" +
        "    ' Arrange\n" +
        "    Range(\"B1\").EntireColumn.ColumnWidth = 25\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = CDbl(Range(\"B1\").ColumnWidth)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestColumnIteration()\n" +
        "    ' Arrange - populate columns A-D\n" +
        "    Dim i As Integer\n" +
        "    For i = 1 To 4\n" +
        "        Cells(1, i).Value = \"col\" & i\n" +
        "    Next i\n" +
        "\n" +
        "    ' Act - count columns beyond B (index > 2)\n" +
        "    Dim c As Object\n" +
        "    Dim count As Integer: count = 0\n" +
        "    For Each c In ActiveSheet.Columns\n" +
        "        If c.Column > 2 Then count = count + 1\n" +
        "    Next\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestColumnHidden()\n" +
        "    ' Arrange\n" +
        "    Columns(\"B\").Hidden = True\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = CInt(Columns(\"B\").Hidden)\n" +
        "\n" +
        "    ' Restore\n" +
        "    Columns(\"B\").Hidden = False\n" +
        "    Range(\"Z2\").Value = CInt(Columns(\"B\").Hidden)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestColumnWidthReset()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").ColumnWidth = 30\n" +
        "    Range(\"Z1\").Value = CDbl(Range(\"A1\").ColumnWidth)\n" +
        "\n" +
        "    ' Act\n" +
        "    Range(\"A1\").ColumnWidth = 8.43\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z2\").Value = CDbl(Range(\"A1\").ColumnWidth)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestColumnProperty()\n" +
        "    ' Arrange & Act\n" +
        "    Dim c As Object\n" +
        "    Set c = ActiveSheet.Columns(3)\n" +
        "\n" +
        "    ' Assert - Column property returns column number\n" +
        "    Range(\"Z1\").Value = CDbl(c.Column)\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testColumnWidthGetSet() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestColumnWidthGetSet");
        assertEquals("ColumnWidth set to 15 should read 15",
            15.0, getNumericResult(0), 0.001);
        assertEquals("EntireColumn.ColumnWidth should match",
            15.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testColumnWidthEntireColumn() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestColumnWidthEntireColumn");
        assertEquals("EntireColumn.ColumnWidth = 25 should read 25",
            25.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testColumnIteration() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestColumnIteration");
        assertEquals("Columns with index > 2 should be counted",
            2.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testColumnHidden() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestColumnHidden");
        assertEquals("Hidden=True should return -1 (VBA True)",
            -1.0, getNumericResult(0), 0.001);
        assertEquals("Hidden=False should return 0 (VBA False)",
            0.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testColumnWidthReset() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestColumnWidthReset");
        assertEquals("ColumnWidth set to 30 should read 30",
            30.0, getNumericResult(0), 0.001);
        assertEquals("ColumnWidth reset to 8.43 should read ~8.43",
            8.43, getNumericResult(1), 0.01);
    }

    @Test
    public void testColumnProperty() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestColumnProperty");
        assertEquals("Columns(3).Column should return 3",
            3.0, getNumericResult(0), 0.001);
    }
}
