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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_basegfx.hxx"

#include <osl/diagnose.h>
#include <basegfx/tools/cmanager.hxx>
#include <basegfx/tools/cacheable.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace basegfx
{
    namespace cache
    {
        void manager::removeNode(node& rNode)
        {
            ::osl::MutexGuard aGuard(m_aMutex);

            if(mpNodes == &rNode)
            {
                mpNodes = rNode.mpNext;
            }

            if(rNode.mpPrev)
            {
                rNode.mpPrev->mpNext = rNode.mpNext;
            }

            if(rNode.mpNext)
            {
                rNode.mpNext->mpPrev = rNode.mpPrev;
            }

            rNode.mpNext = rNode.mpPrev = 0;

            if(!mpNodes)
            {
                onEmpty();
            }
        }

        void manager::insertNode(node& rNode)
        {
            ::osl::MutexGuard aGuard(m_aMutex);
            rNode.mpNext = rNode.mpPrev = 0;

            if(mpNodes)
            {
                rNode.mpNext = mpNodes;
            
                if(mpNodes->mpPrev)
                {
                    mpNodes->mpPrev->mpNext = &rNode;
                }

                rNode.mpPrev = mpNodes->mpPrev;
                mpNodes->mpPrev = &rNode;
            }

            bool bWasEmpty(0 == mpNodes);

            mpNodes = &rNode;

            if(bWasEmpty)
            {
                onFilled();
            }
        }

        manager::manager(sal_uInt32 nLifetime)
        :   mnLifetime(nLifetime),
            mpNodes(0)
        {
        }

        manager::~manager()
        {
            ::osl::MutexGuard aGuard(m_aMutex);

            while(mpNodes)
            {
                delete mpNodes;
            }
        }

        void manager::onEmpty()
        {
        }

        void manager::onFilled()
        {
        }

        const node* manager::getEntry(const cacheable& rCacheable)
        {
            return rCacheable.getNode();
        }

        void manager::trigger()
        {
            ::osl::MutexGuard aGuard(m_aMutex);
            node* pCurr = mpNodes;

            while(pCurr)
            {
                if(pCurr->mnLifetime)
                {
                    pCurr->mnLifetime--;
                    pCurr = pCurr->mpNext;
                }
                else
                {
                    node* pNext = pCurr->mpNext;
                    delete pCurr;
                    pCurr = pNext;
                }
            }
        }
        
        void manager::flush()
        {
            ::osl::MutexGuard aGuard(m_aMutex);

            while(mpNodes)
            {
                delete mpNodes;
            }
        }
    } // end of namespace cache
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////?

namespace basegfx
{
    namespace cache
    {
        node::node(manager& rManager, const cacheable& rCacheable) 
        :   mpNext(0),
            mpPrev(0),
            mrManager(rManager), 
            mrCacheable(rCacheable),
            mnLifetime(rManager.getLifetime())
        {
            if(mrCacheable.getNode())
            {
                OSL_ENSURE(false, "OOps, instance is already buffered (!)");
            }

            const_cast< cacheable& >(mrCacheable).setNode(this);
            mrManager.insertNode(*this);
        }

        node::~node()
        {
            const_cast< cacheable& >(mrCacheable).setNode(0);
            mrManager.removeNode(*this);
        }

        void node::touch()
        {
            mnLifetime = mrManager.getLifetime();
        }
    } // end of namespace cache
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////
// eof
