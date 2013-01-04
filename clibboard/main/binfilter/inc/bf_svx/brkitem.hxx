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


#ifndef _SVX_BRKITEM_HXX
#define _SVX_BRKITEM_HXX

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

// class SvxFmtBreakItem -------------------------------------------------

/*
	[Beschreibung]
	Dieses Item beschreibt ein Umbruch-Attribut
	Automatisch?, Seiten- oder Spaltenumbruch, davor oder danach?
*/
#define	FMTBREAK_NOAUTO	((USHORT)0x0001)

class SvxFmtBreakItem : public SfxEnumItem
{
public:
	TYPEINFO();

	inline SvxFmtBreakItem( const SvxBreak eBrk = SVX_BREAK_NONE,
							const USHORT nWhich = ITEMID_FMTBREAK );
	inline SvxFmtBreakItem( const SvxFmtBreakItem& rBreak );
	inline SvxFmtBreakItem& operator=( const SvxFmtBreakItem& rCpy );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int 			 operator==( const SfxPoolItem& ) const;
	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );


	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;
	virtual SvStream&		 Store( SvStream& , USHORT nItemVersion ) const;
	virtual USHORT			 GetVersion( USHORT nFileVersion ) const;
	virtual SfxPoolItem*	 Create( SvStream&, USHORT ) const;
	virtual USHORT			 GetValueCount() const;

	// MS VC4.0 kommt durcheinander
	void			 SetValue( USHORT nNewVal )
								{SfxEnumItem::SetValue(nNewVal); }
	SvxBreak			 GetBreak() const { return SvxBreak( GetValue() ); }
	void					 SetBreak( const SvxBreak eNew )
								{ SetValue( (USHORT)eNew ); }
};

inline SvxFmtBreakItem::SvxFmtBreakItem( const SvxBreak eBreak,
										 const USHORT nWhich ) :
	SfxEnumItem( nWhich, (USHORT)eBreak )
{}

inline SvxFmtBreakItem::SvxFmtBreakItem( const SvxFmtBreakItem& rBreak ) :
	SfxEnumItem( rBreak )
{}

inline SvxFmtBreakItem& SvxFmtBreakItem::operator=(
	const SvxFmtBreakItem& rBreak )
{
	SetValue( rBreak.GetValue() );
	return *this;
}


}//end of namespace binfilter
#endif

