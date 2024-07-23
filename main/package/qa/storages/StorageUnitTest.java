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


package complex.storages;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.connection.XConnector;
import com.sun.star.connection.XConnection;

import com.sun.star.bridge.XUnoUrlResolver;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import com.sun.star.uno.XNamingService;
import com.sun.star.uno.XComponentContext;

import com.sun.star.container.*;
import com.sun.star.beans.*;
import com.sun.star.lang.*;

import complex.storages.*;
import share.LogWriter;
import stats.SimpleLogWriter;

import java.util.*;
import java.io.*;

import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.openoffice.test.OfficeConnection;
import static org.junit.Assert.*;

/* This unit test for storage objects is designed to
 * test most important statements from storage service
 * specification.
 *
 * Regression tests are added to extend the tested
 * functionalities.
 */
public class StorageUnitTest
{
    private static final OfficeConnection connection = new OfficeConnection();
    private static final LogWriter log = new SimpleLogWriter();
	private XMultiServiceFactory m_xMSF = null;
	private XSingleServiceFactory m_xStorageFactory = null;

    @BeforeClass
    public static void setUpConnection()
        throws Exception
    {
        connection.setUp();
        log.initialize(null, true);
    }

    @AfterClass
    public static void tearDownConnection()
        throws InterruptedException, com.sun.star.uno.Exception
    {
        connection.tearDown();
    }

    @Before
    public void before()
	{
        m_xMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class, connection.getComponentContext().getServiceManager());
		if ( m_xMSF == null )
		{
			fail( "Can't create service factory!" );
			return;
		}

		try {
			Object oStorageFactory = m_xMSF.createInstance( "com.sun.star.embed.StorageFactory" );
			m_xStorageFactory = (XSingleServiceFactory)UnoRuntime.queryInterface( XSingleServiceFactory.class,
																				oStorageFactory );
		}
		catch( Exception e )
		{
			fail( "Can't create storage factory!" );
			return;
		}

		if ( m_xStorageFactory == null )
		{
			fail( "Can't create service factory!" );
			return;
		}
    }

    @Test
	public void ExecuteTest01()
	{
		StorageTest aTest = new Test01( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test01 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest02()
	{
		StorageTest aTest = new Test02( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test02 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest03()
	{
		StorageTest aTest = new Test03( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test03 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest04()
	{
		StorageTest aTest = new Test04( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test04 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest05()
	{
		StorageTest aTest = new Test05( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test05 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest06()
	{
		StorageTest aTest = new Test06( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test06 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest07()
	{
		StorageTest aTest = new Test07( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test07 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest08()
	{
		StorageTest aTest = new Test08( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test08 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest09()
	{
		StorageTest aTest = new Test09( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test09 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest10()
	{
		StorageTest aTest = new Test10( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test10 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest11()
	{
		StorageTest aTest = new Test11( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test11 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest12()
	{
		StorageTest aTest = new Test12( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test12 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest13()
	{
		StorageTest aTest = new Test13( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test13 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest14()
	{
		StorageTest aTest = new Test14( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test14 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest15()
	{
		StorageTest aTest = new Test15( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test15 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest16()
	{
		StorageTest aTest = new Test16( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test16 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest17()
	{
		StorageTest aTest = new Test17( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test17 failed!", aTest.test() );
	}

    @Test
	public void ExecuteTest18()
	{
		StorageTest aTest = new Test18( m_xMSF, m_xStorageFactory, log );
		assertTrue( "Test18 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_114358()
	{
		StorageTest aTest = new RegressionTest_114358( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_114358 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_i29169()
	{
		StorageTest aTest = new RegressionTest_i29169( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_i29169 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_i30400()
	{
		StorageTest aTest = new RegressionTest_i30400( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_i30400 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_i29321()
	{
		StorageTest aTest = new RegressionTest_i29321( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_i29321 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_i30677()
	{
		StorageTest aTest = new RegressionTest_i30677( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_i30677 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_i27773()
	{
		StorageTest aTest = new RegressionTest_i27773( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_i27773 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_i46848()
	{
		StorageTest aTest = new RegressionTest_i46848( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_i46848 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_i55821()
	{
		StorageTest aTest = new RegressionTest_i55821( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_i55821 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_i35095()
	{
		StorageTest aTest = new RegressionTest_i35095( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_i35095 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_i49755()
	{
		StorageTest aTest = new RegressionTest_i49755( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_i49755 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_i59886()
	{
		StorageTest aTest = new RegressionTest_i59886( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_i59886 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_i61909()
	{
		StorageTest aTest = new RegressionTest_i61909( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_i61909 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_i84234()
	{
		StorageTest aTest = new RegressionTest_i84234( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_i84234 failed!", aTest.test() );
	}

    @Test
	public void ExecuteRegressionTest_125919()
	{
		StorageTest aTest = new RegressionTest_125919( m_xMSF, m_xStorageFactory, log );
		assertTrue( "RegressionTest_125919 failed!", aTest.test() );
	}
}

