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


package complex.sc;

import com.sun.star.beans.Property;
import com.sun.star.beans.PropertyAttribute;
import com.sun.star.beans.PropertyVetoException;
import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.beans.XPropertySet;
import com.sun.star.beans.XPropertySetInfo;
import com.sun.star.container.XIndexAccess;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XDrawPages;
import com.sun.star.drawing.XDrawPagesSupplier;
import com.sun.star.drawing.XShape;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sheet.XCellRangeAddressable;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
// import com.sun.star.table.XCell;
import com.sun.star.uno.Any;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.XCloseable;

// import java.io.PrintWriter;

import util.DrawTools;
import util.SOfficeFactory;
import util.ValueComparer;

// import org.junit.After;
import org.junit.AfterClass;
// import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.openoffice.test.OfficeConnection;
import static org.junit.Assert.*;


public class CalcRTL
{
    XSpreadsheetDocument xSheetDoc = null;

    /*
     * In this method a spreadsheet document is opened<br>
     * afterwards all properties of the Spreadsheet are checked.<br>
     * <p>
     * These are
     * <ul>
     *<li> IsVisible
     *<li> PageStyle
     *<li> TableLayout
     *</ul>
     */
    @Test public void checkSpreadsheetProperties() {
        assertTrue("Couldn't open document", openSpreadsheetDocument());

        XPropertySet set =  UnoRuntime.queryInterface(
                                   XPropertySet.class, getSpreadsheet());
        assertTrue("Problems when setting property 'IsVisible'",
               changeProperty(set, "IsVisible", Boolean.FALSE));
        assertTrue("Problems when setting property 'IsVisible'",
               changeProperty(set, "IsVisible", Boolean.TRUE));
        assertTrue("Problems when setting property 'PageStyle'",
               changeProperty(set, "PageStyle", "Report"));
        assertTrue("Problems when setting property 'PageStyle'",
               changeProperty(set, "PageStyle", "Default"));
        assertTrue("Problems when setting property 'TableLayout'",
               changeProperty(set, "TableLayout", 
                              new Short(com.sun.star.text.WritingMode2.RL_TB)));
        assertTrue("Problems when setting property 'TableLayout'",
               changeProperty(set, "TableLayout", 
                              new Short(com.sun.star.text.WritingMode2.LR_TB)));
        assertTrue("Couldn't close document", closeSpreadsheetDocument());
    }

    /*
     * In this method a spreadsheet document is opened and a shape inserted<br>
     * afterwards all calc specific properties of the Shape are checked.<br>
     * <p>
     * These are
     * <ul>
     *<li> Anchor
     *<li> HoriOrientPosition
     *<li> VertOrientPosition
     *</ul>
     */
    @Test public void checkShapeProperties() {
        assertTrue("Couldn't open document", openSpreadsheetDocument());

        XPropertySet set =  UnoRuntime.queryInterface(
                                   XPropertySet.class, getInsertedShape());

        try {
            assertTrue("Problems when setting property 'Anchor'",
                   changeProperty(set, "Anchor", 
                                  getSpreadsheet().getCellByPosition(5, 5)));
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            // assure("Problems when setting property 'Anchor'", false);
            fail("Problems when setting property 'Anchor'");
        }

        assertTrue("Problems when setting property 'HoriOrientPosition'",
               changeProperty(set, "HoriOrientPosition", new Integer(1000)));

        assertTrue("Problems when setting property 'VertOrientPosition'",
               changeProperty(set, "VertOrientPosition", new Integer(1000)));

        assertTrue("Couldn't close document", closeSpreadsheetDocument());
    }

