package fvt.uno.sw.table;

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
import com.sun.star.container.XIndexAccess;
import com.sun.star.frame.XStorable;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;

public class TableInsertBreak {
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
	@Test@Ignore("Bug #120719 - [testUNO patch]the page_after break change to page_before break when save to doc.")
	public void InsertPage_BeforeBreak_Split_KeepTogether() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		//insert page break for table 
		XPropertySet xCursorProps = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable);
		assertEquals("assert default split",true,xCursorProps.getPropertyValue("Split"));
		assertEquals("assert default keep_together",false,xCursorProps.getPropertyValue("KeepTogether"));
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_BEFORE);
		xCursorProps.setPropertyValue("Split",false);
		xCursorProps.setPropertyValue("KeepTogether",true);
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert table break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert table split",false,xCursorProps_Assert_odt.getPropertyValue("Split"));
		assertEquals("assert table keep_tpgether",true,xCursorProps_Assert_odt.getPropertyValue("KeepTogether"));

		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify paragraph background color
		assertEquals("assert table break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert table split",false,xCursorProps_Assert_doc.getPropertyValue("Split"));
		assertEquals("assert table keep_together",true,xCursorProps_Assert_doc.getPropertyValue("KeepTogether"));
	}
	@Test@Ignore("Bug #120719 - [testUNO patch]the page_after break change to page_before break when save to doc.")
	public void InsertPage_AfterBreak() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,xTable);
		//set table break type
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_AFTER);		
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_AFTER,xCursorProps_Assert_odt.getPropertyValue("BreakType"));

		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_AFTER,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
	}
	@Test@Ignore("Bug #120719 - [testUNO patch]the page_after break change to page_before break when save to doc.")
	public void InsertColumn_BeforeBreak() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,xTable);
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.COLUMN_BEFORE);		
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.COLUMN_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));

		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.COLUMN_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
	}
	@Test@Ignore("Bug #120719 - [testUNO patch]the page_after break change to page_before break when save to doc.")
	public void InsertColumn_AfterBreak() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,xTable);
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.COLUMN_AFTER);		
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.COLUMN_AFTER,xCursorProps_Assert_odt.getPropertyValue("BreakType"));

		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify table break
		assertEquals("assert table break",com.sun.star.style.BreakType.COLUMN_AFTER,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
	}
	@Test@Ignore("Bug #120719 - [testUNO patch]the page_after break change to page_before break when save to doc.")
	public void InsertPage_Endnote_BeforeBreak() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,xTable);
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_BEFORE);
		xCursorProps.setPropertyValue("PageDescName","Endnote");
		xCursorProps.setPropertyValue("PageNumberOffset",(short)3);
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Endnote",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Endnote",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}

	@Test@Ignore("Bug #120721 - [testUNO patch]the page_endnote_break type change to page_default_break when save to doc.")
	public void InsertPage_Envelop_BeforeBreak() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,xTable);
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_BEFORE);
		xCursorProps.setPropertyValue("PageDescName","Envelope");
		xCursorProps.setPropertyValue("PageNumberOffset",(short)3);
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Envelope",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Envelope",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}

	@Test
	public void InsertPage_Firstpage_BeforeBreak() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,xTable);
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_BEFORE);
		xCursorProps.setPropertyValue("PageDescName","First Page");
		xCursorProps.setPropertyValue("PageNumberOffset",(short)3);
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","First Page",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","First Page",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the page_endnote_break type change to page_default_break when save to doc.")
	public void InsertPage_Footnote_BeforeBreak() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,xTable);
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_BEFORE);
		xCursorProps.setPropertyValue("PageDescName","Footnote");
		xCursorProps.setPropertyValue("PageNumberOffset",(short)3);
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Footnote",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Footnote",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the page_endnote_break type change to page_default_break when save to doc.")
	public void InsertPage_HTML_BeforeBreak() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,xTable);
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_BEFORE);
		xCursorProps.setPropertyValue("PageDescName","HTML");
		xCursorProps.setPropertyValue("PageNumberOffset",(short)3);
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","HTML",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","HTML",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the page_endnote_break type change to page_default_break when save to doc.")
	public void InsertPage_Index_BeforeBreak() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,xTable);
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_BEFORE);
		xCursorProps.setPropertyValue("PageDescName","Index");
		xCursorProps.setPropertyValue("PageNumberOffset",(short)3);
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Index",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Index",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the page_endnote_break type change to page_default_break when save to doc.")
	public void InsertPage_Landscape_BeforeBreak() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,xTable);
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_BEFORE);
		xCursorProps.setPropertyValue("PageDescName","Landscape");
		xCursorProps.setPropertyValue("PageNumberOffset",(short)3);
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Landscape",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Landscape",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the page_endnote_break type change to page_default_break when save to doc.")
	public void InsertPage_LeftPage_BeforeBreak() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,xTable);
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_BEFORE);
		xCursorProps.setPropertyValue("PageDescName","Left Page");
		xCursorProps.setPropertyValue("PageNumberOffset",(short)3);
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Left Page",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Left Page",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the page_endnote_break type change to page_default_break when save to doc.")
	public void InsertPage_RightPage_BeforeBreak() throws Exception {

		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextTable xTable = (XTextTable)UnoRuntime.queryInterface(XTextTable.class, xWriterFactory.createInstance("com.sun.star.text.TextTable"));
		xText.insertTextContent(xTextCursor,xTable,false);
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,xTable);
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_BEFORE);
		xCursorProps.setPropertyValue("PageDescName","Right Page");
		xCursorProps.setPropertyValue("PageNumberOffset",(short)3);
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

		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.odt")));
		XTextTablesSupplier xTablesSupplier_odt = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_odt );
		XIndexAccess xIndexedTables_odt = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_odt.getTextTables());
		Object xTable_obj_odt=xIndexedTables_odt.getByIndex(0);
		XTextTable xTable_Assert_odt=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_odt);
		XPropertySet xCursorProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_odt);
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Right Page",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the doc document and assert table break
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XTextTablesSupplier xTablesSupplier_doc = (XTextTablesSupplier) UnoRuntime.queryInterface(XTextTablesSupplier.class, assertDocument_doc );
		XIndexAccess xIndexedTables_doc = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, xTablesSupplier_doc.getTextTables());
		Object xTable_obj_doc=xIndexedTables_doc.getByIndex(0);
		XTextTable xTable_Assert_doc=(XTextTable) UnoRuntime.queryInterface(XTextTable.class, xTable_obj_doc);
		XPropertySet xCursorProps_Assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTable_Assert_doc);
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Right Page",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
}
