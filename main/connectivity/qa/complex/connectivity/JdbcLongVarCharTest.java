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


package complex.connectivity;

import com.sun.star.beans.PropertyState;
import com.sun.star.beans.PropertyValue;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sdbc.XResultSet;
import com.sun.star.sdbc.XClob;
import com.sun.star.sdbc.XDriverAccess;
import com.sun.star.sdbc.XParameters;
import com.sun.star.sdbc.XPreparedStatement;
import com.sun.star.sdbc.XResultSetMetaData;
import com.sun.star.sdbc.XResultSetMetaDataSupplier;
import com.sun.star.sdbc.XRow;
import com.sun.star.uno.UnoRuntime;
import org.openoffice.test.OfficeConnection;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

public class JdbcLongVarCharTest
{
    private static final OfficeConnection connection = new OfficeConnection();

    @BeforeClass
    public static void beforeClass() throws Exception
    {
        connection.setUp();
    }

    @AfterClass
    public static void afterClass() throws Exception
    {
        connection.tearDown();
    }

    @Test
    public void testLongVarChar() throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {

        try
        {
            System.out.println("== Start testing ==");

            String url = "jdbc:mysql://localhost:3306/mysql?user=root";
            //String url = "jdbc:ingres://localhost:II7/demodb;AUTO=multi";
            com.sun.star.sdbc.XConnection xConnection = null;
            com.sun.star.beans.PropertyValue prop[] = new PropertyValue[1];
            prop[0] = new PropertyValue("JavaDriverClass", 0, "com.mysql.jdbc.Driver", PropertyState.DIRECT_VALUE);
            //prop[0] = new PropertyValue("JavaDriverClass", 0, "com.ingres.jdbc.IngresDriver", PropertyState.DIRECT_VALUE);

            // get the remote office component context
            XMultiServiceFactory xServiceManager = UnoRuntime.queryInterface(XMultiServiceFactory.class, connection.getComponentContext().getServiceManager());
            Object x = xServiceManager.createInstance("com.sun.star.sdbc.DriverManager");
            com.sun.star.sdbc.XDriverAccess xDriverAccess = (XDriverAccess) UnoRuntime.queryInterface(XDriverAccess.class, x);
            com.sun.star.sdbc.XDriver xDriver = xDriverAccess.getDriverByURL(url);
            xConnection = xDriver.connect(url, prop);

            //Object prepStmnt = xConnection.prepareStatement("SELECT * FROM t1 WHERE t1.c1 = ?");
            Object prepStmnt = xConnection.prepareStatement("SELECT * FROM i90114 WHERE i90114.c1 = ?");
            ((XParameters) UnoRuntime.queryInterface(XParameters.class, prepStmnt)).clearParameters();
            ((XParameters) UnoRuntime.queryInterface(XParameters.class, prepStmnt)).setInt(1, 1);
            XResultSet xResultSet = ((XPreparedStatement) prepStmnt).executeQuery();
            XRow xRow = (XRow) UnoRuntime.queryInterface(XRow.class, xResultSet);

            XResultSetMetaDataSupplier xRsMetaSup = (XResultSetMetaDataSupplier) UnoRuntime.queryInterface(XResultSetMetaDataSupplier.class, xResultSet);
            XResultSetMetaData xRsMetaData = xRsMetaSup.getMetaData();
            int nColumnCount = xRsMetaData.getColumnCount();

            System.out.println("== MetaData ==");
            for (int i = 1; i <= nColumnCount; ++i)
            {
                System.out.println("Name: " + xRsMetaData.getColumnName(i) + " Type: " +
                        xRsMetaData.getColumnType(i));
            }

            System.out.println("== Result ==");
            while (xResultSet.next())
            {
                String str = "not set";

                XClob xClob = null;
                xClob = xRow.getClob(2);
                if (xClob != null)
                {
                    System.out.println("xClob != null");
                    int len = (int) xClob.length();
                    str = xClob.getSubString(1, len);
                }
                else
                {
                    System.out.println("xClob == null");
                }

                System.out.println("c1 (Int): " + xRow.getInt(1) + " c2 (String): " + xRow.getString(2) + " c3 (Clob): " + str);
            }

            xConnection.close();
        }
        catch (java.lang.Exception e)
        {
            System.out.println("== Exception occurred while testing ==");
            e.printStackTrace();
        } finally
        {
            System.out.println("== End testing ==");
            System.exit(0);
        }
    }
}
