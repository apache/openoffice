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



#ifndef _EDITENG_BASELIST_HXX
#define _EDITENG_BASELIST_HXX

template<class T>
class BaseList
{
    std::vector<T*> contents;
public:
    sal_uInt32 Count() const { return contents.size(); }
    T* GetObject( sal_uInt32 index ) const { return contents[index]; }
    T* operator[]( std::size_t index ) const { return contents[index]; }
    T* const* GetData() const { return &contents[0]; }
    void Insert( T *node, sal_uInt32 position ) { contents.insert( contents.begin() + position, node); }
    void Replace( T *node, sal_uInt32 position )
    {
        if ( position < contents.size() )
            contents[ position ] = node;
    }
    void Remove( sal_uInt32 first, sal_uInt32 count = 1 ) { contents.erase( contents.begin() + first, contents.begin() + first + count ); }
    void DeleteAndDestroy( sal_uInt32 first, sal_uInt32 count )
    {
        for ( sal_uInt32 n = first; n < first + count; n++ )
            delete GetObject( n );
        Remove( first, first + count );
    }
};

#endif
