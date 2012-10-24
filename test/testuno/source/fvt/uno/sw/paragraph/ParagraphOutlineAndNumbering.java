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

package fvt.uno.sw.paragraph;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;
import com.sun.star.text.*;
import com.sun.star.beans.*;
import com.sun.star.frame.XStorable;
import com.sun.star.uno.UnoRuntime;

public class ParagraphOutlineAndNumbering {
	private static final UnoApp app = new UnoApp();
	XText xText = null;

	@Before
	public void setUp() throws Exception {
		app.start();

	}

	@After
	public void tearDown() throws Exception {
		//app.close();
	}
	/*
	 * test paragraph background color
	 * 1.new a text document
	 * 2.insert some text
	 * 3.set paragraph outline and numbering
	 * 4.save and close the document
	 * 5.reload the saved document and check the paragraph outline and numbering
	 */	
	@Test@Ignore("Bug #120753 - [testUNO patch]paragraph outline level and numbering style change when save to doc")
	public void testOutlineAndNumbering_NumberStyle() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.\nwe are all living in one earth!Hello,world!Hello,world!\nHello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		XTextCursor xTextCursor=xText.createTextCursor();
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		xTextProps.setPropertyValue("OutlineLevel", (short)6);
		xTextProps.setPropertyValue("NumberingStyleName", "Numbering 2");
		xTextProps.setPropertyValue("ParaIsNumberingRestart", true);
		xTextProps.setPropertyValue("NumberingStartValue", (short)7);
		xTextProps.setPropertyValue("ParaLineNumberCount",true);
		xTextProps.setPropertyValue("ParaLineNumberStartValue",(long)4);
		//save to odt 
		XStorable xStorable_odt = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_odt = new PropertyValue[2];
		aStoreProperties_odt[0] = new PropertyValue();
		aStoreProperties_odt[1] = new PropertyValue();
		aStoreProperties_odt[0].Name = "Override";
		aStoreProperties_odt[0].Value = true;
		aStoreProperties_odt[1].Name = "FilterName";
		aStoreProperties_odt[1].Value = "writer8";
		xStorable_odt.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.odt")), aStoreProperties_odt);
		//save to doc 
		XStorable xStorable_doc = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_doc = new PropertyValue[2];
		aStoreProperties_doc[0] = new PropertyValue();
		aStoreProperties_doc[1] = new PropertyValue();
		aStoreProperties_doc[0].Name = "Override";
		aStoreProperties_doc[0].Value = true;
		aStoreProperties_doc[1].Name = "FilterName";
		aStoreProperties_doc[1].Value = "MS Word 97";
		xStorable_doc.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.doc")), aStoreProperties_doc);	
		//app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph outline and numbering
		assertEquals("assert paragraph outline and numbering",(short)6,xCursorProps_Assert_odt.getPropertyValue("OutlineLevel"));
		assertEquals("assert paragraph outline and numbering","Numbering 2",xCursorProps_Assert_odt.getPropertyValue("NumberingStyleName"));
		assertEquals("assert paragraph outline and numbering",true,xCursorProps_Assert_odt.getPropertyValue("ParaIsNumberingRestart"));
		assertEquals("assert paragraph outline and numbering",(short)7,xCursorProps_Assert_odt.getPropertyValue("NumberingStartValue"));
		assertEquals("assert paragraph outline and numbering",true,xCursorProps_Assert_odt.getPropertyValue("ParaLineNumberCount"));
		assertEquals("assert paragraph outline and numbering",(long)4,xCursorProps_Assert_odt.getPropertyValue("ParaLineNumberStartValue"));
		
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph outline and numbering
		assertEquals("assert paragraph outline and numbering",(short)6,xCursorProps_Assert_doc.getPropertyValue("OutlineLevel"));
		assertEquals("assert paragraph outline and numbering","Numbering 2",xCursorProps_Assert_doc.getPropertyValue("NumberingStyleName"));
		assertEquals("assert paragraph outline and numbering",true,xCursorProps_Assert_doc.getPropertyValue("ParaIsNumberingRestart"));
		assertEquals("assert paragraph outline and numbering",(short)7,xCursorProps_Assert_doc.getPropertyValue("NumberingStartValue"));
		assertEquals("assert paragraph outline and numbering",true,xCursorProps_Assert_doc.getPropertyValue("ParaLineNumberCount"));
		assertEquals("assert paragraph outline and numbering",(long)4,xCursorProps_Assert_doc.getPropertyValue("ParaLineNumberStartValue"));
		
	}
	@Test@Ignore("Bug #120753 - [testUNO patch]paragraph outline level and numbering style change when save to doc")
	public void testOutlineAndNumbering_ListStyle() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.\nwe are all living in one earth!Hello,world!Hello,world!\nHello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		XTextCursor xTextCursor=xText.createTextCursor();
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		xTextProps.setPropertyValue("OutlineLevel", (short)3);
		xTextProps.setPropertyValue("NumberingStyleName", "List 3");
		xTextProps.setPropertyValue("ParaIsNumberingRestart", true);
		xTextProps.setPropertyValue("NumberingStartValue", (short)6);	
		xTextProps.setPropertyValue("ParaLineNumberCount",true);
		xTextProps.setPropertyValue("ParaLineNumberStartValue",(long)4);
		//save to odt 
		XStorable xStorable_odt = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_odt = new PropertyValue[2];
		aStoreProperties_odt[0] = new PropertyValue();
		aStoreProperties_odt[1] = new PropertyValue();
		aStoreProperties_odt[0].Name = "Override";
		aStoreProperties_odt[0].Value = true;
		aStoreProperties_odt[1].Name = "FilterName";
		aStoreProperties_odt[1].Value = "StarOffice XML (Writer)";
		xStorable_odt.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.odt")), aStoreProperties_odt);
		//save to doc 
		XStorable xStorable_doc = (XStorable) UnoRuntime.queryInterface(XStorable.class, xTextDocument);
		PropertyValue[] aStoreProperties_doc = new PropertyValue[2];
		aStoreProperties_doc[0] = new PropertyValue();
		aStoreProperties_doc[1] = new PropertyValue();
		aStoreProperties_doc[0].Name = "Override";
		aStoreProperties_doc[0].Value = true;
		aStoreProperties_doc[1].Name = "FilterName";
		aStoreProperties_doc[1].Value = "MS Word 97";
		xStorable_doc.storeToURL(FileUtil.getUrl(Testspace.getPath("output/test.doc")), aStoreProperties_doc);	
		//app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph outline and numbering
		assertEquals("assert paragraph outline and numbering",(short)3,xCursorProps_Assert_odt.getPropertyValue("OutlineLevel"));
		assertEquals("assert paragraph outline and numbering","List 3",xCursorProps_Assert_odt.getPropertyValue("NumberingStyleName"));
		assertEquals("assert paragraph outline and numbering",true,xCursorProps_Assert_odt.getPropertyValue("ParaIsNumberingRestart"));
		assertEquals("assert paragraph outline and numbering",(short)6,xCursorProps_Assert_odt.getPropertyValue("NumberingStartValue"));
		assertEquals("assert paragraph outline and numbering",true,xCursorProps_Assert_odt.getPropertyValue("ParaLineNumberCount"));
		assertEquals("assert paragraph outline and numbering",(long)4,xCursorProps_Assert_odt.getPropertyValue("ParaLineNumberStartValue"));
		
		
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph outline and numbering
		assertEquals("assert paragraph outline and numbering",(short)3,xCursorProps_Assert_doc.getPropertyValue("OutlineLevel"));
		assertEquals("assert paragraph outline and numbering","List 3",xCursorProps_Assert_doc.getPropertyValue("NumberingStyleName"));
		assertEquals("assert paragraph outline and numbering",true,xCursorProps_Assert_doc.getPropertyValue("ParaIsNumberingRestart"));
		assertEquals("assert paragraph outline and numbering",(short)6,xCursorProps_Assert_doc.getPropertyValue("NumberingStartValue"));  
		assertEquals("assert paragraph outline and numbering",true,xCursorProps_Assert_doc.getPropertyValue("ParaLineNumberCount"));
		assertEquals("assert paragraph outline and numbering",(long)4,xCursorProps_Assert_doc.getPropertyValue("ParaLineNumberStartValue"));
		
	}
}
