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

        // the manager part to allow caching of any c++ object derived from the helper class
        // cacheable, see this calss in the same namespace. normally in file 
        // cacheable.hxx in the same dir as this file.
        // An object derived from cacheable can be cached with this manager class. It
        // will allow you to access cached entries using getEntry() which also resets the
        // lifetime counter if found. If not found the cached object should be incarnated and
        // added to the cache.
        // This mechanism uses the helper class node where the management is done. It 
        // implements basic mechanisms for organizing this.
        // to use this, you need to derive own classes from cmanager and from node.
        //
        // On the cmanager derivation:
        // - react on the cache being empty by overloading (onEmpty())
        // - react on getting the first element by overloading (onFilled())
        // - test if there are cached instances using empty()
        // - try to get a cached entry using getEntry()
        // - trigger the lifetime counter using trigger() which will remove
        //   cached objects with zero count (probably timer based)
        // - implement a access method to your cached data type using getEntry().
        //   When you get one, it's the cached instance. If not, construct your
        //   instance of node (it will be added automatically) and return it
        //
        // On the node derivation:
        // - Add a data entry to hold the cached data
        // - Implement creating that data from the original cached object in the
        //   constructor
        // - destroy it in the destuctor
        //
        // To access your cached instance, you can use getEntry() at the manager instance
        // which may return the cached entry or a newly created one. On that instance,
        // access your cached data.
        // The insertion to manager, access to the instance and the removal at lifetime
        // end are all linear operations.
        // Usage examples would e.g. derive from cmanager and the Timer calss in sal,
        // call trigger in the overloaded timer callback and switch the timer on/off
        // in overlods of onEmpty()/onFilled(). I will use it to e.g. cache Widows-specific
        // data creatatble from system-independent bitmap data.
        class cmanager : protected comphelper::OBaseMutex
        {
        private:
            friend class node;

            sal_uInt32              mnLifetime;
            node*                   mpNodes;

            void removeNode(node& rNode);
            void insertNode(node& rNode);

        protected:
        public:
            // constructor allows to give lifetime count for newly
            // added cached objects
            cmanager(sal_uInt32 nLifetime = 60);
            virtual ~cmanager();

            // react on last cached object removed
            virtual void onEmpty();

            // react on first cached object added
            virtual void onFilled();

            // ask if objects are cached
            bool empty() const { return 0 == mpNodes; }

            // flush all cached objects (delete them)
            void flush();

            // try to access entry
            const node* getEntry(const cacheable& rCacheable);

            // decrease lifetime, delete cached objects which
            // rech zero
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
        class node
        {
        private:
            friend class cmanager;
            friend class cacheable;

            node*               mpNext;
            node*               mpPrev;

            cmanager&           mrManager;
            const cacheable&    mrCacheable;

            sal_uInt32  mnLifetime;

        protected:
        public:
            node(cmanager& rManager, const cacheable& rCacheable);
            virtual ~node();

            void touch();
        };
    } // end of namespace cache
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////

#endif // _BGFX_CACHE_MANAGER_HXX

//////////////////////////////////////////////////////////////////////////////
// eof
