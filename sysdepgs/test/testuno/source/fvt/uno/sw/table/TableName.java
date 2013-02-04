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
import org.junit.Ignore;
import org.junit.Test;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import com.sun.star.uno.UnoRuntime;
import com.sun.star.text.*;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.beans.PropertyValue;
import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XNamed;
import com.sun.star.frame.XStorable;


public class TableName {

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
	 * 2.set table name
	 * 3.save to odt/doc,close it and reopen new saved document
	 * 4.check the table name
	 */
	@Test@Ignore("Bug #120739 - [testUNO patch]the table name change to default name when save to doc.")
	public void testtableName() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XNamed tableName=(XNamed)UnoRuntime.queryInterface(XNamed.class, xTable);
		assertEquals("assert default table name","Table1",tableName.getName());
		tableName.setName("test_TableExample");
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

		//reopen the odt document and assert table border spacing to content
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XNamed tableName_odt=(XNamed)UnoRuntime.queryInterface(XNamed.class, xTable_Assert_odt);
		assertEquals("assert default table name","test_TableExample",tableName_odt.getName());
		
		//reopen the doc document and assert table border spacing to content
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XNamed tableName_doc=(XNamed)UnoRuntime.queryInterface(XNamed.class, xTable_Assert_doc);
		assertEquals("assert default table name","test_TableExample",tableName_doc.getName());
       
	}
}

