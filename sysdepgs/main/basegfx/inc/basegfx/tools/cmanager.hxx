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

#ifndef _BGFX_CACHE_MANAGER_HXX
#define _BGFX_CACHE_MANAGER_HXX

#include <sal/types.h>
#include <comphelper/broadcasthelper.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace basegfx
{
    namespace cache
    {
        class node;
        class cacheable;

        // This caching mechanism for any C++ objects is based on three classes
        // in this namespace, which are: manager, node, cacheable. The roles of these
        // and how to use will be explained below.

        // This is the manager part of the caching mechanism. Any c++ object derived from the 
        // helper class cacheable (see this class in the same namespace) can be cached.
        //
        // An object derived from cacheable can be cached with a manager derived from this manager 
        // class. It will allow you to access cached entries using getEntry() which also resets the
        // lifetime counter if found. If not found the cached object will be incarnated and
        // added to the cache. To do this, the constructor/destructor of the class derived from
        // cacheable is used, where the basic constructor implementation inserts the instance to 
        // the manager and the destructor removes it.
        //
        // This mechanism uses the helper class node where the cache data management is done. It 
        // implements basic mechanisms for organizing the caching mechanism.
        // To use this, you need to derive your own classes from manager and from node
        // respectively.
        //
        // On the manager derivation:
        // - react on the cache being empty by overloading (onEmpty())
        // - react on getting the first element by overloading (onFilled())
        // - test if there are cached instances using empty()
        // - try to get a cached entry using getEntry()
        // - trigger the lifetime counter using trigger() which will remove
        //   cached objects with zero count (probably timer based in real usages,
        //   but this is not a must).
        // - implement a access method to your cached data type using getEntry().
        //   When you get one, it's the cached instance. If not, construct your
        //   instance of node (it will be added automatically) and return it.
        //
        // On the node derivation:
        // - Add a data entry to hold the cached data
        // - Implement creating that data from the original cached object in the
        //   constructor
        // - destroy it in the destuctor
        //
        // On the cacheable derivation:
        // - nothing to do, all managed from the namager/node combination
        //
        // To access your cached instance, you can use getEntry() at the manager instance
        // which may return the cached entry or a newly created one. On that instance,
        // access your cached data. If it was created or cached is transparent.
        // The insertion at the manager, the access to the instance and the removal at 
        // end of lifetime are all linear operations.
        // Note that multiple derivations of cacheable can be cached by a single derivation 
        // (and instance) of a manager; e.g. a manager might cache specialized forms of bitmap 
        // data and polygon data at the same time (or even more classes).
        // Usage examples would e.g. derive from manager and the Timer class in sal,
        // call trigger in the overloaded timer callback and switch the timer on/off
        // in overlods of onEmpty()/onFilled(). I will use it to e.g. cache Widows-specific
        // data creatable from system-independent bitmap data and/or polygon data.
        class manager : protected comphelper::OBaseMutex
        {
        private:
            friend class node;

            // lifetime counter used as start value for existing touched or
            // newly created cache entries
            sal_uInt32              mnLifetime;

            // the cache entries managed by this manager. This pointer is the anchor
            // to a double linked list. That double linked list uses pointers directly
            // in the node (mpNext, mpPrev), insertion and removal is done safely in
            // the private helpers removeNode/insertNode below. This is done self here
            // by purpose; any usage of an stl class would introduce some search access
            // to find the correct node in the removeNode case; this is *not* needed
            // when doing the list self since the node *is* already the entry to work
            // on and thus allows linear time operations.
            node*                   mpNodes;

            void removeNode(node& rNode);
            void insertNode(node& rNode);

        protected:
        public:
            // constructor allows to give a default lifetime count for newly
            // added cached objects
            manager(sal_uInt32 nLifetime = 60);
            virtual ~manager();

            // allow to react on last cached object removed (e.g. stop timer)
            virtual void onEmpty();

            // allow to react on first cached object added (e.g. start timer)
            virtual void onFilled();

            // ask if objects are cached
            bool empty() const { return 0 == mpNodes; }

            // flush all cached objects (delete them)
            void flush();

            // try to access entry
            const node* getEntry(const cacheable& rCacheable);

            // decrease lifetime, delete cached objects which
            // reached end of life
            void trigger();

            // return default lifetime
            sal_uInt32 getLifetime() const { return mnLifetime; }
        };
    } // end of namespace cache
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////

namespace basegfx
{
    namespace cache
    {
        // The node part of the mechanism; it is the data needed to manage
        // the cached data. It builds a double linked list of cached entries 
        // using mpNext/mpPrev to allow linear insertion/removal. It holds
        // contact to the manager (cannot exxist without one, lifetime is bound
        // to manager lifetime). It holds data to te cached data, it only
        // exists if cached data exists for it.
        class node
        {
        private:
            friend class manager;
            friend class cacheable;

            // entries for the double linked list of cached nodes
            node*               mpNext;
            node*               mpPrev;

            // the manager who created this node and to whom it belongs
            manager&            mrManager;

            // the data cached by this node
            const cacheable&    mrCacheable;

            // the remaining lifetime of this node. Zero means still alife,
            // but will be deleted/removed in the next call to trigger() at
            // the manager
            sal_uInt32  mnLifetime;

        protected:
        public:
            node(manager& rManager, const cacheable& rCacheable);
            virtual ~node();

            void touch();
        };
    } // end of namespace cache
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////

#endif // _BGFX_CACHE_MANAGER_HXX

//////////////////////////////////////////////////////////////////////////////
// eof
