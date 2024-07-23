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

import com.sun.star.comp.connections.PipedConnection;
import util.WaitUnreachable;

import org.junit.Test;
import static org.junit.Assert.*;

public final class UnoRuntime_EnvironmentTest {
    @Test
    public void test_getEnvironment() throws java.lang.Exception {
        Object o1 = new Object();
        Object o2 = new Object();

        // get two environments with different contexts
        WaitUnreachable java_environment1 = new WaitUnreachable(
            UnoRuntime.getEnvironment("java", o1));
        WaitUnreachable java_environment2 = new WaitUnreachable(
            UnoRuntime.getEnvironment("java", o2));

        // ensure that the environments are different
        assertTrue("", java_environment1.get() != java_environment2.get());

        // test if we get the same environment when we reget it
        assertTrue("",
               UnoRuntime.areSame(java_environment1.get(),
                                  UnoRuntime.getEnvironment("java", o1)));
        assertTrue("",
               UnoRuntime.areSame(java_environment2.get(),
                                  UnoRuntime.getEnvironment("java", o2)));

        // drop the environments and wait until they are gc
        java_environment1.waitUnreachable();
        java_environment2.waitUnreachable();
    }

    @Test
    public void test_getBridge() throws java.lang.Exception {
        PipedConnection conn = new PipedConnection(new Object[0]);
        new PipedConnection(new Object[] { conn });

        // get a bridge
        IBridge iBridge = UnoRuntime.getBridgeByName(
            "java", null, "remote", "testname",
            new Object[] { "urp", conn, null });

        // reget the bridge, it must be the same as above
        IBridge iBridge_tmp = UnoRuntime.getBridgeByName(
            "java", null, "remote", "testname",
            new Object[] { "urp", conn, null });
        assertTrue("", UnoRuntime.areSame(iBridge_tmp, iBridge));

        // dispose the bridge, this removes the entry from the runtime
        iBridge.dispose();

        conn = new PipedConnection(new Object[0]);
        new PipedConnection(new Object[] { conn });

        // reget the bridge, it must be a different one
        iBridge_tmp = UnoRuntime.getBridgeByName(
            "java", null, "remote", "testname",
            new Object[]{ "urp", conn, null });
        assertTrue("", !UnoRuntime.areSame(iBridge_tmp, iBridge));
    }
}
