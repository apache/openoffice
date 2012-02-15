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



#ifndef _XMLOFF_XMLMETAI_HXX
#define _XMLOFF_XMLMETAI_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include <bf_xmloff/xmlictxt.hxx>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _COM_SUN_STAR_DOCUMENT_XDOCUMENTINFO_HPP_
#include <com/sun/star/document/XDocumentInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
namespace com { namespace sun { namespace star { namespace lang {
	struct Locale;
}}}}
namespace com { namespace sun { namespace star { namespace frame {
	class XModel;
}}}}
namespace binfilter {

class SvXMLTokenMap;


class SfxXMLMetaContext : public SvXMLImportContext
{
private:
	::com::sun::star::uno::Reference<
		::com::sun::star::document::XDocumentInfo>	xDocInfo;
	::com::sun::star::uno::Reference<
		::com::sun::star::beans::XPropertySet>		xInfoProp;
	::com::sun::star::uno::Reference<
		::com::sun::star::beans::XPropertySet>		xDocProp;
	SvXMLTokenMap*									pTokenMap;
	sal_Int32										nUserKeys;
	::rtl::OUStringBuffer							sKeywords;

public:
	SfxXMLMetaContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
							::com::sun::star::frame::XModel>& rDocModel );
	SfxXMLMetaContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
							::com::sun::star::document::XDocumentInfo>&	rDocInfo );
	virtual ~SfxXMLMetaContext();

	// Create child element.
	virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
				const ::rtl::OUString& rLocalName,
				const ::com::sun::star::uno::Reference<
					::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();

	const ::com::sun::star::uno::Reference<
		::com::sun::star::beans::XPropertySet>& GetInfoProp() const
			{ return xInfoProp; }
	const ::com::sun::star::uno::Reference<
		::com::sun::star::beans::XPropertySet>& GetDocProp() const
			{ return xDocProp; }

	void	AddKeyword( const ::rtl::OUString& rKW );
	void	AddUserField( const ::rtl::OUString& rName,
							const ::rtl::OUString& rContent );
};

}//end of namespace binfilter
#endif // _XMLOFF_XMLMETAI_HXX

