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

public class FrameDescription {
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
	public void testFrameDescription() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextFrame xTextFrame = (XTextFrame)UnoRuntime.queryInterface(XTextFrame.class, xWriterFactory.createInstance("com.sun.star.text.TextFrame"));
		xText.insertTextContent(xTextCursor,xTextFrame,false);
		XPropertySet xTextFrameProps = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextFrame);
		xTextFrameProps.setPropertyValue("Title", "Frame");
		xTextFrameProps.setPropertyValue("Description", "Test Frame");
		
		//save and reopen the document
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class,SWUtil.saveTo_Override_reload(xTextDocument, "writer8",Testspace.getPath("output/test.odt"), app));
		XTextFramesSupplier xTFS_odt = (XTextFramesSupplier) UnoRuntime.queryInterface(XTextFramesSupplier.class, assertDocument_odt);
		XNameAccess xTextFrames_odt = xTFS_odt.getTextFrames();
		XPropertySet xTextFrameProps_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextFrames_odt.getByName("Frame1"));
		assertEquals("Test Frame title","Frame", xTextFrameProps_odt.getPropertyValue("Title"));
		assertEquals("Test Frame description","Test Frame", xTextFrameProps_odt.getPropertyValue("Description"));
	}
}
