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

import java.util.LinkedList;

/**
   Helper class to asynchronously execute finalize methods.

   Current JVMs seem not to be robust against long-running finalize methods, in
   that such long-running finalize methods may lead to OutOfMemoryErrors.  This
   class mitigates the problem by asynchronously shifting the bodies of
   potentially long-running finalize methods into an extra thread.  Classes that
   make use of this in their finalize methods are the proxies used in the
   intra-process JNI UNO bridge and the inter-process Java URP UNO bridge (where
   in both cases finalizers lead to synchronous UNO release calls).

   If JVMs are getting more mature and should no longer have problems with
   long-running finalize methods, this class could be removed again.
*/
public final class AsynchronousFinalizer {
    /**
       Add a job to be executed asynchronously.

       The run method of the given job is called exactly once.  If it terminates
       abnormally by throwing any Throwable, that is ignored.

       @param job represents the body of some finalize method; must not be null.
    */
    public static void add(Job job) {
        synchronized (queue) {
            boolean first = queue.isEmpty();
            queue.add(job);
            if (first) {
                queue.notify();
            }
        }
    }

    /**
       An interface to represent bodies of finalize methods.

       Similar to Runnable, except that the run method may throw any Throwable
       (which is effectively ignored by AsynchronousFinalizer.add, similar to
       any Throwables raised by finalize being ignored).
    */
    public interface Job {
        void run() throws Throwable;
    }

    private static final LinkedList queue = new LinkedList();

    static {
        Thread t = new Thread() {
                public void run() {
                    for (;;) {
                        Job j;
                        synchronized (queue) {
                            while (queue.isEmpty()) {
                                try {
                                    queue.wait();
                                } catch (InterruptedException e) {}
                            }
                            j = (Job) queue.remove(0);
                        }
                        try {
                            j.run();
                        } catch (Throwable e) {}
                    }
                }
            };
        t.setDaemon(true);
        t.start();
    }

    private AsynchronousFinalizer() {}
}
