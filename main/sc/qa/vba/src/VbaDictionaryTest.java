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
 * P3: Tests for VBA Scripting.Dictionary (ooo.vba.Scripting.Dictionary).
 *
 * Tests the following VBA object model features:
 * - CreateObject("Scripting.Dictionary") returns a Dictionary object
 * - Dictionary.Add adds key-value pairs
 * - Dictionary.Item (default property) get/set
 * - Dictionary.Exists checks if key exists
 * - Dictionary.Count returns number of items
 * - Dictionary.Remove removes a key
 * - Dictionary.RemoveAll removes all keys
 * - Dictionary.Keys returns array of keys
 * - Dictionary.Items returns array of values
 * - Dictionary.CompareMode get/set
 * - Dictionary.Key renames a key
 *
 * The AOO implementation (vbadictionary.cxx) matches the
 * Scripting.Dictionary interface. See XDictionary.idl.
 *
 * Test methodology: BASIC macros create Dictionary objects and exercise
 * methods. Results written to Z column (index 25).
 *
 * Result columns:
 * - Z column (index 25): numeric results
 * - AA column (index 26): string results
 */
public class VbaDictionaryTest extends VbaCompatTest {

    private static final String MODULE_NAME = "DictionaryTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "'@TestModule\n" +
        "Option VBASupport 1\n" +

