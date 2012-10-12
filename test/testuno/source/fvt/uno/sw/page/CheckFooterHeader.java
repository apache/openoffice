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
package fvt.uno.sw.page;

import static org.openoffice.test.common.Testspace.*;

import java.io.File;
import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.Ignore;
import org.junit.Assert;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import org.openoffice.test.common.FileUtil;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.SWUtil;
import com.sun.star.text.XTextDocument;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.lang.XComponent;
import com.sun.star.table.BorderLine;

/**
 * test footer/header's margins, spacing and height. 
 *
 */
@RunWith(Parameterized.class)
public class CheckFooterHeader {
	UnoApp unoApp = new UnoApp();
	XTextDocument textDocument = null;
	File temp = null;
	String tempFilePathODT = "";	
	String tempFilePathDOC = "";	
	
	private String m_onProperty = "";
	
	private String m_LeftMarginProperty = "";
	private String m_RightMarginProperty = "";
	private String m_BodyDistanceProperty = "";
	private String m_HeightProperty = "";
	
	private int m_LeftMargin = 0;
	private int m_RightMargin = 0;
	private int m_BodyDistance = 0;
	private int m_Height = 0;
	
	
	public CheckFooterHeader(String onProperty, String leftMarginProperty, String rightMarginProperty, String bodyDistanceProperty, String heightProperty,
			int leftMargin, int rightMargin, int bodyDistance, int height){
		m_onProperty = onProperty;
		m_LeftMarginProperty = leftMarginProperty;
		m_RightMarginProperty = rightMarginProperty;		
		m_BodyDistanceProperty = bodyDistanceProperty;
		m_HeightProperty = heightProperty;
		
		m_LeftMargin = leftMargin;
		m_RightMargin = rightMargin;
		m_BodyDistance = bodyDistance;
		m_BodyDistance = bodyDistance;
		m_Height = height;
	}
	
	@Parameters
    public static Collection<Object[]> data(){
    	Object[][] params = new Object[][]{
    			{"FooterIsOn", "FooterLeftMargin", "FooterRightMargin", "FooterBodyDistance","FooterHeight", 1000, 2000, 500,500},
    			{"FooterIsOn", "FooterLeftMargin", "FooterRightMargin", "FooterBodyDistance","FooterHeight", 3000, 1500, 300,400},
    			{"FooterIsOn", "FooterLeftMargin", "FooterRightMargin", "FooterBodyDistance","FooterHeight", 0, 0, 500,500},
    			{"FooterIsOn", "FooterLeftMargin", "FooterRightMargin", "FooterBodyDistance","FooterHeight", 600, 2000, 0,300},
    			{"HeaderIsOn", "HeaderLeftMargin", "HeaderRightMargin", "HeaderBodyDistance","HeaderHeight", 1000, 2000, 500,500},
    			{"HeaderIsOn", "HeaderLeftMargin", "HeaderRightMargin", "HeaderBodyDistance","HeaderHeight", 3000, 1500, 300,400},
    			{"HeaderIsOn", "HeaderLeftMargin", "HeaderRightMargin", "HeaderBodyDistance","HeaderHeight", 0, 0, 500,500},
    			{"HeaderIsOn", "HeaderLeftMargin", "HeaderRightMargin", "HeaderBodyDistance","HeaderHeight", 600, 2000, 0,300}
    			};
    	return Arrays.asList(params);
    }	
    
    /**
     * test header/footer's left margin, right margin, spacing and height.
     * @throws Exception
     */
    //@Ignore("#120796 - header/footer's margins are all set to 0 when export to DOC format")
    @Ignore("#120798 - header/footer's spacing is increased when export to DOC file")
	@Test
	public void testFooterHeader() throws Exception
	{		
		XComponent xComponent = unoApp.newDocument("swriter");
		//turn on header/footer		
		SWUtil.setDefaultPageStyleProperty(xComponent, m_onProperty, new Boolean(true));
		SWUtil.setDefaultPageStyleProperty(xComponent, m_LeftMarginProperty, Integer.valueOf(m_LeftMargin));
		SWUtil.setDefaultPageStyleProperty(xComponent, m_RightMarginProperty, Integer.valueOf(m_RightMargin));
		SWUtil.setDefaultPageStyleProperty(xComponent, m_BodyDistanceProperty, Integer.valueOf(m_BodyDistance));
		SWUtil.setDefaultPageStyleProperty(xComponent, m_HeightProperty, Integer.valueOf(m_Height));
		
		//save as ODT and reopen, get border
		unoApp.saveDocument(xComponent, tempFilePathODT);        
        unoApp.closeDocument(xComponent);
        xComponent = unoApp.loadDocument(tempFilePathODT);        
        
		int leftMargin = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, m_LeftMarginProperty)).intValue();
		int rightMargin = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, m_RightMarginProperty)).intValue();
		int bodyDistance = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, m_BodyDistanceProperty)).intValue();
		int height = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, m_HeightProperty)).intValue();
		
		this.compare("ODT", leftMargin, rightMargin, bodyDistance, height);
		
		//save as DOC and reopen, get properties        
	    SWUtil.saveAsDoc(xComponent, FileUtil.getUrl(tempFilePathDOC));
	    unoApp.closeDocument(xComponent);
	    xComponent = unoApp.loadDocument(tempFilePathDOC);	
	    leftMargin = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, m_LeftMarginProperty)).intValue();
		rightMargin = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, m_RightMarginProperty)).intValue();
		bodyDistance = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, m_BodyDistanceProperty)).intValue();
		height = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, m_HeightProperty)).intValue();
		
		this.compare("DOC", leftMargin, rightMargin, bodyDistance, height);
		
		unoApp.closeDocument(xComponent);
        
	}
	
	private void compare(String preDescription, int leftMargin, int rightMargin, int bodyDistance, int height){		
		Assert.assertEquals(preDescription + ":" + m_LeftMarginProperty,(double)m_LeftMargin, (double)leftMargin, 2);
		Assert.assertEquals(preDescription + ":" + m_RightMarginProperty,(double)m_RightMargin, (double)rightMargin, 2);
		Assert.assertEquals(preDescription + ":" + m_BodyDistanceProperty,(double)m_BodyDistance, (double)bodyDistance, 2);
		Assert.assertEquals(preDescription + ":" + m_HeightProperty,(double)m_Height, (double)height, 2);			
	}
	
	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		unoApp.start();
		
		FileUtil.deleteFile(getPath("temp"));
		temp = new File(getPath("temp"));
		temp.mkdirs();		
		
		tempFilePathODT = temp + "/tempFilePathODT.odt";
		tempFilePathDOC = temp + "/tempFilePathDOC.doc";		
	}

	@After
	public void tearDown() throws Exception {
		unoApp.close();
	}	
	

}
