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


#ifndef _SFX_SHELL_HXX
#define _SFX_SHELL_HXX

#ifndef _DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif
#ifndef _RTTI_HXX //autogen
#include <tools/rtti.hxx>
#endif
#ifndef _SFXBRDCST_HXX //autogen
#include <bf_svtools/brdcst.hxx>
#endif

#include <tools/string.hxx>

namespace binfilter {

class SfxItemPool;
class SfxPoolItem;


struct SfxShell_Impl;

typedef SfxPoolItem* SfxPoolItemPtr;
SV_DECL_PTRARR_DEL( SfxItemPtrArray, SfxPoolItemPtr, 4, 4 )//STRIP008 ;
typedef SfxItemPtrArray SfxItemArray_Impl;

//====================================================================

class SfxShell: public SfxBroadcaster
{
	friend class SfxObjectItem;

	SfxShell_Impl*				pImp;
	SfxItemPool*				pPool;
	String						aName;

private:
								SfxShell( const SfxShell & ); // n.i.
	SfxShell&					operator = ( const SfxShell & ); // n.i.

protected:
								SfxShell();
public:
								TYPEINFO();
	virtual                     ~SfxShell();

	void						SetName( const String &rName ) { aName = rName; }
	const String&   			GetName() const { return aName; }

	SfxItemPool&				GetPool() const { return *pPool; }
	void						SetPool( SfxItemPool *pNewPool ) { pPool = pNewPool; }

	const SfxPoolItem*			GetItem( USHORT nSlotId ) const;
	void						PutItem( const SfxPoolItem& rItem );
	void						RemoveItem( USHORT nSlotId );
};

}//end of namespace binfilter
#endif

