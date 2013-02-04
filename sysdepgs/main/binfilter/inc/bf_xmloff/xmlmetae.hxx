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



#ifndef _XMLOFF_XMLMETAE_HXX
#define _XMLOFF_XMLMETAE_HXX

#ifndef _COM_SUN_STAR_DOCUMENT_XDOCUMENTINFO_HPP_
#include <com/sun/star/document/XDocumentInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_XML_SAX_XDOCUMENTHANDLER_HPP_
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#endif
#ifndef _COM_SUN_STAR_XML_SAX_XATTRIBUTELIST_HPP_
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_DATETIME_HPP_
#include <com/sun/star/util/DateTime.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif
#ifndef _XMLOFF_XMLTOKEN_HXX
#include <bf_xmloff/xmltoken.hxx>
#endif
class Time;

namespace com { namespace sun { namespace star { namespace frame {
	class XModel;
} } } }
namespace binfilter {

class SvXMLNamespaceMap;
class SvXMLAttributeList;
class SvXMLExport;

class SfxXMLMetaExport
{
private:
    SvXMLExport&                                        rExport;
	::com::sun::star::uno::Reference<
		::com::sun::star::document::XDocumentInfo>		xDocInfo;
	::com::sun::star::uno::Reference<
		::com::sun::star::beans::XPropertySet>			xInfoProp;
	::com::sun::star::lang::Locale						aLocale;

	void	SimpleStringElement( const ::rtl::OUString& rPropertyName,
								 sal_uInt16 nNamespace,
							 enum ::binfilter::xmloff::token::XMLTokenEnum eElementName );
	void	SimpleDateTimeElement( const ::rtl::OUString& rPropertyName,
								   sal_uInt16 nNamespace,
							 enum ::binfilter::xmloff::token::XMLTokenEnum eElementName );

public:
	SfxXMLMetaExport( SvXMLExport& rExport,
					  const ::com::sun::star::uno::Reference<
						::com::sun::star::frame::XModel>& rDocModel );

	virtual ~SfxXMLMetaExport();

	// core API
	void Export();

	static ::rtl::OUString GetISODateTimeString(
						const ::com::sun::star::util::DateTime& rDateTime );
	static ::rtl::OUString GetISODurationString( const Time& rTime );
};

}//end of namespace binfilter
#endif // _XMLOFF_XMLMETAE_HXX

