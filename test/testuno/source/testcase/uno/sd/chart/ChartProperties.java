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
package testcase.uno.sd.chart;

import static org.junit.Assert.assertEquals;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;
import org.openoffice.test.uno.UnoApp;

import testlib.uno.ChartUtil;
import testlib.uno.PageUtil;
import testlib.uno.ShapeUtil;
import testlib.uno.TestUtil;

import com.sun.star.awt.FontRelief;
import com.sun.star.awt.FontUnderline;
import com.sun.star.awt.FontWeight;
import com.sun.star.awt.Point;
import com.sun.star.awt.Size;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.chart.ChartDataCaption;
import com.sun.star.chart.ChartLegendPosition;
import com.sun.star.chart.XAxisYSupplier;
import com.sun.star.chart.XChartDocument;
import com.sun.star.chart.XDiagram;
import com.sun.star.drawing.FillStyle;
import com.sun.star.drawing.LineStyle;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XDrawPages;
import com.sun.star.drawing.XDrawPagesSupplier;
import com.sun.star.drawing.XShape;
import com.sun.star.drawing.XShapes;
import com.sun.star.frame.XStorable;
import com.sun.star.lang.XComponent;
import com.sun.star.presentation.XPresentation;
import com.sun.star.presentation.XPresentationSupplier;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.XCloseable;
import com.sun.star.util.XModifiable;

public class ChartProperties {
	UnoApp unoApp = new UnoApp();
	XPresentationSupplier sdDocument = null;
	XPresentation pre = null;
	XComponent precomp = null;
	XComponent impressDocument = null;
	XComponent reLoadFile = null;
	XDrawPagesSupplier drawsupplier = null;
	XDrawPages drawpages = null;
	XShapes xShapes = null;
	XDrawPage xpage = null;
	String filePath = null;
	XChartDocument xChartDoc = null;
	com.sun.star.chart2.XChartDocument xChart2Doc = null;

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
		 if (filePath != null)
		 FileUtil.deleteFile(filePath);
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
	 * Insert default Column Chart to slide.
	 * 
	 * @return
	 * @throws Exception
	 */
	public XChartDocument insertDefaultChart() throws Exception {
		Point po = new Point(1000, 1000);
		xShapes = (XShapes) UnoRuntime.queryInterface(XShapes.class, xpage);
		XShape xShape = ShapeUtil.createShape(impressDocument, po, new Size(
				15000, 9271), "com.sun.star.drawing.OLE2Shape");
		xShapes.add(xShape);
		xChartDoc = ChartUtil.retrieveChartDocument(xShape);
		return xChartDoc;
	}

	/**
	 * Insert default Column Chart2 to slide.
	 * 
	 * @return
	 * @throws Exception
	 */
	public com.sun.star.chart2.XChartDocument insertDefaultChart2()
			throws Exception {
		Point po = new Point(1000, 1000);
		xShapes = (XShapes) UnoRuntime.queryInterface(XShapes.class, xpage);
		XShape xShape = ShapeUtil.createShape(impressDocument, po, new Size(
				15000, 9271), "com.sun.star.drawing.OLE2Shape");
		xShapes.add(xShape);
		xChart2Doc = ChartUtil.retrieveChart2Document(xShape);
		return xChart2Doc;
	}

