/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * <p>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p>
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package fvt.gui.sw.hyperlink;

import static org.junit.Assert.*;
import static org.openoffice.test.common.Testspace.*;
import static org.openoffice.test.vcl.Tester.*;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import java.awt.Rectangle;
import java.io.File;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;

import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import java.util.Arrays;
import java.util.Collection;

import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.GraphicsUtil;
import org.openoffice.test.common.Logger;

import testlib.gui.SCTool;

/**
 * Class to test that clicking certain hyperlinks in a document display
 * a warning dialog.
 */
@RunWith(Parameterized.class)
public class WarningDialog {

    private String link;
    private String type;

    @Parameters
    public static Collection<Object[]> data() {
        return Arrays.asList(new Object[][]{
                // links with extensions
                {"nfs://nonexistant.url.com/evil.jar", "nfs with .jar"},
                {"dav://nonexistant.url.com/evil.jar", "dav with .jar"},
                {"smb://nonexistant.url.com/evil.jar", "smb with .jar"},
                // with path and no extension
                {"nfs://nonexistant.url.com/evil", "nfs with path"},
                {"dav://nonexistant.url.com/evil", "dav with path"},
                {"smb://nonexistant.url.com/evil", "smb with path"},
                // host only
                {"nfs://nonexistant.url.com", "nfs host only"},
                {"dav://nonexistant.url.com", "dav host only"},
                {"smb://nonexistant.url.com", "smb host only"}
        });
    }

    @Rule
    public Logger log = Logger.getLogger(this);

    @BeforeClass
    public static void beforeClass() {
        app.clean();
    }

    @AfterClass
    public static void afterClass() {
        app.stop();
    }

    @Before
    public void before() {
        app.stop();
        app.start();
    }

    public WarningDialog(String link, String type) {
        this.link = link;
        this.type = type;
    }

    /**
     * Test open a hyperlink in a text document.
     * 1. New a text document
     * 2. Insert a hyperlink
     * 3. Open hyperlink
     * 4. Verify security warning dialog is displayed
     *
     * @throws Exception
     */
    @Test
    public void testHyperlinkDisplaysWarning() throws Exception {
        // Create a new text document
        newTextDocument();
        writer.waitForExistence(10, 2);
        // open the hyperlink dialog
        writer.typeKeys("<alt i>"); // insert menu
        writer.typeKeys("h"); // hyperlink
        hyperlinkInetPathComboBox.setText(link); //target
        hyperlinkInetText.setText(link); // displayed text
        hyperlinkDialogOkBtn.click(); // apply
        hyperlinkDialogCancelBtn.click(); // close
        sleep(1);
        typeKeys("<shift F10>"); // context menu
        typeKeys("o"); // open hyperlink
        // we can't be sure of the language so just check for the dialog
        boolean msgExists = activeMsgBox.exists(1); // wait 1 second for the dialog
        if (msgExists) {
            activeMsgBox.no(); // close dialog
        }
        assertTrue("warning not displayed for " + type, msgExists);
        discard();
    }

}