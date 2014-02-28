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

#include <rtl/ustrbuf.hxx>
#include <oox/token/tokens.hxx>
#include <oox/ole/vbaproject.hxx>
#include "pptexooxml.hxx"
#include <cppuhelper/factory.hxx>
#include <rtl/ustring.hxx>
#include <sax/fshelper.hxx>

using ::rtl::OString;
using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using namespace ::com::sun::star;
using namespace ::com::sun::star::animations;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::presentation;
using namespace ::com::sun::star::uno;
using namespace ::ppt;
using namespace ::oox;

using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::beans::XPropertySetInfo;
using ::com::sun::star::lang::XMultiServiceFactory;
using ::com::sun::star::container::XIndexAccess;
using ::com::sun::star::frame::XModel;
using ::com::sun::star::task::XStatusIndicator;
using ::com::sun::star::text::XSimpleText;
using ::sax_fastparser::FastSerializerHelper;
using ::sax_fastparser::FSHelperPtr;

using ::com::sun::star::beans::PropertyValue;
using ::com::sun::star::embed::XStorage;
using ::com::sun::star::io::XOutputStream;
using ::com::sun::star::io::XStream;
using ::com::sun::star::lang::XComponent;
using ::com::sun::star::lang::XMultiServiceFactory;
using ::com::sun::star::lang::XServiceInfo;
using ::com::sun::star::lang::XSingleServiceFactory;
using ::com::sun::star::registry::InvalidRegistryException;
using ::com::sun::star::registry::XRegistryKey;
using ::com::sun::star::uno::Exception;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::uno::XInterface;


// presentation namespaces
#define PRESENTATIONNASS FSNS( XML_xmlns, XML_a ), "http://schemas.openxmlformats.org/drawingml/2006/main", \
                      FSNS( XML_xmlns, XML_p ), "http://schemas.openxmlformats.org/presentationml/2006/main", \
                      FSNS( XML_xmlns, XML_r ), "http://schemas.openxmlformats.org/officeDocument/2006/relationships"


PPTXExport::PPTXExport( const Reference< XComponentContext >& xCtx )
    : XmlFilterBase( xCtx ),
     PPTEXPBase(),
     mnSlideIdMax( 1 << 8 ),
     mnSlideMasterIdMax( 1 << 31 ),
     mnAnimationNodeIdMax( 1 )
{}

PPTXExport::~PPTXExport()
{
}

bool PPTXExport::importDocument() throw()
{
    return false;
}

bool PPTXExport::exportDocument() throw()
{
    addRelation( CREATE_OUSTRING( "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" ), CREATE_OUSTRING( "ppt/presentation.xml" ) );

    mPresentationFS = openFragmentStreamWithSerializer( CREATE_OUSTRING( "ppt/presentation.xml" ),CREATE_OUSTRING( "application/vnd.openxmlformats-officedocument.presentationml.presentation.main+xml" ) );
    mPresentationFS->startElementNS( XML_p, XML_presentation, PRESENTATIONNASS, FSEND );

    mXModel.set( getModel(), UNO_QUERY );
    mXStatusIndicator.set( getStatusIndicator(), UNO_QUERY );

    exportPPT();
    
    mPresentationFS->endElementNS( XML_p, XML_presentation );
    mPresentationFS.reset();
    commitStorage();			
    return true;
}

sal_Bool PPTXExport::ImplCreateDocument()
{
    mbCreateNotes = sal_False;

    for( sal_uInt32 i = 0; i < mnPages; i++ )
    {
        if ( !ImplGetPageByIndex( i, NOTICE ) )
            return sal_False;

    if( ContainsOtherShapeThanPlaceholders( sal_True ) ) {
        mbCreateNotes = sal_True;
        break;
    }
    }

    return sal_True;
}

::oox::ole::VbaProject* PPTXExport::implCreateVbaProject() const
{
    return new ::oox::ole::VbaProject( getComponentContext(), getModel(), CREATE_OUSTRING( "Impress" ) );
}

