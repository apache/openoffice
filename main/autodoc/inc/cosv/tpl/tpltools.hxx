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



#ifndef CSV_TPLTOOLS_HXX
#define CSV_TPLTOOLS_HXX

#include <vector>
#include <map>




namespace csv
{


template <class COLLECTION>
inline void		    erase_container(
						COLLECTION &		o_rCollection );

/// Version for std::map
template <class COLLECTION>
void		  		erase_map_of_heap_ptrs(
						COLLECTION &		o_rCollection );

/// Version for other containers than std::map, with non-pair value_type.
template <class COLLECTION>
void		  		erase_container_of_heap_ptrs(
						COLLECTION &		o_rCollection );

template <class VECTOR_ELEM>
void                adjust_vector_size(
                        std::vector<VECTOR_ELEM> &
                                            io_rVector,
                        uintt               i_nSize,
                        const VECTOR_ELEM & i_nFill );


template <class KEY, class VAL>
const VAL *	        find_in_map(            /// Usable for all kinds of values
						const std::map< KEY, VAL > &
											i_rMap,
						const KEY &			i_rKey );


/** @return the value in the map, if it is in there, else 0.
    @precond VAL has to be convertable to "0".
*/
template <class KEY, class VAL>
VAL					value_from_map(
						const std::map< KEY, VAL > &
											i_rMap,
						const KEY &			i_rKey );

/** @return the value in the map, if it is in there, else i_notFound.
*/
template <class KEY, class VAL>
VAL					value_from_map(
						const std::map< KEY, VAL > &
											i_rMap,
						const KEY &			i_rKey,
						VAL                 i_notFound );

template <class COLLECTION, class VALUE>
bool		  		contains(
						const COLLECTION &	i_collection,
						const VALUE &       i_value );

// Object oriented for_each:
template <class COLLECTION, class CLASS, class MEMFUNC>
void                call_for_each(
                        const COLLECTION &  i_rList,
                        CLASS *             io_pThis,
                        MEMFUNC             i_fMethod );




// IMPLEMENTATION
template <class COLLECTION>
inline void
erase_container( COLLECTION & o_rCollection )
{
	o_rCollection.erase( o_rCollection.begin(),
						 o_rCollection.end() );
}

template <class COLLECTION>
void
erase_map_of_heap_ptrs( COLLECTION & o_rCollection )
{
    typename COLLECTION::iterator itEnd = o_rCollection.end();
    for ( typename COLLECTION::iterator it = o_rCollection.begin();
          it != itEnd;
          ++it )
    {
        delete (*it).second;
    }

	o_rCollection.erase( o_rCollection.begin(),
						 o_rCollection.end() );
}

template <class COLLECTION>
void
erase_container_of_heap_ptrs( COLLECTION & o_rCollection )
{
	typename COLLECTION::iterator itEnd = o_rCollection.end();
	for ( typename COLLECTION::iterator it = o_rCollection.begin();
		  it != itEnd;
		  ++it )
	{
		delete *it;
	}

	o_rCollection.erase( o_rCollection.begin(),
						 o_rCollection.end() );
}

template <class VECTOR_ELEM>
void
adjust_vector_size( std::vector<VECTOR_ELEM> & io_rVector,
                    uintt                      i_nSize,
                    const VECTOR_ELEM &        i_nFill )
{
    if ( io_rVector.size() > i_nSize )
    {
        io_rVector.erase( io_rVector.begin() + i_nSize, io_rVector.end() );
    }
    else
    {
        io_rVector.reserve(i_nSize);
        while ( io_rVector.size() <  i_nSize )
            io_rVector.push_back(i_nFill);
    }
}


template <class KEY, class VAL>
const VAL *
find_in_map( const std::map< KEY, VAL > & i_rMap,
			 const KEY &			      i_rKey )
{
	typename std::map< KEY, VAL >::const_iterator
			ret =  i_rMap.find(i_rKey);
	return ret != i_rMap.end()
				?	& (*ret).second
				:	(const VAL*)0;
}

template <class KEY, class VAL>
VAL
value_from_map( const std::map< KEY, VAL > & i_rMap,
			    const KEY &			         i_rKey )
{
	typename std::map< KEY, VAL >::const_iterator
			ret =  i_rMap.find(i_rKey);
	return ret != i_rMap.end()
				?	(*ret).second
				:	VAL(0);
}

template <class KEY, class VAL>
VAL
value_from_map( const std::map< KEY, VAL > & i_rMap,
			    const KEY &			         i_rKey,
			    VAL                          i_notFound )
{
	typename std::map< KEY, VAL >::const_iterator
			ret =  i_rMap.find(i_rKey);
	return ret != i_rMap.end()
				?	(*ret).second
				:	i_notFound;
}

template <class COLLECTION, class VALUE>
bool
contains( const COLLECTION &  i_collection,
		  const VALUE &       i_value )
{
    return std::find(i_collection.begin(), i_collection.end(), i_value)
           !=
           i_collection.end();
}

template <class COLLECTION, class CLASS, class MEMFUNC>
void
call_for_each( const COLLECTION &  i_rList,
               CLASS *             io_pThis,
               MEMFUNC             i_fMethod )
{
    typename COLLECTION::const_iterator it = i_rList.begin();
    typename COLLECTION::const_iterator itEnd = i_rList.end();
    for ( ; it != itEnd; ++it )
    {
        (io_pThis->*i_fMethod)(*it);
    }
}




}   // namespace csv
#endif
