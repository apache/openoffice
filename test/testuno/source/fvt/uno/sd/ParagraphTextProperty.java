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

package fvt.uno.sd;

import junit.framework.Assert;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.PageUtil;
import testlib.uno.ShapeUtil;

import com.sun.star.awt.Point;
import com.sun.star.awt.Size;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.drawing.TextFitToSizeType;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XDrawPages;
import com.sun.star.drawing.XDrawPagesSupplier;
import com.sun.star.drawing.XShape;
import com.sun.star.drawing.XShapes;
import com.sun.star.frame.XStorable;
import com.sun.star.lang.XComponent;

import com.sun.star.presentation.XPresentation;
import com.sun.star.presentation.XPresentationSupplier;

import com.sun.star.text.ControlCharacter;
import com.sun.star.text.XText;
import com.sun.star.text.XTextCursor;
import com.sun.star.text.XTextRange;
import com.sun.star.uno.UnoRuntime;

public class ParagraphTextProperty {
	XPresentationSupplier sdDocument = null;
	XPresentation pre = null;
	XComponent precomp = null;
	XComponent impressDocument = null;
	XComponent reLoadFile = null;
	XDrawPagesSupplier drawsupplier = null;
	XDrawPages drawpages = null;
	XShapes xShapes = null;
	XDrawPage xpage = null;
	String filePath=null;
    
	UnoApp unoApp = new UnoApp();
	
	/**
	 * @throws java.lang.Exception
	 */
	@Before
	public void setUp() throws Exception {
		unoApp.start();
		createDocumentAndSlide();
	}

	@After
	public void tearDown() throws Exception {
		unoApp.closeDocument(impressDocument);
		unoApp.closeDocument(reLoadFile);
		unoApp.close();
	}

	@Test
	public void testParagraphPropertyShape() throws Exception {
		 Point po = new Point(5000, 5000);
		 xShapes = (XShapes) UnoRuntime.queryInterface(XShapes.class, xpage);
		 // create the shape
		 XShape xRectangle = ShapeUtil.createShape(impressDocument, po, new Size(21000, 12500), "com.sun.star.drawing.RectangleShape");
		 xShapes.add(xRectangle);
		 
		 XPropertySet xShapePropSet = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xRectangle);
		 // TextFitToSize
		 xShapePropSet.setPropertyValue("TextFitToSize", TextFitToSizeType.PROPORTIONAL);
		 // border size
		 xShapePropSet.setPropertyValue("TextLeftDistance", new Integer(2500));
		 xShapePropSet.setPropertyValue("TextRightDistance", new Integer(2500));
		 xShapePropSet.setPropertyValue("TextUpperDistance", new Integer(2500));
		 xShapePropSet.setPropertyValue("TextLowerDistance", new Integer(2500));
		 XPropertySet xTextPropSet = addPortion(xRectangle, "using TextFitToSize", false);
		 xTextPropSet = addPortion(xRectangle, "and a Border distance of 2,5 cm", true);
		 
		 xRectangle = saveAndLoadShape(1,0);
		 
		 Assert.assertEquals("TextLeftDistance is 2500", 2500, xShapePropSet.getPropertyValue("TextLeftDistance"));
		 Assert.assertEquals("TextRightDistance is 2500", 2500, xShapePropSet.getPropertyValue("TextRightDistance"));
		 Assert.assertEquals("TextUpperDistance is 2500", 2500, xShapePropSet.getPropertyValue("TextUpperDistance"));
		 Assert.assertEquals("TextLowerDistance is 2500", 2500, xShapePropSet.getPropertyValue("TextLowerDistance"));
		 
	}
		
	public static XPropertySet addPortion(XShape xShape, String sText, boolean bNewParagraph)
	         throws com.sun.star.lang.IllegalArgumentException {
	     XText xText = (XText)UnoRuntime.queryInterface(XText.class, xShape);
	     XTextCursor xTextCursor = xText.createTextCursor();
	     xTextCursor.gotoEnd(false);
	     if (bNewParagraph) {
	         xText.insertControlCharacter(xTextCursor, ControlCharacter.PARAGRAPH_BREAK, false);
	         xTextCursor.gotoEnd(false);
	     }
	     XTextRange xTextRange = (XTextRange)UnoRuntime.queryInterface(XTextRange.class, xTextCursor);
	     xTextRange.setString(sText);
	     xTextCursor.gotoEnd(true);
	     XPropertySet xPropSet = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, xTextRange);
	     return xPropSet;
	 }
	
	/**
	 * create a new presentation document and insert a new slide.
	 * 
	 * @throws Exception
	 */
	public void createDocumentAndSlide() throws Exception {
		impressDocument = (XComponent) UnoRuntime.queryInterface(
				XComponent.class, unoApp.newDocument("simpress"));
		drawsupplier = (XDrawPagesSupplier) UnoRuntime.queryInterface(
				XDrawPagesSupplier.class, impressDocument);
		drawpages = drawsupplier.getDrawPages();
		drawpages.insertNewByIndex(1);
		xpage = PageUtil.getDrawPageByIndex(impressDocument, 1);
	}
	
	/**
	 * Save presentation and reLoad the presentation and shape in it.
	 * 
	 * @param po
	 * @param shapeType
	 * @return
	 * @throws Exception
	 */
	public XShape saveAndLoadShape(int pageIndex, int shapeIndex) throws Exception {
		reLoadFile = saveAndReloadDoc(impressDocument,
				"impress8", "odp");
		xShapes=ShapeUtil.getShapes(reLoadFile, pageIndex);
		return  (XShape) UnoRuntime.queryInterface(XShape.class, xShapes.getByIndex(shapeIndex));
	}

	/**
	 * save and reload Presentation document.
	 * 
	 * @param presentationDocument
	 * @param sFilter
	 * @param sExtension
	 * @return
	 * @throws Exception
	 */
	private XComponent saveAndReloadDoc(XComponent presentationDocument,
			String sFilter, String sExtension) throws Exception {
		filePath = Testspace.getPath("tmp/paragraphtextproperty."
				+ sExtension);
		PropertyValue[] aStoreProperties = new PropertyValue[2];
		aStoreProperties[0] = new PropertyValue();
		aStoreProperties[1] = new PropertyValue();
		aStoreProperties[0].Name = "Override";
		aStoreProperties[0].Value = true;
		aStoreProperties[1].Name = "FilterName";
		aStoreProperties[1].Value = sFilter;
		XStorable xStorable = (XStorable) UnoRuntime.queryInterface(
				XStorable.class, presentationDocument);
		xStorable.storeToURL(FileUtil.getUrl(filePath), aStoreProperties);

		return (XComponent) UnoRuntime.queryInterface(XComponent.class,
				unoApp.loadDocument(filePath));
	}
}
