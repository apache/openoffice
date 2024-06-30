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



package com.sun.star.lib.util;

import util.WaitUnreachable;

import org.junit.Test;
import static org.junit.Assert.*;

public final class WeakMap_Test {
    @Test
    public void test() {
        WeakMap m = new WeakMap();
        assertTrue("", m.size() == 0);
        assertTrue("", m.isEmpty());
        assertTrue("", !m.containsKey("key1"));
        assertTrue("", !m.containsValue(null));
        WaitUnreachable u1 = new WaitUnreachable(new Object());
        m.put("key1", u1.get());
        WaitUnreachable u2 = new WaitUnreachable(new Disposable());
        m.put("key2", u2.get());
        assertTrue("", m.size() == 2);
        assertTrue("", !m.isEmpty());
        assertTrue("", m.containsKey("key1"));
        assertTrue("", m.containsKey("key2"));
        assertTrue("", !m.containsKey("key3"));
        assertTrue("", m.containsValue(m.get("key1")));
        assertTrue("", m.containsValue(m.get("key2")));
        assertTrue("", WeakMap.getValue(m.get("key1")).equals(u1.get()));
        assertTrue("", WeakMap.getValue(m.get("key2")).equals(u2.get()));
        assertTrue("", m.values().size() == 2);
        assertTrue("", m.values().contains(m.get("key1")));
        assertTrue("", m.values().contains(m.get("key2")));
        u1.waitUnreachable();
        assertTrue("", WeakMap.getValue(m.get("key1")) == null);
        ((Disposable) u2.get()).dispose();
        assertTrue("", WeakMap.getValue(m.get("key2")) == null);
        m.clear();
        u2.waitUnreachable();
        assertTrue("", m.size() == 0);
        m.put("key2", new Object());
        assertTrue("", m.size() == 1);
    }

    // This simple class (single listener, no synchronization) exploits
    // knowledge about the implementation of WeakMap:
    private static final class Disposable implements DisposeNotifier {
        public void addDisposeListener(DisposeListener listener) {
            this.listener = listener;
        }

        public void dispose() {
            if (listener != null) {
                listener.notifyDispose(this);
            }
        }

        private DisposeListener listener = null;
    }
}
