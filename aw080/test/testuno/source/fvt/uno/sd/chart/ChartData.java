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
package fvt.uno.sd.chart;

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
import com.sun.star.awt.Gradient;
import com.sun.star.awt.GradientStyle;
import com.sun.star.awt.Point;
import com.sun.star.awt.Size;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.chart.ChartDataCaption;
import com.sun.star.chart.ChartLegendPosition;
import com.sun.star.chart.XAxisYSupplier;
import com.sun.star.chart.XChartData;
import com.sun.star.chart.XChartDataArray;
import com.sun.star.chart.XChartDocument;
import com.sun.star.chart.XDiagram;
import com.sun.star.chart2.data.XDataSource;
import com.sun.star.chart2.data.XLabeledDataSequence;
import com.sun.star.drawing.DashStyle;
import com.sun.star.drawing.FillStyle;
import com.sun.star.drawing.LineDash;
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
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.XCloseable;
import com.sun.star.util.XModifiable;

public class ChartData {
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
	 * test data series 
	 * @throws Exception
	 */
	@Test
	public void testDataSeries() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		// get the second data series.
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getDiagram()
						.getDataRowProperties(1));
		aDiaProp.setPropertyValue("FillStyle", FillStyle.SOLID);
		aDiaProp.setPropertyValue("FillColor", 0xffff00);
		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getDiagram().getDataRowProperties(1));
		assertEquals("the second DataSeries fill color isn't yellow", 0xffff00,
				aDiaProp.getPropertyValue("FillColor"));

	}
	/**
	 * test data point
	 * @throws Exception
	 */
	@Test
	public void testDataPoint() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		// set data label to the fourth points, the second series.
		XPropertySet aDiaProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xChartDoc.getDiagram()
						.getDataPointProperties(3, 1));
		aDiaProp.setPropertyValue("FillStyle", FillStyle.SOLID);
		aDiaProp.setPropertyValue("FillColor", 0xffff00);
		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		aDiaProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class,
				xChartDoc.getDiagram().getDataPointProperties(3, 1));

		assertEquals(
				"the fourth point of the second series's fill color isn't yellow",
				0xffff00, aDiaProp.getPropertyValue("FillColor"));

	}
	/**
	 * test data of chart, update it with chart
	 * @throws Exception
	 */
	@Test
	public void testDataTable() throws Exception {
		XShape xShape = null;
		insertDefaultChart();
		// get data object from chart
		XChartDataArray array = (XChartDataArray) UnoRuntime.queryInterface(
				XChartDataArray.class, xChartDoc.getData());

		// set new chart data
		double[][] data = new double[][] { { 2, 2 }, { 2, 2 }, { 2, 2 } };
		array.setData(data);
		// update chart with data
		xChartDoc.attachData(xChartDoc.getData());
		// --------------------------
		xShape = saveAndLoadShape(1, 0);
		xChartDoc = ChartUtil.getChartDocument(xShape);
		array = (XChartDataArray) UnoRuntime.queryInterface(
				XChartDataArray.class, xChartDoc.getData());
		data = array.getData();
		assertEquals(2, data[0][0], 0.0);

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
		filePath = Testspace.getPath("tmp/chartdata." + sExtension);
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
