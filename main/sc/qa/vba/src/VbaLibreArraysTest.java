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
 * LibreOffice-style failure hypothesis tests for VBA array bounds and indexing.
 *
 * Based on documented LibreOffice VBA port trouble areas:
 * - VBA implicit arrays (Dim arr(n)) default to lower bound 0 with Option Base 0,
 *   or lower bound 1 with Option Base 1. Explicit bounds (To clause) are preserved.
 * - The VBA Array() function is always 0-based regardless of Option Base.
 * - Split() returns 0-based arrays.
 * - Range.Value/Value2 returns 1-based arrays (1 to rows, 1 to columns).
 * - For Each iteration order follows declaration order.
 *
 * Key hypothesis: AOO's VBA compatibility may have issues distinguishing
 * implicit default bound (0-based vs 1-based), off-by-one errors with
 * LBound/UBound, or incorrect Array() indexing.
 *
 * NOT integrated into the build (not in JunitTest_sc_vba.mk).
 * Status: EXPLORATORY - written but not verified against AOO runtime.
 *
 * Result columns (BASIC -> Java via UNO):
 * - Z column (index 25): numeric results
 * - AA column (index 26): string results
 */
public class VbaLibreArraysTest extends VbaCompatTest {

    private static final String MODULE_NAME = "LibreArrayTests";
    private static final String MODULE_OPTBASE_NAME = "LibreArrayOptBaseTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestImplicitArrayOptionBase0()\n" +
        "    ' VBA default: Option Base 0 is assumed when no Option Base statement\n" +
        "    Dim arr(3) As Variant\n" +
        "    Range(\"Z1\").Value = LBound(arr)\n" +
        "    Range(\"Z2\").Value = UBound(arr)\n" +
        "End Sub\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestExplicitArrayFromTo()\n" +
        "    ' Explicit bounds should be preserved\n" +
        "    Dim arr(1 To 3) As Variant\n" +
        "    arr(1) = 100\n" +
        "    arr(2) = 200\n" +
        "    arr(3) = 300\n" +
        "    Range(\"Z1\").Value = LBound(arr)\n" +
        "    Range(\"Z2\").Value = UBound(arr)\n" +
        "    Range(\"Z3\").Value = arr(1)\n" +
        "    Range(\"Z4\").Value = arr(3)\n" +
        "End Sub\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestExplicitArrayZeroTo()\n" +
        "    Dim arr(0 To 3) As Variant\n" +
        "    Range(\"Z1\").Value = LBound(arr)\n" +
        "    Range(\"Z2\").Value = UBound(arr)\n" +
        "End Sub\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestRangeValue2SingleCellLB()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = 42\n" +
        "\n" +
        "    ' Act - Range.Value2 on single cell returns a scalar, not an array\n" +
        "    Dim val As Variant\n" +
        "    val = ActiveSheet.Range(\"A1\").Value2\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = val\n" +
        "End Sub\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestRangeValue2MultiCellLBound()\n" +
        "    ' Arrange\n" +
        "    Range(\"A1\").Value = 10\n" +
        "    Range(\"A2\").Value = 20\n" +
        "    Range(\"A3\").Value = 30\n" +
        "\n" +
        "    ' Act - Range.Value2 on multi-cell range returns 1-based array in VBA\n" +
        "    Dim arr As Variant\n" +
        "    arr = ActiveSheet.Range(\"A1:A3\").Value2\n" +
        "\n" +
        "    ' Assert - Range.Value arrays are 1-based in VBA\n" +
        "    Range(\"Z1\").Value = LBound(arr, 1)\n" +
        "    Range(\"Z2\").Value = UBound(arr, 1)\n" +
        "    Range(\"Z3\").Value = arr(1, 1)\n" +
        "    Range(\"Z4\").Value = arr(3, 1)\n" +
        "End Sub\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestDynamicArrayReDim()\n" +
        "    ' ReDim with explicit bounds preserves VBA semantics\n" +
        "    Dim arr() As Variant\n" +
        "    ReDim arr(1 To 5)\n" +
        "    Dim i As Long\n" +
        "    For i = 1 To 5\n" +
        "        arr(i) = i * 10\n" +
        "    Next i\n" +
        "    Range(\"Z1\").Value = LBound(arr)\n" +
        "    Range(\"Z2\").Value = UBound(arr)\n" +
        "    Range(\"Z3\").Value = arr(1)\n" +
        "    Range(\"Z4\").Value = arr(5)\n" +
        "    Range(\"Z5\").Value = arr(0)\n" +
        "    ' arr(0) in a 1-to-5 array should be Empty (0 in VBA)\n" +
        "End Sub\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestForEachArrayIteration()\n" +
        "    Dim arr(1 To 3) As Variant\n" +
        "    arr(1) = 10\n" +
        "    arr(2) = 20\n" +
        "    arr(3) = 30\n" +
        "    Dim val As Variant\n" +
        "    Dim sum As Long: sum = 0\n" +
        "    Dim count As Long: count = 0\n" +
        "    For Each val In arr\n" +
        "        sum = sum + val\n" +
        "        count = count + 1\n" +
        "    Next val\n" +
        "    Range(\"Z1\").Value = sum\n" +
        "    Range(\"Z2\").Value = count\n" +
        "End Sub\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestArrayFromSplitFunction()\n" +
        "    Dim arr As Variant\n" +
        "    arr = Split(\"a,b,c\", \",\")\n" +
        "    Range(\"Z1\").Value = LBound(arr)\n" +
        "    Range(\"Z2\").Value = UBound(arr)\n" +
        "    Range(\"Z3\").Value = arr(0)\n" +
        "    Range(\"Z4\").Value = arr(2)\n" +
        "    Range(\"Z5\").Value = UBound(arr) - LBound(arr) + 1\n" +
        "End Sub\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestArrayFunctionZeroBased()\n" +
        "    ' Array() function is always 0-based in VBA regardless of Option Base\n" +
        "    Dim arr As Variant\n" +
        "    arr = Array(10, 20, 30)\n" +
        "    Range(\"Z1\").Value = LBound(arr)\n" +
        "    Range(\"Z2\").Value = UBound(arr)\n" +
        "    Range(\"Z3\").Value = arr(0)\n" +
        "    Range(\"Z4\").Value = arr(2)\n" +
        "End Sub\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestSingleElementArray()\n" +
        "    Range(\"A1\").Value = 5\n" +
        "    Dim val As Variant\n" +
        "    val = ActiveSheet.Range(\"A1\").Value2\n" +
        "    Range(\"Z1\").Value = val\n" +
        "    Dim isArray As Integer\n" +
        "    If IsArray(val) Then\n" +
        "        isArray = 1\n" +
        "    Else\n" +
        "        isArray = 0\n" +
        "    End If\n" +
        "    Range(\"Z2\").Value = isArray\n" +
        "End Sub\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestVariantArrayIsArrayCheck()\n" +
        "    Range(\"A1:A3\").Value = Array(1, 2, 3)\n" +
        "    Dim val As Variant\n" +
        "    val = ActiveSheet.Range(\"A1:A3\").Value2\n" +
        "    Dim isArray As Integer\n" +
        "    If IsArray(val) Then\n" +
        "        isArray = 1\n" +
        "    Else\n" +
        "        isArray = 0\n" +
        "    End If\n" +
        "    Range(\"Z1\").Value = isArray\n" +
        "    Range(\"Z2\").Value = UBound(val, 1) - LBound(val, 1) + 1\n" +
        "End Sub\n";

