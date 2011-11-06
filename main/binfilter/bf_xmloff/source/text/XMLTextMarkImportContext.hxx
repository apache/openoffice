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




#ifndef _XMLOFF_XMLTEXTMARKIMPORTCONTEXT_HXX
#define _XMLOFF_XMLTEXTMARKIMPORTCONTEXT_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include "xmlictxt.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_ 
#include <com/sun/star/uno/Reference.h>
#endif


namespace com { namespace sun { namespace star {
	namespace text {
		class XTextRange;
	}
	namespace xml { namespace sax {
		class XAttributeList;
	} }
} } }
namespace rtl {
	class OUString;
}
namespace binfilter {
class XMLTextImportHelper;


/**
 * import bookmarks and reference marks
 * ( <bookmark>, <bookmark-start>, <bookmark-end>,
 *   <reference>, <reference-start>, <reference-end> )
 * 
 * All elements are handled by the same class due to their similarities.
 */
class XMLTextMarkImportContext : public SvXMLImportContext
{
	
	XMLTextImportHelper& rHelper;

public:
	
	TYPEINFO();

	XMLTextMarkImportContext(
		SvXMLImport& rImport, 
		XMLTextImportHelper& rHlp,
		sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName );

protected:

	virtual void StartElement(
		const ::com::sun::star::uno::Reference< 
			::com::sun::star::xml::sax::XAttributeList> & xAttrList);

public:
	static void CreateAndInsertMark(
		SvXMLImport& rImport,
		const ::rtl::OUString& sServiceName,
		const ::rtl::OUString& sMarkName,
		const ::com::sun::star::uno::Reference<
			::com::sun::star::text::XTextRange> & rRange);

	static sal_Bool FindName(
		SvXMLImport& rImport,
		const ::com::sun::star::uno::Reference<
			::com::sun::star::xml::sax::XAttributeList> & xAttrList,
		::rtl::OUString& sName);
};

}//end of namespace binfilter
#endif
