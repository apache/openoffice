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

#include <xsecctl.hxx>
#include "xsecparser.hxx"
#include <tools/debug.hxx>

#include <com/sun/star/xml/crypto/sax/XKeyCollector.hpp>
#include <com/sun/star/xml/crypto/sax/ElementMarkPriority.hpp>
#include <com/sun/star/xml/crypto/sax/XReferenceResolvedBroadcaster.hpp>
#include <com/sun/star/xml/crypto/sax/XReferenceCollector.hpp>
#include <com/sun/star/xml/crypto/sax/XSignatureVerifyResultBroadcaster.hpp>
#include <com/sun/star/xml/sax/SAXParseException.hpp>

// FIXME: copied from main/sal/rtl/source/strimp.c
static sal_Bool rtl_ImplIsWhitespace( sal_Unicode c )
{
	/* Space or Control character? */
	if ( (c <= 32) && c )
		return sal_True;

	/* Only in the General Punctuation area Space or Control characters are included? */
	if ( (c < 0x2000) || (c > 0x206F) )
		return sal_False;

	if ( ((c >= 0x2000) && (c <= 0x200B)) ||	/* All Spaces			*/
		 (c == 0x2028) ||						/* LINE SEPARATOR		*/
		 (c == 0x2029) )						/* PARAGRAPH SEPARATOR	*/
		return sal_True;

	return sal_False;
}

namespace cssu = com::sun::star::uno;
namespace cssl = com::sun::star::lang;
namespace cssxc = com::sun::star::xml::crypto;
namespace cssxs = com::sun::star::xml::sax;

/* xml security framework components */
#define SIGNATUREVERIFIER_COMPONENT "com.sun.star.xml.crypto.sax.SignatureVerifier"

/* protected: for signature verify */
cssu::Reference< cssxc::sax::XReferenceResolvedListener > XSecController::prepareSignatureToRead(
	sal_Int32 nSecurityId)
{
	if ( m_nStatusOfSecurityComponents != INITIALIZED )
	{
		return NULL;
	}

	sal_Int32 nIdOfSignatureElementCollector;
	cssu::Reference< cssxc::sax::XReferenceResolvedListener > xReferenceResolvedListener;

	nIdOfSignatureElementCollector = 
		m_xSAXEventKeeper->addSecurityElementCollector( cssxc::sax::ElementMarkPriority_BEFOREMODIFY, sal_False);

	m_xSAXEventKeeper->setSecurityId(nIdOfSignatureElementCollector, nSecurityId);
        
        /*
         * create a SignatureVerifier
         */
	cssu::Reference< cssl::XMultiComponentFactory > xMCF( mxCtx->getServiceManager() );
	xReferenceResolvedListener = cssu::Reference< cssxc::sax::XReferenceResolvedListener >(
		xMCF->createInstanceWithContext(
			rtl::OUString::createFromAscii( SIGNATUREVERIFIER_COMPONENT ), mxCtx),
		cssu::UNO_QUERY);
	
	cssu::Reference<cssl::XInitialization> xInitialization(xReferenceResolvedListener, cssu::UNO_QUERY);
	
	cssu::Sequence<cssu::Any> args(5);
	args[0] = cssu::makeAny(rtl::OUString::valueOf(nSecurityId));
	args[1] = cssu::makeAny(m_xSAXEventKeeper);
	args[2] = cssu::makeAny(rtl::OUString::valueOf(nIdOfSignatureElementCollector));
	args[3] = cssu::makeAny(m_xSecurityContext);
	args[4] = cssu::makeAny(m_xXMLSignature);
	xInitialization->initialize(args);
	
	cssu::Reference< cssxc::sax::XSignatureVerifyResultBroadcaster >
		signatureVerifyResultBroadcaster(xReferenceResolvedListener, cssu::UNO_QUERY);
		
	signatureVerifyResultBroadcaster->addSignatureVerifyResultListener( this );

	cssu::Reference<cssxc::sax::XReferenceResolvedBroadcaster> xReferenceResolvedBroadcaster  
		(m_xSAXEventKeeper,
		cssu::UNO_QUERY);
		
	xReferenceResolvedBroadcaster->addReferenceResolvedListener(
		nIdOfSignatureElementCollector,
		xReferenceResolvedListener);
		
	cssu::Reference<cssxc::sax::XKeyCollector> keyCollector (xReferenceResolvedListener, cssu::UNO_QUERY);
	keyCollector->setKeyId(0);
	
	return xReferenceResolvedListener;
}

void XSecController::addSignature()
{
	cssu::Reference< cssxc::sax::XReferenceResolvedListener > xReferenceResolvedListener = NULL;
	sal_Int32 nSignatureId = 0;
	
	
	if (m_bVerifyCurrentSignature)
	{
		chainOn(true);
		xReferenceResolvedListener = prepareSignatureToRead( m_nReservedSignatureId );
		m_bVerifyCurrentSignature = false;
		nSignatureId = m_nReservedSignatureId;
	}
	
	InternalSignatureInformation isi( nSignatureId, xReferenceResolvedListener );
	m_vInternalSignatureInformations.push_back( isi );
}

