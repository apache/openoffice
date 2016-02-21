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
import static org.junit.Assert.*;
import static testlib.uno.GraphicUtil.getSizePixelOfGraphicFile;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.openoffice.test.uno.UnoApp;
import org.openoffice.test.common.FileUtil;
import org.openoffice.test.common.Testspace;

import com.sun.star.beans.Property;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.beans.XPropertySet;
import com.sun.star.beans.XPropertySetInfo;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XShape;
import com.sun.star.drawing.XShapes;
import com.sun.star.graphic.XGraphic;
import com.sun.star.graphic.XGraphicObject;
import com.sun.star.graphic.XGraphicProvider;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XServiceName;
import com.sun.star.uno.Exception;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.awt.Point;
import com.sun.star.awt.Size;
//import com.sun.star.uno.AnyConverter;

public class GraphicUtil {
	
	public static String getUniqueIDbyXGraphic(UnoApp unoApp, XGraphic xgraphic) throws Exception{
		Object graphicObj = unoApp.getServiceFactory().createInstance("com.sun.star.graphic.GraphicObject");
		XGraphicObject xgraphicObj = (XGraphicObject)UnoRuntime.queryInterface(XGraphicObject.class, graphicObj);
		xgraphicObj.setGraphic(xgraphic);
		
		return xgraphicObj.getUniqueID();
	}
	
//	String sUrl = "file:///F:/work/36.gif";
	public static String getUniqueIDOfGraphicFile(UnoApp unoApp, String sUrl) throws Exception{		
		XGraphic xgraphic = getXGraphicOfGraphicFile(unoApp,sUrl);		
		String uniqueID = getUniqueIDbyXGraphic(unoApp, xgraphic);
		
		return uniqueID;
	}
	
	public static XGraphic getXGraphicOfGraphicFile(UnoApp unoApp, String sUrl) throws Exception
	{
		Object graphicObj = unoApp.getServiceFactory().createInstance("com.sun.star.graphic.GraphicProvider");
		XGraphicProvider xgraphicProvider = (XGraphicProvider)UnoRuntime.queryInterface(XGraphicProvider.class, graphicObj);
		PropertyValue[] sourceProps = new PropertyValue[1];
		sourceProps[0]       = new PropertyValue();
		sourceProps[0].Name  = "URL";
		sourceProps[0].Value = sUrl;
		return xgraphicProvider.queryGraphic(sourceProps);		
	}
	
	public static Size getSize100thMMOfGraphicFile(UnoApp unoApp, String sUrl) throws Exception
	{
		Object graphicObj = unoApp.getServiceFactory().createInstance("com.sun.star.graphic.GraphicProvider");
		XGraphicProvider xgraphicProvider = (XGraphicProvider)UnoRuntime.queryInterface(XGraphicProvider.class, graphicObj);
		PropertyValue[] sourceProps = new PropertyValue[1];
		sourceProps[0]       = new PropertyValue();
		sourceProps[0].Name  = "URL";
		sourceProps[0].Value = sUrl;
		XPropertySet xGraphicPro = xgraphicProvider.queryGraphicDescriptor(sourceProps);		
		Size size = (Size)xGraphicPro.getPropertyValue("Size100thMM");
		return size;
	}
	
	public static Size getSizePixelOfGraphicFile(UnoApp unoApp, String sUrl) throws Exception
	{
		Object graphicObj = unoApp.getServiceFactory().createInstance("com.sun.star.graphic.GraphicProvider");
		XGraphicProvider xgraphicProvider = (XGraphicProvider)UnoRuntime.queryInterface(XGraphicProvider.class, graphicObj);
		PropertyValue[] sourceProps = new PropertyValue[1];
		sourceProps[0]       = new PropertyValue();
		sourceProps[0].Name  = "URL";
		sourceProps[0].Value = sUrl;
		XPropertySet xGraphicPro = xgraphicProvider.queryGraphicDescriptor(sourceProps);
		Size size = (Size)xGraphicPro.getPropertyValue("SizePixel");
		return size;
	}
	
	
	//GraphicObjectShape
	public static Object[] getGraphicsOfPage(XDrawPage xDrawPage) throws Exception {
		XShapes m_xdrawShapes = (XShapes) UnoRuntime.queryInterface(XShapes.class, xDrawPage);
		int count = m_xdrawShapes.getCount();
		Object[] temp = new Object[count];
		int graphicNum=0;
		for(int i=0;i<count; i++)
		{
			Object shape = m_xdrawShapes.getByIndex(i);
			XShape xshape = (XShape)UnoRuntime.queryInterface(XShape.class, shape);		
			String type = xshape.getShapeType();
			if(type.equals("com.sun.star.drawing.GraphicObjectShape"))
			{
				temp[graphicNum] = shape;
				graphicNum++;				
			}
		}
		
		Object[] graphics = new Object[graphicNum];
		System.arraycopy(temp, 0, graphics, 0, graphicNum);
		return graphics;
	}
	
	/*Insert a graphic into a Impress
	 * component: the Impress document
	 * toPage: the page that the graphic will be inserted to
	 * graphicURL: the file path of the graphic, e.g. file:///c:/test.png
	 * size: size of the graphic to be inserted
	 * position: position of the graphic to be inserted
	 * */
	public static void insertGraphic(XComponent component, XDrawPage toPage, String graphicURL, Size size, Point position) throws Exception
	{		
		XMultiServiceFactory xDrawFactory = 
	            (XMultiServiceFactory)UnoRuntime.queryInterface(
	                XMultiServiceFactory.class, component);

	    Object oGraphic = xDrawFactory.createInstance("com.sun.star.drawing.GraphicObjectShape");
	    XPropertySet xGraphicPro = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class, oGraphic);
	    xGraphicPro.setPropertyValue("GraphicURL", graphicURL);
	    
	    XShape xDrawShape = (XShape)UnoRuntime.queryInterface(XShape.class, oGraphic);
	    
	    xDrawShape.setSize(size); 
	    xDrawShape.setPosition(position);

	    toPage.add(xDrawShape);
	}
	
}
