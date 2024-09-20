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


package complex.olesimplestorage;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;


import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.openoffice.test.OfficeConnection;
import static org.junit.Assert.*;

/* Document.
 */

public class OLESimpleStorageUnitTest
{
    private XMultiServiceFactory m_xMSF = null;
    
    @Before public void before () {
        System.out.println("before()");
        try {
            m_xMSF = getMSF();
        } catch ( Exception e ){
            fail( "Cannot create service factory!" );
        }
        if ( m_xMSF == null ) {
            fail( "Cannot create service factory!" );
        }
    }
    
    @After public void after () {
        System.out.println("after()");
        m_xMSF = null;
    }
    
    @Test public void ExecuteTest01() {
        System.out.println("ExecuteTest01()");
        OLESimpleStorageTest aTest = new Test01( m_xMSF );
        assertTrue( "Test01 failed!", aTest.test() );
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
//        try
//        {
//            Thread.sleep(5000);
//        }
//        catch (java.lang.InterruptedException e)
//        {
//        }
        System.out.println("tearDownConnection()");
        connection.tearDown();
    }

    private static final OfficeConnection connection = new OfficeConnection();

}
