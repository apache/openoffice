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



package com.sun.star.lib.uno.bridges.javaremote;

import com.sun.star.bridge.XInstanceProvider;
import com.sun.star.lang.DisposedException;
import com.sun.star.lib.TestBed;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;

import org.junit.Test;
import static org.junit.Assert.*;

/**
 * Test case for bug #98508#.
 *
 * <p>Bug #98508# "JAVA UNO bridge is not disposed when Exception occurs during
 * sendReply()" states that the server returning <code>null</code> instead of a
 * valid <code>String</code> from <code>XServiceName.getServiceName</code>
 * causes an exception when sending the reply, but this exception did not cause
 * the bridge to be disposed, it rather caused both client and server to
 * hang.</p>
 *
 * <p>Since null instead of a <code>String</code> no longer causes an exception
 * in the bridge, this test has been redesigned to send a value of a wrong
 * instantiated polymorphic struct type instead.</p>
 *
 * <p>This test has to detect whether the spawned client process indeed hangs,
 * which can not be done reliably.  As an approximation, it waits for 10 sec and
 * considers the process hanging if it has not completed by then.</p>
 */
public final class Bug98508_Test {
    @Test
    public void test() throws Exception {
        TestBed t = new TestBed();
        assertTrue("test", t.execute(new Provider(t), true, Client.class, 10000));
    }

    public static final class Client extends TestBed.Client {
        public static void main(String[] args) {
            new Client().execute();
        }

        protected boolean run(XComponentContext context) throws Throwable {
            Test98508Interface ifc
                = UnoRuntime.queryInterface(
                    Test98508Interface.class,
                    getBridge(context).getInstance(""));
            try {
                ifc.get();
            } catch (DisposedException e) {
                return true;
            }
            return false;
        }
    }

    private static final class Provider implements XInstanceProvider {
        public Provider(TestBed testBed) {
            this.testBed = testBed;
        }

        public Object getInstance(String instanceName) {
            return new Test98508Interface() {
                    public Test98508Struct get() {
                        testBed.serverDone(true);
                        return new Test98508Struct(Boolean.FALSE);
                    }
                };
        }

        private final TestBed testBed;
    }
}
