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



#ifndef _SIGNATURECREATORIMPL_HXX
#define _SIGNATURECREATORIMPL_HXX

#include <com/sun/star/xml/crypto/sax/XBlockerMonitor.hpp>
#include <com/sun/star/xml/crypto/sax/XSignatureCreationResultListener.hpp>
#include <com/sun/star/xml/crypto/sax/XSignatureCreationResultBroadcaster.hpp>
#include <com/sun/star/xml/crypto/XSecurityEnvironment.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase4.hxx>

#include "signatureengine.hxx"

class SignatureCreatorImpl : public cppu::ImplInheritanceHelper4
<
	SignatureEngine,
	com::sun::star::xml::crypto::sax::XBlockerMonitor,
	com::sun::star::xml::crypto::sax::XSignatureCreationResultBroadcaster,
	com::sun::star::lang::XInitialization,
	com::sun::star::lang::XServiceInfo
>
/****** SignatureCreatorImpl.hxx/CLASS SignatureCreatorImpl *******************
 *
 *   NAME
 *	SignatureCreatorImpl -- generates a signature
 *
 *   FUNCTION
 *	Collects all resources for a signature generation, then generates the
 *	signature by invoking a xmlsec-based signature bridge component.
 *
 *   HISTORY
 *	05.01.2004 -	Interface supported: XBlockerMonitor,
 * 			XInitialization, XServiceInfo
 *
 *   AUTHOR
 *	Michael Mi
 *	Email: michael.mi@sun.com
 ******************************************************************************/
{
private:
	/*
	 * the Id of the signature, which is used for the result listener to
	 * identify the signature.
	 */
	sal_Int32 m_nSignatureId;

	/*
	 * the Id of template blocker.
	 */
	sal_Int32 m_nIdOfBlocker;

	/*
	 * the signature creation result
	 */
	bool      m_bCreationSucceed;

	com::sun::star::uno::Reference<
		com::sun::star::xml::crypto::XSecurityEnvironment > m_xSecurityEnvironment;

	virtual void notifyResultListener() const;
	virtual void clearUp( ) const;
	virtual bool checkReady() const;
	virtual void startEngine( const com::sun::star::uno::Reference<
		com::sun::star::xml::crypto::XXMLSignatureTemplate >&
		xSignatureTemplate);

public:
	explicit SignatureCreatorImpl( const com::sun::star::uno::Reference<
		com::sun::star::lang::XMultiServiceFactory >& rxMSF);
	virtual ~SignatureCreatorImpl();

	/* XBlockerMonitor */
	virtual void SAL_CALL setBlockerId( sal_Int32 id );

	/* XSignatureCreationResultBroadcaster */
	void SAL_CALL addSignatureCreationResultListener(
		const com::sun::star::uno::Reference<
			com::sun::star::xml::crypto::sax::XSignatureCreationResultListener >& listener );

	void SAL_CALL removeSignatureCreationResultListener(
		const com::sun::star::uno::Reference<
			com::sun::star::xml::crypto::sax::XSignatureCreationResultListener >& listener );

	/* XInitialization */
	virtual void SAL_CALL initialize(
		const com::sun::star::uno::Sequence< com::sun::star::uno::Any >& aArguments );

	/* XServiceInfo */
	virtual rtl::OUString SAL_CALL getImplementationName(  );
	virtual sal_Bool SAL_CALL supportsService( const rtl::OUString& ServiceName );
	virtual com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(  );
};

rtl::OUString SignatureCreatorImpl_getImplementationName();

sal_Bool SAL_CALL SignatureCreatorImpl_supportsService( const rtl::OUString& ServiceName );

com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL SignatureCreatorImpl_getSupportedServiceNames(  );

com::sun::star::uno::Reference< com::sun::star::uno::XInterface >
SAL_CALL SignatureCreatorImpl_createInstance(
	const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory > & rSMgr);

#endif
