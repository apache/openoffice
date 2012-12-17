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
package fvt.uno.sw.frame;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SWUtil;

import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XNameAccess;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextFrame;
import com.sun.star.text.XTextFramesSupplier;
import com.sun.star.uno.UnoRuntime;

public class FrameBackColor {
	private static final UnoApp app = new UnoApp();
	private XTextDocument xTextDocument=null;
	private XMultiServiceFactory xWriterFactory=null;
	private XText xText=null;
	@Before
	public void setUp() throws Exception {
		app.start();		
	}

	@After
	public void tearDown() throws Exception {
		app.close();
	}

	@Test
	public void testFrameBackColor() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextFrame xTextFrame = (XTextFrame)UnoRuntime.queryInterface(XTextFrame.class, xWriterFactory.createInstance("com.sun.star.text.TextFrame"));
		xText.insertTextContent(xTextCursor,xTextFrame,false);
		XPropertySet xFrameProps = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextFrame);
		xFrameProps.setPropertyValue("BackColor",0x0000FF00);
		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class,SWUtil.saveTo_Override_reload(xTextDocument, "writer8",Testspace.getPath("output/test.odt"), app));
		XTextFramesSupplier xTFS_odt = (XTextFramesSupplier) UnoRuntime.queryInterface(XTextFramesSupplier.class, assertDocument_odt);
		XNameAccess xTextFrames_odt = xTFS_odt.getTextFrames();
		Object xTextFrame_obj_odt=xTextFrames_odt.getByName("Frame1");
		XTextFrame xTextFrame_Assert_odt=(XTextFrame) UnoRuntime.queryInterface(XTextFrame.class, xTextFrame_obj_odt);
		XPropertySet xFrameProps_assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextFrame_Assert_odt);
		assertEquals("verify Frame background color",0x0000FF00,xFrameProps_assert_odt.getPropertyValue("BackColor"));

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class,SWUtil.saveTo_Override_reload(xTextDocument, "MS Word 97",Testspace.getPath("output/test.doc"), app));
		XTextFramesSupplier xTFS_doc = (XTextFramesSupplier) UnoRuntime.queryInterface(XTextFramesSupplier.class, assertDocument_doc);
		XNameAccess xTextFrames_doc = xTFS_doc.getTextFrames();
		Object xTextFrame_obj_doc=xTextFrames_doc.getByName("Frame1");
		XTextFrame xTextFrame_Assert_doc=(XTextFrame) UnoRuntime.queryInterface(XTextFrame.class, xTextFrame_obj_doc);
		XPropertySet xFrameProps_assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextFrame_Assert_doc);
		assertEquals("verify frame background color",0x0000FF00,xFrameProps_assert_doc.getPropertyValue("BackColor"));
	}
}
