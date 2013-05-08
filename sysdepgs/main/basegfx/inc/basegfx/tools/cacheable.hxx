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

#ifndef _BGFX_CACHE_CACHEABLE_HXX
#define _BGFX_CACHE_CACHEABLE_HXX

#include <sal/types.h>
#include <basegfx/tools/cmanager.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace basegfx
{
    namespace cache
    {
        // Helper class to allow caching of a c++ object with tooling. To
        // be able to use this, the object you want to cache needs to be derived 
        // from this tooling class. It just adds a pointer for managing purposes,
        // but needs no virtuality and only this include file. For more info on how 
        // to cache these objects, see the coc::cmanager and node clases in the same 
        // namespace.
        // To use cached instances of this class, ask your manager class derived from
        // the 'manager' class of this namespace for an entry, it will then either have
        // an instance or construct one and add it to the manager.
        // You may still incarnate instances yourself without a manager, these will just
        // not get cached (and there is currently no way to add them to caching after
        // incarnation).
        // The default constructor/destructor will do the needed handling, so an instance
        // can just be deleted, independent of being cached or not.
        class cacheable
        {
        private:
            friend class manager;
            friend class node;

            // the managing data node
            node*       mpNode;

            // access for manager and node
            void setNode(node* pNew) { mpNode = pNew; }
            const node* getNode() const { return mpNode; }

        protected:
            // this needs always to be called when the original data changes, it will remove
            // all evtl. cached instances from the manager
            void onChange() { if(mpNode) delete mpNode; mpNode = 0; }

        public:
            // constructor/destructor/copy constructor; do *not* copy pointer to
            // managing data node
            cacheable() : mpNode(0) {}
            ~cacheable() { if(mpNode) delete mpNode; }
            cacheable(const cacheable& /*rCacheable*/) : mpNode(0) {}

            // assigment operator; do *not* copy managing data node
            cacheable& operator=(const cacheable& /*rCacheable*/) { mpNode = 0; return *this; }
        };
    } // end of namespace cache
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////

#endif // _BGFX_CACHE_CACHEABLE_HXX

//////////////////////////////////////////////////////////////////////////////
// eof
