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
import com.sun.star.table.ShadowFormat;
import com.sun.star.table.ShadowLocation;

/**
 * test footer/header's border's shadow format. 
 * test page border's shadow format
 *
 */
@RunWith(Parameterized.class)
public class CheckShadowFormat {
	UnoApp unoApp = new UnoApp();
	XTextDocument textDocument = null;
	File temp = null;
	String tempFilePathODT = "";	
	String tempFilePathDOC = "";	
	
	private String onProperty = "";
	private String shadowFormatProperty = "";
	
	private ShadowFormat shadowFormat = null;		
	
	public CheckShadowFormat(String onProperty, String shadowFormatProperty, int locationValue, int width, 
			boolean isTransparent, int color){
		this.onProperty = onProperty;
		this.shadowFormatProperty = shadowFormatProperty;
		this.shadowFormat = new ShadowFormat();
		this.shadowFormat.Location = ShadowLocation.fromInt(locationValue);
		this.shadowFormat.ShadowWidth = (short)width;
		this.shadowFormat.IsTransparent = isTransparent;
		this.shadowFormat.Color = color;
	}
	
	/**
	 * ShadowLocation
	 * 0:NONE
	 * 1:TOP_LEFT
	 * 2:TOP_RIGHT
	 * 3:BOTTOM_LEFT
	 * 4:BOTTOM_RIGHT
	 * @return
	 */
	@Parameters
    public static Collection<Object[]> data(){
    	Object[][] params = new Object[][]{
    			{"FooterIsOn", "ShadowFormat", 0, 200, false,255},
    			{"FooterIsOn", "ShadowFormat", 1, 500, false,256},
    			{"FooterIsOn", "ShadowFormat", 2, 50, false,65535},
    			{"FooterIsOn", "ShadowFormat", 3, 30, false,65536},
    			{"FooterIsOn", "ShadowFormat", 4, 1000, false,255},
    			{"FooterIsOn", "FooterShadowFormat", 0, 200, false,256},
    			{"FooterIsOn", "FooterShadowFormat", 1, 500, false,65535},
    			{"FooterIsOn", "FooterShadowFormat", 2, 50, false,65536},
    			{"FooterIsOn", "FooterShadowFormat", 3, 30, false,255},
    			{"FooterIsOn", "FooterShadowFormat", 4, 1000, false,256},
    			{"HeaderIsOn", "HeaderShadowFormat", 0, 200, false,65535},
    			{"HeaderIsOn", "HeaderShadowFormat", 1, 500, false,65536},
    			{"HeaderIsOn", "HeaderShadowFormat", 2, 50, false,255},
    			{"HeaderIsOn", "HeaderShadowFormat", 3, 30, false,256},
    			{"HeaderIsOn", "HeaderShadowFormat", 4, 1000, false,65536}
    			};
    	return Arrays.asList(params);
    }	
    
    /**
     * test footer/header's border's shadow format. 
     * test page border's shadow format
     * @throws Exception
     */
    @Ignore("#120969 - page and page's footer/header's shadow style lost after export to doc format in AOO")
    @Test
	public void testFooterHeader() throws Exception
	{    	
		XComponent xComponent = unoApp.newDocument("swriter");
		//turn on header/footer		
		SWUtil.setDefaultPageStyleProperty(xComponent, onProperty, new Boolean(true));
		SWUtil.setDefaultPageStyleProperty(xComponent, shadowFormatProperty, this.shadowFormat);		
		
		//save as ODT and reopen, get border
		unoApp.saveDocument(xComponent, tempFilePathODT);        
        unoApp.closeDocument(xComponent);
        xComponent = unoApp.loadDocument(tempFilePathODT);        
        
		ShadowFormat actualShadowFormat = (ShadowFormat)SWUtil.getDefaultPageStyleProperty(xComponent, shadowFormatProperty);		
		
		this.compare("ODT", actualShadowFormat);
		
		//save as DOC and reopen, get properties        
	    SWUtil.saveAsDoc(xComponent, FileUtil.getUrl(tempFilePathDOC));
	    unoApp.closeDocument(xComponent);
	    xComponent = unoApp.loadDocument(tempFilePathDOC);	
	    actualShadowFormat = (ShadowFormat)SWUtil.getDefaultPageStyleProperty(xComponent, shadowFormatProperty);
		
		this.compare("DOC", actualShadowFormat);
		
		unoApp.closeDocument(xComponent);
        
	}
	
	private void compare(String preDescription, ShadowFormat actual){		
		Assert.assertEquals(preDescription + ":" + shadowFormatProperty + "-->Location",this.shadowFormat.Location.getValue(), actual.Location.getValue());
		//check shadow width, isTransparent, color if shadow location is not NONE
		if(!this.shadowFormat.Location.equals(ShadowLocation.NONE)){
			Assert.assertEquals(preDescription + ":" + shadowFormatProperty + "-->Width",(double)this.shadowFormat.ShadowWidth, (double)actual.ShadowWidth, 2);
			Assert.assertEquals(preDescription + ":" + shadowFormatProperty + "-->IsTransparent",this.shadowFormat.IsTransparent, actual.IsTransparent);
			Assert.assertEquals(preDescription + ":" + shadowFormatProperty + "-->Color",this.shadowFormat.Color, actual.Color);
		}		
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
