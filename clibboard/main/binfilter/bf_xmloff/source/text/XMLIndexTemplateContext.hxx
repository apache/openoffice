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



#ifndef _XMLOFF_XMLINDEXTEMPLATECONTEXT_HXX_
#define _XMLOFF_XMLINDEXTEMPLATECONTEXT_HXX_

#ifndef _XMLOFF_XMLICTXT_HXX 
#include "xmlictxt.hxx"
#endif

#ifndef _XMLOFF_XMLTOKEN_HXX
#include "xmltoken.hxx"
#endif

#include <vector>

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_ 
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUES_HPP_
#include <com/sun/star/beans/PropertyValues.hpp>
#endif


namespace com { namespace sun { namespace star {
	namespace xml { namespace sax { class XAttributeList; } }
	namespace beans { class XPropertySet; }
} } }
namespace rtl {	class OUString; }
namespace binfilter {
struct SvXMLEnumMapEntry;


// constants for the XMLIndexTemplateContext constructor

// TOC and user defined index:
extern const SvXMLEnumMapEntry aLevelNameTOCMap[];
extern const sal_Char* aLevelStylePropNameTOCMap[];
extern const sal_Bool aAllowedTokenTypesTOC[];
extern const sal_Bool aAllowedTokenTypesUser[];

// alphabetical index:
extern const SvXMLEnumMapEntry aLevelNameAlphaMap[];
extern const sal_Char* aLevelStylePropNameAlphaMap[];
extern const sal_Bool aAllowedTokenTypesAlpha[];

// bibliography:
extern const SvXMLEnumMapEntry aLevelNameBibliographyMap[];
extern const sal_Char* aLevelStylePropNameBibliographyMap[];
extern const sal_Bool aAllowedTokenTypesBibliography[];

// table, illustration and object tables:
extern const SvXMLEnumMapEntry* aLevelNameTableMap;	// NULL: no outline-level
extern const sal_Char* aLevelStylePropNameTableMap[];
extern const sal_Bool aAllowedTokenTypesTable[];


/**
 * Import index entry templates
 */
class XMLIndexTemplateContext : public SvXMLImportContext
{
	// pick up PropertyValues to be turned into a sequence.
	::std::vector< ::com::sun::star::beans::PropertyValues > aValueVector;

	::rtl::OUString sStyleName;

	const SvXMLEnumMapEntry* pOutlineLevelNameMap;
	enum ::binfilter::xmloff::token::XMLTokenEnum eOutlineLevelAttrName;
	const sal_Char** pOutlineLevelStylePropMap;
	const sal_Bool* pAllowedTokenTypesMap;

	sal_Int32 nOutlineLevel;
	sal_Bool bStyleNameOK;
	sal_Bool bOutlineLevelOK;

	// PropertySet of current index
	::com::sun::star::uno::Reference< 
		::com::sun::star::beans::XPropertySet> & rPropertySet;
	
public:

	// constants made available to other contexts (template entry
	// contexts, in particular)
	const ::rtl::OUString sTokenEntryNumber;
	const ::rtl::OUString sTokenEntryText;
	const ::rtl::OUString sTokenTabStop;
	const ::rtl::OUString sTokenText;
	const ::rtl::OUString sTokenPageNumber;
	const ::rtl::OUString sTokenChapterInfo;
	const ::rtl::OUString sTokenHyperlinkStart;
	const ::rtl::OUString sTokenHyperlinkEnd;
	const ::rtl::OUString sTokenBibliographyDataField;

	const ::rtl::OUString sCharacterStyleName;
	const ::rtl::OUString sTokenType;
	const ::rtl::OUString sText;
	const ::rtl::OUString sTabStopRightAligned;
	const ::rtl::OUString sTabStopPosition;
	const ::rtl::OUString sTabStopFillCharacter;
	const ::rtl::OUString sBibliographyDataField;
	const ::rtl::OUString sChapterFormat;

	const ::rtl::OUString sLevelFormat;
	const ::rtl::OUString sParaStyleLevel;


	TYPEINFO();

	XMLIndexTemplateContext(
		SvXMLImport& rImport, 
		::com::sun::star::uno::Reference< 
			::com::sun::star::beans::XPropertySet> & rPropSet,
		sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName,
		const SvXMLEnumMapEntry* aLevelNameMap,
		enum ::binfilter::xmloff::token::XMLTokenEnum eLevelAttrName,
		const sal_Char** aLevelStylePropNameMap,
		const sal_Bool* aAllowedTokenTypes);

	~XMLIndexTemplateContext();

	/** add template; to be called by child template entry contexts */
	void addTemplateEntry(
		const ::com::sun::star::beans::PropertyValues& aValues);

protected:

	virtual void StartElement(
		const ::com::sun::star::uno::Reference< 
			::com::sun::star::xml::sax::XAttributeList> & xAttrList);

	virtual void EndElement();

	virtual SvXMLImportContext *CreateChildContext( 
		sal_uInt16 nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< 
			::com::sun::star::xml::sax::XAttributeList> & xAttrList );
};

}//end of namespace binfilter
#endif
