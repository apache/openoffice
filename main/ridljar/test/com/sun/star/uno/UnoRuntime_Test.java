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



package com.sun.star.uno;

import com.sun.star.beans.Optional;

import org.junit.Test;
import static org.junit.Assert.*;

public final class UnoRuntime_Test {
    @Test
    public void test_generateOid() {
        // Test if UnoRuntime generates an OID for a simple class:
        assertTrue("Test1", UnoRuntime.generateOid(new Test1()) != null);

        // Test if UnoRuntime generates an OID for a class implementing
        // IQueryInterface and returning null from getOid:
        assertTrue("Test2", UnoRuntime.generateOid(new Test2()) != null);

        // Test if a delegator object has the same OID as its creator:
        Test4 test4 = new Test4();
        Ifc ifc = UnoRuntime.queryInterface(Ifc.class, test4);
        assertTrue(
            "Test4",
            UnoRuntime.generateOid(test4).equals(UnoRuntime.generateOid(ifc)));
    }

    @Test
    public void test_queryInterface() {
        // Test if a query for an interface which is not supported returns null:
        assertTrue(
            "Test1",
            UnoRuntime.queryInterface(Ifc.class, new Test1()) == null);

        // Test if a query for an interface which is supported through
        // IQueryInterface succeeds:
        assertTrue(
            "Test2",
            UnoRuntime.queryInterface(Ifc.class, new Test2()) != null);

        // Test if a query for an interface which is directly supported (through
        // inheritance) succeeds:
        assertTrue(
            "Test3",
            UnoRuntime.queryInterface(Ifc.class, new Test3()) != null);
    }

    @Test
    public void test_areSame() {
        assertTrue(
            UnoRuntime.areSame(
                new Any(Type.UNSIGNED_LONG, new Integer(3)),
                new Any(Type.UNSIGNED_LONG, new Integer(3))));
        assertTrue(
            !UnoRuntime.areSame(
                new Any(Type.UNSIGNED_LONG, new Integer(3)), new Integer(3)));
        assertTrue(!UnoRuntime.areSame(new int[] { 1 }, new int[] { 1, 2 }));
        assertTrue(
            UnoRuntime.areSame(
                TypeClass.UNSIGNED_LONG,
                new Any(new Type(TypeClass.class), TypeClass.UNSIGNED_LONG)));
        assertTrue(
            UnoRuntime.areSame(
                new Any(
                    new Type("com.sun.star.beans.Optional<unsigned long>"),
                    new Optional()),
                new Any(
                    new Type("com.sun.star.beans.Optional<unsigned long>"),
                    new Optional(false, new Integer(0)))));
        assertTrue(!UnoRuntime.areSame(new Test1(), new Test2()));
        Test2 test2 = new Test2();
        assertTrue(
            "Test2",
            UnoRuntime.areSame(
                UnoRuntime.queryInterface(Ifc.class, test2), test2));
    }

    @Test
    public void test_completeValue() {
        assertTrue(
            UnoRuntime.completeValue(Type.UNSIGNED_LONG, null).equals(
                new Integer(0)));
        Object v = UnoRuntime.completeValue(
            new Type("[][]unsigned long"), null);
        assertTrue(v instanceof int[][]);
        assertTrue(((int[][]) v).length == 0);
        assertTrue(
            UnoRuntime.completeValue(new Type(TypeClass.class), null) ==
            TypeClass.VOID);
        v = UnoRuntime.completeValue(
            new Type("com.sun.star.beans.Optional<unsigned long>"), null);
        assertTrue(v instanceof Optional);
        assertTrue(!((Optional) v).IsPresent);
        assertTrue(((Optional) v).Value == null);
    }

    @Test
    public void test_currentContext() throws InterruptedException {
        TestThread t1 = new TestThread();
        TestThread t2 = new TestThread();
        t1.start();
        t2.start();
        t1.join();
        t2.join();
        Object v1 = t1.context.getValueByName("");
        Object v2 = t2.context.getValueByName("");
        assertTrue("", t1.context != t2.context);
        assertTrue("", v1 == t1);
        assertTrue("", v2 == t2);
        assertTrue("", v1 != v2);
    }

    private interface Ifc extends XInterface {}

    private static class Test1 {}

    private static class Test2 implements XInterface, IQueryInterface {
        public String getOid() {
            return null;
        }

        public Object queryInterface(Type type) {
            return type.equals(new Type(Ifc.class)) ? t2 : null;
        }

        public boolean isSame(Object object) {
            return object == t2;
        }

        private static final class T2 implements Ifc {}

        private final T2 t2 = new T2();
    }

    private static class Test3 implements Ifc {}

    private static class Test4 implements XInterface, IQueryInterface {
        public String getOid() {
            return null;
        }

        public Object queryInterface(Type type) {
            return type.equals(new Type(Ifc.class)) ? t4 : null;
        }

        public boolean isSame(Object object) {
            return object == t4;
        }

        private final class T4 implements Ifc, IQueryInterface {
            public String getOid() {
                return UnoRuntime.generateOid(Test4.this);
            }

            public Object queryInterface(Type type) {
                return Test4.this.queryInterface(type);
            }

            public boolean isSame(Object object) {
                return UnoRuntime.areSame(Test4.this, object);
            }
        }

        private final T4 t4 = new T4();
    }

    private final class TestThread extends Thread {
        public void run() {
            assertTrue("", UnoRuntime.getCurrentContext() == null);
            context = new TestCurrentContext();
            UnoRuntime.setCurrentContext(context);
            assertTrue("", UnoRuntime.getCurrentContext() == context);
            assertTrue("", context.getValueByName("") == this);
            UnoRuntime.setCurrentContext(null);
            assertTrue("", UnoRuntime.getCurrentContext() == null);
        }

        public XCurrentContext context = null;
    }

    private static final class TestCurrentContext implements XCurrentContext {
        public Object getValueByName(String name) {
            return value;
        }

        private final Object value = Thread.currentThread();
    }
}
