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

import org.junit.Test;
import static org.junit.Assert.*;

public final class Any_Test {
    @Test
    public void testAnyAny() {
        boolean caught = false;
        try {
            new Any(Type.ANY, null);
        } catch (IllegalArgumentException e) {
            caught = true;
        }
        assertTrue(caught);
    }

    @Test
    public void testComplete() {
        assertTrue(Any.complete(Any.VOID) == Any.VOID);
        assertTrue(
            Any.complete(new Integer(10)).equals(
                new Any(Type.LONG, new Integer(10))));
        assertTrue(
            Any.complete(null).equals(
                new Any(new Type(XInterface.class), null)));
        XInterface x = new XInterface() {};
        assertTrue(Any.complete(x).equals(new Any(new Type(XInterface.class), x)));
    }
}
