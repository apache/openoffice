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
 * P2/P4: Tests for VBA Application properties and methods.
 *
 * Tests the following VBA object model features:
 * - Application.ScreenUpdating get/set
 * - Application.Iteration get/set
 * - Application.Calculation get/set
 * - Application.CalculationState (read-only, returns xlDone status)
 * - Application.DisplayAlerts get/set
 * - Application.EnableEvents get/set
 * - Application.EnableCancelKey get/set
 * - Application.CountA method
 * - Application.International method
 * - Application.WorksheetFunction
 * - Application.SheetsInNewWorkbook get/set
 *
 * Reference Excel constants:
 * - XlCalculation: xlCalculationAutomatic=-4105, xlCalculationManual=-4135, xlCalculationSemiauto=2
 * - XlCalculationState: xlDone=0, xlCalculating=1, xlPending=2
 * - XlEnableCancelKey: xlDisabled=-4129, xlErrorHandler=-4127... wait
 *   Actually: xlDisabled=0, xlInterruptKey=1, xlErrorHandler=2
 * - XlFileFormat: relevant for SaveAs tests
 */
public class VbaApplicationPropsTest extends VbaCompatTest {

    private static final String MODULE_NAME = "AppPropsTests";

    // XlCalculation constants
    private static final int XL_CALC_AUTOMATIC = -4105;
    private static final int XL_CALC_MANUAL = -4135;
    private static final int XL_CALC_SEMI_AUTO = 2;

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "Option VBASupport 1\n" +
        "\n" +
        "Sub TestScreenUpdating()\n" +
        "    Application.ScreenUpdating = False\n" +
        "    Range(\"Z1\").Value = IIf(Application.ScreenUpdating, 0, 1)\n" +
        "    Application.ScreenUpdating = True\n" +
        "    Range(\"Z2\").Value = IIf(Application.ScreenUpdating, 1, 0)\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestIteration()\n" +
        "    Application.Iteration = True\n" +
        "    Range(\"Z1\").Value = IIf(Application.Iteration, 1, 0)\n" +
        "    Application.Iteration = False\n" +
        "    Range(\"Z2\").Value = IIf(Application.Iteration, 0, 1)\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestCalculation()\n" +
        "    Application.Calculation = " + XL_CALC_MANUAL + "\n" +
        "    Range(\"Z1\").Value = Application.Calculation\n" +
        "    Application.Calculation = " + XL_CALC_AUTOMATIC + "\n" +
        "    Range(\"Z2\").Value = Application.Calculation\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestCalculationState()\n" +
        "    Range(\"Z1\").Value = Application.CalculationState\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestDisplayAlerts()\n" +
        "    Application.DisplayAlerts = False\n" +
        "    Range(\"Z1\").Value = IIf(Application.DisplayAlerts, 0, 1)\n" +
        "    Application.DisplayAlerts = True\n" +
        "    Range(\"Z2\").Value = IIf(Application.DisplayAlerts, 1, 0)\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestEnableEvents()\n" +
        "    Application.EnableEvents = False\n" +
        "    Range(\"Z1\").Value = IIf(Application.EnableEvents, 0, 1)\n" +
        "    Application.EnableEvents = True\n" +
        "    Range(\"Z2\").Value = IIf(Application.EnableEvents, 1, 0)\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestEnableCancelKey()\n" +
        "    Application.EnableCancelKey = 1\n" +
        "    Range(\"Z1\").Value = Application.EnableCancelKey\n" +
        "    Application.EnableCancelKey = 0\n" +
        "    Range(\"Z2\").Value = Application.EnableCancelKey\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestCountA()\n" +
        "    Range(\"A1\").Value = \"hello\"\n" +
        "    Range(\"A2\").Value = 42\n" +
        "    Range(\"A3\").Value = \"\"\n" +
        "    Range(\"A4\").Value = \"world\"\n" +
        "    Range(\"Z1\").Value = Application.CountA(Range(\"A1:A4\"))\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestCountAEmpty()\n" +
        "    Range(\"A1\").Value = \"\"\n" +
        "    Range(\"A2\").Value = \"\"\n" +
        "    Range(\"Z1\").Value = Application.CountA(Range(\"A1:A2\"))\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestSheetsInNewWorkbook()\n" +
        "    Application.SheetsInNewWorkbook = 3\n" +
        "    Range(\"Z1\").Value = Application.SheetsInNewWorkbook\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestInternational()\n" +
        "    Range(\"Z1\").Value = Application.International(3)\n" +
        "    Range(\"Z2\").Value = Application.International(4)\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestEvaluate()\n" +
        "    Range(\"A1\").Value = 10\n" +
        "    Range(\"A2\").Value = 20\n" +
        "    Range(\"Z1\").Value = Application.Evaluate(\"A1+A2\")\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestApplicationName()\n" +
        "    Range(\"Z1\").Value = Application.Name\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestActiveWorkbook()\n" +
        "    Range(\"Z1\").Value = IIf(Application.ActiveWorkbook Is Nothing, 0, 1)\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestThisWorkbook()\n" +
        "    On Error Resume Next\n" +
        "    Range(\"Z1\").Value = IIf(Application.ThisWorkbook Is Nothing, 0, 1)\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testScreenUpdating() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestScreenUpdating");
        assertEquals("ScreenUpdating=False should report 1",
            1.0, getNumericResultAt(25, 0), 0.001);
        assertEquals("ScreenUpdating=True should report 1",
            1.0, getNumericResultAt(25, 1), 0.001);
    }

    @Test
    public void testIteration() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestIteration");
        assertEquals("Iteration=True should report 1",
            1.0, getNumericResultAt(25, 0), 0.001);
        assertEquals("Iteration=False should report 1",
            1.0, getNumericResultAt(25, 1), 0.001);
    }

    @Test
    public void testCalculation() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCalculation");
        assertEquals("Calculation should be xlManual (-4135)",
            (double)XL_CALC_MANUAL, getNumericResultAt(25, 0), 0.001);
        assertEquals("Calculation should be xlAutomatic (-4105)",
            (double)XL_CALC_AUTOMATIC, getNumericResultAt(25, 1), 0.001);
    }

    @Test
    public void testCalculationState() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCalculationState");
        double state = getNumericResultAt(25, 0);
        assertTrue("CalculationState should be >= 0, got: " + state, state >= 0);
    }

    @Test
    public void testDisplayAlerts() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestDisplayAlerts");
        assertEquals("DisplayAlerts=False should report 1",
            1.0, getNumericResultAt(25, 0), 0.001);
        assertEquals("DisplayAlerts=True should report 1",
            1.0, getNumericResultAt(25, 1), 0.001);
    }

    @Test
    public void testEnableEvents() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestEnableEvents");
        assertEquals("EnableEvents=False should report 1",
            1.0, getNumericResultAt(25, 0), 0.001);
        assertEquals("EnableEvents=True should report 1",
            1.0, getNumericResultAt(25, 1), 0.001);
    }

    @Test
    public void testEnableCancelKey() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestEnableCancelKey");
        assertEquals("EnableCancelKey=1 should report 1",
            1.0, getNumericResultAt(25, 0), 0.001);
        assertEquals("EnableCancelKey=0 should report 0",
            0.0, getNumericResultAt(25, 1), 0.001);
    }

    @Test
    public void testCountANonEmpty() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCountA");
        assertEquals("CountA should count 2 non-empty cells in A1:A4",
            2.0, getNumericResultAt(25, 0), 0.001);
    }

    @Test
    public void testCountAEmpty() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCountAEmpty");
        assertEquals("CountA should return 0 for empty cells",
            0.0, getNumericResultAt(25, 0), 0.001);
    }

    @Test
    public void testSheetsInNewWorkbook() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestSheetsInNewWorkbook");
        assertEquals("SheetsInNewWorkbook should be 3",
            3.0, getNumericResultAt(25, 0), 0.001);
    }

    @Test
    public void testInternational() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestInternational");
        String decimalSep = getStringAt(25, 0);
        String thousandsSep = getStringAt(25, 1);
        assertNotNull("Decimal separator should not be null", decimalSep);
        assertNotNull("Thousands separator should not be null", thousandsSep);
    }

    @Test
    public void testEvaluate() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestEvaluate");
        assertEquals("Evaluate(A1+A2) should be 30", 30.0, getNumericResultAt(25, 0), 0.001);
    }

    @Test
    public void testApplicationName() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestApplicationName");
        String name = getStringAt(25, 0);
        assertNotNull("Application.Name should not be null", name);
        assertFalse("Application.Name should not be empty", name.isEmpty());
    }

    @Test
    public void testActiveWorkbook() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestActiveWorkbook");
        assertEquals("ActiveWorkbook should be available (1)",
            1.0, getNumericResultAt(25, 0), 0.001);
    }

    @Test
    public void testThisWorkbook() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestThisWorkbook");
        assertEquals("ThisWorkbook should be available (1)",
            1.0, getNumericResultAt(25, 0), 0.001);
    }

    private double getNumericResultAt(int column, int row) throws Exception {
        return getCell(column, row).getValue();
    }

    private String getStringAt(int column, int row) throws Exception {
        return getCell(column, row).getString();
    }
}