    private static final String BASIC_SOURCE_OPTBASE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +
        "Option Base 1\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestImplicitArrayOptionBase1()\n" +
        "    ' With Option Base 1, Dim arr(3) -> 1 To 3\n" +
        "    Dim arr(3) As Variant\n" +
        "    Range(\"Z1\").Value = LBound(arr)\n" +
        "    Range(\"Z2\").Value = UBound(arr)\n" +
        "End Sub\n" +
        "\n" +
        "'@TestMethod\n" +
        "Sub TestArrayFunctionStillZeroBased()\n" +
        "    ' Array() function is always 0-based even with Option Base 1\n" +
        "    Dim arr As Variant\n" +
        "    arr = Array(10, 20, 30)\n" +
        "    Range(\"Z1\").Value = LBound(arr)\n" +
        "    Range(\"Z2\").Value = UBound(arr)\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
        addBasicModule(MODULE_OPTBASE_NAME, BASIC_SOURCE_OPTBASE);
    }

    @Test
    public void testImplicitArrayOptionBase0() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestImplicitArrayOptionBase0");
        // With default Option Base 0, Dim arr(3) -> 0 To 3
        assertEquals("Dim arr(3) without Option Base should have LBound 0",
                0.0, getNumericResult(0), 0.001);
        assertEquals("Dim arr(3) without Option Base should have UBound 3",
                3.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testExplicitArrayFromTo() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestExplicitArrayFromTo");
        assertEquals("Dim arr(1 To 3) should have LBound 1",
                1.0, getNumericResult(0), 0.001);
        assertEquals("Dim arr(1 To 3) should have UBound 3",
                3.0, getNumericResult(1), 0.001);
        assertEquals("arr(1) should be 100", 100.0, getNumericResult(2), 0.001);
        assertEquals("arr(3) should be 300", 300.0, getNumericResult(3), 0.001);
    }

    @Test
    public void testExplicitArrayZeroTo() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestExplicitArrayZeroTo");
        assertEquals("Dim arr(0 To 3) should have LBound 0",
                0.0, getNumericResult(0), 0.001);
        assertEquals("Dim arr(0 To 3) should have UBound 3",
                3.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testRangeValue2SingleCellLB() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeValue2SingleCellLB");
        assertEquals("Value2 of single cell should return the scalar value 42",
                42.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testRangeValue2MultiCellLBound() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRangeValue2MultiCellLBound");
        double lbound = getNumericResult(0);
        double ubound = getNumericResult(1);
        assertEquals("VBA Range.Value LBound should be 1 (1-based arrays)",
                1.0, lbound, 0.001);
        assertEquals("VBA Range.Value UBound should be 3 (3 rows)",
                3.0, ubound, 0.001);
        assertEquals("arr(1,1) should be 10 (first element)",
                10.0, getNumericResult(2), 0.001);
        assertEquals("arr(3,1) should be 30 (last element)",
                30.0, getNumericResult(3), 0.001);
    }

    @Test
    public void testDynamicArrayReDim() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestDynamicArrayReDim");
        assertEquals("ReDim arr(1 To 5): LBound should be 1",
                1.0, getNumericResult(0), 0.001);
        assertEquals("ReDim arr(1 To 5): UBound should be 5",
                5.0, getNumericResult(1), 0.001);
        assertEquals("arr(1) should be 10", 10.0, getNumericResult(2), 0.001);
        assertEquals("arr(5) should be 50", 50.0, getNumericResult(3), 0.001);
        // arr(0) in a 1-to-5 array - in VBA this is Empty, which is 0 as double
        assertEquals("arr(0) should be Empty (0) in 1-based array",
                0.0, getNumericResult(4), 0.001);
    }

    @Test
    public void testForEachArrayIteration() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestForEachArrayIteration");
        assertEquals("Sum of 10+20+30 should be 60",
                60.0, getNumericResult(0), 0.001);
        assertEquals("For Each count should be 3",
                3.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testArrayFromSplitFunction() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestArrayFromSplitFunction");
        assertEquals("Split LBound should be 0 (0-based)",
                0.0, getNumericResult(0), 0.001);
        assertEquals("Split UBound should be 2 (3 elements)",
                2.0, getNumericResult(1), 0.001);
        assertEquals("Split arr(0) should be 'a' (0.0 for string comparison)",
                0.0, getNumericResult(2), 0.001);
        assertEquals("Split arr(2) should be 'c' (0.0 for string comparison)",
                0.0, getNumericResult(3), 0.001);
        assertEquals("Split array length should be 3",
                3.0, getNumericResult(4), 0.001);
    }

    @Test
    public void testArrayFunctionZeroBased() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestArrayFunctionZeroBased");
        assertEquals("Array() LBound should be 0 (0-based)",
                0.0, getNumericResult(0), 0.001);
        assertEquals("Array() UBound should be 2 (3 elements)",
                2.0, getNumericResult(1), 0.001);
        assertEquals("Array(0) should be 10",
                10.0, getNumericResult(2), 0.001);
        assertEquals("Array(2) should be 30",
                30.0, getNumericResult(3), 0.001);
    }

    @Test
    public void testImplicitArrayOptionBase1() throws Exception {
        clearResultRow();
        runMacro(MODULE_OPTBASE_NAME, "TestImplicitArrayOptionBase1");
        // With Option Base 1, Dim arr(3) -> 1 To 3
        assertEquals("Dim arr(3) with Option Base 1 should have LBound 1",
                1.0, getNumericResult(0), 0.001);
        assertEquals("Dim arr(3) with Option Base 1 should have UBound 3",
                3.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testArrayFunctionStillZeroBased() throws Exception {
        clearResultRow();
        runMacro(MODULE_OPTBASE_NAME, "TestArrayFunctionStillZeroBased");
        assertEquals("Array() with Option Base 1 should still have LBound 0",
                0.0, getNumericResult(0), 0.001);
        assertEquals("Array() with Option Base 1 should still have UBound 2",
                2.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testSingleElementArray() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestSingleElementArray");
        assertEquals("Single cell Value2 should be 5",
                5.0, getNumericResult(0), 0.001);
        assertEquals("Single cell Value2 should NOT be array (0)",
                0.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testVariantArrayIsArrayCheck() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestVariantArrayIsArrayCheck");
        assertEquals("Range.Value2 of multi-cell should be array (1)",
                1.0, getNumericResult(0), 0.001);
        double dimSize = getNumericResult(1);
        assertTrue("Array dimension should be positive, got: " + dimSize, dimSize > 0);
    }
}
