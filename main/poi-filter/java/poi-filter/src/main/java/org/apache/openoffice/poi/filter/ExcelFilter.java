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
package org.apache.openoffice.poi.filter;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.URI;
import java.net.URISyntaxException;

import org.apache.poi.xssf.streaming.SXSSFCell;
import org.apache.poi.xssf.streaming.SXSSFRow;
import org.apache.poi.xssf.streaming.SXSSFSheet;
import org.apache.poi.xssf.streaming.SXSSFWorkbook;

import com.sun.star.beans.PropertyValue;
import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XNamed;
import com.sun.star.document.XExporter;
import com.sun.star.document.XFilter;
import com.sun.star.io.XOutputStream;
import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.lang.IndexOutOfBoundsException;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.lib.uno.adapter.XOutputStreamToOutputStreamAdapter;
import com.sun.star.lib.uno.helper.ComponentBase;
import com.sun.star.sheet.XCellRangeAddressable;
import com.sun.star.sheet.XSheetCellCursor;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.sheet.XUsedAreaCursor;
import com.sun.star.table.CellContentType;
import com.sun.star.table.CellRangeAddress;
import com.sun.star.table.XCell;
import com.sun.star.text.XTextRange;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;

public class ExcelFilter extends ComponentBase implements XServiceInfo, XExporter, XFilter {
    private static String[] services = new String[] {
            "com.sun.star.document.ExportFilter"
    };
    private XComponentContext componentContext;
    private XComponent document;

    public ExcelFilter(XComponentContext componentContext) {
        this.componentContext = componentContext;
    }

    public static String[] getSupportedServiceNamesStatic() {
        return services.clone();
    }

    // XComponent:
    
    @Override
    protected synchronized void postDisposing() {
        componentContext = null;
    }

    // XServiceInfo:

    @Override
    public String getImplementationName() {
        System.out.println("getImplementationName()");
        return getClass().getName();
    }
    
    @Override
    public String[] getSupportedServiceNames() {
        System.out.println("getSupportedServiceNames()");
        return getSupportedServiceNamesStatic();
    }
    
    @Override
    public boolean supportsService(final String serviceName) {
        System.out.println("supportsService(" + serviceName + ")");
        for (String service : getSupportedServiceNames()) {
            if (service.equals(serviceName)) {
                return true;
            }
        }
        return false;
    }


    // XExporter

    @Override
    public void setSourceDocument(final XComponent document) throws IllegalArgumentException {
        System.out.println("setSourceDocument()");
        this.document = document;
    }


    // XFilter

    @Override
    public void cancel() {
        System.out.println("cancel()");
    }

