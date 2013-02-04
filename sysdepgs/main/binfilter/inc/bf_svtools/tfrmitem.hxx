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


#ifndef _TFRMITEM_HXX
#define _TFRMITEM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif

#include <bf_svtools/poolitem.hxx>

class SvStream;

namespace binfilter
{

enum SfxOpenMode
{
	SfxOpenSelect		= 0,     // selected in view
	SfxOpenOpen			= 1,     // doubleclicked or <enter>
	SfxOpenAddTask		= 2,     // doubleclicked or <enter> with Ctrl-Modifier
	SfxOpenDontKnow		= 3,
	SfxOpenReserved1	= 4,
	SfxOpenReserved2	= 5,
	SfxOpenModeLast		= 5
};

DBG_NAMEEX(SfxTargetFrameItem)

// class SfxTargetFrameItem -------------------------------------------------

class SfxTargetFrameItem : public SfxPoolItem
{
private:
			String			_aFrames[ (USHORT)SfxOpenModeLast+1 ];
public:
			TYPEINFO();

			SfxTargetFrameItem( const SfxTargetFrameItem& rCpy );
			SfxTargetFrameItem( USHORT nWhich );
			~SfxTargetFrameItem();

	virtual	int				operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Create( SvStream&, USHORT nItemVersion ) const;
	virtual SvStream&		Store( SvStream&, USHORT nItemVersion ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;

	virtual	BOOL 			QueryValue( com::sun::star::uno::Any& rVal,
							 			BYTE nMemberId = 0 ) const;
	virtual	BOOL 			PutValue  ( const com::sun::star::uno::Any& rVal,
						     			BYTE nMemberId = 0 );
};

}
#endif

