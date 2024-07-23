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



package testtools.servicetests;

import com.sun.star.uno.UnoRuntime;
import util.WaitUnreachable;

import static org.junit.Assert.*;

public abstract class TestBase {
    public final void runTest() throws Exception {
        TestServiceFactory factory = getTestServiceFactory();
        TestService2 t = UnoRuntime.queryInterface(
            TestService2.class, factory.get());
        assertTrue(t != null);
        assertTrue(UnoRuntime.queryInterface(TestService1.class, t) == t);
        assertTrue(UnoRuntime.queryInterface(XTestService1.class, t) == t);
        assertTrue(UnoRuntime.queryInterface(XTestService2.class, t) == t);
        assertTrue(t.fn1() == 1);
        assertTrue(t.getProp1() == 1);
        t.setProp1(0);
        assertTrue(t.getProp1() == 0);
        assertTrue(t.getProp2() == 2);
        /*try {
            t.getProp3Void();
            fail();
        } catch (VoidPropertyException e) {
        }*/
        assertTrue(t.getProp3Long() == 3);
        /*try {
            t.getProp4None();
            fail();
        } catch (OptionalPropertyException e) {
        }*/
        assertTrue(t.getProp4Long() == 4);
        /*try {
            t.getProp5None();
            fail();
        } catch (OptionalPropertyException e) {
        }
        try {
            t.getProp5Void();
            fail();
        } catch (VoidPropertyException e) {
        }*/
        assertTrue(t.getProp5Long() == 5);
        assertTrue(t.getProp6() == 6);
        /*t.clearProp6();
        try {
            t.getProp6();
            fail();
        } catch (VoidPropertyException e) {
        }*/
        t.setProp6(0);
        assertTrue(t.getProp6() == 0);
        /*try {
            t.getProp7None();
            fail();
        } catch (OptionalPropertyException e) {
        }
        try {
            t.setProp7None(0);
            fail();
        } catch (OptionalPropertyException e) {
        }
        try {
            t.clearProp7None();
            fail();
        } catch (OptionalPropertyException e) {
        }*/
        assertTrue(t.getProp7() == 7);
        /*t.clearProp7();
        try {
            t.getProp7();
            fail();
        } catch (VoidPropertyException e) {
        }*/
        t.setProp7(0);
        assertTrue(t.getProp7() == 0);
        /*try {
            t.getProp8None();
            fail();
        } catch (OptionalPropertyException e) {
        }
        try {
            t.setProp8None(0);
            fail();
        } catch (OptionalPropertyException e) {
        }*/
        assertTrue(t.getProp8Long() == 8);
        t.setProp8Long(0);
        assertTrue(t.getProp8Long() == 0);
        assertTrue(t.fn2() == 2);
        XTestService3 t3 = UnoRuntime.queryInterface(XTestService3.class, t);
        assertTrue(t3 != null);
        assertTrue(t3.fn3() == 3);
        XTestService4 t4 = UnoRuntime.queryInterface(XTestService4.class, t);
        assertTrue(t4 == null);
        WaitUnreachable u = new WaitUnreachable(t);
        t = null;
        WaitUnreachable.ensureFinalization(t3);
        t3 = null;
        WaitUnreachable.ensureFinalization(t4);
        t4 = null;
        u.waitUnreachable();
        factory.dispose();
    }

    protected abstract TestServiceFactory getTestServiceFactory()
        throws Exception;

    protected interface TestServiceFactory {
        Object get() throws Exception;

        void dispose() throws Exception;
    }
}
