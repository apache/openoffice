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



#ifndef _XMLBRSHI_HXX
#define _XMLBRSHI_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include "xmlictxt.hxx"
#endif
namespace rtl { class OUString; }
namespace com { namespace sun { namespace star {
	namespace io { class XOutputStream; }
} } }
namespace binfilter {//STRIP009
class SvXMLImport; 
class SvXMLUnitConverter; 
class SvxBrushItem;
class SwXMLBrushItemImportContext : public SvXMLImportContext
{
private:
	::com::sun::star::uno::Reference < ::com::sun::star::io::XOutputStream > xBase64Stream;
	SvxBrushItem				*pItem;
	
	void ProcessAttrs(
			const ::com::sun::star::uno::Reference<
					::com::sun::star::xml::sax::XAttributeList > & xAttrList,
		   	const SvXMLUnitConverter& rUnitConv );

public:
	TYPEINFO();

	SwXMLBrushItemImportContext(
			SvXMLImport& rImport,
			sal_uInt16 nPrfx,
			const ::rtl::OUString& rLName,
		    const ::com::sun::star::uno::Reference<
					::com::sun::star::xml::sax::XAttributeList > & xAttrList,
			const SvXMLUnitConverter& rUnitConv,
		   	const SvxBrushItem& rItem	);

	SwXMLBrushItemImportContext(
			SvXMLImport& rImport,
			sal_uInt16 nPrfx,
			const ::rtl::OUString& rLName,
			const ::com::sun::star::uno::Reference<
					::com::sun::star::xml::sax::XAttributeList > & xAttrList,
			const SvXMLUnitConverter& rUnitConv,
			sal_uInt16 nWhich	);

	virtual ~SwXMLBrushItemImportContext();

	virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
				const ::rtl::OUString& rLocalName,
			 	const ::com::sun::star::uno::Reference<
					::com::sun::star::xml::sax::XAttributeList > & xAttrList );

	virtual void EndElement();

	const SvxBrushItem& GetItem() const { return *pItem; }
};


} //namespace binfilter
#endif	//  _XMLBRSHI_HXX

