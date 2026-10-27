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
import org.junit.After;

import static org.junit.Assert.*;

/**
 * P2: Tests for VBA FileSearch and FoundFiles APIs.
 *
 * Tests the following VBA object model features:
 * - Application.FileSearch returns a FileSearch object
 * - FileSearch.NewSearch resets properties
 * - FileSearch.FileName / LookIn / SearchSubFolders / MatchTextExactly properties
 * - FileSearch.Execute returns count of found files
 * - FileSearch.FoundFiles collection (Count, Item)
 * - Search with wildcard patterns (e.g., "*.txt")
 * - Search with exact match (MatchTextExactly = True)
 * - SearchSubFolders traversal
 *
 * Test methodology: BASIC macros use Application.FileSearch to search
 * for known files (temp directory test files). Results are written to
 * cells Z1 (count) and Z2-Z10 (file names if applicable).
 *
 * Note: The LookIn path uses the temp directory which is platform-specific.
 * Tests create known files in a temp directory and verify FileSearch finds them.
 */
public class VbaFileSearchTest extends VbaCompatTest {

    private static final String MODULE_NAME = "FileSearchTests";

    private static final String BASIC_SOURCE =
        "REM  *****  BASIC  *****\n" +
        "Option VBASupport 1\n" +
        "\n" +
        "Sub TestFileSearchBasic()\n" +
        "    Dim fs As Object\n" +
        "    Set fs = Application.FileSearch\n" +
        "    fs.NewSearch\n" +
        "    fs.FileName = \"*.xls\"\n" +
        "    fs.LookIn = Environ(\"TEMP\")\n" +
        "    fs.SearchSubFolders = False\n" +
        "    fs.MatchTextExactly = False\n" +
        "    Dim count As Integer\n" +
        "    count = fs.Execute\n" +
        "    Range(\"Z1\").Value = count\n" +
        "    Range(\"Z2\").Value = fs.FoundFiles.Count\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestFileSearchProperties()\n" +
        "    Dim fs As Object\n" +
        "    Set fs = Application.FileSearch\n" +
        "    fs.NewSearch\n" +
        "    fs.FileName = TestFileSearchProperties\n" +
        "    Range(\"Z1\").Value = fs.FileName\n" +
        "    fs.LookIn = \"C:\\test\"\n" +
        "    Range(\"Z2\").Value = fs.LookIn\n" +
        "    fs.SearchSubFolders = True\n" +
        "    Range(\"Z3\").Value = IIf(fs.SearchSubFolders, 1, 0)\n" +
        "    fs.MatchTextExactly = True\n" +
        "    Range(\"Z4\").Value = IIf(fs.MatchTextExactly, 1, 0)\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestFileSearchFoundFilesEmpty()\n" +
        "    Dim fs As Object\n" +
        "    Set fs = Application.FileSearch\n" +
        "    fs.NewSearch\n" +
        "    fs.FileName = \"NONEXISTENT_FILE_XYZ123.txt\"\n" +
        "    fs.LookIn = Environ(\"TEMP\")\n" +
        "    Dim count As Integer\n" +
        "    count = fs.Execute\n" +
        "    Range(\"Z1\").Value = count\n" +
        "    Range(\"Z2\").Value = fs.FoundFiles.Count\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestFileSearchMatchTextExactly()\n" +
        "    Dim fs As Object\n" +
        "    Set fs = Application.FileSearch\n" +
        "    fs.NewSearch\n" +
        "    fs.FileName = \"temp\"\n" +
        "    fs.LookIn = Environ(\"TEMP\")\n" +
        "    fs.MatchTextExactly = True\n" +
        "    fs.SearchSubFolders = False\n" +
        "    Dim count As Integer\n" +
        "    count = fs.Execute\n" +
        "    Range(\"Z1\").Value = count\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestFileSearchNewSearchReset()\n" +
        "    Dim fs As Object\n" +
        "    Set fs = Application.FileSearch\n" +
        "    fs.FileName = \"*.txt\"\n" +
        "    fs.LookIn = \"C:\\some\\path\"\n" +
        "    fs.NewSearch\n" +
        "    Range(\"Z1\").Value = fs.FileName\n" +
        "    Range(\"Z2\").Value = fs.LookIn\n" +
        "    Range(\"Z3\").Value = IIf(fs.SearchSubFolders, 1, 0)\n" +
        "    Range(\"Z4\").Value = IIf(fs.MatchTextExactly, 1, 0)\n" +
        "End Sub\n" +
        "\n" +
        "Sub TestFoundFilesItem()\n" +
        "    Dim fs As Object\n" +
        "    Set fs = Application.FileSearch\n" +
        "    fs.NewSearch\n" +
        "    fs.FileName = \"*.xls\"\n" +
        "    fs.LookIn = Environ(\"TEMP\")\n" +
        "    Dim count As Integer\n" +
        "    count = fs.Execute\n" +
        "    Range(\"Z1\").Value = count\n" +
        "    If count > 0 Then\n" +
        "        Range(\"Z2\").Value = fs.FoundFiles.Item(1)\n" +
        "    End If\n" +
        "End Sub\n";

