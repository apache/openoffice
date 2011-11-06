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



#ifndef _RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif

#ifndef _COM_SUN_STAR_XML_SAX_SAXPARSEEXCEPTION_HDL_
#include <com/sun/star/xml/sax/SAXParseException.hdl>
#endif
#ifndef _SFXDOCFILE_HXX
#include <bf_sfx2/docfile.hxx>
#endif
#ifndef _DRAWDOC_HXX
#include "drawdoc.hxx"
#endif
#ifndef _UTL_STREAM_WRAPPER_HXX_
#include <unotools/streamwrap.hxx>
#endif
#ifndef _XMLGRHLP_HXX
#include <bf_svx/xmlgrhlp.hxx>
#endif

#include "bf_sd/docshell.hxx"
#include "sdxmlwrp.hxx"
#include "strmname.h"

#ifndef _XMLEOHLP_HXX
#include <bf_svx/xmleohlp.hxx>
#endif

#include <bf_sfx2/appuno.hxx>

#ifndef _COM_SUN_STAR_DOCUMENT_XFILTER_HPP_
#include <com/sun/star/document/XFilter.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XIMPORTER_HPP_
#include <com/sun/star/document/XImporter.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_XExporter_HPP_
#include <com/sun/star/document/XExporter.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HXX_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif
#ifndef _COM_SUN_STAR_PACKAGES_ZIP_ZIPIOEXCEPTION_HPP_
#include <com/sun/star/packages/zip/ZipIOException.hpp>
#endif

#include <com/sun/star/xml/sax/XParser.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>

#ifndef _COMPHELPER_GENERICPROPERTYSET_HXX_
#include <comphelper/genericpropertyset.hxx>
#endif

#ifndef INCLUDED_SVTOOLS_SAVEOPT_HXX
#include <bf_svtools/saveopt.hxx>
#endif

// #80365# include necessary for XML progress bar at load time
#ifndef _SFXITEMSET_HXX
#include <bf_svtools/itemset.hxx>
#endif

#ifndef _SFXECODE_HXX
#include <bf_svtools/sfxecode.hxx>
#endif

#include "sderror.hxx"
#include "sdresid.hxx"
#include "glob.hrc"

#ifndef _LEGACYBINFILTERMGR_HXX
#include <legacysmgr/legacy_binfilters_smgr.hxx>	//STRIP002
#endif
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::document;
using namespace rtl;
using namespace comphelper;

#define SD_XML_READERROR 1234

//////////////////////////////////////////////////////////////////////////////

#ifndef SEQTYPE
 #if defined(__SUNPRO_CC) && (__SUNPRO_CC == 0x500)
  #define SEQTYPE(x) (new ::com::sun::star::uno::Type( x ))
 #else
  #define SEQTYPE(x) &(x)
 #endif
#endif

#define MAP_LEN(x) x, sizeof(x) - 1

#define XML_STRING(i, x) sal_Char __READONLY_DATA i[sizeof(x)] = x

XML_STRING( sXML_metaStreamName, "meta.xml");
XML_STRING( sXML_styleStreamName, "styles.xml" );
XML_STRING( sXML_contentStreamName, "content.xml" );
XML_STRING( sXML_oldContentStreamName, "Content.xml" );
XML_STRING( sXML_settingsStreamName, "settings.xml" );

XML_STRING( sXML_export_impress_meta_service, "com.sun.star.comp.Impress.XMLMetaExporter" );
XML_STRING( sXML_export_impress_styles_service, "com.sun.star.comp.Impress.XMLStylesExporter" );
XML_STRING( sXML_export_impress_content_service, "com.sun.star.comp.Impress.XMLContentExporter" );
XML_STRING( sXML_export_impress_settings_service, "com.sun.star.comp.Impress.XMLSettingsExporter" );

