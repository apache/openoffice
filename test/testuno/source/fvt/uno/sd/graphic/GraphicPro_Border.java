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
import org.junit.Ignore;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;
import org.openoffice.test.uno.UnoApp;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;

import testlib.uno.SDUtil;
import testlib.uno.TestUtil;

import com.sun.star.awt.Point;
import com.sun.star.awt.Size;
import com.sun.star.beans.XPropertySet;
import com.sun.star.drawing.LineStyle;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XShape;
import com.sun.star.lang.XComponent;
import com.sun.star.uno.UnoRuntime;
@RunWith(Parameterized.class)
public class GraphicPro_Border {

	private static final UnoApp app = new UnoApp();

	private XComponent m_xSDComponent = null;
	private XDrawPage m_xCurrentPage=null;
	
	private LineStyle m_LineStyle;
	private String m_LineDashName = null;
	private int m_LineColor = 0;
	private LineStyle m_expLineStyle;
	private String m_expLineDashName = null;
	private int m_expLineColor = 0;	
	
	public GraphicPro_Border(LineStyle lineStyle, String lineDashName, int lineColor, LineStyle expLineStyle, String expLineDashName, int expLineColor){
		m_LineStyle = lineStyle;
		m_LineDashName = lineDashName;
		m_LineColor = lineColor;
		m_expLineStyle = expLineStyle;
		m_expLineDashName = expLineDashName;
		m_expLineColor = expLineColor;
	}
	
