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

/** -- C++ Source File -- **/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_xmlsecurity.hxx"
#include <stdio.h>
#include "helper.hxx"

#include "libxml/tree.h"
#include "libxml/parser.h"
#ifndef XMLSEC_NO_XSLT
#include "libxslt/xslt.h"
#endif

                                                                                
#include "securityenvironment_mscryptimpl.hxx"
#include "xmlelementwrapper_xmlsecimpl.hxx"

#include "nspr.h"
#include "prtypes.h"

#include "pk11func.h"
#include "cert.h"
#include "cryptohi.h"
#include "certdb.h"
#include "nss.h"

#include "xmlsec/strings.h"
#include "xmlsec/xmltree.h"

#include <rtl/ustring.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <cppuhelper/servicefactory.hxx>

#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/xml/wrapper/XXMLElementWrapper.hpp>
#include <com/sun/star/xml/wrapper/XXMLDocumentWrapper.hpp>
#include <com/sun/star/xml/crypto/XXMLEncryption.hpp>
#include <com/sun/star/xml/crypto/XXMLEncryptionTemplate.hpp>
#include <com/sun/star/xml/crypto/XXMLSecurityContext.hpp>
#include <com/sun/star/xml/crypto/XSecurityEnvironment.hpp>


using namespace ::rtl ;
using namespace ::cppu ;
using namespace ::com::sun::star::uno ;
using namespace ::com::sun::star::io ;
using namespace ::com::sun::star::ucb ;
using namespace ::com::sun::star::beans ;
using namespace ::com::sun::star::document ;
using namespace ::com::sun::star::lang ;
using namespace ::com::sun::star::registry ;
using namespace ::com::sun::star::xml::wrapper ;
using namespace ::com::sun::star::xml::crypto ;


