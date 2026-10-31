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
 * P1: Tests for VBA string functions and operators derived from SAP IBP samples.
 *
 * Tests the following VBA language features:
 * - InStr, Mid, Left, Len string parsing functions
 * - Filter function (array string matching)
 * - Like operator with wildcard patterns
 * - Double-quote escape in string literals (""")
 * - On Error GoTo with Err.Description
 * - Module-level variable persistence between calls
 *
 * Test methodology: BASIC macros exercise the VBA APIs and write
 * results to cells Z1, Z2, etc. (column 25). Java code verifies via UNO.
 * Follows AAA pattern with '@TestMethod' markers in comments.
 */
public class VbaSapStringTest extends VbaCompatTest {

    private static final String MODULE_NAME = "StringTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestStringParse()\n" +
        "    ' Arrange - parse IBIS-style bracket format: =[PRDID].[].[PRD_001]\n" +
        "    Dim s As String: s = \"=[PRDID].[].[PRD_001]\"\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim pos As Integer: pos = InStr(s, \".].[].[]\")\n" +
        "    Dim result As String: result = Mid(s, pos + 7)\n" +
        "    result = Left(result, Len(result) - 1)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = Len(result)\n" +
        "    Range(\"AA1\").Value = result\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestFilter()\n" +
        "    ' Arrange\n" +
        "    Dim arr As Variant\n" +
        "    arr = Array(\"[PRDID].[].[PRD_001]\", \"[LOCREGION].[].[EUROPE]\")\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim filtered As Variant\n" +
        "    filtered = Filter(arr, \"[PRDID].[].[]\")\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = UBound(filtered) - LBound(filtered) + 1\n" +
        "    Range(\"AA1\").Value = CStr(filtered(0))\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestLikeOperator()\n" +
        "    ' Arrange\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Dim r1 As Integer, r2 As Integer\n" +
        "    If \"PRD_001\" Like \"*PRD*\" Then r1 = 1 Else r1 = 0\n" +
        "    If \"ABC_001\" Like \"*IBP*\" Then r2 = 1 Else r2 = 0\n" +
        "    Range(\"Z1\").Value = r1\n" +
        "    Range(\"AA1\").Value = r2\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestQuoteEscape()\n" +
        "    ' Arrange\n" +
        "    Dim s As String\n" +
        "\n" +
        "    ' Act\n" +
        "    s = \"=@ EPMOlapMemberO(\"\"[ABCID]\")\"\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = Len(s)\n" +
        "    Range(\"AA1\").Value = s\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestErrorHandling()\n" +
        "    ' Arrange\n" +
        "\n" +
        "    ' Act\n" +
        "    On Error GoTo ErrHandler\n" +
        "    Dim x As Integer\n" +
        "    x = CInt(\"not a number\")\n" +
        "    Range(\"Z1\").Value = 0\n" +
        "    Exit Sub\n" +
        "ErrHandler:\n" +
        "    Range(\"Z1\").Value = 1\n" +
        "    Range(\"AA1\").Value = Err.Description\n" +
        "    Resume Next\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestModuleLevelVar()\n" +
        "    ' Module-level counter is incremented in IncCounter, read here\n" +
        "    Range(\"Z1\").Value = counter\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestMidLeftInStrLen()\n" +
        "    ' Arrange\n" +
        "    Dim s As String: s = \"Hello World\"\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = CDbl(Len(s))\n" +
        "    Range(\"AA1\").Value = Left(s, 5)\n" +
        "    Range(\"Z2\").Value = CDbl(InStr(s, \"World\"))\n" +
        "    Range(\"AA2\").Value = Mid(s, 7)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestFilterMultipleMatches()\n" +
        "    ' Arrange\n" +
        "    Dim arr As Variant\n" +
        "    arr = Array(\"[PRDID].[].[PRD_001]\", \"[PRDID].[].[PRD_002]\", \"[LOCREGION].[].[EUROPE]\")\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim filtered As Variant\n" +
        "    filtered = Filter(arr, \"[PRDID].[].[]\")\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = UBound(filtered) - LBound(filtered) + 1\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestLikeQuestionMark()\n" +
        "    ' Arrange\n" +
        "\n" +
        "    ' Act & Assert - ? matches single character\n" +
        "    Dim r1 As Integer, r2 As Integer\n" +
        "    If \"ABC\" Like \"A?C\" Then r1 = 1 Else r1 = 0\n" +
        "    If \"ABC\" Like \"A?D\" Then r2 = 1 Else r2 = 0\n" +
        "    Range(\"Z1\").Value = r1\n" +
        "    Range(\"AA1\").Value = r2\n" +
        "End Sub\n" +

        "Dim counter As Integer\n" +
        "Sub IncCounter(): counter = counter + 1: End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    // --- String parsing tests ---

    @Test
    public void testStringParse() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestStringParse");
        assertEquals("Extracted PRD_001 should have length 7",
            7.0, getNumericResult(0), 0.001);
        assertEquals("Extracted member should be PRD_001",
            "PRD_001", getStringResult(0));
    }

    @Test
    public void testFilter() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestFilter");
        assertEquals("Filter should return 1 element matching [PRDID]",
            1.0, getNumericResult(0), 0.001);
        assertEquals("First filtered element should match",
            "[PRDID].[].[PRD_001]", getStringResult(0));
    }

    @Test
    public void testFilterMultipleMatches() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestFilterMultipleMatches");
        assertEquals("Filter should return 2 elements matching [PRDID]",
            2.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testLikeOperator() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestLikeOperator");
        assertEquals("\"PRD_001\" Like \"*PRD*\" should match (1)",
            1.0, getNumericResult(0), 0.001);
        assertEquals("\"ABC_001\" Like \"*IBP*\" should not match (0)",
            0.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testLikeQuestionMark() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestLikeQuestionMark");
        assertEquals("\"ABC\" Like \"A?C\" should match (1)",
            1.0, getNumericResult(0), 0.001);
        assertEquals("\"ABC\" Like \"A?D\" should not match (0)",
            0.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testQuoteEscape() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestQuoteEscape");
        assertEquals("Escaped string length should be 28",
            28.0, getNumericResult(0), 0.001);
        assertEquals("Unescaped string should be =@ EPMOlapMemberO(\"[ABCID]\")",
            "=@ EPMOlapMemberO(\"[ABCID]\")", getStringResult(0));
    }

    // --- Error handling tests ---

    @Test
    public void testErrorHandling() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestErrorHandling");
        assertEquals("Error handler should be triggered (1)",
            1.0, getNumericResult(0), 0.001);
        String errorMsg = getStringResult(0);
        assertNotNull("Err.Description should not be null", errorMsg);
    }

    // --- Module-level variable test ---

    @Test
    public void testModuleLevelVarIncrement() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "IncCounter");
        runMacro(MODULE_NAME, "IncCounter");
        runMacro(MODULE_NAME, "TestModuleLevelVar");
        assertEquals("Counter should be 2 after two increments",
            2.0, getNumericResult(0), 0.001);
    }

    // --- String function tests ---

    @Test
    public void testMidLeftInStrLen() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestMidLeftInStrLen");
        assertEquals("Len('Hello World') should be 11",
            11.0, getNumericResult(0), 0.001);
        assertEquals("Left('Hello World', 5) should be 'Hello'",
            "Hello", getStringResult(0));
        assertEquals("InStr('Hello World', 'World') should be 7",
            7.0, getNumericResult(1), 0.001);
        assertEquals("Mid('Hello World', 7) should be 'World'",
            "World", getStringResult(1));
    }
}
