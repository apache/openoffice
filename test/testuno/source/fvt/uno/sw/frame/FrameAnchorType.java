package fvt.uno.sw.frame;

import static org.junit.Assert.*;

import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;
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
@RunWith(value = Parameterized.class)
public class FrameAnchorType {
	private static final UnoApp app = new UnoApp();
	private XTextDocument xTextDocument=null;
	private XMultiServiceFactory xWriterFactory=null;
	private XText xText=null;
	private com.sun.star.text.TextContentAnchorType FrameAnchorType;
	public FrameAnchorType(com.sun.star.text.TextContentAnchorType FrameAnchorType){
		this.FrameAnchorType=FrameAnchorType;
	}
	@Parameters
    public static Collection<Object[]> data(){
    	Object[][] params = new Object[][]{
    			{com.sun.star.text.TextContentAnchorType.AS_CHARACTER},
    			{com.sun.star.text.TextContentAnchorType.AT_CHARACTER},
    			{com.sun.star.text.TextContentAnchorType.AT_PAGE},
    			{com.sun.star.text.TextContentAnchorType.AT_PARAGRAPH},
    			};
    	return Arrays.asList(params);
    }
	@Before
	public void setUp() throws Exception {
		app.start();		
	}
	@After
	public void tearDown() throws Exception {
		app.close();
	}

	@Test
	public void testFrameAnchorType() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextFrame xTextFrame1 = (XTextFrame)UnoRuntime.queryInterface(XTextFrame.class, xWriterFactory.createInstance("com.sun.star.text.TextFrame"));
		xText.insertTextContent(xTextCursor,xTextFrame1,false);
		XPropertySet xTextFrameProps1 = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextFrame1);
		xTextFrameProps1.setPropertyValue("AnchorType",FrameAnchorType);
		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, SWUtil.saveTo_Override_reload(xTextDocument,"writer8", Testspace.getPath("output/test.odt"),app));
		XTextFramesSupplier xTFS_odt = (XTextFramesSupplier) UnoRuntime.queryInterface(XTextFramesSupplier.class, assertDocument_odt);
		XNameAccess xTextFrames_odt = xTFS_odt.getTextFrames();
		XTextFrame xTextFrame_Assert1=(XTextFrame) UnoRuntime.queryInterface(XTextFrame.class, xTextFrames_odt.getByName("Frame1"));
		XPropertySet xTextFrameProps_assert1 = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextFrame_Assert1);
		assertEquals("assert shadow anchor type",FrameAnchorType,xTextFrameProps_assert1.getPropertyValue("AnchorType"));
	}
}