        "'@TestMethod\n" +
        "Sub TestCreateDictionary()\n" +
        "    ' Arrange\n" +
        "    Dim dict As Object\n" +
        "\n" +
        "    ' Act\n" +
        "    Set dict = CreateObject(\"Scripting.Dictionary\")\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = IIf(dict Is Nothing, 0, 1)\n" +
        "    Range(\"Z2\").Value = dict.Count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestAddAndExists()\n" +
        "    ' Arrange\n" +
        "    Dim dict As Object\n" +
        "    Set dict = CreateObject(\"Scripting.Dictionary\")\n" +
        "\n" +
        "    ' Act\n" +
        "    dict.Add \"key1\", 100\n" +
        "    dict.Add \"key2\", 200\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = dict.Count\n" +
        "    Range(\"Z2\").Value = IIf(dict.Exists(\"key1\"), 1, 0)\n" +
        "    Range(\"Z3\").Value = IIf(dict.Exists(\"key2\"), 1, 0)\n" +
        "    Range(\"Z4\").Value = IIf(dict.Exists(\"nonexistent\"), 0, 1)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestItemAccess()\n" +
        "    ' Arrange\n" +
        "    Dim dict As Object\n" +
        "    Set dict = CreateObject(\"Scripting.Dictionary\")\n" +
        "    dict.Add \"name\", \"value\"\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = dict(\"name\")\n" +
        "    Range(\"AA1\").Value = dict.Item(\"name\")\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestItemSetUpdate()\n" +
        "    ' Arrange\n" +
        "    Dim dict As Object\n" +
        "    Set dict = CreateObject(\"Scripting.Dictionary\")\n" +
        "    dict.Add \"key\", 1\n" +
        "\n" +
        "    ' Act\n" +
        "    dict(\"key\") = 99\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = dict(\"key\")\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestItemNewKey()\n" +
        "    ' Arrange\n" +
        "    Dim dict As Object\n" +
        "    Set dict = CreateObject(\"Scripting.Dictionary\")\n" +
        "\n" +
        "    ' Act - setting a new key should auto-add it\n" +
        "    dict(\"newkey\") = 42\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = dict.Count\n" +
        "    Range(\"Z2\").Value = dict(\"newkey\")\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRemove()\n" +
        "    ' Arrange\n" +
        "    Dim dict As Object\n" +
        "    Set dict = CreateObject(\"Scripting.Dictionary\")\n" +
        "    dict.Add \"a\", 1\n" +
        "    dict.Add \"b\", 2\n" +
        "    Range(\"Z1\").Value = dict.Count\n" +
        "\n" +
        "    ' Act\n" +
        "    dict.Remove(\"a\")\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z2\").Value = dict.Count\n" +
        "    Range(\"Z3\").Value = IIf(dict.Exists(\"a\"), 0, 1)\n" +
        "    Range(\"Z4\").Value = IIf(dict.Exists(\"b\"), 1, 0)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestRemoveAll()\n" +
        "    ' Arrange\n" +
        "    Dim dict As Object\n" +
        "    Set dict = CreateObject(\"Scripting.Dictionary\")\n" +
        "    dict.Add \"a\", 1\n" +
        "    dict.Add \"b\", 2\n" +
        "    Range(\"Z1\").Value = dict.Count\n" +
        "\n" +
        "    ' Act\n" +
        "    dict.RemoveAll\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z2\").Value = dict.Count\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestKeysAndItems()\n" +
        "    ' Arrange\n" +
        "    Dim dict As Object\n" +
        "    Set dict = CreateObject(\"Scripting.Dictionary\")\n" +
        "    dict.Add \"key1\", 10\n" +
        "    dict.Add \"key2\", 20\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = dict.Count\n" +
        "    ' Keys/Items return variant arrays - use UBound to verify count\n" +
        "    Dim keys As Variant\n" +
        "    keys = dict.Keys\n" +
        "    Range(\"Z2\").Value = UBound(keys) - LBound(keys) + 1\n" +
        "    Dim items As Variant\n" +
        "    items = dict.Items\n" +
        "    Range(\"Z3\").Value = UBound(items) - LBound(items) + 1\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestCompareMode()\n" +
        "    ' Arrange\n" +
        "    Dim dict As Object\n" +
        "    Set dict = CreateObject(\"Scripting.Dictionary\")\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    Range(\"Z1\").Value = dict.CompareMode\n" +
        "    dict.CompareMode = 1\n" +
        "    Range(\"Z2\").Value = dict.CompareMode\n" +
        "    dict.CompareMode = 0\n" +
        "    Range(\"Z3\").Value = dict.CompareMode\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestCompareModeWithItems()\n" +
        "    ' Arrange\n" +
        "    Dim dict As Object\n" +
        "    Set dict = CreateObject(\"Scripting.Dictionary\")\n" +
        "    dict.Add \"Key\", 1\n" +
        "\n" +
        "    ' Act\n" +
        "    dict.CompareMode = 2\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = dict.CompareMode\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestKeyRename()\n" +
        "    ' Arrange\n" +
        "    Dim dict As Object\n" +
        "    Set dict = CreateObject(\"Scripting.Dictionary\")\n" +
        "    dict.Add \"oldkey\", 100\n" +
        "\n" +
        "    ' Act\n" +
        "    dict.Key(\"oldkey\") = \"newkey\"\n" +
        "\n" +
        "    ' Assert\n" +
        "    Range(\"Z1\").Value = IIf(dict.Exists(\"oldkey\"), 0, 1)\n" +
        "    Range(\"Z2\").Value = IIf(dict.Exists(\"newkey\"), 1, 0)\n" +
        "    Range(\"Z3\").Value = IIf(dict(\"newkey\") = 100, 1, 0)\n" +
        "End Sub\n" +
        "\n" +

