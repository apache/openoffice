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



#ifndef _SFXRNGITEM_HXX

#ifndef NUMTYPE

#define NUMTYPE	USHORT
#define SfxXRangeItem SfxRangeItem
#define SfxXRangesItem SfxUShortRangesItem
#include <bf_svtools/rngitem.hxx>
#undef NUMTYPE
#undef SfxXRangeItem
#undef SfxXRangesItem

#ifndef _SFXITEMS_HXX
#define NUMTYPE	ULONG
#define SfxXRangeItem SfxULongRangeItem
#define SfxXRangesItem SfxULongRangesItem
#include <bf_svtools/rngitem.hxx>
#undef NUMTYPE
#undef SfxXRangeItem
#undef SfxXRangesItem
#endif

#define _SFXRNGITEM_HXX

#else

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif

class SvStream;

namespace binfilter
{

// -----------------------------------------------------------------------

class  SfxXRangeItem : public SfxPoolItem
{
private:
	NUMTYPE 					nFrom;
	NUMTYPE 					nTo;
public:
								TYPEINFO();
								SfxXRangeItem();
								SfxXRangeItem( USHORT nWID, NUMTYPE nFrom, NUMTYPE nTo );
								SfxXRangeItem( const SfxXRangeItem& rItem );
	virtual int 				operator==( const SfxPoolItem& ) const;
	virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
									SfxMapUnit eCoreMetric,
									SfxMapUnit ePresMetric,
									XubString &rText,
                                    const ::IntlWrapper * = 0 ) const;
	virtual SfxPoolItem*    	Clone( SfxItemPool *pPool = 0 ) const;
	inline NUMTYPE&				From() { return nFrom; }
	inline NUMTYPE				From() const { return nFrom; }
	inline NUMTYPE&				To() { return nTo; }
	inline NUMTYPE				To() const { return nTo; }
	inline BOOL					HasRange() const { return nTo>nFrom; }
	virtual SfxPoolItem*		Create( SvStream &, USHORT nVersion ) const;
	virtual SvStream&			Store( SvStream &, USHORT nItemVersion ) const;
};

// -----------------------------------------------------------------------

class  SfxXRangesItem : public SfxPoolItem
{
private:
	NUMTYPE*					_pRanges;

public:
								TYPEINFO();
								SfxXRangesItem();
								SfxXRangesItem( USHORT nWID, SvStream &rStream );
								SfxXRangesItem( const SfxXRangesItem& rItem );
	virtual 					~SfxXRangesItem();
	virtual int 				operator==( const SfxPoolItem& ) const;
	virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
									SfxMapUnit eCoreMetric,
									SfxMapUnit ePresMetric,
									XubString &rText,
                                    const ::IntlWrapper * = 0 ) const;
	virtual SfxPoolItem*    	Clone( SfxItemPool *pPool = 0 ) const;
	inline const NUMTYPE*		GetRanges() const { return _pRanges; }
	virtual SfxPoolItem*		Create( SvStream &, USHORT nVersion ) const;
	virtual SvStream&			Store( SvStream &, USHORT nItemVersion ) const;
};

}

#endif
#endif
