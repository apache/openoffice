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


#ifndef SC_XMLSORTI_HXX
#define SC_XMLSORTI_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include <bf_xmloff/xmlictxt.hxx>
#endif
#ifndef _XMLOFF_XMLIMP_HXX
#include <bf_xmloff/xmlimp.hxx>
#endif
#ifndef _COM_SUN_STAR_UTIL_SORTFIELD_HPP_
#include <com/sun/star/util/SortField.hpp>
#endif
#ifndef _COM_SUN_STAR_TABLE_CELLADDRESS_HPP_
#include <com/sun/star/table/CellAddress.hpp>
#endif

#include "xmldrani.hxx"
namespace binfilter {

class ScXMLImport;

class ScXMLSortContext : public SvXMLImportContext
{
	ScXMLDatabaseRangeContext* pDatabaseRangeContext;

	::com::sun::star::uno::Sequence < ::com::sun::star::util::SortField> aSortFields;
	::com::sun::star::table::CellAddress aOutputPosition;
	::rtl::OUString	sCountry;
	::rtl::OUString	sLanguage;
	::rtl::OUString	sAlgorithm;
	sal_Int16	nUserListIndex;
	sal_Bool	bCopyOutputData : 1;
	sal_Bool	bBindFormatsToContent : 1;
	sal_Bool	bIsCaseSensitive : 1;
	sal_Bool	bEnabledUserList : 1;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLSortContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList,
										ScXMLDatabaseRangeContext* pTempDatabaseRangeContext);

	virtual ~ScXMLSortContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();

	void AddSortField(const ::rtl::OUString& sFieldNumber, const ::rtl::OUString& sDataType, const ::rtl::OUString& sOrder);
};

class ScXMLSortByContext : public SvXMLImportContext
{
	ScXMLSortContext* pSortContext;

	::rtl::OUString	sFieldNumber;
	::rtl::OUString	sDataType;
	::rtl::OUString	sOrder;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLSortByContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList,
										ScXMLSortContext* pTempSortContext);

	virtual ~ScXMLSortByContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

} //namespace binfilter
#endif
