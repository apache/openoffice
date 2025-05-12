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

import com.sun.star.i18n.CalendarDisplayIndex;
import com.sun.star.i18n.CalendarFieldIndex;
import com.sun.star.i18n.CalendarItem;
import com.sun.star.i18n.XCalendar;
import com.sun.star.i18n.XLocaleData;
import com.sun.star.lang.Locale;
import com.sun.star.lang.XMultiServiceFactory;
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
* Testing <code>com.sun.star.i18n.XCalendar</code>
* interface methods :
* <ul>
*  <li><code> loadDefaultCalendar()</code></li>
*  <li><code> loadCalendar()</code></li>
*  <li><code> getLoadedCalendar()</code></li>
*  <li><code> getAllCalendars()</code></li>
*  <li><code> getUniqueID()</code></li>
*  <li><code> setDateTime()</code></li>
*  <li><code> getDateTime()</code></li>
*  <li><code> setValue()</code></li>
*  <li><code> getValue()</code></li>
*  <li><code> isValid()</code></li>
*  <li><code> addValue()</code></li>
*  <li><code> getFirstDayOfWeek()</code></li>
*  <li><code> setFirstDayOfWeek()</code></li>
*  <li><code> setMinimumNumberOfDaysForFirstWeek()</code></li>
*  <li><code> getMinimumNumberOfDaysForFirstWeek()</code></li>
*  <li><code> getNumberOfMonthsInYear()</code></li>
*  <li><code> getNumberOfDaysInWeek()</code></li>
*  <li><code> getMonths()</code></li>
*  <li><code> getDays()</code></li>
*  <li><code> getDisplayName()</code></li>
* </ul> <p>
* Test is <b> NOT </b> multithread compliant. <p>
* @see com.sun.star.i18n.XCalendar
*/
public class XCalendarTest {
    private static final UnoApp app = new UnoApp();

    private XComponentContext xContext = null;
    private boolean debug = false;
    public XCalendar oObj = null;
    public double newDTime = 1000.75;
    public short newValue = 2;
    public final short firstDay = 2;
    public short mdfw = 3;
    double aOriginalDTime = 0;
    Locale[] installed_locales;

    static class CalendarData {
        String[][] calendars;
        int[] count;
    };


    @BeforeClass
    public static void setUp() throws Exception {
        app.start();
    }

    @AfterClass
    public static void tearDown() throws Exception {
        app.close();
    }

    @Before
    public void before() throws Exception {
        xContext = app.getComponentContext();

        XLocaleData locData = null;
        locData = UnoRuntime.queryInterface(
            XLocaleData.class,
            xContext.getServiceManager().createInstanceWithContext("com.sun.star.i18n.LocaleData", xContext)
        );
        oObj = UnoRuntime.queryInterface(
            XCalendar.class,
            xContext.getServiceManager().createInstanceWithContext("com.sun.star.i18n.LocaleCalendar", xContext)
        );
        installed_locales = locData.getAllInstalledLocaleNames();
        oObj.loadDefaultCalendar(installed_locales[0]);
        aOriginalDTime = oObj.getDateTime();
        
        debug = false;
    }

    /**
     * Restore the changed time during the test to the original value of the
     * machine: has to be correct for the following interface tests.
     */
    @After
    public void after() throws Exception {
        oObj.loadDefaultCalendar(installed_locales[0]);
        oObj.setDateTime(aOriginalDTime);
    }
    
    /**
    * Loads default calendar for different locales. <p>
    * Has <b> OK </b> status if method loads calendar, that is
    * default for a given locale.
    */
    @Test
    public void _loadDefaultCalendar() {
        for (int i=0; i<installed_locales.length; i++) {
            String lang = "Language: "+installed_locales[i].Language +
                          ", Country: "+ installed_locales[i].Country +
                          ", Variant: "+ installed_locales[i].Variant;
            oObj.loadDefaultCalendar(installed_locales[i]);
            Assert.assertTrue(lang, oObj.getLoadedCalendar().Default);
        }
    }

    /**
    * Tries to obtain calendars for a number of locales. <p>
    * Has <b> OK </b> status if the method returns more than zero calendars for
    * every locale.
    */
    @Test
    public void _getAllCalendars() {
        getAllCalendars();
    }

    private CalendarData getAllCalendars() {
        CalendarData data = new CalendarData();
        data.calendars = new String[installed_locales.length][];
        data.count = new int[installed_locales.length];
        for (int i=0; i<installed_locales.length; i++) {
            String lang = "Language: "+installed_locales[i].Language +
                          ", Country: "+ installed_locales[i].Country +
                          ", Variant: "+ installed_locales[i].Variant;
            data.calendars[i] = oObj.getAllCalendars(installed_locales[i]);
            data.count[i] = data.calendars[i].length-1;
            Assert.assertTrue(lang, data.calendars[i].length > 0);
        }
        return data;
    }

    /**
    * Loads calendars for a number of locales. <p>
    * Has <b> OK </b> status if loaded calendar names are equal to gotten
    * calendar names after loading.<p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> getAllCalendars() </code> : gets all calendars for a given
    *  locale </li>
    * </ul>
    */
    @Test
    public void _loadCalendar() {
        loadCalendar();
    }

