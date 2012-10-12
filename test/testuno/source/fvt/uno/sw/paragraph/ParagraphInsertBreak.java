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

public class ParagraphInsertBreak {
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
	@Test
	public void InsertPage_BeforeBreak_NoSplit_KeepTogether() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_BEFORE);
		xCursorProps.setPropertyValue("ParaSplit",false);
		xCursorProps.setPropertyValue("ParaKeepTogether",true);
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break",false,xCursorProps_Assert_odt.getPropertyValue("ParaSplit"));
		assertEquals("assert paragraph break",true,xCursorProps_Assert_odt.getPropertyValue("ParaKeepTogether"));
		
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break",false,xCursorProps_Assert_doc.getPropertyValue("ParaSplit"));
		assertEquals("assert paragraph break",true,xCursorProps_Assert_doc.getPropertyValue("ParaKeepTogether"));
	}
	@Test
	public void InsertPage_BeforeBreak_Orphan_WindowControl() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
		xCursorProps.setPropertyValue("BreakType",com.sun.star.style.BreakType.PAGE_BEFORE);
		xCursorProps.setPropertyValue("ParaOrphans",(byte)2);
		xCursorProps.setPropertyValue("ParaWidows",(byte)2);
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break",(byte)2,xCursorProps_Assert_odt.getPropertyValue("ParaOrphans"));
		assertEquals("assert paragraph break",(byte)2,xCursorProps_Assert_odt.getPropertyValue("ParaWidows"));
		
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break",(byte)2,xCursorProps_Assert_odt.getPropertyValue("ParaOrphans"));
		assertEquals("assert paragraph break",(byte)2,xCursorProps_Assert_odt.getPropertyValue("ParaWidows"));
	}
	@Test@Ignore("Bug #120719 - [testUNO patch]the page_after break change to page_before break when save to doc.")
	public void InsertPage_AfterBreak() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_AFTER,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_AFTER,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
	}
	@Test@Ignore("Bug #120719 - [testUNO patch]the page_after break change to page_before break when save to doc.")
	public void InsertColumn_BeforeBreak() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.COLUMN_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.COLUMN_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
	}
	@Test@Ignore("Bug #120719 - [testUNO patch]the page_after break change to page_before break when save to doc.")
	public void InsertColumn_AfterBreak() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.COLUMN_AFTER,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.COLUMN_AFTER,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the endnote page break change to page default break when save to doc.")
	public void InsertPage_Endnote_BeforeBreak() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Endnote",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Endnote",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	
	@Test@Ignore("Bug #120721 - [testUNO patch]the endnote page break change to page default break when save to doc.")
	public void InsertPage_Envelop_BeforeBreak() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Envelope",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Envelope",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	
	@Test
	public void InsertPage_Firstpage_BeforeBreak() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","First Page",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","First Page",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the endnote page break change to page default break when save to doc.")
	public void InsertPage_Footnote_BeforeBreak() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Footnote",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Footnote",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the endnote page break change to page default break when save to doc.")
	public void InsertPage_HTML_BeforeBreak() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","HTML",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","HTML",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the endnote page break change to page default break when save to doc.")
	public void InsertPage_Index_BeforeBreak() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Index",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Index",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the endnote page break change to page default break when save to doc.")
	public void InsertPage_Landscape_BeforeBreak() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Landscape",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Landscape",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the endnote page break change to page default break when save to doc.")
	public void InsertPage_LeftPage_BeforeBreak() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Left Page",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Left Page",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
	@Test@Ignore("Bug #120721 - [testUNO patch]the endnote page break change to page default break when save to doc.")
	public void InsertPage_RightPage_BeforeBreak() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!Hello,world!" +
				"Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//set paragraph break type
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
		XPropertySet xCursorProps_Assert_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_odt.getText().createTextCursor());
		//verify paragraph break
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_odt.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Right Page",xCursorProps_Assert_odt.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_odt.getPropertyValue("PageNumberOffset"));
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		//verify paragraph background color
		assertEquals("assert paragraph break",com.sun.star.style.BreakType.PAGE_BEFORE,xCursorProps_Assert_doc.getPropertyValue("BreakType"));
		assertEquals("assert paragraph break","Right Page",xCursorProps_Assert_doc.getPropertyValue("PageDescName"));
		assertEquals("assert paragraph break",(short)3,xCursorProps_Assert_doc.getPropertyValue("PageNumberOffset"));
	}
}
