package fvt.uno.sw.frame;

import static org.junit.Assert.*;

import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.AfterClass;
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
import com.sun.star.table.ShadowFormat;
import com.sun.star.table.ShadowLocation;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextFrame;
import com.sun.star.text.XTextFramesSupplier;
import com.sun.star.uno.UnoRuntime;
@RunWith(value = Parameterized.class)
public class FrameShadow {
	private static final UnoApp app = new UnoApp();
	private XTextDocument xTextDocument=null;
	private XMultiServiceFactory xWriterFactory=null;
	private XText xText=null;
	private int shadowColor;
	private short shadowWidth;
	private ShadowLocation shadowLocation;
	public FrameShadow(ShadowLocation shadowLocation,short shadowWidth,int shadowColor){
		this.shadowLocation=shadowLocation;
		this.shadowWidth=shadowWidth;
		this.shadowColor=shadowColor;
	}
	@Parameters
    public static Collection<Object[]> data(){
    	Object[][] params = new Object[][]{
    			{ShadowLocation.NONE,(short)0,0},
    			{ShadowLocation.TOP_LEFT,(short)101,0x00FF00FF},
    			{ShadowLocation.TOP_RIGHT,(short)101,0x00FF00FF},
    			{ShadowLocation.BOTTOM_LEFT,(short)101,0x00FF00FF},
    			{ShadowLocation.BOTTOM_RIGHT,(short)101,0x00FF00FF},	
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
	public void testFrameShadow() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextFrame xTextFrame1 = (XTextFrame)UnoRuntime.queryInterface(XTextFrame.class, xWriterFactory.createInstance("com.sun.star.text.TextFrame"));
		xText.insertTextContent(xTextCursor,xTextFrame1,false);
		XPropertySet xTextFrameProps1 = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextFrame1);
		ShadowFormat shadowFormat=new ShadowFormat();
		shadowFormat.Color=shadowColor;
		shadowFormat.Location=shadowLocation;
		shadowFormat.ShadowWidth=shadowWidth;
		xTextFrameProps1.setPropertyValue("ShadowFormat",shadowFormat);
		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, SWUtil.saveTo_Override_reload(xTextDocument,"writer8", Testspace.getPath("output/test.odt"),app));
		XTextFramesSupplier xTFS_odt = (XTextFramesSupplier) UnoRuntime.queryInterface(XTextFramesSupplier.class, assertDocument_odt);
		XNameAccess xTextFrames_odt = xTFS_odt.getTextFrames();
		XTextFrame xTextFrame_Assert1=(XTextFrame) UnoRuntime.queryInterface(XTextFrame.class, xTextFrames_odt.getByName("Frame1"));
		XPropertySet xTextFrameProps_assert1 = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextFrame_Assert1);
		ShadowFormat shadowFormat_Assert1=(ShadowFormat) UnoRuntime.queryInterface(ShadowFormat.class, xTextFrameProps_assert1.getPropertyValue("ShadowFormat"));
		assertEquals("assert shadow color",shadowColor,shadowFormat_Assert1.Color);
		assertEquals("assert shadow location",shadowLocation,shadowFormat_Assert1.Location);
		assertEquals("assert shadow width",shadowWidth,shadowFormat_Assert1.ShadowWidth);
	}
}
