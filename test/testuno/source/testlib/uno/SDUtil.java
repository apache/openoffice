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

import java.util.HashMap;

import org.openoffice.test.common.Testspace;

import com.sun.star.beans.PropertyValue;
import com.sun.star.container.XIndexAccess;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XDrawPagesSupplier;
import com.sun.star.drawing.XShapes;
import com.sun.star.frame.XStorable;
import com.sun.star.lang.XComponent;
import com.sun.star.uno.UnoRuntime;

/**
 * 
 *
 */
public class SDUtil {

	private static HashMap filterName = new HashMap(); 
	
	private SDUtil() {

	}

	public static Object getPageByIndex(XComponent doc, int index) throws Exception {
		XDrawPagesSupplier xDrawPagesSupplier = (XDrawPagesSupplier) UnoRuntime.queryInterface(XDrawPagesSupplier.class, doc);
		Object drawPages = xDrawPagesSupplier.getDrawPages();
		XIndexAccess xIndexedDrawPages = (XIndexAccess) UnoRuntime.queryInterface(XIndexAccess.class, drawPages);
		return xIndexedDrawPages.getByIndex(index);
	}

	public static Object getShapeOfPageByIndex(Object page, int index) throws Exception {
		XDrawPage xDrawPage = (XDrawPage) UnoRuntime.queryInterface(XDrawPage.class, page);
		XShapes m_xdrawShapes = (XShapes) UnoRuntime.queryInterface(XShapes.class, xDrawPage);
		return m_xdrawShapes.getByIndex(index);
	}
	
	public static void saveFileAs(XComponent sdComponent, String fileName, String extName) throws Exception {
		
		initFilterName();

		String storeUrl = Testspace.getUrl("temp/" + fileName + "." + extName);
		
		PropertyValue[] storeProps = new PropertyValue[2];
		storeProps[0] = new PropertyValue();
		storeProps[0].Name = "FilterName";
		storeProps[0].Value = filterName.get(extName);
		storeProps[1] = new PropertyValue();
		storeProps[1].Name = "Overwrite";
		storeProps[1].Value = new Boolean(true);
		
		XStorable sdStorable = 
				(XStorable) UnoRuntime.queryInterface(XStorable.class, sdComponent);
		sdStorable.storeAsURL(storeUrl, storeProps);
	}
	
	private static void initFilterName() throws Exception {
		if (filterName.size() > 0) {
			return;
		}
		
		filterName.put("odp", "impress8");
		filterName.put("ppt", "MS PowerPoint 97");
	}

}