XML_STRING( sXML_export_draw_meta_service, "com.sun.star.comp.Draw.XMLMetaExporter" );
XML_STRING( sXML_export_draw_styles_service, "com.sun.star.comp.Draw.XMLStylesExporter" );
XML_STRING( sXML_export_draw_content_service, "com.sun.star.comp.Draw.XMLContentExporter" );
XML_STRING( sXML_export_draw_settings_service, "com.sun.star.comp.Draw.XMLSettingsExporter" );

XML_STRING( sXML_import_impress_service, "com.sun.star.comp.Impress.XMLImporter" );
XML_STRING( sXML_import_impress_meta_service, "com.sun.star.comp.Impress.XMLMetaImporter" );
XML_STRING( sXML_import_impress_styles_service, "com.sun.star.comp.Impress.XMLStylesImporter" );
XML_STRING( sXML_import_impress_content_service, "com.sun.star.comp.Impress.XMLContentImporter" );
XML_STRING( sXML_import_impress_settings_service, "com.sun.star.comp.Impress.XMLSettingsImporter" );

XML_STRING( sXML_import_draw_service, "com.sun.star.comp.Draw.XMLImporter" );
XML_STRING( sXML_import_draw_meta_service, "com.sun.star.comp.Draw.XMLMetaImporter" );
XML_STRING( sXML_import_draw_styles_service, "com.sun.star.comp.Draw.XMLStylesImporter" );
XML_STRING( sXML_import_draw_content_service, "com.sun.star.comp.Draw.XMLContentImporter" );
XML_STRING( sXML_import_draw_settings_service, "com.sun.star.comp.Draw.XMLSettingsImporter" );

struct XML_SERVICEMAP
{
	const sal_Char* mpService;
	const sal_Char* mpStream;
	sal_Bool mbPlain;
};


// ----------------
// - SdXMLWrapper -
// ----------------

SdXMLFilter::SdXMLFilter( SfxMedium& rMedium, SdDrawDocShell& rDocShell, sal_Bool bShowProgress, SdXMLFilterMode eFilterMode ) :
	SdFilter( rMedium, rDocShell, bShowProgress ), meFilterMode( eFilterMode )
{
}

