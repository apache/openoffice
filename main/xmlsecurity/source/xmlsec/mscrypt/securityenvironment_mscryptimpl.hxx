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



#ifndef _XSECURITYENVIRONMENT_MSCRYPTIMPL_HXX_
#define _XSECURITYENVIRONMENT_MSCRYPTIMPL_HXX_

#ifdef _MSC_VER
#pragma warning(push,1)
#endif
#include <windows.h>
#include <wincrypt.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <sal/config.h>
#include <rtl/ustring.hxx>
#include <cppuhelper/factory.hxx>
#include <cppuhelper/implbase4.hxx>
#include <com/sun/star/uno/Exception.hpp>

#ifndef _COM_SUN_STAR_UNO_REFERENCE_HPP_
#include <com/sun/star/uno/Reference.hxx>
#endif
#include <com/sun/star/lang/XSingleServiceFactory.hpp>

#ifndef _COM_SUN_STAR_LANG_XSECVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/xml/crypto/XSecurityEnvironment.hpp>
#include <com/sun/star/security/XCertificate.hpp>
#include <com/sun/star/security/CertificateCharacters.hpp>
#include <com/sun/star/security/CertificateValidity.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>

#include <list>
#include "xmlsec/xmlsec.h"

#include "sal/types.h"


class SecurityEnvironment_MSCryptImpl : public ::cppu::WeakImplHelper4<
	::com::sun::star::xml::crypto::XSecurityEnvironment ,
	::com::sun::star::lang::XInitialization ,
	::com::sun::star::lang::XServiceInfo ,
	::com::sun::star::lang::XUnoTunnel >
{
	private :
		//cyrpto provider and key container
		HCRYPTPROV							m_hProv ;
		LPCTSTR								m_pszContainer ;

		//Key store
		HCERTSTORE							m_hKeyStore ;

		//Certificate store
		HCERTSTORE							m_hCertStore ;

		// i120675, save the store handles
		HCERTSTORE 							m_hMySystemStore;
		HCERTSTORE 							m_hRootSystemStore;
		HCERTSTORE 							m_hTrustSystemStore;
		HCERTSTORE 							m_hCaSystemStore;

		//Enable default system cryptography setting
		sal_Bool							m_bEnableDefault ;

		//External keys
		std::list< HCRYPTKEY >				m_tSymKeyList ;
		std::list< HCRYPTKEY >				m_tPubKeyList ;
		std::list< HCRYPTKEY >				m_tPriKeyList ;

		//Service manager
		::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xServiceManager ;

	public :
		SecurityEnvironment_MSCryptImpl( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& aFactory ) ;
		virtual ~SecurityEnvironment_MSCryptImpl() ;

		//Methods from XSecurityEnvironment
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > > SAL_CALL getPersonalCertificates() ;

		virtual ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > SAL_CALL getCertificate( const ::rtl::OUString& issuerName, const ::com::sun::star::uno::Sequence< sal_Int8 >& serialNumber ) ;

		virtual ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > SAL_CALL getCertificate( const ::rtl::OUString& issuerName, const ::rtl::OUString& serialNumber ) ;

		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > > SAL_CALL buildCertificatePath( const ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate >& beginCert ) ;

		virtual ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > SAL_CALL createCertificateFromRaw( const ::com::sun::star::uno::Sequence< sal_Int8 >& rawCertificate ) ;

		virtual ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate > SAL_CALL createCertificateFromAscii( const ::rtl::OUString& asciiCertificate ) ;

		virtual ::sal_Int32 SAL_CALL verifyCertificate(
            const ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate >& xCert,
            const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference<
            ::com::sun::star::security::XCertificate > >& intermediateCertificates) ;
	virtual ::sal_Int32 SAL_CALL getCertificateCharacters( const ::com::sun::star::uno::Reference< ::com::sun::star::security::XCertificate >& xCert ) ;

		virtual ::rtl::OUString SAL_CALL getSecurityEnvironmentInformation(  );


		//Methods from XInitialization
		virtual void SAL_CALL initialize(
			const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments
		) ;

		//Methods from XServiceInfo
		virtual ::rtl::OUString SAL_CALL getImplementationName() ;

		virtual sal_Bool SAL_CALL supportsService(
			const ::rtl::OUString& ServiceName
		) ;

		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() ;

		//Helper for XServiceInfo
		static ::com::sun::star::uno::Sequence< ::rtl::OUString > impl_getSupportedServiceNames() ;

		static ::rtl::OUString impl_getImplementationName() ;

		//Helper for registry
		static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL impl_createInstance( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& aServiceManager ) ;

		static ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > impl_createFactory( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& aServiceManager ) ;

		//Methods from XUnoTunnel
		virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );

		static const ::com::sun::star::uno::Sequence< sal_Int8 >& getUnoTunnelId() ;
		static SecurityEnvironment_MSCryptImpl* getImplementation( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > xObj ) ;

		//Native methods
		virtual HCRYPTPROV getCryptoProvider() ;

		virtual void setCryptoProvider( HCRYPTPROV aProv ) ;

		virtual LPCTSTR getKeyContainer() ;

		virtual void setKeyContainer( LPCTSTR aKeyContainer ) ;

		virtual HCERTSTORE getCryptoSlot() ;

		virtual void setCryptoSlot( HCERTSTORE aKeyStore ) ;

		virtual HCERTSTORE getCertDb() ;

		virtual void setCertDb( HCERTSTORE aCertDb ) ;

		virtual void adoptSymKey( HCRYPTKEY aSymKey ) ;

		virtual void rejectSymKey( HCRYPTKEY aSymKey ) ;

		virtual HCRYPTKEY getSymKey( unsigned int position ) ;

		virtual void adoptPubKey( HCRYPTKEY aPubKey ) ;

		virtual void rejectPubKey( HCRYPTKEY aPubKey ) ;

		virtual HCRYPTKEY getPubKey( unsigned int position ) ;

		virtual void adoptPriKey( HCRYPTKEY aPriKey ) ;

		virtual void rejectPriKey( HCRYPTKEY aPriKey ) ;

		virtual HCRYPTKEY getPriKey( unsigned int position ) ;

		virtual void enableDefaultCrypt( sal_Bool enable ) ;

		virtual sal_Bool defaultEnabled() ;

		//Native methods
		virtual xmlSecKeysMngrPtr createKeysManager() ;

		virtual void destroyKeysManager(xmlSecKeysMngrPtr pKeysMngr) ;
} ;

#endif	// _XSECURITYENVIRONMENT_MSCRYPTIMPL_HXX_
