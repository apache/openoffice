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

import com.sun.star.comp.helper.Bootstrap;
import com.sun.star.container.XSet;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;

import org.junit.Test;

public final class LocalServiceTest extends TestBase {
    @Test
    public void test() throws Exception {
        super.runTest();
    }

    protected TestServiceFactory getTestServiceFactory() throws Exception {
        return new TestServiceFactory() {
                public Object get() throws Exception {
                    XComponentContext context
                        = Bootstrap.createInitialComponentContext(null);
                    XMultiComponentFactory serviceManager
                        = context.getServiceManager();
                    UnoRuntime.queryInterface(XSet.class, serviceManager).
                        insert(new TestService());
                    return serviceManager.createInstanceWithContext(
                        "testtools.servicetests.TestService2", context);
                }

                public void dispose() throws Exception {}
            };
    }
}
