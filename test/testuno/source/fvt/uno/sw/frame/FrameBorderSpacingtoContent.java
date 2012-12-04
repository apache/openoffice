package fvt.uno.sw.frame;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SWUtil;

import com.sun.star.uno.UnoRuntime;
import com.sun.star.text.*;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XNameAccess;


public class FrameBorderSpacingtoContent {

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
	public void testFrameBorderSpacingtoContent() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextFrame xTextFrame = (XTextFrame)UnoRuntime.queryInterface(XTextFrame.class, xWriterFactory.createInstance("com.sun.star.text.TextFrame"));
		xText.insertTextContent(xTextCursor,xTextFrame,false);
		//set frame border spacing to content
		XPropertySet xCursorProps = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTextFrame);
		xCursorProps.setPropertyValue("LeftBorderDistance",499);
		xCursorProps.setPropertyValue("RightBorderDistance",499);
		xCursorProps.setPropertyValue("TopBorderDistance",499);
		xCursorProps.setPropertyValue("BottomBorderDistance",499);
		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class,SWUtil.saveTo_Override_reload(xTextDocument, "writer8",Testspace.getPath("output/test.odt"), app));
		XTextFramesSupplier xTFS_odt = (XTextFramesSupplier) UnoRuntime.queryInterface(XTextFramesSupplier.class, assertDocument_odt);
		XNameAccess xTextFrames_odt = xTFS_odt.getTextFrames();
		Object xTextFrame_obj_odt=xTextFrames_odt.getByName("Frame1");
		XTextFrame xTextFrame_Assert_odt=(XTextFrame) UnoRuntime.queryInterface(XTextFrame.class, xTextFrame_obj_odt);
		XPropertySet xCursorProps_assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTextFrame_Assert_odt);
		assertEquals("assert table border spacing to content",499,xCursorProps_assert_odt.getPropertyValue("LeftBorderDistance"));
		assertEquals("assert table border spacing to content",499,xCursorProps_assert_odt.getPropertyValue("RightBorderDistance"));
		assertEquals("assert table border spacing to content",499,xCursorProps_assert_odt.getPropertyValue("TopBorderDistance"));
		assertEquals("assert table border spacing to content",499,xCursorProps_assert_odt.getPropertyValue("BottomBorderDistance"));

		//reopen the document 
		XTextDocument assertDocument_doc=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class,SWUtil.saveTo_Override_reload(xTextDocument, "MS Word 97",Testspace.getPath("output/test.doc"), app));
		XTextFramesSupplier xTFS_doc = (XTextFramesSupplier) UnoRuntime.queryInterface(XTextFramesSupplier.class, assertDocument_doc);
		XNameAccess xTextFrames_doc = xTFS_doc.getTextFrames();
		Object xTextFrame_obj_doc=xTextFrames_doc.getByName("Frame1");
		XTextFrame xTextFrame_Assert_doc=(XTextFrame) UnoRuntime.queryInterface(XTextFrame.class, xTextFrame_obj_doc);
		XPropertySet xCursorProps_assert_doc = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xTextFrame_Assert_doc);
		assertEquals("assert table border spacing to content",499,xCursorProps_assert_doc.getPropertyValue("LeftBorderDistance"));
		assertEquals("assert table border spacing to content",499,xCursorProps_assert_doc.getPropertyValue("RightBorderDistance"));
		assertEquals("assert table border spacing to content",499,xCursorProps_assert_doc.getPropertyValue("TopBorderDistance"));
		assertEquals("assert table border spacing to content",499,xCursorProps_assert_doc.getPropertyValue("BottomBorderDistance"));
	}
}

