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



#ifndef _SVXE3DITEM_HXX
#define _SVXE3DITEM_HXX

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _VECTOR3D_HXX
#include <bf_goodies/vector3d.hxx>
#endif
class SvStream;
namespace binfilter {

#ifndef _SVXVECT3DITEM_HXX
#define _SVXVECT3DITEM_HXX


DBG_NAMEEX(SvxVector3DItem)//STRIP008

class SvxVector3DItem: public SfxPoolItem
{
	Vector3D				aVal;

public:
							TYPEINFO();
							SvxVector3DItem();
							SvxVector3DItem( USHORT nWhich, const Vector3D& rVal );
							SvxVector3DItem( const SvxVector3DItem& );
							~SvxVector3DItem() {
								 DBG_DTOR(SvxVector3DItem, 0); }

	virtual int				operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVersion) const;
	virtual SvStream&		Store(SvStream &, USHORT nItemVersion ) const;

	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	const Vector3D&			GetValue() const { return aVal; }
			void			SetValue( const Vector3D& rNewVal ) {
								 DBG_ASSERT( GetRefCount() == 0, "SetValue() with pooled item" );
								 aVal = rNewVal;
							 }

	virtual USHORT GetVersion (USHORT nFileFormatVersion) const;
};

#endif

}//end of namespace binfilter
#endif

