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

import complex.connectivity.dbase.DBaseDateFunctions;
import complex.connectivity.dbase.DBaseStringFunctions;
import complex.connectivity.dbase.DBaseSqlTests;
import complex.connectivity.dbase.DBaseNumericFunctions;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import org.openoffice.test.Argument;
import org.openoffice.test.OfficeConnection;

public class DBaseDriverTest
{
    private static final OfficeConnection connection = new OfficeConnection();

    @BeforeClass
    public static void beforeClass() throws Exception {
        connection.setUp();
    }

    @AfterClass
    public static void afterClass() throws Exception {
        connection.tearDown();
    }

    @Test
    public void Functions() throws com.sun.star.uno.Exception, com.sun.star.beans.UnknownPropertyException
    {
        XMultiServiceFactory xMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class, connection.getComponentContext().getServiceManager());

        DBaseStringFunctions aStringTest = new DBaseStringFunctions(xMSF);
        aStringTest.testFunctions();

        DBaseNumericFunctions aNumericTest = new DBaseNumericFunctions(xMSF);
        aNumericTest.testFunctions();

        DBaseDateFunctions aDateTest = new DBaseDateFunctions(xMSF);
        aDateTest.testFunctions();

        DBaseSqlTests aSqlTest = new DBaseSqlTests(xMSF);
        aSqlTest.testFunctions();
    }
}
