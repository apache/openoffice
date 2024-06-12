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



package complex.numberformatter;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.XNumberFormatsSupplier;
import com.sun.star.util.XNumberFormatter;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.openoffice.test.OfficeConnection;
import static org.junit.Assert.*;

public class NumberFormatterUnitTest {
    private XMultiServiceFactory m_xMSF = null;
    
    @Before public void before() {
        try {
            m_xMSF = getMSF();
        } catch (Exception e) {
            fail ("Cannot create service factory!");
        }
        if (m_xMSF == null) {
            fail ("Cannot create service factory!");
        }
    }
    
    @After public void after() {
        m_xMSF = null;
    }
    
    @Test
    public void testDollarDotNumberFormat() throws Exception
    {
        Object numberFormatterService = m_xMSF.createInstance( "com.sun.star.util.NumberFormatter" );
        XNumberFormatter numberFormatter = UnoRuntime.queryInterface( XNumberFormatter.class, numberFormatterService );
        Object numberFormatsSupplierService = m_xMSF.createInstance( "com.sun.star.util.NumberFormatsSupplier" );
        XNumberFormatsSupplier numberFormatsSupplier = UnoRuntime.queryInterface( XNumberFormatsSupplier.class, numberFormatsSupplierService );
        numberFormatter.attachNumberFormatsSupplier( numberFormatsSupplier );

        // Bug 82687 - Text entry or CSV import treats $.nn as text
        double number = numberFormatter.convertStringToNumber(0, "$.1");
        assertEquals(0.1, number, 0.00001);
    }

    private XMultiServiceFactory getMSF()
    {
        final XMultiServiceFactory xMSF1 = UnoRuntime.queryInterface(XMultiServiceFactory.class, connection.getComponentContext().getServiceManager());
        return xMSF1;
    }

    // setup and close connections
    @BeforeClass public static void setUpConnection() throws Exception {
        System.out.println("setUpConnection()");
        connection.setUp();
    }

    @AfterClass public static void tearDownConnection()
        throws InterruptedException, com.sun.star.uno.Exception
    {
        System.out.println("tearDownConnection()");
        connection.tearDown();
    }

    private static final OfficeConnection connection = new OfficeConnection();

}
