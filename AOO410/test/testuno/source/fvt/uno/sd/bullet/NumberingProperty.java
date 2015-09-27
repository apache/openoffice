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
/**
 * 
 */
package fvt.uno.sd.bullet;
import static org.junit.Assert.*;
import static testlib.uno.PageUtil.getDrawPageByIndex;
import static testlib.uno.ShapeUtil.addPortion;
import static testlib.uno.ShapeUtil.getPortion;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;
import org.openoffice.test.uno.UnoApp;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;

import testlib.uno.SDUtil;

import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XIndexReplace;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XShape;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XComponent;
import com.sun.star.style.NumberingType;
import com.sun.star.text.HoriOrientation;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.UnoRuntime;

/**
 * @author LouQL
 *
 */
public class NumberingProperty {

	private static final UnoApp app = new UnoApp();

	private XComponent m_xSDComponent = null;
	private String m_filePath = null;
	private XPropertySet m_xtextProps = null;

	@Before
	public void setUpDocument() throws Exception {
		m_filePath = Testspace.getPath("temp/NumberingProperty.odp");
//		m_filePath = "F:/aa.odp";
		if (FileUtil.fileExists(m_filePath)) {//load
			m_xtextProps = load();
		} else {//new
			m_xSDComponent = (XComponent) UnoRuntime.queryInterface(
					XComponent.class, app.newDocument("simpress"));
			Object firstPage = getDrawPageByIndex(m_xSDComponent, 0);			
			Object secondTextBox = SDUtil.getShapeOfPageByIndex(firstPage, 1);
			XShape xsecondTextBox = (XShape)UnoRuntime.queryInterface(XShape.class, secondTextBox);
			m_xtextProps = addPortion(xsecondTextBox, 
					"test the property of Numbering bullets. There are two lines in this test",
					false);
			setNumberingType(m_xtextProps, NumberingType.ARABIC, (short)0);
		}
	}

	private XPropertySet load() throws Exception{
		m_xSDComponent = (XComponent) UnoRuntime.queryInterface(XComponent.class, 
				app.loadDocument(m_filePath));
		Object firstPage = getDrawPageByIndex(m_xSDComponent, 0);
		XDrawPage firstpage = getDrawPageByIndex(m_xSDComponent, 0);
		Object secondTextBox = SDUtil.getShapeOfPageByIndex(firstPage, 1);
		XShape xsecondTextBox = (XShape)UnoRuntime.queryInterface(XShape.class, secondTextBox);		
		return getPortion(xsecondTextBox, 0);
	}
	
	@After
	public void tearDownDocument() {
		app.closeDocument(m_xSDComponent);
		//remove the temp file
		FileUtil.deleteFile(Testspace.getPath("temp"));
	}

	@BeforeClass
	public static void setUpConnection() throws Exception {
		app.start();
	}

	@AfterClass
	public static void tearDownConnection() throws InterruptedException,
			Exception {
		app.close();
		
	}

	/*NumberingType: specifies the type of numbering
	 * GUI entry:Numbering and Bullet dialog->Customize->Numbering
	 * */
	private void setNumberingType(XPropertySet textPros, short numberingType, short level) throws Exception{
		Object numberingRules = textPros.getPropertyValue("NumberingRules");
		XIndexReplace xReplace = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingRules); 		
		
		PropertyValue[] props = new PropertyValue[1];
		props[0] = new PropertyValue();
	    props[0].Name = "NumberingType";
	    props[0].Value = numberingType;    
	        
	    xReplace.replaceByIndex(level, props);
	    
