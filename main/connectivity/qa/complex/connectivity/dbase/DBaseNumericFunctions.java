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

public class DBaseNumericFunctions
{
    private final String where = "FROM \"biblio\" \"biblio\" where \"Identifier\" = 'BOR00'";
    private final XMultiServiceFactory m_xORB;

    public DBaseNumericFunctions(final XMultiServiceFactory _xORB)
    {
        m_xORB = _xORB;
    }

    public void testFunctions() throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRowSet xRowRes = (XRowSet) UnoRuntime.queryInterface(XRowSet.class,
                m_xORB.createInstance("com.sun.star.sdb.RowSet"));

        System.out.println("starting Numeric function test");
        // set the properties needed to connect to a database
        final XPropertySet xProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xRowRes);
        xProp.setPropertyValue("DataSourceName", "Bibliography");

        xProp.setPropertyValue("CommandType", Integer.valueOf(com.sun.star.sdb.CommandType.COMMAND));

        abs(xRowRes);
        acos(xRowRes);
        asin(xRowRes);
        atan(xRowRes);
        atan2(xRowRes);
        ceiling(xRowRes);
        cos(xRowRes);
        degrees(xRowRes);
        exp(xRowRes);
        floor(xRowRes);
        log(xRowRes);
        log10(xRowRes);
        mod(xRowRes);
        pi(xRowRes);
        pow(xRowRes);
        radians(xRowRes);
        round(xRowRes);
        sign(xRowRes);
        sin(xRowRes);
        sqrt(xRowRes);
        tan(xRowRes);
    }

    private XRow execute(final XRowSet xRowRes,final  String sql) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XPropertySet xProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xRowRes);
        xProp.setPropertyValue("Command", "SELECT " + sql + where);
        xRowRes.execute();
        final XResultSet xRes = (XResultSet) UnoRuntime.queryInterface(XResultSet.class, xRowRes);
        assertTrue("No valid row! ", xRes.next());

        return (XRow) UnoRuntime.queryInterface(XRow.class, xRes);
    }

    private void abs(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "ABS(2),ABS(-32) ");
        assertTrue("ABS(2) failed!", row.getInt(1) == 2);
        assertTrue("ABS(-32) failed!", row.getInt(2) == 32);
    }

    private void sign(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "SIGN(-32),SIGN(0),SIGN(234) ");
        assertTrue("SIGN(-32)failed!", row.getInt(1) == -1);
        assertTrue("SIGN(0) failed!", row.getInt(2) == 0);
        assertTrue("SIGN(234) failed!", row.getInt(3) == 1);
    }

    private void mod(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "MOD(234, 10) ");
        assertTrue("MOD(234, 10) failed!", row.getInt(1) == 4);
    }

    private void floor(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "FLOOR(1.23),FLOOR(-1.23) ");
        assertTrue("FLOOR(1.23) failed!", row.getInt(1) == 1);
        assertTrue("FLOOR(-1.23) failed!", row.getInt(2) == -2);
    }

    private void ceiling(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "CEILING(1.23),CEILING(-1.23) ");
        assertTrue("CEILING(1.23) failed!", row.getInt(1) == 2);
        assertTrue("CEILING(-1.23) failed!", row.getInt(2) == -1);
    }

    private void round(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "ROUND(-1.23),ROUND(1.298, 1) ");
        assertTrue("ROUND(-1.23) failed!", row.getInt(1) == -1);
        assertTrue("ROUND(1.298, 1) failed!", row.getDouble(2) == 1.3);
    }

    private void exp(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "EXP(2),EXP(-2) ");
        assertTrue("EXP(2) failed!", (float) row.getDouble(1) == (float) java.lang.Math.exp(2));
        assertTrue("EXP(-2) failed!", (float) row.getDouble(2) == (float) java.lang.Math.exp(-2));
    }

    private void log(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "LOG(2),LOG(-2) ");
        assertTrue("LOG(2) failed!", (float) row.getDouble(1) == (float) java.lang.Math.log(2));
        row.getDouble(2);
        assertTrue("LOG(-2) failed!", row.wasNull());
    }

    private void log10(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "LOG10(100) ");
        assertTrue("LOG10(100) failed!", row.getDouble(1) == 2.0);
    }

    private void pow(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "POWER(2,2) ");
        assertTrue("POWER(2,2) failed!", row.getDouble(1) == 4.0);
    }

    private void sqrt(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "SQRT(4) ");
        assertTrue("SQRT(4) failed!", row.getDouble(1) == 2.0);
    }

    private void pi(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "PI() ");
        assertTrue("PI() failed!", (float) row.getDouble(1) == (float) java.lang.Math.PI);
    }

    private void cos(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "COS(PI()) ");
        assertTrue("COS(PI()) failed!", row.getDouble(1) == -1.0);
    }

    private void sin(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "SIN(2) ");
        assertTrue("SIN(PI()) failed!", (float) row.getDouble(1) == (float) java.lang.Math.sin(2));
    }

    private void tan(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "TAN(PI()+1) ");
        assertTrue("TAN(PI()+1) failed!", (float) row.getDouble(1) == (float) java.lang.Math.tan(java.lang.Math.PI + 1.0));
    }

    private void acos(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "ACOS(1) ");
        assertTrue("ACOS(1) failed!", (float) row.getDouble(1) == 0.0);
    }

    private void asin(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "ASIN(0) ");
        assertTrue("ASIN(0) failed!", (float) row.getDouble(1) == (float) java.lang.Math.asin(0.0));
    }

    private void atan(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "ATAN(0) ");
        assertTrue("ATAN(0) failed!", row.getDouble(1) == 0.0);
    }

    private void atan2(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "ATAN2(0,2) ");
        assertTrue("ATAN2(0,2) failed!", (float) row.getDouble(1) == 0.0);
    }

    private void degrees(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "DEGREES(PI()) ");
        assertTrue("DEGREES(PI()) failed!", row.getDouble(1) == 180.0);
    }

    private void radians(final XRowSet xRowRes) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRow row = execute(xRowRes, "RADIANS(90) ");
        assertTrue("RADIANS(90) failed!", (float) row.getDouble(1) == (float) (java.lang.Math.PI / 2.0));
    }
}
