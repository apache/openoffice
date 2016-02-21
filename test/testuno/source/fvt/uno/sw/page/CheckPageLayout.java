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
import java.lang.Enum;

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
import com.sun.star.style.PageStyleLayout;

/**
 * test page's layout settings 
 *
 */
@RunWith(Parameterized.class)
public class CheckPageLayout {
	UnoApp unoApp = new UnoApp();
	XTextDocument textDocument = null;
	File temp = null;
	String tempFilePathODT = "";	
	String tempFilePathDOC = "";	
	
	private String pageStyleLayoutProperty = "PageStyleLayout";	
	
	private PageStyleLayout pageStyleLayout = PageStyleLayout.getDefault();	
	
	
	public CheckPageLayout(int styleValue){
		this.pageStyleLayout = PageStyleLayout.fromInt(styleValue);
	}
	/**
	 * 0:ALL
	 * 1:LEFT
	 * 2:RIGHT
	 * 3:MIRRORED
	 * @return
	 */
	@Parameters
    public static Collection<Object[]> data(){
    	Object[][] params = new Object[][]{
    			{0},
    			{1},
    			{2},
    			{3}    			
    			};
    	return Arrays.asList(params);
    }	
    
    /**
     * test page's layout settings 
     * @throws Exception
     */   
    @Ignore("#120964 - page layout 'only left' and 'only right' all changed to default value 'right and left' after export to doc format in AOO")
	@Test
	public void testPageStyleLayout() throws Exception
	{		
		XComponent xComponent = unoApp.newDocument("swriter");		
		SWUtil.setDefaultPageStyleProperty(xComponent, this.pageStyleLayoutProperty, this.pageStyleLayout);
		
		//save as ODT and reopen, get border
		unoApp.saveDocument(xComponent, tempFilePathODT);        
        unoApp.closeDocument(xComponent);
        xComponent = unoApp.loadDocument(tempFilePathODT);        
        
       	PageStyleLayout actualPageStyleLayout = (PageStyleLayout)SWUtil.getDefaultPageStyleProperty(xComponent, this.pageStyleLayoutProperty);
				
		this.compare("ODT", actualPageStyleLayout);
		
		//save as DOC and reopen, get properties        
	    SWUtil.saveAsDoc(xComponent, FileUtil.getUrl(tempFilePathDOC));
	    unoApp.closeDocument(xComponent);
	    xComponent = unoApp.loadDocument(tempFilePathDOC);	
	    
	    actualPageStyleLayout = (PageStyleLayout)SWUtil.getDefaultPageStyleProperty(xComponent, this.pageStyleLayoutProperty);
		
	    this.compare("DOC", actualPageStyleLayout);
		
		unoApp.closeDocument(xComponent);
        
	}
	
	private void compare(String preDescription, PageStyleLayout actual){		
		Assert.assertEquals(preDescription + ":" + this.pageStyleLayoutProperty,this.pageStyleLayout.getValue(), actual.getValue());
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
