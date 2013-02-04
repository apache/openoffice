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


#ifndef _SVX_FONTITEM_HXX
#define _SVX_FONTITEM_HXX

#include <bf_svtools/bf_solar.h>

// include ---------------------------------------------------------------

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _FONT_HXX //autogen
#include <vcl/font.hxx>
#endif

namespace rtl
{
	class OUString;
}
namespace binfilter {
class SvXMLUnitConverter;
// class SvxFontItem -----------------------------------------------------

/*
	[Beschreibung]
	Dieses Item beschreibt einen Font.
*/

class SvxFontItem : public SfxPoolItem
{
	String  aFamilyName;
	String  aStyleName;
	FontFamily  eFamily;
	FontPitch ePitch;
	rtl_TextEncoding eTextEncoding;

    static BOOL bEnableStoreUnicodeNames;

public:
	TYPEINFO();

	SvxFontItem( const USHORT nId = ITEMID_FONT );
	SvxFontItem( const FontFamily eFam, const String& rFamilyName,
		const String& rStyleName,
		const FontPitch eFontPitch = PITCH_DONTKNOW,
		const rtl_TextEncoding eFontTextEncoding = RTL_TEXTENCODING_DONTKNOW,
		const USHORT nId = ITEMID_FONT );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int 			 operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*     Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	 Create(SvStream &, USHORT) const;
	virtual SvStream&		 Store(SvStream &, USHORT nItemVersion) const;
	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );


	// ZugriffsMethoden:
	inline String &GetFamilyName() { return aFamilyName; }
	inline const String &GetFamilyName() const { return aFamilyName; }

	inline String &GetStyleName() { return aStyleName; }
	inline const String &GetStyleName() const { return aStyleName; }

	inline FontFamily &GetFamily() { return eFamily; }
	inline FontFamily GetFamily() const { return eFamily; }

	inline FontPitch &GetPitch() { return ePitch; }
	inline FontPitch GetPitch() const { return ePitch; }

	inline rtl_TextEncoding &GetCharSet() { return eTextEncoding; }
	inline rtl_TextEncoding GetCharSet() const { return eTextEncoding; }

	inline SvxFontItem& operator=(const SvxFontItem& rFont)
	{
		aFamilyName =  rFont.GetFamilyName();
		aStyleName =   rFont.GetStyleName();
		eFamily =      rFont.GetFamily();
		ePitch =   rFont.GetPitch();
		eTextEncoding = rFont.GetCharSet();
		return *this;
	}

    static void EnableStoreUnicodeNames( BOOL bEnable ) { bEnableStoreUnicodeNames = bEnable; }

};

void GetDefaultFonts( SvxFontItem& rLatin, SvxFontItem& rAsian,
						SvxFontItem& rComplex );

}//end of namespace binfilter
#endif // #ifndef _SVX_FONTITEM_HXX

