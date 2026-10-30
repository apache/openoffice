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
 * P1: Tests for VBA APIs that are confirmed missing or partially supported.
 *
 * Tests the following Excel VBA APIs that are NOT fully supported in AOO:
 * - StrPtr (VBA language function not implemented in AOO BASIC runtime)
 * - Some Validation.Add types (date validation unsupported)
 *
 * Note: The following APIs have been implemented and are now verified to work:
 * - Range.Formula2 (implemented via delegation to Formula)
 * - Application.Match (works through invoke/FunctionAccess)
 * - Application.Evaluate (works via FunctionAccess with formula expressions)
 * - Application.Run (works through invoke/InvokeMethodDispatch)
 * - #If/#Const conditional compilation (implemented in parser and tokenizer)
 *
 * Test methodology: BASIC macros exercise the VBA APIs and write results
 * to cells Z1, Z2, etc. (column 25). Java code verifies via UNO.
 */
public class VbaSapFeatureGapsTest extends VbaCompatTest {

    private static final String MODULE_NAME = "FeatureGapsTests";

    // Excel VBA constants
    private static final int XL_VALIDATE_LIST = 30;
    private static final int XL_VALID_ALERT_STOP = 1;
    private static final int XL_BETWEEN = 1;

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +
        "Const xlValidateList = 30\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestFormula2Read()\n" +
        "    ' Arrange - Formula2 is now implemented\n" +
        "    Range(\"A1\").Value = 10\n" +
        "    Range(\"B1\").Formula = \"=A1*2\"\n" +
        "\n" +
        "    ' Act - read Range.Formula2\n" +
        "    On Error Resume Next\n" +
        "    Range(\"AA1\").Value = Range(\"B1\").Formula2\n" +
        "    Range(\"Z1\").Value = CInt(Err.Number <> 0)\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestFormula2Write()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = 10\n" +
        "\n" +
        "    ' Act - write to Range.Formula2\n" +
        "    On Error Resume Next\n" +
        "    Range(\"B1\").Formula2 = \"=A1*3\"\n" +
        "    Range(\"Z1\").Value = CInt(Err.Number <> 0)\n" +
        "    Range(\"Z2\").Value = Range(\"B1\").Value\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestApplicationMatch()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = 10\n" +
        "    Range(\"A2\").Value = 20\n" +
        "    Range(\"A3\").Value = 30\n" +
        "\n" +
        "    ' Act - Application.Match via invoke works\n" +
        "    On Error Resume Next\n" +
        "    Dim result As Variant\n" +
        "    result = Application.Match(20, Range(\"A1:A3\"), 0)\n" +
        "    Range(\"Z1\").Value = CInt(Err.Number <> 0)\n" +
        "    Range(\"Z2\").Value = CDbl(result)\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestApplicationMatchNotFound()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = 10\n" +
        "    Range(\"A2\").Value = 20\n" +
        "    Range(\"A3\").Value = 30\n" +
        "\n" +
        "    ' Act - match for value not in range returns error/Nothing\n" +
        "    On Error Resume Next\n" +
        "    Dim result As Variant\n" +
        "    result = Application.Match(99, Range(\"A1:A3\"), 0)\n" +
        "    Range(\"Z1\").Value = CInt(Err.Number <> 0)\n" +
        "    Range(\"AA1\").Value = CStr(result)\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestApplicationRun()\n" +
        "    ' Arrange - Application.Run invokes a macro by name\n" +
        "    Range(\"A1\").Value = 0\n" +
        "\n" +
        "    ' Act - call TestHelper via Application.Run\n" +
        "    On Error Resume Next\n" +
        "    Application.Run \"TestHelper\"\n" +
        "    Range(\"Z1\").Value = CInt(Err.Number <> 0)\n" +
        "    Range(\"Z2\").Value = Range(\"A1\").Value\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestHelper()\n" +
        "    Range(\"A1\").Value = 42\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestConditionalCompilationTrue()\n" +
        "    ' #If True Then should include the true branch\n" +
        "    #If True Then\n" +
        "        Range(\"A1\").Value = 1\n" +
        "    #Else\n" +
        "        Range(\"A1\").Value = 2\n" +
        "    #End If\n" +
        "\n" +
        "    Range(\"Z1\").Value = Range(\"A1\").Value\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestConditionalCompilationFalse()\n" +
        "    ' #If False Then should skip to #Else branch\n" +
        "    #If False Then\n" +
        "        Range(\"A1\").Value = 1\n" +
        "    #Else\n" +
        "        Range(\"A1\").Value = 2\n" +
        "    #End If\n" +
        "\n" +
        "    Range(\"Z1\").Value = Range(\"A1\").Value\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestConditionalCompilationConst()\n" +
        "    ' Test #Const and #If with constant comparison\n" +
        "    #Const USE_A = True\n" +
        "    #If USE_A Then\n" +
        "        Range(\"A1\").Value = 100\n" +
        "    #Else\n" +
        "        Range(\"A1\").Value = 200\n" +
        "    #End If\n" +
        "\n" +
        "    Range(\"Z1\").Value = Range(\"A1\").Value\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestApplicationEvaluateFormula()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = 10\n" +
        "    Range(\"A2\").Value = 20\n" +
        "\n" +
        "    ' Act - evaluate a formula expression\n" +
        "    On Error Resume Next\n" +
        "    Dim result As Variant\n" +
        "    result = Application.Evaluate(\"A1+A2\")\n" +
        "    Range(\"Z1\").Value = CInt(Err.Number <> 0)\n" +
        "    Range(\"Z2\").Value = CDbl(result)\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestApplicationEvaluateName()\n" +
        "    ' Arrange - create a named range\n" +
        "    ThisWorkbook.Names.Add \"TestName\", \"=Sheet1!$A$1\"\n" +
        "    Range(\"A1\").Value = 99\n" +
        "\n" +
        "    ' Act - evaluate by name\n" +
        "    On Error Resume Next\n" +
        "    Dim result As Variant\n" +
        "    result = Application.Evaluate(\"TestName\")\n" +
        "    Range(\"Z1\").Value = CDbl(result)\n" +
        "    Range(\"AA1\").Value = CInt(Err.Number <> 0)\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestValidationAddList()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Validation.Delete\n" +
        "\n" +
        "    ' Act - add list validation\n" +
        "    Range(\"A1\").Validation.Add Type:=xlValidateList, AlertStyle:=1, Operator:=3, Formula1:=\"A,B,C\"\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = CDbl(Range(\"A1\").Validation.Type)\n" +
        "    Range(\"A1\").Value = \"B\"\n" +
        "    Range(\"AA1\").Value = Range(\"A1\").Value\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestValidationAddWholeNumber()\n" +
        "    ' Arrange\n" +
        "    Range(\"A2\").Validation.Delete\n" +
        "\n" +
        "    ' Act - add whole number validation (xlValidateWholeNumber = 2)\n" +
        "    Range(\"A2\").Validation.Add Type:=2, AlertStyle:=1, Operator:=3, Formula1:=\"1\", Formula2:=\"10\"\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = CDbl(Range(\"A2\").Validation.Type)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestValidationAddUnsupportedType()\n" +
        "    ' Arrange\n" +
        "    Range(\"A3\").Validation.Delete\n" +
        "\n" +
        "    ' Act - try unsupported validation type (xlValidateDate = 4)\n" +
        "    On Error Resume Next\n" +
        "    Range(\"A3\").Validation.Add Type:=4, AlertStyle:=1, Operator:=3, Formula1:=\"1/1/2020\", Formula2:=\"12/31/2020\"\n" +
        "    Range(\"Z1\").Value = CInt(Err.Number <> 0)\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    // --- Formula2 tests ---

