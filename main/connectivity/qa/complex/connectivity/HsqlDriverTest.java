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

import complex.connectivity.hsqldb.TestCacheSize;
import com.sun.star.frame.XModel;
import com.sun.star.frame.XStorable;

import com.sun.star.lang.*;
import com.sun.star.document.XDocumentSubStorageSupplier;

 
import org.hsqldb.lib.StopWatch;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.beans.PropertyState;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.embed.XStorage;
import com.sun.star.sdbc.XDataSource;
import com.sun.star.sdbc.XDriver;
import connectivity.tools.HsqlDatabase;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import org.openoffice.test.OfficeConnection;

public class HsqlDriverTest {
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
    public void test(){
        XDataSource ds = null;
        System.gc();
        XMultiServiceFactory xMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class, connection.getComponentContext().getServiceManager());
        try {
            HsqlDatabase database = new HsqlDatabase( xMSF );
            ds = database.getDataSource().getXDataSource();
        } catch(Exception ex) {
            throw new RuntimeException("factory: unable to construct data source" );
        }
        
        try{
            XDocumentSubStorageSupplier doc = (XDocumentSubStorageSupplier)UnoRuntime.queryInterface(XDocumentSubStorageSupplier.class,ds);
            XStorage stor = doc.getDocumentSubStorage("database",4);
            try{
            if ( stor.isStreamElement("db.log") )
                stor.removeElement("db.log");
            } catch(Exception e){}
            try{
            if ( stor.isStreamElement("db.properties") )
                stor.removeElement("db.properties");
            } catch(Exception e){}
            try{
            if ( stor.isStreamElement("db.script") )
                stor.removeElement("db.script");
            } catch(Exception e){}
            try{
            if ( stor.isStreamElement("db.script.new") )
                stor.removeElement("db.script.new");
            } catch(Exception e){}
            XStorable mod = (XStorable)UnoRuntime.queryInterface(XStorable.class,ds);
            mod.store();
            XComponent xComp = (XComponent)UnoRuntime.queryInterface(XComponent.class,stor);
            if ( xComp != null )
                xComp.dispose();
        } catch(Exception e){}

        com.sun.star.beans.PropertyValue[] info = null;
        XDriver drv = null;
        try{
            XDocumentSubStorageSupplier doc = (XDocumentSubStorageSupplier)UnoRuntime.queryInterface(XDocumentSubStorageSupplier.class,ds);
            XModel mod = (XModel)UnoRuntime.queryInterface(XModel.class,ds);
            XStorage stor = doc.getDocumentSubStorage("database",4);
            info = new com.sun.star.beans.PropertyValue[]{
                new com.sun.star.beans.PropertyValue("Storage",0,stor,PropertyState.DIRECT_VALUE)
                ,new com.sun.star.beans.PropertyValue("URL",0,mod.getURL(),PropertyState.DIRECT_VALUE)
            };        
            drv = (XDriver)UnoRuntime.queryInterface(XDriver.class, xMSF.createInstance("com.sun.star.sdbcx.comp.hsqldb.Driver"));
        

            TestCacheSize test = new TestCacheSize(xMSF,info,drv);

            StopWatch     sw   = new StopWatch();      

            try{
                test.setUp();
                test.testFillUp();
                test.checkResults();
                test.tearDown();
                System.out.println("Total Test Time: " + sw.elapsedTime());
            } catch(Exception e){}

            try{
                XStorable mod2 = (XStorable)UnoRuntime.queryInterface(XStorable.class,ds);
                mod2.store();
            } catch(Exception e){}
        }catch(Exception e){}
    }
    public void test2(){
        System.gc();
       
        com.sun.star.beans.PropertyValue[] info = null;
        XDriver drv = null;
        XMultiServiceFactory xMSF = UnoRuntime.queryInterface(XMultiServiceFactory.class, connection.getComponentContext().getServiceManager());
        try{
            info = new com.sun.star.beans.PropertyValue[]{
                new com.sun.star.beans.PropertyValue("JavaDriverClass",0,"org.hsqldb.jdbcDriver",PropertyState.DIRECT_VALUE)
                ,new com.sun.star.beans.PropertyValue("ParameterNameSubstitution",0, false,PropertyState.DIRECT_VALUE)
            };        
            drv = (XDriver)UnoRuntime.queryInterface(XDriver.class,xMSF.createInstance("com.sun.star.comp.sdbc.JDBCDriver"));
            TestCacheSize test = new TestCacheSize(xMSF,info,drv);
            test.setURL("jdbc:hsqldb:g:\\hsql\\db");


            StopWatch     sw   = new StopWatch();      

            try{
                test.setUp();
                test.testFillUp();
                test.checkResults();
                test.tearDown();
                System.out.println("Total Test Time: " + sw.elapsedTime());
            } catch(Exception e){}
        }catch(Exception e){}
    }
}
