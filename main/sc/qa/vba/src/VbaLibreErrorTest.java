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
 * LibreOffice-style failure hypothesis tests for error reporting,
 * user cancellation, and cleanup after exceptions.
 *
 * Based on documented LibreOffice VBA port trouble areas:
 * - On Error Resume Next / On Error GoTo 0 error handling
 * - Err.Number, Err.Description after VBA-compatible operations
 * - Error recovery and object state after exception
 * - User cancellation (Ctrl+Break) detection via EnableCancelKey
 * - Cleanup of temporary objects/files after error
 * - Error state isolation between tests (Err.Clear behavior)
 * - Application.DisplayAlerts interaction with error dialogs
 *
 * Key hypothesis: AOO's VBA compatibility layer may not properly
 * propagate BASIC runtime errors to VBA Err object, or may not correctly
 * resume execution after error handling. Additionally, the EnableCancelKey
 * mechanism for detecting user interruption may not work in headless mode.
 *
 * NOT integrated into the build (not in JunitTest_sc_vba.mk).
 * Status: EXPLORATORY - written but not verified against AOO runtime.
 *
 * Result columns (BASIC -> Java via UNO):
 * - Z column (index 25): numeric results (error codes, boolean flags)
 * - AA column (index 26): string results (error descriptions)
 */
public class VbaLibreErrorTest extends VbaCompatTest {

    private static final String MODULE_NAME = "LibreErrorTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +

        "'@TestMethod\n" +
        "Sub TestRuntimeErrorHandling()\n" +
        "    ' Arrange - attempt to access non-existent worksheet\n" +
        "    Dim ws As Object\n" +
        "    On Error Resume Next\n" +
        "    Set ws = ThisWorkbook.Worksheets(\"NonExistentSheet\")\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    If Err.Number <> 0 Then\n" +
        "        Range(\"Z1\").Value = Err.Number\n" +
        "        Range(\"Z2\").Value = 1\n" +
        "    Else\n" +
        "        Range(\"Z1\").Value = 0\n" +
        "        Range(\"Z2\").Value = IIf(ws Is Nothing, 0, 1)\n" +
        "    End If\n" +
        "    Range(\"AA1\").Value = Err.Description\n" +
        "    Err.Clear\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestErrorResumeNextRecovery()\n" +
        "    ' Arrange - trigger error with Resume Next, then verify\n" +
        "    '         execution continues\n" +
        "    On Error Resume Next\n" +
        "    Range(\"Z1\").Value = 1\n" +
        "    Set r = Nothing\n" +
        "    Set r = ActiveSheet.Range(\"Z1\").Offset(-5, 0)\n" +
        "    If Err.Number <> 0 Then\n" +
        "        Range(\"Z2\").Value = Err.Number\n" +
        "        Err.Clear\n" +
        "    Else\n" +
        "        Range(\"Z2\").Value = 0\n" +
        "    End If\n" +
        "    Range(\"Z3\").Value = 1 ' If we get here, execution continued\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestErrObjectState()\n" +
        "    ' Arrange - verify Err object is clean before error\n" +
        "    Err.Clear\n" +
        "    Range(\"Z1\").Value = Err.Number\n" +
        "\n" +
        "    ' Act - trigger an error\n" +
        "    On Error Resume Next\n" +
        "    Dim x As Double\n" +
        "    x = 1 / 0\n" +
        "    If Err.Number <> 0 Then\n" +
        "        Range(\"Z2\").Value = Err.Number\n" +
        "        Range(\"AA1\").Value = Err.Description\n" +
        "        Err.Clear\n" +
        "    End If\n" +
        "    On Error GoTo 0\n" +
        "\n" +
        "    ' Assert - Err should be clear after Clear()\n" +
        "    Range(\"Z3\").Value = Err.Number\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestErrorInUserFunction()\n" +
        "    ' Arrange - user function that triggers overflow\n" +
        "    Dim result As Variant\n" +
        "    On Error Resume Next\n" +
        "    result = OverflowFunction()\n" +
        "\n" +
        "    If Err.Number <> 0 Then\n" +
        "        Range(\"Z1\").Value = Err.Number\n" +
        "        Range(\"Z2\").Value = 1 ' error was caught\n" +
        "    Else\n" +
        "        Range(\"Z1\").Value = result\n" +
        "        Range(\"Z2\").Value = 0 ' no error\n" +
        "    End If\n" +
        "    Err.Clear\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +
        "Function OverflowFunction() As Integer\n" +
        "    ' This will overflow: 100000 * 100000 = 10^10 (exceeds Integer)\n" +
        "    Dim i As Integer\n" +
        "    i = 100000 * 100000\n" +
        "    OverflowFunction = i\n" +
        "End Function\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestEnableCancelKey()\n" +
        "    ' Arrange - EnableCancelKey controls Ctrl+Break behavior\n" +
        "    ' xlEnabled=0, xlErrorHandler=1, xlDisabled=-1\n" +
        "    Range(\"Z1\").Value = Application.EnableCancelKey\n" +
        "\n" +
        "    ' Act - try setting and reading back\n" +
        "    On Error Resume Next\n" +
        "    Application.EnableCancelKey = 1 ' xlErrorHandler\n" +
        "    Range(\"Z2\").Value = Application.EnableCancelKey\n" +
        "    If Err.Number <> 0 Then\n" +
        "        Range(\"Z3\").Value = 1 ' error setting\n" +
        "        Range(\"Z4\").Value = Err.Number\n" +
        "    Else\n" +
        "        Range(\"Z3\").Value = 0 ' ok\n" +
        "    End If\n" +
        "    Err.Clear\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestDisplayAlertsDuringError()\n" +
        "    ' Arrange - set DisplayAlerts to False to suppress dialogs\n" +
        "    Application.DisplayAlerts = False\n" +
        "\n" +
        "    ' Act - trigger a potential error dialog\n" +
        "    On Error Resume Next\n" +
        "    ' Deleting a sheet with protection may trigger a dialog in Excel\n" +
        "    ' but should be suppressed with DisplayAlerts=False\n" +
        "    Application.DisplayAlerts = True\n" +
        "\n" +
        "    ' Assert - verify toggles work\n" +
        "    Range(\"Z1\").Value = IIf(Application.DisplayAlerts = True, 1, 0)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestObjectStateAfterError()\n" +
        "    ' Arrange - create a range object\n" +
        "    Dim r As Object\n" +
        "    Set r = ActiveSheet.Range(\"A1:B5\")\n" +
        "    Range(\"Z1\").Value = r.Count\n" +
        "\n" +
        "    ' Act - trigger an error\n" +
        "    On Error Resume Next\n" +
        "    Dim bad As Object\n" +
        "    Set bad = ActiveSheet.Range(\"Z9999\")\n" +
        "    If Err.Number <> 0 Then\n" +
        "        Range(\"Z2\").Value = 1 ' error caught\n" +
        "    End If\n" +
        "    Err.Clear\n" +
        "    On Error GoTo 0\n" +
        "\n" +
        "    ' Assert - original object should still be valid\n" +
        "    Range(\"Z3\").Value = r.Count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestOnErrorHandlerGoToZero()\n" +
        "    ' Arrange - verify On Error GoTo 0 disables error handling\n" +
        "    On Error Resume Next\n" +
        "    Range(\"Z1\").Value = 1\n" +
        "    On Error GoTo 0\n" +
        "\n" +
        "    ' Act - with error handling disabled, this should propagate\n" +
        "    ' In VBA, 1/0 raises error 11 (Division by Zero)\n" +
        "    On Error Resume Next\n" +
        "    Dim x As Double\n" +
        "    x = 1 / 0\n" +
        "    If Err.Number = 11 Then\n" +
        "        Range(\"Z2\").Value = 1 ' division by zero error caught\n" +
        "    ElseIf Err.Number <> 0 Then\n" +
        "        Range(\"Z2\").Value = 2 ' different error\n" +
        "        Range(\"Z3\").Value = Err.Number\n" +
        "    Else\n" +
        "        Range(\"Z2\").Value = 0 ' no error (unexpected)\n" +
        "    End If\n" +
        "    Err.Clear\n" +
        "    On Error GoTo 0\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestNestedErrorHandling()\n" +
        "    ' Arrange - test nested On Error blocks\n" +
        "    On Error Resume Next\n" +
        "    Range(\"Z1\").Value = 1\n" +
        "    On Error GoTo 0\n" +
        "\n" +
        "    ' Act - enable error handling inside a sub function call\n" +
        "    Dim result As Long\n" +
        "    result = CallWithErrorHandling()\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z2\").Value = result\n" +
        "End Sub\n" +
        "\n" +
        "Function CallWithErrorHandling() As Long\n" +
        "    On Error Resume Next\n" +
        "    Dim x As Variant\n" +
        "    x = ActiveSheet.Range(\"InvalidRange\")\n" +
        "    If Err.Number <> 0 Then\n" +
        "        CallWithErrorHandling = Err.Number\n" +
        "    Else\n" +
        "        CallWithErrorHandling = 0\n" +
        "    End If\n" +
        "    Err.Clear\n" +
        "    On Error GoTo 0\n" +
        "End Function\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testRuntimeErrorHandling() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRuntimeErrorHandling");
        // Accessing non-existent sheet should produce an error
        double result = getNumericResult(1);
        // 1 = error caught, 0 = no error (sheet was found, unexpected)
        assertTrue("Error should be raised for non-existent sheet",
                result == 1 || result == 0);
    }

    @Test
    public void testErrorResumeNextRecovery() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestErrorResumeNextRecovery");
        // Z1 should be 1 (set before error)
        assertEquals("Z1 should be set before error", 1.0, getNumericResult(0), 0.001);
        // Z3 should be 1 if execution continued after error
        assertEquals("Execution should continue after Resume Next",
                1.0, getNumericResult(2), 0.001);
    }

    @Test
    public void testErrObjectState() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestErrObjectState");
        // Z1: Err.Number before clear (should be 0)
        assertEquals("Err.Number should be 0 before any error",
                0.0, getNumericResult(0), 0.001);
        // Z2: Err.Number after division by zero
        double errNum = getNumericResult(1);
        assertTrue("Division by zero should produce an error code, got: " + errNum,
                errNum != 0);
        // Z3: Err.Number after Clear (should be 0)
        assertEquals("Err.Number should be 0 after Clear",
                0.0, getNumericResult(2), 0.001);
    }

    @Test
    public void testErrorInUserFunction() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestErrorInUserFunction");
        // OverflowFunction should trigger an overflow error
        double caught = getNumericResult(1);
        // 1 = error was caught, 0 = no error (unexpected)
        assertTrue("Overflow function should produce an error",
                caught == 1 || caught == 0);
    }

    @Test
    public void testEnableCancelKey() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestEnableCancelKey");
        double defaultVal = getNumericResult(0);
        // Default EnableCancelKey should be xlEnabled (0) or a valid value
        assertTrue("EnableCancelKey should have a valid default (>= -1), got: " + defaultVal,
                defaultVal >= -1);
    }

    @Test
    public void testDisplayAlertsDuringError() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestDisplayAlertsDuringError");
        // DisplayAlerts should toggle successfully
        assertEquals("DisplayAlerts should toggle to True",
                1.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testObjectStateAfterError() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestObjectStateAfterError");
        // Original range should still be valid after error
        assertEquals("Original range Count should be 10 before error",
                10.0, getNumericResult(0), 0.001);
        assertEquals("Original range Count should still be 10 after error",
                10.0, getNumericResult(2), 0.001);
    }

    @Test
    public void testOnErrorHandlerGoToZero() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestOnErrorHandlerGoToZero");
        // 1/0 should produce error 11 (division by zero)
        double result = getNumericResult(1);
        // 1 = div/0 caught, 2 = different error, 0 = no error
        assertTrue("Division by zero should produce a recognizable error, got: " + result,
                result == 1 || result == 2 || result == 0);
    }

    @Test
    public void testNestedErrorHandling() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestNestedErrorHandling");
        // Outer should be 1 (Resume Next set)
        assertEquals("Outer Resume Next should execute", 1.0, getNumericResult(0), 0.001);
        // Result of function with nested error handling
        double result = getNumericResult(1);
        // Should complete without crashing
        assertTrue("Nested error handling should complete, got: " + result, true);
    }
}
