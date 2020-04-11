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



package com.sun.star.lib.uno.helper;
import java.lang.ref.WeakReference;
import com.sun.star.uno.XAdapter;
import com.sun.star.uno.XReference;
import java.util.List;
import java.util.Collections;
import java.util.LinkedList;

/** An XAdapter implementation that holds a weak reference (java.lang.ref.WeakReference) 
 *  to an object. Clients can register listener (com.sun.star.lang.XReference) which 
 *  are notified when the the object (the one which is kept weak) is being finalized. That 
 *  is, that object is being destroyed because there are not any hard references 
 *  to it.
 */
public class WeakAdapter implements XAdapter
{
    private final boolean DEBUG= false;
    // references the XWeak implementation
    private WeakReference m_weakRef;
    // contains XReference objects registered by addReference
    private List m_xreferenceList;
    
    /**
     *@param component the object that is to be held weak
     */
    public WeakAdapter(Object component)
    {
        m_weakRef= new WeakReference(component);
        m_xreferenceList= Collections.synchronizedList( new LinkedList());
    }
    
    /** Called by the XWeak implementation (WeakBase) when it is being finalized.
     *  It is only being called once.
     *  The registererd XReference listeners are notified. On notification they are 
     *  to unregister themselves. The notification is thread-safe. However, it is possible 
     *  to add a listener during the notification process, which will never receive a 
     *  notification. To prevent this, one would have to synchronize this method with 
     *  the addReference method. But this can result in deadlocks in a multithreaded 
     *  environment.
     */
    void referentDying()
    {
        //synchronized call
        Object[] references= m_xreferenceList.toArray();
        for (int i= references.length; i > 0; i--)
        {
            ((XReference) references[i-1]).dispose();
        }
    }
    
    /** Method  of com.sun.star.uno.XAdapter. It is called to obtain a hard reference
     *  to the object which is kept weak by this instance.
     *  @return hard reference to the object
     */
    public Object queryAdapted()
    {
        return m_weakRef.get();
    }

    /** Method of com.sun.star.uno.XAdapter. Called by clients to register listener which
     * are notified when the weak object is dying.
     *@param xReference a listener
     */
    public void removeReference(XReference xReference)
    {
        m_xreferenceList.remove(xReference);
    }

    /** Method of com.sun.star.uno.XAdapter. Called by clients to unregister listeners.
     *@param xReference listener
     */
    public void addReference(XReference xReference)
    {
        m_xreferenceList.add(xReference);
    }
}

