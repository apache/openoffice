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


#ifndef _SFXSZITEM_HXX
#define _SFXSZITEM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SV_GEN_HXX //autogen
#include <tools/gen.hxx>
#endif

#include <bf_svtools/poolitem.hxx>

class SvStream;

namespace binfilter
{

class SfxArguments;

DBG_NAMEEX(SfxSizeItem)

// -----------------------------------------------------------------------

class SfxSizeItem : public SfxPoolItem
{
private:
	Size					 aVal;

public:
							 TYPEINFO();
							 SfxSizeItem();
							 SfxSizeItem( USHORT nWhich, const Size& rVal );
							 SfxSizeItem( const SfxSizeItem& );
							 ~SfxSizeItem() { DBG_DTOR(SfxSizeItem, 0); }

	virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
									SfxMapUnit eCoreMetric,
									SfxMapUnit ePresMetric,
									XubString &rText,
                                    const ::IntlWrapper * = 0 ) const;

	virtual int 			 operator==( const SfxPoolItem& ) const;
	virtual	BOOL 			 QueryValue( com::sun::star::uno::Any& rVal,
							 			 BYTE nMemberId = 0 ) const;
	virtual	BOOL 			 PutValue( const com::sun::star::uno::Any& rVal,
						   			   BYTE nMemberId = 0 );

	virtual SfxPoolItem*     Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	 Create(SvStream &, USHORT nItemVersion) const;
	virtual SvStream&		 Store(SvStream &, USHORT nItemVersion) const;

	const Size&				GetValue() const { return aVal; }
	void					SetValue( const Size& rNewVal ) {
								 DBG_ASSERT( GetRefCount() == 0, "SetValue() with pooled item" );
								 aVal = rNewVal; }
};

}

#endif

