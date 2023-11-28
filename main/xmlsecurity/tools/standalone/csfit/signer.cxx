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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_xmlsecurity.hxx"

#include <stdio.h>
#include "helper.hxx"

#include "libxml/tree.h"
#include "libxml/parser.h"
#ifndef XMLSEC_NO_XSLT
#include "libxslt/xslt.h"
#endif

#include "securityenvironment_nssimpl.hxx"
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

#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/xml/wrapper/XXMLElementWrapper.hpp>
#include <com/sun/star/xml/wrapper/XXMLDocumentWrapper.hpp>
#include <com/sun/star/xml/crypto/XXMLSignature.hpp>
#include <com/sun/star/xml/crypto/XXMLSignatureTemplate.hpp>
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
using namespace ::com::sun::star::xml::wrapper ;
using namespace ::com::sun::star::xml::crypto ;


int SAL_CALL main( int argc, char **argv )
{
	CERTCertDBHandle*	certHandle ;
	PK11SlotInfo*		slot ;
	xmlDocPtr			doc ;
	xmlNodePtr			tplNode ;
	xmlNodePtr			tarNode ;
	xmlAttrPtr			idAttr ;
	xmlChar*			idValue ;
	xmlAttrPtr			uriAttr ;
	xmlChar*			uriValue ;
	OUString*			uri ;
	Reference< XUriBinding >	xUriBinding ;
	FILE*				dstFile ;

	if( argc != 5 ) {
		fprintf( stderr, "Usage: %s < CertDir > <file_url of template> <file_url of result> <rdb file>\n\n" , argv[0] ) ;
		return 1 ;
	}

	for( int hhh = 0 ; hhh < 10 ; hhh ++ ) {
		slot = NULL ;
		doc = NULL ;
		uri = NULL ;
		dstFile = NULL ;


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

	if( PK11_NeedLogin( slot ) ) {
		SECStatus nRet = PK11_Authenticate( slot, PR_TRUE, NULL );
		if( nRet != SECSuccess ) {
			fprintf( stderr , "### cannot authehticate the crypto token!\n" ) ;
			goto done ;
		}
	}

	//Load XML document
	doc = xmlParseFile( argv[2] ) ;
	if( doc == NULL || xmlDocGetRootElement( doc ) == NULL ) {
		fprintf( stderr , "### Cannot load template xml document!\n" ) ;
		goto done ;
	}

	//Find the signature template
	tplNode = xmlSecFindNode( xmlDocGetRootElement( doc ), xmlSecNodeSignature, xmlSecDSigNs ) ;
	if( tplNode == NULL ) {
		fprintf( stderr , "### Cannot find the signature template!\n" ) ;
		goto done ;
	}

	//Find the element with ID attribute
	//Here we only try to find the "document" node.
	tarNode = xmlSecFindNode( xmlDocGetRootElement( doc ), ( xmlChar* )"document", ( xmlChar* )"http://openoffice.org/2000/office" ) ;
	if( tarNode == NULL ) {
		tarNode = xmlSecFindNode( xmlDocGetRootElement( doc ), ( xmlChar* )"document", NULL ) ;
	}

	//Find the "id" attrbute in the element
	if( tarNode != NULL ) {
		if( ( idAttr = xmlHasProp( tarNode, ( xmlChar* )"id" ) ) != NULL ) {
			//NULL
		} else if( ( idAttr = xmlHasProp( tarNode, ( xmlChar* )"Id" ) ) != NULL ) {
			//NULL
		} else {
			idAttr = NULL ;
		}
	}

	//Add ID to DOM
	if( idAttr != NULL ) {
		idValue = xmlNodeListGetString( tarNode->doc, idAttr->children, 1 ) ;
		if( idValue == NULL ) {
			fprintf( stderr , "### the ID value is NULL!\n" ) ;
			goto done ;
		}

		if( xmlAddID( NULL, doc, idValue, idAttr ) == NULL ) {
			fprintf( stderr , "### Can not add the ID value!\n" ) ;
			goto done ;
		}
	}

	//Reference handler
	//Find the signature reference
	tarNode = xmlSecFindNode( tplNode, xmlSecNodeReference, xmlSecDSigNs ) ;
	if( tarNode == NULL ) {
		fprintf( stderr , "### Cannot find the signature reference!\n" ) ;
		goto done ;
	}

	//Find the "URI" attrbute in the reference
	uriAttr = xmlHasProp( tarNode, ( xmlChar* )"URI" ) ;
	if( tarNode == NULL ) {
		fprintf( stderr , "### Cannot find URI of the reference!\n" ) ;
		goto done ;
	}

	//Get the "URI" attrbute value
	uriValue = xmlNodeListGetString( tarNode->doc, uriAttr->children, 1 ) ;
	if( uriValue == NULL ) {
		fprintf( stderr , "### the URI value is NULL!\n" ) ;
		goto done ;
	}

	if( strchr( ( const char* )uriValue, '/' ) != NULL && strchr( ( const char* )uriValue, '#' ) == NULL ) {
		fprintf( stdout , "### Find a stream URI [%s]\n", uriValue ) ;
	//	uri = new ::rtl::OUString( ( const sal_Unicode* )uriValue ) ;
		uri = new ::rtl::OUString( ( const sal_Char* )uriValue, xmlStrlen( uriValue ), RTL_TEXTENCODING_ASCII_US ) ;
	}

	if( uri != NULL ) {
		fprintf( stdout , "### Find the URI [%s]\n", OUStringToOString( *uri , RTL_TEXTENCODING_ASCII_US ).getStr() ) ;
		Reference< XInputStream > xStream = createStreamFromFile( *uri ) ;
		if( !xStream.is() ) {
			fprintf( stderr , "### Can not get the URI stream!\n" ) ;
			goto done ;
		}

		xUriBinding = new OUriBinding( *uri, xStream ) ;
	}

	try {
		Reference< XMultiComponentFactory > xManager = NULL ;
		Reference< XComponentContext > xContext = NULL ;

		xManager = serviceManager( xContext , OUString::createFromAscii( "local" ), OUString::createFromAscii( argv[4] ) ) ;
		OSL_ENSURE( xManager.is() ,
			"ServicesManager - "
			"Cannot get service manager" ) ;

		//Create signature template
		Reference< XInterface > element =
			xManager->createInstanceWithContext( OUString::createFromAscii( "com.sun.star.xml.security.bridge.xmlsec.XMLElementWrapper_XmlSecImpl" ) , xContext ) ;
		OSL_ENSURE( element.is() ,
			"Signer - "
			"Cannot get service instance of \"wrapper.XMLElementWrapper\"" ) ;

		Reference< XXMLElementWrapper > xElement( element , UNO_QUERY ) ;
		OSL_ENSURE( xElement.is() ,
			"Signer - "
			"Cannot get interface of \"XXMLElement\" from service \"xsec.XMLElement\"" ) ;

		Reference< XUnoTunnel > xEleTunnel( xElement , UNO_QUERY ) ;
		OSL_ENSURE( xEleTunnel.is() ,
			"Signer - "
			"Cannot get interface of \"XUnoTunnel\" from service \"xsec.XMLElement\"" ) ;

		XMLElementWrapper_XmlSecImpl* pElement = ( XMLElementWrapper_XmlSecImpl* )xEleTunnel->getSomething( XMLElementWrapper_XmlSecImpl::getUnoTunnelImplementationId() ) ;
		OSL_ENSURE( pElement != NULL ,
			"Signer - "
			"Cannot get implementation of \"xsec.XMLElement\"" ) ;

		//Set signature template
		pElement->setNativeElement( tplNode ) ;

		//Build XML Signature template
		Reference< XInterface > signtpl =
			xManager->createInstanceWithContext( OUString::createFromAscii( "com.sun.star.xml.crypto.XMLSignatureTemplate" ) , xContext ) ;
		OSL_ENSURE( signtpl.is() ,
			"Signer - "
			"Cannot get service instance of \"xsec.XMLSignatureTemplate\"" ) ;

		Reference< XXMLSignatureTemplate > xTemplate( signtpl , UNO_QUERY ) ;
		OSL_ENSURE( xTemplate.is() ,
			"Signer - "
			"Cannot get interface of \"XXMLSignatureTemplate\" from service \"xsec.XMLSignatureTemplate\"" ) ;

		//Import the signature template
		xTemplate->setTemplate( xElement ) ;

		//Import the URI/Stream binding
		if( xUriBinding.is() )
			xTemplate->setBinding( xUriBinding ) ;

		//Create security environment
		//Build Security Environment
		Reference< XInterface > xsecenv =
			xManager->createInstanceWithContext( OUString::createFromAscii("com.sun.star.xml.security.bridge.xmlsec.SecurityEnvironment_NssImpl"), xContext ) ;
		OSL_ENSURE( xsecenv.is() ,
			"Signer - "
			"Cannot get service instance of \"xsec.SecurityEnvironment\"" ) ;

		Reference< XSecurityEnvironment > xSecEnv( xsecenv , UNO_QUERY ) ;
		OSL_ENSURE( xSecEnv.is() ,
			"Signer - "
			"Cannot get interface of \"XSecurityEnvironment\" from service \"xsec.SecurityEnvironment\"" ) ;

		//Setup key slot and certDb
		Reference< XUnoTunnel > xEnvTunnel( xsecenv , UNO_QUERY ) ;
		OSL_ENSURE( xElement.is() ,
			"Signer - "
			"Cannot get interface of \"XUnoTunnel\" from service \"xsec.SecurityEnvironment\"" ) ;

		SecurityEnvironment_NssImpl* pSecEnv = ( SecurityEnvironment_NssImpl* )xEnvTunnel->getSomething( SecurityEnvironment_NssImpl::getUnoTunnelId() ) ;
		OSL_ENSURE( pSecEnv != NULL ,
			"Signer - "
			"Cannot get implementation of \"xsec.SecurityEnvironment\"" ) ;

		pSecEnv->setCryptoSlot( slot ) ;
		pSecEnv->setCertDb( certHandle ) ;

		//Build XML Security Context
		Reference< XInterface > xmlsecctx =
			xManager->createInstanceWithContext( OUString::createFromAscii("com.sun.star.xml.security.bridge.xmlsec.XMLSecurityContext_NssImpl"), xContext ) ;
		OSL_ENSURE( xsecenv.is() ,
			"Signer - "
			"Cannot get service instance of \"xsec.XMLSecurityContext\"" ) ;

		Reference< XXMLSecurityContext > xSecCtx( xmlsecctx , UNO_QUERY ) ;
		OSL_ENSURE( xSecCtx.is() ,
			"Signer - "
			"Cannot get interface of \"XXMLSecurityContext\" from service \"xsec.XMLSecurityContext\"" ) ;

		xSecCtx->setSecurityEnvironment( xSecEnv ) ;

		//Generate XML signature
		Reference< XInterface > xmlsigner =
			xManager->createInstanceWithContext( OUString::createFromAscii("com.sun.star.xml.security.bridge.xmlsec.XMLSignature_NssImpl"), xContext ) ;
		OSL_ENSURE( xmlsigner.is() ,
			"Signer - "
			"Cannot get service instance of \"xsec.XMLSignature\"" ) ;

		Reference< XXMLSignature > xSigner( xmlsigner , UNO_QUERY ) ;
		OSL_ENSURE( xSigner.is() ,
			"Signer - "
			"Cannot get interface of \"XXMLSignature\" from service \"xsec.XMLSignature\"" ) ;

		//perform signature
		xTemplate = xSigner->generate( xTemplate , xSecCtx ) ;
		OSL_ENSURE( xTemplate.is() ,
			"Signer - "
			"Cannot generate the xml signature" ) ;
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
	if( uri != NULL )
		delete uri ;

	if( dstFile != NULL )
		fclose( dstFile ) ;

	if( doc != NULL )
		xmlFreeDoc( doc ) ;

	if( slot != NULL )
		PK11_FreeSlot( slot ) ;

	PK11_LogoutAll() ;
	NSS_Shutdown() ;

	/* Shutdown libxslt/libxml */
	#ifndef XMLSEC_NO_XSLT
	xsltCleanupGlobals();            
	#endif /* XMLSEC_NO_XSLT */
	xmlCleanupParser();

	}

	return 0;
}

