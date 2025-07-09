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

import com.sun.star.i18n.XIndexEntrySupplier;
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
* Testing <code>com.sun.star.i18n.XIndexEntrySupplier</code>
* interface methods:
* <ul>
*  <li><code> getIndexCharacter() </code></li>
*  <li><code> getIndexFollowPageWord() </code></li>
* </ul><p>
* Test is <b> NOT </b> multithread compliant. <p>
* @see com.sun.star.i18n.XIndexEntrySupplier
*/
public class XIndexEntrySupplierTest {
    private static final UnoApp app = new UnoApp();

    private XComponentContext xContext = null;
    public XIndexEntrySupplier oObj = null;
    public String[] languages = new String[]{"de","en","es","fr","ja","ko","zh"};
    public String[] countries = new String[]{"DE","US","ES","FR","JP","KR","CN"};
    public String[] onePage = new String[]{"f.","p."," s."," sv","p.","",""};
    public String[] morePages = new String[]{"ff.","pp."," ss."," sv","pp.","",""};

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
            XIndexEntrySupplier.class,
            xContext.getServiceManager().createInstanceWithContext("com.sun.star.i18n.IndexEntrySupplier", xContext)
        );
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if the method returns right index for several
    * locales and word.
    */
    @Test
    public void _getIndexCharacter() {
        boolean res = true;
        System.out.println("getIndexCharacter('chapter', getLocale(i), '')");
        for (int i=0; i<7; i++) {
            System.out.print("getIndexCharacter('chapter', " + countries[i] + ") :");
            String get = oObj.getIndexCharacter("chapter", getLocale(i), "");
            System.out.println(get);
            res &= get.equals("C");
        }
        Assert.assertTrue("getIndexCharacter()", res);
    }

    /**
    * Test calls the method with two different parameters: for one page and
    * for several pages, after every call result is checked. <p>
    * Has <b> OK </b> status if method returns right index for several locales.
    */
    @Test
    public void _getIndexFollowPageWord() {
        boolean res = true;

        for (int i=0; i<7; i++) {
            String get = oObj.getIndexFollowPageWord(true, getLocale(i));
            if (! get.equals(morePages[i]) ) {
                System.out.println("Language: " + languages[i]);
                System.out.println("Getting: #" + get + "#");
                System.out.println("Expected: #" + morePages[i] + "#");
            }
            res &= get.equals(morePages[i]);
            get = oObj.getIndexFollowPageWord(false,getLocale(i));
            if (! get.equals(onePage[i]) ) {
                System.out.println("Language: " + languages[i]);
                System.out.println("Getting: #" + get + "#");
                System.out.println("Expected: #" + onePage[i] + "#");
            }
            res &= get.equals(onePage[i]);
        }
        Assert.assertTrue("getIndexFollowPageWord()", res);
    }

    /**
    * Method returns locale for a given language and country.
    * @param k index of needed locale.
    * @return Locale by the index from arrays defined above
    */
    public Locale getLocale(int k) {
        return new Locale(languages[k], countries[k], "");
    }


} // end XIndexEntrySupplier
