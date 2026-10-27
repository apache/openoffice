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
 * LibreOffice-style failure hypothesis tests for Variant type handling
 * in VBA-compatible BASIC.
 *
 * Based on documented LibreOffice VBA port trouble areas:
 * - VBA-specific Variant subtypes (Empty, Null, Nothing, Error)
 * - TypeName behavior for different Variant values
 * - VarType codes matching VBA constants
 * - Argument passing through UNO bridge preserves Variant semantics
 * - Return type boxing/unboxing in VBA user functions
 * - IsEmpty / IsNull / IsNumeric / IsError checks
 *
 * Key hypothesis: AOO's VBA compatibility layer may not correctly
 * translate VBA Variant subtypes through the UNO Any/AnySequence bridge,
 * causing type mismatches or loss of Empty/Null semantics.
 *
 * NOT integrated into the build (not in JunitTest_sc_vba.mk).
 * Status: EXPLORATORY - written but not verified against AOO runtime.
 *
 * Result columns (BASIC -> Java via UNO):
 * - Z column (index 25): numeric results (VarType codes, boolean flags)
 * - AA column (index 26): string results (TypeName, string conversions)
 */
public class VbaLibreVariantTest extends VbaCompatTest {

    private static final String MODULE_NAME = "LibreVariantTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +

