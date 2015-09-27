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
import com.sun.star.text.WrapTextMode;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextFrame;
import com.sun.star.text.XTextFramesSupplier;
import com.sun.star.uno.UnoRuntime;
@RunWith(value = Parameterized.class)
public class FrameWrapType {
	private static final UnoApp app = new UnoApp();
	private XTextDocument xTextDocument=null;
	private XMultiServiceFactory xWriterFactory=null;
	private XText xText=null;
	private int FrameWrapType;
	public FrameWrapType(int FrameWrapType){
		this.FrameWrapType=FrameWrapType;
	}
	/*
	 * 0:NONE 	  
	 * 1:THROUGHT 	 
	 * 2:PARALLEL 	  
	 * 3:DYNAMIC 	  
	 * 4:LEFT 	  
	 * 5:RIGHT
	 */
	@Parameters
    public static Collection<Object[]> data(){
    	Object[][] params = new Object[][]{
    	{0},{1},{2},{3},{4},{5},
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
		xTextFrameProps1.setPropertyValue("Surround",FrameWrapType);
		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, SWUtil.saveTo_Override_reload(xTextDocument,"writer8", Testspace.getPath("output/test.odt"),app));
		XTextFramesSupplier xTFS_odt = (XTextFramesSupplier) UnoRuntime.queryInterface(XTextFramesSupplier.class, assertDocument_odt);
		XNameAccess xTextFrames_odt = xTFS_odt.getTextFrames();
		XTextFrame xTextFrame_Assert1=(XTextFrame) UnoRuntime.queryInterface(XTextFrame.class, xTextFrames_odt.getByName("Frame1"));
		XPropertySet xTextFrameProps_assert1 = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xTextFrame_Assert1);
		WrapTextMode surrond=(WrapTextMode)UnoRuntime.queryInterface(WrapTextMode.class,xTextFrameProps_assert1.getPropertyValue("Surround"));
		assertEquals("assert frame wrap type",FrameWrapType,surrond.getValue());
	}
}