sal_Int32 ReadThroughComponent(
	Reference<io::XInputStream> xInputStream,
	Reference<XComponent> xModelComponent,
	const String& rStreamName,
	Reference<lang::XMultiServiceFactory> & rFactory,
	const sal_Char* pFilterName,
	Sequence<Any> rFilterArguments,
	const OUString& rName,
	sal_Bool bMustBeSuccessfull,
	sal_Bool bEncrypted )
{
	DBG_ASSERT(xInputStream.is(), "input stream missing");
	DBG_ASSERT(xModelComponent.is(), "document missing");
	DBG_ASSERT(rFactory.is(), "factory missing");
	DBG_ASSERT(NULL != pFilterName,"I need a service name for the component!");

    RTL_LOGFILE_CONTEXT( aLog, "ReadThroughComponent" );

	// prepare ParserInputSrouce
	xml::sax::InputSource aParserInput;
	aParserInput.sSystemId = rName;
	aParserInput.aInputStream = xInputStream;

	// get parser
	Reference< xml::sax::XParser > xParser(
		rFactory->createInstance(
			OUString::createFromAscii("com.sun.star.xml.sax.Parser") ),
		UNO_QUERY );
	DBG_ASSERT( xParser.is(), "Can't create parser" );
	if( !xParser.is() )
		return SD_XML_READERROR;
    RTL_LOGFILE_CONTEXT_TRACE( aLog, "parser created" );

	// get filter
	Reference< xml::sax::XDocumentHandler > xFilter(
		rFactory->createInstanceWithArguments(
			OUString::createFromAscii(pFilterName), rFilterArguments),
		UNO_QUERY );
	DBG_ASSERT( xFilter.is(), "Can't instantiate filter component." );
	if( !xFilter.is() )
		return SD_XML_READERROR;
    RTL_LOGFILE_CONTEXT_TRACE1( aLog, "%s created", pFilterName );

	// connect parser and filter
	xParser->setDocumentHandler( xFilter );

	// connect model and filter
	Reference < XImporter > xImporter( xFilter, UNO_QUERY );
	xImporter->setTargetDocument( xModelComponent );
	// finally, parser the stream
    RTL_LOGFILE_CONTEXT_TRACE( aLog, "parsing stream" );
	try
	{
		xParser->parseStream( aParserInput );
	}
	catch( xml::sax::SAXParseException& r )
	{
		if( bEncrypted )
			return ERRCODE_SFX_WRONGPASSWORD;

#if OSL_DEBUG_LEVEL > 1
		ByteString aError( "SAX parse exception catched while importing:\n" );
		aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
		DBG_ERROR( aError.GetBuffer() );
#endif

		String sErr( String::CreateFromInt32( r.LineNumber ));
		sErr += ',';
		sErr += String::CreateFromInt32( r.ColumnNumber );

		if( rStreamName.Len() )
		{
			return *new TwoStringErrorInfo(
							(bMustBeSuccessfull ? ERR_FORMAT_FILE_ROWCOL
										   	 	: WARN_FORMAT_FILE_ROWCOL),
					    	rStreamName, sErr,
							ERRCODE_BUTTON_OK | ERRCODE_MSG_ERROR );
		}
		else
		{
			DBG_ASSERT( bMustBeSuccessfull, "Warnings are not supported" );
			return *new StringErrorInfo( ERR_FORMAT_ROWCOL, sErr,
							 ERRCODE_BUTTON_OK | ERRCODE_MSG_ERROR );
		}
	}
	catch( xml::sax::SAXException& r )
	{
		if( bEncrypted )
			return ERRCODE_SFX_WRONGPASSWORD;

#if OSL_DEBUG_LEVEL > 1
		ByteString aError( "SAX exception catched while importing:\n" );
		aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
		DBG_ERROR( aError.GetBuffer() );
#endif
		return SD_XML_READERROR;
	}
	catch( packages::zip::ZipIOException& r )
	{
#if OSL_DEBUG_LEVEL > 1
		ByteString aError( "Zip exception catched while importing:\n" );
		aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
		DBG_ERROR( aError.GetBuffer() );
#endif
		return ERRCODE_IO_BROKENPACKAGE;
	}
	catch( io::IOException& r )
	{
#if OSL_DEBUG_LEVEL > 1
		ByteString aError( "IO exception catched while importing:\n" );
		aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
		DBG_ERROR( aError.GetBuffer() );
#endif
		return SD_XML_READERROR;
	}
	catch( uno::Exception& r )
	{
#if OSL_DEBUG_LEVEL > 1
		ByteString aError( "uno exception catched while importing:\n" );
		aError += ByteString( String( r.Message), RTL_TEXTENCODING_ASCII_US );
		DBG_ERROR( aError.GetBuffer() );
#endif
		return SD_XML_READERROR;
	}

	// success!
	return 0;
}

sal_Int32 ReadThroughComponent(
	SvStorage* pStorage,
	Reference<XComponent> xModelComponent,
	const sal_Char* pStreamName,
	const sal_Char* pCompatibilityStreamName,
	Reference<lang::XMultiServiceFactory> & rFactory,
	const sal_Char* pFilterName,
	Sequence<Any> rFilterArguments,
	const OUString& rName,
	sal_Bool bMustBeSuccessfull )
{
	DBG_ASSERT(NULL != pStorage, "Need storage!");
	DBG_ASSERT(NULL != pStreamName, "Please, please, give me a name!");

	// open stream (and set parser input)
	OUString sStreamName = OUString::createFromAscii(pStreamName);
	if (! pStorage->IsStream(sStreamName))
	{
		// stream name not found! Then try the compatibility name.
		// if no stream can be opened, return immediatly with OK signal

		// do we even have an alternative name?
		if ( NULL == pCompatibilityStreamName )
			return 0;

		// if so, does the stream exist?
		sStreamName = OUString::createFromAscii(pCompatibilityStreamName);
		if (! pStorage->IsStream(sStreamName) )
			return 0;
	}

	// get input stream
	SvStorageStreamRef xEventsStream;
	xEventsStream = pStorage->OpenStream( sStreamName,
										  STREAM_READ | STREAM_NOCREATE );
	Any aAny;
	sal_Bool bEncrypted =
		xEventsStream->GetProperty(
				OUString( RTL_CONSTASCII_USTRINGPARAM("Encrypted") ), aAny ) &&
		aAny.getValueType() == ::getBooleanCppuType() &&
		*(sal_Bool *)aAny.getValue();

	Reference<io::XInputStream> xInputStream = xEventsStream->GetXInputStream();

	// read from the stream
	return ReadThroughComponent(
		xInputStream, xModelComponent, sStreamName, rFactory,
		pFilterName, rFilterArguments,
		rName, bMustBeSuccessfull, bEncrypted );
}