void XSecController::addReference( const rtl::OUString& ouUri)
{
	InternalSignatureInformation &isi = m_vInternalSignatureInformations[m_vInternalSignatureInformations.size()-1];
	isi.addReference(TYPE_SAMEDOCUMENT_REFERENCE,ouUri, -1 );
}

void XSecController::addStreamReference( 
	const rtl::OUString& ouUri,
	bool isBinary )
{
        sal_Int32 type = (isBinary?TYPE_BINARYSTREAM_REFERENCE:TYPE_XMLSTREAM_REFERENCE);

	InternalSignatureInformation &isi = m_vInternalSignatureInformations[m_vInternalSignatureInformations.size()-1];

	if ( isi.xReferenceResolvedListener.is() )
	{
	        /*
	         * get the input stream
	         */
        	cssu::Reference< com::sun::star::io::XInputStream > xObjectInputStream
        		= getObjectInputStream( ouUri );
		
		if ( xObjectInputStream.is() )
		{
			cssu::Reference<cssxc::XUriBinding> xUriBinding
				(isi.xReferenceResolvedListener, cssu::UNO_QUERY);
			xUriBinding->setUriBinding(ouUri, xObjectInputStream);
		}
	}
	
	isi.addReference(type, ouUri, -1);
}

void XSecController::setReferenceCount() const
{
	const InternalSignatureInformation &isi = m_vInternalSignatureInformations[m_vInternalSignatureInformations.size()-1];
	
	if ( isi.xReferenceResolvedListener.is() )
	{
		const SignatureReferenceInformations &refInfors = isi.signatureInfor.vSignatureReferenceInfors;
		
		int refNum = refInfors.size();
		sal_Int32 referenceCount = 0;
			
		for(int i=0 ; i<refNum; ++i)
		{
			if (refInfors[i].nType == TYPE_SAMEDOCUMENT_REFERENCE )
			/*
			 * same-document reference
			 */
			{
				referenceCount++;
			}
		}
		
		cssu::Reference<cssxc::sax::XReferenceCollector> xReferenceCollector
			(isi.xReferenceResolvedListener, cssu::UNO_QUERY);
		xReferenceCollector->setReferenceCount( referenceCount );
	}
}

void XSecController::setIfEmpty(rtl::OUString &variable, const rtl::OUString &value) {
    if (variable.getLength() == 0) {
        variable = value;
    } else if (variable != value) {
        throw cssu::RuntimeException(rtl::OUString::createFromAscii("Value already set. Tampering?"), *this);
    }
}

void XSecController::setX509IssuerName( rtl::OUString& ouX509IssuerName )
{
	InternalSignatureInformation &isi = m_vInternalSignatureInformations[m_vInternalSignatureInformations.size()-1];
	setIfEmpty(isi.signatureInfor.ouX509IssuerName, ouX509IssuerName);
}

void XSecController::setX509SerialNumber( rtl::OUString& ouX509SerialNumber )
{
	InternalSignatureInformation &isi = m_vInternalSignatureInformations[m_vInternalSignatureInformations.size()-1];
	setIfEmpty(isi.signatureInfor.ouX509SerialNumber, ouX509SerialNumber);
}

void XSecController::setX509Certificate( rtl::OUString& ouX509Certificate )
{
	InternalSignatureInformation &isi = m_vInternalSignatureInformations[m_vInternalSignatureInformations.size()-1];
    if (isi.signatureInfor.ouX509Certificate.getLength()) {
        // We allow to re-set the same certificate only.
        // Whitespace may change.
        const sal_Int32 l1 = isi.signatureInfor.ouX509Certificate.getLength();
        const sal_Int32 l2 = ouX509Certificate.getLength();
        const sal_Unicode *s1 = isi.signatureInfor.ouX509Certificate.getStr();
        const sal_Unicode *s2 = ouX509Certificate.getStr();
        sal_Int32 i1 = 0, i2 = 0;
        while ((i1 < l1) && (i2 < l2)) {
            const sal_Unicode &c1 = s1[i1];
            const sal_Unicode &c2 = s2[i2];
            if (rtl_ImplIsWhitespace(c1)) {
                ++i1;
                continue;
            }
            if (rtl_ImplIsWhitespace(c2)) {
                ++i2;
                continue;
            }
            if (c1 != c2) {
                throw cssu::RuntimeException(rtl::OUString::createFromAscii("Value already set. Tampering?"), *this);
            }
            ++i1;
            ++i2;
        }
        // We could still have whitespace at the end of both strings
        while ((i1 < l1) && rtl_ImplIsWhitespace(s1[l1])) {
            ++i1;
        }
        while ((i2 < l2) && rtl_ImplIsWhitespace(s2[l2])) {
            ++i2;
        }
        if ((i1 != l1) || (i2 != l2)) {
            throw cssu::RuntimeException(rtl::OUString::createFromAscii("Value already set. Tampering?"), *this);
        }
    }
    isi.signatureInfor.ouX509Certificate = ouX509Certificate;
}

