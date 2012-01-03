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


#ifndef _SXCIKITM_HXX
#define _SXCIKITM_HXX

#ifndef _SVDDEF_HXX //autogen
#include <bf_svx/svddef.hxx>
#endif

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
namespace binfilter {

enum SdrCircKind {SDRCIRC_FULL,
				  SDRCIRC_SECT,
				  SDRCIRC_CUT,
				  SDRCIRC_ARC};

class SdrCircKindItem: public SfxEnumItem {
public:
	TYPEINFO();
	SdrCircKindItem(SdrCircKind eKind=SDRCIRC_FULL): SfxEnumItem(SDRATTR_CIRCKIND,eKind) {}
	SdrCircKindItem(SvStream& rIn)                 : SfxEnumItem(SDRATTR_CIRCKIND,rIn)   {}
	virtual SfxPoolItem* Clone(SfxItemPool* pPool=NULL) const;
	virtual SfxPoolItem* Create(SvStream& rIn, USHORT nVer) const;
	virtual USHORT       GetValueCount() const; // { return 4; }
			SdrCircKind  GetValue() const      { return (SdrCircKind)SfxEnumItem::GetValue(); }

	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

};

}//end of namespace binfilter
#endif
