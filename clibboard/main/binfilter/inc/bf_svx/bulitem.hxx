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


#ifndef _SVX_BULITEM_HXX
#define _SVX_BULITEM_HXX

#include <bf_svtools/bf_solar.h>

namespace binfilter {

// include ---------------------------------------------------------------

// define ----------------------------------------------------------------

// Styles
#define BS_ABC_BIG			0
#define BS_ABC_SMALL		1
#define BS_ROMAN_BIG		2
#define BS_ROMAN_SMALL		3
#define BS_123				4
#define BS_NONE				5
#define BS_BULLET			6
#define BS_BMP				128

// Justification
#define BJ_HLEFT			0x01
#define BJ_HRIGHT			0x02
#define BJ_HCENTER			0x04
#define BJ_VTOP				0x08
#define BJ_VBOTTOM  		0x10
#define BJ_VCENTER			0x20

// Valid-Bits
// Erstmal nur die Werte, die vom Dialog geaendert werden...
#define VALID_FONTCOLOR		0x0001
#define VALID_FONTNAME		0x0002
#define VALID_SYMBOL		0x0004
#define VALID_BITMAP		0x0008
#define VALID_SCALE			0x0010
#define VALID_START			0x0020
#define VALID_STYLE			0x0040
#define VALID_PREVTEXT		0x0080
#define VALID_FOLLOWTEXT	0x0100
}//end of namespace binfilter
#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _VCL_FONT_HXX //autogen
#include <vcl/font.hxx>
#endif
#ifndef _BF_GOODIES_GRAPHICOBJECT_HXX //autogen
#include <bf_goodies/graphicobject.hxx>
#endif

// class SvxBulletItem ---------------------------------------------------
namespace binfilter {
class SvxBulletItem : public SfxPoolItem
{
	Font	        aFont;
	BfGraphicObject*  pGraphicObject;
	String	        aPrevText;
	String	        aFollowText;
	USHORT      	nStart;
	USHORT	        nStyle;
	long  	        nWidth;
	USHORT	        nScale;
	sal_Unicode	    cSymbol;
	BYTE	        nJustify;
	USHORT	        nValidMask;	// Nur temporaer fuer GetAttribs/SetAttribs, wegen des grossen Bullets

#ifdef _SVX_BULITEM_CXX
	void	SetDefaultFont_Impl();
	void	SetDefaults_Impl();
#endif

public:
	TYPEINFO();

	SvxBulletItem( USHORT nWhich = 0 );
	SvxBulletItem( SvStream& rStrm, USHORT nWhich = 0 );
	SvxBulletItem( const SvxBulletItem& );
	~SvxBulletItem();

	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*    Create( SvStream&, USHORT nVersion ) const;
	virtual SvStream&       Store( SvStream & , USHORT nItemVersion ) const;

	sal_Unicode	        GetSymbol() const { return cSymbol; }
	String	            GetPrevText() const { return aPrevText; }
	String	            GetFollowText() const { return aFollowText; }

	USHORT				GetStart() const { return nStart; }
	long  				GetWidth() const { return nWidth; }
	USHORT  			GetStyle() const { return nStyle; }
	BYTE				GetJustification() const { return nJustify; }
	Font				GetFont() const { return aFont; }
	USHORT				GetScale() const { return nScale; }



	void	            SetSymbol( sal_Unicode c) { cSymbol = c; }
	void	            SetPrevText( const String& rStr) { aPrevText = rStr;}
	void	            SetFollowText(const String& rStr) { aFollowText=rStr;}

	void				SetStart( USHORT nNew ) { nStart = nNew; }
	void				SetWidth( long nNew ) { nWidth = nNew; }
	void				SetStyle( USHORT nNew ) { nStyle = nNew; }
	void				SetJustification( BYTE nNew ) { nJustify = nNew; }
	void				SetFont( const Font& rNew) { aFont = rNew; }
	void				SetScale( USHORT nNew ) { nScale = nNew; }

	virtual USHORT		GetVersion(USHORT nFileVersion) const;
	virtual int			operator==( const SfxPoolItem& ) const;

	static void			StoreFont( SvStream&, const Font& );
	static Font			CreateFont( SvStream&, USHORT nVer );

	USHORT&				GetValidMask() 					{ return nValidMask;	}
	USHORT				GetValidMask() const 			{ return nValidMask;	}
	USHORT				IsValid( USHORT nFlag ) const	{ return nValidMask & nFlag; }
	void				SetValid( USHORT nFlag, BOOL bValid )
						{
							if ( bValid )
								nValidMask |= nFlag;
							else
								nValidMask &= ~nFlag;
						}
};

}//end of namespace binfilter
#endif