void XSecController::setSignatureValue( rtl::OUString& ouSignatureValue )
{
	InternalSignatureInformation &isi = m_vInternalSignatureInformations[m_vInternalSignatureInformations.size()-1];
	isi.signatureInfor.ouSignatureValue = ouSignatureValue;
}

void XSecController::setDigestValue( rtl::OUString& ouDigestValue )
{
	SignatureInformation &si = m_vInternalSignatureInformations[m_vInternalSignatureInformations.size()-1].signatureInfor;
	SignatureReferenceInformation &reference = si.vSignatureReferenceInfors[si.vSignatureReferenceInfors.size()-1];
	reference.ouDigestValue = ouDigestValue;
}

void XSecController::setDate( rtl::OUString& ouDate )
{
	InternalSignatureInformation &isi = m_vInternalSignatureInformations[m_vInternalSignatureInformations.size()-1];
	convertDateTime( isi.signatureInfor.stDateTime, ouDate );
	setIfEmpty(isi.signatureInfor.ouDateTime, ouDate);
}

/*
void XSecController::setTime( rtl::OUString& ouTime )
{
	InternalSignatureInformation &isi = m_vInternalSignatureInformations[m_vInternalSignatureInformations.size()-1];
	isi.signatureInfor.ouTime = ouTime;
}
*/

void XSecController::setId( rtl::OUString& ouId )
{
	InternalSignatureInformation &isi = m_vInternalSignatureInformations[m_vInternalSignatureInformations.size()-1];
	isi.signatureInfor.ouSignatureId = ouId;
}

void XSecController::setPropertyId( rtl::OUString& ouPropertyId )
{
	InternalSignatureInformation &isi = m_vInternalSignatureInformations[m_vInternalSignatureInformations.size()-1];
	isi.signatureInfor.ouPropertyId = ouPropertyId;
}

/* public: for signature verify */
void XSecController::collectToVerify( const rtl::OUString& referenceId )
{
	/* DBG_ASSERT( m_xSAXEventKeeper.is(), "the SAXEventKeeper is NULL" ); */
	
	if ( m_nStatusOfSecurityComponents == INITIALIZED )
	/*
	 * if all security components are ready, verify the signature.
	 */
	{
		bool bJustChainingOn = false;
		cssu::Reference< cssxs::XDocumentHandler > xHandler = NULL;
		
		int i,j;
		int sigNum = m_vInternalSignatureInformations.size();
		
		for (i=0; i<sigNum; ++i)
		{
			InternalSignatureInformation& isi = m_vInternalSignatureInformations[i];
			SignatureReferenceInformations& vReferenceInfors = isi.signatureInfor.vSignatureReferenceInfors;
			int refNum = vReferenceInfors.size();
			
			for (j=0; j<refNum; ++j)
			{
				SignatureReferenceInformation &refInfor = vReferenceInfors[j];
				
				if (refInfor.ouURI == referenceId)
				{
					if (chainOn(false))
					{
						bJustChainingOn = true;
						xHandler = m_xSAXEventKeeper->setNextHandler(NULL);
					}
					
					sal_Int32 nKeeperId = m_xSAXEventKeeper->addSecurityElementCollector(
						cssxc::sax::ElementMarkPriority_BEFOREMODIFY, sal_False );
	
					cssu::Reference<cssxc::sax::XReferenceResolvedBroadcaster> xReferenceResolvedBroadcaster  
						(m_xSAXEventKeeper,
						cssu::UNO_QUERY );
						
					cssu::Reference<cssxc::sax::XReferenceCollector> xReferenceCollector
						( isi.xReferenceResolvedListener, cssu::UNO_QUERY );
					
					m_xSAXEventKeeper->setSecurityId(nKeeperId, isi.signatureInfor.nSecurityId);
					xReferenceResolvedBroadcaster->addReferenceResolvedListener( nKeeperId, isi.xReferenceResolvedListener);
					xReferenceCollector->setReferenceId( nKeeperId );
					
					isi.vKeeperIds[j] = nKeeperId;
					break;
				}
			}
		}
	
		if ( bJustChainingOn )
		{
			cssu::Reference< cssxs::XDocumentHandler > xSEKHandler(m_xSAXEventKeeper, cssu::UNO_QUERY);
			if (m_xElementStackKeeper.is())
			{
				m_xElementStackKeeper->retrieve(xSEKHandler, sal_True);
			}
			m_xSAXEventKeeper->setNextHandler(xHandler);
		}
	}
}

void XSecController::addSignature( sal_Int32 nSignatureId )
{
	DBG_ASSERT( m_pXSecParser != NULL, "No XSecParser initialized" );
	
	m_nReservedSignatureId = nSignatureId;
	m_bVerifyCurrentSignature = true;
}

cssu::Reference< cssxs::XDocumentHandler > XSecController::createSignatureReader()
{
	m_pXSecParser = new XSecParser( this, NULL );
	cssu::Reference< cssl::XInitialization > xInitialization = m_pXSecParser;
	
	setSAXChainConnector(xInitialization, NULL, NULL);
	
	return m_pXSecParser;
}
	
void XSecController::releaseSignatureReader()
{
	clearSAXChainConnector( );
	m_pXSecParser = NULL;
}

