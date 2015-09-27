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
package fvt.uno.sw.field;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SWUtil;

import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XEnumeration;
import com.sun.star.container.XEnumerationAccess;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextField;
import com.sun.star.text.XTextFieldsSupplier;
import com.sun.star.uno.UnoRuntime;
public class TitleField {

	private static final UnoApp app = new UnoApp();
	private static XTextDocument odtDocument = null;
	private static XTextDocument docDocument = null;
	private  static String odtSample = "uno/sw/field/Test_Sample.odt";
	private static String docSample = "uno/sw/field/Test_Sample.doc";
	
	private  static String odtSaveAsDocSample = "uno/sw/field/odtSaveAsDoc.doc";
	private static String docSaveAsODTSample = "uno/sw/field/docSaveAsODT.odt";

	@Before
	public void setUpDocument() throws Exception {
		
	}

	@After
	public void tearDownDocument() {
	

	}

	@BeforeClass
	public static void setUpConnection() throws Exception {
		app.start();
	}

	@AfterClass
	public static void tearDownConnection() throws InterruptedException,
			Exception {
		app.close();
	}
	
	/**
	 * 
	 * Test Title Field Can created and Saved in odt file
	 * 1.launch a odt document
	 * 2.Create a title field at end of this page
	 * 3.Save and Reopen this document
	 * 4.Save it as doc format and reload
	 * @throws Throwable
	 */
	@Test
	public void testTitleFieldODT() throws Throwable {
		odtDocument = SWUtil.openDocument(Testspace.prepareData(odtSample), app);
		
		
		String titleStr = "Title Test";
		createTitleFiled(odtDocument);
	
		assertTrue("Verify Title field is created use exsit Title", isContainTitleField(odtDocument, titleStr));
		assertTrue("Verify Title field is created use exsit Title, can get text from document", 
				isContainTitleText(odtDocument, titleStr));
		//Set Title 
		SWUtil.setDocumentProperty(odtDocument, "Title", titleStr);
		//Verfiy after set Title
		assertTrue("Verify Title field is created, can get text from document", 
				isContainTitleText(odtDocument, titleStr));
		
		odtDocument = SWUtil.saveAndReload(odtDocument, app);
		//verify after save and reload
		assertTrue("Verify Title field is created, after save, still exist.", isContainTitleField(odtDocument, titleStr));
		assertTrue("Verify Title field is created, can get text from document after saved.", 
				isContainTitleText(odtDocument, titleStr));
		SWUtil.saveAsDoc(odtDocument, Testspace.getUrl(odtSaveAsDocSample));
		app.closeDocument(odtDocument);
		docDocument = SWUtil.openDocumentFromURL(Testspace.getUrl(odtSaveAsDocSample), app);
		assertTrue("Verify Title field is created, after saved to doc format, field still exist.", isContainTitleField(docDocument, titleStr));
		assertTrue("Verify Title field is created, after saved to doc format, can get text from document", 
				isContainTitleText(docDocument, titleStr));
		app.closeDocument(docDocument);
	}
	
	/**
	 *  Test Title Field Can created and Saved in Doc file
	 * 1.launch a doc document
	 * 2.Create a Title field at end of this page
	 * 3.Save and Reopen this document, check Title field
	 * 3.Save as odt format and reload
	 * @throws Throwable
	 */
//	@Test
//	public void testPageCountFieldDOC() throws Throwable {
//		docDocument = SWUtil.openDocument(Testspace.prepareData(docSample), app);
//		String TitleStr = "Title Test";
//		SWUtil.setDocumentProperty(docDocument, "Title", TitleStr);
//		createTitleFiled(docDocument);
//		
//		assertTrue("Verify Title field is created.", isContainTitleField(docDocument, TitleStr));
//		assertTrue("Verify Title field is created, can get text from document", 
//				isContainTitleText(docDocument, TitleStr));
//		
//		docDocument = SWUtil.saveAndReload(docDocument, app);
//		//verify after save and reload
//		assertTrue("Verify Title field is created, after save, still exist.", isContainTitleField(docDocument, TitleStr));
//		assertTrue("Verify Title field is created, can get text from document after saved.", 
//				isContainTitleText(docDocument, TitleStr));
//		SWUtil.saveAsODT(docDocument, Testspace.getUrl(docSaveAsODTSample));
//		app.closeDocument(docDocument);
//		odtDocument = SWUtil.openDocumentFromURL(Testspace.getUrl(docSaveAsODTSample), app);
//		assertTrue("Verify Title field is created, after saved to doc format, field still exist.", isContainTitleField(odtDocument, TitleStr));
//		assertTrue("Verify Title field is created, after saved to doc format, can get text from document", 
//				isContainTitleText(odtDocument, TitleStr));
//		app.closeDocument(odtDocument);
//	}
	

	
	/**
	 * Create a Title field at start of this document
	 * @param document
	 * @throws Exception
	 */
	private void createTitleFiled(XTextDocument document) throws Exception {
		
		XMultiServiceFactory sevriceFactory = (XMultiServiceFactory) UnoRuntime.queryInterface(XMultiServiceFactory.class, document);
		XTextField  TitleField = (XTextField)UnoRuntime.queryInterface(XTextField.class, sevriceFactory.createInstance("com.sun.star.text.textfield.docinfo.Title"));
		
	
		SWUtil.moveCuror2Start(document);
		document.getText().insertTextContent(document.getText().getStart(), TitleField, true);
		
	}

	
	/**
	 * Check is contain Title content at start of this document
	 * 
	 * @param document
	 * @param content
	 * @return
	 */
	private boolean isContainTitleText(XTextDocument document, String content) {
		String documentString = document.getText().getString().trim();
		return documentString.indexOf(content) == 0;
	}
	
	/**
	 * Check is contain Title field
	 * @param document
	 * @throws Exception
	 */
	private boolean isContainTitleField(XTextDocument document, String content) throws Exception {
		XTextFieldsSupplier fieldsSupplier = (XTextFieldsSupplier) UnoRuntime.queryInterface(XTextFieldsSupplier.class, document);
		XEnumerationAccess xEnumeratedFields = fieldsSupplier.getTextFields();
		XEnumeration enumeration = xEnumeratedFields.createEnumeration();
		while (enumeration.hasMoreElements()) {
			  	Object field =  enumeration.nextElement();
				XPropertySet props = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, field);
				String strContent = (String) props.getPropertyValue("CurrentPresentation");
			    return content.equals(strContent);
			
		}
		return false;
		
	}
}