int SAL_CALL main( int argc, char **argv )
{
	CERTCertDBHandle*	certHandle = NULL ;
	PK11SlotInfo*		slot = NULL ;
	xmlDocPtr			doc = NULL ;
	xmlNodePtr			tplNode ;
	xmlNodePtr			tarNode ;
	FILE*				dstFile = NULL ;


	if( argc != 5 ) {
		fprintf( stderr, "Usage: %s < CertDir > <input file_url> <output file_url> <rdb file>\n\n" , argv[0] ) ;
		return 1 ;
	}

	//Init libxml and libxslt libraries
	xmlInitParser();
	LIBXML_TEST_VERSION
	xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
	xmlSubstituteEntitiesDefault(1);

	#ifndef XMLSEC_NO_XSLT
	xmlIndentTreeOutput = 1; 
	#endif // XMLSEC_NO_XSLT


	//Initialize NSPR and NSS
	PR_Init( PR_SYSTEM_THREAD, PR_PRIORITY_NORMAL, 1 ) ;
	PK11_SetPasswordFunc( PriPK11PasswordFunc ) ;
	if( NSS_Init( argv[1] ) != SECSuccess ) {
		fprintf( stderr , "### cannot initialize NSS!\n" ) ;
		goto done ;
	}

	certHandle = CERT_GetDefaultCertDB() ;
	slot = PK11_GetInternalKeySlot() ;

	//Load XML document
	doc = xmlParseFile( argv[2] ) ;
	if( doc == NULL || xmlDocGetRootElement( doc ) == NULL ) {
		fprintf( stderr , "### Cannot load template xml document!\n" ) ;
		goto done ;
	}

	//Find the encryption template
	tplNode = xmlSecFindNode( xmlDocGetRootElement( doc ), xmlSecNodeEncryptedData, xmlSecEncNs ) ;
	if( tplNode == NULL ) {
		fprintf( stderr , "### Cannot find the encryption template!\n" ) ;
		goto done ;
	}


	try {
		Reference< XMultiComponentFactory > xManager = NULL ;
		Reference< XComponentContext > xContext = NULL ;

		xManager = serviceManager( xContext , OUString::createFromAscii( "local" ), OUString::createFromAscii( argv[4] ) ) ;

		//Create encryption template
		Reference< XInterface > tplElement =
			xManager->createInstanceWithContext( OUString::createFromAscii( "com.sun.star.xml.xsec.XMLElementWrapper" ) , xContext ) ;
		OSL_ENSURE( tplElement.is() ,
			"Decryptor - "
			"Cannot get service instance of \"xsec.XMLElementWrapper\"" ) ;

		Reference< XXMLElementWrapper > xTplElement( tplElement , UNO_QUERY ) ;
		OSL_ENSURE( xTplElement.is() ,
			"Decryptor - "
			"Cannot get interface of \"XXMLElementWrapper\" from service \"xsec.XMLElementWrapper\"" ) ;

		Reference< XUnoTunnel > xTplEleTunnel( xTplElement , UNO_QUERY ) ;
		OSL_ENSURE( xTplEleTunnel.is() ,
			"Decryptor - "
			"Cannot get interface of \"XUnoTunnel\" from service \"xsec.XMLElementWrapper\"" ) ;

		XMLElementWrapper_XmlSecImpl* pTplElement = ( XMLElementWrapper_XmlSecImpl* )xTplEleTunnel->getSomething( XMLElementWrapper_XmlSecImpl::getUnoTunnelImplementationId() ) ;
		OSL_ENSURE( pTplElement != NULL ,
			"Decryptor - "
			"Cannot get implementation of \"xsec.XMLElementWrapper\"" ) ;

		pTplElement->setNativeElement( tplNode ) ;

		//Build XML Encryption template
		Reference< XInterface > enctpl =
			xManager->createInstanceWithContext( OUString::createFromAscii("com.sun.star.xml.xsec.XMLEncryptionTemplate"), xContext ) ;
		OSL_ENSURE( enctpl.is() ,
			"Decryptor - "
			"Cannot get service instance of \"xsec.XMLEncryptionTemplate\"" ) ;

		Reference< XXMLEncryptionTemplate > xTemplate( enctpl , UNO_QUERY ) ;
		OSL_ENSURE( xTemplate.is() ,
			"Decryptor - "
			"Cannot get interface of \"XXMLEncryptionTemplate\" from service \"xsec.XMLEncryptionTemplate\"" ) ;

		//Import the encryption template
		xTemplate->setTemplate( xTplElement ) ;

		//Create security environment
		//Build Security Environment
		Reference< XInterface > xsecenv =
			xManager->createInstanceWithContext( OUString::createFromAscii("com.sun.star.xml.xsec.SecurityEnvironment"), xContext ) ;
		OSL_ENSURE( xsecenv.is() ,
			"Decryptor - "
			"Cannot get service instance of \"xsec.SecurityEnvironment\"" ) ;

		Reference< XSecurityEnvironment > xSecEnv( xsecenv , UNO_QUERY ) ;
		OSL_ENSURE( xSecEnv.is() ,
			"Decryptor - "
			"Cannot get interface of \"XSecurityEnvironment\" from service \"xsec.SecurityEnvironment\"" ) ;

		//Setup key slot and certDb
		Reference< XUnoTunnel > xEnvTunnel( xsecenv , UNO_QUERY ) ;
		OSL_ENSURE( xEnvTunnel.is() ,
			"Decryptor - "
			"Cannot get interface of \"XUnoTunnel\" from service \"xsec.SecurityEnvironment\"" ) ;

		SecurityEnvironment_XmlSecImpl* pSecEnv = ( SecurityEnvironment_XmlSecImpl* )xEnvTunnel->getSomething( SecurityEnvironment_XmlSecImpl::getUnoTunnelId() ) ;
		OSL_ENSURE( pSecEnv != NULL ,
			"Decryptor - "
			"Cannot get implementation of \"xsec.SecurityEnvironment\"" ) ;

		pSecEnv->setCryptoSlot( slot ) ;
		pSecEnv->setCertDb( certHandle ) ;


		//Build XML Security Context
		Reference< XInterface > xmlsecctx =
			xManager->createInstanceWithContext( OUString::createFromAscii("com.sun.star.xml.xsec.XMLSecurityContext"), xContext ) ;
		OSL_ENSURE( xmlsecctx.is() ,
			"Decryptor - "
			"Cannot get service instance of \"xsec.XMLSecurityContext\"" ) ;

		Reference< XXMLSecurityContext > xSecCtx( xmlsecctx , UNO_QUERY ) ;
		OSL_ENSURE( xSecCtx.is() ,
			"Decryptor - "
			"Cannot get interface of \"XXMLSecurityContext\" from service \"xsec.XMLSecurityContext\"" ) ;

		xSecCtx->setSecurityEnvironment( xSecEnv ) ;


		//Get encrypter
		Reference< XInterface > xmlencrypter =
			xManager->createInstanceWithContext( OUString::createFromAscii("com.sun.star.xml.xsec.XMLEncryption"), xContext ) ;
		OSL_ENSURE( xmlencrypter.is() ,
			"Decryptor - "
			"Cannot get service instance of \"xsec.XMLEncryption\"" ) ;

		Reference< XXMLEncryption > xEncrypter( xmlencrypter , UNO_QUERY ) ;
		OSL_ENSURE( xEncrypter.is() ,
			"Decryptor - "
			"Cannot get interface of \"XXMLEncryption\" from service \"xsec.XMLEncryption\"" ) ;


		//Perform decryption
		Reference< XXMLElementWrapper> xDecrRes = xEncrypter->decrypt( xTemplate , xSecCtx ) ;
		OSL_ENSURE( xDecrRes.is() ,
			"Decryptor - "
			"Cannot decrypt the xml document" ) ;
	} catch( Exception& e ) {
		fprintf( stderr , "Error Message: %s\n" , OUStringToOString( e.Message , RTL_TEXTENCODING_ASCII_US ).getStr() ) ;
		goto done ;
	}

	dstFile = fopen( argv[3], "w" ) ;
	if( dstFile == NULL ) {
		fprintf( stderr , "### Can not open file %s\n", argv[3] ) ;
		goto done ;
	}

	//Save result
	xmlDocDump( dstFile, doc ) ;

done:
	if( dstFile != NULL )
		fclose( dstFile ) ;

	if( slot != NULL )
		PK11_FreeSlot( slot ) ;

	PK11_LogoutAll() ;
	NSS_Shutdown() ;

	/* Shutdown libxslt/libxml */
	#ifndef XMLSEC_NO_XSLT
	xsltCleanupGlobals();            
	#endif /* XMLSEC_NO_XSLT */
	xmlCleanupParser();

	return 0;
}

