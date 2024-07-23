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

public class DBaseSqlTests
{
    private final XMultiServiceFactory m_xORB;

    public DBaseSqlTests(final XMultiServiceFactory _xORB)
    {
        m_xORB = _xORB;
    }

    public void testFunctions() throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        final XRowSet xRowRes = (XRowSet) UnoRuntime.queryInterface(XRowSet.class,
                m_xORB.createInstance("com.sun.star.sdb.RowSet"));

        System.out.println("starting SQL test");
        // set the properties needed to connect to a database
        final XPropertySet xProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xRowRes);
        xProp.setPropertyValue("DataSourceName", "Bibliography");
        xProp.setPropertyValue("CommandType", Integer.valueOf(com.sun.star.sdb.CommandType.COMMAND));

        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where \"Identifier\" like 'B%'");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where not \"Identifier\" like 'B%'");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where not \"Identifier\" not like 'B%'");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where not(0 = 1)");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where 0 = 0");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where (0 = 0)");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where 0 <> 1");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where 0 < 1");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where 2 > 1");
        execute(xRowRes,"1,1+1,'a' + 'b' FROM \"biblio\" \"biblio\" where 2 > 1");
        // execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where (0 = 0) is true");
        // execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where not (0 = 0) is not true");
        // execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where 1 between 0 and 2");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where not \"Identifier\" is NULL");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where \"Identifier\" is not NULL");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where \"Identifier\" = \"Identifier\"");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where not(not(\"Identifier\" = \"Identifier\"))");
        execute(xRowRes,"1 FROM \"biblio\" \"biblio\" where (1 = 1 and 2 = 1) or 3 = 33 or 4 = 44 or ('a' = 'a' and 'b' = 'b')");
    }

    private void execute(final XRowSet xRowRes, String sql) throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        try
        {
            final XPropertySet xProp = (XPropertySet) UnoRuntime.queryInterface(XPropertySet.class, xRowRes);
            xProp.setPropertyValue("Command", "SELECT " + sql);
            xRowRes.execute();
        }
        catch(SQLException e)
        {
            System.err.println(sql + " Error: " + e.getMessage());
        }
    }

    
}