    @Test public void checkInfluenceOfSpreadsheetChange() {
        assertTrue("Couldn't open document", openSpreadsheetDocument());

        XShape oShape = getInsertedShape();
        XPropertySet sheetProps = UnoRuntime.queryInterface(
                                          XPropertySet.class, getSpreadsheet());
        XPropertySet shapeProps = UnoRuntime.queryInterface(
                                          XPropertySet.class, oShape);

        String[] previous = getShapeProps(shapeProps, oShape);
        assertTrue("Problems when setting property 'TableLayout'",
               changeProperty(sheetProps, "TableLayout", 
                              new Short(com.sun.star.text.WritingMode2.RL_TB)));

        String[] RL_TB = getShapeProps(shapeProps, oShape);
        assertTrue("Problems when setting property 'TableLayout'",
               changeProperty(sheetProps, "TableLayout", 
                              new Short(com.sun.star.text.WritingMode2.LR_TB)));

        String[] LR_TB = getShapeProps(shapeProps, oShape);
        assertTrue("Anchor has changed",
               (previous[0].equals(RL_TB[0]) && previous[0].equals(LR_TB[0])));
        assertEquals("HoriOrientPosition has changed",
               2100, (Integer.valueOf(previous[1]).intValue() + Integer.valueOf(
                                                                  RL_TB[1])
                                                                 .intValue() + 
                   Integer.valueOf(LR_TB[1]).intValue()));
        assertEquals("VertOrientPosition has changed",
                3*Integer.valueOf(previous[2]).intValue(),
               (Integer.valueOf(previous[2]).intValue() + Integer.valueOf(
                                                                  RL_TB[2])
                                                                 .intValue() + 
                   Integer.valueOf(LR_TB[2]).intValue()));
        assertTrue("x-position hasn't changed",
               (previous[3].equals(LR_TB[3]) && 
                   ((Integer.valueOf(previous[3]).intValue() * (-1)) - 
                       oShape.getSize().Width != Integer.valueOf(LR_TB[2])
                                                        .intValue())));
        assertTrue("Couldn't close document", closeSpreadsheetDocument());
    }

    protected String[] getShapeProps(XPropertySet set, XShape oShape) {
        String[] reValue = new String[4];

        try {
            reValue[0] = toString(getRealValue(set.getPropertyValue("Anchor")));
            reValue[1] = toString(set.getPropertyValue("HoriOrientPosition"));
            reValue[2] = toString(set.getPropertyValue("VertOrientPosition"));
            reValue[3] = toString(new Integer(oShape.getPosition().X));
        } catch (com.sun.star.beans.UnknownPropertyException e) {
        } catch (com.sun.star.lang.WrappedTargetException e) {
        }

        System.out.println("\t Anchor :" + reValue[0]);
        System.out.println("\t HoriOrientPosition :" + reValue[1]);
        System.out.println("\t VertOrientPosition :" + reValue[2]);
        System.out.println("\t Shape Position (x,y) : (" + oShape.getPosition().X +
                    "," + oShape.getPosition().Y + ")");

        return reValue;
    }

    /*
     * this method opens a calc document and sets the corresponding class variable xSheetDoc
     */
    protected boolean openSpreadsheetDocument() {
        // SOfficeFactory SOF = SOfficeFactory.getFactory(
        //                             (XMultiServiceFactory) param.getMSF());

        final XMultiServiceFactory xMsf = UnoRuntime.queryInterface(XMultiServiceFactory.class, connection.getComponentContext().getServiceManager());
        SOfficeFactory SOF = SOfficeFactory.getFactory(xMsf);

        boolean worked = true;

        try {
            System.out.println("creating a sheetdocument");
            xSheetDoc = SOF.createCalcDoc(null);
        } catch (com.sun.star.uno.Exception e) {
            // Some exception occured.FAILED
            worked = false;
            e.printStackTrace(/*(PrintWriter) log*/ System.out);
        }

        return worked;
    }

    /*
     * this method closes a calc document and resets the corresponding class variable xSheetDoc
     */
    protected boolean closeSpreadsheetDocument() {
        boolean worked = true;

        System.out.println("    disposing xSheetDoc ");

        try {
            XCloseable oCloser =  UnoRuntime.queryInterface(
                                         XCloseable.class, xSheetDoc);
            oCloser.close(true);
        } catch (com.sun.star.util.CloseVetoException e) {
            worked = false;
            System.out.println("Couldn't close document");
        } catch (com.sun.star.lang.DisposedException e) {
            worked = false;
            System.out.println("Document already disposed");
        } catch (java.lang.NullPointerException e) {
            worked = false;
            System.out.println("Couldn't get XCloseable");
        }

        xSheetDoc = null;

        return worked;
    }

    /*
     * This method gets the first Sheet of the SpreadsheetDocument
     *
     */
    protected XSpreadsheet getSpreadsheet() {
        XSpreadsheet oSheet = null;

        System.out.println("getting sheets");

        XSpreadsheets xSpreadsheets = xSheetDoc.getSheets();

        System.out.println("getting a sheet");

        XIndexAccess oIndexAccess =  UnoRuntime.queryInterface(
                                            XIndexAccess.class, xSpreadsheets);

        try {
            oSheet =  UnoRuntime.queryInterface(
                             XSpreadsheet.class, oIndexAccess.getByIndex(0));
        } catch (com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(System.out);
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            e.printStackTrace(System.out);
        }

        return oSheet;
    }