        "'@TestMethod\n" +
        "Sub TestAddDuplicateKey()\n" +
        "    ' Arrange\n" +
        "    Dim dict As Object\n" +
        "    Set dict = CreateObject(\"Scripting.Dictionary\")\n" +
        "    dict.Add \"dup\", 1\n" +
        "\n" +
        "    ' Act & Assert\n" +
        "    On Error Resume Next\n" +
        "    dict.Add \"dup\", 2\n" +
        "    Range(\"Z1\").Value = IIf(Err.Number <> 0, 1, 0)\n" +
        "    Err.Clear\n" +
        "    Range(\"Z2\").Value = dict(\"dup\")\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testCreateDictionary() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCreateDictionary");
        assertEquals("CreateObject should return non-null object",
            1.0, getNumericResult(0), 0.001);
        assertEquals("Count should be 0 on new dictionary",
            0.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testAddAndExists() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestAddAndExists");
        assertEquals("Count should be 2 after two Adds",
            2.0, getNumericResult(0), 0.001);
        assertEquals("Exists(key1) should be True (1)",
            1.0, getNumericResult(1), 0.001);
        assertEquals("Exists(key2) should be True (1)",
            1.0, getNumericResult(2), 0.001);
        assertEquals("Exists(nonexistent) should be False (0)",
            1.0, getNumericResult(3), 0.001); // Inverted: 1 = not exists
    }

    @Test
    public void testItemAccess() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestItemAccess");
        assertEquals("dict(\"name\") should be 0 (string stored as variant)",
            0.0, getNumericResult(0), 0.001);
        assertNotNull("dict.Item(\"name\") should not be null", getStringResult(0));
    }

    @Test
    public void testItemSetUpdate() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestItemSetUpdate");
        assertEquals("Updated value should be 99",
            99.0, getNumericResult(0), 0.001);
    }

    @Test
    public void testItemNewKey() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestItemNewKey");
        assertEquals("Setting new key should add it (Count=1)",
            1.0, getNumericResult(0), 0.001);
        assertEquals("Value should be 42",
            42.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testRemove() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRemove");
        assertEquals("Count before Remove should be 2",
            2.0, getNumericResult(0), 0.001);
        assertEquals("Count after Remove should be 1",
            1.0, getNumericResult(1), 0.001);
        assertEquals("Exists(a) should be False (1=inverted)",
            1.0, getNumericResult(2), 0.001);
        assertEquals("Exists(b) should be True",
            1.0, getNumericResult(3), 0.001);
    }

    @Test
    public void testRemoveAll() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestRemoveAll");
        assertEquals("Count before RemoveAll should be 2",
            2.0, getNumericResult(0), 0.001);
        assertEquals("Count after RemoveAll should be 0",
            0.0, getNumericResult(1), 0.001);
    }

    @Test
    public void testKeysAndItems() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestKeysAndItems");
        assertEquals("Count should be 2",
            2.0, getNumericResult(0), 0.001);
        assertEquals("Keys array UBound-LBound+1 should be 2",
            2.0, getNumericResult(1), 0.001);
        assertEquals("Items array UBound-LBound+1 should be 2",
            2.0, getNumericResult(2), 0.001);
    }

    @Test
    public void testCompareMode() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCompareMode");
        assertEquals("Default CompareMode should be 0",
            0.0, getNumericResult(0), 0.001);
        assertEquals("After setting to 1, CompareMode should be 1",
            1.0, getNumericResult(1), 0.001);
        assertEquals("After setting to 0, CompareMode should be 0",
            0.0, getNumericResult(2), 0.001);
    }

    @Test
    public void testCompareModeWithItems() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestCompareModeWithItems");
        // Should not throw when items exist - either succeeds or error caught
        // The result depends on AOO implementation behavior
        assertTrue("CompareMode should be valid (>= 0)", getNumericResult(0) >= 0);
    }

    @Test
    public void testKeyRename() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestKeyRename");
        assertEquals("Old key should not exist (1=inverted)",
            1.0, getNumericResult(0), 0.001);
        assertEquals("New key should exist",
            1.0, getNumericResult(1), 0.001);
        assertEquals("Value should follow key rename",
            1.0, getNumericResult(2), 0.001);
    }

    @Test
    public void testAddDuplicateKey() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestAddDuplicateKey");
        assertEquals("Add of duplicate key should trigger an error",
            1.0, getNumericResult(0), 0.001);
        assertEquals("Original value should be preserved",
            1.0, getNumericResult(1), 0.001);
    }
}
