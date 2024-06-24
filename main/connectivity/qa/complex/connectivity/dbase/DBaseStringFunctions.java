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

public class DBaseStringFunctions
{
    private String where = "FROM \"biblio\" \"biblio\" where \"Identifier\" = 'BOR00'";
    private final XMultiServiceFactory m_xORB;

    public DBaseStringFunctions(final XMultiServiceFactory _xORB)
    {
        m_xORB = _xORB;
    }

    public void testFunctions() throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRowSet xRowRes = (XRowSet) UnoRuntime.queryInterface(XRowSet.class,
                m_xORB.createInstance("com.sun.star.sdb.RowSet"));

        System.out.println("starting String function test");
        // set the properties needed to connect to a database
        final XPropertySet xProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xRowRes);
        xProp.setPropertyValue("DataSourceName", "Bibliography");

        xProp.setPropertyValue("CommandType", Integer.valueOf(com.sun.star.sdb.CommandType.COMMAND));

        upper(xRowRes);
        lower(xRowRes);
        acsii(xRowRes);
        char_length(xRowRes);
        concat(xRowRes);
        chartest(xRowRes);
        locate(xRowRes);
        substring(xRowRes);
        ltrim(xRowRes);
        rtrim(xRowRes);
        space(xRowRes);
        replace(xRowRes);
        repeat(xRowRes);
        insert(xRowRes);
        left(xRowRes);
        right(xRowRes);
    }

    private XRow execute(final XRowSet xRowRes, String sql) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XPropertySet xProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xRowRes);
        xProp.setPropertyValue("Command", "SELECT " + sql + where);
        xRowRes.execute();
        final XResultSet xRes = (XResultSet) UnoRuntime.queryInterface(XResultSet.class, xRowRes);
        assertTrue("No valid row! ", xRes.next());

        return (XRow) UnoRuntime.queryInterface(XRow.class, xRes);
    }

    private void upper(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "upper('test'),UCASE('test') ");
        assertTrue("upper('test') failed!", row.getString(1).equals("TEST"));
        assertTrue("ucase('test') failed!", row.getString(2).equals("TEST"));
    }

    private void lower(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "lower('TEST'),LCASE('TEST') ");
        assertTrue("lower('TEST') failed!", row.getString(1).equals("test"));
        assertTrue("lcase('TEST') failed!", row.getString(2).equals("test"));
        final String temp = where;
        where = "FROM \"biblio\" \"biblio\" where LOWER(\"Identifier\") like 'bor%'";
        execute(xRowRes, "lower('TEST'),LCASE('TEST') ");
        where = temp;
    }

    private void acsii(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "ASCII('2') ");
        assertTrue("acsii('2') failed!", row.getInt(1) == 50);
    }

    private void char_length(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "char_length('test'),character_length('test'),OCTET_LENGTH('test') ");
        assertTrue("char_length('test') failed!", row.getInt(1) == 4);
        assertTrue("character_length('test') failed!", row.getInt(2) == 4);
        assertTrue("OCTET_LENGTH('test') failed!", row.getInt(3) == 4);
    }

    private void concat(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "CONCAT('Hello',' ','World') ");
        assertTrue("CONCAT('Hello',' ',,'World') failed!", row.getString(1).equals("Hello World"));
    }

    private void locate(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "LOCATE('bar', 'foobarbar') ");
        assertTrue("LOCATE('bar', 'foobarbar') failed!", row.getInt(1) == 4);
    }

    private void substring(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "SUBSTRING('Quadratically',5) ");
        assertTrue("SUBSTRING('Quadratically',5) failed!", row.getString(1).equals("ratically"));
    }

    private void ltrim(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "LTRIM('  barbar') ");
        assertTrue("LTRIM('  barbar') failed!", row.getString(1).equals("barbar"));
    }

    private void rtrim(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "RTRIM('barbar  ') ");
        assertTrue("RTRIM('barbar  ') failed!", row.getString(1).equals("barbar"));
    }

    private void space(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "space(6) ");
        assertTrue("space(6) failed!", row.getString(1).equals("      "));
    }

    private void replace(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "REPLACE('www.OOo.com', 'w', 'Ww') ");
        assertTrue("REPLACE('www.OOo.com', 'w', 'Ww') failed!", row.getString(1).equals("WwWwWw.OOo.com"));
    }

    private void repeat(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "REPEAT('OOo', 3) ");
        assertTrue("REPEAT('OOo', 3) failed!", row.getString(1).equals("OOoOOoOOo"));
    }

    private void insert(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "INSERT('Quadratic', 3, 4, 'What') ");
        assertTrue("INSERT('Quadratic', 3, 4, 'What') failed!", row.getString(1).equals("QuWhattic"));
    }

    private void left(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "LEFT('foobarbar', 5) ");
        assertTrue("LEFT('foobarbar', 5) failed!", row.getString(1).equals("fooba"));
    }

    private void right(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "RIGHT('foobarbar', 4) ");
        assertTrue("RIGHT('foobarbar', 4) failed!", row.getString(1).equals("rbar"));
    }

    private void chartest(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "CHAR(ascii('t'),ascii('e'),ascii('s'),ascii('t')) ");
        assertTrue("CHAR(ascii('t'),ascii('e'),ascii('s'),ascii('t')) failed!", row.getString(1).equals("test"));
    }
}
