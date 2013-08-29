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

package fvt.mix;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import org.openoffice.test.OpenOffice;
import org.openoffice.test.uno.UnoApp;
import org.openoffice.test.vcl.widgets.VclApp;
import org.openoffice.test.vcl.widgets.VclListBox;
import org.openoffice.test.vcl.widgets.VclTabPage;
import org.openoffice.test.vcl.widgets.VclWindow;

import com.sun.star.beans.XPropertySet;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;

/**
 * Demo for testing with both UNO and VCLAuto 
 * @author test
 *
 */
public class MixedTest {
	OpenOffice aoo;
	UnoApp unoApp;
	VclApp vclApp;
	VclWindow writer;
	VclTabPage effectsPage;
	VclListBox colorList;
	XTextDocument textDocument;
	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		OpenOffice aoo = OpenOffice.getDefault();
		unoApp = new UnoApp(aoo);
		vclApp = new VclApp(aoo);
		writer = new VclWindow(vclApp, "SW_HID_EDIT_WIN");
		effectsPage = new VclTabPage(vclApp, "CUI_HID_SVXPAGE_CHAR_EFFECTS");
		colorList = new VclListBox(vclApp, "cui:ListBox:RID_SVXPAGE_CHAR_EFFECTS:LB_FONTCOLOR");
		unoApp.start();
	}

	@After
	public void tearDown() throws Exception {
		unoApp.closeDocument(textDocument);
		unoApp.close();
	}

	@Test
	public void testUseBothUNOAndGuiAPI()  throws Exception  {
		//Use UNO API to create a new document
		textDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, unoApp.newDocument("swriter"));
		XText xText = textDocument.getText();
		xText.setString("UNO: Hello World!");
		//Input something by typing keyboard
		writer.typeKeys("GUI: Hello world!");
		//Set text color to green via GUI
		writer.drag(10, 10, 300, 400);
		writer.menuItem("Format->Character...").select();
		effectsPage.select();
		colorList.select("Light green");
		effectsPage.ok();
		//Verify the result via UNO API
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		Assert.assertEquals("Text Color", 0x0000FF00, xps.getPropertyValue("CharColor"));
	}
}
