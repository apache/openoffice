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
 * test page's back color,
 * test page footer/header's back color. 
 *
 */
@RunWith(Parameterized.class)
public class CheckBackColor {
	UnoApp unoApp = new UnoApp();
	XTextDocument textDocument = null;
	File temp = null;
	String tempFilePathODT = "";	
	String tempFilePathDOC = "";	
	
	private String onProperty = "";
	private String backColorProperty = "";	
	
	private int backColor = 0;	
	
	
	public CheckBackColor(String onProperty, String backColorProperty, 	int backColor){
		this.onProperty = onProperty;
		this.backColorProperty = backColorProperty;		
		
		this.backColor = backColor;		
	}
	
	@Parameters
    public static Collection<Object[]> data(){
    	Object[][] params = new Object[][]{
    			{"FooterIsOn", "BackColor", 255},
    			{"FooterIsOn", "BackColor", 65535},  
    			{"FooterIsOn", "FooterBackColor", 255},
    			{"FooterIsOn", "FooterBackColor", 0},    			
    			{"HeaderIsOn", "HeaderBackColor", 65536},
    			{"HeaderIsOn", "HeaderBackColor", 65535}    			
    			};
    	return Arrays.asList(params);
    }	
    
    /**
     * test header/footer's back color and back graphic.
     * @throws Exception
     */
    @Ignore("#120949 - header/footer's background lost when export to doc format ")    
	@Test
	public void testFooterHeaderBackground() throws Exception
	{		
		XComponent xComponent = unoApp.newDocument("swriter");
		//turn on header/footer		
		SWUtil.setDefaultPageStyleProperty(xComponent, onProperty, new Boolean(true));
		SWUtil.setDefaultPageStyleProperty(xComponent, backColorProperty, Integer.valueOf(backColor));		
		
		//save as ODT and reopen, get back color
		unoApp.saveDocument(xComponent, tempFilePathODT);        
        unoApp.closeDocument(xComponent);
        xComponent = unoApp.loadDocument(tempFilePathODT);        
        
		int color = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, backColorProperty)).intValue();		
		
		
		this.compare("ODT", color);
		
		//save as DOC and reopen, only get back color        
	    SWUtil.saveAsDoc(xComponent, FileUtil.getUrl(tempFilePathDOC));
	    unoApp.closeDocument(xComponent);
	    xComponent = unoApp.loadDocument(tempFilePathDOC);	
	    color = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, backColorProperty)).intValue();
		
		this.compare("DOC", color);
		
		unoApp.closeDocument(xComponent);        
	}
	
	private void compare(String preDescription, int color){		
		Assert.assertEquals(preDescription + ":" + backColorProperty,(double)backColor, color, 2);				
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
