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
import org.junit.Test;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import com.sun.star.style.NumberingType;
import com.sun.star.text.*;
import com.sun.star.beans.*;
import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XIndexReplace;
import com.sun.star.frame.XStorable;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;

public class ParagraphNumberingAndBulletAlignment {
	private static final UnoApp app = new UnoApp();
	XText xText = null;

	@Before
	public void setUp() throws Exception {
		app.start();

	}

	@After
	public void tearDown() throws Exception {
		app.close();
	}
	/*
	 * test paragraph background color
	 * 1.new a text document
	 * 2.insert some text
	 * 3.set paragraph numbering alignment
	 * 4.save and close the document
	 * 5.reload the saved document and check the paragraph numbering alignment
	 */	
	@Test
	public void testNumberingBulletAlign_Right() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.\nwe are all living in one earth!\nHello,world!Hello,world!Hello,world!\nHello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		//create cursor to select paragraph and formatting paragraph
		XTextCursor xTextCursor = xText.createTextCursor();    
		//create paragraph property set
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//create document service factory
		XMultiServiceFactory  xWriterFactory= (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);		
		//set numbering character
		XIndexAccess xNumRule = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,xWriterFactory.createInstance("com.sun.star.text.NumberingRules"));
		PropertyValue[] propsRule = {new PropertyValue(),new PropertyValue()};
		propsRule[1].Name = "NumberingType";
		propsRule[1].Value = NumberingType.ARABIC;
		propsRule[0].Name = "Adjust";
		propsRule[0].Value = HoriOrientation.RIGHT;	
		XIndexReplace xReplaceRule = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule);
		xReplaceRule.replaceByIndex(0, propsRule);  
		//set paragraph numbering and bullet character
		xTextProps.setPropertyValue("NumberingRules", xNumRule);
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
		app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		XIndexAccess xNumRule_assert_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_odt.getPropertyValue("NumberingRules"));
		XIndexReplace xReplaceRule_assert_odt = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule_assert_odt);
		PropertyValue[] propsRule_assert_odt=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xReplaceRule_assert_odt.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","Adjust",propsRule_assert_odt[0].Name);
		assertEquals("assert numbering and bullet",HoriOrientation.RIGHT,propsRule_assert_odt[0].Value);
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_odt[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ARABIC,propsRule_assert_odt[11].Value);

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		XIndexAccess xNumRule_assert_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_doc.getPropertyValue("NumberingRules"));
		PropertyValue[] propsRule_assert_doc=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xNumRule_assert_doc.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","Adjust",propsRule_assert_doc[0].Name);
		assertEquals("assert numbering and bullet",HoriOrientation.RIGHT,propsRule_assert_doc[0].Value);
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_doc[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ARABIC,propsRule_assert_doc[11].Value);
	}
	@Test
	public void testNumberingBulletAlign_Left() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.\nwe are all living in one earth!\nHello,world!Hello,world!Hello,world!\nHello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		//create cursor to select paragraph and formatting paragraph
		XTextCursor xTextCursor = xText.createTextCursor();    
		//create paragraph property set
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//create document service factory
		XMultiServiceFactory  xWriterFactory= (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);		
		//set numbering character
		XIndexAccess xNumRule = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,xWriterFactory.createInstance("com.sun.star.text.NumberingRules"));
		PropertyValue[] propsRule = {new PropertyValue(),new PropertyValue()};
		propsRule[1].Name = "NumberingType";
		propsRule[1].Value = NumberingType.ARABIC;
		propsRule[0].Name = "Adjust";
		propsRule[0].Value = HoriOrientation.LEFT;	
		XIndexReplace xReplaceRule = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule);
		xReplaceRule.replaceByIndex(0, propsRule);  
		//set paragraph numbering and bullet character
		xTextProps.setPropertyValue("NumberingRules", xNumRule);
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
		app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		XIndexAccess xNumRule_assert_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_odt.getPropertyValue("NumberingRules"));
		XIndexReplace xReplaceRule_assert_odt = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule_assert_odt);
		PropertyValue[] propsRule_assert_odt=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xReplaceRule_assert_odt.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","Adjust",propsRule_assert_odt[0].Name);
		assertEquals("assert numbering and bullet",HoriOrientation.LEFT,propsRule_assert_odt[0].Value);
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_odt[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ARABIC,propsRule_assert_odt[11].Value);

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		XIndexAccess xNumRule_assert_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_doc.getPropertyValue("NumberingRules"));
		PropertyValue[] propsRule_assert_doc=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xNumRule_assert_doc.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","Adjust",propsRule_assert_doc[0].Name);
		assertEquals("assert numbering and bullet",HoriOrientation.LEFT,propsRule_assert_doc[0].Value);
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_doc[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ARABIC,propsRule_assert_doc[11].Value);
	}
	@Test
	public void testNumberingBulletAlign_Center() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.\nwe are all living in one earth!\nHello,world!Hello,world!Hello,world!\nHello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		//create cursor to select paragraph and formatting paragraph
		XTextCursor xTextCursor = xText.createTextCursor();    
		//create paragraph property set
		XPropertySet xTextProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//create document service factory
		XMultiServiceFactory  xWriterFactory= (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);		
		//set numbering character
		XIndexAccess xNumRule = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,xWriterFactory.createInstance("com.sun.star.text.NumberingRules"));
		PropertyValue[] propsRule = {new PropertyValue(),new PropertyValue()};
		propsRule[1].Name = "NumberingType";
		propsRule[1].Value = NumberingType.ARABIC;
		propsRule[0].Name = "Adjust";
		propsRule[0].Value = HoriOrientation.CENTER;	
		XIndexReplace xReplaceRule = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule);
		xReplaceRule.replaceByIndex(0, propsRule);  
		//set paragraph numbering and bullet character
		xTextProps.setPropertyValue("NumberingRules", xNumRule);
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
		app.closeDocument(xTextDocument);

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		XIndexAccess xNumRule_assert_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_odt.getPropertyValue("NumberingRules"));
		XIndexReplace xReplaceRule_assert_odt = (XIndexReplace) UnoRuntime.queryInterface(XIndexReplace.class, xNumRule_assert_odt);
		PropertyValue[] propsRule_assert_odt=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xReplaceRule_assert_odt.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","Adjust",propsRule_assert_odt[0].Name);
		assertEquals("assert numbering and bullet",HoriOrientation.CENTER,propsRule_assert_odt[0].Value);
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_odt[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ARABIC,propsRule_assert_odt[11].Value);

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		XIndexAccess xNumRule_assert_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xCursorProps_Assert_doc.getPropertyValue("NumberingRules"));
		PropertyValue[] propsRule_assert_doc=(PropertyValue[]) UnoRuntime.queryInterface(PropertyValue[].class,xNumRule_assert_doc.getByIndex(0));
		//verify paragraph numbering and bullet alignment
		assertEquals("assert numbering and bullet","Adjust",propsRule_assert_doc[0].Name);
		assertEquals("assert numbering and bullet",HoriOrientation.CENTER,propsRule_assert_doc[0].Value);
		assertEquals("assert numbering and bullet","NumberingType",propsRule_assert_doc[11].Name);
		assertEquals("assert numbering and bullet",NumberingType.ARABIC,propsRule_assert_doc[11].Value);
	}
}
