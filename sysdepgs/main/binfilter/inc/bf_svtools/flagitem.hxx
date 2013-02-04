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


#ifndef _SFXFLAGITEM_HXX
#define _SFXFLAGITEM_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#include <bf_svtools/bf_solar.h>
#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif
#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif

class SvStream;

namespace binfilter
{

extern USHORT nSfxFlagVal[16];

// -----------------------------------------------------------------------

DBG_NAMEEX_VISIBILITY(SfxFlagItem, )

class  SfxFlagItem: public SfxPoolItem
{
	USHORT					 nVal;

public:
							 TYPEINFO();

							 SfxFlagItem( USHORT nWhich = 0, USHORT nValue = 0 );
							 SfxFlagItem( const SfxFlagItem& );

							 ~SfxFlagItem() {
								DBG_DTOR(SfxFlagItem, 0); }

	virtual BYTE			 GetFlagCount() const;
	virtual XubString		 GetFlagText( BYTE nFlag ) const;

	virtual int 			 operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*     Create(SvStream &, USHORT nVersion) const;
	virtual SvStream&		 Store(SvStream &, USHORT nItemVersion) const;

	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
									SfxMapUnit eCoreMetric,
									SfxMapUnit ePresMetric,
									XubString &rText,
                                    const ::IntlWrapper * = 0 ) const;
			USHORT           GetValue() const { return nVal; }
			void			 SetValue( USHORT nNewVal ) {
								 DBG_ASSERT( GetRefCount() == 0, "SetValue() with pooled item" );
								 nVal = nNewVal;
							 }
			int 			 GetFlag( BYTE nFlag ) const {
								 return ( (nVal & nSfxFlagVal[nFlag]) != 0 ); }
};

}

#endif
