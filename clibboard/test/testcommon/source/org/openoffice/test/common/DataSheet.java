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
package org.openoffice.test.common;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

/**
 * The class can be used to output data into an Microsoft Excel 2003 XML file.
 * The file can be opened with OpenOffice or MS Office.
 *
 */
public class DataSheet {
	private File file = null;
	private Document doc = null;
	private Element workBookEl = null;
	private static final SimpleDateFormat DATEFORMAT = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS");
	
	public DataSheet(File file) {
		this(file, false);
	}
	
	public DataSheet(File file, boolean append) {
		this.file = file;
		
		if (append && file.exists()) {
			doc = FileUtil.parseXML(file.getAbsolutePath());
			if (doc != null) {
				NodeList nodes = doc.getElementsByTagName("Workbook");
				if (nodes.getLength() > 0) {
					workBookEl = (Element) nodes.item(0);
					return;
				}
			}
		} 
		
		doc = FileUtil.newXML();
		workBookEl = doc.createElement("Workbook");
		workBookEl.setAttribute("xmlns", "urn:schemas-microsoft-com:office:spreadsheet");
		workBookEl.setAttribute("xmlns:ss", "urn:schemas-microsoft-com:office:spreadsheet");
		doc.appendChild(workBookEl);
//		Element stylesEl = doc.createElement("Styles");
//		workBookEl.appendChild(stylesEl);
//		Element styleEl = doc.createElement("Style");
//		stylesEl.appendChild(styleEl);
//		styleEl.setAttribute("ss:ID", "ce1");
//		Element numberFormatEl = doc.createElement("NumberFormat");
//		styleEl.appendChild(numberFormatEl);
//		numberFormatEl.setAttribute("ss:Format", "General Date");
		
	}
	
	
	private Element getTableElement(String sheetName) {
		NodeList nodes = workBookEl.getElementsByTagName("Worksheet");
		for (int i = 0; i < nodes.getLength(); i++) {
			Element e = (Element) nodes.item(0);
			if (sheetName.equals(e.getAttribute("ss:Name"))){
				return (Element) e.getElementsByTagName("Table").item(0);
			}
		}
		
		Element worksheetEl = doc.createElement("Worksheet");
		worksheetEl.setAttribute("ss:Name", sheetName);
		workBookEl.appendChild(worksheetEl);
		Element tableEl = doc.createElement("Table");
		worksheetEl.appendChild(tableEl);
		return tableEl;
	}
	
	public void addRow(String sheetName, Object... datas) {
		Element tableEl = getTableElement(sheetName);
		Element rowEl = doc.createElement("Row");
		for (Object o : datas) {
			Element cellEl = doc.createElement("Cell");
			rowEl.appendChild(cellEl);
			Element dataEl = doc.createElement("Data");
			cellEl.appendChild(dataEl);
			if (o instanceof Number) {
				dataEl.setAttribute("ss:Type", "Number");
				dataEl.setTextContent(o.toString());
			} else if (o instanceof Boolean) {
				dataEl.setAttribute("ss:Type", "Boolean");
				dataEl.setTextContent(o.toString());
			} else if (o instanceof Date) {
				dataEl.setAttribute("ss:Type", "String");
				dataEl.setTextContent(DATEFORMAT.format((Date) o));
			} else {
				dataEl.setAttribute("ss:Type", "String");
				dataEl.setTextContent(o == null ? "" : o.toString());
			}
		}
		
		tableEl.appendChild(rowEl);
		FileUtil.storeXML(doc, file);
	}
}