	    m_xtextProps.setPropertyValue("NumberingRules", numberingRules);
		  //set numbering level to 0			
		m_xtextProps.setPropertyValue("NumberingLevel", level);
	}
	
		
	
	@Test
	/*
	 * Prefix: the prefix of the numbering symbol
	 * GUI entry: Numbering and Bullet dialog->customize->Before
	 * */
	public void testPrefix() throws Exception {
		String prefix = "Prefix";
		Object numberingrules = m_xtextProps.getPropertyValue("NumberingRules");
		
		XIndexReplace xReplace = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules); 		
		
		PropertyValue[] props = new PropertyValue[1];
		props[0] = new PropertyValue();
	    props[0].Name = "Prefix";
	    props[0].Value = prefix;    
	        
	    xReplace.replaceByIndex(0, props);
	    
	    m_xtextProps.setPropertyValue("NumberingRules", numberingrules);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		//reopen
		m_xtextProps = load();
				    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
					
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules2);
		
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0); 
		for(int i=0;i<proValues2.length;i++)
		{
			if(proValues2[i].Name.equals("Prefix"))
			{
				assertEquals("Prefix should be "+prefix, prefix, proValues2[i].Value);
				break;
			}
		}		
		
	}
	
	@Test
	/*
	 * Suffix: the suffix of the numbering symbol
	 * GUI entry: Numbering and Bullet dialog->customize->After
	 * */
	public void testSuffix() throws Exception {
		String suffix = "--";
		Object numberingrules = m_xtextProps.getPropertyValue("NumberingRules");
		
		XIndexReplace xReplace = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules); 		
		
		PropertyValue[] props = new PropertyValue[1];
		props[0] = new PropertyValue();
	    props[0].Name = "Suffix";
	    props[0].Value = suffix;    
	        
	    xReplace.replaceByIndex(0, props);
	    
	    m_xtextProps.setPropertyValue("NumberingRules", numberingrules);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		//reopen
		m_xtextProps = load();
				    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
					
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules2);
		
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0); 
		for(int i=0;i<proValues2.length;i++)
		{
			if(proValues2[i].Name.equals("Suffix"))
			{
				assertEquals("Suffix should be "+suffix, suffix, proValues2[i].Value);
				break;
			}
		}
		
		
	}
	
	@Test
	/*
	 * StartWith: specifies the start value for the numbering.
	 * GUI entry: Numbering and Bullet dialog->customize->Start At
	 * */
	public void testStartWith() throws Exception {
		short startWith = 5;
		Object numberingrules = m_xtextProps.getPropertyValue("NumberingRules");
		
		XIndexReplace xReplace = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules); 		
		
		PropertyValue[] props = new PropertyValue[1];
		props[0] = new PropertyValue();
	    props[0].Name = "StartWith";
	    props[0].Value = startWith;    
	        
	    xReplace.replaceByIndex(0, props);
	    
	    m_xtextProps.setPropertyValue("NumberingRules", numberingrules);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		//reopen
		m_xtextProps = load();
				    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
					
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules2);
		
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0); 

		String sStartWith = Short.toString(startWith);
		for(int i=0;i<proValues2.length;i++)
		{
			if(proValues2[i].Name.equals("StartWith"))
			{
				assertEquals("StartWith should be "+sStartWith, startWith, proValues2[i].Value);
				break;
			}
		}
		
	}

	
	
	
	@Test
	/*FirstLineOffset: specifies the offset between the beginning of the first line 
	 * and the beginning of the following lines of the paragraph.   
	*GUI entry: paragraph dialog->Position tab->Width of numbering
	*/
	public void testFirstLineOffset() throws Exception {
		Integer firstLineOffset = -4500;
		Object numberingrules = m_xtextProps.getPropertyValue("NumberingRules");
		XIndexAccess xNum = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,
				numberingrules); 
		XIndexReplace xReplace = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, xNum); 		
		
		PropertyValue[] props = new PropertyValue[1];
		props[0] = new PropertyValue();
	    props[0].Name = "FirstLineOffset";
	    props[0].Value = firstLineOffset;    
	        
	    xReplace.replaceByIndex(0, props);
	    
	    m_xtextProps.setPropertyValue("NumberingRules", numberingrules);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		//reopen
		m_xtextProps = load();
				    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
					
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules2);
		
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0); 

		for(int i=0;i<proValues2.length;i++)
		{
			if(proValues2[i].Name.equals("FirstLineOffset"))
			{
				assertEquals("SymbolTextDistance should be "+Integer.toString(firstLineOffset), 
						firstLineOffset, proValues2[i].Value);
				break;
			}
		}
		
	}
	
	@Test
	/*Leftmargin: specifies the left margin of the numbering  
	 * GUI entry: Numbering and Bullet dialog->Position->Indent = Leftmargin+FirstLineOffset
	*/
	public void testLeftMargin() throws Exception {
		Integer leftMargin = 2000;
		Object numberingrules = m_xtextProps.getPropertyValue("NumberingRules");
		XIndexAccess xNum = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,
				numberingrules); 
		XIndexReplace xReplace = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, xNum); 		
		
		PropertyValue[] props = new PropertyValue[1];
		props[0] = new PropertyValue();
	    props[0].Name = "LeftMargin";
	    props[0].Value = leftMargin;    
	        
	    xReplace.replaceByIndex(0, props);
	    
	    m_xtextProps.setPropertyValue("NumberingRules", numberingrules);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		//reopen
		m_xtextProps = load();
				    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
					
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules2);
		
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0); 

		for(int i=0;i<proValues2.length;i++)
		{
			if(proValues2[i].Name.equals("LeftMargin"))
			{
				assertEquals("Leftmargin should be "+Integer.toString(leftMargin), leftMargin, proValues2[i].Value);
				break;
			}
		}
		
	}
	@Ignore
	@Test
	/*symbolTextDistance: specifies the distance between the numbering symbol and the text of the paragraph.  
	*GUI entry: ??
	*This property cannot be set, it's always the same value as FirstLineOffset
	*/
	public void testSymbolTextDistance() throws Exception {
		Integer symbolTextDistance = -2000;
		Object numberingrules = m_xtextProps.getPropertyValue("NumberingRules");
		XIndexAccess xNum = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class,
				numberingrules); 
		XIndexReplace xReplace = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, xNum); 		
		
		PropertyValue[] props = new PropertyValue[1];
		props[0] = new PropertyValue();
	    props[0].Name = "SymbolTextDistance";
	    props[0].Value = symbolTextDistance;    
	        
	    xReplace.replaceByIndex(0, props);
