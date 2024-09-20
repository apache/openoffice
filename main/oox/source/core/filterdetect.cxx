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



#include "oox/core/filterdetect.hxx"
#include "oox/core/encryption.hxx"

#include <com/sun/star/io/XStream.hpp>
#include <com/sun/star/logging/LogLevel.hpp>
#include <comphelper/docpasswordhelper.hxx>
#include <comphelper/mediadescriptor.hxx>
#include "oox/core/fastparser.hxx"
#include "oox/core/relationshandler.hxx"
#include "oox/helper/attributelist.hxx"
#include "oox/helper/binaryinputstream.hxx"
#include "oox/helper/binaryoutputstream.hxx"
#include "oox/helper/zipstorage.hxx"
#include "oox/ole/olestorage.hxx"

namespace oox {
namespace core {

// ============================================================================

using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::logging;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;

using ::comphelper::MediaDescriptor;
using ::comphelper::SequenceAsHashMap;
using ::rtl::OUString;

// ============================================================================

FilterDetectDocHandler::FilterDetectDocHandler( OUString& rFilterName ) :
    mrFilterName( rFilterName )
{
    maContextStack.reserve( 2 );
}

FilterDetectDocHandler::~FilterDetectDocHandler()
{
}

void SAL_CALL FilterDetectDocHandler::startDocument()
    throw (SAXException, RuntimeException)
{
}

void SAL_CALL FilterDetectDocHandler::endDocument()
    throw (SAXException, RuntimeException)
{
}

void SAL_CALL FilterDetectDocHandler::setDocumentLocator( const Reference<XLocator>& /*xLocator*/ )
    throw (SAXException, RuntimeException)
{
}

void SAL_CALL FilterDetectDocHandler::startFastElement(
        sal_Int32 nElement, const Reference< XFastAttributeList >& rAttribs )
    throw (SAXException,RuntimeException)
{
    AttributeList aAttribs( rAttribs );
    switch ( nElement )
    {
        // cases for _rels/.rels
        case PR_TOKEN( Relationships ):
        break;
        case PR_TOKEN( Relationship ):
            if( !maContextStack.empty() && (maContextStack.back() == PR_TOKEN( Relationships )) )
                parseRelationship( aAttribs );
        break;

        // cases for [Content_Types].xml
        case PC_TOKEN( Types ):
        break;
        case PC_TOKEN( Default ):
            if( !maContextStack.empty() && (maContextStack.back() == PC_TOKEN( Types )) )
                parseContentTypesDefault( aAttribs );
        break;
        case PC_TOKEN( Override ):
            if( !maContextStack.empty() && (maContextStack.back() == PC_TOKEN( Types )) )
                parseContentTypesOverride( aAttribs );
        break;
    }
    maContextStack.push_back( nElement );
}

void SAL_CALL FilterDetectDocHandler::startUnknownElement(
    const OUString& /*Namespace*/, const OUString& /*Name*/, const Reference<XFastAttributeList>& /*Attribs*/ )
    throw (SAXException, RuntimeException)
{
}

void SAL_CALL FilterDetectDocHandler::endFastElement( sal_Int32 /*nElement*/ )
    throw (SAXException, RuntimeException)
{
    maContextStack.pop_back();
}

void SAL_CALL FilterDetectDocHandler::endUnknownElement(
    const OUString& /*Namespace*/, const OUString& /*Name*/ ) throw (SAXException, RuntimeException)
{
}

Reference<XFastContextHandler> SAL_CALL FilterDetectDocHandler::createFastChildContext(
    sal_Int32 /*Element*/, const Reference<XFastAttributeList>& /*Attribs*/ )
    throw (SAXException, RuntimeException)
{
    return this;
}

Reference<XFastContextHandler> SAL_CALL FilterDetectDocHandler::createUnknownChildContext(
    const OUString& /*Namespace*/, const OUString& /*Name*/, const Reference<XFastAttributeList>& /*Attribs*/)
    throw (SAXException, RuntimeException)
{
    return this;
}

void SAL_CALL FilterDetectDocHandler::characters( const OUString& /*aChars*/ )
    throw (SAXException, RuntimeException)
{
}

void SAL_CALL FilterDetectDocHandler::ignorableWhitespace( const OUString& /*aWhitespaces*/ )
    throw (SAXException, RuntimeException)
{
}

void SAL_CALL FilterDetectDocHandler::processingInstruction(
    const OUString& /*aTarget*/, const OUString& /*aData*/ )
    throw (SAXException, RuntimeException)
{
}

void FilterDetectDocHandler::parseRelationship( const AttributeList& rAttribs )
{
    OUString aType = rAttribs.getString( XML_Type, OUString() );
    if( aType.equalsAscii( "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" ) )
        maTargetPath = RelationsFragment::removeDuplicateSlashes( OUString( sal_Unicode( '/' ) ) + rAttribs.getString( XML_Target, OUString() ) );
}

OUString FilterDetectDocHandler::getFilterNameFromContentType( const OUString& rContentType ) const
{
    if( rContentType.equalsAscii( "application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml" ) ||
        rContentType.equalsAscii( "application/vnd.ms-word.document.macroEnabled.main+xml" ) )
        return CREATE_OUSTRING( "writer_MS_Word_2007" );

    if( rContentType.equalsAscii( "application/vnd.openxmlformats-officedocument.wordprocessingml.template.main+xml" ) ||
        rContentType.equalsAscii( "application/vnd.ms-word.template.macroEnabledTemplate.main+xml" ) )
        return CREATE_OUSTRING( "writer_MS_Word_2007_Template" );

    if( rContentType.equalsAscii( "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml" ) ||
        rContentType.equalsAscii( "application/vnd.ms-excel.sheet.macroEnabled.main+xml" ) )
        return CREATE_OUSTRING( "MS Excel 2007 XML" );

    if( rContentType.equalsAscii( "application/vnd.openxmlformats-officedocument.spreadsheetml.template.main+xml" ) ||
        rContentType.equalsAscii( "application/vnd.ms-excel.template.macroEnabled.main+xml" ) )
        return CREATE_OUSTRING( "MS Excel 2007 XML Template" );

    if( rContentType.equalsAscii( "application/vnd.ms-excel.sheet.binary.macroEnabled.main" ) )
        return CREATE_OUSTRING( "MS Excel 2007 Binary" );

    if( rContentType.equalsAscii( "application/vnd.openxmlformats-officedocument.presentationml.presentation.main+xml" ) ||
        rContentType.equalsAscii( "application/vnd.ms-powerpoint.presentation.macroEnabled.main+xml" ) )
        return CREATE_OUSTRING( "MS PowerPoint 2007 XML" );

    if( rContentType.equalsAscii( "application/vnd.openxmlformats-officedocument.presentationml.template.main+xml" ) ||
        rContentType.equalsAscii( "application/vnd.ms-powerpoint.template.macroEnabled.main+xml" ) )
        return CREATE_OUSTRING( "MS PowerPoint 2007 XML Template" );

    return OUString();
}

void FilterDetectDocHandler::parseContentTypesDefault( const AttributeList& rAttribs )
{
    // only if no overridden part name found
    if( mrFilterName.getLength() == 0 )
    {
        // check if target path ends with extension
        OUString aExtension = rAttribs.getString( XML_Extension, OUString() );
        sal_Int32 nExtPos = maTargetPath.getLength() - aExtension.getLength();
        if( (nExtPos > 0) && (maTargetPath[ nExtPos - 1 ] == '.') && maTargetPath.match( aExtension, nExtPos ) )
            mrFilterName = getFilterNameFromContentType( rAttribs.getString( XML_ContentType, OUString() ) );
    }
}

void FilterDetectDocHandler::parseContentTypesOverride( const AttributeList& rAttribs )
{
    if( rAttribs.getString( XML_PartName, OUString() ).equals( maTargetPath ) )
        mrFilterName = getFilterNameFromContentType( rAttribs.getString( XML_ContentType, OUString() ) );
}

// ============================================================================

/* Helper for XServiceInfo */
Sequence< OUString > FilterDetect_getSupportedServiceNames()
{
    Sequence< OUString > aServiceNames( 1 );
    aServiceNames[ 0 ] = CREATE_OUSTRING( "com.sun.star.frame.ExtendedTypeDetection" );
    return aServiceNames;
}

/* Helper for XServiceInfo */
OUString FilterDetect_getImplementationName()
{
    return CREATE_OUSTRING( "com.sun.star.comp.oox.FormatDetector" );
}

/* Helper for registry */
Reference< XInterface > SAL_CALL FilterDetect_createInstance( const Reference< XComponentContext >& rxContext ) throw( Exception )
{
    return static_cast< ::cppu::OWeakObject* >( new FilterDetect( rxContext ) );
}

// ----------------------------------------------------------------------------

FilterDetect::FilterDetect( const Reference< XComponentContext >& rxContext ) throw( RuntimeException ) :
    mxContext( rxContext, UNO_SET_THROW ),
    logger( rxContext )
{
}

FilterDetect::~FilterDetect()
{
}

namespace {

// ----------------------------------------------------------------------------

bool lclIsZipPackage( const Reference< XComponentContext >& rxContext, const Reference< XInputStream >& rxInStrm )
{
    ZipStorage aZipStorage( rxContext, rxInStrm );
    return aZipStorage.isStorage();
}

// the password verifier ------------------------------------------------------

class PasswordVerifier : public ::comphelper::IDocPasswordVerifier
{
public:
    explicit            PasswordVerifier( const ::boost::shared_ptr< EncryptionInfo >& rEncryptInfo, const ::comphelper::EventLogger& rLogger );

