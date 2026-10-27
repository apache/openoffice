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
import static org.junit.Assert.*;

/**
 * P1: Tests for VBA FormatCondition and FormatConditions APIs.
 *
 * Tests the following VBA object model features:
 * - Range.FormatConditions.Add with xlExpression and xlCellValue types
 * - FormatCondition.Type, Formula1, Formula2, Operator properties
 * - FormatCondition.Modify to change existing conditions
 * - FormatCondition.Delete to remove individual conditions
 * - FormatConditions.Delete to remove all conditions
 * - FormatConditions.Count and Item collection access
 * - For Each enumeration over FormatConditions
 *
 * Reference Excel constants:
 * - XlFormatConditionType: xlCellValue=1, xlExpression=2
 * - XlFormatConditionOperator: xlBetween=1, xlEqual=3, xlGreater=5,
 *   xlGreaterEqual=7, xlLess=6, xlLessEqual=8, xlNotBetween=2, xlNotEqual=4
 *
 * Test methodology: BASIC macros exercise the VBA APIs and write results
 * to cell A1 (numeric) and cell B1 (string). Java code verifies via UNO.
 */
public class VbaFormatConditionTest extends VbaCompatTest {

    private static final String MODULE_NAME = "FormatCondTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "Option VBASupport 1\n" +
        "\n" +
        "' Constants matching Excel VBA enum values\n" +
        "Const xlCellValue = 1\n" +
        "Const xlExpression = 2\n" +
        "Const xlBetween = 1\n" +
        "Const xlEqual = 3\n" +
        "Const xlGreater = 5\n" +
        "Const xlGreaterEqual = 7\n" +
        "Const xlLess = 6\n" +
        "Const xlLessEqual = 8\n" +
        "Const xlNotBetween = 2\n" +
        "Const xlNotEqual = 4\n" +
        "\n" +

        "Sub TestAddExpressionCondition()\n" +
        "    Dim conds As Object\n" +
        "    Set conds = ActiveSheet.Range(\"A1:A10\").FormatConditions\n" +
        "    conds.Delete\n" +
        "    Dim cond As Object\n" +
        "    Set cond = conds.Add(xlExpression, , \"=A1>5\")\n" +
        "    Range(\"Z1\").Value = cond.Type\n" +
        "    Range(\"Z2\").Value = cond.Formula1\n" +
        "End Sub\n" +
        "\n" +

        "Sub TestAddCellValueCondition()\n" +
        "    Dim conds As Object\n" +
        "    Set conds = ActiveSheet.Range(\"A1:A10\").FormatConditions\n" +
        "    conds.Delete\n" +
        "    Dim cond As Object\n" +
        "    Set cond = conds.Add(xlCellValue, xlGreater, \"5\")\n" +
        "    Range(\"Z1\").Value = cond.Type\n" +
        "    Range(\"Z2\").Value = cond.Operator\n" +
        "    Range(\"Z3\").Value = cond.Formula1\n" +
        "End Sub\n" +
        "\n" +

        "Sub TestModifyCondition()\n" +
        "    Dim conds As Object\n" +
        "    Set conds = ActiveSheet.Range(\"A1:A10\").FormatConditions\n" +
        "    conds.Delete\n" +
        "    Dim cond As Object\n" +
        "    Set cond = conds.Add(xlCellValue, xlGreater, \"5\")\n" +
        "    cond.Modify xlExpression, , \"=A1<3\"\n" +
        "    Range(\"Z1\").Value = cond.Type\n" +
        "    Range(\"Z2\").Value = cond.Formula1\n" +
        "End Sub\n" +
        "\n" +

        "Sub TestCountAfterDeleteAll()\n" +
        "    Dim conds As Object\n" +
        "    Set conds = ActiveSheet.Range(\"A1:A10\").FormatConditions\n" +
        "    conds.Delete\n" +
        "    conds.Add xlExpression, , \"=A1>5\"\n" +
        "    conds.Add xlExpression, , \"=A1<3\"\n" +
        "    Range(\"Z1\").Value = conds.Count\n" +
        "    conds.Delete\n" +
        "    Range(\"Z2\").Value = conds.Count\n" +
        "End Sub\n" +
        "\n" +

        "Sub TestDeleteSingleCondition()\n" +
        "    Dim conds As Object\n" +
        "    Set conds = ActiveSheet.Range(\"A1:A10\").FormatConditions\n" +
        "    conds.Delete\n" +
        "    Dim cond As Object\n" +
        "    Set cond = conds.Add(xlExpression, , \"=A1>5\")\n" +
        "    Range(\"Z1\").Value = conds.Count\n" +
        "    cond.Delete\n" +
        "    Range(\"Z2\").Value = conds.Count\n" +
        "End Sub\n" +
        "\n" +

