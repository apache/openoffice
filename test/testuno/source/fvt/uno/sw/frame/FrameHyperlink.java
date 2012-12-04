package fvt.uno.sw.frame;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.Test;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SWUtil;

import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XNameAccess;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextFrame;
import com.sun.star.text.XTextFramesSupplier;
import com.sun.star.uno.UnoRuntime;

public class FrameHyperlink {
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
		app.closeDocument(xTextDocument);
	}
	@AfterClass
	public static void tearDownConnection() throws Exception {
		app.close();
	}

	@Test
	public void testFrameHyperlink() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextFrame xTextFrame = (XTextFrame)UnoRuntime.queryInterface(XTextFrame.class, xWriterFactory.createInstance("com.sun.star.text.TextFrame"));
		xText.insertTextContent(xTextCursor,xTextFrame,false);
		XPropertySet xTextFrameProps = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextFrame);
		xTextFrameProps.setPropertyValue("HyperLinkURL","http://www.google.com.hk/");
		xTextFrameProps.setPropertyValue("HyperLinkTarget","google");
		xTextFrameProps.setPropertyValue("HyperLinkName","FrameHyperlinkToGoogle");

		//save and reopen the document
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class,SWUtil.saveTo_Override_reload(xTextDocument, "writer8",Testspace.getPath("output/test.odt"), app));
		XTextFramesSupplier xTFS_odt = (XTextFramesSupplier) UnoRuntime.queryInterface(XTextFramesSupplier.class, assertDocument_odt);
		XNameAccess xTextFrames_odt = xTFS_odt.getTextFrames();
		XTextFrame xTextFrame_Assert=(XTextFrame) UnoRuntime.queryInterface(XTextFrame.class, xTextFrames_odt.getByName("Frame1"));
		XPropertySet xTextFrameProps_assert = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextFrame_Assert);
		assertEquals("assert Frame hyperlink url","http://www.google.com.hk/", xTextFrameProps_assert.getPropertyValue("HyperLinkURL"));
		assertEquals("assert Frame hyperlink target","google", xTextFrameProps_assert.getPropertyValue("HyperLinkTarget"));
		assertEquals("assert Frame hyperlink name", "FrameHyperlinkToGoogle",xTextFrameProps_assert.getPropertyValue("HyperLinkName"));
	}
}
