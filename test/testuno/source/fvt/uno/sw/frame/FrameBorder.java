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

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SWUtil;

import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XNameAccess;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.table.BorderLine;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextFrame;
import com.sun.star.text.XTextFramesSupplier;
import com.sun.star.uno.UnoRuntime;

public class FrameBorder {
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
	public void testInsertFrame() throws Exception {
		xTextDocument =(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, app.newDocument("swriter"));
		xText=xTextDocument.getText();
		XTextCursor xTextCursor = xText.createTextCursor();
		// get internal service factory of the document
		xWriterFactory =(XMultiServiceFactory)UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDocument);
		// Create a new table from the document's factory
		XTextFrame xTextFrame = (XTextFrame)UnoRuntime.queryInterface(XTextFrame.class, xWriterFactory.createInstance("com.sun.star.text.TextFrame"));
		xText.insertTextContent(xTextCursor,xTextFrame,false);		
		XPropertySet xTextFramerops = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextFrame);
		//set frame border	
		BorderLine[]borderLine=new BorderLine[] {new BorderLine(),new BorderLine(),new BorderLine(),new BorderLine()};
		borderLine[0].Color=0x00FF0000;
		borderLine[0].InnerLineWidth=101;
		borderLine[0].OuterLineWidth=19;
		borderLine[0].LineDistance=100;
		borderLine[1].Color =0x00FFFF00;
		borderLine[1].InnerLineWidth=101;
		borderLine[1].OuterLineWidth=19;
		borderLine[1].LineDistance=101;	
		borderLine[2].Color =0x0000FF00;
		borderLine[2].InnerLineWidth=150;
		borderLine[2].OuterLineWidth=19;
		borderLine[2].LineDistance=101;
		borderLine[3].Color =0x0000FF00;
		borderLine[3].InnerLineWidth=150;
		borderLine[3].OuterLineWidth=19;
		borderLine[3].LineDistance=101;			
		xTextFramerops.setPropertyValue("LeftBorder", borderLine[0]);
		xTextFramerops.setPropertyValue("RightBorder", borderLine[1]);
		xTextFramerops.setPropertyValue("TopBorder", borderLine[2]);
		xTextFramerops.setPropertyValue("BottomBorder", borderLine[3]);
		//reopen the document 
		XTextDocument assertDocument_odt=(XTextDocument)UnoRuntime.queryInterface(XTextDocument.class, SWUtil.saveTo_Override_reload(xTextDocument,"writer8", Testspace.getPath("output/test.odt"),app));
		XTextFramesSupplier xTFS_odt = (XTextFramesSupplier) UnoRuntime.queryInterface(XTextFramesSupplier.class, assertDocument_odt);
		XNameAccess xTextFrames_odt = xTFS_odt.getTextFrames();
		XPropertySet xFrameProps_Assert_odt = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextFrames_odt.getByName("Frame1"));
		BorderLine LeftBorder_Assert_odt=(BorderLine) UnoRuntime.queryInterface(BorderLine.class, xFrameProps_Assert_odt.getPropertyValue("LeftBorder"));
		assertEquals("assert topline color as setting",0x00FF0000,LeftBorder_Assert_odt.Color);
		assertEquals("assert topline innerline width as setting",101,LeftBorder_Assert_odt.InnerLineWidth);
		assertEquals("assert topline outerlinewidth as setting",19,LeftBorder_Assert_odt.OuterLineWidth);
		assertEquals("assert topline linedistance as setting",101,LeftBorder_Assert_odt.LineDistance);
		BorderLine RightBorder_Assert_odt=(BorderLine) UnoRuntime.queryInterface(BorderLine.class, xFrameProps_Assert_odt.getPropertyValue("RightBorder"));
		assertEquals("assert bottomline color as setting",0x00FFFF00,RightBorder_Assert_odt.Color);
		assertEquals("assert bottomline innerline width as setting",101,RightBorder_Assert_odt.InnerLineWidth);
		assertEquals("assert bottomline outerlinewidth as setting",19,RightBorder_Assert_odt.OuterLineWidth);
		assertEquals("assert bottomline linedistance as setting",101,RightBorder_Assert_odt.LineDistance);
		BorderLine TopBorder_Assert_odt=(BorderLine) UnoRuntime.queryInterface(BorderLine.class, xFrameProps_Assert_odt.getPropertyValue("TopBorder"));
		assertEquals("assert leftline color as setting",0x0000FF00,TopBorder_Assert_odt.Color);
		assertEquals("assert leftline innerline width as setting",150,TopBorder_Assert_odt.InnerLineWidth);
		assertEquals("assert leftline outerlinewidth as setting",19,TopBorder_Assert_odt.OuterLineWidth);
		assertEquals("assert leftline linedistance as setting",101,TopBorder_Assert_odt.LineDistance);	
		BorderLine BottomBorder_Assert_odt=(BorderLine) UnoRuntime.queryInterface(BorderLine.class, xFrameProps_Assert_odt.getPropertyValue("BottomBorder"));
		assertEquals("assert rightline color as setting",0x0000FF00,BottomBorder_Assert_odt.Color);
		assertEquals("assert rightline linedistance as setting",101,BottomBorder_Assert_odt.LineDistance);		
		assertEquals("assert rightline innerline width as setting",150,BottomBorder_Assert_odt.InnerLineWidth);
		assertEquals("assert rightline outerlinewidth as setting",19,BottomBorder_Assert_odt.OuterLineWidth);	
	}
}
