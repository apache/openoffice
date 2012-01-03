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



#ifndef _SVX_XLNDSIT_HXX
#define _SVX_XLNDSIT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVX_XIT_HXX //autogen
#include <bf_svx/xit.hxx>
#endif

#ifndef _SVX_XLINIIT_HXX //autogen
#include <bf_svx/xdash.hxx>
#endif
namespace binfilter {

class SdrModel;

//--------------------
// class XLineDashItem
//--------------------
class XLineDashItem : public NameOrIndex
{
	XDash   aDash;

public:
							TYPEINFO();
							XLineDashItem() {}
							XLineDashItem(const String& rName, const XDash& rTheDash);
							XLineDashItem(SfxItemPool* pPool, const XDash& rTheDash);
							XLineDashItem(const XLineDashItem& rItem);
							XLineDashItem(SvStream& rIn);

	virtual int             operator==(const SfxPoolItem& rItem) const;
	virtual SfxPoolItem*    Clone(SfxItemPool* pPool = 0) const;
	virtual SfxPoolItem*    Create(SvStream& rIn, USHORT nVer) const;
	virtual SvStream&       Store(SvStream& rOut, USHORT nItemVersion ) const;

	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );


	const XDash&			GetValue(const XDashTable* pTable = 0) const;
	void					SetValue(const XDash& rNew)   { aDash = rNew; Detach(); }

	static BOOL CompareValueFunc( const NameOrIndex* p1, const NameOrIndex* p2 );
	XLineDashItem* checkForUniqueItem( SdrModel* pModel ) const;
};

}//end of namespace binfilter
#endif