    /*
     * This method changes a given property to a given value
     * Parameters:
     * @param set : The XPropertySet that contains the properties
     * @param pName : The name of the Property that has to be changed
     * @param pValue : The value, the property should be changed too.
     */
    protected boolean changeProperty(XPropertySet set, String pName, 
                                     Object pValue) {
        boolean worked = true;

        try {
            Object oldValue = set.getPropertyValue(pName);

            // for an exception thrown during setting new value
            // to pass it to checkResult method
            Exception exception = null;

            try {
                set.setPropertyValue(pName, pValue);
            } catch (IllegalArgumentException e) {
                exception = e;
            } catch (PropertyVetoException e) {
                exception = e;
            } catch (WrappedTargetException e) {
                exception = e;
            } catch (UnknownPropertyException e) {
                exception = e;
            } catch (RuntimeException e) {
                exception = e;
            }

            // getting result value
            Object resValue = set.getPropertyValue(pName);


            // checking results
            worked = checkResult(set, pName, oldValue, pValue, resValue, 
                                 exception);
        } catch (Exception e) {
            System.out.println("Exception occurred while testing property '" + 
                               pName + "'");
            e.printStackTrace();
            worked = false;
        }

        return worked;
    }

    /*
     * This methods checks if a property has changed as expected
     * Parameters:
     * @param set : the given XPropertySet
     * @param propName : the name of the property that has been changed
     * @param oldValue : the value of the property before it has been changed
     * @param newValue : the value the property has been set to
     * @param resValue : the value getPropertyValue returned for the property
     * @param exception : the exeption thrown during the change of the property
     */
    protected boolean checkResult(XPropertySet set, String propName, 
                                  Object oldValue, Object newValue, 
                                  Object resValue, Exception exception)
                           throws Exception {
        XPropertySetInfo info = set.getPropertySetInfo();
        Property prop = info.getPropertyByName(propName);

        oldValue = getRealValue(oldValue);
        newValue = getRealValue(newValue);
        resValue = getRealValue(resValue);

        short attr = prop.Attributes;
        boolean readOnly = (prop.Attributes & PropertyAttribute.READONLY) != 0;
        boolean maybeVoid = (prop.Attributes & PropertyAttribute.MAYBEVOID) != 0;

        //check get-set methods
        if (maybeVoid) {
            System.out.println("Property " + propName + " is void");
        }

        if (readOnly) {
            System.out.println("Property " + propName + " is readOnly");
        }

        if (util.utils.isVoid(oldValue) && !maybeVoid) {
            System.out.println(propName + " is void, but it's not MAYBEVOID");

            return false;
        } else if (oldValue == null) {
            System.out.println(propName +
                        " has null value, and therefore can't be changed");

            return true;
        } else if (readOnly) {
            // check if exception was thrown
            if (exception != null) {
                if (exception instanceof PropertyVetoException) {
                    // the change of read only prohibited - OK
                    System.out.println("Property is ReadOnly and wasn't changed");
                    System.out.println("Property '" + propName + "' OK");

                    return true;
                } else if (exception instanceof IllegalArgumentException) {
                    // the change of read only prohibited - OK
                    System.out.println("Property is ReadOnly and wasn't changed");
                    System.out.println("Property '" + propName + "' OK");

                    return true;
                } else if (exception instanceof UnknownPropertyException) {
                    // the change of read only prohibited - OK
                    System.out.println("Property is ReadOnly and wasn't changed");
                    System.out.println("Property '" + propName + "' OK");

                    return true;
                } else if (exception instanceof RuntimeException) {
                    // the change of read only prohibited - OK
                    System.out.println("Property is ReadOnly and wasn't changed");
                    System.out.println("Property '" + propName + "' OK");

                    return true;
                } else {
                    throw exception;
                }
            } else {
                // if no exception - check that value
                // has not changed
                if (!ValueComparer.equalValue(resValue, oldValue)) {
                    System.out.println("Read only property '" + propName +
                                "' has changed");

                    try {
                        if (!util.utils.isVoid(oldValue) && 
                                oldValue instanceof Any) {
                            oldValue = AnyConverter.toObject(
                                               new Type(((Any) oldValue).getClass()), 
                                               oldValue);
                        }

                        System.out.println("old = " + toString(oldValue));
                        System.out.println("new = " + toString(newValue));
                        System.out.println("result = " + toString(resValue));
                    } catch (com.sun.star.lang.IllegalArgumentException iae) {
                    }

                    return false;
                } else {
                    System.out.println("Read only property '" + propName +
                                "' hasn't changed");
                    System.out.println("Property '" + propName + "' OK");

                    return true;
                }
            }
        } else {
            if (exception == null) {
                // if no exception thrown
                // check that the new value is set
                if ((!ValueComparer.equalValue(resValue, newValue)) || 
                        (ValueComparer.equalValue(resValue, oldValue))) {
                    System.out.println("Value for '" + propName +
                                "' hasn't changed as expected");

                    try {
                        if (!util.utils.isVoid(oldValue) && 
                                oldValue instanceof Any) {
                            oldValue = AnyConverter.toObject(
                                               new Type(((Any) oldValue).getClass()), 
                                               oldValue);
                        }

                        System.out.println("old = " + toString(oldValue));
                        System.out.println("new = " + toString(newValue));
                        System.out.println("result = " + toString(resValue));
                    } catch (com.sun.star.lang.IllegalArgumentException iae) {
                    }

                    if (resValue != null) {
                        if ((!ValueComparer.equalValue(resValue, oldValue)) || 
                                (!resValue.equals(oldValue))) {
                            System.out.println("But it has changed.");

                            return true;
                        } else {
                            return false;
                        }
                    } else {
                        return false;
                    }

                    //tRes.tested(propName, false);
                } else {
                    System.out.println("Property '" + propName + "' OK");

                    try {
                        if (!util.utils.isVoid(oldValue) && 
                                oldValue instanceof Any) {
                            oldValue = AnyConverter.toObject(
                                               new Type(((Any) oldValue).getClass()), 
                                               oldValue);
                        }

                        System.out.println("old = " + toString(oldValue));
                        System.out.println("new = " + toString(newValue));
                        System.out.println("result = " + toString(resValue));
                    } catch (com.sun.star.lang.IllegalArgumentException iae) {
                    }

                    return true;
                }
            } else {
                throw exception;
            }
        }
    }

