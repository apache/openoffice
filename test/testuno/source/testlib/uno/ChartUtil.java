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
package testlib.uno;

import com.sun.star.beans.XPropertySet;
import com.sun.star.chart.XChartDocument;
import com.sun.star.chart.XDiagram;
import com.sun.star.drawing.XShape;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;

public class ChartUtil {

	/**
	 * Retrieve Chart document as model of the OLE Shape(use to create chart)
	 * @param xShape
	 * @return
	 * @throws Exception
	 */
	public static XChartDocument retrieveChartDocument(XShape xShape)throws Exception{
		XChartDocument aChartDoc = null;
		final String msChartClassID = "12dcae26-281f-416f-a234-c3086127382e";
		// make the OLE shape a chart
		XPropertySet aShapeProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xShape);
		// set the class id for charts
		aShapeProp.setPropertyValue("CLSID", msChartClassID);
		// retrieve the chart document as model of the OLE shape
		aChartDoc = (XChartDocument) UnoRuntime.queryInterface(
				XChartDocument.class, aShapeProp.getPropertyValue("Model"));
		return aChartDoc;
	}
	
	/**
	 * Create Chart in ChartDocument.
	 * @param aChartDoc
	 * @param ChartType
	 * @return
	 * @throws Exception
	 */
	public static XDiagram createChart(XChartDocument aChartDoc, String ChartType)
			throws Exception {
	
		// let aChartDoc be a valid XChartDocument
		// get the factory that can create diagrams
		XMultiServiceFactory aFact = (XMultiServiceFactory) UnoRuntime
				.queryInterface(XMultiServiceFactory.class, aChartDoc);
		XDiagram aDiagram = (XDiagram) UnoRuntime.queryInterface(
				XDiagram.class, aFact.createInstance(ChartType));
		return aDiagram;
	}
	
	
	/**
	 * Get Chart Doc from a Shape
	 * @param xShape
	 * @return
	 * @throws Exception
	 */
	public static XChartDocument getChartDocument(XShape xShape)throws Exception{
		XChartDocument aChartDoc = null;
		XPropertySet aShapeProp = (XPropertySet) UnoRuntime.queryInterface(
				XPropertySet.class, xShape);
		// retrieve the chart document as model of the OLE shape
		aChartDoc = (XChartDocument) UnoRuntime.queryInterface(
				XChartDocument.class, aShapeProp.getPropertyValue("Model"));
		return aChartDoc;
		
	}
}
