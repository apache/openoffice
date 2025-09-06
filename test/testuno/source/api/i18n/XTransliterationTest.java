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

import com.sun.star.i18n.TransliterationModules;
import com.sun.star.i18n.TransliterationModulesNew;
import com.sun.star.i18n.TransliterationType;
import com.sun.star.i18n.XTransliteration;
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
* Testing <code>com.sun.star.i18n.XTransliteration</code>
* interface methods :
* <ul>
*  <li><code> getName()</code></li>
*  <li><code> getType()</code></li>
*  <li><code> loadModule()</code></li>
*  <li><code> loadModuleNew()</code></li>
*  <li><code> loadModuleByImplName()</code></li>
*  <li><code> loadModulesByImplNames()</code></li>
*  <li><code> getAvailableModules()</code></li>
*  <li><code> transliterate()</code></li>
*  <li><code> folding()</code></li>
*  <li><code> equals()</code></li>
*  <li><code> transliterateRange()</code></li>
* </ul> <p>
* Test is <b> NOT </b> multithread compliant. <p>
* @see com.sun.star.i18n.XTransliteration
*/
public class XTransliterationTest {
    private static final UnoApp app = new UnoApp();

    private XComponentContext xContext = null;
    public XTransliteration oObj = null;
    private String[] mod = null ;
    private Locale loc = new Locale("en", "EN", "") ;


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
            XTransliteration.class,
            xContext.getServiceManager().createInstanceWithContext("com.sun.star.i18n.Transliteration", xContext)
        );
    }

    /**
    * Gets all available transliteration modules. <p>
    * Has <b>OK</b> status if array returned has at least
    * one module name.
    */
    @Test
    public void _getAvailableModules() {
        mod = oObj.getAvailableModules(loc, TransliterationType.ONE_TO_ONE);

        if (mod != null) {
            System.out.println("Available modules :") ;
            for (int i = 0; i < mod.length; i++) {
                System.out.println("  '" + mod[i] + "'") ;
            }
        } else {
            System.out.println("!!! NULL returned !!!") ;
        }

        Assert.assertTrue("getAvailableModules()", mod != null && mod.length > 0) ;
    }

    /**
    * Calls the method for load IGNORE_CASE module and checks the name returned
    * by the method <code>getName</code>. <p>
    * Has <b>OK</b> status if the method <code>getName</code> returns the
    * string "case ignore (generic)".
    */
    @Test
    public void _loadModule() {
        System.out.println("Load module IGNORE_CASE");
        oObj.loadModule(TransliterationModules.IGNORE_CASE, loc);

        String name = oObj.getName();
        boolean res = name.equals("case ignore (generic)");
        System.out.println("getName return: " + name);

        Assert.assertTrue("loadModule()", res );
    }

    /**
     * Loads <code>LOWERCASE_UPPERCASE</code> module and checks the current
     * name of object. <p>
     *
     * Has <b>OK</b> status if the name of the object is equals to
     * 'lower_to_upper(generic)'
     */
    @Test
    public void _loadModuleNew() {
        boolean result = true ;

        oObj.loadModuleNew(
            new TransliterationModulesNew[]
            {TransliterationModulesNew.LOWERCASE_UPPERCASE}, loc);

        String name = oObj.getName();
        result = name.equals("lower_to_upper(generic)");
        System.out.println("getName return: " + name);

        Assert.assertTrue("loadModuleNew()", result);
    }

    /**
    * Calls the method for load LOWERCASE_UPPERCASE module and
    * checks the name returned by the method <code>getName</code>. <p>
    * Has <b>OK</b> status if the method <code>getName</code> returns the
    * string "lower_to_upper(generic)".
    */
    @Test
    public void _loadModuleByImplName() {
        System.out.println("Load module LOWERCASE_UPPERCASE");
        oObj.loadModuleByImplName("LOWERCASE_UPPERCASE", loc);

        String name = oObj.getName();
        boolean res = name.equals("lower_to_upper(generic)");
        System.out.println("getName return: " + name);

        Assert.assertTrue("loadModuleByImplName()", res);
    }

    /**
    * Calls the method for load UPPERCASE_LOWERCASE module and
    * checks the name returned by the method <code>getName</code>. <p>
    * Has <b>OK</b> status if the method <code>getName</code> returns the
    * string "upper_to_lower(generic)".
    */
    @Test
    public void _loadModulesByImplNames() {
        System.out.println("Load module UPPERCASE_LOWERCASE");
        oObj.loadModulesByImplNames(new String[]{"UPPERCASE_LOWERCASE"}, loc);

        String name = oObj.getName();
        boolean res = name.equals("upper_to_lower(generic)");
        System.out.println("getName return: " + name);

        Assert.assertTrue("loadModulesByImplNames()", res);
    }

    /**
     * Loads <code>LOWERCASE_UPPERCASE</code> module and checks current type.
     * <p>Has <b>OK</b> status if the type is <code>ONE_TO_ONE</code>
     */
    @Test
    public void _getType() {
        oObj.loadModule(TransliterationModules.LOWERCASE_UPPERCASE, loc);
        boolean result = oObj.getType() == TransliterationType.ONE_TO_ONE;
        Assert.assertTrue("getType()", result);
    }

    /**
    * Loads UPPERCASE_LOWERCASE module and
    * checks the name returned by the method <code>getName</code>. <p>
    *
    * Has <b>OK</b> status if the method <code>getName</code> returns the
    * string "upper_to_lower(generic)".
    */
    @Test
    public void _getName() {
        oObj.loadModule(TransliterationModules.LOWERCASE_UPPERCASE, loc);

        String name = oObj.getName();
        boolean res = name.equals("lower_to_upper(generic)");
        System.out.println("getName return: " + name);

        Assert.assertTrue("getName()", res);
    }

    /**
    * First loads <code>LOWERCASE_UPPERCASE</code> module.
    * Then tries to transliterate (make uppercase) a substring. <p>
    * Has <b>OK</b> status if all chars were made uppercase,
    * and array returned has size as substring length, and its
    * elements are positions of substring characters in the source
    * string.
    */
    @Test
    public void _transliterate() {
        oObj.loadModule(TransliterationModules.LOWERCASE_UPPERCASE, loc);

        int[][] offs = new int[1][] ;

        String out = oObj.transliterate("AaBbCc", 1, 4, offs) ;

        boolean result = "ABBC".equals(out) && offs[0].length == 4 &&
            offs[0][0] == 1 &&
            offs[0][1] == 2 &&
            offs[0][2] == 3 &&
            offs[0][3] == 4 ;

        Assert.assertTrue("transliterate()", result) ;
    }


    /**
    * First loads <code>LOWERCASE_UPPERCASE</code> module.
    * Tries to transliterate a range of two characters. <p>
    * Has <b>OK</b> status if the appropriate String array
    * returned (not null, length = 4, with two ranges
    * (a, i), (A, I) in any order).
    */
    @Test
    public void _transliterateRange() {
        oObj.loadModule(TransliterationModules.IGNORE_CASE, loc);

        String[] out = oObj.transliterateRange("a", "i") ;

        System.out.println("transliterateRange return:");
        for(int i = 0; i < out.length; i++) {
            System.out.println(out[i]);
        }

        boolean bOK = out != null &&
            out.length == 4 &&
            ("A".equals(out[0]) && "I".equals(out[1]) &&
            "a".equals(out[2]) && "i".equals(out[3])) ||
            ("a".equals(out[0]) && "i".equals(out[1]) &&
            "A".equals(out[2]) && "I".equals(out[3])) ;

        if (!bOK) {
            System.out.println("Unexpected range returned :");
            for (int i = 0; i < out.length; i++) {
                System.out.print("'" + out[i] +"', ");
            }
            System.out.println();
        }

        Assert.assertTrue("transliterateRange()", bOK);
    }

    /**
    * This method is used internally by <code>equals</code>
    * method so it indirectly tested in this method. <p>
    * Always has <b>OK</b> status.
    */
    @Test
    public void _folding() {
        oObj.loadModule(TransliterationModules.LOWERCASE_UPPERCASE, loc);

        int[][] offs = new int[1][] ;

        String out = oObj.folding("AaBbCc", 1, 4, offs) ;

        boolean result = "ABBC".equals(out) && offs[0].length == 4 &&
            offs[0][0] == 1 &&
            offs[0][1] == 2 &&
            offs[0][2] == 3 &&
            offs[0][3] == 4 ;


        Assert.assertTrue("folding()", result) ;
    }


    /**
    * First loads <code>LOWERCASE_UPPERCASE</code> module.
    * Tries to compare two equal substrings. <p>
    * Has <b>OK</b> status if the method returned <code>true</code>.
    */
    @Test
    public void _equals() {
        oObj.loadModule(TransliterationModules.LOWERCASE_UPPERCASE, loc);

        int[] match1 = new int[1],
              match2 = new int[1] ;

        boolean res = oObj.equals("aAbBcC", 1, 3, match1, "aAbBcC", 1,
            3, match2) ;

        System.out.println("Returned : " + res + " Match1 = " + match1[0] +
            " Match2 = " + match2[0]) ;

        Assert.assertTrue("equals()", res) ;
    }

    /**
     * Test performed for sets of equal substrings, not equal
     * substrings, and with out of bounds offset and length
     * parameters.<p>
     *
     * Has <b>OK</b> status if comparings of equal substrings
     * always return 0, if comparisons of none equal returns
     * proper value according to lexicographical order and if
     * comparisons with invalid parameters return none 0 value.
     */
    @Test
    public void _compareSubstring() {
        oObj.loadModule(TransliterationModules.LOWERCASE_UPPERCASE, loc);
        boolean result = true ;

        // substrings below must be equal
        result &= testSubstring("", 0, 0, "", 0, 0, 0) ;
        result &= testSubstring("aa", 1, 0, "", 0, 0, 0) ;
        result &= testSubstring("aa", 1, 0, "aa", 2, 0, 0) ;
        result &= testSubstring("a", 0, 1, "a", 0, 1, 0) ;
        result &= testSubstring("ab", 0, 2, "ab", 0, 2, 0) ;
        result &= testSubstring("abc", 1, 2, "abc", 1, 2, 0) ;
        result &= testSubstring("abcdef", 0, 3, "123abc", 3, 3, 0) ;
        result &= testSubstring("abcdef", 1, 1, "123abc", 4, 1, 0) ;

        // substrings below must NOT be equal
        result &= testSubstring("a", 0, 1, "a", 0, 0, 1) ;
        result &= testSubstring("aaa", 1, 1, "", 0, 0, 1) ;
        result &= testSubstring("bbb", 2, 1, "aaa", 2, 1, 1) ;
        result &= testSubstring("abc", 0, 3, "abc", 0, 2, 1) ;
        result &= testSubstring("bbc", 1, 2, "bbc", 0, 2, 1) ;

        // testing with wrong offsets and lengths

        Assert.assertTrue("compareSubstring()", result) ;
    }

    /**
     * Performs tesing of two substrings. Also testing of opposite
     * substrings order performed.
     * @return <code>true</code> if substrings are equal and returned
     * value is 0 for both orders,
     * if substrings are different and expected value
     * returned for direct order and opposite value returned for
     * opposite order.
     */
    private boolean testSubstring(String str1, int p1, int len1,
        String str2, int p2, int len2, int expRes) {

        boolean ret = true ;

        int res = -666 ;
        try {
            res = oObj.compareSubstring(str1, p1, len1, str2, p2, len2);
        } catch (java.lang.NullPointerException e) {
            System.out.println("Exception while method calling occurs :" + e);
        }

        if (res != expRes) {
            System.out.print("Comparing FAILED; return: " + res + ", expected: " +
                expRes + " ");
            ret = false ;
        } else {
            System.out.print("Comparing OK : ");
        }
        System.out.println("('" + str1 + "', " + p1 + ", " + len1 + ", '" +
            str2 + "', " + p2 + ", " + len2 + ")");

        res = -666 ;
        try {
            res = oObj.compareSubstring(str2, p2, len2, str1, p1, len1);
        } catch (java.lang.NullPointerException e) {
            System.out.println("Exception while method calling occurs :" + e);
        }

        if (res != -expRes) {
            System.out.print("Comparing FAILED; return: " + res + ", expected: " +
                -expRes  + " ");
            ret = false ;
        } else {
            System.out.print("Comparing OK :");
        }
        System.out.println("('" + str2 + "', " + p2 + ", " + len2 + ", '" +
            str1 + "', " + p1 + ", " + len1 + ")");

        return ret ;
    }

    /**
     * Test performed for sets of equal strings and not equal
     * strings.<p>
     *
     * Has <b>OK</b> status if comparings of equal strings
     * always return 0 and if comparisons of none equal returns
     * proper value according to lexicographical order .
     */
    @Test
    public void _compareString() {
        oObj.loadModule(TransliterationModules.LOWERCASE_UPPERCASE, loc);
        boolean result = true ;

        result &= testString("", "", 0) ;
        result &= testString("a", "", 1) ;
        result &= testString("a", "a", 0) ;
        result &= testString("A", "a", 0) ;
        result &= testString("b", "a", 1) ;
        result &= testString("\n", "\n", 0) ;
        result &= testString("\n", "\t", 1) ;
        result &= testString("aaa", "aaa", 0) ;
        result &= testString("aaA", "aaa", 0) ;
        result &= testString("aaa", "aa", 1) ;
        result &= testString("ab", "aaa", 1) ;
        result &= testString("aba", "aa", 1) ;
        result &= testString("aaa\t\na", "aaa\t\na", 0) ;
        result &= testString("aaa\t\nb", "aaa\t\na", 1) ;

        Assert.assertTrue("compareString()", result) ;
    }

    /**
     * Performs tesing of two strings. If the expected value is not 0
     * (i.e. strings are not equal), then also testing of opposite
     * strings order performed.
     * @return <code>true</code> if strings are equal and returned
     * value is 0, if strings are different and expected value
     * returned for direct order and opposite value returned for
     * opposite order.
     */
    protected boolean testString(String str1, String str2, int expRes) {
        if (expRes == 0) return testString(str1, str2, expRes, false) ;
        return testString(str1, str2, expRes, true) ;
    }

    private boolean testString(String str1, String str2, int expRes,
        boolean testReverse) {

        boolean ret = true ;

        int res = -666 ;
        try {
            res = oObj.compareString(str1, str2);
        } catch (java.lang.NullPointerException e) {
            System.out.println("Exception while method calling occurs :" + e);
        }

        if (res == expRes) {
            System.out.println("Comparing of '" + str1 + "' and '" + str2 + "' OK" );
        } else {
            System.out.println("Comparing of '" + str1 + "' and '" + str2 +
                "' FAILED; return: " + res + ", expected: " + expRes);
            ret = false ;
        }

        if (!testReverse) return ret ;

        res = -666 ;
        try {
            res = oObj.compareString(str2, str1);
        } catch (java.lang.NullPointerException e) {
            System.out.println("Exception while method calling occurs :" + e);
        }

        if (res == -expRes) {
            System.out.println("Comparing of '" + str2 + "' and '" + str1 + "' OK" );
        } else {
            System.out.println("Comparing of '" + str2 + "' and '" + str1 +
                "' FAILED; return: " + res + ", expected: " + -expRes);
            ret = false ;
        }

        return ret ;
    }
}
