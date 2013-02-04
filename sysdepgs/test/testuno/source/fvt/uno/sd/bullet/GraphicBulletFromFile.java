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
/*
 * Select a external picture from a file as graphic bullet
 * */
package fvt.uno.sd.bullet;
import static org.junit.Assert.*;
import static org.openoffice.test.common.Testspace.prepareData;
import static testlib.uno.PageUtil.getDrawPageByIndex;
import static testlib.uno.ShapeUtil.addPortion;
import static testlib.uno.ShapeUtil.getPortion;
import static testlib.uno.GraphicUtil.getUniqueIDOfGraphicFile;

import java.io.File;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.openoffice.test.uno.UnoApp;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;

import testlib.uno.SDUtil;

import com.sun.star.awt.Size;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XIndexReplace;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XShape;
import com.sun.star.lang.XComponent;
import com.sun.star.style.NumberingType;
import com.sun.star.uno.UnoRuntime;

public class GraphicBulletFromFile {

	private static final UnoApp app = new UnoApp();

	private XComponent m_xSDComponent = null;
	private String m_filePath = null;
	private XPropertySet m_xtextProps = null;	
	private String m_GraphicPath = null;

	@Before
	public void setUpDocument() throws Exception {
		m_filePath = Testspace.getPath("temp/GraphicBulletFromFile.odp");
		String abslotePath = prepareData("uno/sd/36.gif");	
		m_GraphicPath = FileUtil.getUrl(new File(abslotePath));	
//		m_GraphicPath = "file:///F:/work/36.gif";
		if (FileUtil.fileExists(m_filePath)) {//load
			m_xtextProps = load();	  
		} else {//new
			m_xSDComponent = (XComponent) UnoRuntime.queryInterface(
					XComponent.class, app.newDocument("simpress"));
			Object firstPage = getDrawPageByIndex(m_xSDComponent, 0);			
			Object firstTextBox = SDUtil.getShapeOfPageByIndex(firstPage, 0);
			XShape xfirstTextBox = (XShape)UnoRuntime.queryInterface(XShape.class, firstTextBox);
			m_xtextProps = addPortion(xfirstTextBox, "test Graphic Bullet From a File", false);
		}
	}
	
	private XPropertySet load() throws Exception{
		m_xSDComponent = (XComponent) UnoRuntime.queryInterface(XComponent.class, 
				app.loadDocument(m_filePath));
		Object firstPage = getDrawPageByIndex(m_xSDComponent, 0);
		XDrawPage firstpage = getDrawPageByIndex(m_xSDComponent, 0);
		Object firstTextBox = SDUtil.getShapeOfPageByIndex(firstPage, 0);
		XShape xfirstTextBox = (XShape)UnoRuntime.queryInterface(XShape.class, firstTextBox);		
		return getPortion(xfirstTextBox, 0);
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

	@Test
	public void testGraphicBulletFromFile() throws Exception {				
				
		Object numberingrules = m_xtextProps.getPropertyValue("NumberingRules");
		
		XIndexReplace xReplace = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules); 		
		
		PropertyValue[] props = new PropertyValue[3];
		props[0] = new PropertyValue();
	    props[0].Name = "NumberingType";
	    props[0].Value = new Short(NumberingType.BITMAP );
	    
	    props[1] = new PropertyValue();
	    props[1].Name = "GraphicURL";
	    props[1].Value = "vnd.sun.star.GraphicObject:"+getUniqueIDOfGraphicFile(app, m_GraphicPath);
	    
	    props[2] = new PropertyValue();
	    props[2].Name = "GraphicSize";	    
	    props[2].Value = new Size(1000,1000);
	    
	    xReplace.replaceByIndex(0, props);
	    
	    m_xtextProps.setPropertyValue("NumberingRules", numberingrules);
		  //set numbering level to 0			
		m_xtextProps.setPropertyValue("NumberingLevel", new Short((short)0));

		    	    
		app.saveDocument(m_xSDComponent, m_filePath);
//			app.closeDocument(m_xSDComponent);
		m_xSDComponent.dispose();
			//reopen
		m_xtextProps = load();
					    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
						
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
		            XIndexReplace.class, numberingrules2);
			
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0);  
		assertEquals("NumberingType should be BITMAP", NumberingType.BITMAP, proValues2[0].Value);
		String uniqueID = getUniqueIDOfGraphicFile(app, m_GraphicPath);
		assertEquals("Graphic should be the one with uniqueID"+uniqueID, "vnd.sun.star.GraphicObject:"+uniqueID, proValues2[6].Value);

	}
}
