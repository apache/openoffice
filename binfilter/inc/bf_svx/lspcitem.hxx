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


#ifndef _SVX_LSPCITEM_HXX
#define _SVX_LSPCITEM_HXX

// include ---------------------------------------------------------------

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
#ifndef _SVX_SVXENUM_HXX
#include <bf_svx/svxenum.hxx>
#endif

#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif

namespace rtl
{
	class OUString;
}
namespace binfilter {
class SvXMLUnitConverter;
// class SvxLineSpacingItem ----------------------------------------------

/*
[Beschreibung]
Dieses Item beschreibt den Abstand zwischen den Zeilen.
*/

class SvxLineSpacingItem : public SfxEnumItemInterface
{
	friend SvStream& operator<<( SvStream&, SvxLineSpacingItem& ); //$ ostream

	short nInterLineSpace;
	USHORT nLineHeight;
	BYTE nPropLineSpace;
	SvxLineSpace eLineSpace;
	SvxInterLineSpace eInterLineSpace;

public:
	TYPEINFO();

	// Der Writer verlaesst sich auf eine Default-Hoehe von 200!
	// Eigentlich wuerde ich alle Werte mit 0 initialisieren, aber wer kann
	// die Folgen beim Writer absehen ?
	// => lieber einen krummen Wert als Default, aber der Programmierer
	// sieht, dass dort etwas besonderes passiert.
	SvxLineSpacingItem( USHORT nHeight = 200, const USHORT nId = ITEMID_LINESPACING );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int 			 operator==( const SfxPoolItem& ) const;
	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );


	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	 Create(SvStream &, USHORT) const;
	virtual SvStream&		 Store(SvStream &, USHORT nItemVersion ) const;

	// Methoden zum Abfragen und Aendern
	// Interlinespace wird zur Hoehe addiert.
	inline short GetInterLineSpace() const { return nInterLineSpace; }
	inline void SetInterLineSpace( const short nSpace )
	{
		nInterLineSpace = nSpace;
		eInterLineSpace = SVX_INTER_LINE_SPACE_FIX;
	}

	// Bestimmt absolute oder minimale Zeilenhoehe.
	inline USHORT GetLineHeight() const { return nLineHeight; }
	inline void SetLineHeight( const USHORT nHeight )
	{
		nLineHeight = nHeight;
		eLineSpace = SVX_LINE_SPACE_MIN;
	}

	// Vergroessert oder verkleinert die Zeilenhoehe.
	BYTE GetPropLineSpace() const { return nPropLineSpace; }
	inline void SetPropLineSpace( const BYTE nProp )
	{
		nPropLineSpace = nProp;
		eInterLineSpace = SVX_INTER_LINE_SPACE_PROP;
	}

	inline SvxLineSpace &GetLineSpaceRule() { return eLineSpace; }
	inline SvxLineSpace GetLineSpaceRule() const { return eLineSpace; }

	inline SvxInterLineSpace &GetInterLineSpaceRule() { return eInterLineSpace; }
	inline SvxInterLineSpace GetInterLineSpaceRule() const { return eInterLineSpace; }

	virtual USHORT			GetValueCount() const;
	virtual String			GetValueTextByPos( USHORT nPos ) const;
	virtual USHORT			GetEnumValue() const;
	virtual void			SetEnumValue( USHORT nNewVal );
};

}//end of namespace binfilter
#endif