//	    PropertyValue[] proValues = (PropertyValue[])xReplace.getByIndex(0); 
//	    for(int m=0;m<proValues.length;m++)
//	    {
//	    	System.out.println(proValues[m].Name+"="+proValues[m].Value);
//	    }
	    
	    m_xtextProps.setPropertyValue("NumberingRules", numberingrules);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		//reopen
		m_xtextProps = load();
				    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
					
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules2);
		
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0); 

		for(int i=0;i<proValues2.length;i++)
		{
			if(proValues2[i].Name.equals("SymbolTextDistance"))
			{
				assertEquals("SymbolTextDistance should be "+Integer.toString(symbolTextDistance), 
						symbolTextDistance, proValues2[i].Value);
				break;
			}
		}
		
	}
	
	/*
	 * Ajust: adjusts the numbering (HoriOrientation_LEFT/RIGHT/CENTER)
	 * GUI entry: Numbering and Bullet dialog->Position->Numbering alignment
	 * */
	@Test
	public void testAjustRight() throws Exception {
		Short ajust = HoriOrientation.RIGHT;
		setAjust(ajust);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		//reopen
		m_xtextProps = load();
				    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
					
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules2);
		
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0); 
		
		for(int i=0;i<proValues2.length;i++)
		{
			if(proValues2[i].Name.equals("Adjust"))
			{
				assertEquals("Ajust should be HoriOrientation.RIGHT", HoriOrientation.RIGHT, AnyConverter.toUnsignedShort(proValues2[i].Value));
				break;
			}
		}		
	}
	private void setAjust(Short ajust) throws Exception{
		Object numberingrules = m_xtextProps.getPropertyValue("NumberingRules");
		
		XIndexReplace xReplace = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules); 		
		
		PropertyValue[] props = new PropertyValue[1];
		props[0] = new PropertyValue();
	    props[0].Name = "Adjust";
	    props[0].Value = ajust;    
	        
	    xReplace.replaceByIndex(0, props);
	    
	    m_xtextProps.setPropertyValue("NumberingRules", numberingrules);
	}
	
	/*
	 * Ajust: adjusts the numbering (HoriOrientation_LEFT/RIGHT/CENTER)
	 * GUI entry: Numbering and Bullet dialog->Position->Numbering alignment
	 * */
	@Test
	public void testAjustCenter() throws Exception {
		Short ajust = HoriOrientation.CENTER;
		setAjust(ajust);		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		//reopen
		m_xtextProps = load();
				    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
					
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules2);
		
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0); 
		String sAdjust = Short.toString(ajust);
		for(int i=0;i<proValues2.length;i++)
		{
			if(proValues2[i].Name.equals("Adjust"))
			{
				assertEquals("Ajust should be HoriOrientation.CENTER", HoriOrientation.CENTER, AnyConverter.toUnsignedShort(proValues2[i].Value));
				break;
			}
		}	
		
	}
	
	/*
	 * Ajust: adjusts the numbering (HoriOrientation_LEFT/RIGHT/CENTER)
	 * GUI entry: Numbering and Bullet dialog->Position->Numbering alignment
	 * */
	@Test
	public void testAjustLeft() throws Exception {
		Short ajust = HoriOrientation.LEFT;
		setAjust(ajust);		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		//reopen
		m_xtextProps = load();
				    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
					
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules2);
		
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0); 
		String sAdjust = Short.toString(ajust);
		for(int i=0;i<proValues2.length;i++)
		{
			if(proValues2[i].Name.equals("Adjust"))
			{
				assertEquals("Ajust should be HoriOrientation.LEFT", 
						HoriOrientation.LEFT, AnyConverter.toUnsignedShort(proValues2[i].Value));
				break;
			}
		}		
	}

	@Test
	/*BulletColor: contains the color for the symbol. 
	 * In SDK docs it says this is only valid if the numbering type is NumberingType::CHAR_SPECIAL. 
	 * But infact it also valid here
	*GUI entry: Bullet and Numbering dialog->Customize->Color
	*/
	public void testSymbolColor() throws Exception {
		Object numberingrules = m_xtextProps.getPropertyValue("NumberingRules");
		
		XIndexReplace xReplace = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules); 		
		
		PropertyValue[] props = new PropertyValue[1];
		props[0] = new PropertyValue();
	    props[0].Name = "BulletColor";
	    props[0].Value = new Integer(255);    
	        
	    xReplace.replaceByIndex(0, props);
	    
	    m_xtextProps.setPropertyValue("NumberingRules", numberingrules);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		//reopen
		m_xtextProps = load();
				    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
					
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules2);
		
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0); 

		for(int i=0;i<proValues2.length;i++)
		{
			
			if(proValues2[i].Name.equals("BulletColor"))
			{
				assertEquals("BulletColor should be Blue", 
						new Integer(255), proValues2[i].Value);
				break;
			}
		}
		
	}
	
	@Test
	/*BulletRelSize: contains the size of the symbol relative to the high of the paragraph
	 * In SDK docs it says this is only valid if the numbering type is NumberingType::CHAR_SPECIAL. 
	 * But in fact it also valid here
	*GUI entry: Bullet and Numbering dialog->Customize->Relative Size
	*/
	public void testSymbolSize() throws Exception {
		Object numberingrules = m_xtextProps.getPropertyValue("NumberingRules");
		
		XIndexReplace xReplace = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules); 		
		
		PropertyValue[] props = new PropertyValue[1];
		props[0] = new PropertyValue();
	    props[0].Name = "BulletRelSize";
	    props[0].Value = new Short((short)100);    
	        
	    xReplace.replaceByIndex(0, props);
	    
	    m_xtextProps.setPropertyValue("NumberingRules", numberingrules);
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
		//reopen
		m_xtextProps = load();
				    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
					
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules2);
		
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0); 

		for(int i=0;i<proValues2.length;i++)
		{
			if(proValues2[i].Name.equals("BulletRelSize"))
			{
				assertEquals("BulletRelSize should be 100%", 
						new Short((short)100), proValues2[i].Value);
				break;
			}
		}
		
	}

}
