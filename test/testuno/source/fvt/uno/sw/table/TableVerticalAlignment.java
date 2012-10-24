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

package fvt.uno.sw.table;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import com.sun.star.uno.UnoRuntime;
import com.sun.star.text.*;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XIndexAccess;
import com.sun.star.frame.XStorable;


public class TableVerticalAlignment {

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
	/*
	 * test table border spacing to content
	 * 1.new a text document and create a table
	 * 2.set table cell vertical alignment
	 * 3.save to odt/doc,close it and reopen new saved document
	 * 4.check the table cell vertical alignment
	 */
	@Test
	public void testtableVerticalAlignment_Bottom() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		String[] cellName=xTable.getCellNames();
		int i=0;
		while(cellName[i] != null)
		{
		XPropertySet xCursorProps = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable.getCellByName(cellName[i]));
		xCursorProps.setPropertyValue("VertOrient",VertOrientation.BOTTOM);
		i++;
		if(i==4)break;
		}
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
		app.closeDocument(xTextDocument);

		//reopen the odt document and assert table vertical alignment
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		String[] cellName_assert_odt=xTable_Assert_odt.getCellNames();
		int j=0;
		while(cellName_assert_odt[j] != null)
		{
		XPropertySet xCursorProps_assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt.getCellByName(cellName_assert_odt[j]));
		assertEquals("assert table border spacing to content",VertOrientation.BOTTOM,xCursorProps_assert_odt.getPropertyValue("VertOrient"));
		j++;
		if(j==4)break;
		}
		
		//reopen the doc document and assert table vertical alignment
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		String[] cellName_assert_doc=xTable_Assert_doc.getCellNames();
		int k=0;
		while(cellName_assert_doc[k] != null)
		{
		XPropertySet xCursorProps_assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc.getCellByName(cellName_assert_doc[k]));
		assertEquals("assert table vertical alignment",VertOrientation.BOTTOM,xCursorProps_assert_odt.getPropertyValue("VertOrient"));
		k++;
		if(k==4)break;
		}
	}
	@Test
	public void testtableVerticalAlignment_Center() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		String[] cellName=xTable.getCellNames();
		int i=0;
		while(cellName[i] != null)
		{
		XPropertySet xCursorProps = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable.getCellByName(cellName[i]));
		xCursorProps.setPropertyValue("VertOrient",VertOrientation.CENTER);
		i++;
		if(i==4)break;
		}
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
		app.closeDocument(xTextDocument);

		//reopen the odt document and assert table vertical alignment
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		String[] cellName_assert_odt=xTable_Assert_odt.getCellNames();
		int j=0;
		while(cellName_assert_odt[j] != null)
		{
		XPropertySet xCursorProps_assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt.getCellByName(cellName_assert_odt[j]));
		assertEquals("assert table border spacing to content",VertOrientation.CENTER,xCursorProps_assert_odt.getPropertyValue("VertOrient"));
		j++;
		if(j==4)break;
		}
		
		//reopen the doc document and assert table vertical alignment
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		String[] cellName_assert_doc=xTable_Assert_doc.getCellNames();
		int k=0;
		while(cellName_assert_doc[k] != null)
		{
		XPropertySet xCursorProps_assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc.getCellByName(cellName_assert_doc[k]));
		assertEquals("assert table vertical alignment",VertOrientation.CENTER,xCursorProps_assert_odt.getPropertyValue("VertOrient"));
		k++;
		if(k==4)break;
		}
	}
	@Test
	public void testtableVerticalAlignment_Top() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		String[] cellName=xTable.getCellNames();
		int i=0;
		while(cellName[i] != null)
		{
		XPropertySet xCursorProps = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable.getCellByName(cellName[i]));
		xCursorProps.setPropertyValue("VertOrient",VertOrientation.TOP);
		i++;
		if(i==4)break;
		}
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
		app.closeDocument(xTextDocument);

		//reopen the odt document and assert table vertical alignment
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		String[] cellName_assert_odt=xTable_Assert_odt.getCellNames();
		int j=0;
		while(cellName_assert_odt[j] != null)
		{
		XPropertySet xCursorProps_assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt.getCellByName(cellName_assert_odt[j]));
		assertEquals("assert table border spacing to content",VertOrientation.TOP,xCursorProps_assert_odt.getPropertyValue("VertOrient"));
		j++;
		if(j==4)break;
		}
		
		//reopen the doc document and assert table vertical alignment
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		String[] cellName_assert_doc=xTable_Assert_doc.getCellNames();
		int k=0;
		while(cellName_assert_doc[k] != null)
		{
		XPropertySet xCursorProps_assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc.getCellByName(cellName_assert_doc[k]));
		assertEquals("assert table vertical alignment",VertOrientation.TOP,xCursorProps_assert_odt.getPropertyValue("VertOrient"));
		k++;
		if(k==4)break;
		}
	}
}

