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



#ifndef _XMLOFF_METAIMPORTCOMPONENT_HXX
#define _XMLOFF_METAIMPORTCOMPONENT_HXX

#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XDOCUMENTINFO_HPP_
#include <com/sun/star/document/XDocumentInfo.hpp>
#endif
namespace binfilter {

class XMLMetaImportComponent : public SvXMLImport
{
private:
	::com::sun::star::uno::Reference<
		::com::sun::star::document::XDocumentInfo>	xDocInfo;

public:
	// #110680#
	// XMLMetaImportComponent() throw();
	XMLMetaImportComponent(
		const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory
		) throw();

	~XMLMetaImportComponent() throw();


protected:

	virtual SvXMLImportContext* CreateContext(
		sal_uInt16 nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference<
			::com::sun::star::xml::sax::XAttributeList > & xAttrList );

	// XImporter
    virtual void SAL_CALL setTargetDocument( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& xDoc )
    	throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
};


// global functions to support the component

::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
	XMLMetaImportComponent_getSupportedServiceNames()
	throw();

::rtl::OUString SAL_CALL XMLMetaImportComponent_getImplementationName()
	throw();

::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
	XMLMetaImportComponent_createInstance(
		const ::com::sun::star::uno::Reference<
			::com::sun::star::lang::XMultiServiceFactory > & )
	throw( ::com::sun::star::uno::Exception );

}//end of namespace binfilter
#endif

