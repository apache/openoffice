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
 * test page's back graphic,
 * test page footer/header's back graphic. 
 *
 */
@RunWith(Parameterized.class)
public class CheckBackGraphic {
	UnoApp unoApp = new UnoApp();
	XTextDocument textDocument = null;
	File temp = null;
	String tempFilePathODT = "";	
	String tempFilePathDOC = "";	
	
	private String onProperty = "";	
	private String backGraphicURLProperty = "";		
	
	private String backGraphicPath = "";
	private String backGraphicURL = "";
	
	
	public CheckBackGraphic(String onProperty, String backGraphicURLProperty, String backGraphicPath){
		this.onProperty = onProperty;		
		this.backGraphicURLProperty = backGraphicURLProperty;		
		
		this.backGraphicPath = backGraphicPath;
	}
	
	@Parameters
    public static Collection<Object[]> data(){
    	Object[][] params = new Object[][]{
    			{"FooterIsOn", "BackGraphicURL", "uno/sw/page/BackGraphic1.jpg"},
    			{"FooterIsOn", "BackGraphicURL", "uno/sw/page/BackGraphic2.jpg"},  
    			{"FooterIsOn", "FooterBackGraphicURL", "uno/sw/page/BackGraphic1.jpg"},
    			{"FooterIsOn", "FooterBackGraphicURL", "uno/sw/page/BackGraphic2.jpg"},    			
    			{"HeaderIsOn", "HeaderBackGraphicURL", "uno/sw/page/BackGraphic1.jpg"},
    			{"HeaderIsOn", "HeaderBackGraphicURL", "uno/sw/page/BackGraphic2.jpg"}    			
    			};
    	return Arrays.asList(params);
    }	
    
    /**
     * test header/footer's back color and back graphic.
     * @throws Exception
     */     
	@Test
	public void testFooterHeaderBackground() throws Exception
	{			
		String graphicUrl = FileUtil.getUrl(prepareData(backGraphicPath));
		backGraphicURL = graphicUrl;
		XComponent xComponent = unoApp.newDocument("swriter");
		//turn on header/footer		
		SWUtil.setDefaultPageStyleProperty(xComponent, onProperty, new Boolean(true));		
		SWUtil.setDefaultPageStyleProperty(xComponent, backGraphicURLProperty, backGraphicURL);		
		
		//save as ODT and reopen, get back color and back graphic
		unoApp.saveDocument(xComponent, tempFilePathODT);        
        unoApp.closeDocument(xComponent);
        xComponent = unoApp.loadDocument(tempFilePathODT);        
		
		String graphic = (String)SWUtil.getDefaultPageStyleProperty(xComponent, backGraphicURLProperty);
		
		
		Assert.assertEquals("ODT:" + backGraphicURLProperty, backGraphicURL, graphic);	
		
		unoApp.closeDocument(xComponent);        
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
