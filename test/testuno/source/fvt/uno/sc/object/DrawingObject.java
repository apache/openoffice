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

package fvt.uno.sc.object;

import static org.junit.Assert.assertEquals;

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

import testlib.uno.SCUtil;
import testlib.uno.ShapeUtil;

import com.sun.star.awt.Point;
import com.sun.star.awt.Size;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XDrawPageSupplier;
import com.sun.star.drawing.XShape;
import com.sun.star.drawing.XShapes;
import com.sun.star.lang.XComponent;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.uno.UnoRuntime;

/**
 *  Check the shapes can be added and removed
 * 
 */
@RunWith(value = Parameterized.class)
public class DrawingObject {

	private String inputType;
	private String fileType;
	
	private static final UnoApp unoApp = new UnoApp();
	
	XComponent scComponent = null;
	XSpreadsheetDocument scDocument = null;
	
	@Parameters
	public static Collection<Object[]> data() throws Exception {

		return Arrays.asList(new Object[][] {
			{"com.sun.star.drawing.RectangleShape",  "ods"},
			{"com.sun.star.drawing.EllipseShape",  "ods"},
			{"com.sun.star.drawing.LineShape",  "ods"},
			{"com.sun.star.drawing.GraphicObjectShape",  "ods"},
			{"com.sun.star.drawing.MeasureShape",  "ods"},
			{"com.sun.star.drawing.PageShape",  "ods"},
			{"com.sun.star.drawing.TextShape",  "ods"},
			{"com.sun.star.drawing.RectangleShape",  "xls"},
			{"com.sun.star.drawing.EllipseShape",  "xls"},
			{"com.sun.star.drawing.LineShape",  "xls"},
			{"com.sun.star.drawing.MeasureShape",  "xls"},
			{"com.sun.star.drawing.PageShape",  "xls"},
			{"com.sun.star.drawing.TextShape",  "xls"},
		});
	}
	
	public DrawingObject( String inputType, String fileType) {
//		this.expected = expected;
		this.inputType = inputType;
//		this.numberData = numberData;
		this.fileType = fileType;
	}
	
	@Before
	public void setUp() throws Exception {
		scComponent = unoApp.newDocument("scalc");
		scDocument = SCUtil.getSCDocument(scComponent);
	}

	@After
	public void tearDown() throws Exception {
		unoApp.closeDocument(scComponent);
		
	}
	
	@BeforeClass
	public static void setUpConnection() throws Exception {
		unoApp.start();
	}

	@AfterClass
	public static void tearDownConnection() throws InterruptedException, Exception {
		unoApp.close();
		SCUtil.clearTempDir();	
	}
	
	/**
	 * Check Drawing Object
	 * 1. Create a spreadsheet file.
	 * 2. Add shape.
	 * 3. Save file as ODF/MSBinary format.
	 * 4. Close and reopen file.  -> Check the shape added.
	 * 5. Remove shape.
	 * 6. Save file as ODF/MSBinary format.
	 * 7. Close and reopen file.  -> Check the shape removed.
	 * @throws Exception
	 */
	
	@Test
	public void testDrawingObject() throws Exception {

		String fileName = inputType;
		
	    scDocument = SCUtil.getSCDocument(scComponent);
		XSpreadsheet xSheet = SCUtil.getCurrentSheet(scDocument);
		XDrawPageSupplier xDrawPageSupplier =
        		(XDrawPageSupplier)UnoRuntime.queryInterface(XDrawPageSupplier.class, xSheet);
        XDrawPage xDrawPage = xDrawPageSupplier.getDrawPage();
        XShapes xShapes = (XShapes) UnoRuntime.queryInterface(XShapes.class, xDrawPage);
       
        //Execute add shape action
        Point po = new Point(100, 100);
        Size Si = new Size(5000, 5000);
        XShape xShape = ShapeUtil.createShape(scComponent, po, Si, inputType);
        xShapes.add(xShape);
          
        //Verify shape is added correctly
        
        //Verify number of shape in sheet.
  		assertEquals("Verify number of shape in sheet add execute add action.",1, xShapes.getCount());
		
  		//Save it, close and reopen it
		SCUtil.saveFileAs(scComponent, fileName, fileType);
		scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);

		xSheet = SCUtil.getCurrentSheet(scDocument);
	    xDrawPageSupplier =
	    		(XDrawPageSupplier)UnoRuntime.queryInterface(XDrawPageSupplier.class, xSheet);
		xDrawPage = xDrawPageSupplier.getDrawPage();
		xShapes = (XShapes) UnoRuntime.queryInterface(XShapes.class, xDrawPage);
		        
		//Verify number of shape in sheet.
		assertEquals("Verify number of shape in sheet.",1, xShapes.getCount());
		
		//Verify correct shape type added.
		assertEquals("Verify shape type is correct.",inputType, xShape.getShapeType());
	
		xShape = (XShape) UnoRuntime.queryInterface(XShape.class, xShapes.getByIndex(0));
		
		//Execute remove drawing objects
		xShapes.remove(xShape);
	
		//Verify results after remove shape
        assertEquals("Verify results after remove shape.",0, xShapes.getCount());
        
        //Save and reload document
        
        //Save the modified spreadsheet first
        SCUtil.save(scDocument);
        
        //close it and reload it
        scDocument = SCUtil.reloadFile(unoApp, scDocument, fileName + "." + fileType);
        
        //Get Draw page
      	xSheet = SCUtil.getCurrentSheet(scDocument);
      	xDrawPageSupplier =
      			(XDrawPageSupplier)UnoRuntime.queryInterface(XDrawPageSupplier.class, xSheet);
      	xDrawPage = xDrawPageSupplier.getDrawPage();
        xShapes = (XShapes) UnoRuntime.queryInterface(XShapes.class, xDrawPage);
   		        
        //Verify number of shape in sheet after executing remove action.
        assertEquals("Verify 0 shape in sheet after executing remove action.",
        		0, xShapes.getCount());
        
      
		


	}
	
}
