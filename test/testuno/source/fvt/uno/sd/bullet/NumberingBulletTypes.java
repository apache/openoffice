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
import static testlib.uno.GraphicUtil.getUniqueIDOfGraphicFile;
import static testlib.uno.PageUtil.getDrawPageByIndex;
import static testlib.uno.ShapeUtil.addPortion;
import static testlib.uno.ShapeUtil.getPortion;

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

import com.sun.star.awt.Size;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XIndexReplace;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XShape;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XComponent;
import com.sun.star.style.NumberingType;
import com.sun.star.uno.UnoRuntime;

/**
 * @author LouQL
 *
 */
@RunWith(Parameterized.class)
public class NumberingBulletTypes {

	private static final UnoApp app = new UnoApp();

	private XComponent m_xSDComponent = null;
	private String m_filePath = null;
	private XPropertySet m_xtextProps = null;
	private short m_numberingType = 0;
	private short m_expectType=0;

	@Before
	public void setUpDocument() throws Exception {
		m_filePath = Testspace.getPath("temp/NumberingBulletTypes.odp");
		if (FileUtil.fileExists(m_filePath)) {//load
			m_xtextProps = load();
		} else {//new
			m_xSDComponent = (XComponent) UnoRuntime.queryInterface(
					XComponent.class, app.newDocument("simpress"));
			Object firstPage = getDrawPageByIndex(m_xSDComponent, 0);			
			Object secondTextBox = SDUtil.getShapeOfPageByIndex(firstPage, 1);
			XShape xsecondTextBox = (XShape)UnoRuntime.queryInterface(XShape.class, secondTextBox);
			m_xtextProps = addPortion(xsecondTextBox, "Numbering bullets", false);
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

	@Parameters
    public static Collection<Short[]> data(){
    	Short[][] numberingTypes = new Short[][]{
    			{NumberingType.CHARS_UPPER_LETTER, NumberingType.CHARS_UPPER_LETTER},
    			{NumberingType.CHARS_LOWER_LETTER, NumberingType.CHARS_LOWER_LETTER},
    			{NumberingType.ROMAN_UPPER, NumberingType.ROMAN_UPPER},
    			{NumberingType.ROMAN_LOWER, NumberingType.ROMAN_LOWER},
    			{NumberingType.ARABIC, NumberingType.ARABIC},
    			//bug: 120752
//    			{NumberingType.CIRCLE_NUMBER, NumberingType.CIRCLE_NUMBER},
    			{NumberingType.NUMBER_NONE, NumberingType.NUMBER_NONE}};
    	return Arrays.asList(numberingTypes);
    }
    
    public NumberingBulletTypes(short input, short expect){
    	m_numberingType = input;
    	m_expectType = expect;
    }
    
    /*NumberingType: specifies the type of numbering
	 * GUI entry:Numbering and Bullet dialog->Customize->Numbering
	 * */
	@Test
	public void testNumberingTypes() throws Exception {
		Object numberingrules = m_xtextProps.getPropertyValue("NumberingRules");
		
		XIndexReplace xReplace = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules); 		
		
		PropertyValue[] props = new PropertyValue[1];
		props[0] = new PropertyValue();
	    props[0].Name = "NumberingType";
	    props[0].Value = m_numberingType;    
	        
	    xReplace.replaceByIndex(0, props);
	    
	    m_xtextProps.setPropertyValue("NumberingRules", numberingrules);
		  //set numbering level to 0			
		m_xtextProps.setPropertyValue("NumberingLevel", new Short((short)0));
		
		app.saveDocument(m_xSDComponent, m_filePath);
		app.closeDocument(m_xSDComponent);
//		m_xSDComponent.dispose();
		//reopen
		m_xtextProps = load();
				    
		Object numberingrules2 = m_xtextProps.getPropertyValue("NumberingRules");
					
		XIndexReplace xReplace2 = (XIndexReplace) UnoRuntime.queryInterface(
	             XIndexReplace.class, numberingrules2);
		
		PropertyValue[] proValues2 = (PropertyValue[])xReplace2.getByIndex(0); 
		
		assertEquals("NumberingType should be"+m_numberingType, m_expectType, proValues2[0].Value);
	}
}