    /*
     * Returns a String representation of a given object, returns "null" in case of a NullObject
     * @param obj : the given object
     */
    protected String toString(Object obj) {
        return (obj == null) ? "null" : obj.toString();
    }

    /*
     * Returns a human readable String representation for CellRangeAddressable
     */
    protected Object getRealValue(Object obj) {
        Object value = null;

        if (UnoRuntime.queryInterface(XCellRangeAddressable.class, obj) != null) {
            XCellRangeAddressable aCell =  UnoRuntime.queryInterface(
                                                  XCellRangeAddressable.class, 
                                                  obj);
            value = "Cell in Column " + aCell.getRangeAddress().StartColumn + 
                    " and Row " + aCell.getRangeAddress().StartRow;
        } else {
            return obj;
        }

        return value;
    }

    /*
     * This method inserts a RectangleShape into the calc document xSheetDoc and returns the resultung XShape-object
     */
    protected XShape getInsertedShape() {
        XShape insertedShape = null;

        try {
            System.out.println("getting Drawpages");

            XDrawPagesSupplier oDPS =  UnoRuntime.queryInterface(
                                              XDrawPagesSupplier.class, 
                                              xSheetDoc);
            XDrawPages oDP =  oDPS.getDrawPages();
            XDrawPage firstDrawPage =  UnoRuntime.queryInterface(
                                              XDrawPage.class, 
                                              oDP.getByIndex(0));

            final XMultiServiceFactory xMsf = UnoRuntime.queryInterface(XMultiServiceFactory.class, connection.getComponentContext().getServiceManager());
            SOfficeFactory SOF = SOfficeFactory.getFactory(xMsf);

            // SOfficeFactory SOF = SOfficeFactory.getFactory(
            //                              (XMultiServiceFactory) param.getMSF());
            XComponent xComp =  UnoRuntime.queryInterface(
                                       XComponent.class, xSheetDoc);
            insertedShape = SOF.createShape(xComp, 5000, 3500, 700, 500, 
                                            "Rectangle");
            DrawTools.getShapes(firstDrawPage).add(insertedShape);
        } catch (com.sun.star.lang.WrappedTargetException e) {
            System.out.println("Couldn't create instance");
            e.printStackTrace();

            return null;
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            System.out.println("Couldn't create instance");
            e.printStackTrace();

            return null;
        }

        return insertedShape;
    }




    @BeforeClass public static void setUpConnection() throws Exception {
        connection.setUp();
    }

    @AfterClass public static void tearDownConnection()
        throws InterruptedException, com.sun.star.uno.Exception
    {
        connection.tearDown();
    }

    private static final OfficeConnection connection = new OfficeConnection();

}
