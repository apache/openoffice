package fvt.uno.sw.paragraph;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import com.sun.star.style.TabAlign;
import com.sun.star.style.TabStop;
import com.sun.star.text.*;
import com.sun.star.beans.*;
import com.sun.star.frame.XStorable;
import com.sun.star.uno.UnoRuntime;

public class ParagraphTabs {
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
	public void ParagraphTabs_Center() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//paraTabStops.
		TabStop[] tabStop=new TabStop[1];
		tabStop[0]=new TabStop();
		tabStop[0].Position=5001;
		tabStop[0].Alignment=TabAlign.CENTER;
		tabStop[0].FillChar='_';
		//set paragraph tab stops
		xCursorProps.setPropertyValue("ParaTabStops",tabStop);
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
		Object paraTabs_obj_odt=xCursorProps_Assert_odt.getPropertyValue("ParaTabStops");
		TabStop[] paraTabs_assert_odt=(TabStop[]) UnoRuntime.queryInterface(TabStop[].class, paraTabs_obj_odt);
		//verify paragraph tabs
		assertEquals("assert paragraph tab setting",TabAlign.CENTER,paraTabs_assert_odt[0].Alignment);
		assertEquals("assert paragraph tab setting",'_',paraTabs_assert_odt[0].FillChar);
		assertEquals("assert paragraph tab setting",5001,paraTabs_assert_odt[0].Position);
		
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		TabStop[] paraTabs_assert_doc=(TabStop[]) UnoRuntime.queryInterface(TabStop[].class, xCursorProps_Assert_doc.getPropertyValue("ParaTabStops"));
		//verify paragraph tabs
		assertEquals("assert paragraph tab setting",TabAlign.CENTER,paraTabs_assert_doc[0].Alignment);
		assertEquals("assert paragraph tab setting",'_',paraTabs_assert_doc[0].FillChar);
		assertEquals("assert paragraph tab setting",5001,paraTabs_assert_doc[0].Position);
	}
	@Test
	public void ParagraphTabs_Left() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//paraTabStops.
		TabStop[] tabStop=new TabStop[1];
		tabStop[0]=new TabStop();
		tabStop[0].Position=5001;
		tabStop[0].Alignment=TabAlign.LEFT;
		tabStop[0].FillChar='.';
		//set paragraph tab stops
		xCursorProps.setPropertyValue("ParaTabStops",tabStop);
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
		Object paraTabs_obj_odt=xCursorProps_Assert_odt.getPropertyValue("ParaTabStops");
		TabStop[] paraTabs_assert_odt=(TabStop[]) UnoRuntime.queryInterface(TabStop[].class, paraTabs_obj_odt);
		//verify paragraph tabs
		assertEquals("assert paragraph tab setting",TabAlign.LEFT,paraTabs_assert_odt[0].Alignment);
		assertEquals("assert paragraph tab setting",'.',paraTabs_assert_odt[0].FillChar);
		assertEquals("assert paragraph tab setting",5001,paraTabs_assert_odt[0].Position);
		
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		TabStop[] paraTabs_assert_doc=(TabStop[]) UnoRuntime.queryInterface(TabStop[].class, xCursorProps_Assert_doc.getPropertyValue("ParaTabStops"));
		//verify paragraph tabs
		assertEquals("assert paragraph tab setting",TabAlign.LEFT,paraTabs_assert_doc[0].Alignment);
		assertEquals("assert paragraph tab setting",'.',paraTabs_assert_doc[0].FillChar);
		assertEquals("assert paragraph tab setting",5001,paraTabs_assert_doc[0].Position);
	}
	@Test
	public void ParagraphTabs_Right() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//paraTabStops.
		TabStop[] tabStop=new TabStop[1];
		tabStop[0]=new TabStop();
		tabStop[0].Position=5001;
		tabStop[0].Alignment=TabAlign.RIGHT;
		tabStop[0].FillChar='-';
		//set paragraph tab stops
		xCursorProps.setPropertyValue("ParaTabStops",tabStop);
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
		Object paraTabs_obj_odt=xCursorProps_Assert_odt.getPropertyValue("ParaTabStops");
		TabStop[] paraTabs_assert_odt=(TabStop[]) UnoRuntime.queryInterface(TabStop[].class, paraTabs_obj_odt);
		//verify paragraph tabs
		assertEquals("assert paragraph tab setting",TabAlign.RIGHT,paraTabs_assert_odt[0].Alignment);
		assertEquals("assert paragraph tab setting",'-',paraTabs_assert_odt[0].FillChar);
		assertEquals("assert paragraph tab setting",5001,paraTabs_assert_odt[0].Position);
		
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		TabStop[] paraTabs_assert_doc=(TabStop[]) UnoRuntime.queryInterface(TabStop[].class, xCursorProps_Assert_doc.getPropertyValue("ParaTabStops"));
		//verify paragraph tabs
		assertEquals("assert paragraph tab setting",TabAlign.RIGHT,paraTabs_assert_doc[0].Alignment);
		assertEquals("assert paragraph tab setting",'-',paraTabs_assert_doc[0].FillChar);
		assertEquals("assert paragraph tab setting",5001,paraTabs_assert_doc[0].Position);
	}
	@Test
	public void ParagraphTabs_Decimal() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//paraTabStops.
		TabStop[] tabStop=new TabStop[1];
		tabStop[0]=new TabStop();
		tabStop[0].Position=5001;
		tabStop[0].Alignment=TabAlign.DECIMAL;
		tabStop[0].DecimalChar='.';
		tabStop[0].FillChar='-';
		//set paragraph tab stops
		xCursorProps.setPropertyValue("ParaTabStops",tabStop);
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
		Object paraTabs_obj_odt=xCursorProps_Assert_odt.getPropertyValue("ParaTabStops");
		TabStop[] paraTabs_assert_odt=(TabStop[]) UnoRuntime.queryInterface(TabStop[].class, paraTabs_obj_odt);
		//verify paragraph tabs
		assertEquals("assert paragraph tab setting",TabAlign.DECIMAL,paraTabs_assert_odt[0].Alignment);
		assertEquals("assert paragraph tab setting",'-',paraTabs_assert_odt[0].FillChar);
		assertEquals("assert paragraph tab setting",5001,paraTabs_assert_odt[0].Position);
		assertEquals("assert paragraph tab setting",'.',paraTabs_assert_odt[0].DecimalChar);
		
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		TabStop[] paraTabs_assert_doc=(TabStop[]) UnoRuntime.queryInterface(TabStop[].class, xCursorProps_Assert_doc.getPropertyValue("ParaTabStops"));
		//verify paragraph tabs
		assertEquals("assert paragraph tab setting",TabAlign.DECIMAL,paraTabs_assert_doc[0].Alignment);
		assertEquals("assert paragraph tab setting",'-',paraTabs_assert_doc[0].FillChar);
		assertEquals("assert paragraph tab setting",5001,paraTabs_assert_doc[0].Position);
		assertEquals("assert paragraph tab setting",'.',paraTabs_assert_doc[0].DecimalChar);
	}
	@Test@Ignore("Bug #120748 - [testUNO patch]the tabstops character of paragraph change to default when save to doc.")
	public void ParagraphTabs_Decimal_UserDefineCharacter() throws Exception {

		XTextDocument xTextDocument = (XTextDocument) UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));// new a text document
		xText = xTextDocument.getText();
		xText.setString("we are Chinese,they are American.we are all living in one earth!Hello,world!Hello,world!");
		// create text cursor for selecting and formatting text
		XTextCursor xTextCursor = xText.createTextCursor();
		XPropertySet xCursorProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextCursor);
		//paraTabStops.
		TabStop[] tabStop=new TabStop[1];
		tabStop[0]=new TabStop();
		tabStop[0].Position=5001;
		tabStop[0].Alignment=TabAlign.DECIMAL;
		tabStop[0].DecimalChar='@';
		tabStop[0].FillChar='%';
		//set paragraph tab stops
		xCursorProps.setPropertyValue("ParaTabStops",tabStop);
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
		Object paraTabs_obj_odt=xCursorProps_Assert_odt.getPropertyValue("ParaTabStops");
		TabStop[] paraTabs_assert_odt=(TabStop[]) UnoRuntime.queryInterface(TabStop[].class, paraTabs_obj_odt);
		//verify paragraph tabs
		assertEquals("assert paragraph tab setting",TabAlign.DECIMAL,paraTabs_assert_odt[0].Alignment);
		assertEquals("assert paragraph tab setting",'%',paraTabs_assert_odt[0].FillChar);
		assertEquals("assert paragraph tab setting",5001,paraTabs_assert_odt[0].Position);
		assertEquals("assert paragraph tab setting",'@',paraTabs_assert_odt[0].DecimalChar);
		
		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.loadDocument(Testspace.getPath("output/test.doc")));
		XPropertySet xCursorProps_Assert_doc = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, assertDocument_doc.getText().createTextCursor());
		TabStop[] paraTabs_assert_doc=(TabStop[]) UnoRuntime.queryInterface(TabStop[].class, xCursorProps_Assert_doc.getPropertyValue("ParaTabStops"));
		//verify paragraph tabs
		assertEquals("assert paragraph tab setting",TabAlign.DECIMAL,paraTabs_assert_doc[0].Alignment);
		assertEquals("assert paragraph tab setting",'%',paraTabs_assert_doc[0].FillChar);
		assertEquals("assert paragraph tab setting",5001,paraTabs_assert_doc[0].Position);
		assertEquals("assert paragraph tab setting",'@',paraTabs_assert_doc[0].DecimalChar);
	}
}