// -----------------------------------------------------------------------------

sal_Bool SdXMLFilter::Import()
{
    OSL_ASSERT("XML import removed");
  sal_uInt32  nRet = 0;
  return nRet == 0;
}

// -----------------------------------------------------------------------------

sal_Bool SdXMLFilter::Export()
{
#ifdef TIMELOG
    RTL_LOGFILE_CONTEXT_AUTHOR ( aLog, "sd", "cl93746", "SdXMLFilter::Export" );
	ByteString aFile( mrMedium.GetName(), RTL_TEXTENCODING_ASCII_US );
	RTL_LOGFILE_CONTEXT_TRACE1( aLog, "exporting %s", aFile.GetBuffer() );
#endif

	SvXMLEmbeddedObjectHelper*	pObjectHelper = NULL;
	SvXMLGraphicHelper*			pGraphicHelper = NULL;
	sal_Bool					bDocRet = FALSE;

	try
	{
		if( !mxModel.is() )
		{
			DBG_ERROR("Got NO Model in XMLExport");
			return FALSE;
		}

		uno::Reference< lang::XServiceInfo > xServiceInfo( mxModel, uno::UNO_QUERY );

        if( !xServiceInfo.is() || !xServiceInfo->supportsService( OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.drawing.GenericDrawingDocument" ) ) ) )
		{
			DBG_ERROR( "Model is no DrawingDocument in XMLExport" );
			return FALSE;
		}

		uno::Reference< lang::XMultiServiceFactory> xServiceFactory( ::legacy_binfilters::getLegacyProcessServiceFactory() );

		if( !xServiceFactory.is() )
		{
			DBG_ERROR( "got no service manager" );
			return FALSE;
		}

		uno::Reference< uno::XInterface > xWriter( xServiceFactory->createInstance( OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.xml.sax.Writer" ) ) ) );

		if( !xWriter.is() )
		{
			DBG_ERROR( "com.sun.star.xml.sax.Writer service missing" );
			return FALSE;
		}

		uno::Reference<xml::sax::XDocumentHandler>	xHandler( xWriter, uno::UNO_QUERY );

		/** property map for export info set */
		PropertyMapEntry aExportInfoMap[] =
		{
			// #82003#
			{ MAP_LEN( "ProgressRange" ),	0, &::getCppuType((const sal_Int32*)0), ::com::sun::star::beans::PropertyAttribute::MAYBEVOID, 0},
			{ MAP_LEN( "ProgressMax" ),		0, &::getCppuType((const sal_Int32*)0), ::com::sun::star::beans::PropertyAttribute::MAYBEVOID, 0},
			{ MAP_LEN( "ProgressCurrent" ),	0, &::getCppuType((const sal_Int32*)0), ::com::sun::star::beans::PropertyAttribute::MAYBEVOID, 0},
			{ MAP_LEN( "UsePrettyPrinting"),0, &::getBooleanCppuType(),				::com::sun::star::beans::PropertyAttribute::MAYBEVOID, 0},

			{ MAP_LEN( "PageLayoutNames" ), 0, SEQTYPE(::getCppuType((const OUString*)0)), 	::com::sun::star::beans::PropertyAttribute::MAYBEVOID,     0},
			{ NULL, 0, 0, NULL, 0, 0 }
		};

		uno::Reference< beans::XPropertySet > xInfoSet( GenericPropertySet_CreateInstance( new PropertySetInfo( aExportInfoMap ) ) );


		SvtSaveOptions aSaveOpt;
		OUString sUsePrettyPrinting(RTL_CONSTASCII_USTRINGPARAM("UsePrettyPrinting"));
		sal_Bool bUsePrettyPrinting( aSaveOpt.IsPrettyPrinting() );
		xInfoSet->setPropertyValue( sUsePrettyPrinting, makeAny( bUsePrettyPrinting ) );

		SvStorage* pStorage = mrMedium.GetOutputStorage( sal_True );

		// initialize descriptor
		uno::Sequence< beans::PropertyValue > aDescriptor( 1 );
		beans::PropertyValue* pProps = aDescriptor.getArray();

		pProps[0].Name = OUString( RTL_CONSTASCII_USTRINGPARAM( "FileName" ) );
		pProps[0].Value <<= OUString( mrMedium.GetName() );

		{
			uno::Reference< document::XEmbeddedObjectResolver > xObjectResolver;
			uno::Reference< document::XGraphicObjectResolver >	xGrfResolver;

			// create helper for graphic and ole export if we have a storage
			if( pStorage )
			{
				SvPersist *pPersist = mrDocShell.GetDoc()->GetPersist();
				if( pPersist )
				{
					pObjectHelper = SvXMLEmbeddedObjectHelper::Create( *pStorage, *pPersist, EMBEDDEDOBJECTHELPER_MODE_WRITE, sal_False );
					xObjectResolver = pObjectHelper;
				}

				pGraphicHelper = SvXMLGraphicHelper::Create( *pStorage, GRAPHICHELPER_MODE_WRITE, FALSE );
				xGrfResolver = pGraphicHelper;
			}

			// #82003#
			if(mbShowProgress)
			{
				CreateStatusIndicator();
				if(mxStatusIndicator.is())
				{
					sal_Int32 nProgressRange(1000000);
					sal_Int32 nProgressCurrent(0);
					OUString aMsg = String( SdResId( STR_SAVE_DOC ) );
					mxStatusIndicator->start(aMsg, nProgressRange);

					// set ProgressRange
					uno::Any aProgRange;
					aProgRange <<= nProgressRange;
					xInfoSet->setPropertyValue(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ProgressRange")), aProgRange);

					// set ProgressCurrent
					uno::Any aProgCurrent;
					aProgCurrent <<= nProgressCurrent;
					xInfoSet->setPropertyValue(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ProgressCurrent")), aProgCurrent);
				}
			}

			uno::Reference< lang::XComponent > xComponent( mxModel, uno::UNO_QUERY );

			XML_SERVICEMAP aServices[5]; sal_uInt16 i = 0;
			aServices[i  ].mpService = IsDraw() ? sXML_export_draw_styles_service : sXML_export_impress_styles_service;
			aServices[i  ].mpStream  = sXML_styleStreamName;
			aServices[i++].mbPlain = sal_False;

			aServices[i  ].mpService = IsDraw() ? sXML_export_draw_content_service : sXML_export_impress_content_service;
			aServices[i  ].mpStream  = sXML_contentStreamName;
			aServices[i++].mbPlain = sal_False;

			aServices[i  ].mpService = IsDraw() ? sXML_export_draw_settings_service : sXML_export_impress_settings_service;
			aServices[i  ].mpStream  = sXML_settingsStreamName;
			aServices[i++].mbPlain = sal_False;

			if( mrDocShell.GetCreateMode() != SFX_CREATE_MODE_EMBEDDED )
			{
				aServices[i  ].mpService = IsDraw() ? sXML_export_draw_meta_service : sXML_export_impress_meta_service;
				aServices[i  ].mpStream  = sXML_metaStreamName;
				aServices[i++].mbPlain = sal_True;
			};

			aServices[i].mpService = NULL;
			aServices[i].mpStream  = NULL;

			XML_SERVICEMAP* pServices = aServices;

			// doc export
			do
			{
				RTL_LOGFILE_CONTEXT_TRACE1( aLog, "exporting substream %s", pServices->mpStream );

				uno::Reference<io::XOutputStream> xDocOut;
				SvStorageStreamRef xDocStream;

				if( pStorage )
				{
					const OUString sDocName( OUString::createFromAscii( pServices->mpStream ) );
					xDocStream = pStorage->OpenStream( sDocName, STREAM_WRITE | STREAM_SHARE_DENYWRITE | STREAM_TRUNC  );
					DBG_ASSERT(xDocStream.Is(), "Can't create output stream in package!");
					if( !xDocStream.Is() )
						return sal_False;

					xDocStream->SetVersion( pStorage->GetVersion() );
//					xDocStream->SetKey( pStorage->GetKey() );
					xDocStream->SetBufferSize( 16*1024 );
					xDocOut = new ::utl::OOutputStreamWrapper( *xDocStream );

					uno::Any aAny; aAny <<= OUString( RTL_CONSTASCII_USTRINGPARAM("text/xml") );
					xDocStream->SetProperty(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("MediaType")), aAny);

					if( pServices->mbPlain )
					{
						xDocStream->SetProperty( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Compressed") ), uno::makeAny( (sal_Bool) sal_False ) );
					}
					else
					{
						xDocStream->SetProperty( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Encrypted") ), uno::makeAny( (sal_Bool)sal_True ) );
					}

				}

				uno::Reference< io::XActiveDataSource > xDocSrc( xWriter, uno::UNO_QUERY );
				xDocSrc->setOutputStream( xDocOut );

				uno::Sequence< uno::Any > aArgs( 2 + ( mxStatusIndicator.is() ? 1 : 0 ) + ( xGrfResolver.is() ? 1 : 0 ) + ( xObjectResolver.is() ? 1 : 0 ) );
				uno::Any* pArgs = aArgs.getArray();
				if( xGrfResolver.is() ) 		*pArgs++ <<= xGrfResolver;
				if( xObjectResolver.is() )		*pArgs++ <<= xObjectResolver;
				if( mxStatusIndicator.is() )	*pArgs++ <<= mxStatusIndicator;

				*pArgs++ <<= xInfoSet;
				*pArgs   <<= xHandler;

				uno::Reference< document::XFilter > xFilter( xServiceFactory->createInstanceWithArguments( OUString::createFromAscii( pServices->mpService ), aArgs ), uno::UNO_QUERY );
				if( xFilter.is() )
				{
					uno::Reference< document::XExporter > xExporter( xFilter, uno::UNO_QUERY );
					if( xExporter.is() )
					{
						xExporter->setSourceDocument( xComponent );

						bDocRet = xFilter->filter( aDescriptor );

						if(bDocRet && xDocStream.Is())
							xDocStream->Commit();
					}
				}

				pServices++;
			}
			while( bDocRet && pServices->mpService );

			// #82003#
			if(mbShowProgress)
			{
				if(mxStatusIndicator.is())
					mxStatusIndicator->end();
			}
		}
	}
	catch(uno::Exception e)
	{
#if OSL_DEBUG_LEVEL > 1
		ByteString aError( "uno Exception caught while exporting:\n" );
		aError += ByteString( String( e.Message), RTL_TEXTENCODING_ASCII_US );
		DBG_ERROR( aError.GetBuffer() );
#endif
		bDocRet = sal_False;
	}

	if( pGraphicHelper )
		SvXMLGraphicHelper::Destroy( pGraphicHelper );

	if( pObjectHelper )
		SvXMLEmbeddedObjectHelper::Destroy( pObjectHelper );


	return bDocRet;
}
}
