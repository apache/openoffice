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



#ifndef _XMLOFF_XMLINDEXBIBLIOGRAPHYCONFIGURATIONCONTEXT_HXX_
#define _XMLOFF_XMLINDEXBIBLIOGRAPHYCONFIGURATIONCONTEXT_HXX_

#ifndef _XMLOFF_XMLSTYLE_HXX
#include "xmlstyle.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_ 
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_ 
#include <com/sun/star/uno/Sequence.h>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_ 
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif


#include <vector>

namespace com { namespace sun { namespace star {
	namespace xml { namespace sax { class XAttributeList; } }
} } }
namespace binfilter {


/**
 * Import bibliography configuration.
 *
 * Little cheat: Cover all child elements in CreateChildContext.
 */
class XMLIndexBibliographyConfigurationContext : public SvXMLStyleContext
{
	const ::rtl::OUString sFieldMaster_Bibliography;
	const ::rtl::OUString sBracketBefore;
	const ::rtl::OUString sBracketAfter;
	const ::rtl::OUString sIsNumberEntries;
	const ::rtl::OUString sIsSortByPosition;
	const ::rtl::OUString sSortKeys;
	const ::rtl::OUString sSortKey;
	const ::rtl::OUString sIsSortAscending;
	const ::rtl::OUString sSortAlgorithm;
	const ::rtl::OUString sLocale;

	::rtl::OUString sSuffix;
	::rtl::OUString sPrefix;
    ::rtl::OUString sAlgorithm;
    ::com::sun::star::lang::Locale aLocale;
	sal_Bool bNumberedEntries;
	sal_Bool bSortByPosition;

	::std::vector< ::com::sun::star::uno::Sequence<
						::com::sun::star::beans::PropertyValue> > aSortKeys;

public:

	TYPEINFO();

	XMLIndexBibliographyConfigurationContext(
		SvXMLImport& rImport, 
		sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< 
			::com::sun::star::xml::sax::XAttributeList> & xAttrList);

	~XMLIndexBibliographyConfigurationContext();

protected:

	virtual void StartElement(
		const ::com::sun::star::uno::Reference< 
			::com::sun::star::xml::sax::XAttributeList> & xAttrList);

	virtual void CreateAndInsert( sal_Bool bOverwrite );

	virtual SvXMLImportContext *CreateChildContext( 
		sal_uInt16 nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< 
			::com::sun::star::xml::sax::XAttributeList> & xAttrList );

	void ProcessAttribute(
        sal_uInt16 nPrefix,
		const ::rtl::OUString sLocalName,
		const ::rtl::OUString sValue);
};

}//end of namespace binfilter
#endif
