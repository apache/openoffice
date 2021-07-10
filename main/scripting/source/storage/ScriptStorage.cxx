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
#include "precompiled_scripting.hxx"
#include <osl/file.hxx>
#include <osl/time.h>
#include <cppuhelper/implementationentry.hxx>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/ucb/CommandAbortedException.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>
#include <com/sun/star/xml/sax/XExtendedDocumentHandler.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>

#include <util/util.hxx>
#include <rtl/uri.hxx>


#include "ScriptData.hxx"
#include "ScriptInfo.hxx"
#include "ScriptStorage.hxx"
#include "ScriptElement.hxx"
#include "ScriptMetadataImporter.hxx"
#include "ScriptURI.hxx"

using namespace ::rtl;
using namespace ::cppu;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::drafts::com::sun::star::script::framework;

namespace scripting_impl
{

ScriptLanguages_hash* ScriptStorage::mh_scriptLangs = NULL;

const sal_Char* const SERVICE_NAME = 
    "drafts.com.sun.star.script.framework.storage.ScriptStorage";
const sal_Char* const IMPL_NAME = 
    "drafts.com.sun.star.script.framework.storage.ScriptStorage";

const sal_Char * const SCRIPT_DIR = "/Scripts";
const sal_Char * const SCRIPT_PARCEL = "/parcel-descriptor.xml";
const sal_Char * const SCRIPT_PARCEL_NAME_ONLY = "parcel-descriptor";

static OUString ss_implName = OUString::createFromAscii( IMPL_NAME );
static OUString ss_serviceName = OUString::createFromAscii( SERVICE_NAME );
static Sequence< OUString > ss_serviceNames = 
    Sequence< OUString >( &ss_serviceName, 1 );

const sal_uInt16 NUMBER_STORAGE_INITIALIZE_ARGS = 3;

//extern ::rtl_StandardModuleCount s_moduleCount;



//*************************************************************************
ScriptStorage::ScriptStorage( const Reference <
                              XComponentContext > & xContext )
throw ( RuntimeException )
        : m_xContext( xContext, UNO_SET_THROW ), m_bInitialised( false )
{
    OSL_TRACE( "< ScriptStorage ctor called >\n" );

    m_xMgr.set( m_xContext->getServiceManager(), UNO_SET_THROW );

    if( !mh_scriptLangs )
    {
        ::osl::MutexGuard guard( ::osl::Mutex::getGlobalMutex() );
        if( !mh_scriptLangs )
        {
            mh_scriptLangs = new ScriptLanguages_hash();
            Reference< lang::XMultiServiceFactory > xConfigProvFactory(
                m_xMgr->createInstanceWithContext( OUString::createFromAscii( "com.sun.star.configuration.ConfigurationProvider" ), m_xContext ),
                UNO_QUERY_THROW );
            // create an instance of the ConfigurationAccess for accessing the
            // scripting runtime settings
            beans::PropertyValue configPath;
            configPath.Name = ::rtl::OUString::createFromAscii( "nodepath" );
            configPath.Value <<= ::rtl::OUString::createFromAscii( "org.openoffice.Office.Scripting/ScriptRuntimes" );
            Sequence < Any > aargs( 1 );
            aargs[ 0 ] <<= configPath;

            Reference< container::XNameAccess > xNameAccess(
                xConfigProvFactory->createInstanceWithArguments(
                    OUString::createFromAscii( "com.sun.star.configuration.ConfigurationAccess" ),
                    aargs
                ),
                UNO_QUERY_THROW );

            Sequence< OUString > names = xNameAccess->getElementNames();
            for( int i = 0 ; i < names.getLength() ; i++ )
            {
                OSL_TRACE(  "Getting propertyset for Lang=%s", 
                    ::rtl::OUStringToOString( names[i], RTL_TEXTENCODING_ASCII_US ).pData->buffer );
                Reference< beans::XPropertySet > xPropSet( xNameAccess->getByName( names[i] ), UNO_QUERY_THROW );
                Any aProp = xPropSet->getPropertyValue( 
                        OUString::createFromAscii( "SupportedFileExtensions") );
                Sequence< OUString > extns;
                if( sal_False == ( aProp >>= extns ) )
                {
                    throw RuntimeException(
                        OUSTR( "ScriptStorage:ScriptStorage: can't get runtime extensions" ),
                        Reference< XInterface > () );
                }
                for( int j = 0 ; j < extns.getLength() ; j++ )
                {
                    OSL_TRACE(  "Adding Lang=%s, Extn=%s\n", 
                        ::rtl::OUStringToOString( names[i], RTL_TEXTENCODING_ASCII_US ).pData->buffer,
                        ::rtl::OUStringToOString( extns[j], RTL_TEXTENCODING_ASCII_US ).pData->buffer );
                    (*mh_scriptLangs)[ extns[j] ] =
                        names[i];
                }
            }
        }
    }
//    s_moduleCount.modCnt.acquire( &s_moduleCount.modCnt );
}

//*************************************************************************
ScriptStorage::~ScriptStorage() SAL_THROW( () )
{
    OSL_TRACE( "< ScriptStorage dtor called >\n" );
//    s_moduleCount.modCnt.release( &s_moduleCount.modCnt );
}

//*************************************************************************
void
ScriptStorage::initialize( const Sequence <Any> & args )
throw ( RuntimeException, Exception )
{
    OSL_TRACE( "Entering ScriptStorage::initialize\n" );

    // Should not be renitialized
    if ( m_bInitialised )
    {
        throw RuntimeException( 
            OUSTR( "ScriptStorage::initialize already initialized" ),
            Reference<XInterface> () );
    }

    {   // Protect member variable writes
        ::osl::Guard< osl::Mutex > aGuard( m_mutex );

        // Check args
        if ( args.getLength() != NUMBER_STORAGE_INITIALIZE_ARGS )
        {
            OSL_TRACE( "ScriptStorage::initialize: got wrong number of args\n" );
            throw RuntimeException(
                OUSTR( "Invalid number of arguments provided!" ),
                Reference< XInterface >() );
        }

        if ( sal_False == ( args[ 0 ] >>= m_xSimpleFileAccess ) )
        {
            throw RuntimeException(
                OUSTR( "Invalid XSimpleFileAccess argument provided!" ),
                Reference< XInterface >() );
        }

        if ( sal_False == ( args[ 1 ] >>= m_scriptStorageID ) )
        {
            throw RuntimeException(
                OUSTR( "Invalid ScriptStorage ID argument provided!" ),
                Reference< XInterface >() );
        
        }
        if ( sal_False == ( args[ 2 ] >>= m_stringUri ) )
        {
            throw RuntimeException(
                OUSTR( "Invalid String Uri argument provided!" ),
                Reference< XInterface >() );
        }
    } // End - Protect member variable writes

    OSL_TRACE(  "uri: %s\n", ::rtl::OUStringToOString( 
        m_stringUri, RTL_TEXTENCODING_ASCII_US ).pData->buffer );

    try
    {
        // need to check for what???
        // what we have is a URI for the filesystem or document
        // we need to check of the last element in the path has an
        // extension that is associated with a script (eg. .bsh, .js etc)
        OUString fileExtension = getFileExtension( m_stringUri );
        // and see if this is in our scripts map
        ScriptLanguages_hash::iterator h_it = mh_scriptLangs->find( fileExtension );
        if ( h_it != mh_scriptLangs->end() ) 
        {
            createForFilesystem( fileExtension );
        }
        else 
        {
            create();
        }
    }
    catch ( RuntimeException & re )
    {
        OSL_TRACE( "caught com::sun::star::uno::RuntimeException in ScriptStorage::initialize" );
        throw RuntimeException(
            OUSTR( "ScriptStorage::initialize RuntimeException: " ).concat( re.Message ),
            Reference< XInterface > () );
    }
    catch ( Exception & ue )
    {
        OSL_TRACE( "caught com::sun::star::uno::Exception in ScriptStorage::initialize" );
        throw RuntimeException(
            OUSTR( "ScriptStorage::initialize Exception: " ).concat( ue.Message ),
            Reference< XInterface > () );
    }
#ifdef _DEBUG
    catch ( ... )
    {
        OSL_TRACE( "caught unknown Exception in ScriptStorage::initialize" );
        throw RuntimeException(
            OUSTR( "ScriptStorage::initialize unknown exception: " ),
            Reference< XInterface > () );
    }
#endif

    OSL_TRACE( "Parsed the XML\n" );

    m_bInitialised = true;
}

void
ScriptStorage::create()
throw ( RuntimeException, Exception )
{
    ::osl::Guard< osl::Mutex > aGuard( m_mutex );
    try
    {
        // clear existing hashmap - rebuilding from scratch to avoid having 
        // to search for deleted elements on refresh
        mh_implementations.clear();

        OUString xStringUri(m_stringUri);

        ScriptMetadataImporter* SMI = new ScriptMetadataImporter( m_xContext );
        Reference< xml::sax::XExtendedDocumentHandler > xSMI( SMI, UNO_SET_THROW );

        xStringUri = xStringUri.concat( ::rtl::OUString::createFromAscii( 
            SCRIPT_DIR ) );

       // No Scripts directory - just return
       if ( ! m_xSimpleFileAccess->isFolder( xStringUri ) )
       {
            OSL_TRACE( "ScriptStorage::initialize: no Scripts dir for this storage - install problem\n" );
           return;
       }
       
        // get the list of language folders under the Scripts directory
        Sequence< ::rtl::OUString > languageDirs =
            m_xSimpleFileAccess->getFolderContents( xStringUri, true );

        Reference< io::XInputStream > xInput;
        sal_Int32 languageDirsLength = languageDirs.getLength();
        for ( sal_Int32 i = 0; i < languageDirsLength ; ++i )
        {
            OSL_TRACE(  "contains: %s\n", ::rtl::OUStringToOString( 
                languageDirs[ i ], RTL_TEXTENCODING_ASCII_US ).pData->buffer );

            if ( ! m_xSimpleFileAccess->isFolder( languageDirs[ i ] ) )
            {
                continue;
            }

            //get the list of parcel folders for each language folder
            // under Scripts
            Sequence< ::rtl::OUString > parcelDirs =
                m_xSimpleFileAccess->getFolderContents( languageDirs[ i ], true );

            sal_Int32 parcelDirsLength = parcelDirs.getLength();
            for ( sal_Int32 j = 0; j < parcelDirsLength ; ++j )
            {
                OSL_TRACE(  "contains: %s\n",
                    ::rtl::OUStringToOString( parcelDirs[ j ],
                    RTL_TEXTENCODING_ASCII_US ).pData->buffer );

                OUString parcelFile = parcelDirs[ j ].concat(
                    ::rtl::OUString::createFromAscii( SCRIPT_PARCEL ) );

                // Do not have a valid parcel.xml
                if ( !m_xSimpleFileAccess->exists( parcelFile ) ||
                        m_xSimpleFileAccess->isFolder( parcelFile ) )
                {
                    continue;
                }
                OSL_TRACE(  "parcel file: %s\n",
                    ::rtl::OUStringToOString( parcelFile, 
                    RTL_TEXTENCODING_ASCII_US ).pData->buffer );

                xInput = m_xSimpleFileAccess->openFileRead( parcelFile );
                // Failed to get input stream
                if ( !xInput.is() )
                {
                    continue;
                }

                OSL_TRACE( "Parse the metadata \n" );
                Datas_vec vScriptDatas;
                try
                {
                    SMI->parseMetaData( xInput, parcelDirs[ j ], vScriptDatas );
                }
                catch ( xml::sax::SAXException & saxe )
                {
                    if ( xInput.is() )
                    {
                        xInput->closeInput();
                    }
                    OSL_TRACE(
                        "caught com::sun::star::xml::sax::SAXException in ScriptStorage::create %s",
                        ::rtl::OUStringToOString( saxe.Message,
                        RTL_TEXTENCODING_ASCII_US ).pData->buffer  );

                    continue;
                }
                catch ( io::IOException & ioe )
                {
                    if ( xInput.is() )
                    {
                        xInput->closeInput();
                    }
                    OSL_TRACE( 
                        "caught com::sun::star::io::IOException in ScriptStorage::create" );
                    continue;
                }
                xInput->closeInput();

                updateMaps( vScriptDatas );
            }
        }
    }
    catch ( io::IOException & ioe )
    {
        //From ScriptMetadata Importer
        OSL_TRACE( "caught com::sun::star::io::IOException in ScriptStorage::create" );
        throw RuntimeException(
            OUSTR( "ScriptStorage::create IOException: " ).concat( ioe.Message ),
            Reference< XInterface > () );

    }
    catch ( ucb::CommandAbortedException & cae )
    {
        OSL_TRACE( "caught com::sun::star::ucb::CommandAbortedException in ScriptStorage::create" );
        throw RuntimeException(
            OUSTR(
                "ScriptStorage::create CommandAbortedException: " ).concat( cae.Message ),
            Reference< XInterface > () );
    }
    catch ( RuntimeException & re )
    {
        OSL_TRACE( "caught com::sun::star::uno::RuntimeException in ScriptStorage::create" );
        throw RuntimeException(
            OUSTR( "ScriptStorage::create RuntimeException: " ).concat( re.Message ),
            Reference< XInterface > () );
    }
    catch ( Exception & ue )
    {
        OSL_TRACE( "caught com::sun::star::uno::Exception in ScriptStorage::create" );
        throw RuntimeException(
            OUSTR( "ScriptStorage::create Exception: " ).concat( ue.Message ),
            Reference< XInterface > () );
    }
#ifdef _DEBUG
    catch ( ... )
    {
        OSL_TRACE( "caught unknown Exception in ScriptStorage::create" );
        throw RuntimeException(
            OUSTR( "ScriptStorage::initialize unknown exception: " ),
            Reference< XInterface > () );
    }
#endif

    OSL_TRACE( "Parsed the XML\n" );

    m_bInitialised = true;
}

//*************************************************************************
// private method to create the usual data structures for scripts located
// on the filesystem. 
// parcelURI = the path to the script
// functionName = the full filename with extension
// logicalName = the filename without the extension
void
ScriptStorage::createForFilesystem( const OUString & fileExtension )
throw ( RuntimeException, Exception )
{
    // need to decode as file urls are encoded
    OUString xStringUri = ::rtl::Uri::decode( m_stringUri, 
        rtl_UriDecodeWithCharset, RTL_TEXTENCODING_ASCII_US );

    // no x-platform issues here as we are dealing with URLs
    sal_Int32 lastFileSep = xStringUri.lastIndexOf( '/' );
    // the char just after the filesep
    lastFileSep += 1;
    sal_Int32 lastFileExt = xStringUri.lastIndexOf( fileExtension );
    OUString searchString = OUString::createFromAscii( "://" );
    sal_Int32 searchStringLength = searchString.getLength();
    sal_Int32 startPath = xStringUri.indexOf( searchString );
    sal_Int32 uriLength = xStringUri.getLength();
    OUString fileNameNoExt = xStringUri.copy( lastFileSep , 
        lastFileExt - lastFileSep  - 1 );
    OUString fileName = xStringUri.copy( lastFileSep, uriLength - lastFileSep );
    OUString filePath = xStringUri.copy( startPath + searchStringLength, 
        lastFileSep - startPath - searchStringLength );
    OUString filePathWithName = xStringUri.copy( startPath + searchStringLength,
        uriLength - startPath - searchStringLength );

    ScriptData scriptData;
    scriptData.language = mh_scriptLangs->find( fileExtension )->second;
    OSL_TRACE( "\t language = %s", ::rtl::OUStringToOString( 
        scriptData.language, RTL_TEXTENCODING_ASCII_US ).pData->buffer );

    // do we need to encode this?
    scriptData.functionname = fileName;
    OSL_TRACE( "\t functionName = %s", ::rtl::OUStringToOString( 
        scriptData.functionname, RTL_TEXTENCODING_ASCII_US ).pData->buffer );
    //scriptData.functionname = ::rtl::Uri::encode( fileName, 
        //rtl_UriCharClassUricNoSlash, rtl_UriEncodeCheckEscapes,
        //RTL_TEXTENCODING_ASCII_US );
    
    scriptData.parcelURI = filePath;
    OSL_TRACE( "\t parcelURI = %s", ::rtl::OUStringToOString( 
        scriptData.parcelURI, RTL_TEXTENCODING_ASCII_US ).pData->buffer );
    scriptData.logicalname = fileNameNoExt;
    OSL_TRACE( "\t logicalName = %s", ::rtl::OUStringToOString( 
        scriptData.logicalname, RTL_TEXTENCODING_ASCII_US ).pData->buffer );
    
    // and now push onto the usual structures
    ScriptFunction_hash sfh;
    sfh[ scriptData.functionname ] = scriptData;
    mh_implementations[ scriptData.language ] = sfh;
    m_bInitialised = true; 
}

//*************************************************************************
// private method to return the file extension, eg. bsh, js etc
OUString 
ScriptStorage::getFileExtension( const OUString & stringUri )
{
    OUString fileExtension;
    sal_Int32 lastDot = stringUri.lastIndexOf( '.' );
    if( lastDot > 0 ) {
        sal_Int32 stringUriLength = stringUri.getLength();
        fileExtension = stringUri.copy( lastDot +1 , stringUriLength - lastDot - 1 );
    } 
    else 
    {
        fileExtension = OUString::createFromAscii("");
    }
    return fileExtension;
}

//*************************************************************************
// private method for updating hashmaps
void
ScriptStorage::updateMaps( const Datas_vec & vScriptDatas )
{

    Datas_vec::const_iterator it_end = vScriptDatas.end();
    // step through the vector of ScripImplInfos returned from parse
    for ( Datas_vec::const_iterator it = vScriptDatas.begin() ; it != it_end; ++it )
    {
        //find the Datas_vec for this logical name
        ScriptData_hash::iterator h_it = mh_implementations.find( it->language );

        if ( h_it == mh_implementations.end() )
        {
            //if it's null, need to create a new Datas_vec
            OSL_TRACE( 
                     "updateMaps: new language: %s\n", rtl::OUStringToOString(
                         it->language, RTL_TEXTENCODING_ASCII_US ).pData->buffer );
            OSL_TRACE( 
                     "updateMaps: adding functionname: %s\n", rtl::OUStringToOString(
                         it->functionname, RTL_TEXTENCODING_ASCII_US ).pData->buffer );

            ScriptFunction_hash sfh;
            sfh[ it->functionname ] = *it;
            mh_implementations[ it->language ] = sfh;
        }
        else
        {
            OSL_TRACE( 
                     "updateMaps: adding functionname: %s\n", rtl::OUStringToOString(
                         it->functionname, RTL_TEXTENCODING_ASCII_US ).pData->buffer );
            OSL_TRACE(  "                    language name: %s\n", 
                rtl::OUStringToOString( it->functionname, 
                RTL_TEXTENCODING_ASCII_US ).pData->buffer );

            h_it->second[ it->functionname ] = *it;
        }
    }
}

//*************************************************************************
// XScriptStorageExport::save
void
ScriptStorage::save()
throw ( RuntimeException )
{
    ::osl::Guard< osl::Mutex > aGuard( m_mutex );
    Reference< io::XActiveDataSource > xSource;
    Reference< io::XOutputStream > xOS;

    // xScriptInvocation = Reference<XScriptInvocation>(xx, UNO_QUERY_THROW);
    Reference< xml::sax::XExtendedDocumentHandler > xHandler;

    OUString parcel_suffix = OUString::createFromAscii( SCRIPT_PARCEL );
    OUString ou_parcel = OUString( 
        RTL_CONSTASCII_USTRINGPARAM( SCRIPT_PARCEL_NAME_ONLY ) );

    try
    {
        ScriptData_hash::iterator it_end = mh_implementations.end();
        for ( ScriptData_hash::iterator it = mh_implementations.begin() ; it != it_end; ++it )
        {
            ::rtl::OUString logName = it->first;
            ScriptFunction_hash::iterator it_sfh_end = it->second.end();
            for ( ScriptFunction_hash::iterator it_sfh = it->second.begin();
                    it_sfh != it_sfh_end ; ++it_sfh )
            {
                ScriptOutput_hash::const_iterator it_parcels =
                    mh_parcels.find( it_sfh->second.parcelURI );
                if ( it_parcels == mh_parcels.end() )
                {
                    //create new outputstream
                    OUString parcel_xml_path = it_sfh->second.parcelURI.concat( 
                        parcel_suffix );
                    m_xSimpleFileAccess->kill( parcel_xml_path );
                    xOS = m_xSimpleFileAccess->openFileWrite( parcel_xml_path );

                    OSL_TRACE(  "saving: %s\n", rtl::OUStringToOString( 
                        it_sfh->second.parcelURI.concat( OUString::createFromAscii( 
                        "/parcel.xml" ) ), 
                        RTL_TEXTENCODING_ASCII_US ).pData->buffer );

                    xHandler.set( 
                        m_xMgr->createInstanceWithContext(
                            OUString::createFromAscii( "com.sun.star.xml.sax.Writer" ),
                            m_xContext
                        ),
                        UNO_QUERY_THROW
                    );
                    xSource.set( xHandler, UNO_QUERY_THROW );
                    xSource->setOutputStream( xOS );

                    writeMetadataHeader( xHandler );

                    mh_parcels[ it_sfh->second.parcelURI ] = xHandler;
                }
                else
                {
                    xHandler = it_parcels->second;
                }

                ScriptElement* pSE = new ScriptElement( it_sfh->second );
                // this is to get pSE released correctly
                Reference < xml::sax::XAttributeList > xal( pSE );
                pSE->dump( xHandler );
            }
        }

        ScriptOutput_hash::const_iterator out_it_end = mh_parcels.end();

        for ( ScriptOutput_hash::const_iterator out_it = mh_parcels.begin();
                out_it != out_it_end; ++out_it )
        {
            out_it->second->ignorableWhitespace( ::rtl::OUString() );
            out_it->second->endDocument();
            xSource.set( out_it->second, UNO_QUERY );
            Reference< io::XOutputStream > xOS = xSource->getOutputStream();
            xOS->closeOutput();

        }
        
        // clear the hash map, as all output streams have been closed.
        // need to re-create on next save
        mh_parcels.clear();
    }
    // *** TODO - other exception handling IO etc.
    catch ( RuntimeException & re )
    {
        OSL_TRACE( "caught com::sun::star::uno::RuntimeException in ScriptStorage::save" );
        throw RuntimeException(
            OUSTR( "ScriptStorage::save RuntimeException: " ).concat( 
            re.Message ),
            Reference< XInterface > () );
    }
}

//*************************************************************************
void 
ScriptStorage::refresh()
throw (RuntimeException)
{
    OSL_TRACE("** => ScriptStorage: in refresh()\n");

    // guard against concurrent refreshes
    ::osl::Guard< ::osl::Mutex > aGuard( m_mutex );

    try
    {
        create();

    }
    catch ( RuntimeException & re )
    {
        OSL_TRACE( "caught com::sun::star::uno::RuntimeException in ScriptStorage::refresh" );
        throw RuntimeException(
            OUSTR( "ScriptStorage::refresh RuntimeException: " ).concat( re.Message ),
            Reference< XInterface > () );
    }
    catch ( Exception & ue )
    {
        OSL_TRACE( "caught com::sun::star::uno::Exception in ScriptStorage::refresh" );
        throw RuntimeException(
            OUSTR( "ScriptStorage::refresh Exception: " ).concat( ue.Message ),
            Reference< XInterface > () );
    }
}

//*************************************************************************
void
ScriptStorage::writeMetadataHeader( 
    Reference <xml::sax::XExtendedDocumentHandler> & xHandler )
{
    xHandler->startDocument();
    OUString aDocTypeStr( RTL_CONSTASCII_USTRINGPARAM(
        "<!DOCTYPE  parcel SYSTEM \"scripting.dtd\">" ) );
    xHandler->unknown( aDocTypeStr );
    xHandler->ignorableWhitespace( OUString() );
}


//*************************************************************************
Sequence< ::rtl::OUString >
ScriptStorage::getScriptLogicalNames()
throw ( RuntimeException )
{
    Sequence< ::rtl::OUString  > results;
    // comment out the rest, and ultimately remove method
    /*ScriptInfo_hash::iterator h_it = mh_implementations.begin();
    ScriptInfo_hash::iterator h_itEnd =  mh_implementations.end();
    if ( h_it == h_itEnd  )
    {
        OSL_TRACE( "ScriptStorage::getImplementations: EMPTY STORAGE");
        return results;
    }
    results.realloc( mh_implementations.size() );

    //find the implementations for the given logical name
    try
    {

        ::osl::Guard< osl::Mutex > aGuard( m_mutex );

        for ( sal_Int32 count = 0; h_it != h_itEnd ; ++h_it )
        {
            ::rtl::OUString logicalName = h_it->first;
            OSL_TRACE( "Adding %s at index %d ", ::rtl::OUStringToOString( 
                logicalName, RTL_TEXTENCODING_ASCII_US ).pData->buffer, count); 
            results[ count++ ] = logicalName;
        }

    }
    catch ( RuntimeException & re )
    {
        throw RuntimeException(
            OUSTR( "ScriptStorage::getScriptLogicalNames RuntimeException: " ).concat( re.Message ),
            Reference< XInterface > () );
    }
    catch ( Exception & e )
    {
        throw RuntimeException( OUSTR( 
            "ScriptStorage::getScriptLogicalNames Exception: " ).concat( 
            e.Message ), Reference< XInterface > () );
    } */
    return results;
}

//*************************************************************************
Sequence< Reference< storage::XScriptInfo > >
ScriptStorage::getImplementations( const ::rtl::OUString & queryURI )
throw ( lang::IllegalArgumentException,
        RuntimeException )
{
    ::osl::Guard< osl::Mutex > aGuard( m_mutex );
// format is script:://[function_name]?language=[languge]&location=[location]
// LogicalName is now not used anymore, further more the ScriptURI class
// will be retired also and a new UNO service will be used. Additionally the
// parcel-description will also need to be modified to remove logical name
// ScriprtMetaDataImporter has been modified to ignore the Logical name
// definined in the parcel-desc.xml. As an interim temp solution the  Datas_vec
// structure that is returned from ScriptMetDataImporter sets the logicalname 
// to the function name. ScriptURI class has been changed in the same way.
// 
    Sequence< Reference< storage::XScriptInfo > > results;
    ScriptURI scriptURI( queryURI );
    OSL_TRACE( "getting impl for language %s, function name: %s",
        ::rtl::OUStringToOString( scriptURI.getLanguage(), 
        RTL_TEXTENCODING_ASCII_US ).pData->buffer,
        ::rtl::OUStringToOString( scriptURI.getFunctionName(), 
        RTL_TEXTENCODING_ASCII_US ).pData->buffer );
    ScriptData_hash::iterator h_itEnd =  mh_implementations.end();
    ScriptData_hash::iterator h_it = mh_implementations.begin();
    if ( h_it == h_itEnd )
    {
        OSL_TRACE( "ScriptStorage::getImplementations: EMPTY STORAGE" );
        return results;
    }

    //find the implementations for the given language 
    h_it = mh_implementations.find( scriptURI.getLanguage() );
 
    if ( h_it == h_itEnd )
    {
        OSL_TRACE( "ScriptStorage::getImplementations: no impls found for %s",
            ::rtl::OUStringToOString( scriptURI.getLanguage(), 
            RTL_TEXTENCODING_ASCII_US ).pData->buffer );
        return results;
    }
     
    //find the implementations for the given language 
    ScriptFunction_hash::const_iterator it_datas = h_it->second.find( 
        scriptURI.getLogicalName() );
    ScriptFunction_hash::const_iterator it_datas_end = h_it->second.end();

    if ( it_datas == it_datas_end )
    {
        OSL_TRACE( "ScriptStorage::getImplementations: no impls found for %s",
            ::rtl::OUStringToOString( scriptURI.getFunctionName(), 
            RTL_TEXTENCODING_ASCII_US ).pData->buffer );
        return results;
    }
    
    results.realloc( 1 );
    ScriptData scriptData = it_datas->second;
    OSL_TRACE( "ScriptStorage::getImplementations: impls found for %s",
        ::rtl::OUStringToOString( scriptData.functionname, 
        RTL_TEXTENCODING_ASCII_US ).pData->buffer );
    Reference< storage::XScriptInfo > xScriptInfo = 
        new ScriptInfo ( scriptData, m_scriptStorageID );
    results[ 0 ] = xScriptInfo;

    return results;
}

//*************************************************************************
Sequence< Reference< storage::XScriptInfo > > SAL_CALL
ScriptStorage::getAllImplementations() throw ( RuntimeException )
{
    ::osl::Guard< osl::Mutex > aGuard( m_mutex );
    Sequence< Reference< storage::XScriptInfo > > results;
    ScriptData_hash::iterator h_itEnd =  mh_implementations.end();
    ScriptData_hash::iterator h_it = mh_implementations.begin();
    if ( h_it == h_itEnd )
    {
        OSL_TRACE( "ScriptStorage::getImplementations: EMPTY STORAGE" );
        return results;
    }

    
    //iterate through each logical name and gather each implementation
    //for that name
    for ( sal_Int32 count = 0; h_it !=  h_itEnd; ++h_it )
    {
        results.realloc( h_it->second.size() + count );
        OSL_TRACE( "Adding implementations for %s",
            ::rtl::OUStringToOString( h_it->first,
                RTL_TEXTENCODING_ASCII_US ).pData->buffer );
        ScriptFunction_hash::const_iterator it_sfh = h_it->second.begin();
        ScriptFunction_hash::const_iterator it_sfh_end = h_it->second.end();
        OSL_TRACE( "Adding %d to sequence of impls ", h_it->second.size() );
        for ( ; it_sfh != it_sfh_end ; ++it_sfh )
        {
            Reference< storage::XScriptInfo > xScriptInfo = new ScriptInfo (
            it_sfh->second, m_scriptStorageID );
 
            results[ count++ ] = xScriptInfo;
        }
    }
    return results;
 
}

//*************************************************************************
OUString SAL_CALL ScriptStorage::getImplementationName( )
throw( RuntimeException )
{
    return ss_implName;
}

//*************************************************************************
sal_Bool SAL_CALL ScriptStorage::supportsService( const OUString& serviceName )
throw( RuntimeException )
{
    OUString const * pNames = ss_serviceNames.getConstArray();
    for ( sal_Int32 nPos = ss_serviceNames.getLength(); nPos--; )
    {
        if ( serviceName.equals( pNames[ nPos ] ) )
        {
            return sal_True;
        }
    }
    return sal_False;
}

//*************************************************************************
Sequence<OUString> SAL_CALL ScriptStorage::getSupportedServiceNames( )
throw( RuntimeException )
{
    return ss_serviceNames;
}

} // namespace scripting_impl


namespace scripting_runtimemgr
{

//*************************************************************************
Reference<XInterface> SAL_CALL ss_create(
    const Reference< XComponentContext > & xCompC )
{
    return ( cppu::OWeakObject * ) new ::scripting_impl::ScriptStorage( xCompC );
}

//*************************************************************************
Sequence<OUString> ss_getSupportedServiceNames( )
SAL_THROW( () )
{
    return ::scripting_impl::ss_serviceNames;
}

//*************************************************************************
OUString ss_getImplementationName( )
SAL_THROW( () )
{
    return ::scripting_impl::ss_implName;
}
}//end namespace