    private CalendarData loadCalendar() {
        CalendarData data = getAllCalendars();
        for (int i=0; i<installed_locales.length; i++) {
            String lang = "Language: "+installed_locales[i].Language +
                          ", Country: "+ installed_locales[i].Country +
                          ", Variant: "+ installed_locales[i].Variant;
            oObj.loadCalendar(data.calendars[i][0], installed_locales[i]);
            Assert.assertEquals(lang, data.calendars[i][0], oObj.getLoadedCalendar().Name);
        }
        return data;
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if loaded calendar names are equal to gotten
    * calendar names after loading.<p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> loadCalendar() </code> : loads calendar using a given name
    *  and locale </li>
    * </ul>
    */
    @Test
    public void _getLoadedCalendar() {
        CalendarData data = loadCalendar();
        for (int i=0; i<installed_locales.length; i++) {
            String lang = "Language: "+installed_locales[i].Language +
                          ", Country: "+ installed_locales[i].Country +
                          ", Variant: "+ installed_locales[i].Variant;
            oObj.loadCalendar(data.calendars[i][0], installed_locales[i]);
            Assert.assertEquals(lang, data.calendars[i][0], oObj.getLoadedCalendar().Name);
        }
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if the method returns value that's equal to a
    * calendar name. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> loadCalendar() </code> :  loads calendar using a given name
    *  and locale </li>
    * </ul>
    */
    @Test
    public void _getUniqueID() {
        CalendarData data = getAllCalendars();
        for (int i=0; i<installed_locales.length; i++) {
            String lang = "Language: "+installed_locales[i].Language +
                          ", Country: "+ installed_locales[i].Country +
                          ", Variant: "+ installed_locales[i].Variant;
            oObj.loadCalendar(data.calendars[i][0], installed_locales[i]);
            String uID = oObj.getUniqueID();
            Assert.assertEquals(lang, uID, data.calendars[i][0]);
        }
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if the method returns value, that's equal to
    * value set before. <p>
    */
    @Test
    public void _setDateTime() {
        for (int i=0; i<installed_locales.length; i++) {
            String lang = "Language: "+installed_locales[i].Language +
                          ", Country: "+ installed_locales[i].Country +
                          ", Variant: "+ installed_locales[i].Variant;
            oObj.setDateTime(newDTime);
            double aDTime = oObj.getDateTime();
            Assert.assertTrue(lang, aDTime == newDTime);
        }
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if the method returns value, that's equal to
    * value set before. <p>
    */
    @Test
    public void _getDateTime() {
        for (int i=0; i<installed_locales.length; i++) {
            String lang = "Language: "+installed_locales[i].Language +
                          ", Country: "+ installed_locales[i].Country +
                          ", Variant: "+ installed_locales[i].Variant;
            oObj.setDateTime(newDTime);
            double aDTime = oObj.getDateTime();
            Assert.assertTrue(lang, aDTime == newDTime);
        }
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if the method returns value, that's equal to
    * value set before. <p>
    */
    @Test
    public void _setValue() {
        CalendarData data = getAllCalendars();
        for (int i=0; i<installed_locales.length; i++) {
            String error = "";
            String lang = "Language: "+installed_locales[i].Language +
                          ", Country: "+ installed_locales[i].Country +
                          ", Variant: "+ installed_locales[i].Variant +
                          ", Name: "+data.calendars[i][data.count[i]];
            String[] names = new String[]{"DAY_OF_MONTH",
                "HOUR","MINUTE","SECOND","MILLISECOND",
                "YEAR","MONTH"};
            oObj.loadCalendar(data.calendars[i][data.count[i]],installed_locales[i]);
            short[] fields = new short[]{CalendarFieldIndex.DAY_OF_MONTH,
                                         CalendarFieldIndex.HOUR,
                                         CalendarFieldIndex.MINUTE,
                                         CalendarFieldIndex.SECOND,
                                         CalendarFieldIndex.MILLISECOND,
                                         CalendarFieldIndex.YEAR,
                                         CalendarFieldIndex.MONTH
            };
            for (int k=0; k<fields.length;k++) {
                
                oObj.setDateTime(0.0);
                
                // save the current values for debug purposes
                short[] oldValues = new short[fields.length];
                for (int n=0; n < oldValues.length; n++){
                    oldValues[n] = oObj.getValue(fields[n]);
                }
                
                short set = oObj.getValue(fields[k]);
                if (fields[k] == CalendarFieldIndex.MONTH) set = newValue;
                oObj.setValue(fields[k],set);
                short get = oObj.getValue(fields[k]);
                if (get != set) {
                    if (debug)
                        System.out.println("ERROR occur: tried to set " + names[k] + " to value " + set);
                        System.out.println("list of values BEFORE set " + names[k] + " to value " + set + ":");
                        for (int n=0; n < oldValues.length; n++){
                            System.out.println(names[n] + ":" + oldValues[n]);
                        }
                        System.out.println("list of values AFTER set " + names[k] + " to value " + set + ":");
                        for (int n=0; n < fields.length;n++){
                            System.out.println(names[n] + ":" + oObj.getValue(fields[n]));
                        }
                        
                    error += "failed for "+names[k]+" expected "+
                                set+" gained "+get+" ; \n";
                }
            }
            Assert.assertTrue(error, error.equals(""));
        }
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if the method returns value, that's equal to
    * value set before. <p>
    */
    @Test
    public void _getValue() {
        _setValue();
        short aValue = oObj.getValue(CalendarFieldIndex.MONTH);
        Assert.assertEquals(
            "the returned value is not the expected value:" +
            "expected: " + newValue + "  returned value: " + aValue,
            aValue, newValue);
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if value, added by the method is greater than
    * previously defined "newValue".
    * <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> getValue() </code> : gets the value of a field </li>
    * </ul>
    */
    @Test
    public void _addValue() {
        _setValue();
        oObj.addValue(CalendarFieldIndex.MONTH, 1);
        short aValue = oObj.getValue(CalendarFieldIndex.MONTH);
        Assert.assertTrue(
            "the returned value is not the expected value:" +
            "expected: " + newValue + "  returned value: " + aValue,
            aValue > newValue);
    }

    /**
    * Test calls the method. <p>
    * Has <b> OK </b> status if the method successfully returns
    * and no exceptions were thrown.
    */
    @Test
    public void _setFirstDayOfWeek() {
        oObj.setFirstDayOfWeek(firstDay);
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if the method returns value that is equal to
    * value set before. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> setFirstDayOfWeek() </code> : set the first day of a
    *  week</li>
    * </ul>
    */
    @Test
    public void _getFirstDayOfWeek() {
        _setFirstDayOfWeek();
        short aFirstDayOfWeek = oObj.getFirstDayOfWeek();
        Assert.assertEquals("getFirstDayOfWeek()", aFirstDayOfWeek, firstDay);
    }

    /**
    * Test calls the method. <p>
    * Has <b> OK </b> status if the method successfully returns
    * and no exceptions were thrown.
    */
    @Test
    public void _setMinimumNumberOfDaysForFirstWeek() {
        oObj.setMinimumNumberOfDaysForFirstWeek(mdfw);
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if the method returns value that is equal to
    * value set before. <p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> setMinimumNumberOfDaysForFirstWeek() </code> : sets how
    *  many days of a week must reside in the first week of a year</li>
    * </ul>
    */
    @Test
    public void _getMinimumNumberOfDaysForFirstWeek() {
        _setMinimumNumberOfDaysForFirstWeek();
        short aShort = oObj.getMinimumNumberOfDaysForFirstWeek();
        Assert.assertEquals("getMinimumNumberOfDaysForFirstWeek()", aShort, mdfw);
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if the method returns 12.
    */
    @Test
    public void _getNumberOfMonthsInYear() {
        short aShort = oObj.getNumberOfMonthsInYear();
        Assert.assertTrue("getNumberOfMonthsInYear()", (aShort == (short) 12));
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if the method returns 7.
    */
    @Test
    public void _getNumberOfDaysInWeek() {
        short aShort = oObj.getNumberOfDaysInWeek();
        Assert.assertTrue("getNumberOfDaysInWeek()", (aShort == (short) 7));
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if length of array, returned by the method is 12.
    */
    @Test
    public void _getMonths() {
        CalendarItem[] months = oObj.getMonths();
        Assert.assertTrue("getMonths()", months.length == 12);
    }

    /**
    * Test calls the method, then result is checked. <p>
    * Has <b> OK </b> status if length of array, returned by the method is 7.
    */
    @Test
    public void _getDays() {
        CalendarItem[] Days = oObj.getDays();
        Assert.assertTrue("getDays()", Days.length == 7);
    }

    /**
    * After loading calendar, test calls the method, then result is checked.<p>
    * Has <b> OK </b> status if length of string, returned by the method is 3.
    */
    @Test
    public void _getDisplayName() {
        CalendarData data = getAllCalendars();
        oObj.loadCalendar(data.calendars[0][0],installed_locales[0]);
        String DisplayName = oObj.getDisplayName(CalendarDisplayIndex.MONTH,
            newValue, (short) 0);
        Assert.assertTrue("getDisplayName()", DisplayName.length() == 3);
    }


    /**
    * The test sets obviously wrong value, then calls a method. After that the
    * test sets correct value, and again calls a method. <p>
    * Has <b> OK </b> status if the method returns true when valid month is
    * set, and if the method returns false when set month is not valid.
    */
    @Test
    public void _isValid() {
        oObj.loadDefaultCalendar(installed_locales[0]);
        oObj.setValue(CalendarFieldIndex.MONTH, (short) 37);
        Assert.assertTrue(!oObj.isValid());
        oObj.setValue(CalendarFieldIndex.MONTH, (short) 10);
        Assert.assertTrue(oObj.isValid());
    }

    /**
    * Method returns locale for a given language and country.
    * @param localeIndex index of needed locale.
    */
/*    public Locale getLocale(int localeIndex) {
        return new Locale(languages[localeIndex], countries[localeIndex], "");
    }*/

}
