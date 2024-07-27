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


package complex.connectivity.dbase;

import com.sun.star.uno.UnoRuntime;
import com.sun.star.sdbc.*;
import com.sun.star.beans.XPropertySet;
import com.sun.star.lang.XMultiServiceFactory;

import static org.junit.Assert.*;

public class DBaseDateFunctions
{

    private final String where = "FROM \"biblio\" \"biblio\" where \"Identifier\" = 'BOR00'";
    private final XMultiServiceFactory m_xORB;

    public DBaseDateFunctions(final XMultiServiceFactory _xORB)
    {
        m_xORB = _xORB;
    }

    public void testFunctions() throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRowSet xRowRes = (XRowSet) UnoRuntime.queryInterface(XRowSet.class,
                m_xORB.createInstance("com.sun.star.sdb.RowSet"));

        System.out.println("starting DateTime function test!");
        // set the properties needed to connect to a database
        final XPropertySet xProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xRowRes);
        xProp.setPropertyValue("DataSourceName", "Bibliography");

        xProp.setPropertyValue("CommandType", Integer.valueOf(com.sun.star.sdb.CommandType.COMMAND));

        curdate(xRowRes);
        curtime(xRowRes);
        dayname(xRowRes);
        dayofmonth(xRowRes);
        dayofweek(xRowRes);
        dayofyear(xRowRes);
        hour(xRowRes);
        minute(xRowRes);
        month(xRowRes);
        monthname(xRowRes);
        now(xRowRes);
        quarter(xRowRes);
        second(xRowRes);
        week(xRowRes);
        year(xRowRes);
    }

    private XRow execute(final XRowSet xRowRes, final String sql) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XPropertySet xProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xRowRes);
        xProp.setPropertyValue("Command", "SELECT " + sql + where);
        xRowRes.execute();
        final XResultSet xRes = (XResultSet) UnoRuntime.queryInterface(XResultSet.class, xRowRes);
        assertTrue("No valid row! ", xRes.next());

        return (XRow) UnoRuntime.queryInterface(XRow.class, xRes);
    }

    private void dayofweek(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "DAYOFWEEK('1998-02-03') ");
        assertTrue("DAYOFWEEK('1998-02-03') failed!", row.getInt(1) == 3);
    }

    private void dayofmonth(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "DAYOFMONTH('1998-02-03') ");
        assertTrue("DAYOFMONTH('1998-02-03') failed!", row.getInt(1) == 3);
    }

    private void dayofyear(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "DAYOFYEAR('1998-02-03') ");
        assertTrue("DAYOFYEAR('1998-02-03') failed!", row.getInt(1) == 34);
    }

    private void month(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "month('1998-02-03') ");
        assertTrue("month('1998-02-03') failed!", row.getInt(1) == 2);
    }

    private void dayname(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "DAYNAME('1998-02-05') ");
        assertTrue("DAYNAME('1998-02-05') failed!", row.getString(1).equals("Thursday"));
    }

    private void monthname(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "MONTHNAME('1998-02-05') ");
        assertTrue("MONTHNAME('1998-02-05') failed!", row.getString(1).equals("February"));
    }

    private void quarter(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "QUARTER('98-01-01'),QUARTER('98-04-01'),QUARTER('98-07-01'),QUARTER('98-10-01') ");
        assertTrue("QUARTER('98-01-01') failed!", row.getInt(1) == 1);
        assertTrue("QUARTER('98-04-01') failed!", row.getInt(2) == 2);
        assertTrue("QUARTER('98-07-01') failed!", row.getInt(3) == 3);
        assertTrue("QUARTER('98-10-01') failed!", row.getInt(4) == 4);
    }

    private void week(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "WEEK('1998-02-20') ");
        assertTrue("WEEK('1998-02-20') failed!", row.getInt(1) == 7);
    }

    private void year(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "YEAR('98-02-03') ");
        assertTrue("YEAR('98-02-03') failed!", row.getInt(1) == 98);
    }

    private void hour(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "HOUR('10:05:03') ");
        assertTrue("HOUR('10:05:03') failed!", row.getInt(1) == 10);
    }

    private void minute(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "MINUTE('98-02-03 10:05:03') ");
        assertTrue("MINUTE('98-02-03 10:05:03') failed!", row.getInt(1) == 5);
    }

    private void second(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "SECOND('10:05:03') ");
        assertTrue("SECOND('10:05:03') failed!", row.getInt(1) == 3);
    }

    private void curdate(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "CURDATE() ");
        final com.sun.star.util.Date aDate = row.getDate(1);
        System.out.println("CURDATE() is '" + aDate.Year + "-" + aDate.Month + "-" + aDate.Day + "'");
    }

    private void curtime(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "CURTIME() ");
        final com.sun.star.util.Time aTime = row.getTime(1);
        System.out.println("CURTIME() is '" + aTime.Hours + ":" + aTime.Minutes + ":" + aTime.Seconds + "'");
    }

    private void now(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "NOW() ");
        final com.sun.star.util.DateTime aTime = row.getTimestamp(1);
        System.out.println("NOW() is '" + aTime.Year + "-" + aTime.Month + "-" + aTime.Day + "'");
        System.out.println("'" + aTime.Hours + ":" + aTime.Minutes + ":" + aTime.Seconds + "'");
    }
}