void PPTXExport::ImplWriteSlide( sal_uInt32 nPageNum, sal_uInt32 nMasterNum, sal_uInt16 /* nMode */,
                                       sal_Bool bHasBackground, Reference< XPropertySet > aXBackgroundPropSet )
{
   
    // slides list
    if( nPageNum == 0 )
        mPresentationFS->startElementNS( XML_p, XML_sldIdLst, FSEND );

    // add explicit relation of presentation to this slide
    OUString sRelId = addRelation( mPresentationFS->getOutputStream(),
                                   CREATE_OUSTRING( "http://schemas.openxmlformats.org/officeDocument/2006/relationships/slide" ),
                                   OUStringBuffer()
                                   .appendAscii( "slides/slide" )
                                   .append( (sal_Int32) nPageNum + 1 )
                                   .appendAscii( ".xml" )
                                   .makeStringAndClear() );

    mPresentationFS->singleElementNS( XML_p, XML_sldId,
                                      XML_id, OString::valueOf( (sal_Int32) GetNewSlideId() ).getStr(),
                                      FSNS( XML_r, XML_id ),  OUStringToOString( sRelId, RTL_TEXTENCODING_UTF8 ).getStr(),
                                      FSEND );

    if( nPageNum == mnPages - 1 )
        mPresentationFS->endElementNS( XML_p, XML_sldIdLst );

    FSHelperPtr pFS = openFragmentStreamWithSerializer( OUStringBuffer()
                                                        .appendAscii( "ppt/slides/slide" )
                                                        .append( (sal_Int32) nPageNum + 1 )
                                                        .appendAscii( ".xml" )
                                                        .makeStringAndClear(),
                                                        CREATE_OUSTRING( "application/vnd.openxmlformats-officedocument.presentationml.slide+xml" ) );

    if( mpSlidesFSArray.size() < mnPages )
    mpSlidesFSArray.resize( mnPages );
    mpSlidesFSArray[ nPageNum ] = pFS;

    const char* pShow = NULL;

    if(ImplGetPropertyValue( mXPagePropSet, String( RTL_CONSTASCII_USTRINGPARAM( "Visible" ) ) ) ) {
    sal_Bool bShow(sal_False);
    if( ( mAny >>= bShow ) && !bShow )
        pShow = "0";
    }

    pFS->startElementNS( XML_p, XML_sld, PRESENTATIONNASS,
             XML_show, pShow,
             FSEND );

    pFS->startElementNS( XML_p, XML_cSld, FSEND );

    // background
 //   if( bHasBackground ) {
 //       ImplWriteBackground( pFS, aXBackgroundPropSet );
 //   }

 //   WriteShapeTree( pFS, NORMAL, sal_False );

    pFS->endElementNS( XML_p, XML_cSld );

//    WriteTransition( pFS );
//    WriteAnimations( pFS );

    pFS->endElementNS( XML_p, XML_sld );
}

// UNO stuff so that the filter is registered

#define PPTX_EXPORT_IMPL_NAME "com.sun.star.comp.Impress.OOXMLExporter"

OUString SdOOXMLExport_getImplementationName()
{
	return OUString( RTL_CONSTASCII_USTRINGPARAM( PPTX_EXPORT_IMPL_NAME ) );
}
::rtl::OUString PPTXExport::implGetImplementationName() const
{
    return CREATE_OUSTRING( "TODO" );
}

Sequence< OUString > SAL_CALL SdOOXMLExport_getSupportedServiceNames() throw()
{
	const OUString aServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.document.ExportFilter" ) );
	const Sequence< OUString > aSeq( &aServiceName, 1 );
	return aSeq;
}

Reference< XInterface > SAL_CALL SdOOXMLExport_createInstance(const Reference< XMultiServiceFactory > & rSMgr ) throw( Exception )
{
    Reference< XInterface > xRet;
    Reference< XComponentContext > xCtx;
    Reference< XPropertySet > const xProps( rSMgr, UNO_QUERY );
    if ( xProps.is() )
    {
        try 
        {
            xCtx.set( xProps->getPropertyValue( rtl::OUString::createFromAscii( "DefaultContext" ) ), UNO_QUERY );
        }
        catch ( UnknownPropertyException & e ) 
        {
        }
    }
    if ( xCtx.is() )
        xRet.set( (cppu::OWeakObject*) new PPTXExport( xCtx ) );
	return xRet;
}

#ifdef __cplusplus
extern "C"
{
#endif

	SAL_DLLPUBLIC_EXPORT void SAL_CALL component_getImplementationEnvironment( const sal_Char ** ppEnvTypeName, uno_Environment ** /* ppEnv */ )
	{
		*ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
	}

	SAL_DLLPUBLIC_EXPORT sal_Bool SAL_CALL component_writeInfo( void* /* pServiceManager */, void* pRegistryKey )
	{
		sal_Bool bRet = false;

		if ( pRegistryKey )
		{
			try
			{
				Reference< XRegistryKey > xNewKey1(
					static_cast< XRegistryKey* >( pRegistryKey )->createKey(
					OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.comp.Impress.OOXMLExporter/UNO/SERVICES/" )) ) );
				xNewKey1->createKey( SdOOXMLExport_getSupportedServiceNames().getConstArray()[0] );

				bRet = sal_True;
			}
			catch ( InvalidRegistryException& )
			{
				OSL_ENSURE( false, "### InvalidRegistryException!" );
			}
		}

		return bRet;
	}

SAL_DLLPUBLIC_EXPORT void* SAL_CALL component_getFactory( const sal_Char* pImplName, void* pServiceManager, void* /* pRegistryKey */ )
{
    Reference< XSingleServiceFactory > xFactory;
    void* pRet = 0;

    if ( rtl_str_compare( pImplName, PPTX_EXPORT_IMPL_NAME ) == 0 ) {
        const OUString aServiceName( OUString::createFromAscii( PPTX_EXPORT_IMPL_NAME ) );

        xFactory = Reference< XSingleServiceFactory >( ::cppu::createSingleFactory(
                    reinterpret_cast< XMultiServiceFactory* >( pServiceManager ),
                    SdOOXMLExport_getImplementationName(),
                    SdOOXMLExport_createInstance,
                    SdOOXMLExport_getSupportedServiceNames() ) );
    } 
   
    if ( xFactory.is() )
    {
        xFactory->acquire();
        pRet = xFactory.get();
    }

    return pRet;
}
	
#ifdef __cplusplus
}
#endif

