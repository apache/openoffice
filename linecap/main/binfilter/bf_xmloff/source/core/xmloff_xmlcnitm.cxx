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



#ifndef _TOOLS_DEBUG_HXX 
#include <tools/debug.hxx>
#endif
#ifndef _COM_SUN_STAR_XML_ATTRIBUTEDATA_HPP_
#include <com/sun/star/xml/AttributeData.hpp>
#endif

#include "xmlcnimp.hxx"
namespace binfilter {

using namespace rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::xml;

typedef ::rtl::OUString *OUStringPtr;
SV_DECL_PTRARR_DEL( SvXMLAttrContainerData_Impl, OUStringPtr, 5, 5 )
SV_IMPL_PTRARR( SvXMLAttrContainerData_Impl, OUStringPtr )


SvXMLAttrContainerData::SvXMLAttrContainerData(
							const SvXMLAttrContainerData& rImpl ) :
	aNamespaceMap( rImpl.aNamespaceMap ),
	pLNames( new SvXMLAttrContainerData_Impl ),
	pValues( new SvXMLAttrContainerData_Impl )
{
	USHORT nCount = rImpl.pLNames->Count();
	for( USHORT i=0; i<nCount; i++ )
	{
		aPrefixPoss.Insert( rImpl.aPrefixPoss[i], i );
		pLNames->Insert( new OUString( *(*rImpl.pLNames)[i] ), i );
		pValues->Insert( new OUString( *(*rImpl.pValues)[i] ), i );
	}
}

SvXMLAttrContainerData::SvXMLAttrContainerData() :
	pLNames( new SvXMLAttrContainerData_Impl ),
	pValues( new SvXMLAttrContainerData_Impl )
{
}

SvXMLAttrContainerData::~SvXMLAttrContainerData() 
{
	delete pLNames;
	delete pValues;
}

int SvXMLAttrContainerData::operator ==(
						const SvXMLAttrContainerData& rCmp ) const
{
	BOOL bRet = pLNames->Count() == rCmp.pLNames->Count() &&
				aNamespaceMap == rCmp.aNamespaceMap;
	if( bRet )
	{
		USHORT nCount = pLNames->Count();
		USHORT i;
		for( i=0; bRet && i < nCount; i++ )
			bRet = aPrefixPoss[i] == rCmp.aPrefixPoss[i];

		if( bRet )
		{
			for( i=0; bRet && i < nCount; i++ )
				bRet = *(*pLNames)[i] == *(*rCmp.pLNames)[i] &&
					   *(*pValues)[i] == *(*rCmp.pValues)[i];
		}
	}

	return (int)bRet;
}

BOOL SvXMLAttrContainerData::AddAttr( const OUString& rLName,
									   	   const OUString& rValue )
{
	aPrefixPoss.Insert( USHRT_MAX, aPrefixPoss.Count() );
	pLNames->Insert( new OUString(rLName), pLNames->Count() );
	pValues->Insert( new OUString(rValue), pValues->Count() );

	return TRUE;
}

BOOL SvXMLAttrContainerData::AddAttr( const OUString& rPrefix,
									   	   const OUString& rNamespace,
									       const OUString& rLName,
									 	   const OUString& rValue )
{
	USHORT nPos = aNamespaceMap.Add( rPrefix, rNamespace );
	aPrefixPoss.Insert( nPos, aPrefixPoss.Count() );
	pLNames->Insert( new OUString(rLName), pLNames->Count() );
	pValues->Insert( new OUString(rValue), pValues->Count() );

	return TRUE;
}

BOOL SvXMLAttrContainerData::AddAttr( const OUString& rPrefix,
									       const OUString& rLName,
									 	   const OUString& rValue )
{
	USHORT nPos = aNamespaceMap.GetIndexByPrefix( rPrefix );
	if( USHRT_MAX == nPos )
		return FALSE;

	aPrefixPoss.Insert( nPos, aPrefixPoss.Count() );
	pLNames->Insert( new OUString(rLName), pLNames->Count() );
	pValues->Insert( new OUString(rValue), pValues->Count() );

	return TRUE;
}

BOOL SvXMLAttrContainerData::SetAt( USHORT i,
			const ::rtl::OUString& rLName, const ::rtl::OUString& rValue )
{
	if( i >= GetAttrCount() )
		return FALSE;

	*(*pLNames)[i] = rLName;
	*(*pValues)[i] = rValue;
	aPrefixPoss[i] = USHRT_MAX;

	return TRUE;
}

BOOL SvXMLAttrContainerData::SetAt( USHORT i,
			const ::rtl::OUString& rPrefix, const ::rtl::OUString& rNamespace,
			const ::rtl::OUString& rLName, const ::rtl::OUString& rValue )
{
	if( i >= GetAttrCount() )
		return FALSE;

	USHORT nPos = aNamespaceMap.Add( rPrefix, rNamespace );
	if( USHRT_MAX == nPos )
		return FALSE;

	*(*pLNames)[i] = rLName;
	*(*pValues)[i] = rValue;
	aPrefixPoss[i] = nPos;

	return TRUE;
}

BOOL SvXMLAttrContainerData::SetAt( USHORT i,
			const ::rtl::OUString& rPrefix,
			const ::rtl::OUString& rLName,
			const ::rtl::OUString& rValue )
{
	if( i >= GetAttrCount() )
		return FALSE;

	USHORT nPos = aNamespaceMap.GetIndexByPrefix( rPrefix );
	if( USHRT_MAX == nPos )
		return FALSE;

	*(*pLNames)[i] = rLName;
	*(*pValues)[i] = rValue;
	aPrefixPoss[i] = nPos;

	return TRUE;
}

void SvXMLAttrContainerData::Remove( USHORT i )
{
	if( i < GetAttrCount() )
	{
		delete (*pLNames)[i];
		pLNames->Remove( i );
		delete (*pValues)[i];
		pValues->Remove( i );
		aPrefixPoss.Remove( i );
	}
	else
	{
		DBG_ERROR( "illegal index" );
	}
}

sal_uInt16 SvXMLAttrContainerData::GetAttrCount() const
{
	return pLNames->Count();
}

const ::rtl::OUString& SvXMLAttrContainerData::GetAttrLName(sal_uInt16 i) const
{
	OSL_ENSURE( i >= 0 && i < pLNames->Count(),
				"SvXMLAttrContainerData::GetLName: illegal index" );
	return *(*pLNames)[i];
}

const ::rtl::OUString& SvXMLAttrContainerData::GetAttrValue(sal_uInt16 i) const
{
	OSL_ENSURE( i >= 0 && i < pValues->Count(),
				"SvXMLAttrContainerData::GetValue: illegal index" );
	return *(*pValues)[i];
}

}//end of namespace binfilter