    virtual ::comphelper::DocPasswordVerifierResult
                        verifyPassword( const OUString& rPassword, Sequence< NamedValue >& o_rEncryptionData );
    virtual ::comphelper::DocPasswordVerifierResult
                        verifyEncryptionData( const Sequence< NamedValue >& rEncryptionData );

private:
    const ::boost::shared_ptr< EncryptionInfo> encryptionInfo;
    const ::comphelper::EventLogger            logger;
};

PasswordVerifier::PasswordVerifier( const ::boost::shared_ptr< EncryptionInfo>& rEncryptInfo, const ::comphelper::EventLogger& rLogger ) :
    encryptionInfo( rEncryptInfo ),
    logger( rLogger )
{
}

::comphelper::DocPasswordVerifierResult PasswordVerifier::verifyPassword( const OUString& rPassword, Sequence< NamedValue >& o_rEncryptionData )
{
    try
    {
        o_rEncryptionData = encryptionInfo->verifyPassword( rPassword );
        if( o_rEncryptionData.hasElements() )
        {
            logger.log( LogLevel::FINE, OUString::createFromAscii( "Password is correct" ) );
            return ::comphelper::DocPasswordVerifierResult_OK;
        }
        else
        {
            logger.log( LogLevel::WARNING, OUString::createFromAscii( "Password is incorrect" ) );
            return ::comphelper::DocPasswordVerifierResult_WRONG_PASSWORD;
        }
    }
    catch ( const Exception &e )
    {
        logger.log( LogLevel::WARNING, "Error verifying password: $1$", e.Message );
        return ::comphelper::DocPasswordVerifierResult_ABORT;
    }
}

::comphelper::DocPasswordVerifierResult PasswordVerifier::verifyEncryptionData( const Sequence< NamedValue >& rEncryptionData )
{
    try
    {
        bool bResult = encryptionInfo->verifyEncryptionData( rEncryptionData );
        if( bResult )
        {
            logger.log( LogLevel::FINE, OUString::createFromAscii( "EncryptionData is correct" ) );
            return ::comphelper::DocPasswordVerifierResult_OK;
        }
        else
        {
            logger.log( LogLevel::WARNING, OUString::createFromAscii( "EncryptionData is incorrect" ) );
            return ::comphelper::DocPasswordVerifierResult_WRONG_PASSWORD;
        }
    }
    catch ( const Exception& e )
    {
        logger.log( LogLevel::WARNING, "Error verifying EncryptionData: $1$", e.Message );
        return ::comphelper::DocPasswordVerifierResult_ABORT;
    }
}

} // namespace

// ----------------------------------------------------------------------------

Reference< XInputStream > FilterDetect::extractUnencryptedPackage( MediaDescriptor& rMediaDesc ) const
{
    // try the plain input stream
    Reference< XInputStream > xInStrm( rMediaDesc[ MediaDescriptor::PROP_INPUTSTREAM() ], UNO_QUERY );
    if( !xInStrm.is() || lclIsZipPackage( mxContext, xInStrm ) )
        return xInStrm;

    // check if a temporary file is passed in the 'ComponentData' property
    Reference< XStream > xDecrypted( rMediaDesc.getComponentDataEntry( CREATE_OUSTRING( "DecryptedPackage" ) ), UNO_QUERY );
    if( xDecrypted.is() )
    {
        Reference< XInputStream > xDecrInStrm = xDecrypted->getInputStream();
        if( lclIsZipPackage( mxContext, xDecrInStrm ) )
            return xDecrInStrm;
    }

    // try to decrypt an encrypted OLE package
    ::oox::ole::OleStorage aOleStorage( mxContext, xInStrm, false );
    if( aOleStorage.isStorage() ) try
    {
        // open the required input streams in the encrypted package
        Reference< XInputStream > xEncryptionInfo( aOleStorage.openInputStream( CREATE_OUSTRING( "EncryptionInfo" ) ), UNO_SET_THROW );
        Reference< XInputStream > xEncryptedPackage( aOleStorage.openInputStream( CREATE_OUSTRING( "EncryptedPackage" ) ), UNO_SET_THROW );

        // read the encryption info stream
        ::boost::shared_ptr< EncryptionInfo > encryptionInfo( EncryptionInfo::readEncryptionInfo( mxContext, xEncryptionInfo ) );

        // check flags and algorithm IDs, required are AES128 and SHA-1
        bool bImplemented = encryptionInfo->isImplemented();
        if( bImplemented )
        {
            /*  "VelvetSweatshop" is the built-in default encryption
                password used by MS Excel for the "workbook protection"
                feature with password. Try this first before prompting the
                user for a password. */
            ::std::vector< OUString > aDefaultPasswords;
            aDefaultPasswords.push_back( CREATE_OUSTRING( "VelvetSweatshop" ) );

            /*  Use the comphelper password helper to request a password.
                This helper returns either with the correct password
                (according to the verifier), or with an empty string if
                user has cancelled the password input dialog. */
            PasswordVerifier aVerifier( encryptionInfo, logger );
            Sequence< NamedValue > aEncryptionData = ::comphelper::DocPasswordHelper::requestAndVerifyDocPassword(
                aVerifier, rMediaDesc, ::comphelper::DocPasswordRequestType_MS, &aDefaultPasswords );

            if( aEncryptionData.getLength() == 0 )
            {
                rMediaDesc[ MediaDescriptor::PROP_ABORTED() ] <<= true;
            }
            else
            {
                // create temporary file for unencrypted package
                Reference< XMultiComponentFactory > xFactory( mxContext->getServiceManager(), UNO_QUERY_THROW );
                Reference< XStream > xTempFile( xFactory->createInstanceWithContext( CREATE_OUSTRING( "com.sun.star.io.TempFile" ), mxContext ), UNO_QUERY_THROW );
                Reference< XOutputStream > xDecryptedPackage( xTempFile->getOutputStream(), UNO_SET_THROW );
                BinaryXOutputStream aDecryptedPackage( xDecryptedPackage, true );
                BinaryXInputStream aEncryptedPackage( xEncryptedPackage, true );

                encryptionInfo->decryptStream( aEncryptedPackage, aDecryptedPackage );
                aDecryptedPackage.seekToStart();

                // store temp file in media descriptor to keep it alive
                rMediaDesc.setComponentDataEntry( CREATE_OUSTRING( "DecryptedPackage" ), Any( xTempFile ) );

                Reference< XInputStream > xDecrInStrm = xTempFile->getInputStream();
                if( lclIsZipPackage( mxContext, xDecrInStrm ) )
                    return xDecrInStrm;
            }
        }
        else
            logger.log( LogLevel::WARNING, "Encryption type not implemented" );
    }
    catch( Exception& e )
    {
        logger.log( LogLevel::WARNING, "Error in ::oox::core::FilterDetect::extractUnencryptedPackage(): $1$", e.Message );
    }

    return Reference< XInputStream >();
}

// com.sun.star.lang.XServiceInfo interface -----------------------------------

OUString SAL_CALL FilterDetect::getImplementationName() throw( RuntimeException )
{
    return FilterDetect_getImplementationName();
}

sal_Bool SAL_CALL FilterDetect::supportsService( const OUString& rServiceName ) throw( RuntimeException )
{
    const Sequence< OUString > aServices = FilterDetect_getSupportedServiceNames();
    const OUString* pArray = aServices.getConstArray();
    const OUString* pArrayEnd = pArray + aServices.getLength();
    return ::std::find( pArray, pArrayEnd, rServiceName ) != pArrayEnd;
}

Sequence< OUString > SAL_CALL FilterDetect::getSupportedServiceNames() throw( RuntimeException )
{
    return FilterDetect_getSupportedServiceNames();
}

// com.sun.star.document.XExtendedFilterDetection interface -------------------

OUString SAL_CALL FilterDetect::detect( Sequence< PropertyValue >& rMediaDescSeq ) throw( RuntimeException )
{
    OUString aFilterName;
    MediaDescriptor aMediaDesc( rMediaDescSeq );

    /*  Check that the user has not chosen to abort detection, e.g. by hitting
        'Cancel' in the password input dialog. This may happen because this
        filter detection is used by different filters. */
    bool bAborted = aMediaDesc.getUnpackedValueOrDefault( MediaDescriptor::PROP_ABORTED(), false );
    if( !bAborted ) try
    {
        aMediaDesc.addInputStream();

        /*  Get the unencrypted input stream. This may include creation of a
            temporary file that contains the decrypted package. This temporary
            file will be stored in the 'ComponentData' property of the media
            descriptor. */
        Reference< XInputStream > xInStrm( extractUnencryptedPackage( aMediaDesc ), UNO_SET_THROW );

        // stream must be a ZIP package
        ZipStorage aZipStorage( mxContext, xInStrm );
        if( aZipStorage.isStorage() )
        {
            // create the fast parser, register the XML namespaces, set document handler
            FastParser aParser( mxContext );
            aParser.registerNamespace( NMSP_packageRel );
            aParser.registerNamespace( NMSP_officeRel );
            aParser.registerNamespace( NMSP_packageContentTypes );
            aParser.setDocumentHandler( new FilterDetectDocHandler( aFilterName ) );

            /*  Parse '_rels/.rels' to get the target path and '[Content_Types].xml'
                to determine the content type of the part at the target path. */
            aParser.parseStream( aZipStorage, CREATE_OUSTRING( "_rels/.rels" ) );
            aParser.parseStream( aZipStorage, CREATE_OUSTRING( "[Content_Types].xml" ) );
        }
    }
    catch( Exception& e )
    {
        logger.log( LogLevel::WARNING, "Error in ::oox::core::FilterDetect::detect(): $1$", e.Message );
    }

    // write back changed media descriptor members
    aMediaDesc >> rMediaDescSeq;
    return aFilterName;
}

// ============================================================================

} // namespace core
} // namespace oox