        "Sub TestItemAccess()\n" +
        "    Dim conds As Object\n" +
        "    Set conds = ActiveSheet.Range(\"A1:A10\").FormatConditions\n" +
        "    conds.Delete\n" +
        "    conds.Add xlExpression, , \"=A1>5\"\n" +
        "    conds.Add xlExpression, , \"=B1<3\"\n" +
        "    Range(\"Z1\").Value = conds.Count\n" +
        "    Dim cond As Object\n" +
        "    Set cond = conds.Item(1)\n" +
        "    Range(\"Z2\").Value = cond.Formula1\n" +
        "    Set cond = conds.Item(2)\n" +
        "    Range(\"Z3\").Value = cond.Formula1\n" +
        "End Sub\n" +
        "\n" +

        "Sub TestEnumeration()\n" +
        "    Dim conds As Object\n" +
        "    Set conds = ActiveSheet.Range(\"A1:A10\").FormatConditions\n" +
        "    conds.Delete\n" +
        "    conds.Add xlExpression, , \"=A1>5\"\n" +
        "    conds.Add xlExpression, , \"=B1<3\"\n" +
        "    Dim count As Integer\n" +
        "    count = 0\n" +
        "    Dim cond As Object\n" +
        "    For Each cond In conds\n" +
        "        count = count + 1\n" +
        "    Next cond\n" +
        "    Range(\"Z1\").Value = count\n" +
        "End Sub\n" +
        "\n" +

        "Sub TestFormula2()\n" +
        "    Dim conds As Object\n" +
        "    Set conds = ActiveSheet.Range(\"A1:A10\").FormatConditions\n" +
        "    conds.Delete\n" +
        "    Dim cond As Object\n" +
        "    Set cond = conds.Add(xlCellValue, xlBetween, \"5\")\n" +
        "    cond.Formula1 = \"10\"\n" +
        "    cond.Formula2 = \"20\"\n" +
        "    Range(\"Z1\").Value = cond.Formula1\n" +
        "    Range(\"Z2\").Value = cond.Formula2\n" +
        "End Sub\n" +
        "\n" +

        "Sub TestDeleteOnEmpty()\n" +
        "    Dim conds As Object\n" +
        "    Set conds = ActiveSheet.Range(\"A1:A10\").FormatConditions\n" +
        "    conds.Delete\n" +
        "    Range(\"Z1\").Value = conds.Count\n" +
        "End Sub\n";

    @Override
    @Before
    public void setUp() throws Exception {
        super.setUp();
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testAddExpressionCondition() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestAddExpressionCondition");
        assertEquals("Type should be xlExpression (2)", 2.0, getNumericResultAt(25, 0), 0.001);
        assertEquals("Formula1 should match", "=A1>5", getStringAt(25, 1));
    }

    @Test
    public void testAddCellValueCondition() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestAddCellValueCondition");
        assertEquals("Type should be xlCellValue (1)", 1.0, getNumericResultAt(25, 0), 0.001);
        assertEquals("Operator should be xlGreater (5)", 5.0, getNumericResultAt(25, 1), 0.001);
        assertEquals("Formula1 should be 5", 5.0, getNumericResultAt(25, 2), 0.001);
    }

    @Test
    public void testModifyCondition() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestModifyCondition");
        assertEquals("Type should be xlExpression (2) after Modify",
            2.0, getNumericResultAt(25, 0), 0.001);
        assertEquals("Formula1 should be updated", "=A1<3", getStringAt(25, 1));
    }

    @Test
    public void testDeleteAllConditions() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCountAfterDeleteAll");
        assertEquals("Should have 2 conditions before Delete",
            2.0, getNumericResultAt(25, 0), 0.001);
        assertEquals("Should have 0 conditions after Delete",
            0.0, getNumericResultAt(25, 1), 0.001);
    }

    @Test
    public void testDeleteSingleCondition() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestDeleteSingleCondition");
        assertEquals("Should have 1 condition before Delete",
            1.0, getNumericResultAt(25, 0), 0.001);
        assertEquals("Should have 0 conditions after Delete",
            0.0, getNumericResultAt(25, 1), 0.001);
    }

    @Test
    public void testItemAccess() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestItemAccess");
        assertEquals("Should have 2 conditions",
            2.0, getNumericResultAt(25, 0), 0.001);
        // Formula1 values are written to Z2 and Z3 as numbers
        assertEquals("Formula1 of item 1 should be 5",
            5.0, getNumericResultAt(25, 1), 0.001);
        assertEquals("Formula1 of item 2 should be 3",
            3.0, getNumericResultAt(25, 2), 0.001);
    }

    @Test
    public void testEnumeration() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestEnumeration");
        assertEquals("Enumeration should count 2 conditions",
            2.0, getNumericResultAt(25, 0), 0.001);
    }

    @Test
    public void testFormula2Property() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestFormula2");
        assertEquals("Formula1 should be 10",
            10.0, getNumericResultAt(25, 0), 0.001);
        assertEquals("Formula2 should be 20",
            20.0, getNumericResultAt(25, 1), 0.001);
    }

    @Test
    public void testDeleteOnEmpty() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestDeleteOnEmpty");
        assertEquals("Count should be 0 on fresh range",
            0.0, getNumericResultAt(25, 0), 0.001);
    }

    private double getNumericResultAt(int column, int row) throws Exception {
        return getCell(column, row).getValue();
    }

    private String getStringAt(int column, int row) throws Exception {
        return getCell(column, row).getString();
    }
}
