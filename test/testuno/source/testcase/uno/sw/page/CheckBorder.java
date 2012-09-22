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
 * test page's border
 * test page's footer/header's border 
 *
 */
@RunWith(Parameterized.class)
public class CheckBorder {
	UnoApp unoApp = new UnoApp();
	XTextDocument textDocument = null;
	File temp = null;
	String tempFilePathODT = "";	
	String tempFilePathDOC = "";	
	
	private String onProperty = "";
	private String borderProperty = "";
	private String borderDistanceProperty = "";
	private BorderLine borderLine = null;
	private int borderDistance = 0;
	
	
	public CheckBorder(String onProperty, String borderProperty, String borderDistanceProperty, 
			int color, int lineDistance, int innerLineWidth, int outerLineWidth, int borderDistance){
		this.borderLine = new BorderLine();
		this.borderLine.Color = color;
		this.borderLine.LineDistance = (short)lineDistance;
		this.borderLine.InnerLineWidth = (short)innerLineWidth;
		this.borderLine.OuterLineWidth = (short)outerLineWidth;
		
		this.onProperty = onProperty;
		this.borderProperty = borderProperty;
		this.borderDistanceProperty = borderDistanceProperty;	
		
		this.borderDistance = borderDistance;
	}
	
	@Parameters
    public static Collection<Object[]> data(){
    	Object[][] params = new Object[][]{
    			/*{"FooterIsOn", "LeftBorder", "LeftBorderDistance", 255, 100, 50,70,199},
    			{"FooterIsOn", "RightBorder", "RightBorderDistance", 256, 0, 0,0,20},
    			{"FooterIsOn", "BottomBorder", "BottomBorderDistance", 65536, 15, 10,30,300},
    			{"FooterIsOn", "TopBorder", "TopBorderDistance", 65535, 6, 100,200,400},
    			{"FooterIsOn", "FooterLeftBorder", "FooterLeftBorderDistance", 255, 100, 50,70,199},
    			{"FooterIsOn", "FooterRightBorder", "FooterRightBorderDistance", 256, 0, 0,0,20},
    			{"FooterIsOn", "FooterBottomBorder", "FooterBottomBorderDistance", 65536, 15, 10,30,300},
    			{"FooterIsOn", "FooterTopBorder", "FooterTopBorderDistance", 65535, 6, 100,200,400},*/
    			{"HeaderIsOn", "HeaderLeftBorder", "HeaderLeftBorderDistance", 255, 100, 50,70,0},
    			{"HeaderIsOn", "HeaderRightBorder", "HeaderRightBorderDistance", 256, 0, 0,0,100},
    			{"HeaderIsOn", "HeaderBottomBorder", "HeaderBottomBorderDistance", 65536, 15, 10,30,900},
    			{"HeaderIsOn", "HeaderTopBorder", "HeaderTopBorderDistance", 65535, 6, 100,200,50}
    			};
    	return Arrays.asList(params);
    }	
    
    /**
     * test page's border
     * test page's footer/header's border
     * @throws Exception
     */
    @Ignore("#120822 - header/footer's border styles are lost when export to DOC format")
	@Test
	public void testFooterHeaderBorder() throws Exception
	{		
		XComponent xComponent = unoApp.newDocument("swriter");
		//turn on header/footer		
		SWUtil.setDefaultPageStyleProperty(xComponent, onProperty, new Boolean(true));
		SWUtil.setDefaultPageStyleProperty(xComponent, borderProperty, borderLine);
		SWUtil.setDefaultPageStyleProperty(xComponent, borderDistanceProperty, Integer.valueOf(borderDistance));
		
		//save as ODT and reopen, get border
		unoApp.saveDocument(xComponent, tempFilePathODT);        
        unoApp.closeDocument(xComponent);
        xComponent = unoApp.loadDocument(tempFilePathODT);
        
        BorderLine actualBorderLine = (BorderLine)SWUtil.getDefaultPageStyleProperty(xComponent, borderProperty);
		int actualBorderDistance = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, borderDistanceProperty)).intValue();
		
		this.compareBorder("ODT:", borderLine, borderDistance);
		
		//save as DOC and reopen, get border        
	    SWUtil.saveAsDoc(xComponent, FileUtil.getUrl(tempFilePathDOC));
	    unoApp.closeDocument(xComponent);
	    xComponent = unoApp.loadDocument(tempFilePathDOC);	
	    actualBorderLine = (BorderLine)SWUtil.getDefaultPageStyleProperty(xComponent, borderProperty);
	    actualBorderDistance = ((Integer)SWUtil.getDefaultPageStyleProperty(xComponent, borderDistanceProperty)).intValue();
		
		this.compareBorder("DOC:", actualBorderLine, actualBorderDistance);
		
		unoApp.closeDocument(xComponent);
        
	}
	
	private void compareBorder(String preDes, BorderLine actualBorderLine, int actualBorderDistance){		
		Assert.assertEquals(preDes + borderProperty + "-->color.",(double)borderLine.Color, (double)actualBorderLine.Color, 2);		
		Assert.assertEquals(preDes + borderProperty + "-->LineDistance.", (double)borderLine.LineDistance, (double)actualBorderLine.LineDistance, 2);
		Assert.assertEquals(preDes + borderProperty + "-->InnerLineWidth.", (double)borderLine.InnerLineWidth, (double)actualBorderLine.InnerLineWidth, 2);
		Assert.assertEquals(preDes + borderProperty + "-->OuterLineWidth.", (double)borderLine.OuterLineWidth, (double)actualBorderLine.OuterLineWidth, 2);
		
		Assert.assertEquals(preDes + "-->" + borderDistanceProperty, (double)borderLine.OuterLineWidth, (double)actualBorderLine.OuterLineWidth, 2);
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
