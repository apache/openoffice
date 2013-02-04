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
package fvt.uno.sd.graphic;
import static org.junit.Assert.*;
import static testlib.uno.GraphicUtil.getGraphicsOfPage;
import static testlib.uno.GraphicUtil.getSizePixelOfGraphicFile;
import static testlib.uno.GraphicUtil.insertGraphic;
import static testlib.uno.SDUtil.saveFileAs;

import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;
import org.openoffice.test.uno.UnoApp;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;

import testlib.uno.SDUtil;

import com.sun.star.awt.Point;
import com.sun.star.awt.Size;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XShape;
import com.sun.star.lang.XComponent;
import com.sun.star.uno.UnoRuntime;

@RunWith(Parameterized.class)
public class GraphicPro_Size {

	private static final UnoApp app = new UnoApp();

	private XComponent m_xSDComponent = null;
	private XDrawPage m_xCurrentPage=null;
	private Size m_Size=null;
	private Size m_ExpectedSize=null;
	private String m_fileType = null;

	public GraphicPro_Size(Size size, String fileType, Size expectedSize){
		m_Size = size;
		m_fileType = fileType;
		m_ExpectedSize = expectedSize;
	}
	@Parameters
    public static Collection<Object[]> data() {     
        Size sMinimum = new Size(1,1); //minimum in UI
        Size sNormal = new Size(2000,2000);
        Size sMaximum = new Size(52448,28852); //maximum in UI
        
    	return Arrays.asList(new Object[][]{        		
        		{sMinimum, "odp", sMinimum},
        		{sMinimum, "ppt", sMinimum},
        		{sNormal, "odp", sNormal},
        		{sNormal, "ppt", sNormal},
        		{sMaximum, "odp", sMaximum},
        		{sMaximum, "ppt", sMaximum}        		       		
        });
    }
    
	@Before
	public void setUpDocument() throws Exception {
		m_xSDComponent = (XComponent) UnoRuntime.queryInterface(
				XComponent.class, app.newDocument("simpress"));		
		Object drawPage = SDUtil.getPageByIndex(m_xSDComponent, 0);		        
		m_xCurrentPage = (XDrawPage)UnoRuntime.queryInterface(XDrawPage.class, drawPage);
		
		String graphicURL = FileUtil.getUrl(Testspace.prepareData("uno/sd/36.gif"));		
		Size orgSize = getSizePixelOfGraphicFile(app,graphicURL);
	    Size newSize = new Size(orgSize.Width*2645/100, orgSize.Height*2645/100);
		insertGraphic(m_xSDComponent, m_xCurrentPage, graphicURL, newSize, new Point(5000, 5000));
	}

	@After
	public void tearDownDocument() {
		app.closeDocument(m_xSDComponent);

	}

	@BeforeClass
	public static void setUpConnection() throws Exception {
		app.start();
	}

	@AfterClass
	public static void tearDownConnection() throws InterruptedException,
			Exception {
		app.close();
		//remove the temp file
		FileUtil.deleteFile(Testspace.getPath("temp"));
	}

	private XDrawPage load(String filePath) throws Exception{
		m_xSDComponent = (XComponent) UnoRuntime.queryInterface(XComponent.class, 
				app.loadDocument(filePath));		
		Object drawPage = SDUtil.getPageByIndex(m_xSDComponent, 0);	      
		return (XDrawPage)UnoRuntime.queryInterface(XDrawPage.class, drawPage);
	}
	
	@Test
	public void testGraphicSize() throws Exception{
		String fileName = "GraphicPro_Size";		
		String filePath = Testspace.getPath("temp/"+fileName+"."+m_fileType);
		Object[] graphics = getGraphicsOfPage(m_xCurrentPage);
		Object oGraphic = graphics[0];
		XShape xGraphicShape = (XShape)UnoRuntime.queryInterface(XShape.class, oGraphic);
		
		xGraphicShape.setSize(m_Size);
		
		saveFileAs(m_xSDComponent, fileName, m_fileType);
		app.closeDocument(m_xSDComponent);
		
		XDrawPage CurrentPage = load(filePath);
		Object oGraphic2 = getGraphicsOfPage(CurrentPage)[0];
		XShape xGraphicShape2 = (XShape)UnoRuntime.queryInterface(XShape.class, oGraphic2);
		
		assertEquals("Height of graphic error", m_ExpectedSize.Height, xGraphicShape2.getSize().Height, 3);
		assertEquals("Width of graphic error", m_ExpectedSize.Width, xGraphicShape2.getSize().Width, 3);
	}
}
