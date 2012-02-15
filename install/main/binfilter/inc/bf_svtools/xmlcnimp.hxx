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



#ifndef _SVTOOLS_XMLCNIMP_HXX
#define _SVTOOLS_XMLCNIMP_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif

#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif

#ifndef _SVTOOLS_NMSPMAP_HXX
#include <xmloff/nmspmap.hxx>
#endif

namespace rtl { class OUString; }

namespace binfilter
{

typedef ::rtl::OUString *OUStringPtr;

SV_DECL_PTRARR_DEL( OUStringsDtor2_Impl, OUStringPtr, 5, 5 )

class SvXMLAttrContainerItem_Impl
{
	SvXMLNamespaceMap	aNamespaceMap;
	SvUShorts			aPrefixPoss;
	OUStringsDtor2_Impl	aLNames;
	OUStringsDtor2_Impl	aValues;

	inline sal_uInt16 GetPrefixPos( sal_uInt16 i ) const;
public:

	SvXMLAttrContainerItem_Impl( const SvXMLAttrContainerItem_Impl& rImpl );
	SvXMLAttrContainerItem_Impl();

	int  operator ==( const SvXMLAttrContainerItem_Impl& rCmp ) const;

	BOOL AddAttr( const ::rtl::OUString& rLName, const ::rtl::OUString& rValue );
	BOOL AddAttr( const ::rtl::OUString& rPrefix, const ::rtl::OUString& rNamespace,
			  	  const ::rtl::OUString& rLName, const ::rtl::OUString& rValue );
	BOOL AddAttr( const ::rtl::OUString& rPrefix,
				  const ::rtl::OUString& rLName,
				  const ::rtl::OUString& rValue );

	sal_uInt16 GetAttrCount() const { return aLNames.Count(); }
	inline ::rtl::OUString GetAttrNamespace( sal_uInt16 i ) const;
	inline ::rtl::OUString GetAttrPrefix( sal_uInt16 i ) const;
	inline const ::rtl::OUString& GetAttrLName( sal_uInt16 i ) const;
	inline const ::rtl::OUString& GetAttrValue( sal_uInt16 i ) const;

	sal_uInt16 GetFirstNamespaceIndex() const { return aNamespaceMap.GetFirstIndex(); }
	sal_uInt16 GetNextNamespaceIndex( sal_uInt16 nIdx ) const { return aNamespaceMap.GetNextIndex( nIdx ); }
	inline const ::rtl::OUString& GetNamespace( sal_uInt16 i ) const;
	inline const ::rtl::OUString& GetPrefix( sal_uInt16 i ) const;

	BOOL SetAt( sal_uInt16 i,
				const ::rtl::OUString& rLName, const ::rtl::OUString& rValue );
	BOOL SetAt( sal_uInt16 i,
				const ::rtl::OUString& rPrefix, const ::rtl::OUString& rNamespace,
				const ::rtl::OUString& rLName, const ::rtl::OUString& rValue );
	BOOL SetAt( sal_uInt16 i,
				const ::rtl::OUString& rPrefix,
				const ::rtl::OUString& rLName,
				const ::rtl::OUString& rValue );

	void Remove( sal_uInt16 i );
};

inline sal_uInt16 SvXMLAttrContainerItem_Impl::GetPrefixPos( sal_uInt16 i ) const
{
//	DBG_ASSERT( i >= 0 && i < aPrefixPoss.Count(),
//				"SvXMLAttrContainerItem_Impl::GetPrefixPos: illegal index" );
	return aPrefixPoss[i];
}

inline ::rtl::OUString SvXMLAttrContainerItem_Impl::GetAttrNamespace( sal_uInt16 i ) const
{
	::rtl::OUString sRet;
	sal_uInt16 nPos = GetPrefixPos( i );
	if( USHRT_MAX != nPos )
		sRet = aNamespaceMap.GetNameByIndex( nPos );
	return sRet;
}

inline ::rtl::OUString SvXMLAttrContainerItem_Impl::GetAttrPrefix( sal_uInt16 i ) const
{
	::rtl::OUString sRet;
	sal_uInt16 nPos = GetPrefixPos( i );
	if( USHRT_MAX != nPos )
		sRet = aNamespaceMap.GetPrefixByIndex( nPos );
	return sRet;
}

inline const ::rtl::OUString& SvXMLAttrContainerItem_Impl::GetAttrLName(sal_uInt16 i) const
{
	DBG_ASSERT( i >= 0 && i < aLNames.Count(),
				"SvXMLAttrContainerItem_Impl::GetLName: illegal index" );
	return *aLNames[i];
}

inline const ::rtl::OUString& SvXMLAttrContainerItem_Impl::GetAttrValue(sal_uInt16 i) const
{
	DBG_ASSERT( i >= 0 && i < aValues.Count(),
				"SvXMLAttrContainerItem_Impl::GetValue: illegal index" );
	return *aValues[i];
}

inline const ::rtl::OUString& SvXMLAttrContainerItem_Impl::GetNamespace(
															sal_uInt16 i ) const
{
	return aNamespaceMap.GetNameByIndex( i );
}

inline const ::rtl::OUString& SvXMLAttrContainerItem_Impl::GetPrefix( sal_uInt16 i ) const
{
	return aNamespaceMap.GetPrefixByIndex( i );
}

}

#endif

