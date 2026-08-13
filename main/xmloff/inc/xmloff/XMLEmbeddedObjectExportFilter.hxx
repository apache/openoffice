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



#ifndef _XMLOFF_XMLEMBEDDEDOBJECTEXPORTFILTER_HXX
#define _XMLOFF_XMLEMBEDDEDOBJECTEXPORTFILTER_HXX

#include <com/sun/star/xml/sax/XExtendedDocumentHandler.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase3.hxx>

class XMLEmbeddedObjectExportFilter : public ::cppu::WeakImplHelper3<
			 ::com::sun::star::xml::sax::XExtendedDocumentHandler,
			 ::com::sun::star::lang::XServiceInfo,
			 ::com::sun::star::lang::XInitialization>
{
	::com::sun::star::uno::Reference<
		::com::sun::star::xml::sax::XDocumentHandler > xHandler;
	::com::sun::star::uno::Reference<
		::com::sun::star::xml::sax::XExtendedDocumentHandler > xExtHandler;

public:
	XMLEmbeddedObjectExportFilter() throw();
	XMLEmbeddedObjectExportFilter( const ::com::sun::star::uno::Reference<
		::com::sun::star::xml::sax::XDocumentHandler > & rHandler ) throw();
	virtual ~XMLEmbeddedObjectExportFilter () throw();

	// ::com::sun::star::xml::sax::XDocumentHandler
	virtual void SAL_CALL startDocument(void);
	virtual void SAL_CALL endDocument(void);
	virtual void SAL_CALL startElement(const ::rtl::OUString& aName,
							  const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttribs);
	virtual void SAL_CALL endElement(const ::rtl::OUString& aName);
	virtual void SAL_CALL characters(const ::rtl::OUString& aChars);
	virtual void SAL_CALL ignorableWhitespace(const ::rtl::OUString& aWhitespaces);
	virtual void SAL_CALL processingInstruction(const ::rtl::OUString& aTarget,
									   const ::rtl::OUString& aData);
	virtual void SAL_CALL setDocumentLocator(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XLocator > & xLocator);

	// ::com::sun::star::xml::sax::XExtendedDocumentHandler
	virtual void SAL_CALL startCDATA(void);
	virtual void SAL_CALL endCDATA(void);
	virtual void SAL_CALL comment(const ::rtl::OUString& sComment);
	virtual void SAL_CALL allowLineBreak(void);
	virtual void SAL_CALL unknown(const ::rtl::OUString& sString);

	// XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

	// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

};

#endif	//  _XMLOFF_XMLEMBEDDEDOBJECTEXPORTFILTER_HXX
