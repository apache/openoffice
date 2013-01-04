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



#ifndef _SVX_ROTMODIT_HXX
#define _SVX_ROTMODIT_HXX

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
namespace binfilter {

//----------------------------------------------------------------------------
//	Ausrichtung bei gedrehtem Text

enum SvxRotateMode
{
	SVX_ROTATE_MODE_STANDARD,
	SVX_ROTATE_MODE_TOP,
	SVX_ROTATE_MODE_CENTER,
	SVX_ROTATE_MODE_BOTTOM
};

class SvxRotateModeItem: public SfxEnumItem
{
public:
				TYPEINFO();

                SvxRotateModeItem( SvxRotateMode eMode=SVX_ROTATE_MODE_STANDARD, USHORT nWhich=0);
				SvxRotateModeItem( const SvxRotateModeItem& rItem );
				~SvxRotateModeItem();

	virtual USHORT				GetValueCount() const;
	virtual SfxPoolItem*		Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*		Create(SvStream &, USHORT) const;
	virtual USHORT				GetVersion( USHORT nFileVersion ) const;
	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );
};

}//end of namespace binfilter
#endif

