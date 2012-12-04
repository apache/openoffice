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
public class FramePosition {
	private static final UnoApp app = new UnoApp();
	private XTextDocument xTextDocument=null;
	private XMultiServiceFactory xWriterFactory=null;
	private XText xText=null;
	private short HoriOrient;
	private short HoriOrientRelation;
	private short VertOrient;
	private short VertOrientRelation;
	public FramePosition(short HoriOrient,short HoriOrientRelation,short VertOrient,short VertOrientRelation){
		this.HoriOrient=HoriOrient;
		this.HoriOrientRelation = HoriOrientRelation;
		this.VertOrient=VertOrient;
		this.VertOrientRelation = VertOrientRelation;	
	}
	@Parameters
    public static Collection<Object[]> data(){
    	Object[][] params = new Object[][]{
    			{(short)0,(short)0,(short)0,(short)0},
    			{(short)1,(short)1,(short)1,(short)1},
    			{(short)2,(short)2,(short)2,(short)2},
    			{(short)3,(short)3,(short)3,(short)2},
    			{(short)0,(short)4,(short)2,(short)0},
    			{(short)1,(short)5,(short)2,(short)2},		
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
	public void testFramePosition() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextFrame xTextFrame = (XTextFrame)UnoRuntime.queryInterface(XTextFrame.class, xWriterFactory.createInstance("com.sun.star.text.TextFrame"));
		xText.insertTextContent(xTextCursor,xTextFrame,false);
		XPropertySet xFrameProps = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextFrame);
		xFrameProps.setPropertyValue("HoriOrient", HoriOrient);
		xFrameProps.setPropertyValue("HoriOrientRelation", HoriOrientRelation);
		xFrameProps.setPropertyValue("VertOrient", VertOrient);
		xFrameProps.setPropertyValue("VertOrientRelation", VertOrientRelation);
		
		//reopen the odt document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class,SWUtil.saveTo_Override_reload(xTextDocument, "writer8",Testspace.getPath("output/test.odt"), app));
		XTextFramesSupplier xTFS_odt = (XTextFramesSupplier) UnoRuntime.queryInterface(XTextFramesSupplier.class, assertDocument_odt);
		XNameAccess xTextFrames_odt = xTFS_odt.getTextFrames();
		Object xTextFrame_obj_odt=xTextFrames_odt.getByName("Frame1");
		XTextFrame xTextFrame_Assert_odt=(XTextFrame) UnoRuntime.queryInterface(XTextFrame.class, xTextFrame_obj_odt);
		XPropertySet xFramProps_odt = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextFrame_Assert_odt);
		assertEquals("verify Frame position",HoriOrient,xFramProps_odt.getPropertyValue("HoriOrient"));
		assertEquals("verify Frame position",HoriOrientRelation,xFramProps_odt.getPropertyValue("HoriOrientRelation"));
		assertEquals("verify Frame position",VertOrient,xFramProps_odt.getPropertyValue("VertOrient"));
		assertEquals("verify Frame position",VertOrientRelation,xFramProps_odt.getPropertyValue("VertOrientRelation"));
	}
}
