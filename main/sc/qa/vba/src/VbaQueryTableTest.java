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
 * P3: Tests for VBA Range.QueryTable API.
 *
 * Tests the following VBA object model features:
 * - Range.QueryTable returns a QueryTable object (not Nothing)
 * - QueryTable.Refresh method (returns Boolean indicating if link was updated)
 * - QueryTable.Delete method (removes from internal link manager)
 * - QueryTable.Refresh with background query parameter
 *
 * Note: The current AOO QueryTable implementation (vbaquerytable.cxx)
 * only implements Refresh and Delete. Properties like Connection,
 * CommandText, Destination are NOT implemented per the IDL.
 * Tests on a plain range (without external data links) verify that
 * these methods do not throw and return appropriate values.
 *
 * Test methodology: BASIC macros access Range.QueryTable and call
 * methods. Results written to Z column (index 25).
 *
 * Result columns:
 * - Z column (index 25): numeric results (1 = success/true, 0 = failure/false)
 * - AA column (index 26): string results
 */
public class VbaQueryTableTest extends VbaCompatTest {

    private static final String MODULE_NAME = "QueryTableTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +

        "'@TestMethod\n" +
        "Sub TestQueryTableNotNull()\n" +
        "    ' Arrange\n" +
        "    Dim qt As Object\n" +
        "\n" +
        "    ' Act\n" +
        "    Set qt = ActiveSheet.Range(\"A1:A10\").QueryTable\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = IIf(qt Is Nothing, 0, 1)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRefreshNoLink()\n" +
        "    ' Arrange\n" +
        "    Dim qt As Object\n" +
        "    Set qt = ActiveSheet.Range(\"A1:A10\").QueryTable\n" +
        "    Dim result As Boolean\n" +
        "\n" +
        "    ' Act - no area link exists, so Refresh should return False\n" +
        "    result = qt.Refresh(False)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = IIf(result, 1, 0)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestDeleteNoLink()\n" +
        "    ' Arrange\n" +
        "    Dim qt As Object\n" +
        "    Set qt = ActiveSheet.Range(\"A1:A10\").QueryTable\n" +
        "\n" +
        "    ' Act - Delete should not throw even if no link exists\n" +
        "    qt.Delete\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = 1\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRefreshBackgroundQuery()\n" +
        "    ' Arrange\n" +
        "    Dim qt As Object\n" +
        "    Set qt = ActiveSheet.Range(\"B1:B5\").QueryTable\n" +
        "\n" +
        "    ' Act\n" +
        "    Dim result As Boolean\n" +
        "    result = qt.Refresh(True)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = IIf(result, 1, 0)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestQueryTableOnDifferentRange()\n" +
        "    ' Arrange\n" +
        "    Dim qt As Object\n" +
        "\n" +
        "    ' Act\n" +
        "    Set qt = ActiveSheet.Range(\"C1:C10\").QueryTable\n" +
        "    qt.Delete\n" +
        "    Dim refreshResult As Boolean\n" +
        "    refreshResult = qt.Refresh(False)\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = IIf(qt Is Nothing, 0, 1)\n" +
        "    Range(\"Z2\").Value = IIf(refreshResult, 1, 0)\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testQueryTableNotNull() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestQueryTableNotNull");
        assertEquals("Range.QueryTable should return a non-null object",
            1.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testRefreshNoLink() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRefreshNoLink");
        // No area link exists on a fresh document, so Refresh should return False (0)
        assertEquals("Refresh should return False (0) when no link exists",
            0.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testDeleteNoLink() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestDeleteNoLink");
        // Delete should not throw and should set result to 1
        assertEquals("Delete should complete without error",
            1.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testRefreshBackgroundQuery() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRefreshBackgroundQuery");
        // With no link, Refresh with background should also return False
        assertEquals("Refresh(True) should return False (0) when no link exists",
            0.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testQueryTableOnDifferentRange() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestQueryTableOnDifferentRange");
        assertEquals("Range.QueryTable should return non-null on any range",
            1.0, getNumericResult(0), 0.001);
        assertEquals("Refresh should return False when no link",
            0.0, getNumericResult(1), 0.001);
    }
}
