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


#ifndef _CNTWALL_HXX
#define _CNTWALL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef SHL_HXX
#include <tools/shl.hxx>
#endif
#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif
#ifndef _TOOLS_COLOR_HXX
#include <tools/color.hxx>
#endif

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif

class SvStream;

namespace binfilter {

class  CntWallpaperItem : public SfxPoolItem
{
private:
	UniString				_aURL;
	Color					_nColor;
	USHORT					_nStyle;

public:
							TYPEINFO();

							CntWallpaperItem( USHORT nWhich, SvStream& rStream, USHORT nVersion );
							CntWallpaperItem( const CntWallpaperItem& rCpy );
							~CntWallpaperItem();

	virtual USHORT GetVersion(USHORT) const;

	virtual	int				operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Create( SvStream&, USHORT nItemVersion ) const;
	virtual SvStream&		Store( SvStream&, USHORT nItemVersion ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;

	virtual	BOOL 			QueryValue( ::com::sun::star::uno::Any& rVal,
							 			BYTE nMemberId = 0 ) const;
	virtual	BOOL 			PutValue  ( const ::com::sun::star::uno::Any& rVal,
						     			BYTE nMemberId = 0 );

	void					SetBitmapURL( const UniString& rURL ) { _aURL = rURL; }
	void					SetColor( Color nColor ) { _nColor = nColor; }
	void					SetStyle( USHORT nStyle ) { _nStyle = nStyle; }

	const UniString&		GetBitmapURL() const { return _aURL; }
	Color					GetColor() const { return _nColor; }
	USHORT					GetStyle() const { return _nStyle; }
};

}

////////////////////////////////////////////////////////////////////////////////

#endif // _CNTWALL_HXX