        "'@TestMethod\n" +
        "Sub TestEmptyVariant()\n" +
        "    ' Arrange - declare without assignment\n" +
        "    Dim v As Variant\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = IIf(IsEmpty(v), 1, 0)\n" +
        "    Range(\"Z2\").Value = VarType(v)\n" +
        "    Range(\"AA1\").Value = TypeName(v)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestNullVariant()\n" +
        "    ' Arrange\n" +
        "    Dim v As Variant\n" +
        "    v = Null\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = IIf(IsNull(v), 1, 0)\n" +
        "    Range(\"Z2\").Value = VarType(v)\n" +
        "    Range(\"AA1\").Value = TypeName(v)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestNothingVariant()\n" +
        "    ' Arrange\n" +
        "    Dim r As Variant\n" +
        "    Set r = Nothing\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = IIf(r Is Nothing, 1, 0)\n" +
        "    Range(\"AA1\").Value = TypeName(r)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestNumericVariantTypes()\n" +
        "    ' Arrange - test various numeric types\n" +
        "    Dim d As Double: d = 3.14\n" +
        "    Dim i As Integer: i = 42\n" +
        "    Dim l As Long: l = 1000000\n" +
        "\n" +
        "    ' Act & Assert - VarType constants:\n" +
        "    ' vbDouble=5, vbInteger=2, vbLong=3\n" +
        "    Range(\"Z1\").Value = VarType(d)\n" +
        "    Range(\"Z2\").Value = VarType(i)\n" +
        "    Range(\"Z3\").Value = VarType(l)\n" +
        "    Range(\"AA1\").Value = TypeName(d)\n" +
        "    Range(\"AA2\").Value = TypeName(i)\n" +
        "    Range(\"AA3\").Value = TypeName(l)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestStringVariantType()\n" +
        "    ' Arrange\n" +
        "    Dim s As Variant\n" +
        "    s = \"Hello VBA\"\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = VarType(s)\n" +
        "    Range(\"AA1\").Value = TypeName(s)\n" +
        "    Range(\"Z2\").Value = Len(s)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestErrorVariant()\n" +
        "    ' Arrange - create an error variant\n" +
        "    Dim v As Variant\n" +
        "    v = CVErr(9) ' xlErrRef\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = IIf(IsError(v), 1, 0)\n" +
        "    Range(\"Z2\").Value = VarType(v)\n" +
        "    Range(\"AA1\").Value = TypeName(v)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestVariantArrayPassing()\n" +
        "    ' Arrange - create array and pass to function\n" +
        "    Dim arr(1 To 3) As Variant\n" +
        "    arr(1) = 10\n" +
        "    arr(2) = 20\n" +
        "    arr(3) = 30\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim result As Variant\n" +
        "    result = SumVariantArray(arr)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = result\n" +
        "    Range(\"AA1\").Value = TypeName(result)\n" +
        "End Sub\n" +
        "\n" +
        "Function SumVariantArray(arr As Variant) As Double\n" +
        "    Dim sum As Double: sum = 0\n" +
        "    Dim i As Long\n" +
        "    For i = LBound(arr) To UBound(arr)\n" +
        "        sum = sum + arr(i)\n" +
        "    Next i\n" +
        "    SumVariantArray = sum\n" +
        "End Function\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestBooleanVariant()\n" +
        "    ' Arrange\n" +
        "    Dim b As Variant\n" +
        "    b = True\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = VarType(b)\n" +
        "    Range(\"AA1\").Value = TypeName(b)\n" +
        "    Range(\"Z2\").Value = IIf(b, 1, 0)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestDateVariant()\n" +
        "    ' Arrange\n" +
        "    Dim d As Variant\n" +
        "    d = DateSerial(2023, 6, 15)\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = VarType(d)\n" +
        "    Range(\"AA1\").Value = TypeName(d)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestIntegerDivisionVariant()\n" +
        "    ' Arrange\n" +
        "    Dim a As Variant, b As Variant\n" +
        "    a = 10\n" +
        "    b = 3\n" +
        "\n" +
        "    ' Act & Assert - VBA \"\\\" is integer division\n" +
        "    Range(\"Z1\").Value = a / b\n" +
        "    Range(\"Z2\").Value = a \\ b\n" +
        "    Range(\"AA1\").Value = TypeName(a / b)\n" +
        "    Range(\"AA2\").Value = TypeName(a \\ b)\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testEmptyVariant() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestEmptyVariant");
        // Empty variant: IsEmpty should be True, VarType=0, TypeName="Empty"
        assertEquals("IsEmpty(unassigned Variant) should be True",
                1.0, getNumericResult(0), 0.001);
        assertEquals("VarType(Empty) should be 0",
                0.0, getNumericResult(1), 0.001);
        assertEquals("TypeName(Empty) should be 'Empty'",
                "Empty", getStringResult(0));
    }

    @Test
    public void testNullVariant() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestNullVariant");
        assertEquals("IsNull(Null) should be True",
                1.0, getNumericResult(0), 0.001);
        assertEquals("VarType(Null) should be 1 (vbNull)",
                1.0, getNumericResult(1), 0.001);
        assertEquals("TypeName(Null) should be 'Null'",
                "Null", getStringResult(0));
    }

    @Test
    public void testNothingVariant() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestNothingVariant");
        assertEquals("Nothing check should return True (1)",
                1.0, getNumericResult(0), 0.001);
        assertEquals("TypeName(Nothing) should be 'Nothing' in VBA mode",
                "Nothing", getStringResult(0));
    }

    @Test
    public void testNumericVariantTypes() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestNumericVariantTypes");
        // vbDouble=5, vbInteger=2, vbLong=3
        assertEquals("VarType(Double 3.14) should be 5",
                5.0, getNumericResult(0), 0.001);
        assertEquals("VarType(Integer 42) should be 2",
                2.0, getNumericResult(1), 0.001);
        assertEquals("VarType(Long 1000000) should be 3",
                3.0, getNumericResult(2), 0.001);
        assertEquals("TypeName(Double) should be 'Double'",
                "Double", getStringResult(0));
        assertEquals("TypeName(Integer) should be 'Integer'",
                "Integer", getStringResult(1));
        assertEquals("TypeName(Long) should be 'Long'",
                "Long", getStringResult(2));
    }

    @Test
    public void testStringVariantType() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestStringVariantType");
        assertEquals("VarType(String) should be 8 (vbString)",
                8.0, getNumericResult(0), 0.001);
        assertEquals("TypeName(String) should be 'String'",
                "String", getStringResult(0));
        assertEquals("Len of 'Hello VBA' should be 9",
                9.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testErrorVariant() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestErrorVariant");
        assertEquals("IsError(CVErr(9)) should be True",
                1.0, getNumericResult(0), 0.001);
        assertEquals("VarType(Error) should be 10 (vbError)",
                10.0, getNumericResult(1), 0.001);
        assertEquals("TypeName(CVErr) should be 'Error'",
                "Error", getStringResult(0));
    }

    @Test
    public void testVariantArrayPassing() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestVariantArrayPassing");
        assertEquals("SumVariantArray([10,20,30]) should be 60",
                60.0, getNumericResult(0), 0.001);
        assertEquals("Return type should be 'Double'",
                "Double", getStringResult(0));
    }

    @Test
    public void testBooleanVariant() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestBooleanVariant");
        assertEquals("VarType(Boolean) should be 11 (vbBoolean)",
                11.0, getNumericResult(0), 0.001);
        assertEquals("TypeName(Boolean) should be 'Boolean'",
                "Boolean", getStringResult(0));
        assertEquals("Boolean True should be 1",
                1.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testDateVariant() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestDateVariant");
        assertEquals("VarType(Date) should be 7 (vbDate)",
                7.0, getNumericResult(0), 0.001);
        assertEquals("TypeName(Date) should be 'Date'",
                "Date", getStringResult(0));
    }

    @Test
    public void testIntegerDivisionVariant() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestIntegerDivisionVariant");
        // 10 / 3 = 3.333..., 10 \ 3 = 3
        double regularDiv = getNumericResult(0);
        assertTrue("10/3 should be ~3.33, got: " + regularDiv,
                regularDiv > 3.3 && regularDiv < 3.4);
        assertEquals("10 \\ 3 should be 3 (integer division)",
                3.0, getNumericResult(1), 0.001);
        assertEquals("TypeName(10/3) should be 'Double'",
                "Double", getStringResult(0));
        assertEquals("TypeName(10\\3) should be 'Integer'",
                "Integer", getStringResult(1));
    }
}
