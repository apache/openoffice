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


#ifndef _SVX_CMAPITEM_HXX
#define _SVX_CMAPITEM_HXX

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

// class SvxCaseMapItem --------------------------------------------------

/*	[Beschreibung]

	Dieses Item beschreibt die Schrift-Ausrichtung (Versalien, Kapitaelchen,...).
*/

class SvxCaseMapItem : public SfxEnumItem
{
public:
	TYPEINFO();

	SvxCaseMapItem( const SvxCaseMap eMap = SVX_CASEMAP_NOT_MAPPED,
					const USHORT nId = ITEMID_CASEMAP );

	// "pure virtual Methoden" vom SfxPoolItem + SfxEnumItem

	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*    Create(SvStream &, USHORT) const;
	virtual SvStream&		Store(SvStream &, USHORT nItemVersion) const;
	virtual USHORT          GetValueCount() const;

	// MS VC4.0 kommt durcheinander
	void			 		SetValue( USHORT nNewVal )
								{SfxEnumItem::SetValue(nNewVal); }

	inline SvxCaseMapItem& operator=(const SvxCaseMapItem& rMap)
		{
			SetValue( rMap.GetValue() );
			return *this;
		}

	// enum cast
	SvxCaseMap 				GetCaseMap() const
								{ return (SvxCaseMap)GetValue(); }
	void 					SetCaseMap( SvxCaseMap eNew )
								{ SetValue( (USHORT)eNew ); }
	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );
};

}//end of namespace binfilter
#endif