	@Parameters
	public static Collection<Object[]> data() throws Exception {
		int[] colorList = TestUtil.randColorList(13);

		return Arrays.asList(new Object[][] {
			//{lineStyle, LineDashName, line Color, file type, expLineStyle, expLineDashName, expLineColor} 
			{LineStyle.NONE, "Invisible", colorList[0], LineStyle.NONE, "Invisible", colorList[0]},   
			{LineStyle.SOLID,"Continuous", colorList[1], LineStyle.SOLID,"Continuous", colorList[1]},
			{LineStyle.DASH,"Ultrafine Dashed", colorList[2], LineStyle.DASH,"Ultrafine Dashed", colorList[2]},
			{LineStyle.DASH,"Fine Dashed", colorList[3], LineStyle.DASH,"Fine Dashed", colorList[3]},
			{LineStyle.DASH,"Ultrafine 2 Dots 3 Dashes", colorList[4], LineStyle.DASH,"Ultrafine 2 Dots 3 Dashes", colorList[4]},
			{LineStyle.DASH,"Fine Dotted", colorList[5],  LineStyle.DASH,"Fine Dotted", colorList[5]},
			{LineStyle.DASH,"Line with Fine Dots", colorList[6], LineStyle.DASH,"Line with Fine Dots", colorList[6]},
			{LineStyle.DASH,"Fine Dashed (var)", colorList[7], LineStyle.DASH,"Fine Dashed (var)", colorList[7]},
			{LineStyle.DASH,"3 Dashes 3 Dots (var)", colorList[8], LineStyle.DASH,"3 Dashes 3 Dots (var)", colorList[8]},
			{LineStyle.DASH,"Ultrafine Dotted (var)", colorList[9], LineStyle.DASH,"Ultrafine Dotted (var)", colorList[9]},
			{LineStyle.DASH,"Line Style 9", colorList[10], LineStyle.DASH,"Line Style 9", colorList[10]},
			{LineStyle.DASH,"2 Dots 1 Dash", colorList[11], LineStyle.DASH,"2 Dots 1 Dash", colorList[11]},
			{LineStyle.DASH,"Dashed (var)", colorList[12], LineStyle.DASH,"Dashed (var)", colorList[12]},
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
	public void testGraphicBorder_ODP() throws Exception {
		String fileName = "GraphicPro_LineColor";		
		String fileType = ".odp";
		String filePath = Testspace.getPath("temp/"+fileName+"."+fileType);
		Object[] graphics = getGraphicsOfPage(m_xCurrentPage);
		Object oGraphic = graphics[0];
		XShape xGraphicShape = (XShape)UnoRuntime.queryInterface(XShape.class, oGraphic);
		
		XPropertySet xPropSet = (XPropertySet)UnoRuntime.queryInterface(
			     XPropertySet.class, xGraphicShape );

		xPropSet.setPropertyValue( "LineStyle", this.m_LineStyle);
		if(m_LineStyle == LineStyle.DASH)
		{
			xPropSet.setPropertyValue( "LineDashName", this.m_LineDashName);
		}
		if(m_LineStyle != LineStyle.NONE)
			xPropSet.setPropertyValue( "LineColor", this.m_LineColor);
		
		saveFileAs(m_xSDComponent, fileName, fileType);
		app.closeDocument(m_xSDComponent);
		
		XDrawPage CurrentPage = load(filePath);
		Object oGraphic2 = getGraphicsOfPage(CurrentPage)[0];
		XShape xGraphicShape2 = (XShape)UnoRuntime.queryInterface(XShape.class, oGraphic2);
		XPropertySet xPropSet2 = (XPropertySet)UnoRuntime.queryInterface(
			     XPropertySet.class, xGraphicShape2 );
		
		assertEquals("line style changed", this.m_expLineStyle, xPropSet2.getPropertyValue("LineStyle"));
		if(m_LineStyle != LineStyle.NONE)
			assertEquals("line color changed", this.m_expLineColor, xPropSet2.getPropertyValue("LineColor"));
		if(m_LineStyle == LineStyle.DASH)
			assertEquals("line DashName changed", this.m_expLineDashName, xPropSet2.getPropertyValue("LineDashName"));
	}
	
	@Ignore("Bug #120982 - [From Symphony]graphic border missing after open .ppt file in Aoo")
	@Test
	public void testGraphicBorder_PPT() throws Exception {
		String fileName = "GraphicPro_LineColor";		
		String fileType = ".ppt";
		String filePath = Testspace.getPath("temp/"+fileName+"."+fileType);
		Object[] graphics = getGraphicsOfPage(m_xCurrentPage);
		Object oGraphic = graphics[0];
		XShape xGraphicShape = (XShape)UnoRuntime.queryInterface(XShape.class, oGraphic);
		
		XPropertySet xPropSet = (XPropertySet)UnoRuntime.queryInterface(
			     XPropertySet.class, xGraphicShape );

		xPropSet.setPropertyValue( "LineStyle", this.m_LineStyle);
		if(m_LineStyle == LineStyle.DASH)
		{
			xPropSet.setPropertyValue( "LineDashName", this.m_LineDashName);
		}
		if(m_LineStyle != LineStyle.NONE)
			xPropSet.setPropertyValue( "LineColor", this.m_LineColor);
		
		saveFileAs(m_xSDComponent, fileName, fileType);
		app.closeDocument(m_xSDComponent);
		
		XDrawPage CurrentPage = load(filePath);
		Object oGraphic2 = getGraphicsOfPage(CurrentPage)[0];
		XShape xGraphicShape2 = (XShape)UnoRuntime.queryInterface(XShape.class, oGraphic2);
		XPropertySet xPropSet2 = (XPropertySet)UnoRuntime.queryInterface(
			     XPropertySet.class, xGraphicShape2 );
		
		assertEquals("line style changed", this.m_expLineStyle, xPropSet2.getPropertyValue("LineStyle"));
		if(m_LineStyle != LineStyle.NONE)
			assertEquals("line color changed", this.m_expLineColor, xPropSet2.getPropertyValue("LineColor"));
		if(m_LineStyle == LineStyle.DASH)
			assertEquals("line DashName changed", this.m_expLineDashName, xPropSet2.getPropertyValue("LineDashName"));
	}
	
}
