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

package fvt.uno.sw;

import static org.openoffice.test.common.Testspace.*;

import java.io.File;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.Assert;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SWUtil;

import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XText;
import com.sun.star.beans.XPropertySet;
import com.sun.star.beans.PropertyValue;
import com.sun.star.frame.*;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.lang.XComponent;


public class DocumentTest {
	UnoApp unoApp = new UnoApp();
	XTextDocument textDocument = null;
	File temp = null;
	String workingFilePath = "";
	String workingTemplatePath = "";

	@Before
	public void setUp() throws Exception {
		unoApp.start();
		
		FileUtil.deleteFile(getPath("temp"));
		temp = new File(getPath("temp"));
		temp.mkdirs();
		
		//copy sample file to temp folder
		workingFilePath = prepareData("uno/sw/DocumentTest.odt");	
		workingTemplatePath = prepareData("uno/sw/DocumentTest.ott");			
	}

	@After
	public void tearDown() throws Exception {
		unoApp.close();
	}	
	/**
	 * test new document and close document
	 * @throws Exception
	 */
	@Test
	public void testNewAndCloseDocument() throws Exception
	{
		XComponent component = unoApp.newDocument("swriter");
		textDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, component);
		XTitle xTitle = (XTitle)UnoRuntime.queryInterface(XTitle.class, textDocument);
		String title = xTitle.getTitle();
		Assert.assertEquals("New Document title start with \"Untitled\"",true, title.startsWith("Untitled"));
		unoApp.closeDocument(component);
		XDesktop xDesktop = unoApp.getDesktop();
		XFrame xFrame     = (xDesktop == null) ? null : xDesktop.getCurrentFrame();
		XController xCtrl = (xFrame == null)   ? null : xFrame.getController();
		XModel xModel     = (xCtrl == null)    ? null : xCtrl.getModel();
		Assert.assertTrue("Document has been closed.",xModel==null);
	}	
	/**
	 * test new document from template
	 * @throws Exception
	 */
	@Test
	public void testNewDocumentFromTemplate() throws Exception
	{					
		XComponent component = SWUtil.newDocumentFromTemplate(workingTemplatePath,unoApp);
		textDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, component);
		XTitle xTitle = (XTitle)UnoRuntime.queryInterface(XTitle.class, textDocument);
		XText xText = textDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();	
		xTextCursor.gotoEnd(true);		
		XPropertySet xPropertySet = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		String paraStyle = (String)xPropertySet.getPropertyValue("ParaStyleName");		
        Assert.assertEquals("new document from template, heading style in template is remained. ", "Heading 1", paraStyle); 		       
        Assert.assertEquals("new document from template, title start with \"Untitled\".", true, xTitle.getTitle().startsWith("Untitled"));
	}
	
	/**
	 * test save document as odt
	 * @throws Exception
	 */
	@Test
	public void testSaveDocument() throws Exception
	{			
		XComponent component = unoApp.loadDocument(workingFilePath);
		textDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, component);
		XText xText = textDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();	
		xTextCursor.gotoEnd(true);		
		XPropertySet xPropertySet = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);		
		xPropertySet.setPropertyValue("ParaStyleName", "Heading 1");				
		SWUtil.save(textDocument);         
        unoApp.closeDocument(textDocument);              
        component = unoApp.loadDocument(workingFilePath);
		textDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, component);
		xText = textDocument.getText();
		xTextCursor = xText.createTextCursor();	
		xTextCursor.gotoEnd(true);		
		xPropertySet = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);				
        Assert.assertEquals("Modify plain text to heading 1 style. ", "Heading 1", (String)xPropertySet.getPropertyValue("ParaStyleName"));        
	}
	
	/**
	 * test save document as doc
	 * @throws Exception
	 */
	@Test
	public void testSaveAsDocument() throws Exception
	{		
		File saveAsFile = new File(workingFilePath + ".doc");			
		XComponent component = unoApp.loadDocument(workingFilePath);		
		textDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, component);
		XText xText = textDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();		
		XPropertySet xPropertySet = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		
		xPropertySet.setPropertyValue("ParaStyleName", "Heading 1");
		xText.insertString(xTextCursor, "test Save odt as doc.", false);
		SWUtil.saveAsDoc(textDocument, FileUtil.getUrl(saveAsFile));      
        Assert.assertTrue("Save odt document as doc the file exist: " + saveAsFile.getAbsolutePath(), saveAsFile.exists()); 
	}
	
	/**
	 * test export document as pdf
	 * @throws Exception
	 */
	@Test
	public void testExportAsPDF() throws Exception
	{		
		File saveAsFile = new File(workingFilePath + ".pdf");		
		XComponent component = unoApp.loadDocument(workingFilePath);
		textDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, component);
				
		XStorable xStorable = (XStorable)UnoRuntime.queryInterface(XStorable.class, component);
        PropertyValue[] storeProps = new PropertyValue[3];        
        
        storeProps[0] = new PropertyValue();
        storeProps[0].Name = "Overwrite";
        storeProps[0].Value = new Boolean(true);
        
        storeProps[1] = new PropertyValue();
        storeProps[1].Name = "FilterName";
        storeProps[1].Value = "writer_pdf_Export";  
        
        storeProps[2] = new PropertyValue();
        storeProps[2].Name = "CompressionMode";
        storeProps[2].Value = "1";         
        
        xStorable.storeToURL(FileUtil.getUrl(saveAsFile), storeProps); 
       
        Assert.assertTrue("Export document as PDF.", saveAsFile.exists()); 
	}
	
	/**
	 * test save document as template
	 * @throws Exception
	 */
	@Test
	public void testSaveAsTemplate() throws Exception
	{
		File saveAsFile = new File(workingFilePath + ".ott");			
		XComponent component = unoApp.loadDocument(workingFilePath);		
		textDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, component);
		XText xText = textDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();	
		xTextCursor.gotoEnd(true);		
		XPropertySet xPropertySet = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		
		xPropertySet.setPropertyValue("ParaStyleName", "Heading 1");		
		
		XStorable xStorable = (XStorable)UnoRuntime.queryInterface(XStorable.class, component);
        xStorable.store();    
        
		PropertyValue[] storeProps = new PropertyValue[3];
		storeProps[0] = new PropertyValue();
		storeProps[0].Name="TemplateName";
		storeProps[0].Value="MyNewCreatedTemplate";
		
		storeProps[1] = new PropertyValue();
		storeProps[1].Name="TemplateRegionName";
		storeProps[1].Value="My Templates";
		
		storeProps[2] = new PropertyValue();
		storeProps[2].Name="AsTemplate";
		storeProps[2].Value=new Boolean(true);
		
		xStorable.storeToURL(FileUtil.getUrl(saveAsFile), storeProps);
		unoApp.closeDocument(textDocument);		
		Assert.assertTrue("Export document as ott.", saveAsFile.exists());
	}

}
