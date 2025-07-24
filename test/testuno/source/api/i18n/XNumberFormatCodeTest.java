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



package api.i18n;

import com.sun.star.i18n.KNumberFormatType;
import com.sun.star.i18n.KNumberFormatUsage;
import com.sun.star.i18n.NumberFormatCode;
import com.sun.star.i18n.NumberFormatIndex;
import com.sun.star.i18n.XNumberFormatCode;
import com.sun.star.lang.Locale;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Assert;
import org.junit.Test;
import org.openoffice.test.uno.UnoApp;


/**
* Testing <code>com.sun.star.i18n.XNumberFormatCode</code>
* interface methods:
* <ul>
*  <li><code> getDefault() </code></li>
*  <li><code> getFormatCode() </code></li>
*  <li><code> getAllFormatCode() </code></li>
*  <li><code> getAllFormatCodes() </code></li>
* </ul><p>
* Test is <b> NOT </b> multithread compliant. <p>
* @see com.sun.star.i18n.XNumberFormatCode
*/
public class XNumberFormatCodeTest {
    private static final UnoApp app = new UnoApp();

    private XComponentContext xContext = null;
    public XNumberFormatCode oObj = null;
    public String[] languages = new String[]
        {"de","en","es","fr","ko","ko","zh"};
    public String[] countries = new String[]
        {"DE","US","ES","FR","KR","KR","CN"};

    // setup and close connections
    @BeforeClass
    public static void setUpConnection() throws Exception
    {
        app.start();
    }

    @AfterClass
    public static void tearDownConnection() throws InterruptedException, com.sun.star.uno.Exception
    {
        app.close();
    }

    @Before
    public void before() throws Exception {
        xContext = app.getComponentContext();
        oObj = UnoRuntime.queryInterface(
            XNumberFormatCode.class,
            xContext.getServiceManager().createInstanceWithContext("com.sun.star.i18n.NumberFormatCodeMapper", xContext)
        );
    }


    /**
    * Test calls the method twice with two different format types as
    * parameters for each locale. Result is checked after every call.<p>
    * Has <b> OK </b> status if both times returned structure's field 'Code'
    * does not equal to empty string.
    */
    @Test
    public void _getDefault() {
        boolean res = true;
        NumberFormatCode nfc = null;

        for (int i=0;i<7;i++) {
            nfc = oObj.getDefault(KNumberFormatType.SHORT,
                KNumberFormatUsage.DATE, getLocale(i));
            String str = nfc.Code;
            if (str.equals("")) {
                System.out.println("'NumberFormat.code.equals(\"\") = true' for"
                    + " language: " + languages[i]);
                System.out.println("Usage: oObj.getDefault(KNumberFormatType.SHORT,"
                    + " KNumberFormatUsage.DATE,new Locale(" + languages[i]
                    + "," + countries[i] + ",\"\");");
            }
            res &= !str.equals("");

            nfc = oObj.getDefault(KNumberFormatType.LONG,
                KNumberFormatUsage.DATE,getLocale(i));
            str = nfc.Code;
            if (str.equals("")) {
                System.out.println("'NumberFormat.code.equals(\"\") = true' for "
                    + "language: " + languages[i]);
                System.out.println("Usage: oObj.getDefault(KNumberFormatType.LONG,"
                    + " KNumberFormatUsage.DATE,new Locale(" + languages[i]
                    + "," + countries[i] + ",\"\");");
            }
            res &= ( ! str.equals("") );
        }
        Assert.assertTrue("getDefault()", res);
    }

    /**
    * Test calls the method twice for each locale with two different arguments.
    * After every call result is checked.<p>
    * Has <b> OK </b> status if both times returned structure's field 'Code'
    * does not equal to a empty string.
    */
    @Test
    public void _getFormatCode() {
        boolean res = true;
        NumberFormatCode nfc = null;

        for (int i=0;i<7;i++) {
            nfc = oObj.getFormatCode
                (NumberFormatIndex.DATE_SYSTEM_SHORT,getLocale(i));
            res &= ( ! nfc.Code.equals("") );
            nfc = oObj.getFormatCode
                (NumberFormatIndex.DATE_SYSTEM_LONG,getLocale(i));
            res &= ( ! nfc.Code.equals("") );
        }
        Assert.assertTrue("getFormatCode()", res);
    }

    /**
    * Test calls the method twice with two different arguments for each locale.
    * After every call result is checked.<p>
    * Has <b> OK </b> status if both times returned array's length does not
    * equal to zero.
    */
    @Test
    public void _getAllFormatCode() {
        boolean res = true;
        NumberFormatCode[] nfc = null;

        for (int i=0;i<7;i++) {
            nfc = oObj.getAllFormatCode(KNumberFormatUsage.DATE, getLocale(i));
            res &= ( nfc.length != 0 );
            nfc = oObj.getAllFormatCode(KNumberFormatUsage.TIME, getLocale(i));
            res &= ( nfc.length != 0 );
        }
        Assert.assertTrue("getAllFormatCode()", res);
    }

    /**
    * Test calls the method for each locale. <p>
    * Has <b> OK </b> status if returned array's length does not equal to zero.
    */
    @Test
    public void _getAllFormatCodes() {
        boolean res = true;
        NumberFormatCode[] nfc = null;

        for (int i=0;i<7;i++) {
            nfc = oObj.getAllFormatCodes(getLocale(i));
            res &= ( nfc.length != 0 );
        }
        Assert.assertTrue("getAllFormatCodes()", res);
    }

    /**
    * Method returns locale for a given language and country.
    * @param k index of needed locale.
    * @return Locale by the index from arrays defined above
    */
    public Locale getLocale(int k) {
        return new Locale(languages[k], countries[k], "");
    }



} // end XNumberFormatCode