    @Override
    public boolean filter(final PropertyValue[] properties) {
        System.out.println("filter!!!");
        try (PrintWriter pw = new PrintWriter("/tmp/log.txt")) {
            pw.println("Filter called!!!");
            pw.flush();
            for (final PropertyValue property : properties) {
                pw.println(property.Name + "=" + property.Value.toString());
            }            
        } catch (FileNotFoundException fileNotFoundException) {
        }

        MediaDescriptor mediaDescriptor = new MediaDescriptor(properties);
        try {
            System.out.println("Starting");
            SXSSFWorkbook poiWorkbook = new SXSSFWorkbook(100);
            try {
                System.out.println("Created POI workbook");
                XSpreadsheetDocument spreadSheetDocument = UnoRuntime.queryInterface(XSpreadsheetDocument.class, document);
                if (spreadSheetDocument == null) {
                    return false;
                }
                XSpreadsheets spreadsheets = spreadSheetDocument.getSheets();
                XIndexAccess spreadsheetsIndexAccess = UnoRuntime.queryInterface(XIndexAccess.class, spreadsheets);
                for (int i = 0; i < spreadsheetsIndexAccess.getCount(); i++) {
                    System.out.println("Saving sheet " + i);
                    XSpreadsheet spreadsheet = (XSpreadsheet) AnyConverter.toObject(XSpreadsheet.class, spreadsheetsIndexAccess.getByIndex(i));
                    XNamed named = UnoRuntime.queryInterface(XNamed.class, spreadsheet);
                    SXSSFSheet poiSheet = poiWorkbook.createSheet(named.getName());
                    saveSheet(spreadsheet, poiSheet);
                }
                
                // FIXME: other ways of specifying where to save the file?
                Object unoOutputStreamObject = mediaDescriptor.get(MediaDescriptor.OutputStream);
                OutputStream javaOutputStream = null;
                try {
                    if (unoOutputStreamObject != null) {
                        // "If used when storing a document: writing must be done using this stream"
                        System.out.println("Using output stream");
                        XOutputStream unoOutputStream = (XOutputStream) AnyConverter.toObject(XOutputStream.class, unoOutputStreamObject);
                        javaOutputStream = new XOutputStreamToOutputStreamAdapter(unoOutputStream); 
                    } else {
                        String urlString = mediaDescriptor.getUnpackedValueOrDefault(MediaDescriptor.URL, (String)null);
                        if (urlString != null) {
                            URI uri = new URI(urlString);
                            File file = new File(uri);
                            System.out.println("Using file " + file.getAbsolutePath());
                            javaOutputStream = new FileOutputStream(file);
                        } else {
                            throw new IOException("Unsupported data source!");
                        }
                    }
                    poiWorkbook.write(javaOutputStream);
                } finally {
                    if (javaOutputStream != null) {
                        javaOutputStream.close();
                    }
                }
            } finally {
                poiWorkbook.dispose();
            }
            System.out.println("Success!");
            return true;
        } catch (WrappedTargetException wrappedTargetException) {
            wrappedTargetException.printStackTrace();
        } catch (FileNotFoundException fileNotFoundException) {
            fileNotFoundException.printStackTrace();
        } catch (IOException ioException) {
            ioException.printStackTrace();
        } catch (URISyntaxException uriSyntaxException) {
            uriSyntaxException.printStackTrace();
        } catch (IndexOutOfBoundsException e) {
            e.printStackTrace();
        } catch (Throwable t) {
            t.printStackTrace();
        }
        return false;
    }
    
    private void saveSheet(XSpreadsheet spreadsheet, SXSSFSheet poiSheet) {
        XSheetCellCursor cursor = spreadsheet.createCursor();
        XUsedAreaCursor usedAreaCursor = UnoRuntime.queryInterface(XUsedAreaCursor.class, cursor);
        XCellRangeAddressable cellRangeAddressable = UnoRuntime.queryInterface(XCellRangeAddressable.class, cursor);
        usedAreaCursor.gotoEndOfUsedArea(true);
        CellRangeAddress usedArea = cellRangeAddressable.getRangeAddress();
        // FIXME: narrow down the cells to save further using ScUsedAreaIterator
        
        for (int row = usedArea.StartRow; row <= usedArea.EndRow; row++) {
            boolean hasData = false;
            for (int col = usedArea.StartColumn; col <= usedArea.EndColumn; col++) {
                try {
                    XCell cell = spreadsheet.getCellByPosition(col, row);
                    if (cell.getType() != CellContentType.EMPTY) {
                        hasData = true;
                        break;
                    }
                } catch (IndexOutOfBoundsException indexOutOfBoundsException) {
                    indexOutOfBoundsException.printStackTrace();
                }
            }
            if (!hasData) {
                continue;
            }
            
            SXSSFRow poiRow = poiSheet.createRow(row);
            System.out.println("Creating row " + row);
            for (int col = usedArea.StartColumn; col <= usedArea.EndColumn; col++) {
                try {
                    XCell cell = spreadsheet.getCellByPosition(col, row);
                    if (cell.getType() == CellContentType.TEXT) {
                        XTextRange textRange = UnoRuntime.queryInterface(XTextRange.class, cell);
                        SXSSFCell poiCell = poiRow.createCell(col);
                        System.out.println("Adding cell " + col);
                        poiCell.setCellValue(textRange.getString());
                    } else if (cell.getType() == CellContentType.VALUE) {
                        SXSSFCell poiCell = poiRow.createCell(col);
                        System.out.println("Adding cell " + col);
                        poiCell.setCellValue(cell.getValue());
                    }
                } catch (IndexOutOfBoundsException indexOutOfBoundsException) {
                    indexOutOfBoundsException.printStackTrace();
                }
            }   
        }
    }
}
