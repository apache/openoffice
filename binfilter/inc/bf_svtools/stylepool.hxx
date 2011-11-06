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


#ifndef INCLUDED_SVTOOLS_STYLEPOOL_HXX
#define INCLUDED_SVTOOLS_STYLEPOOL_HXX

#include <boost/shared_ptr.hpp>
#include <rtl/ustring.hxx>

#ifndef _SFXITEMSET_HXX
#include <bf_svtools/itemset.hxx>
#endif

namespace binfilter
{

class StylePoolImpl;
class StylePoolIterImpl;
class IStylePoolIteratorAccess;

class  StylePool
{
private:
    StylePoolImpl *pImpl;
public:
    typedef boost::shared_ptr<SfxItemSet> SfxItemSet_Pointer_t;

    /** Insert a SfxItemSet into the style pool. 

        The pool makes a copy of the provided SfxItemSet. 

        @param SfxItemSet
        the SfxItemSet to insert

        @return a shared pointer to the SfxItemSet
    */
    virtual SfxItemSet_Pointer_t insertItemSet( const SfxItemSet& rSet );

    /** Create an iterator

        The iterator walks through the StylePool

        @attention every change, e.g. destruction, of the StylePool could cause undefined effects.

        @postcond the iterator "points before the first" SfxItemSet of the pool.
        The first StylePoolIterator::getNext() call will deliver the first SfxItemSet.
    */    
    virtual IStylePoolIteratorAccess* createIterator();

    /** Returns the number of styles
    */
    virtual sal_Int32 getCount() const;

    virtual ~StylePool();

    static ::rtl::OUString nameOf( SfxItemSet_Pointer_t pSet );
};

class  IStylePoolIteratorAccess
{
public:
    /** Delivers a shared pointer to the next SfxItemSet of the pool
        If there is no more SfxItemSet, the delivered share_pointer is empty.
    */
    virtual StylePool::SfxItemSet_Pointer_t getNext() = 0;
    virtual ::rtl::OUString getName() = 0;
    virtual ~IStylePoolIteratorAccess() {};
};

}

#endif
