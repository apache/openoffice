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
 * LibreOffice-style failure hypothesis tests for locale-sensitive number parsing
 * and nonbreaking space handling.
 *
 * Based on documented LibreOffice VBA port trouble areas:
 * - Locale-dependent number parsing (1.5 vs 1,5 format)
 * - Nonbreaking space as thousands separator in some locales
 * - VBA's CDbl/CStr/CDec functions with locale-sensitive parsing
 * - Cell value entry with different decimal separators
 * - Format function with locale-specific format strings
 * - Input parsing in numeric VBA functions
 *
 * Key hypothesis: AOO's VBA compatibility may not correctly handle
 * locale-sensitive number conversion, particularly with nonbreaking space (NBSP,
 * 0xA0) as thousands separator, which is common in European locales.
 * VBA's CDbl function uses the system locale for parsing, but AOO's BASIC
 * compiler may use a fixed locale or handle NBSP differently.
 *
 * NOTE: These tests should be run under multiple locales to exercise
 * locale-specific behavior. The NBSP test case is most meaningful under
 * locales that use NBSP as a thousands separator (e.g., de-DE, fr-FR).
 *
 * NOT integrated into the build (not in JunitTest_sc_vba.mk).
 * Status: EXPLORATORY - written but not verified against AOO runtime.
 *
 * Result columns (BASIC -> Java via UNO):
 * - Z column (index 25): numeric results
 * - AA column (index 26): string results
 */
public class VbaLibreLocaleTest extends VbaCompatTest {

    private static final String MODULE_NAME = "LibreLocaleTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +

