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


#ifndef _LCKBITEM_HXX
#define _LCKBITEM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif

namespace binfilter
{

// -----------------------------------------------------------------------

class  SfxLockBytesItem : public SfxPoolItem
{
	SvLockBytesRef			_xVal;

public:
							TYPEINFO();
							SfxLockBytesItem();
							SfxLockBytesItem( USHORT nWhich, SvStream & );
							SfxLockBytesItem( const SfxLockBytesItem& );
							~SfxLockBytesItem();

	virtual int 			operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nItemVersion) const;
	virtual SvStream&		Store(SvStream &, USHORT nItemVersion ) const;

	SvLockBytes*			GetValue() const { return _xVal; }

	virtual	BOOL 			PutValue  ( const com::sun::star::uno::Any& rVal,
						    			BYTE nMemberId = 0 );
	virtual	BOOL 			QueryValue( com::sun::star::uno::Any& rVal,
										BYTE nMemberId = 0 ) const;
};

}

#endif