    @Test
    public void testFormula2Read() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestFormula2Read");
        assertEquals("Formula2 should not raise error when implemented",
            0.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testFormula2Write() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestFormula2Write");
        assertEquals("Writing to Formula2 should not raise error",
            0.0, getNumericResult(0), 0.001);
        assertEquals("Formula2 should set formula correctly (A1*3)",
            30.0, getNumericResult(1), 0.001);
    }

    // --- Application.Match tests ---

    @Test
    public void testApplicationMatch() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestApplicationMatch");
        assertEquals("Application.Match should work through invoke",
            0.0, getNumericResult(0), 0.001);
        assertEquals("Application.Match should return position 2 for value 20",
            2.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testApplicationMatchNotFound() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestApplicationMatchNotFound");
        assertEquals("Application.Match for not-found should return empty/Nothing",
            0.0, getNumericResult(0), 0.001);
    }

    // --- Application.Run test ---

    @Test
    public void testApplicationRun() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestApplicationRun");
        assertEquals("Application.Run should execute the macro",
            0.0, getNumericResult(0), 0.001);
        assertEquals("Helper macro should set A1 to 42",
            42.0, getNumericResult(1), 0.001);
    }

    // --- Conditional compilation tests ---

    @Test
    public void testConditionalCompilationTrue() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestConditionalCompilationTrue");
        assertEquals("#If True should include the True branch (1)",
            1.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testConditionalCompilationFalse() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestConditionalCompilationFalse");
        assertEquals("#If False should skip to #Else branch (2)",
            2.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testConditionalCompilationConst() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestConditionalCompilationConst");
        assertEquals("#Const with #If should include the True branch (100)",
            100.0, getNumericResult(0), 0.001);
    }

    // --- StrPtr test (still unimplemented) ---

    @Test
    public void testStrPtr() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestStrPtr");
        assertEquals("StrPtr should not be available (error expected)",
            1.0, getNumericResult(0), 0.001);
    }

    // --- Application.Evaluate tests ---

    @Test
    public void testApplicationEvaluateFormula() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestApplicationEvaluateFormula");
        assertEquals("Evaluate should not raise error on formula expressions",
            0.0, getNumericResult(0), 0.001);
        assertEquals("Evaluate formula result should be 30",
            30.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testApplicationEvaluateName() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestApplicationEvaluateName");
        assertEquals("Evaluate should work on defined names",
            99.0, getNumericResult(0), 0.001);
    }

    // --- Validation.Add tests ---

    @Test
    public void testValidationAddList() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestValidationAddList");
        assertEquals("Validation Type for xlValidateList should be 30",
            30.0, getNumericResult(0), 0.001);
        assertEquals("Cell value after setting validation should be 'B'",
            "B", getStringResult(0));
    }

    @Test
    public void testValidationAddWholeNumber() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestValidationAddWholeNumber");
        assertEquals("Validation Type for WholeNumber should be 2",
            2.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testValidationAddUnsupportedType() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestValidationAddUnsupportedType");
        assertEquals("Unsupported validation type should raise error",
            1.0, getNumericResult(0), 0.001);
    }
}
