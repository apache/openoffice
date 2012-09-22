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
package testcase.uno.sw.page;

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
 * test page's orientation, width, height, margins 
 *
 */
@RunWith(Parameterized.class)
public class CheckPage {
	UnoApp unoApp = new UnoApp();
	XTextDocument textDocument = null;
	File temp = null;
	String tempFilePathODT = "";	
	String tempFilePathDOC = "";	
	
	private String isLandscapeProperty = "IsLandscape";
	private String widthProperty = "Width";
	private String heightProperty = "Height";
	private String topMarginProperty = "TopMargin";
	private String bottomMarginProperty = "BottomMargin";
	private String leftMarginProperty = "LeftMargin";
	private String rightMarginProperty = "RightMargin";	
	
	private boolean isLandscape = false;
	private int width = 0;
	private int height = 0;
	private int topMargin = 0;
	private int bottomMargin = 0;
	private int leftMargin = 0;
	private int rightMargin = 0;	
	
	
	public CheckPage(boolean isLandscape, int width, int height, int topMargin, int bottomMargin, int leftMargin, int rightMargin){
		this.isLandscape = isLandscape;
		this.width = width;
		this.height = height;
		this.topMargin = topMargin;
		this.bottomMargin = bottomMargin;
		this.leftMargin = leftMargin;
		this.rightMargin = rightMargin;
	}
	
	@Parameters
    public static Collection<Object[]> data(){
    	Object[][] params = new Object[][]{
    			{false,10000, 20000, 1000,2000, 0, 0},
    			{false, 30000,40000, 0, 0, 100, 200},
    			{true, 900, 10000, 0,0,0,0},
    			{true, 20000, 30000, 300, 300, 300, 300}    			
    			};
    	return Arrays.asList(params);
    }	
    
    /**
     * test page's orientation, width, height, margins 
     * @throws Exception
     */    
	@Test
	public void testPage() throws Exception
	{		
		XComponent xComponent = unoApp.newDocument("swriter");
			
		SWUtil.setDefaultPageStyleProperty(xComponent, isLandscapeProperty, new Boolean(this.isLandscape));
		SWUtil.setDefaultPageStyleProperty(xComponent, widthProperty, Integer.valueOf(this.width));
		SWUtil.setDefaultPageStyleProperty(xComponent, heightProperty, Integer.valueOf(this.height));
		SWUtil.setDefaultPageStyleProperty(xComponent, topMarginProperty, Integer.valueOf(this.topMargin));
		SWUtil.setDefaultPageStyleProperty(xComponent, bottomMarginProperty, Integer.valueOf(this.bottomMargin));
		SWUtil.setDefaultPageStyleProperty(xComponent, leftMarginProperty, Integer.valueOf(this.leftMargin));
		SWUtil.setDefaultPageStyleProperty(xComponent, rightMarginProperty, Integer.valueOf(this.rightMargin));
		
		//save as ODT and reopen, get border
		unoApp.saveDocument(xComponent, tempFilePathODT);        
        unoApp.closeDocument(xComponent);
        xComponent = unoApp.loadDocument(tempFilePathODT);        
        
        boolean actualIsLandScape = ((Boolean)SWUtil.getDefaultPageStyleProperty(xComponent, isLandscapeProperty)).booleanValue();
		int actualWidth = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, widthProperty)).intValue();
		int actualHeight = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, heightProperty)).intValue();
		int actualTop = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, topMarginProperty)).intValue();
		int actualBottom = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, bottomMarginProperty)).intValue();
		int actualLeft = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, leftMarginProperty)).intValue();
		int actualRight = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, rightMarginProperty)).intValue();
        
		
		
		this.compare("ODT", actualIsLandScape,actualWidth,actualHeight,actualTop,actualBottom, actualLeft,actualRight);
		
		//save as DOC and reopen, get properties        
	    SWUtil.saveAsDoc(xComponent, FileUtil.getUrl(tempFilePathDOC));
	    unoApp.closeDocument(xComponent);
	    xComponent = unoApp.loadDocument(tempFilePathDOC);	
	    
	    actualIsLandScape = ((Boolean)SWUtil.getDefaultPageStyleProperty(xComponent, isLandscapeProperty)).booleanValue();
	    actualWidth = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, widthProperty)).intValue();
		actualHeight = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, heightProperty)).intValue();
		actualTop = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, topMarginProperty)).intValue();
		actualBottom = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, bottomMarginProperty)).intValue();
		actualLeft = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, leftMarginProperty)).intValue();
		actualRight = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, rightMarginProperty)).intValue();
		
		this.compare("DOC", actualIsLandScape,actualWidth,actualHeight,actualTop,actualBottom, actualLeft,actualRight);
		
		unoApp.closeDocument(xComponent);
        
	}
	
	private void compare(String preDescription, boolean isLandScape, int width, int height, int top, int bottom, int left, int right){		
		Assert.assertEquals(preDescription + ":" + this.isLandscapeProperty,this.isLandscape, isLandScape);
		Assert.assertEquals(preDescription + ":" + this.widthProperty,(double)this.width, (double)width, 2);
		Assert.assertEquals(preDescription + ":" + this.heightProperty,(double)this.height, (double)height, 2);
		Assert.assertEquals(preDescription + ":" + this.topMarginProperty,(double)this.topMargin, (double)top, 2);	
		Assert.assertEquals(preDescription + ":" + this.bottomMarginProperty,(double)this.bottomMargin, (double)bottom, 2);
		Assert.assertEquals(preDescription + ":" + this.leftMarginProperty,(double)this.leftMargin, (double)left, 2);
		Assert.assertEquals(preDescription + ":" + this.rightMarginProperty,(double)this.rightMargin, (double)right, 2);		
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