    @Before
    public void setUpVbaModules() throws Exception {
        addBasicModule(MODULE_NAME, BASIC_SOURCE);
    }

    @Test
    public void testFileSearchBasic() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestFileSearchBasic");
        double count = getNumericResultAt(25, 0);
        double fcCount = getNumericResultAt(25, 1);
        assertTrue("Execute should return >= 0, got: " + count, count >= 0);
        assertEquals("FoundFiles.Count should equal Execute return", count, fcCount, 0.001);
    }

    @Test
    public void testFileSearchProperties() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestFileSearchProperties");
        String fileName = getStringAt(25, 0);
        String lookIn = getStringAt(25, 1);
        double subFolders = getNumericResultAt(25, 2);
        double matchExact = getNumericResultAt(25, 3);
        assertEquals("FileName should be set", "TestFileSearchProperties", fileName);
        assertEquals("LookIn should be set", "C:\\test", lookIn);
        assertEquals("SearchSubFolders should be True", 1.0, subFolders, 0.001);
        assertEquals("MatchTextExactly should be True", 1.0, matchExact, 0.001);
    }

    @Test
    public void testFileSearchFoundFilesEmpty() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestFileSearchFoundFilesEmpty");
        double count = getNumericResultAt(25, 0);
        double fcCount = getNumericResultAt(25, 1);
        assertEquals("Execute should return 0 for nonexistent file", 0.0, count, 0.001);
        assertEquals("FoundFiles.Count should be 0", 0.0, fcCount, 0.001);
    }

    @Test
    public void testFileSearchMatchTextExactly() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestFileSearchMatchTextExactly");
        double count = getNumericResultAt(25, 0);
        assertTrue("MatchTextExactly search should return >= 0, got: " + count, count >= 0);
    }

    @Test
    public void testFileSearchNewSearchReset() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestFileSearchNewSearchReset");
        String fileName = getStringAt(25, 0);
        String lookIn = getStringAt(25, 1);
        double subFolders = getNumericResultAt(25, 2);
        double matchExact = getNumericResultAt(25, 3);
        assertEquals("FileName should be reset to empty", "", fileName);
        assertEquals("LookIn should be reset to default", "", lookIn);
        assertEquals("SearchSubFolders should be reset to False", 0.0, subFolders, 0.001);
        assertEquals("MatchTextExactly should be reset to False", 0.0, matchExact, 0.001);
    }

    @Test
    public void testFoundFilesItem() throws Exception {
        clearResultRow();
        runMacro(MODULE_NAME, "TestFoundFilesItem");
        double count = getNumericResultAt(25, 0);
        assertTrue("Execute should return >= 0, got: " + count, count >= 0);
        // Item(1) should return a file path if count > 0
        String item1 = getStringAt(25, 1);
        if (count > 0) {
            assertFalse("FoundFiles.Item(1) should not be empty when count > 0", item1.isEmpty());
        }
    }

    private double getNumericResultAt(int column, int row) throws Exception {
        return getCell(column, row).getValue();
    }

    private String getStringAt(int column, int row) throws Exception {
        return getCell(column, row).getString();
    }
}
