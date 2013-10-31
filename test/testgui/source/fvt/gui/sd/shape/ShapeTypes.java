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




package fvt.gui.sd.shape;
import static org.junit.Assert.*;
import static org.openoffice.test.vcl.Tester.sleep;
import static testlib.gui.AppTool.*;
import static testlib.gui.UIMap.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.openoffice.test.common.Logger;

import testlib.gui.AppTool;



public class ShapeTypes {

	@Rule
	public Logger log = Logger.getLogger(this);
	
	@Before
	public void setUp() throws Exception {
		app.start();
		AppTool.newPresentation();

		// Pop up navigator panel
		if (!sdNavigatorDlg.exists()) {
			app.dispatch(".uno:Navigator");
		}
		//Pop up drawing toolbar
		if(!sdDrawingToolbar.exists()){
			app.dispatch(".uno:AvailableToolbars?Toolbar:string=toolbar");
		}
	}

	@After
	public void tearDown() throws Exception {
		//close navigator
		if (sdNavigatorDlg.exists()) {
			app.dispatch(".uno:Navigator");
		}
		AppTool.discard();
		app.stop();
	}

	/**
	 * Insert a new CallOut shape 
	 * @throws Exception
	 */
	@Test
	public void testCalloutShapes() throws Exception{

		impress.focus();
		//before insert CallOut Shape
		sdNavigator.focus();
		sdNavigator.waitForEnabled( 2.0, 0.1);
		sdNavigatorShapeFilter.click();
		typeKeys("<down><down>");
		typeKeys("<enter>");
		sdNavigator.select(0);
		typeKeys("<enter>");
		String[] allShapes=sdNavigator.getAllItemsText();
		assertEquals(3, allShapes.length);
		
		//After insert CallOut shape
		sdCalloutShapes.click();
		impress.focus();
		impress.drag(100, 100, 200, 200);
		sdNavigatorDlg.focus();
		sdNavigator.waitForEnabled( 2.0, 0.1);
		sdNavigatorShapeFilter.click();
		typeKeys("<down><down>");
		typeKeys("<enter>");
		sdNavigator.focus();
		sdNavigator.select(0);
		typeKeys("<enter>");
		allShapes=sdNavigator.getAllItemsText();
		assertEquals(4, allShapes.length);
	}

	/**
	 * Insert a new Star shape
	 * @throws Exception
	 */
	@Test
	public void testStarsShapes() throws Exception{

		impress.focus();
		//before insert CallOut Shape
		sdNavigator.focus();
		sdNavigator.waitForEnabled( 2.0, 0.1);
		sdNavigatorShapeFilter.click();
		typeKeys("<down><down>");
		typeKeys("<enter>");
		sdNavigator.select(0);
		typeKeys("<enter>");
		String[] allShapes=sdNavigator.getAllItemsText();
		assertEquals(3, allShapes.length);
		
		//After insert CallOut shape
		sdStarShapes.click();
		impress.focus();
		impress.drag(100, 100, 200, 200);
		sdNavigatorDlg.focus();
		sdNavigator.waitForEnabled( 2.0, 0.1);
		sdNavigatorShapeFilter.click();
		typeKeys("<down><down>");
		typeKeys("<enter>");
		sdNavigator.focus();
		sdNavigator.select(0);
		typeKeys("<enter>");
		allShapes=sdNavigator.getAllItemsText();
		assertEquals(4, allShapes.length);
	}

}

