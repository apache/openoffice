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
        // helper class to allow caching of a c++ object with tooling. To
        // be able to use this, the object you want to cache needs to be derived 
        // from this tooling class. It just adds a pointer for managing purposes,
        // but needs no virtuality and only this include file. For more info on how 
        // to cache these objects, see the coc::cmanager and node
        // clases in the same namespace (normally in the file coc_manager.hxx in 
        // the same dir as this file)
        class cacheable
        {
        private:
            friend class cmanager;
            friend class node;

            // the managing data node
            node*       mpNode;

            // access for manager and node
            void setNode(node* pNew) { mpNode = pNew; }
            const node* getNode() const { return mpNode; }

        protected:
            // call this always when the original data changes, will remove
            // all evtl. cached instances
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
