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
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XNameAccess;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.style.GraphicLocation;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextFrame;
import com.sun.star.text.XTextFramesSupplier;
import com.sun.star.uno.UnoRuntime;
import testlib.uno.SWUtil;
@RunWith(value = Parameterized.class)
public class FrameBackGraphic {
	private static final UnoApp app = new UnoApp();
	private XTextDocument xTextDocument=null;
	private XMultiServiceFactory xWriterFactory=null;
	private XText xText=null;
	private int backGraphicLocation;
	private String graphicURL=null;
	private String graphicName=null;
	public FrameBackGraphic(String graphicURL,String graphicName,int backGraphicLocation){
		this.graphicURL=graphicURL;
		this.graphicName=graphicName;
		this.backGraphicLocation = backGraphicLocation;		
	}
	@Parameters
    public static Collection<Object[]> data(){
    	Object[][] params = new Object[][]{
    			{FileUtil.getUrl(Testspace.prepareData("uno/Desert.jpg")),"draw_jpg_Export",1},
    			{FileUtil.getUrl(Testspace.prepareData("uno/Desert.jpg")),"draw_jpg_Export",2},
    			{FileUtil.getUrl(Testspace.prepareData("uno/Desert.jpg")),"draw_jpg_Export",3},
    			{FileUtil.getUrl(Testspace.prepareData("uno/Desert.jpg")),"draw_jpg_Export",4},
    			{FileUtil.getUrl(Testspace.prepareData("uno/Desert.jpg")),"draw_jpg_Export",5},
    			{FileUtil.getUrl(Testspace.prepareData("uno/Desert.jpg")),"draw_jpg_Export",6},
    			{FileUtil.getUrl(Testspace.prepareData("uno/Desert.jpg")),"draw_jpg_Export",7},
    			{FileUtil.getUrl(Testspace.prepareData("uno/Desert.jpg")),"draw_jpg_Export",8},
    			{FileUtil.getUrl(Testspace.prepareData("uno/Desert.jpg")),"draw_jpg_Export",9},
    			{FileUtil.getUrl(Testspace.prepareData("uno/Desert.jpg")),"draw_jpg_Export",10},
    			{FileUtil.getUrl(Testspace.prepareData("uno/Desert.jpg")),"draw_jpg_Export",11}		
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
	public void testFrameBackGraphic() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new Frame from the document's factory
		XTextFrame xTextFrame1 = (XTextFrame)UnoRuntime.queryInterface(XTextFrame.class, xWriterFactory.createInstance("com.sun.star.text.TextFrame"));
		xText.insertTextContent(xTextCursor,xTextFrame1,false);
		XPropertySet xFrameProps1 = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextFrame1);
		xFrameProps1.setPropertyValue("BackGraphicURL",graphicURL);
		xFrameProps1.setPropertyValue("BackGraphicFilter",graphicName);
		xFrameProps1.setPropertyValue("BackGraphicLocation",backGraphicLocation);
		XTextDocument xTextDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class,SWUtil.saveTo_Override_reload(xTextDocument, "writer8",Testspace.getPath("output/test.odt"), app));
		XTextFramesSupplier xTFS_odt = (XTextFramesSupplier) UnoRuntime.queryInterface(XTextFramesSupplier.class,xTextDocument_odt);
		XNameAccess xTextFrames_odt = xTFS_odt.getTextFrames();
		Object xFrame_obj1=xTextFrames_odt.getByName("Frame1");
		XTextFrame xFrame_Assert1=(XTextFrame) UnoRuntime.queryInterface(XTextFrame.class, xFrame_obj1);
		XPropertySet xFrameProps1_assert = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xFrame_Assert1);
		GraphicLocation graphiclocation=(GraphicLocation)xFrameProps1_assert.getPropertyValue("BackGraphicLocation");
		assertEquals("verify Frame backgraphic location",backGraphicLocation,graphiclocation.getValue());
		assertEquals("verify Frame backgraphic fileter",graphicName,xFrameProps1_assert.getPropertyValue("BackGraphicFilter"));
		assertEquals("verify Frame backgraphic URL",graphicURL,xFrameProps1_assert.getPropertyValue("BackGraphicURL"));
	}
}
