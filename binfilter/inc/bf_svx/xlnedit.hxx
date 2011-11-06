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



#ifndef _SVX_XLNEDIT_HXX
#define _SVX_XLNEDIT_HXX

#ifndef _XPOLY_HXX //autogen
#include <bf_svx/xpoly.hxx>
#endif
#ifndef _SVX_XIT_HXX //autogen
#include <bf_svx/xit.hxx>
#endif
namespace binfilter {

class SdrModel;

//----------------------
// class XLineEndItem
//----------------------
class XLineEndItem : public NameOrIndex
{
	XPolygon    aXPolygon;

public:
			TYPEINFO();
			XLineEndItem(long nIndex = -1);
			XLineEndItem(const String& rName, const XPolygon& rXPolygon);
			XLineEndItem(SfxItemPool* pPool, const XPolygon& rXPolygon);
			XLineEndItem(const XLineEndItem& rItem);
			XLineEndItem(SvStream& rIn);

	virtual int             operator==(const SfxPoolItem& rItem) const;
	virtual SfxPoolItem*    Clone(SfxItemPool* pPool = 0) const;
	virtual SfxPoolItem*    Create(SvStream& rIn, USHORT nVer) const;
	virtual SvStream&       Store(SvStream& rOut, USHORT nItemVersion ) const;

	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );


	const XPolygon& GetValue(const XLineEndTable* pTable = 0) const;
	void            SetValue(const XPolygon& rXPolygon) { aXPolygon = rXPolygon; Detach(); }

	XLineEndItem* checkForUniqueItem( SdrModel* pModel ) const;
};

}//end of namespace binfilter
#endif