        "'@TestMethod\n" +
        "Sub TestCDblFromString()\n" +
        "    ' Arrange - standard decimal string\n" +
        "    Dim s As String: s = \"3.14\"\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = CDbl(s)\n" +
        "    Range(\"AA1\").Value = TypeName(CDbl(s))\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestCDblWithComma()\n" +
        "    ' Arrange - comma as decimal separator (common in EU locale)\n" +
        "    Dim s As String: s = \"3,14\"\n" +
        "\n" +
        "    ' Act - CDbl should handle locale-appropriate separator\n" +
        "    ' In en-US, this should produce 3.14 (VBA CDbl uses system locale)\n" +
        "    ' In de-DE/environments using comma, this should be 3.14\n" +
        "    Dim result As Double\n" +
        "    On Error Resume Next\n" +
        "    result = CDbl(s)\n" +
        "    If Err.Number <> 0 Then\n" +
        "        Range(\"Z1\").Value = -1\n" +
        "        Range(\"Z2\").Value = Err.Number\n" +
        "    Else\n" +
        "        Range(\"Z1\").Value = result\n" +
        "    End If\n" +
        "    Err.Clear\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestNonBreakingSpaceInNumber()\n" +
        "    ' Arrange - NBSP (0xA0) as thousands separator\n" +
        "    ' VBA uses NBSP as thousands separator in system locale,\n" +
        "    ' e.g., \"1" + (char)0xA0 + "000\" should parse as 1000 in some locales\n" +
        "    Dim s As String\n" +
        "    s = \"1\" & ChrW(&hA0) & \"000\"\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim result As Variant\n" +
        "    On Error Resume Next\n" +
        "    result = CDbl(s)\n" +
        "    If Err.Number <> 0 Then\n" +
        "        Range(\"Z1\").Value = -1\n" +
        "    Else\n" +
        "        Range(\"Z1\").Value = result\n" +
        "    End If\n" +
        "    Err.Clear\n" +
        "    On Error GoTo 0\n" +
        "\n" +
        "    ' Assert - verify the string length includes NBSP\n" +
        "    Range(\"Z2\").Value = Len(s)\n" +
        "    Range(\"Z3\").Value = AscW(Mid(s, 2, 1))\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestNumberInCell()\n" +
        "    ' Arrange - enter a number string into a cell\n" +
        "    ' Cells accept string values and convert to numbers\n" +
        "    Range(\"A1\").Value = \"123.45\"\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim val As Variant\n" +
        "    val = Range(\"A1\").Value\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = val\n" +
        "    Range(\"AA1\").Value = TypeName(val)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestVBAFormatFunction()\n" +
        "    ' Arrange\n" +
        "    Dim val As Double: val = 1234.5\n" +
        "\n" +
        "    ' Act - Format with currency\n" +
        "    Dim s1 As String\n" +
        "    s1 = Format(val, \"Currency\")\n" +
        "\n" +
        "    ' Act - Format with fixed\n" +
        "    Dim s2 As String\n" +
        "    s2 = Format(val, \"Fixed\")\n" +
        "\n" +
        "    ' Act - Format with custom pattern\n" +
        "    Dim s3 As String\n" +
        "    s3 = Format(val, \"0.00\")\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"AA1\").Value = s1\n" +
        "    Range(\"AA2\").Value = s2\n" +
        "    Range(\"AA3\").Value = s3\n" +
        "    Range(\"Z1\").Value = Len(s1)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestStrFunction()\n" +
        "    ' Arrange - VBA Str() function produces locale-aware output\n" +
        "    ' Str(123.45) produces \" 123.45\" (leading space for positive)\n" +
        "    Dim val As Double: val = 123.45\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"AA1\").Value = Str(val)\n" +
        "    Range(\"Z1\").Value = Len(Str(val))\n" +
        "    Range(\"Z2\").Value = Asc(Mid(Str(val), 1, 1))\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestValFunction()\n" +
        "    ' Arrange - Val() extracts numeric prefix from string\n" +
        "    ' Val uses system locale for decimal separator\n" +
        "    Dim s1 As String: s1 = \"123.45abc\"\n" +
        "    Dim s2 As String: s2 = \"abc123\"\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = Val(s1)\n" +
        "    Range(\"Z2\").Value = Val(s2)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestCSngPrecision()\n" +
        "    ' Arrange - CSng converts to Single precision\n" +
        "    ' VBA Single has ~7 significant digits, Double has ~15\n" +
        "    Dim d As Double: d = 1.23456789\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = d\n" +
        "    Range(\"Z2\").Value = CSng(d)\n" +
        "    Range(\"AA1\").Value = TypeName(CSng(d))\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestIntegerDivisionWithNegative()\n" +
        "    ' Arrange - negative integer division behavior in VBA\n" +
        "    ' VBA \\ truncates toward zero (like C, not floor division)\n" +
        "    Dim a As Integer: a = -10\n" +
        "    Dim b As Integer: b = 3\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = a \\ b\n" +
        "    ' -10 \\ 3 = -3 (truncation toward zero)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestModOperator()\n" +
        "    ' Arrange\n" +
        "    Dim a As Integer: a = 17\n" +
        "    Dim b As Integer: b = 5\n" +
        "\n" +
        "    ' Act & Assert - VBA Mod returns remainder\n" +
        "    Range(\"Z1\").Value = a Mod b\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testCDblFromString() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCDblFromString");
        assertEquals("CDbl(\"3.14\") should be 3.14",
                3.14, getNumericResult(0), 0.001);
    }

    @Test
    public void testCDblWithComma() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCDblWithComma");
        // Behavior depends on locale - either succeeds (if comma is decimal)
        // or returns -1 (error) if not supported
        double result = getNumericResult(0);
        assertTrue("CDbl(\"3,14\") should either succeed or error, got: " + result,
                result == -1 || (result > 3.0 && result < 4.0));
    }

    @Test
    public void testNonBreakingSpaceInNumber() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestNonBreakingSpaceInNumber");
        // NBSP (0xA0) should be character code 160
        assertEquals("String should be 5 chars (1 + NBSP + 3 zeros)",
                5.0, getNumericResult(1), 0.001);
        assertEquals("Second char should be NBSP (160)",
                160.0, getNumericResult(2), 0.001);
        double result = getNumericResult(0);
        // Either parses as 1000 or fails (-1)
        assertTrue("NBSP number should parse to 1000 or fail, got: " + result,
                result == -1 || result == 1000.0);
    }

    @Test
    public void testNumberInCell() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestNumberInCell");
        // Cell value "123.45" should be stored as numeric
        assertEquals("Cell with \"123.45\" should be 123.45",
                123.45, getNumericResult(0), 0.001);
    }

    @Test
    public void testVBAFormatFunction() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestVBAFormatFunction");
        String formatted = getStringResult(0);
        assertNotNull("Format(Currency) should produce a string", formatted);
        assertTrue("Formatted currency should have positive length",
                formatted.length() > 0);
    }

    @Test
    public void testStrFunction() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestStrFunction");
        // VBA Str(123.45) = " 123.45" (leading space for positive numbers)
        String strResult = getStringResult(0);
        assertNotNull("Str() should produce a string", strResult);
        assertTrue("Str() result should have positive length", strResult.length() > 0);
    }

    @Test
    public void testValFunction() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestValFunction");
        assertEquals("Val(\"123.45abc\") should extract 123.45",
                123.45, getNumericResult(0), 0.001);
        assertEquals("Val(\"abc123\") should be 0 (no leading digits)",
                0.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testCSngPrecision() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCSngPrecision");
        String typeName = getStringResult(0);
        assertEquals("CSng should return Single type", "Single", typeName);
    }

    @Test
    public void testIntegerDivisionWithNegative() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestIntegerDivisionWithNegative");
        // VBA integer division truncates toward zero
        assertEquals("-10 \\ 3 should be -3 (truncation toward zero)",
                -3.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testModOperator() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestModOperator");
        assertEquals("17 Mod 5 should be 2",
                2.0, getNumericResult(0), 0.001);
    }
}
