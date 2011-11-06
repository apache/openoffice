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



#ifndef _SVX_XMLCNITM_HXX
#define _SVX_XMLCNITM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif
namespace rtl { class OUString; }
namespace binfilter {
class SvXMLNamespaceMap;
class SvXMLAttrContainerData;
//STRIP008 namespace rtl { class OUString; }

//============================================================================


class SvXMLAttrContainerItem: public SfxPoolItem
{
	SvXMLAttrContainerData	*pImpl;

public:
	TYPEINFO();

	SvXMLAttrContainerItem( sal_uInt16 nWhich = 0 );
	SvXMLAttrContainerItem( const SvXMLAttrContainerItem& );
	virtual ~SvXMLAttrContainerItem();

	virtual int operator==( const SfxPoolItem& ) const;


	virtual sal_uInt16 GetVersion( sal_uInt16 nFileFormatVersion ) const;

	virtual	BOOL        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	BOOL			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	virtual SfxPoolItem *Clone( SfxItemPool * = 0) const
	{ return new SvXMLAttrContainerItem( *this ); }

	sal_Bool AddAttr( const ::rtl::OUString& rLName,
			  	  	  const ::rtl::OUString& rValue );
	sal_Bool AddAttr( const ::rtl::OUString& rPrefix,
			  	  	  const ::rtl::OUString& rNamespace,
			  	  	  const ::rtl::OUString& rLName,
			  	  	  const ::rtl::OUString& rValue );

	sal_uInt16 GetAttrCount() const;
	::rtl::OUString GetAttrNamespace( sal_uInt16 i ) const;
	::rtl::OUString GetAttrPrefix( sal_uInt16 i ) const;
	const ::rtl::OUString& GetAttrLName( sal_uInt16 i ) const;
	const ::rtl::OUString& GetAttrValue( sal_uInt16 i ) const;

	sal_uInt16 GetFirstNamespaceIndex() const;
	sal_uInt16 GetNextNamespaceIndex( sal_uInt16 nIdx ) const;
	const ::rtl::OUString& GetNamespace( sal_uInt16 i ) const;
	const ::rtl::OUString& GetPrefix( sal_uInt16 i ) const;
};

}//end of namespace binfilter
#endif //  _SVX_XMLCNITM_HXX

