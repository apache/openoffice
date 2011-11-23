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


#ifndef _CNTRSRT_HXX
#define _CNTRSRT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _CONTNR_HXX //autogen
#include <tools/contnr.hxx>
#endif

namespace binfilter {

#define DECLARE_CONTAINER_SORT_COMMON( ClassName, Type )						\
	ClassName( const ClassName& );											\
    ClassName& operator =( const ClassName& );								\
public:                                       								\
	using Container::Count;                    								\
																			\
	ClassName( USHORT  InitSize, USHORT  ReSize ) :							\
		Container( CONTAINER_MAXBLOCKSIZE, InitSize, ReSize )	{}			\
																			\
    BOOL Insert( Type* pObj );												\
							   												\
	Type *Remove( ULONG nPos ) 												\
		{ return (Type *)Container::Remove( nPos ); }						\
																			\
	Type *Remove( Type* pObj );												\
							   												\
	void DeleteAndDestroy( ULONG nPos )										\
	{                                  										\
		Type *pObj = Remove( nPos );   										\
		if( pObj )                     										\
			delete pObj;               										\
	}                                  										\
							   												\
	void DeleteAndDestroy()													\
		{ while( Count() ) DeleteAndDestroy( 0 ); }							\
																			\
    Type* GetObject( ULONG nPos ) const										\
		{ return (Type *)Container::GetObject( nPos ); }					\
																			\
	Type* operator[]( ULONG nPos ) const 									\
		{ return GetObject(nPos); }											\
																			\
	BOOL Seek_Entry( const Type *pObj, ULONG* pPos ) const;					\
																			\
	ULONG GetPos( const Type* pObj ) const;									\


#define DECLARE_CONTAINER_SORT( ClassName, Type )							\
class ClassName : private Container											\
{																			\
	DECLARE_CONTAINER_SORT_COMMON( ClassName, Type )						\
	~ClassName() {} 														\
};																			\


#define DECLARE_CONTAINER_SORT_DEL( ClassName, Type )							\
class ClassName : private Container											\
{																			\
	DECLARE_CONTAINER_SORT_COMMON( ClassName, Type )							\
	~ClassName() { DeleteAndDestroy(); }									\
};																			\


#define IMPL_CONTAINER_SORT( ClassName, Type, SortFunc )					\
BOOL ClassName::Insert( Type *pObj )                                        \
{                                                                           \
	ULONG nPos;                                                             \
	BOOL bExist = Seek_Entry( pObj, &nPos );                                \
	if( !bExist )                                                           \
		Container::Insert( pObj, nPos );                                    \
	return !bExist;                                                         \
}                                                                           \
																			\
Type *ClassName::Remove( Type* pObj )                                       \
{                                                                           \
	ULONG nPos;                                                             \
	if( Seek_Entry( pObj, &nPos ) )                                         \
		return Remove( nPos );                                              \
	else                                                                    \
		return 0;                                                           \
}                                                                           \
																			\
ULONG ClassName::GetPos( const Type* pObj ) const                           \
{                                                                           \
	ULONG nPos;                                                             \
	if( Seek_Entry( pObj, &nPos ) )                                         \
		return nPos;                                                        \
	else                                                                    \
		return CONTAINER_ENTRY_NOTFOUND;                                    \
}                                                                           \
																			\
BOOL ClassName::Seek_Entry( const Type* pObj, ULONG* pPos ) const           \
{                                                                           \
	register ULONG nO  = Count(),                                           \
			nM,                                                             \
			nU = 0;                                                         \
	if( nO > 0 )                                                            \
	{                                                                       \
		nO--;                                                               \
		while( nU <= nO )                                                   \
		{                                                                   \
			nM = nU + ( nO - nU ) / 2;                                      \
			int nCmp = SortFunc( *GetObject(nM), *pObj );				    \
																			\
			if( 0 == nCmp )                              					\
			{                                                               \
				if( pPos ) *pPos = nM;                                      \
				return TRUE;                                                \
			}                                                               \
			else if( nCmp < 0 )                       						\
				nU = nM + 1;                                                \
			else if( nM == 0 )                                              \
			{                                                               \
				if( pPos ) *pPos = nU;                                      \
				return FALSE;                                               \
			}                                                               \
			else                                                            \
				nO = nM - 1;                                                \
		}                                                                   \
	}                                                                       \
	if( pPos ) *pPos = nU;                                                  \
	return FALSE;                                                           \
}                                                                           \

}

#endif
