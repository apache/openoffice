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



#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif

#ifndef _XMLOFF_I18NMAP_HXX
#include "i18nmap.hxx"
#endif
namespace binfilter {

using namespace rtl;

class SvI18NMapEntry_Impl
{
	USHORT			nKind;
	OUString		aName;
	OUString		aNewName;

public:

	const OUString& GetNewName() const { return aNewName; }

	SvI18NMapEntry_Impl( USHORT nKnd, const OUString& rName,
						 const OUString& rNewName ) :
		nKind( nKnd ),
		aName( rName ),
		aNewName( rNewName )
	{}

	SvI18NMapEntry_Impl( USHORT nKnd, const OUString& rName ) :
		nKind( nKnd ),
		aName( rName )
	{}

	BOOL operator==( const SvI18NMapEntry_Impl& r ) const
	{
		return nKind == r.nKind &&
			   aName == r.aName;
	}

	BOOL operator<( const SvI18NMapEntry_Impl& r ) const
	{
		return nKind < r.nKind ||
			   ( nKind == r.nKind &&
				 aName < r.aName);
	}
};

typedef SvI18NMapEntry_Impl *SvI18NMapEntry_ImplPtr;
SV_DECL_PTRARR_SORT_DEL( SvI18NMap_Impl, SvI18NMapEntry_ImplPtr, 20, 5 )
SV_IMPL_OP_PTRARR_SORT( SvI18NMap_Impl, SvI18NMapEntry_ImplPtr )

// ---------------------------------------------------------------------

SvI18NMapEntry_Impl *SvI18NMap::_Find( USHORT nKind,
									 const OUString& rName ) const
{
	SvI18NMapEntry_Impl *pRet = 0;
	SvI18NMapEntry_Impl aTst( nKind, rName );

	USHORT nPos;
	if( pImpl->Seek_Entry( &aTst, &nPos ) )
	{
		pRet = (*pImpl)[nPos];
	}

	return pRet;
}

SvI18NMap::SvI18NMap() :
	pImpl( 0 )
{
	pImpl = new SvI18NMap_Impl;
}

SvI18NMap::~SvI18NMap()
{
	delete pImpl;
}

void SvI18NMap::Add( USHORT nKind, const OUString& rName, 
					 const OUString& rNewName )
{
	SvI18NMapEntry_Impl *pEntry = _Find( nKind, rName );
	DBG_ASSERT( !pEntry, "SvI18NMap::Add: item registered already" );
	if( !pEntry )
	{
		pEntry = new SvI18NMapEntry_Impl( nKind, rName, rNewName );
		pImpl->Insert( pEntry );
	}
}

const OUString& SvI18NMap::Get( USHORT nKind, const OUString& rName ) const
{
	SvI18NMapEntry_Impl *pEntry = _Find( nKind, rName );
	if( pEntry )
		return pEntry->GetNewName();
	else
		return rName;
}


}//end of namespace binfilter
