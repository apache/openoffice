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



#ifndef _XMLSECURITY_DOCUMENTDIGITALSIGNATURES_HXX
#define _XMLSECURITY_DOCUMENTDIGITALSIGNATURES_HXX

#include <cppuhelper/implbase2.hxx>

#include "com/sun/star/lang/XInitialization.hpp"
#include <com/sun/star/security/XDocumentDigitalSignatures.hpp>
#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <xmlsecurity/documentsignaturehelper.hxx>

namespace com { namespace  sun { namespace star {

	namespace uno {
		class XComponentContext;
	}
}}}

class DocumentDigitalSignatures : public cppu::WeakImplHelper2
<
	com::sun::star::security::XDocumentDigitalSignatures,
	com::sun::star::lang::XInitialization
>
{
private:
    com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext > mxCtx;
    // will be set by XInitialization. If not we assume true. false means an earlier version.
    ::rtl::OUString m_sODFVersion;
    //The number of arguments which were passed in XInitialization::initialize
    int m_nArgumentsCount;
    //Indicates if the document already contains a document signature
    bool m_bHasDocumentSignature;

    sal_Bool ImplViewSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xSignStream, DocumentSignatureMode eMode, bool bReadOnly );
    sal_Bool ImplViewSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignStream, DocumentSignatureMode eMode, bool bReadOnly );
    com::sun::star::uno::Sequence< ::com::sun::star::security::DocumentSignatureInformation > ImplVerifySignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& rxStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignStream, DocumentSignatureMode eMode );

public:
    DocumentDigitalSignatures( const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext>& rxCtx );

    // for service registration...
	static ::rtl::OUString GetImplementationName();
	static ::com::sun::star::uno::Sequence < ::rtl::OUString > GetSupportedServiceNames();

	//XInitialization
	void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

    // XDocumentDigitalSignatures
    ::sal_Bool SAL_CALL signDocumentContent( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xSignStream );
    ::com::sun::star::uno::Sequence< ::com::sun::star::security::DocumentSignatureInformation > SAL_CALL verifyDocumentContentSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream );
    void SAL_CALL showDocumentContentSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream );
    ::rtl::OUString SAL_CALL getDocumentContentSignatureDefaultStreamName(  );
    ::sal_Bool SAL_CALL signScriptingContent( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xSignStream );
    ::com::sun::star::uno::Sequence< ::com::sun::star::security::DocumentSignatureInformation > SAL_CALL verifyScriptingContentSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream );
    void SAL_CALL showScriptingContentSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream );
    ::rtl::OUString SAL_CALL getScriptingContentSignatureDefaultStreamName(  );
    ::sal_Bool SAL_CALL signPackage( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream >& xSignStream );
    ::com::sun::star::uno::Sequence< ::com::sun::star::security::DocumentSignatureInformation > SAL_CALL verifyPackageSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& Storage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream );
    void SAL_CALL showPackageSignatures( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xSignInStream );
    ::rtl::OUString SAL_CALL getPackageSignatureDefaultStreamName(  );
    void SAL_CALL showCertificate( const ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate >& Certificate );
    void SAL_CALL manageTrustedSources(  );
    ::sal_Bool SAL_CALL isAuthorTrusted( const ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate >& Author );
    ::sal_Bool SAL_CALL isLocationTrusted( const ::rtl::OUString& Location );
    void SAL_CALL addAuthorToTrustedSources( const ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate >& Author );
    void SAL_CALL addLocationToTrustedSources( const ::rtl::OUString& Location );

};

com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SAL_CALL DocumentDigitalSignatures_CreateInstance(
	const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >& rCtx);

#endif // _XMLSECURITY_DOCUMENTDIGITALSIGNATURES_HXX
