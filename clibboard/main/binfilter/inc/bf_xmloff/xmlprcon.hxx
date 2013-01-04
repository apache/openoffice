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



#ifndef _XMLOFF_XMLPROPERTYSETCONTEXT_HXX
#define _XMLOFF_XMLPROPERTYSETCONTEXT_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include <bf_xmloff/xmlictxt.hxx>
#endif

#ifndef _XMLOFF_XMLIMPPR_HXX
#include <bf_xmloff/xmlimppr.hxx>
#endif
#ifndef _XMLOFF_PROPERTYSETMAPPER_HXX
#include <bf_xmloff/xmlprmap.hxx>
#endif
#ifndef _UNIVERSALL_REFERENCE_HXX
#include <bf_xmloff/uniref.hxx>
#endif

namespace rtl { class OUString; }
namespace binfilter {

class SvXMLImportPropertyMapper;

class SvXMLPropertySetContext : public SvXMLImportContext
{
protected:
	sal_Int32 nStartIdx;
	sal_Int32 nEndIdx;
	::std::vector< XMLPropertyState > &rProperties;
	UniReference < SvXMLImportPropertyMapper >   xMapper;

public:

	SvXMLPropertySetContext(
			SvXMLImport& rImport, USHORT nPrfx,
			const ::rtl::OUString& rLName,
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList,
			::std::vector< XMLPropertyState > &rProps,
			const UniReference < SvXMLImportPropertyMapper > &rMap,
		  	sal_Int32 nStartIdx = -1, sal_Int32 nEndIdx = -1 );

	virtual ~SvXMLPropertySetContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );

	// This method is called from this instance implementation of
	// CreateChildContext if the element matches an entry in the
	// SvXMLImportItemMapper with the mid flag MID_FLAG_ELEMENT_ITEM_IMPORT
	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
								   const ::rtl::OUString& rLocalName,
								   const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList,
								   ::std::vector< XMLPropertyState > &rProperties,
								   const XMLPropertyState& rProp );

};

}//end of namespace binfilter
#endif	//  _XMLOFF_XMLPROPERTYSETCONTEXT_HXX