	// --------------Chart Area----------------------
	/**
	 * test Chart Area properties about Border Line
	 * 
	 * @throws Exception
	 */
	@Test
	public void testAreaBordersProperties() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getArea());
		aDiaProp.setPropertyValue("LineStyle", LineStyle.DASH);
		aDiaProp.setPropertyValue("LineColor", 0x00ff00);

		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getArea());
		// ----------------------------
		assertEquals("Not Dash Line Style", LineStyle.DASH,
				aDiaProp.getPropertyValue("LineStyle"));
		assertEquals("Not Yellow Line", 0x00ff00,
				aDiaProp.getPropertyValue("LineColor"));

	}

	/**
	 * test Area Properties about Gradient Fill
	 * @throws Exception
	 */
	@Test
	public void testAreaFillProperties() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getArea());
		aDiaProp.setPropertyValue("FillStyle", FillStyle.GRADIENT);
		aDiaProp.setPropertyValue("FillGradientName", "Radial red/yellow");
		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getArea());
		// ----------------------------
		assertEquals("Not Gradient Fill Style", FillStyle.GRADIENT,
				aDiaProp.getPropertyValue("FillStyle"));
		assertEquals("Not Radial red/yellow Gradient", "Radial red/yellow",
				aDiaProp.getPropertyValue("FillGradientName"));

	}

	/**
	 * test Area properties about Transparency
	 * @throws Exception
	 */
	@Test
	public void testAreaTransparencyProperties() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getArea());
		aDiaProp.setPropertyValue("FillStyle", FillStyle.SOLID);
		aDiaProp.setPropertyValue("FillColor", 0xffff00);
		aDiaProp.setPropertyValue("FillTransparence", 50);
		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getArea());
		// ----------------------------
		assertEquals("Area Fill Transparency is not 50%", new Short("50"),
				aDiaProp.getPropertyValue("FillTransparence"));

	}

	// ------------Chart Legend-------------------
	/**
	 * test Legend Border Line Properties
	 * @throws Exception
	 */
	@Test
	public void testLegendBorderProperties() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getLegend());
		aDiaProp.setPropertyValue("LineStyle", LineStyle.DASH);
		aDiaProp.setPropertyValue("LineColor", 0x00ff00);
		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getLegend());
		// ----------------------------
		assertEquals("Not Dash Line Style", LineStyle.DASH,
				aDiaProp.getPropertyValue("LineStyle"));
		assertEquals("Not Yellow Line", 0x00ff00,
				aDiaProp.getPropertyValue("LineColor"));

	}

	/**
	 * test Legend Fill and transparence properties
	 * @throws Exception
	 */
	@Test
	public void testLegendFillAndTransparence() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getLegend());
		aDiaProp.setPropertyValue("FillStyle", FillStyle.SOLID);
		aDiaProp.setPropertyValue("FillColor", 0xffff00);
		aDiaProp.setPropertyValue("FillTransparence", 50);
		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getLegend());
		// ----------------------------

		assertEquals("Area Fill Transparency is not 50%", new Short("50"),
				aDiaProp.getPropertyValue("FillTransparence"));

	}
	
	/**
	 * test Legend Char Font
	 * @throws Exception
	 */
	@Test
	public void testLegendCharFont() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getLegend());
		aDiaProp.setPropertyValue("CharFontName", "Arial");
		aDiaProp.setPropertyValue("CharWeight", FontWeight.BOLD);
		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getLegend());
		// ----------------------------
		assertEquals("Legend font name is not Arial", "Arial",
				aDiaProp.getPropertyValue("CharFontName"));
		assertEquals("Legend font weight is not BOLD", FontWeight.BOLD,
				aDiaProp.getPropertyValue("CharWeight"));

	}

	/**
	 * test Legend Char Font Effects
	 * @throws Exception
	 */
	@Test
	public void testLegendCharFontEffects() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getLegend());
		aDiaProp.setPropertyValue("CharColor", 0xffff00);
		aDiaProp.setPropertyValue("CharUnderline", FontUnderline.DOUBLE);
		aDiaProp.setPropertyValue("CharUnderlineColor", 0xffff00);
		aDiaProp.setPropertyValue("CharRelief", FontRelief.ENGRAVED);

		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getLegend());
		// ----------------------------
		assertEquals("Legend font color is not yellow", 0xffff00,
				aDiaProp.getPropertyValue("CharColor"));
		assertEquals("Legend font underline is not DOUBLE",
				FontUnderline.DOUBLE,
				aDiaProp.getPropertyValue("CharUnderline"));
		assertEquals("Legend font underline color is not yellow", 0xffff00,
				aDiaProp.getPropertyValue("CharUnderlineColor"));
		assertEquals("Legend font relief is not engraved", FontRelief.ENGRAVED,
				aDiaProp.getPropertyValue("CharRelief"));

	}

	/**
	 * test Legend Position in Chart
	 * @throws Exception
	 */
	@Test
	public void testLegendposition() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getLegend());
		aDiaProp.setPropertyValue("Alignment", ChartLegendPosition.LEFT);

		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getLegend());
		// ----------------------------
		assertEquals("Legend position is not left", ChartLegendPosition.LEFT,
				aDiaProp.getPropertyValue("Alignment"));

	}

	// --------------Chart Title---------------
	/**
	 * test Addition and Modification of Title and subTitle
	 * @throws Exception
	 */
	@Test
	public void testTitleAndSubTitle() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getTitle());
		aDiaProp.setPropertyValue("String", "TestTitle");
		XPropertySet aDiaProp2 = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getSubTitle());
		aDiaProp2.setPropertyValue("String", "TestSubTitle");
		// save and load shape
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getTitle());
		aDiaProp2 = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getSubTitle());
		assertEquals("Chart title is wrong", "TestTitle",
				aDiaProp.getPropertyValue("String"));
		assertEquals("Chart sub title is wrong", "TestSubTitle",
				aDiaProp2.getPropertyValue("String"));
		// modify title and subtitle
		aDiaProp.setPropertyValue("String", "AnotherTestTitle");
		aDiaProp2.setPropertyValue("String", "AnotherTestSubTitle");

		// close document and save modification.
		XModifiable modified = (XModifiable) UnoRuntime.queryInterface(
				XModifiable.class, impressDocument);
		XCloseable closer = (XCloseable) UnoRuntime.queryInterface(
				XCloseable.class, impressDocument);
		if (modified != null)
			modified.setModified(true);
		closer.close(true);

		// load and get Chart
		impressDocument = (XComponent) UnoRuntime.queryInterface(XComponent.class,
				unoApp.loadDocument(filePath));
		xShapes = ShapeUtil.getShapes(reLoadFile, 1);
		xShape = (XShape) UnoRuntime.queryInterface(XShape.class,
				xShapes.getByIndex(0));
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getTitle());
		aDiaProp2 = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getSubTitle());
		assertEquals("Chart title is wrong", "AnotherTestTitle",
				aDiaProp.getPropertyValue("String"));
		assertEquals("Chart sub title is wrong", "AnotherTestSubTitle",
				aDiaProp2.getPropertyValue("String"));
	}

	/**
	 * test Data label and show it as value
	 * @throws Exception
	 */
	@Test
	public void testDataLabel() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		// set data label to the fourth points, the second series.
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getDiagram()
						.getDataPointProperties(3, 1));
		aDiaProp.setPropertyValue("DataCaption", ChartDataCaption.VALUE);

		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getDiagram().getDataPointProperties(3, 1));

		assertEquals(
				"Data label of fourth points, the second series not show as value",
				ChartDataCaption.VALUE,
				aDiaProp.getPropertyValue("DataCaption"));

	}

	/**
	 * test GridLine of Y Axis.
	 * @throws Exception
	 */
	@Test
	public void testGridLine() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		XDiagram xdiagram = xChartDoc.getDiagram();
		// get Y axis
		XAxisYSupplier aYAxisSupplier = (XAxisYSupplier) UnoRuntime
				.queryInterface(XAxisYSupplier.class, xdiagram);
		// get Y axis's gridline
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, aYAxisSupplier.getYMainGrid());
		aDiaProp.setPropertyValue("LineColor", 0xffff00);

		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		xdiagram = xChartDoc.getDiagram();
		aYAxisSupplier = (XAxisYSupplier) UnoRuntime.queryInterface(
				XAxisYSupplier.class, xdiagram);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				aYAxisSupplier.getYMainGrid());
		assertEquals("the Y grid line color is not yellow", 0xffff00,
				aDiaProp.getPropertyValue("LineColor"));
	}

	/**
	 * test Wall Fill properties 
	 * @throws Exception
	 */
	@Test
	public void testWall() throws Exception {
		XShape xShape = null;
		insertDefaultChart2();
		com.sun.star.chart2.XDiagram xDiagram2 = xChart2Doc.getFirstDiagram();
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xDiagram2.getWall());
		aDiaProp.setPropertyValue("FillStyle", FillStyle.SOLID);
		aDiaProp.setPropertyValue("FillColor", 0xffff00);
		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChart2Doc = ChartUtil.getChart2Document(xShape);
		xDiagram2 = xChart2Doc.getFirstDiagram();
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xDiagram2.getWall());
		assertEquals("the Chart wall is not yellow", 0xffff00,
				aDiaProp.getPropertyValue("FillColor"));
	}

	/**
	 * Save presentation and reLoad the presentation and shape in it.
	 * 
	 * @param po
	 * @param shapeType
	 * @return
	 * @throws Exception
	 */
	public XShape saveAndLoadShape(int pageIndex, int shapeIndex)
			throws Exception {
		reLoadFile = saveAsAndReloadDoc(impressDocument, "impress8", "odp");
		xShapes = ShapeUtil.getShapes(reLoadFile, pageIndex);
		return (XShape) UnoRuntime.queryInterface(XShape.class,
				xShapes.getByIndex(shapeIndex));
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
	private XComponent saveAsAndReloadDoc(XComponent presentationDocument,
			String sFilter, String sExtension) throws Exception {
		filePath = Testspace.getPath("tmp/chartproperties." + sExtension);
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
