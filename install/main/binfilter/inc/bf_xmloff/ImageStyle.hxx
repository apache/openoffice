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



#ifndef _XMLOFF_IMAGESTYLE_HXX
#define _XMLOFF_IMAGESTYLE_HXX

#ifndef _COM_SUN_STAR_XML_SAX_XDOCUMENTHANDLER_HPP_
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#endif
namespace binfilter {

class SvXMLNamespaceMap;
class SvXMLAttributeList;
class SvXMLUnitConverter;
class SvXMLExport;
class SvXMLImport;

class XMLImageStyle
{
public:
	XMLImageStyle();
	~XMLImageStyle();

	sal_Bool exportXML( const ::rtl::OUString& rStrName, const ::com::sun::star::uno::Any& rValue, SvXMLExport& rExport );
	sal_Bool importXML( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList, ::com::sun::star::uno::Any& rValue, ::rtl::OUString& rStrName, SvXMLImport& rImport );

private:

	sal_Bool ImpExportXML( const ::rtl::OUString& rStrName, const ::com::sun::star::uno::Any& rValue,
						   SvXMLExport& rExport );
	sal_Bool ImpImportXML( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList, 
						   ::com::sun::star::uno::Any& rValue, ::rtl::OUString& rStrName,
						   SvXMLImport& rImport );
};

}//end of namespace binfilter
#endif // _XMLOFF_IMAGESTYLE_HXX
