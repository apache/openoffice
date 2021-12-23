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
#include "precompiled_xmloff.hxx"

#ifdef PRECOMPILED
#include "filt_pch.hxx"
#endif
#include "unointerfacetouniqueidentifiermapper.hxx"
#include <osl/mutex.hxx>
#include <rtl/uuid.h>
#include <tools/debug.hxx>
#include <tools/urlobj.hxx>
#include <comphelper/genericpropertyset.hxx>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/document/XBinaryStreamResolver.hpp>
#include <com/sun/star/xml/sax/SAXInvalidCharacterException.hpp>
#include <com/sun/star/uri/XUriReferenceFactory.hpp>
#include <com/sun/star/uri/UriReferenceFactory.hpp>
#include <comphelper/processfactory.hxx>
#include <comphelper/configurationhelper.hxx>
#include <xmloff/attrlist.hxx>
#include <xmloff/nmspmap.hxx>
#include <xmloff/xmluconv.hxx>
#include "xmloff/xmlnmspe.hxx"
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmlexp.hxx>
#include <xmloff/xmlnumfe.hxx>
#include <xmloff/xmlmetae.hxx>
#include <xmloff/XMLSettingsExportContext.hxx>
#include <xmloff/families.hxx>
#include <xmloff/XMLEventExport.hxx>
#include "XMLStarBasicExportHandler.hxx"
#include "XMLScriptExportHandler.hxx"
#include <xmloff/SettingsExportHelper.hxx>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/container/XIndexContainer.hpp>
#include <com/sun/star/document/XEventsSupplier.hpp>
#include <com/sun/star/document/XViewDataSupplier.hpp>
#include <xmloff/GradientStyle.hxx>
#include <xmloff/HatchStyle.hxx>
#include <xmloff/ImageStyle.hxx>
#include <TransGradientStyle.hxx>
#include <xmloff/MarkerStyle.hxx>
#include <xmloff/DashStyle.hxx>
#include <xmloff/XMLFontAutoStylePool.hxx>
#include "XMLImageMapExport.hxx"
#include "XMLBase64Export.hxx"
#include "xmloff/xmlerror.hxx"
#include <com/sun/star/lang/ServiceNotRegisteredException.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include "xmloff/XMLFilterServiceNames.h"
#include "xmloff/XMLEmbeddedObjectExportFilter.hxx"
#include "XMLBasicExportFilter.hxx"
#include <vos/mutex.hxx>
#include <rtl/logfile.hxx>
#include <cppuhelper/implbase1.hxx>
#include <comphelper/extract.hxx>
#include "PropertySetMerger.hxx"

#include "svl/urihelper.hxx"
#include "xmloff/xformsexport.hxx"

#include <unotools/docinfohelper.hxx>
#include <unotools/bootstrap.hxx>
#include <unotools/configmgr.hxx>
#include <tools/inetdef.hxx>
#include <com/sun/star/document/XDocumentProperties.hpp>
#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>

#include <com/sun/star/rdf/XMetadatable.hpp>
#include "RDFaExportHelper.hxx"


using ::rtl::OUString;

using namespace ::osl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::io;
using namespace ::xmloff::token;

sal_Char __READONLY_DATA sXML_1_1[] = "1.1";
sal_Char __READONLY_DATA sXML_1_2[] = "1.2";

const sal_Char *sOpenOfficeOrgProject ="OpenOffice.org_project";

// #i115030#: the XSLT is not finished, and not available via HTTP
const sal_Char s_grddl_xsl[] = "http://FIXME";

#define LOGFILE_AUTHOR "mb93740"

#define XML_MODEL_SERVICE_WRITER	"com.sun.star.text.TextDocument"
#define XML_MODEL_SERVICE_CALC		"com.sun.star.sheet.SpreadsheetDocument"
#define XML_MODEL_SERVICE_DRAW		"com.sun.star.drawing.DrawingDocument"
#define XML_MODEL_SERVICE_IMPRESS	"com.sun.star.presentation.PresentationDocument"
#define XML_MODEL_SERVICE_MATH		"com.sun.star.formula.FormulaProperties"
#define XML_MODEL_SERVICE_CHART		"com.sun.star.chart.ChartDocument"

#define XML_USEPRETTYPRINTING       "UsePrettyPrinting"

#define C2U(cChar) OUString( RTL_CONSTASCII_USTRINGPARAM(cChar) )

struct XMLServiceMapEntry_Impl
{
	const sal_Char *sModelService;
	sal_Int32	   nModelServiceLen;
	const sal_Char *sFilterService;
	sal_Int32	   nFilterServiceLen;
};

#define SERVICE_MAP_ENTRY( app ) \
	{ XML_MODEL_SERVICE_##app, sizeof(XML_MODEL_SERVICE_##app)-1, \
	  XML_EXPORT_FILTER_##app, sizeof(XML_EXPORT_FILTER_##app)-1 }

const XMLServiceMapEntry_Impl aServiceMap[] =
{
	SERVICE_MAP_ENTRY( WRITER ),
	SERVICE_MAP_ENTRY( CALC ),
	SERVICE_MAP_ENTRY( IMPRESS ),// Impress supports DrawingDocument, too, so
	SERVICE_MAP_ENTRY( DRAW ),	 // it must appear before Draw
	SERVICE_MAP_ENTRY( MATH ),
	SERVICE_MAP_ENTRY( CHART ),
	{ 0, 0, 0, 0 }
};

//==============================================================================

class SAL_DLLPRIVATE SettingsExportFacade : public ::xmloff::XMLSettingsExportContext
{
public:
    SettingsExportFacade( SvXMLExport& i_rExport )
        :m_rExport( i_rExport )
    {
    }

    virtual ~SettingsExportFacade()
    {
    }

    virtual void    AddAttribute( enum ::xmloff::token::XMLTokenEnum i_eName,
				                  const ::rtl::OUString& i_rValue );
    virtual void    AddAttribute( enum ::xmloff::token::XMLTokenEnum i_eName,
				                  enum ::xmloff::token::XMLTokenEnum i_eValue );

    virtual void    StartElement( enum ::xmloff::token::XMLTokenEnum i_eName,
                                  const sal_Bool i_bIgnoreWhitespace );
    virtual void    EndElement(   const sal_Bool i_bIgnoreWhitespace );

    virtual void    Characters( const ::rtl::OUString& i_rCharacters );

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
                    GetServiceFactory() const;
private:
    SvXMLExport&                    m_rExport;
    ::std::stack< ::rtl::OUString > m_aElements;
};

void SettingsExportFacade::AddAttribute( enum ::xmloff::token::XMLTokenEnum i_eName, const ::rtl::OUString& i_rValue )
{
    m_rExport.AddAttribute( XML_NAMESPACE_CONFIG, i_eName, i_rValue );
}

void SettingsExportFacade::AddAttribute( enum ::xmloff::token::XMLTokenEnum i_eName, enum ::xmloff::token::XMLTokenEnum i_eValue )
{
    m_rExport.AddAttribute( XML_NAMESPACE_CONFIG, i_eName, i_eValue );
}

void SettingsExportFacade::StartElement( enum ::xmloff::token::XMLTokenEnum i_eName, const sal_Bool i_bIgnoreWhitespace )
{
    const ::rtl::OUString sElementName( m_rExport.GetNamespaceMap().GetQNameByKey( XML_NAMESPACE_CONFIG, GetXMLToken( i_eName ) ) );
    m_rExport.StartElement( sElementName, i_bIgnoreWhitespace );
    m_aElements.push( sElementName );
}

void SettingsExportFacade::EndElement( const sal_Bool i_bIgnoreWhitespace )
{
    const ::rtl::OUString sElementName( m_aElements.top() );
    m_rExport.EndElement( sElementName, i_bIgnoreWhitespace );
    m_aElements.pop();
}

void SettingsExportFacade::Characters( const ::rtl::OUString& i_rCharacters )
{
    m_rExport.GetDocHandler()->characters( i_rCharacters );
}

Reference< XMultiServiceFactory > SettingsExportFacade::GetServiceFactory() const
{
    return m_rExport.getServiceFactory();
}

//==============================================================================

class SvXMLExportEventListener : public cppu::WeakImplHelper1<
							com::sun::star::lang::XEventListener >
{
private:
	SvXMLExport*	pExport;

public:
							SvXMLExportEventListener(SvXMLExport* pExport);
	virtual					~SvXMLExportEventListener();

							// XEventListener
	virtual void SAL_CALL disposing(const lang::EventObject& rEventObject) throw(::com::sun::star::uno::RuntimeException);
};

SvXMLExportEventListener::SvXMLExportEventListener(SvXMLExport* pTempExport)
	: pExport(pTempExport)
{
}

SvXMLExportEventListener::~SvXMLExportEventListener()
{
}

// XEventListener
void SAL_CALL SvXMLExportEventListener::disposing( const lang::EventObject& )
	throw(uno::RuntimeException)
{
	if (pExport)
	{
		pExport->DisposingModel();
		pExport = NULL;
	}
}

//==============================================================================

class SvXMLExport_Impl
{
public:
    SvXMLExport_Impl();

    ::comphelper::UnoInterfaceToUniqueIdentifierMapper  maInterfaceToIdentifierMapper;
    uno::Reference< uri::XUriReferenceFactory >         mxUriReferenceFactory;
    rtl::OUString                                       msPackageURI;
    rtl::OUString                                       msPackageURIScheme;
    // --> OD 2006-09-27 #i69627#
    sal_Bool                                            mbOutlineStyleAsNormalListStyle;
    // <--
    // --> PB 2007-07-06 #i146851#
    sal_Bool                                            mbSaveBackwardCompatibleODF;
    // <--

	uno::Reference< embed::XStorage >					mxTargetStorage;

	SvtSaveOptions										maSaveOptions;

    /// relative path of stream in package, e.g. "someobject/content.xml"
    ::rtl::OUString mStreamPath;

    const uno::Reference< uno::XComponentContext > mxComponentContext;

    /// name of stream in package, e.g., "content.xml"
    ::rtl::OUString mStreamName;

    /// stack of backed up namespace maps
    /// long: depth at which namespace map has been backed up into the stack
    ::std::stack< ::std::pair< SvXMLNamespaceMap *, long > > mNamespaceMaps;
    /// counts depth (number of open elements/start tags)
    long mDepth;

    ::std::auto_ptr< ::xmloff::RDFaExportHelper> mpRDFaHelper;

    // --> OD 2008-11-26 #158694#
    sal_Bool                                            mbExportTextNumberElement;
    // <--
    sal_Bool                                            mbNullDateInitialized;

    void SetSchemeOf( const ::rtl::OUString& rOrigFileName )
    {
        sal_Int32 nSep = rOrigFileName.indexOf(':');
        if( nSep != -1 )
            msPackageURIScheme = rOrigFileName.copy( 0, nSep );
    }
};

SvXMLExport_Impl::SvXMLExport_Impl()
    // --> OD 2006-09-27 #i69627#
    : mbOutlineStyleAsNormalListStyle( false )
    // <--
    // --> PB 2007-07-06 #i146851#
        ,mbSaveBackwardCompatibleODF( sal_True )
    // <--
        ,mxComponentContext( ::comphelper::getProcessComponentContext() )
        ,mStreamName()
        ,mNamespaceMaps()
        ,mDepth(0)
        ,mpRDFaHelper() // lazy
    // --> OD 2008-11-26 #158694#
        ,mbExportTextNumberElement( sal_False )
    // <--
        ,mbNullDateInitialized( sal_False )
{
    OSL_ENSURE(mxComponentContext.is(), "SvXMLExport: no ComponentContext");
    if (!mxComponentContext.is()) throw uno::RuntimeException();
    mxUriReferenceFactory = uri::UriReferenceFactory::create(
        mxComponentContext );
}

//==============================================================================

void SvXMLExport::SetDocHandler( const uno::Reference< xml::sax::XDocumentHandler > &rHandler )
{
	mxHandler = rHandler;
	mxExtHandler = uno::Reference<xml::sax::XExtendedDocumentHandler>( mxHandler, UNO_QUERY );
}

void SvXMLExport::_InitCtor()
{
    // note: it is not necessary to add XML_NP_XML (it is declared implicitly)
	if( (getExportFlags() & ~EXPORT_OASIS) != 0 )
	{
		mpNamespaceMap->Add( GetXMLToken(XML_NP_OFFICE), GetXMLToken(XML_N_OFFICE), XML_NAMESPACE_OFFICE );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_OOO), GetXMLToken(XML_N_OOO), XML_NAMESPACE_OOO );
	}
	if( (getExportFlags() & (EXPORT_STYLES|EXPORT_MASTERSTYLES|EXPORT_AUTOSTYLES|EXPORT_FONTDECLS) ) != 0 )
	{
		mpNamespaceMap->Add( GetXMLToken(XML_NP_FO), GetXMLToken(XML_N_FO_COMPAT), XML_NAMESPACE_FO );
	}
	if( (getExportFlags() & (EXPORT_META|EXPORT_STYLES|EXPORT_MASTERSTYLES|EXPORT_AUTOSTYLES|EXPORT_CONTENT|EXPORT_SCRIPTS|EXPORT_SETTINGS) ) != 0 )
	{
		mpNamespaceMap->Add( GetXMLToken(XML_NP_XLINK), GetXMLToken(XML_N_XLINK), XML_NAMESPACE_XLINK );
	}
	if( (getExportFlags() & EXPORT_SETTINGS) != 0 )
	{
		mpNamespaceMap->Add( GetXMLToken(XML_NP_CONFIG), GetXMLToken(XML_N_CONFIG), XML_NAMESPACE_CONFIG );
	}

	if( (getExportFlags() & (EXPORT_META|EXPORT_MASTERSTYLES|EXPORT_CONTENT) ) != 0 )
	{
		mpNamespaceMap->Add( GetXMLToken(XML_NP_DC), GetXMLToken(XML_N_DC), XML_NAMESPACE_DC );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_META), GetXMLToken(XML_N_META), XML_NAMESPACE_META );
	}
	if( (getExportFlags() & (EXPORT_STYLES|EXPORT_MASTERSTYLES|EXPORT_AUTOSTYLES|EXPORT_CONTENT|EXPORT_FONTDECLS) ) != 0 )
	{
		mpNamespaceMap->Add( GetXMLToken(XML_NP_STYLE), GetXMLToken(XML_N_STYLE), XML_NAMESPACE_STYLE );
	}

	// namespaces for documents
	if( (getExportFlags() & (EXPORT_STYLES|EXPORT_AUTOSTYLES|EXPORT_MASTERSTYLES|EXPORT_CONTENT) ) != 0 )
	{
		mpNamespaceMap->Add( GetXMLToken(XML_NP_DC),    GetXMLToken(XML_N_DC),      XML_NAMESPACE_DC );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_TEXT),  GetXMLToken(XML_N_TEXT),    XML_NAMESPACE_TEXT );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_DRAW),  GetXMLToken(XML_N_DRAW),    XML_NAMESPACE_DRAW );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_DR3D),  GetXMLToken(XML_N_DR3D),    XML_NAMESPACE_DR3D );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_SVG),   GetXMLToken(XML_N_SVG_COMPAT),  XML_NAMESPACE_SVG );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_CHART), GetXMLToken(XML_N_CHART),   XML_NAMESPACE_CHART );
        mpNamespaceMap->Add( GetXMLToken(XML_NP_RPT),   GetXMLToken(XML_N_RPT),     XML_NAMESPACE_REPORT );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_TABLE), GetXMLToken(XML_N_TABLE),   XML_NAMESPACE_TABLE );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_NUMBER),GetXMLToken(XML_N_NUMBER),  XML_NAMESPACE_NUMBER );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_OOOW),  GetXMLToken(XML_N_OOOW),    XML_NAMESPACE_OOOW );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_OOOC),  GetXMLToken(XML_N_OOOC),    XML_NAMESPACE_OOOC );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_OF),    GetXMLToken(XML_N_OF),      XML_NAMESPACE_OF );

        if (getDefaultVersion() > SvtSaveOptions::ODFVER_012)
        {
            mpNamespaceMap->Add(
                GetXMLToken(XML_NP_TABLE_EXT), GetXMLToken(XML_N_TABLE_EXT), XML_NAMESPACE_TABLE_EXT);
        }
	}
	if( (getExportFlags() & (EXPORT_MASTERSTYLES|EXPORT_CONTENT) ) != 0 )
	{
		mpNamespaceMap->Add( GetXMLToken(XML_NP_MATH), GetXMLToken(XML_N_MATH), XML_NAMESPACE_MATH );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_FORM), GetXMLToken(XML_N_FORM), XML_NAMESPACE_FORM );
	}
	if( (getExportFlags() & (EXPORT_STYLES|EXPORT_AUTOSTYLES|EXPORT_MASTERSTYLES|EXPORT_CONTENT|EXPORT_SCRIPTS) ) != 0 )
	{
		mpNamespaceMap->Add( GetXMLToken(XML_NP_SCRIPT), GetXMLToken(XML_N_SCRIPT), XML_NAMESPACE_SCRIPT );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_DOM), GetXMLToken(XML_N_DOM), XML_NAMESPACE_DOM );
	}
	if( (getExportFlags() & EXPORT_CONTENT ) != 0 )
	{
		mpNamespaceMap->Add( GetXMLToken(XML_NP_XFORMS_1_0), GetXMLToken(XML_N_XFORMS_1_0), XML_NAMESPACE_XFORMS );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_XSD), GetXMLToken(XML_N_XSD), XML_NAMESPACE_XSD );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_XSI), GetXMLToken(XML_N_XSI), XML_NAMESPACE_XSI );
		mpNamespaceMap->Add( GetXMLToken(XML_NP_FIELD), GetXMLToken(XML_N_FIELD), XML_NAMESPACE_FIELD );
	}
    // RDFa: needed for content and header/footer styles
    if( (getExportFlags() & (EXPORT_STYLES|EXPORT_AUTOSTYLES|EXPORT_MASTERSTYLES|EXPORT_CONTENT) ) != 0 )
    {
        mpNamespaceMap->Add( GetXMLToken(XML_NP_XHTML),
            GetXMLToken(XML_N_XHTML), XML_NAMESPACE_XHTML );
    }
    // GRDDL: to convert RDFa and meta.xml to RDF
    if( (getExportFlags() & (EXPORT_META|EXPORT_STYLES|EXPORT_AUTOSTYLES|EXPORT_MASTERSTYLES|EXPORT_CONTENT) ) != 0 )
    {
        mpNamespaceMap->Add( GetXMLToken(XML_NP_GRDDL),
            GetXMLToken(XML_N_GRDDL), XML_NAMESPACE_GRDDL );
    }

	mxAttrList = (xml::sax::XAttributeList*)mpAttrList;

	msPicturesPath = OUString( RTL_CONSTASCII_USTRINGPARAM( "#Pictures/" ) );
	msObjectsPath = OUString( RTL_CONSTASCII_USTRINGPARAM( "#./" ) );
	msGraphicObjectProtocol = OUString( RTL_CONSTASCII_USTRINGPARAM( "vnd.sun.star.GraphicObject:" ) );
	msEmbeddedObjectProtocol = OUString( RTL_CONSTASCII_USTRINGPARAM( "vnd.sun.star.EmbeddedObject:" ) );

	if (mxModel.is() && !mxEventListener.is())
	{
		mxEventListener.set( new SvXMLExportEventListener(this));
		mxModel->addEventListener(mxEventListener);
	}

    // --> OD 2006-03-10 #i51726# - determine model type
    _DetermineModelType();
    // <--

	mbEnableExperimentalOdfExport = getenv("ENABLE_EXPERIMENTAL_ODF_EXPORT") != NULL;

    // --> PB 2007-07-06 #146851# - load mbSaveBackwardCompatibleODF from configuration

	// cl: but only if we do export to current oasis format, old openoffice format *must* always be compatible
	if( (getExportFlags() & EXPORT_OASIS) != 0 )
	{
		sal_Bool bTemp = sal_True;
		if ( ::comphelper::ConfigurationHelper::readDirectKey(
				getServiceFactory(),
				C2U("org.openoffice.Office.Common/"), C2U("Save/Document"), C2U("SaveBackwardCompatibleODF"),
				::comphelper::ConfigurationHelper::E_READONLY ) >>= bTemp )
		{
			mpImpl->mbSaveBackwardCompatibleODF = bTemp;
		}
	}
    // <--
}

// --> OD 2006-03-14 #i51726#
void SvXMLExport::_DetermineModelType()
{
    meModelType = SvtModuleOptions::E_UNKNOWN_FACTORY;

    if ( mxModel.is() )
    {
        meModelType = SvtModuleOptions::ClassifyFactoryByModel( mxModel );
    }
}
// <--

// #110680#
SvXMLExport::SvXMLExport(
	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
	MapUnit eDfltUnit, const enum XMLTokenEnum eClass, sal_uInt16 nExportFlags )
:	mpImpl( new SvXMLExport_Impl ),
	// #110680#
	mxServiceFactory(xServiceFactory),
	mpAttrList( new SvXMLAttributeList ),
	mpNamespaceMap( new SvXMLNamespaceMap ),
	// #110680#
	mpUnitConv( new SvXMLUnitConverter( MAP_100TH_MM, eDfltUnit, getServiceFactory() ) ),
	mpNumExport(0L),
	mpProgressBarHelper( NULL ),
	mpEventExport( NULL ),
	mpImageMapExport( NULL ),
	mpXMLErrors( NULL ),
	mbExtended( sal_False ),
	meClass( eClass ),
	mnExportFlags( nExportFlags ),
	mnErrorFlags( ERROR_NO ),
	msWS( GetXMLToken(XML_WS) ),
	mbSaveLinkedSections(sal_True)
{
	DBG_ASSERT( mxServiceFactory.is(), "got no service manager" );
	_InitCtor();
}

// #110680#
SvXMLExport::SvXMLExport(
	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
	const OUString &rFileName,
	const uno::Reference< xml::sax::XDocumentHandler > & rHandler,
	MapUnit eDfltUnit	)
:	mpImpl( new SvXMLExport_Impl ),
	// #110680#
	mxServiceFactory(xServiceFactory),
	mxHandler( rHandler ),
	mxExtHandler( rHandler, uno::UNO_QUERY ),
	mpAttrList( new SvXMLAttributeList ),
	msOrigFileName( rFileName ),
	mpNamespaceMap( new SvXMLNamespaceMap ),
	// #110680#
	mpUnitConv( new SvXMLUnitConverter( MAP_100TH_MM, eDfltUnit, getServiceFactory() ) ),
	mpNumExport(0L),
	mpProgressBarHelper( NULL ),
	mpEventExport( NULL ),
	mpImageMapExport( NULL ),
	mpXMLErrors( NULL ),
	mbExtended( sal_False ),
	meClass( XML_TOKEN_INVALID ),
	mnExportFlags( 0 ),
	mnErrorFlags( ERROR_NO ),
	msWS( GetXMLToken(XML_WS) ),
	mbSaveLinkedSections(sal_True)
{
    mpImpl->SetSchemeOf( msOrigFileName );
    DBG_ASSERT( mxServiceFactory.is(), "got no service manager" );
	_InitCtor();

	if (mxNumberFormatsSupplier.is())
		mpNumExport = new SvXMLNumFmtExport(*this, mxNumberFormatsSupplier);
}

// #110680#
SvXMLExport::SvXMLExport(
	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
	const OUString &rFileName,
	const uno::Reference< xml::sax::XDocumentHandler > & rHandler,
	const Reference< XModel >& rModel,
	sal_Int16 eDfltUnit	)
:	mpImpl( new SvXMLExport_Impl ),
	// #110680#
	mxServiceFactory(xServiceFactory),
	mxModel( rModel ),
	mxHandler( rHandler ),
	mxExtHandler( rHandler, uno::UNO_QUERY ),
	mxNumberFormatsSupplier (rModel, uno::UNO_QUERY),
	mpAttrList( new SvXMLAttributeList ),
	msOrigFileName( rFileName ),
	mpNamespaceMap( new SvXMLNamespaceMap ),
	// #110680#
	// pUnitConv( new SvXMLUnitConverter( MAP_100TH_MM, SvXMLUnitConverter::GetMapUnit(eDfltUnit) ) ),
	mpUnitConv( new SvXMLUnitConverter( MAP_100TH_MM, SvXMLUnitConverter::GetMapUnit(eDfltUnit), getServiceFactory() ) ),
	mpNumExport(0L),
	mpProgressBarHelper( NULL ),
	mpEventExport( NULL ),
	mpImageMapExport( NULL ),
	mpXMLErrors( NULL ),
	mbExtended( sal_False ),
	meClass( XML_TOKEN_INVALID ),
	mnExportFlags( 0 ),
	mnErrorFlags( ERROR_NO ),
	msWS( GetXMLToken(XML_WS) ),
	mbSaveLinkedSections(sal_True)
{
    mpImpl->SetSchemeOf( msOrigFileName );
	DBG_ASSERT( mxServiceFactory.is(), "got no service manager" );
	_InitCtor();

	if (mxNumberFormatsSupplier.is())
		mpNumExport = new SvXMLNumFmtExport(*this, mxNumberFormatsSupplier);
}

// #110680#
SvXMLExport::SvXMLExport(
	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
	const OUString &rFileName,
	const uno::Reference< xml::sax::XDocumentHandler > & rHandler,
	const Reference< XModel >& rModel,
	const Reference< document::XGraphicObjectResolver >& rEmbeddedGraphicObjects,
	sal_Int16 eDfltUnit	)
:	mpImpl( new SvXMLExport_Impl ),
	// #110680#
	mxServiceFactory(xServiceFactory),
	mxModel( rModel ),
	mxHandler( rHandler ),
	mxExtHandler( rHandler, uno::UNO_QUERY ),
	mxNumberFormatsSupplier (rModel, uno::UNO_QUERY),
	mxGraphicResolver( rEmbeddedGraphicObjects ),
	mpAttrList( new SvXMLAttributeList ),
	msOrigFileName( rFileName ),
	mpNamespaceMap( new SvXMLNamespaceMap ),
	// #110680#
	mpUnitConv( new SvXMLUnitConverter( MAP_100TH_MM, SvXMLUnitConverter::GetMapUnit(eDfltUnit), getServiceFactory() ) ),
	mpNumExport(0L),
	mpProgressBarHelper( NULL ),
	mpEventExport( NULL ),
	mpImageMapExport( NULL ),
	mpXMLErrors( NULL ),
	mbExtended( sal_False ),
	meClass( XML_TOKEN_INVALID ),
	mnExportFlags( 0 ),
	mnErrorFlags( ERROR_NO ),
	msWS( GetXMLToken(XML_WS) ),
	mbSaveLinkedSections(sal_True)
{
    mpImpl->SetSchemeOf( msOrigFileName );
	DBG_ASSERT( mxServiceFactory.is(), "got no service manager" );
	_InitCtor();

	if (mxNumberFormatsSupplier.is())
		mpNumExport = new SvXMLNumFmtExport(*this, mxNumberFormatsSupplier);
}

SvXMLExport::~SvXMLExport()
{
	delete mpXMLErrors;
	delete mpImageMapExport;
    delete mpEventExport;
	delete mpNamespaceMap;
	delete mpUnitConv;
	if (mpProgressBarHelper || mpNumExport)
	{
		if (mxExportInfo.is())
		{
			uno::Reference< beans::XPropertySetInfo > xPropertySetInfo = mxExportInfo->getPropertySetInfo();
			if (xPropertySetInfo.is())
			{
				if (mpProgressBarHelper)
				{
					OUString sProgressMax(RTL_CONSTASCII_USTRINGPARAM(XML_PROGRESSMAX));
					OUString sProgressCurrent(RTL_CONSTASCII_USTRINGPARAM(XML_PROGRESSCURRENT));
                    OUString sRepeat(RTL_CONSTASCII_USTRINGPARAM(XML_PROGRESSREPEAT));
					if (xPropertySetInfo->hasPropertyByName(sProgressMax) &&
						xPropertySetInfo->hasPropertyByName(sProgressCurrent))
					{
						sal_Int32 nProgressMax(mpProgressBarHelper->GetReference());
						sal_Int32 nProgressCurrent(mpProgressBarHelper->GetValue());
						uno::Any aAny;
						aAny <<= nProgressMax;
						mxExportInfo->setPropertyValue(sProgressMax, aAny);
						aAny <<= nProgressCurrent;
						mxExportInfo->setPropertyValue(sProgressCurrent, aAny);
					}
                    if (xPropertySetInfo->hasPropertyByName(sRepeat))
                        mxExportInfo->setPropertyValue(sRepeat, cppu::bool2any(mpProgressBarHelper->GetRepeat()));
				}
				if (mpNumExport && (mnExportFlags & (EXPORT_AUTOSTYLES | EXPORT_STYLES)))
				{
					OUString sWrittenNumberFormats(RTL_CONSTASCII_USTRINGPARAM(XML_WRITTENNUMBERSTYLES));
					if (xPropertySetInfo->hasPropertyByName(sWrittenNumberFormats))
					{
						uno::Sequence<sal_Int32> aWasUsed;
						mpNumExport->GetWasUsed(aWasUsed);
						uno::Any aAny;
						aAny <<= aWasUsed;
						mxExportInfo->setPropertyValue(sWrittenNumberFormats, aAny);
					}
				}
			}
		}
		delete mpProgressBarHelper;
		delete mpNumExport;
	}

	xmloff::token::ResetTokens();

	if (mxEventListener.is() && mxModel.is())
		mxModel->removeEventListener(mxEventListener);

	delete mpImpl;
}

///////////////////////////////////////////////////////////////////////

// XExporter
void SAL_CALL SvXMLExport::setSourceDocument( const uno::Reference< lang::XComponent >& xDoc )
	throw(lang::IllegalArgumentException, uno::RuntimeException)
{
	mxModel = uno::Reference< frame::XModel >::query( xDoc );
	if( !mxModel.is() )
		throw lang::IllegalArgumentException();
	if (mxModel.is() && ! mxEventListener.is())
	{
		mxEventListener.set( new SvXMLExportEventListener(this));
		mxModel->addEventListener(mxEventListener);
	}

	if(!mxNumberFormatsSupplier.is() )
	{
		mxNumberFormatsSupplier = mxNumberFormatsSupplier.query( mxModel );
		if(mxNumberFormatsSupplier.is() && mxHandler.is())
			mpNumExport = new SvXMLNumFmtExport(*this, mxNumberFormatsSupplier);
	}
	if (mxExportInfo.is())
	{
		uno::Reference< beans::XPropertySetInfo > xPropertySetInfo = mxExportInfo->getPropertySetInfo();
		if (xPropertySetInfo.is())
		{
			OUString sUsePrettyPrinting(RTL_CONSTASCII_USTRINGPARAM(XML_USEPRETTYPRINTING));
			if (xPropertySetInfo->hasPropertyByName(sUsePrettyPrinting))
			{
				uno::Any aAny = mxExportInfo->getPropertyValue(sUsePrettyPrinting);
				try
				{
					if (::cppu::any2bool(aAny))
						mnExportFlags |= EXPORT_PRETTY;
					else
						mnExportFlags &= ~EXPORT_PRETTY;
				}
				catch ( lang::IllegalArgumentException& )
				{
					DBG_ERRORFILE("why is bUsePrettyPrint not boolean?");
				}
			}

            if (mpNumExport && (mnExportFlags & (EXPORT_AUTOSTYLES | EXPORT_STYLES)))
			{
				OUString sWrittenNumberFormats(RTL_CONSTASCII_USTRINGPARAM(XML_WRITTENNUMBERSTYLES));
				if (xPropertySetInfo->hasPropertyByName(sWrittenNumberFormats))
				{
					uno::Any aAny = mxExportInfo->getPropertyValue(sWrittenNumberFormats);
					uno::Sequence<sal_Int32> aWasUsed;
					if(aAny >>= aWasUsed)
						mpNumExport->SetWasUsed(aWasUsed);
				}
			}
		}
	}

    // --> PB 2007-07-06 #i146851#
    if ( mpImpl->mbSaveBackwardCompatibleODF )
        mnExportFlags |= EXPORT_SAVEBACKWARDCOMPATIBLE;
    else
        mnExportFlags &= ~EXPORT_SAVEBACKWARDCOMPATIBLE;
    // <--

	// namespaces for user defined attributes
	Reference< XMultiServiceFactory > xFactory( mxModel,	UNO_QUERY );
	if( xFactory.is() )
	{
		try
		{
			Reference < XInterface > xIfc =
				xFactory->createInstance(OUString(RTL_CONSTASCII_USTRINGPARAM(
											"com.sun.star.xml.NamespaceMap")) );
			if( xIfc.is() )
			{
				Reference< XNameAccess > xNamespaceMap( xIfc, UNO_QUERY );
				if( xNamespaceMap.is() )
				{
					Sequence< OUString > aPrefixes( xNamespaceMap->getElementNames() );

					OUString* pPrefix = aPrefixes.getArray();
					const sal_Int32 nCount = aPrefixes.getLength();
					sal_Int32 nIndex;
					OUString aURL;

					for( nIndex = 0; nIndex < nCount; ++nIndex, ++pPrefix )
					{
						if( xNamespaceMap->getByName( *pPrefix ) >>= aURL )
							_GetNamespaceMap().Add( *pPrefix, aURL, XML_NAMESPACE_UNKNOWN );
					}
				}
			}
		}
		catch( com::sun::star::uno::Exception& )
		{
		}
	}

    // --> OD 2006-03-10 #i51726# - determine model type
    _DetermineModelType();
    // <--
}

// XInitialize
void SAL_CALL SvXMLExport::initialize( const uno::Sequence< uno::Any >& aArguments )
	throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException)
{
    // #93186# we need to queryInterface every single Any with any expected outcome. This variable hold the queryInterface results.

	const sal_Int32 nAnyCount = aArguments.getLength();
	const uno::Any* pAny = aArguments.getConstArray();

	for( sal_Int32 nIndex = 0; nIndex < nAnyCount; nIndex++, pAny++ )
	{
        Reference<XInterface> xValue;
        *pAny >>= xValue;

        // status indicator
        uno::Reference<task::XStatusIndicator> xTmpStatus( xValue, UNO_QUERY );
        if ( xTmpStatus.is() )
			mxStatusIndicator = xTmpStatus;

        // graphic resolver
        uno::Reference<document::XGraphicObjectResolver> xTmpGraphic(
            xValue, UNO_QUERY );
        if ( xTmpGraphic.is() )
            mxGraphicResolver = xTmpGraphic;

        // object resolver
        uno::Reference<document::XEmbeddedObjectResolver> xTmpObjectResolver(
            xValue, UNO_QUERY );
        if ( xTmpObjectResolver.is() )
            mxEmbeddedResolver = xTmpObjectResolver;

        // document handler
        uno::Reference<xml::sax::XDocumentHandler> xTmpDocHandler(
            xValue, UNO_QUERY );
        if( xTmpDocHandler.is() )
		{
			mxHandler = xTmpDocHandler;
			*pAny >>= mxExtHandler;

			if (mxNumberFormatsSupplier.is() && mpNumExport == NULL)
				mpNumExport = new SvXMLNumFmtExport(*this, mxNumberFormatsSupplier);
		}

        // property set to transport data across
        uno::Reference<beans::XPropertySet> xTmpPropertySet(
            xValue, UNO_QUERY );
        if( xTmpPropertySet.is() )
			mxExportInfo = xTmpPropertySet;
	}

	if( mxExportInfo.is() )
	{
		uno::Reference< beans::XPropertySetInfo > xPropertySetInfo =
			mxExportInfo->getPropertySetInfo();
		OUString sPropName(
				RTL_CONSTASCII_USTRINGPARAM("BaseURI" ) );
		if( xPropertySetInfo->hasPropertyByName(sPropName) )
		{
			uno::Any aAny = mxExportInfo->getPropertyValue(sPropName);
			aAny >>= msOrigFileName;
            mpImpl->msPackageURI = msOrigFileName;
            mpImpl->SetSchemeOf( msOrigFileName );
		}
		OUString sRelPath;
		sPropName = OUString( RTL_CONSTASCII_USTRINGPARAM("StreamRelPath" ) );
		if( xPropertySetInfo->hasPropertyByName(sPropName) )
		{
			uno::Any aAny = mxExportInfo->getPropertyValue(sPropName);
			aAny >>= sRelPath;
		}
		OUString sName;
		sPropName = OUString( RTL_CONSTASCII_USTRINGPARAM("StreamName" ) );
		if( xPropertySetInfo->hasPropertyByName(sPropName) )
		{
			uno::Any aAny = mxExportInfo->getPropertyValue(sPropName);
			aAny >>= sName;
		}
		if( msOrigFileName.getLength() && sName.getLength() )
		{
			INetURLObject aBaseURL( msOrigFileName );
			if( sRelPath.getLength() )
				aBaseURL.insertName( sRelPath );
			aBaseURL.insertName( sName );
			msOrigFileName = aBaseURL.GetMainURL(INetURLObject::DECODE_TO_IURI);
		}
        mpImpl->mStreamName = sName; // Note: may be empty (XSLT)

        // --> OD 2006-09-26 #i69627#
        const ::rtl::OUString sOutlineStyleAsNormalListStyle(
                RTL_CONSTASCII_USTRINGPARAM("OutlineStyleAsNormalListStyle") );
        if( xPropertySetInfo->hasPropertyByName( sOutlineStyleAsNormalListStyle ) )
        {
            uno::Any aAny = mxExportInfo->getPropertyValue( sOutlineStyleAsNormalListStyle );
            aAny >>= (mpImpl->mbOutlineStyleAsNormalListStyle);
        }
        // <--

		OUString sTargetStorage( RTL_CONSTASCII_USTRINGPARAM("TargetStorage") );
		if( xPropertySetInfo->hasPropertyByName( sTargetStorage ) )
            mxExportInfo->getPropertyValue( sTargetStorage ) >>= mpImpl->mxTargetStorage;

        // --> OD 2008-11-26 #158694#
        const ::rtl::OUString sExportTextNumberElement(
                RTL_CONSTASCII_USTRINGPARAM("ExportTextNumberElement") );
        if( xPropertySetInfo->hasPropertyByName( sExportTextNumberElement ) )
        {
            uno::Any aAny = mxExportInfo->getPropertyValue( sExportTextNumberElement );
            aAny >>= (mpImpl->mbExportTextNumberElement);
        }
        // <--
    }

}

// XFilter
sal_Bool SAL_CALL SvXMLExport::filter( const uno::Sequence< beans::PropertyValue >& aDescriptor ) throw(uno::RuntimeException)
{
    RTL_LOGFILE_CONTEXT_AUTHOR( aLogContext, "xmloff", LOGFILE_AUTHOR,
                                "SvXMLExport::filter" );

    // check for xHandler first... should have been supplied in initialize
    if( !mxHandler.is() )
        return sal_False;

	try
	{
		const sal_uInt32 nTest =
			EXPORT_META|EXPORT_STYLES|EXPORT_CONTENT|EXPORT_SETTINGS;
		if( (mnExportFlags & nTest) == nTest && !msOrigFileName.getLength() )
		{
			// evaluate descriptor only for flat files and if a base URI
			// has not been provided already
			const sal_Int32 nPropCount = aDescriptor.getLength();
			const beans::PropertyValue* pProps = aDescriptor.getConstArray();

			for( sal_Int32 nIndex = 0; nIndex < nPropCount; nIndex++, pProps++ )
			{
				const OUString& rPropName = pProps->Name;
				const Any& rValue = pProps->Value;

				if( rPropName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "FileName" ) ) )
				{
					if( !(rValue >>= msOrigFileName ) )
						return sal_False;
				}
				else if (rPropName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "FilterName" ) ) )
				{
					if( !(rValue >>= msFilterName ) )
						return sal_False;
				}
			}
		}

#ifdef TIMELOG
        if (GetModel().is())
        {
            // print a trace message with the URL
            ByteString aUrl( (String) GetModel()->getURL(),
                             RTL_TEXTENCODING_ASCII_US );
            RTL_LOGFILE_CONTEXT_TRACE1( aLogContext, "%s", aUrl.GetBuffer() );

            // we also want a trace message with the document class
            ByteString aClass( (String)GetXMLToken(meClass),
                               RTL_TEXTENCODING_ASCII_US );
            RTL_LOGFILE_CONTEXT_TRACE1( aLogContext, "class=\"%s\"",
                                        aClass.GetBuffer() );
        }
#endif

		exportDoc( meClass );
	}
	catch( uno::Exception e )
	{
		// We must catch exceptions, because according to the
		// API definition export must not throw one!
        Sequence<OUString> aSeq(0);
        SetError( XMLERROR_FLAG_ERROR | XMLERROR_FLAG_SEVERE | XMLERROR_API,
                  aSeq, e.Message, NULL );
	}

    // return true only if no error occurred
    return (GetErrorFlags() & (ERROR_DO_NOTHING|ERROR_ERROR_OCCURED)) == 0;
}

void SAL_CALL SvXMLExport::cancel() throw(uno::RuntimeException)
{
    // stop export
    Sequence<OUString> aEmptySeq;
    SetError(XMLERROR_CANCEL|XMLERROR_FLAG_SEVERE, aEmptySeq);
}

::rtl::OUString SAL_CALL SvXMLExport::getName(  )
    throw (::com::sun::star::uno::RuntimeException)
{
    return msFilterName;
}

void SAL_CALL SvXMLExport::setName( const ::rtl::OUString& )
    throw (::com::sun::star::uno::RuntimeException)
{
    // do nothing, because it is not possible to set the FilterName
}


// XServiceInfo
OUString SAL_CALL SvXMLExport::getImplementationName(  ) throw(uno::RuntimeException)
{
	OUString aStr;
	return aStr;
}

sal_Bool SAL_CALL SvXMLExport::supportsService( const OUString& rServiceName ) throw(uno::RuntimeException)
{
    return
        rServiceName.equalsAsciiL(
            "com.sun.star.document.ExportFilter",
            sizeof("com.sun.star.document.ExportFilter")-1 ) ||
        rServiceName.equalsAsciiL(
            "com.sun.star.xml.XMLExportFilter",
            sizeof("com.sun.star.xml.XMLExportFilter")-1);
}

uno::Sequence< OUString > SAL_CALL SvXMLExport::getSupportedServiceNames(  )
	throw(uno::RuntimeException)
{
    uno::Sequence<OUString> aSeq(2);
    aSeq[0] = OUString(
        RTL_CONSTASCII_USTRINGPARAM("com.sun.star.document.ExportFilter"));
    aSeq[1] = OUString(
        RTL_CONSTASCII_USTRINGPARAM("com.sun.star.xml.XMLExportFilter"));
    return aSeq;
}

///////////////////////////////////////////////////////////////////////

::rtl::OUString
SvXMLExport::EnsureNamespace(::rtl::OUString const & i_rNamespace,
    ::rtl::OUString const & i_rPreferredPrefix)
{
    ::rtl::OUString sPrefix;
    sal_uInt16 nKey( _GetNamespaceMap().GetKeyByName( i_rNamespace ) );
    if( XML_NAMESPACE_UNKNOWN == nKey )
    {
        // There is no prefix for the namespace, so
        // we have to generate one and have to add it.
        sPrefix = i_rPreferredPrefix;
        nKey = _GetNamespaceMap().GetKeyByPrefix( sPrefix );
        sal_Int32 n( 0 );
        ::rtl::OUStringBuffer buf;
        while( nKey != USHRT_MAX )
        {
            buf.append( i_rPreferredPrefix );
            buf.append( ++n );
            sPrefix = buf.makeStringAndClear();
            nKey = _GetNamespaceMap().GetKeyByPrefix( sPrefix );
        }

        if (mpImpl->mNamespaceMaps.empty()
            || (mpImpl->mNamespaceMaps.top().second != mpImpl->mDepth))
        {
            // top was created for lower depth... need a new namespace map!
            mpImpl->mNamespaceMaps.push(
                ::std::make_pair(mpNamespaceMap, mpImpl->mDepth) );
            mpNamespaceMap = new SvXMLNamespaceMap( *mpNamespaceMap );
        }

        // add the namespace to the map and as attribute
        mpNamespaceMap->Add( sPrefix, i_rNamespace );
        buf.append( GetXMLToken(XML_XMLNS) );
        buf.append( sal_Unicode(':') );
        buf.append( sPrefix );
        AddAttribute( buf.makeStringAndClear(), i_rNamespace );
    }
    else
    {
        // If there is a prefix for the namespace, reuse that.
        sPrefix = _GetNamespaceMap().GetPrefixByKey( nKey );
    }
    return sPrefix;
}

///////////////////////////////////////////////////////////////////////

void SvXMLExport::AddAttributeASCII( sal_uInt16 nPrefixKey,
									 const sal_Char *pName,
							  		 const sal_Char *pValue )
{
	OUString sName( OUString::createFromAscii( pName ) );
	OUString sValue( OUString::createFromAscii( pValue ) );

    mpAttrList->AddAttribute(
        _GetNamespaceMap().GetQNameByKey( nPrefixKey, sName ), sValue );
}

void SvXMLExport::AddAttribute( sal_uInt16 nPrefixKey, const sal_Char *pName,
							  const OUString& rValue )
{
	OUString sName( OUString::createFromAscii( pName ) );

    mpAttrList->AddAttribute(
        _GetNamespaceMap().GetQNameByKey( nPrefixKey, sName ), rValue );
}

void SvXMLExport::AddAttribute( sal_uInt16 nPrefixKey, const OUString& rName,
							  const OUString& rValue )
{
    mpAttrList->AddAttribute(
        _GetNamespaceMap().GetQNameByKey( nPrefixKey, rName ), rValue );
}

void SvXMLExport::AddAttribute( sal_uInt16 nPrefixKey,
                                enum XMLTokenEnum eName,
                                const OUString& rValue )
{
    mpAttrList->AddAttribute(
        _GetNamespaceMap().GetQNameByKey( nPrefixKey, GetXMLToken(eName) ),
        rValue );
}

void SvXMLExport::AddAttribute( sal_uInt16 nPrefixKey,
                                enum XMLTokenEnum eName,
                                enum XMLTokenEnum eValue)
{
    mpAttrList->AddAttribute(
        _GetNamespaceMap().GetQNameByKey( nPrefixKey, GetXMLToken(eName) ),
        GetXMLToken(eValue) );
}

void SvXMLExport::AddAttribute( const ::rtl::OUString& rQName,
                                const ::rtl::OUString& rValue )
{
      mpAttrList->AddAttribute(
        rQName,
        rValue );
}

void SvXMLExport::AddAttribute( const ::rtl::OUString& rQName,
                                enum ::xmloff::token::XMLTokenEnum eValue )
{
      mpAttrList->AddAttribute(
        rQName,
        GetXMLToken(eValue) );
}

void SvXMLExport::AddAttributeList( const uno::Reference< xml::sax::XAttributeList >& xAttrList )
{
	if( xAttrList.is())
		mpAttrList->AppendAttributeList( xAttrList );
}

void SvXMLExport::ClearAttrList()
{
	mpAttrList->Clear();
}

#ifdef DBG_UTIL
void SvXMLExport::CheckAttrList()
{
	DBG_ASSERT( !mpAttrList->getLength(),
				"XMLExport::CheckAttrList: list is not empty" );
}
#endif

void SvXMLExport::ImplExportMeta()
{
	CheckAttrList();

    _ExportMeta();
}

void SvXMLExport::ImplExportSettings()
{
	CheckAttrList();

    ::std::list< SettingsGroup > aSettings;
    sal_Int32 nSettingsCount = 0;

    // view settings
    uno::Sequence< beans::PropertyValue > aViewSettings;
	GetViewSettingsAndViews( aViewSettings );
    aSettings.push_back( SettingsGroup( XML_VIEW_SETTINGS, aViewSettings ) );
    nSettingsCount += aViewSettings.getLength();

    // configuration settings
	uno::Sequence<beans::PropertyValue> aConfigSettings;
	GetConfigurationSettings( aConfigSettings );
    aSettings.push_back( SettingsGroup( XML_CONFIGURATION_SETTINGS, aConfigSettings ) );
    nSettingsCount += aConfigSettings.getLength();

    // any document specific settings
    nSettingsCount += GetDocumentSpecificSettings( aSettings );

	{
		SvXMLElementExport aElem( *this,
								nSettingsCount != 0,
								XML_NAMESPACE_OFFICE, XML_SETTINGS,
								sal_True, sal_True );

        SettingsExportFacade aSettingsExportContext( *this );
		XMLSettingsExportHelper aSettingsExportHelper( aSettingsExportContext );

        for (   ::std::list< SettingsGroup >::const_iterator settings = aSettings.begin();
                settings != aSettings.end();
                ++settings
            )
        {
            if ( !settings->aSettings.getLength() )
                continue;

	        OUString sSettingsName( GetXMLToken( settings->eGroupName ) );
	        OUString sQName = GetNamespaceMap().GetQNameByKey( XML_NAMESPACE_OOO, sSettingsName );
	        aSettingsExportHelper.exportAllSettings( settings->aSettings, sQName );
        }
	}
}

void SvXMLExport::ImplExportStyles( sal_Bool )
{
	CheckAttrList();

//	AddAttribute( XML_NAMESPACE_NONE, XML_ID, XML_STYLES_ID );
	{
		// <office:styles>
		SvXMLElementExport aElem( *this, XML_NAMESPACE_OFFICE, XML_STYLES,
								sal_True, sal_True );

		_ExportStyles( sal_False );
	}

    // transfer style names (+ families) TO other components (if appropriate)
    if( ( ( mnExportFlags & EXPORT_CONTENT ) == 0 ) && mxExportInfo.is() )
    {
        static OUString sStyleNames( RTL_CONSTASCII_USTRINGPARAM("StyleNames") );
        static OUString sStyleFamilies( RTL_CONSTASCII_USTRINGPARAM("StyleFamilies") );
        uno::Reference< beans::XPropertySetInfo > xPropertySetInfo = mxExportInfo->getPropertySetInfo();
        if ( xPropertySetInfo->hasPropertyByName( sStyleNames ) && xPropertySetInfo->hasPropertyByName( sStyleFamilies ) )
        {
            Sequence<sal_Int32> aStyleFamilies;
            Sequence<OUString> aStyleNames;
            mxAutoStylePool->GetRegisteredNames( aStyleFamilies, aStyleNames );
            mxExportInfo->setPropertyValue( sStyleNames, makeAny( aStyleNames ) );
            mxExportInfo->setPropertyValue( sStyleFamilies,
                                           makeAny( aStyleFamilies ) );
        }
    }
}

void SvXMLExport::ImplExportAutoStyles( sal_Bool )
{
    // transfer style names (+ families) FROM other components (if appropriate)
    OUString sStyleNames( RTL_CONSTASCII_USTRINGPARAM("StyleNames") );
    OUString sStyleFamilies( RTL_CONSTASCII_USTRINGPARAM("StyleFamilies") );
    if( ( ( mnExportFlags & EXPORT_STYLES ) == 0 )
        && mxExportInfo.is()
        && mxExportInfo->getPropertySetInfo()->hasPropertyByName( sStyleNames )
        && mxExportInfo->getPropertySetInfo()->hasPropertyByName( sStyleFamilies ) )
    {
        Sequence<sal_Int32> aStyleFamilies;
        mxExportInfo->getPropertyValue( sStyleFamilies ) >>= aStyleFamilies;
        Sequence<OUString> aStyleNames;
        mxExportInfo->getPropertyValue( sStyleNames ) >>= aStyleNames;
        mxAutoStylePool->RegisterNames( aStyleFamilies, aStyleNames );
    }

//	AddAttributeASCII( XML_NAMESPACE_NONE, XML_ID, XML_AUTO_STYLES_ID );
	{
		// <office:automatic-styles>
		SvXMLElementExport aElem( *this, XML_NAMESPACE_OFFICE,
								  XML_AUTOMATIC_STYLES, sal_True, sal_True );

#if 0
		AddAttribute( XML_NAMESPACE_XLINK, XML_TYPE, XML_SIMPLE );
		AddAttribute( XML_NAMESPACE_XLINK, XML_HREF, XML_STYLES_HREF );
		AddAttribute( XML_NAMESPACE_XLINK, XML_ACTUATE, XML_ONLOAD );
		AddAttribute( XML_NAMESPACE_XLINK, XML_ROLE,
				   	  pNamespaceMap->GetQNameByKey( XML_NAMESPACE_OFFICE,
                                               GetXMLToken(XML_STYLESHEET)) );
		{
			// <office:use-styles>
			SvXMLElementExport aElem( *this, XML_NAMESPACE_OFFICE,
									  XML_USE_STYLES, sal_True, sal_True );
		}
#endif
		_ExportAutoStyles();
	}
}

void SvXMLExport::ImplExportMasterStyles( sal_Bool )
{
	{
		// <office:master-styles>
		SvXMLElementExport aElem( *this, XML_NAMESPACE_OFFICE, XML_MASTER_STYLES,
								sal_True, sal_True );

		_ExportMasterStyles();
	}

#if 0
	AddAttribute( XML_NAMESPACE_XLINK, XML_TYPE, XML_SIMPLE );
	AddAttribute( XML_NAMESPACE_XLINK, XML_HREF, XML_AUTO_STYLES_HREF );
	AddAttribute( XML_NAMESPACE_XLINK, XML_ACTUATE, XML_ONLOAD );
	AddAttribute( XML_NAMESPACE_XLINK, XML_ROLE,
				  pNamespaceMap->GetQNameByKey( XML_NAMESPACE_OFFICE,
                                                GetXMLToken(XML_STYLESHEET) ) );
	{
		// <office:use-styles>
		SvXMLElementExport aElem( *this, XML_NAMESPACE_OFFICE,
								  XML_USE_STYLES, sal_True, sal_True );
	}
#endif
}

void SvXMLExport::ImplExportContent()
{
	CheckAttrList();

	CheckAttrList();

	{
		SvXMLElementExport aElemrnt( *this, XML_NAMESPACE_OFFICE, XML_BODY,
								  sal_True, sal_True );
		{
			XMLTokenEnum eClass = meClass;
			if( XML_TEXT_GLOBAL == eClass )
			{
				AddAttribute( XML_NAMESPACE_TEXT, XML_GLOBAL,
                      GetXMLToken( XML_TRUE ) );
				eClass = XML_TEXT;
			}
		    // <office:body ...>
		    SetBodyAttributes();
			SvXMLElementExport aElem( *this, meClass != XML_TOKEN_INVALID,
									  XML_NAMESPACE_OFFICE, eClass,
								  	  sal_True, sal_True );

			_ExportContent();
		}
	}
}

void SvXMLExport::SetBodyAttributes()
{
}

static void
lcl_AddGrddl(SvXMLExport & rExport, const sal_Int32 /*nExportMode*/)
{
    // check version >= 1.2
    switch (rExport.getDefaultVersion()) {
        case SvtSaveOptions::ODFVER_011: // fall thru
        case SvtSaveOptions::ODFVER_010: return;
        default: break;
    }

    // #i115030#: disabled
#if 0
    if (EXPORT_SETTINGS != nExportMode) // meta, content, styles
    {
        rExport.AddAttribute( XML_NAMESPACE_GRDDL, XML_TRANSFORMATION,
            OUString::createFromAscii(s_grddl_xsl) );
    }
#endif
}

sal_uInt32 SvXMLExport::exportDoc( enum ::xmloff::token::XMLTokenEnum eClass )
{
	bool bOwnGraphicResolver = false;
	bool bOwnEmbeddedResolver = false;

	if( !mxGraphicResolver.is() || !mxEmbeddedResolver.is() )
	{
		Reference< XMultiServiceFactory > xFactory( mxModel,	UNO_QUERY );
		if( xFactory.is() )
		{
			try
			{
				if( !mxGraphicResolver.is() )
				{
					mxGraphicResolver = Reference< XGraphicObjectResolver >::query(
						xFactory->createInstance(
							OUString(RTL_CONSTASCII_USTRINGPARAM(
								"com.sun.star.document.ExportGraphicObjectResolver"))));
					bOwnGraphicResolver = mxGraphicResolver.is();
				}

				if( !mxEmbeddedResolver.is() )
				{
					mxEmbeddedResolver = Reference< XEmbeddedObjectResolver >::query(
						xFactory->createInstance(
							OUString(RTL_CONSTASCII_USTRINGPARAM(
								"com.sun.star.document.ExportEmbeddedObjectResolver"))));
					bOwnEmbeddedResolver = mxEmbeddedResolver.is();
				}
			}
			catch( com::sun::star::uno::Exception& )
			{
			}
		}
	}
	if( (getExportFlags() & EXPORT_OASIS) == 0 )
	{
		Reference< lang::XMultiServiceFactory > xFactory = getServiceFactory();
		if( xFactory.is() )
		{
			try
			{
				::comphelper::PropertyMapEntry aInfoMap[] =
				{
					{ "Class", sizeof("Class")-1, 0,
						&::getCppuType((::rtl::OUString*)0),
			  			PropertyAttribute::MAYBEVOID, 0},
					{ NULL, 0, 0, NULL, 0, 0 }
				};
				Reference< XPropertySet > xConvPropSet(
					::comphelper::GenericPropertySet_CreateInstance(
							new ::comphelper::PropertySetInfo( aInfoMap ) ) );

				Any aAny;
				aAny <<= GetXMLToken( eClass );
				xConvPropSet->setPropertyValue(
						OUString(RTL_CONSTASCII_USTRINGPARAM("Class")), aAny );

				Reference< XPropertySet > xPropSet =
					mxExportInfo.is()
                    ?  PropertySetMerger_CreateInstance( mxExportInfo,
                                                         xConvPropSet )
                    : xConvPropSet;

                Sequence<Any> aArgs( 3 );
				aArgs[0] <<= mxHandler;
				aArgs[1] <<= xPropSet;
				aArgs[2] <<= mxModel;

				// get filter component
				Reference< xml::sax::XDocumentHandler > xTmpDocHandler(
					xFactory->createInstanceWithArguments(
					OUString::createFromAscii("com.sun.star.comp.Oasis2OOoTransformer"),
								aArgs), UNO_QUERY);
				OSL_ENSURE( xTmpDocHandler.is(),
					"can't instantiate OASIS transformer component" );
				if( xTmpDocHandler.is() )
				{
					mxHandler = xTmpDocHandler;
					mxExtHandler = uno::Reference<xml::sax::XExtendedDocumentHandler>( mxHandler, UNO_QUERY );
				}
			}
			catch( com::sun::star::uno::Exception& )
			{
			}
		}
	}


	mxHandler->startDocument();

	// <?xml version="1.0" encoding="UTF-8"?>
//	xHandler->processingInstruction( S2U( sXML_xml ), S2U( sXML_xml_pi ) );

	// <office:document ...>
	CheckAttrList();

	// namespace attributes
	// ( The namespace decls should be first attributes in the element;
	//   some faulty XML parsers (JAXP1.1) have a problem with this,
	//   also it's more elegant )
	sal_uInt16 nPos = mpNamespaceMap->GetFirstKey();
	while( USHRT_MAX != nPos )
	{
		mpAttrList->AddAttribute( mpNamespaceMap->GetAttrNameByKey( nPos ),
								  mpNamespaceMap->GetNameByKey( nPos ) );
		nPos = mpNamespaceMap->GetNextKey( nPos );
	}



	// office:version = ...
	if( !mbExtended )
    {
        const sal_Char* pVersion = 0;
        switch( getDefaultVersion() )
        {        
        case SvtSaveOptions::ODFVER_LATEST: pVersion = sXML_1_2; break;
        case SvtSaveOptions::ODFVER_012: pVersion = sXML_1_2; break;
        case SvtSaveOptions::ODFVER_011: pVersion = sXML_1_1; break;
        case SvtSaveOptions::ODFVER_010: break;

        default:
            DBG_ERROR("xmloff::SvXMLExport::exportDoc(), unexpected odf default version!");
        }

        if( pVersion )
		    AddAttribute( XML_NAMESPACE_OFFICE, XML_VERSION,
                              OUString::createFromAscii(pVersion) );
    }

	{
		enum XMLTokenEnum eRootService = XML_TOKEN_INVALID;
		const sal_Int32 nExportMode = mnExportFlags & (EXPORT_META|EXPORT_STYLES|EXPORT_CONTENT|EXPORT_SETTINGS);

        lcl_AddGrddl(*this, nExportMode);

		if( EXPORT_META == nExportMode )
		{
			// export only meta
			eRootService = XML_DOCUMENT_META;
		}
		else if ( EXPORT_SETTINGS == nExportMode )
		{
			// export only settings
			eRootService = XML_DOCUMENT_SETTINGS;
		}
		else if( EXPORT_STYLES == nExportMode )
		{
			// export only styles
			eRootService = XML_DOCUMENT_STYLES;
		}
		else if( EXPORT_CONTENT == nExportMode )
		{
			// export only content
			eRootService = XML_DOCUMENT_CONTENT;
		}
		else
		{
			// the god'ol one4all element
			eRootService = XML_DOCUMENT;
			// office:mimetype = ... (only for stream containing the content)
			if( eClass != XML_TOKEN_INVALID )
			{
				OUString aTmp( RTL_CONSTASCII_USTRINGPARAM("application/vnd.oasis.opendocument.") );
				aTmp += GetXMLToken( eClass );
				AddAttribute( XML_NAMESPACE_OFFICE, XML_MIMETYPE, aTmp );
			}
		}

//		if( (getExportFlags() & EXPORT_NODOCTYPE) == 0 &&
//			xExtHandler.is() )
//		{
//			OUStringBuffer aDocType(
//               GetXMLToken(XML_XML_DOCTYPE_PREFIX).getLength() +
//				GetXMLToken(XML_XML_DOCTYPE_SUFFIX).getLength() + 30 );
//
//			aDocType.append( GetXMLToken(XML_XML_DOCTYPE_PREFIX) );
//			aDocType.append( GetNamespaceMap().GetQNameByKey(
//						   XML_NAMESPACE_OFFICE, GetXMLToken(eRootService) ) );
//			aDocType.append( GetXMLToken(XML_XML_DOCTYPE_SUFFIX) );
//			xExtHandler->unknown( aDocType.makeStringAndClear() );
//		}

		SvXMLElementExport aElem( *this, XML_NAMESPACE_OFFICE, eRootService, sal_True, sal_True );

		// meta information
		if( mnExportFlags & EXPORT_META )
			ImplExportMeta();

		// settings
		if( mnExportFlags & EXPORT_SETTINGS )
			ImplExportSettings();

		// scripts
		if( mnExportFlags & EXPORT_SCRIPTS )
			_ExportScripts();

		// font declerations
		if( mnExportFlags & EXPORT_FONTDECLS )
			_ExportFontDecls();

		// styles
		if( mnExportFlags & EXPORT_STYLES )
			ImplExportStyles( sal_False );

		// autostyles
		if( mnExportFlags & EXPORT_AUTOSTYLES )
			ImplExportAutoStyles( sal_False );

		// masterstyles
		if( mnExportFlags & EXPORT_MASTERSTYLES )
			ImplExportMasterStyles( sal_False );

		// contnt
		if( mnExportFlags & EXPORT_CONTENT )
			ImplExportContent();
	}


	mxHandler->endDocument();

	if( bOwnGraphicResolver )
	{
		Reference< XComponent > xComp( mxGraphicResolver, UNO_QUERY );
		xComp->dispose();
	}

	if( bOwnEmbeddedResolver )
	{
		Reference< XComponent > xComp( mxEmbeddedResolver, UNO_QUERY );
		xComp->dispose();
	}

	return 0;
}

void SvXMLExport::ResetNamespaceMap()
{
    delete mpNamespaceMap;    mpNamespaceMap = new SvXMLNamespaceMap;
}

void SvXMLExport::_ExportMeta()
{
    OUString generator( ::utl::DocInfoHelper::GetGeneratorString() );
    Reference< XDocumentPropertiesSupplier > xDocPropsSupplier(mxModel,
        UNO_QUERY);
    if (xDocPropsSupplier.is()) {
        Reference<XDocumentProperties> xDocProps(
            xDocPropsSupplier->getDocumentProperties());
        if (!xDocProps.is()) throw;
        // update generator here
        xDocProps->setGenerator(generator);
        SvXMLMetaExport * pMeta = new SvXMLMetaExport(*this, xDocProps);
        uno::Reference<xml::sax::XDocumentHandler> xMeta(pMeta);
        pMeta->Export();
    } else {
        // office:meta
		SvXMLElementExport aElem( *this, XML_NAMESPACE_OFFICE, XML_META,
								sal_True, sal_True );
        {
    // BM: #i60323# export generator even if xInfoProp is empty (which is the
    // case for charts). The generator does not depend on xInfoProp
            SvXMLElementExport anElem( *this, XML_NAMESPACE_META, XML_GENERATOR,
                                      sal_True, sal_True );
            Characters(generator);
        }
    }
}

void SvXMLExport::_ExportScripts()
{
    SvXMLElementExport aElement( *this, XML_NAMESPACE_OFFICE, XML_SCRIPTS, sal_True, sal_True );

    // export Basic macros (only for FlatXML)
    if ( mnExportFlags & EXPORT_EMBEDDED )
    {
        ::rtl::OUString aValue( GetNamespaceMap().GetPrefixByKey( XML_NAMESPACE_OOO ) );
        aValue += ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ":Basic" ) );
        AddAttribute( XML_NAMESPACE_SCRIPT, XML_LANGUAGE, aValue );

        SvXMLElementExport aElem( *this, XML_NAMESPACE_OFFICE, XML_SCRIPT, sal_True, sal_True );

        // initialize Basic
        if ( mxModel.is() )
        {
            Reference< beans::XPropertySet > xPSet( mxModel, UNO_QUERY );
            if ( xPSet.is() )
	            xPSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "BasicLibraries" ) ) );
        }

	    Reference< document::XExporter > xExporter;
	    Reference< lang::XMultiServiceFactory > xMSF( getServiceFactory() );
        if ( xMSF.is() )
        {
            Reference < XDocumentHandler > xHdl( new XMLBasicExportFilter( mxHandler ) );
	        Sequence < Any > aArgs( 1 );
	        aArgs[0] <<= xHdl;
            xExporter.set( xMSF->createInstanceWithArguments(
                OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.document.XMLOasisBasicExporter" ) ), aArgs ),
                UNO_QUERY );
        }

        OSL_ENSURE( xExporter.is(),
            "SvXMLExport::_ExportScripts: can't instantiate export filter component for Basic macros" );

        if ( xExporter.is() )
        {
            Reference< XComponent > xComp( mxModel, UNO_QUERY );
            xExporter->setSourceDocument( xComp );
            Reference< XFilter > xFilter( xExporter, UNO_QUERY );
            if ( xFilter.is() )
            {
                Sequence < PropertyValue > aMediaDesc( 0 );
                xFilter->filter( aMediaDesc );
            }
        }
    }

	// export document events
	Reference< document::XEventsSupplier > xEvents( GetModel(), UNO_QUERY );
	GetEventExport().Export( xEvents, sal_True );
}

void SvXMLExport::_ExportFontDecls()
{
	if( mxFontAutoStylePool.is() )
		mxFontAutoStylePool->exportXML();
}

void SvXMLExport::_ExportStyles( sal_Bool )
{
	uno::Reference< lang::XMultiServiceFactory > xFact( GetModel(), uno::UNO_QUERY );
	if( xFact.is())
	{
		// export (fill-)gradient-styles
		try
		{
			uno::Reference< container::XNameAccess > xGradient( xFact->createInstance( OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.GradientTable") ) ), uno::UNO_QUERY );
			if( xGradient.is() )
			{
				XMLGradientStyleExport aGradientStyle( *this );

				if( xGradient->hasElements() )
				{
					uno::Sequence< OUString > aNamesSeq ( xGradient->getElementNames() );
					sal_Int32 nCount = aNamesSeq.getLength();
					for( sal_Int32 i=0; i<nCount; i++ )
					{
						const OUString& rStrName = aNamesSeq[ i ];

						try
						{
							uno::Any aValue = xGradient->getByName( rStrName );

							aGradientStyle.exportXML( rStrName, aValue );
						}
						catch( container::NoSuchElementException& )
						{}
					}
				}
			}
		}
		catch( lang::ServiceNotRegisteredException& )
		{}

		// export (fill-)hatch-styles
		try
		{
			uno::Reference< container::XNameAccess > xHatch( xFact->createInstance( OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.HatchTable") ) ), uno::UNO_QUERY );
			if( xHatch.is() )
			{
				XMLHatchStyleExport aHatchStyle( *this );

				if( xHatch->hasElements() )
				{
					uno::Sequence< OUString > aNamesSeq ( xHatch->getElementNames() );
					sal_Int32 nCount = aNamesSeq.getLength();
					for( sal_Int32 i=0; i<nCount; i++ )
					{
						const OUString& rStrName = aNamesSeq[ i ];

						try
						{
							uno::Any aValue = xHatch->getByName( rStrName );

							aHatchStyle.exportXML( rStrName, aValue );
						}
						catch( container::NoSuchElementException& )
						{}
					}
				}
			}
		}
		catch( lang::ServiceNotRegisteredException& )
		{}

		// export (fill-)bitmap-styles
		try
		{
			uno::Reference< container::XNameAccess > xBitmap( xFact->createInstance( OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.BitmapTable") ) ), uno::UNO_QUERY );
			if( xBitmap.is() )
			{
				XMLImageStyle aImageStyle;

				if( xBitmap->hasElements() )
				{
					uno::Sequence< OUString > aNamesSeq ( xBitmap->getElementNames() );
					sal_Int32 nCount = aNamesSeq.getLength();
					for( sal_Int32 i=0; i<nCount; i++ )
					{
						const OUString& rStrName = aNamesSeq[ i ];

						try
						{
							uno::Any aValue = xBitmap->getByName( rStrName );

							aImageStyle.exportXML( rStrName, aValue, *this );
						}
						catch( container::NoSuchElementException& )
						{}
					}
				}
			}
		}
		catch( lang::ServiceNotRegisteredException& )
		{}

		// export transparency-gradient -styles
		try
		{
			uno::Reference< container::XNameAccess > xTransGradient( xFact->createInstance( OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.TransparencyGradientTable") ) ), uno::UNO_QUERY );
			if( xTransGradient.is() )
			{
				XMLTransGradientStyleExport aTransGradientstyle( *this );

				if( xTransGradient->hasElements() )
				{
					uno::Sequence< OUString > aNamesSeq ( xTransGradient->getElementNames() );
					sal_Int32 nCount = aNamesSeq.getLength();
					for( sal_Int32 i=0; i<nCount; i++ )
					{
						const OUString& rStrName = aNamesSeq[ i ];

						try
						{
							uno::Any aValue = xTransGradient->getByName( rStrName );

							aTransGradientstyle.exportXML( rStrName, aValue );
						}
						catch( container::NoSuchElementException& )
						{}
					}
				}
			}
		}
		catch( lang::ServiceNotRegisteredException& )
		{}

		// export marker-styles
		try
		{
			uno::Reference< container::XNameAccess > xMarker( xFact->createInstance( OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.MarkerTable") ) ), uno::UNO_QUERY );
			if( xMarker.is() )
			{
				XMLMarkerStyleExport aMarkerStyle( *this );

				if( xMarker->hasElements() )
				{
					uno::Sequence< OUString > aNamesSeq ( xMarker->getElementNames() );
					sal_Int32 nCount = aNamesSeq.getLength();
					for( sal_Int32 i=0; i<nCount; i++ )
					{
						const OUString& rStrName = aNamesSeq[ i ];

						try
						{
							uno::Any aValue = xMarker->getByName( rStrName );

							aMarkerStyle.exportXML( rStrName, aValue );
						}
						catch( container::NoSuchElementException& )
						{}
					}
				}
			}
		}
		catch( lang::ServiceNotRegisteredException& )
		{}

		// export dash-styles
		try
		{
			uno::Reference< container::XNameAccess > xDashes( xFact->createInstance( OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.DashTable") ) ), uno::UNO_QUERY );
			if( xDashes.is() )
			{
				XMLDashStyleExport aDashStyle( *this );

				if( xDashes->hasElements() )
				{
					uno::Sequence< OUString > aNamesSeq ( xDashes->getElementNames() );
					sal_Int32 nCount = aNamesSeq.getLength();
					for( sal_Int32 i=0; i<nCount; i++ )
					{
						const OUString& rStrName = aNamesSeq[ i ];

						try
						{
							uno::Any aValue = xDashes->getByName( rStrName );

							aDashStyle.exportXML( rStrName, aValue );
						}
						catch( container::NoSuchElementException& )
						{}
					}
				}
			}
		}
		catch( lang::ServiceNotRegisteredException& )
		{}
	}
}

XMLTextParagraphExport* SvXMLExport::CreateTextParagraphExport()
{
	return new XMLTextParagraphExport( *this, *(GetAutoStylePool().get()) );
}

XMLShapeExport* SvXMLExport::CreateShapeExport()
{
	return new XMLShapeExport(*this);
}

SvXMLAutoStylePoolP* SvXMLExport::CreateAutoStylePool()
{
	return new SvXMLAutoStylePoolP(*this);
}

XMLPageExport* SvXMLExport::CreatePageExport()
{
	return new XMLPageExport( *this );
}

SchXMLExportHelper* SvXMLExport::CreateChartExport()
{
	return new SchXMLExportHelper(*this,*GetAutoStylePool().get());
}

XMLFontAutoStylePool* SvXMLExport::CreateFontAutoStylePool()
{
	return new XMLFontAutoStylePool( *this );
}

xmloff::OFormLayerXMLExport* SvXMLExport::CreateFormExport()
{
	return new xmloff::OFormLayerXMLExport(*this);
}

void SvXMLExport::GetViewSettingsAndViews(uno::Sequence<beans::PropertyValue>& rProps)
{
	GetViewSettings(rProps);
	uno::Reference<document::XViewDataSupplier> xViewDataSupplier(GetModel(), uno::UNO_QUERY);
	if(xViewDataSupplier.is())
	{
		uno::Reference<container::XIndexAccess> xIndexAccess;
		xViewDataSupplier->setViewData( xIndexAccess ); // make sure we get a newly created sequence
		xIndexAccess = xViewDataSupplier->getViewData();
		sal_Bool bAdd = sal_False;
		uno::Any aAny;
		if(xIndexAccess.is() && xIndexAccess->hasElements() )
		{
			sal_Int32 nCount = xIndexAccess->getCount();
			for (sal_Int32 i = 0; i < nCount; i++)
			{
				aAny = xIndexAccess->getByIndex(i);
				uno::Sequence<beans::PropertyValue> aProps;
				if( aAny >>= aProps )
				{
					if( aProps.getLength() > 0 )
					{
						bAdd = sal_True;
						break;
					}
				}
			}
		}

		if( bAdd )
		{
			sal_Int32 nOldLength(rProps.getLength());
			rProps.realloc(nOldLength + 1);
			beans::PropertyValue aProp;
			aProp.Name = OUString(RTL_CONSTASCII_USTRINGPARAM("Views"));
			aProp.Value <<= xIndexAccess;
			rProps[nOldLength] = aProp;
		}
	}
}

void SvXMLExport::GetViewSettings(uno::Sequence<beans::PropertyValue>&)
{
}

void SvXMLExport::GetConfigurationSettings(uno::Sequence<beans::PropertyValue>&)
{
}

sal_Int32 SvXMLExport::GetDocumentSpecificSettings( ::std::list< SettingsGroup >& _out_rSettings )
{
    (void)_out_rSettings;
    return 0;
}

void SvXMLExport::addDataStyle(const sal_Int32 nNumberFormat, sal_Bool /*bTimeFormat*/ )
{
	if(mpNumExport)
		mpNumExport->SetUsed(nNumberFormat);
}

void SvXMLExport::exportDataStyles()
{
	if(mpNumExport)
		mpNumExport->Export(sal_False);
}

void SvXMLExport::exportAutoDataStyles()
{
	if(mpNumExport)
		mpNumExport->Export(sal_True);

	if (mxFormExport.is())
		mxFormExport->exportAutoControlNumberStyles();
}

OUString SvXMLExport::getDataStyleName(const sal_Int32 nNumberFormat, sal_Bool /*bTimeFormat*/ ) const
{
	OUString sTemp;
	if(mpNumExport)
		sTemp = mpNumExport->GetStyleName(nNumberFormat);
	return sTemp;
}

void SvXMLExport::exportAnnotationMeta(const uno::Reference<drawing::XShape>&)
{
}

sal_Int32 SvXMLExport::dataStyleForceSystemLanguage(sal_Int32 nFormat) const
{
    return ( mpNumExport != NULL )
                 ? mpNumExport->ForceSystemLanguage( nFormat ) : nFormat;
}


OUString SvXMLExport::AddEmbeddedGraphicObject( const OUString& rGraphicObjectURL )
{
	OUString sRet( rGraphicObjectURL );
	if( 0 == rGraphicObjectURL.compareTo( msGraphicObjectProtocol,
                                          msGraphicObjectProtocol.getLength() ) &&
        mxGraphicResolver.is() )
	{
		if( (getExportFlags() & EXPORT_EMBEDDED) == 0 )
			sRet = mxGraphicResolver->resolveGraphicObjectURL( rGraphicObjectURL );
		else
			sRet = OUString();
	}
	else
		sRet = GetRelativeReference( sRet );

	return sRet;
}

sal_Bool SvXMLExport::AddEmbeddedGraphicObjectAsBase64( const OUString& rGraphicObjectURL )
{
    sal_Bool bRet = sal_False;

	if( (getExportFlags() & EXPORT_EMBEDDED) != 0 &&
		0 == rGraphicObjectURL.compareTo( msGraphicObjectProtocol,
                                          msGraphicObjectProtocol.getLength() ) &&
		mxGraphicResolver.is() )
	{
        Reference< XBinaryStreamResolver > xStmResolver( mxGraphicResolver, UNO_QUERY );

        if( xStmResolver.is() )
        {
            Reference< XInputStream > xIn( xStmResolver->getInputStream( rGraphicObjectURL ) );

            if( xIn.is() )
            {
				XMLBase64Export aBase64Exp( *this );
				bRet = aBase64Exp.exportOfficeBinaryDataElement( xIn );
            }
        }
    }

	return bRet;
}

OUString SvXMLExport::AddEmbeddedObject( const OUString& rEmbeddedObjectURL )
{
	OUString sRet;
	if( (0 == rEmbeddedObjectURL.compareTo( msEmbeddedObjectProtocol,
				msEmbeddedObjectProtocol.getLength() ) ||
		 0 == rEmbeddedObjectURL.compareTo( msGraphicObjectProtocol,
				msGraphicObjectProtocol.getLength() ) ) &&
		mxEmbeddedResolver.is() )
	{
		sRet =
			mxEmbeddedResolver->resolveEmbeddedObjectURL( rEmbeddedObjectURL );
	}
	else
		sRet = GetRelativeReference( rEmbeddedObjectURL );

	return sRet;
}

sal_Bool SvXMLExport::AddEmbeddedObjectAsBase64( const OUString& rEmbeddedObjectURL )
{
	sal_Bool bRet = sal_False;
	if( (0 == rEmbeddedObjectURL.compareTo( msEmbeddedObjectProtocol,
				msEmbeddedObjectProtocol.getLength() ) ||
		 0 == rEmbeddedObjectURL.compareTo( msGraphicObjectProtocol,
				msGraphicObjectProtocol.getLength() ) ) &&
		mxEmbeddedResolver.is() )
	{
		Reference < XNameAccess > xNA( mxEmbeddedResolver, UNO_QUERY );
		if( xNA.is() )
		{
			Any aAny = xNA->getByName( rEmbeddedObjectURL );
			Reference < XInputStream > xIn;
			aAny >>= xIn;
			if( xIn.is() )
			{
				XMLBase64Export aBase64Exp( *this );
				bRet = aBase64Exp.exportOfficeBinaryDataElement( xIn );
			}
		}
	}

	return bRet;
}

OUString SvXMLExport::EncodeStyleName(
		const OUString& rName,
	    sal_Bool *pEncoded ) const
{
	return GetMM100UnitConverter().encodeStyleName( rName, pEncoded );
}

ProgressBarHelper*	SvXMLExport::GetProgressBarHelper()
{
	if (!mpProgressBarHelper)
	{
		mpProgressBarHelper = new ProgressBarHelper(mxStatusIndicator, sal_True);

		if (mxExportInfo.is())
		{
			uno::Reference< beans::XPropertySetInfo > xPropertySetInfo = mxExportInfo->getPropertySetInfo();
			if (xPropertySetInfo.is())
			{
				OUString sProgressRange(RTL_CONSTASCII_USTRINGPARAM(XML_PROGRESSRANGE));
				OUString sProgressMax(RTL_CONSTASCII_USTRINGPARAM(XML_PROGRESSMAX));
				OUString sProgressCurrent(RTL_CONSTASCII_USTRINGPARAM(XML_PROGRESSCURRENT));
                OUString sRepeat(RTL_CONSTASCII_USTRINGPARAM(XML_PROGRESSREPEAT));
				if (xPropertySetInfo->hasPropertyByName(sProgressMax) &&
					xPropertySetInfo->hasPropertyByName(sProgressCurrent) &&
					xPropertySetInfo->hasPropertyByName(sProgressRange))
				{
					uno::Any aAny;
					sal_Int32 nProgressMax(0);
					sal_Int32 nProgressCurrent(0);
					sal_Int32 nProgressRange(0);
					aAny = mxExportInfo->getPropertyValue(sProgressRange);
					if (aAny >>= nProgressRange)
						mpProgressBarHelper->SetRange(nProgressRange);
					aAny = mxExportInfo->getPropertyValue(sProgressMax);
					if (aAny >>= nProgressMax)
						mpProgressBarHelper->SetReference(nProgressMax);
					aAny = mxExportInfo->getPropertyValue(sProgressCurrent);
					if (aAny >>= nProgressCurrent)
						mpProgressBarHelper->SetValue(nProgressCurrent);
				}
                if (xPropertySetInfo->hasPropertyByName(sRepeat))
                {
                    uno::Any aAny = mxExportInfo->getPropertyValue(sRepeat);
                    if (aAny.getValueType() == getBooleanCppuType())
                        mpProgressBarHelper->SetRepeat(::cppu::any2bool(aAny));
                    else {
                        DBG_ERRORFILE("why is it no boolean?");
                    }
                }
			}
		}
	}
	return mpProgressBarHelper;
}

XMLEventExport& SvXMLExport::GetEventExport()
{
	if( NULL == mpEventExport)
	{
		// create EventExport on demand
		mpEventExport = new XMLEventExport(*this, NULL);

		// and register standard handlers + names
		OUString sStarBasic(RTL_CONSTASCII_USTRINGPARAM("StarBasic"));
		mpEventExport->AddHandler(sStarBasic, new XMLStarBasicExportHandler());
		OUString sScript(RTL_CONSTASCII_USTRINGPARAM("Script"));
		mpEventExport->AddHandler(sScript, new XMLScriptExportHandler());
		mpEventExport->AddTranslationTable(aStandardEventTable);
	}

	return *mpEventExport;
}

XMLImageMapExport& SvXMLExport::GetImageMapExport()
{
	// image map export, create on-demand
	if( NULL == mpImageMapExport )
	{
		mpImageMapExport = new XMLImageMapExport(*this);
	}

	return *mpImageMapExport;
}

// XUnoTunnel & co
const uno::Sequence< sal_Int8 > & SvXMLExport::getUnoTunnelId() throw()
{
	static uno::Sequence< sal_Int8 > * pSeq = 0;
	if( !pSeq )
	{
		Guard< Mutex > aGuard( Mutex::getGlobalMutex() );
		if( !pSeq )
		{
			static uno::Sequence< sal_Int8 > aSeq( 16 );
			rtl_createUuid( (sal_uInt8*)aSeq.getArray(), 0, sal_True );
			pSeq = &aSeq;
		}
	}
	return *pSeq;
}

SvXMLExport* SvXMLExport::getImplementation( uno::Reference< uno::XInterface > xInt ) throw()
{
	uno::Reference< lang::XUnoTunnel > xUT( xInt, uno::UNO_QUERY );
	if( xUT.is() )
	{
		return
			reinterpret_cast<SvXMLExport*>(
				sal::static_int_cast<sal_IntPtr>(
					xUT->getSomething( SvXMLExport::getUnoTunnelId())));
	}
	else
		return NULL;
}

// XUnoTunnel
sal_Int64 SAL_CALL SvXMLExport::getSomething( const uno::Sequence< sal_Int8 >& rId )
	throw( uno::RuntimeException )
{
	if( rId.getLength() == 16 && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
														 rId.getConstArray(), 16 ) )
	{
		return sal::static_int_cast<sal_Int64>(reinterpret_cast<sal_uIntPtr>(this));
	}
	return 0;
}

sal_Bool SvXMLExport::ExportEmbeddedOwnObject( Reference< XComponent >& rComp )
{
	OUString sFilterService;
    bool bIsChart = false;

    Reference < lang::XServiceInfo > xServiceInfo( rComp, UNO_QUERY );
	if( xServiceInfo.is() )
	{
		const XMLServiceMapEntry_Impl *pEntry = aServiceMap;
		while( pEntry->sModelService )
		{
			OUString sModelService( pEntry->sModelService,
									pEntry->nModelServiceLen,
								   	RTL_TEXTENCODING_ASCII_US );
			if( xServiceInfo->supportsService( sModelService ) )
			{
				sFilterService = OUString( pEntry->sFilterService,
										   pEntry->nFilterServiceLen,
								   		   RTL_TEXTENCODING_ASCII_US );
                bIsChart = sModelService.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( XML_MODEL_SERVICE_CHART ));
                break;
			}
			pEntry++;
		}
	}

	OSL_ENSURE( sFilterService.getLength(), "no export filter for own object" );

	if( !sFilterService.getLength() )
		return sal_False;

	Reference < XDocumentHandler > xHdl =
		new XMLEmbeddedObjectExportFilter( mxHandler );

    Sequence < Any > aArgs( 1 );
    // #144135# the filters for embedded objects in flat format are always
    // instantiated as Oasis filters and transformed afterwards. Therefore, all
    // special handling that is done if the exportFlags do not contain
    // EXPORT_OASIS must be changed to properties being passed in the info
    // propertyset

	if( ! (getExportFlags() & EXPORT_OASIS) &&
        bIsChart )
    {
        static ::comphelper::PropertyMapEntry aInfoMap[] =
        {
            { RTL_CONSTASCII_STRINGPARAM("ExportTableNumberList"), 0, &::getBooleanCppuType(), PropertyAttribute::MAYBEVOID, 0},
            { NULL, 0, 0, NULL, 0, 0 }
        };
        Reference< XPropertySet > xInfoProp(
            ::comphelper::GenericPropertySet_CreateInstance(
                new ::comphelper::PropertySetInfo( aInfoMap )));

        if( bIsChart )
            xInfoProp->setPropertyValue( OUString( RTL_CONSTASCII_USTRINGPARAM("ExportTableNumberList")), makeAny( true ));

        aArgs.realloc( 2 );
        aArgs[1] <<= xInfoProp;
    }
	aArgs[0] <<= xHdl;

	// #110680#
	// Reference< lang::XMultiServiceFactory > xServiceFactory = comphelper::getProcessServiceFactory();
	Reference< lang::XMultiServiceFactory > xServiceFactory = getServiceFactory();

	Reference< document::XExporter > xExporter(
		xServiceFactory->createInstanceWithArguments( sFilterService, aArgs),
											   UNO_QUERY);
	OSL_ENSURE( xExporter.is(),
				"can't instantiate export filter component for own object" );
	if( !xExporter.is() )
		return sal_False;

	xExporter->setSourceDocument( rComp );

	Reference<XFilter> xFilter( xExporter, UNO_QUERY );

	Sequence < PropertyValue > aMediaDesc( 0 );
	return xFilter->filter( aMediaDesc );
}

OUString SvXMLExport::GetRelativeReference(const OUString& rValue)
{
    OUString sValue( rValue );
    // #i65474# handling of fragment URLs ("#....") is undefined
    // they are stored 'as is'
    uno::Reference< uri::XUriReference > xUriRef;
    if(sValue.getLength() && sValue.getStr()[0] != '#')
    {
        try
        {
            xUriRef = mpImpl->mxUriReferenceFactory->parse( rValue );
            if( xUriRef.is() && !xUriRef->isAbsolute() )
            {
                //#i61943# relative URLs need special handling
                INetURLObject aTemp( mpImpl->msPackageURI );
                bool bWasAbsolute = false;
                sValue = aTemp.smartRel2Abs(sValue, bWasAbsolute ).GetMainURL(INetURLObject::DECODE_TO_IURI);
            }
        }
        catch( uno::Exception& )
        {
        }
    }
    OUString sRet = sValue;
    if( xUriRef.is() )//no conversion for empty values or for fragments
    {
        //conversion for matching schemes only
        if( xUriRef->getScheme() == mpImpl->msPackageURIScheme )
        {
            sValue = INetURLObject::GetRelURL( msOrigFileName, sValue, 
                INetURLObject::WAS_ENCODED, INetURLObject::DECODE_TO_IURI, RTL_TEXTENCODING_UTF8, INetURLObject::FSYS_DETECT);
        }
    }
    return sValue;
}

void SvXMLExport::StartElement(sal_uInt16 nPrefix,
                        enum ::xmloff::token::XMLTokenEnum eName,
					    sal_Bool bIgnWSOutside )
{
	StartElement(_GetNamespaceMap().GetQNameByKey( nPrefix,
        GetXMLToken(eName) ), bIgnWSOutside);
}

void SvXMLExport::StartElement(const OUString& rName,
					    sal_Bool bIgnWSOutside )
{
	if ((mnErrorFlags & ERROR_DO_NOTHING) != ERROR_DO_NOTHING)
	{
		try
		{
			if( bIgnWSOutside && ((mnExportFlags & EXPORT_PRETTY) == EXPORT_PRETTY))
				mxHandler->ignorableWhitespace( msWS );
			mxHandler->startElement( rName, GetXAttrList() );
		}
		catch ( SAXInvalidCharacterException& e )
		{
            Sequence<OUString> aPars(1);
            aPars[0] = rName;
			SetError( XMLERROR_SAX|XMLERROR_FLAG_WARNING, aPars, e.Message, NULL );
		}
		catch ( SAXException& e )
		{
            Sequence<OUString> aPars(1);
            aPars[0] = rName;
            SetError( XMLERROR_SAX|XMLERROR_FLAG_ERROR|XMLERROR_FLAG_SEVERE,
                      aPars, e.Message, NULL );
		}
	}
    ClearAttrList();
    ++mpImpl->mDepth; // increment nesting depth counter
}

void SvXMLExport::Characters(const ::rtl::OUString& rChars)
{
	if ((mnErrorFlags & ERROR_DO_NOTHING) != ERROR_DO_NOTHING)
	{
		try
		{
			mxHandler->characters(rChars);
		}
		catch ( SAXInvalidCharacterException& e )
		{
            Sequence<OUString> aPars(1);
            aPars[0] = rChars;
			SetError( XMLERROR_SAX|XMLERROR_FLAG_WARNING, aPars, e.Message, NULL );
		}
		catch ( SAXException& e )
		{
            Sequence<OUString> aPars(1);
            aPars[0] = rChars;
            SetError( XMLERROR_SAX|XMLERROR_FLAG_ERROR|XMLERROR_FLAG_SEVERE,
                      aPars, e.Message, NULL );
		}
	}
}

void SvXMLExport::EndElement(sal_uInt16 nPrefix,
						enum ::xmloff::token::XMLTokenEnum eName,
						sal_Bool bIgnWSInside )
{
	EndElement(_GetNamespaceMap().GetQNameByKey( nPrefix, GetXMLToken(eName) ),
        bIgnWSInside);
}

void SvXMLExport::EndElement(const OUString& rName,
						sal_Bool bIgnWSInside )
{
    // decrement nesting depth counter & (maybe) restore namespace map
    --mpImpl->mDepth;
    if (!mpImpl->mNamespaceMaps.empty() &&
        (mpImpl->mNamespaceMaps.top().second == mpImpl->mDepth))
    {
        delete mpNamespaceMap;
        mpNamespaceMap = mpImpl->mNamespaceMaps.top().first;
        mpImpl->mNamespaceMaps.pop();
    }
    OSL_ENSURE(mpImpl->mNamespaceMaps.empty() ||
        (mpImpl->mNamespaceMaps.top().second < mpImpl->mDepth),
        "SvXMLExport: NamespaceMaps corrupted");

	if ((mnErrorFlags & ERROR_DO_NOTHING) != ERROR_DO_NOTHING)
	{
		try
		{
			if( bIgnWSInside && ((mnExportFlags & EXPORT_PRETTY) == EXPORT_PRETTY))
				mxHandler->ignorableWhitespace( msWS );
			mxHandler->endElement( rName );
		}
		catch ( SAXException& e )
		{
            Sequence<OUString> aPars(1);
            aPars[0] = rName;
            SetError( XMLERROR_SAX|XMLERROR_FLAG_ERROR|XMLERROR_FLAG_SEVERE,
                      aPars, e.Message, NULL );
		}
	}
}

void SvXMLExport::IgnorableWhitespace()
{
	if ((mnExportFlags & EXPORT_PRETTY) != EXPORT_PRETTY)
		return;

	if ((mnErrorFlags & ERROR_DO_NOTHING) != ERROR_DO_NOTHING)
	{
		try
		{
			mxHandler->ignorableWhitespace( msWS );
		}
		catch ( SAXException& e )
		{
            Sequence<OUString> aPars(0);
            SetError( XMLERROR_SAX|XMLERROR_FLAG_ERROR|XMLERROR_FLAG_SEVERE,
                      aPars, e.Message, NULL );
		}
	}
}


void SvXMLExport::SetError(
    sal_Int32 nId,
    const Sequence<OUString>& rMsgParams,
    const OUString& rExceptionMessage,
    const Reference<XLocator>& rLocator )
{
    // allow multi-threaded access to the cancel() method
    static ::vos::OMutex aMutex;
    ::vos::OGuard aGuard(aMutex);

    // maintain error flags
    if ( ( nId & XMLERROR_FLAG_ERROR ) != 0 )
        mnErrorFlags |= ERROR_ERROR_OCCURED;
    if ( ( nId & XMLERROR_FLAG_WARNING ) != 0 )
        mnErrorFlags |= ERROR_WARNING_OCCURED;
    if ( ( nId & XMLERROR_FLAG_SEVERE ) != 0 )
        mnErrorFlags |= ERROR_DO_NOTHING;

    // create error lsit on demand
    if ( mpXMLErrors == NULL )
        mpXMLErrors = new XMLErrors();

    // save error information
    mpXMLErrors->AddRecord( nId, rMsgParams, rExceptionMessage, rLocator );
}

void SvXMLExport::SetError(
    sal_Int32 nId,
    const Sequence<OUString>& rMsgParams)
{
    OUString sEmpty;
    SetError( nId, rMsgParams, sEmpty, NULL );
}


XMLErrors* SvXMLExport::GetErrors()
{
	return mpXMLErrors;
}

void SvXMLExport::DisposingModel()
{
	mxModel.clear();
    // --> OD 2006-03-13 #i51726#
    meModelType = SvtModuleOptions::E_UNKNOWN_FACTORY;;
    // <--
	mxEventListener.clear();
}

// #110680#
::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > SvXMLExport::getServiceFactory()
{
	// #110680#
	return mxServiceFactory;
}

uno::Reference< uno::XComponentContext >
SvXMLExport::GetComponentContext() const
{
    return mpImpl->mxComponentContext;
}

::comphelper::UnoInterfaceToUniqueIdentifierMapper& SvXMLExport::getInterfaceToIdentifierMapper()
{
	return mpImpl->maInterfaceToIdentifierMapper;
}

// --> OD 2006-09-27 #i69627#
sal_Bool SvXMLExport::writeOutlineStyleAsNormalListStyle() const
{
    return mpImpl->mbOutlineStyleAsNormalListStyle;
}
// <--

uno::Reference< embed::XStorage > SvXMLExport::GetTargetStorage()
{
	return mpImpl->mxTargetStorage;
}

/// returns the currently configured default version for odf export
SvtSaveOptions::ODFDefaultVersion SvXMLExport::getDefaultVersion() const
{
	if( mpImpl )
		return mpImpl->maSaveOptions.GetODFDefaultVersion();

	// fatal error, use current version as default
	return SvtSaveOptions::ODFVER_012;
}

::rtl::OUString SvXMLExport::GetStreamName() const
{
    return mpImpl->mStreamName;
}

void
SvXMLExport::AddAttributeIdLegacy(
        sal_uInt16 const nLegacyPrefix, ::rtl::OUString const& rValue)
{
    switch (getDefaultVersion()) {
        case SvtSaveOptions::ODFVER_011: // fall thru
        case SvtSaveOptions::ODFVER_010: break;
        default: // ODF 1.2: xml:id
            AddAttribute(XML_NAMESPACE_XML, XML_ID, rValue);
    }
    // in ODF 1.1 this was form:id, anim:id, draw:id, or text:id
    // backward compatibility: in ODF 1.2 write _both_ id attrs
    AddAttribute(nLegacyPrefix, XML_ID, rValue);
    // FIXME: this function simply assumes that rValue is unique
}

void
SvXMLExport::AddAttributeXmlId(uno::Reference<uno::XInterface> const & i_xIfc)
{
    // check version >= 1.2
    switch (getDefaultVersion()) {
        case SvtSaveOptions::ODFVER_011: // fall thru
        case SvtSaveOptions::ODFVER_010: return;
        default: break;
    }
    const uno::Reference<rdf::XMetadatable> xMeta(i_xIfc,
        uno::UNO_QUERY);
//FIXME not yet...
//    OSL_ENSURE(xMeta.is(), "xml:id: not XMetadatable");
    if ( xMeta.is() )
    {
        const beans::StringPair mdref( xMeta->getMetadataReference() );
        if ( !mdref.Second.equalsAscii("") )
        {
            const ::rtl::OUString streamName( GetStreamName() );
            if ( streamName.getLength() )
            {
                if ( streamName.equals(mdref.First) )
                {
                    AddAttribute( XML_NAMESPACE_XML, XML_ID, mdref.Second );
                }
                else
                {
                    OSL_ENSURE(false, "SvXMLExport::AddAttributeXmlId: "
                         "invalid stream name");
                }
            }
            else
            {
                // FIXME: this is ugly
                // there is no stream name (e.g. XSLT, flat-xml format)!
                // but how do we ensure uniqueness in this case?
                // a) just omit styles.xml ids -- they are unlikely anyway...
                // b) somehow find out whether we are currently exporting styles
                //    or content, and prefix "s" or "c" => unique
                if ( mdref.First.equalsAscii("content.xml") )
                {
                    AddAttribute( XML_NAMESPACE_XML, XML_ID, mdref.Second );
                }
                else
                {
                    OSL_TRACE("SvXMLExport::AddAttributeXmlId: "
                        "no stream name given: dropping styles.xml xml:id");
                }
            }
        }
    }
}

void
SvXMLExport::AddAttributesRDFa(
    uno::Reference<text::XTextContent> const & i_xTextContent)
{
    // check version >= 1.2
    switch (getDefaultVersion()) {
        case SvtSaveOptions::ODFVER_011: // fall thru
        case SvtSaveOptions::ODFVER_010: return;
        default: break;
    }

    const uno::Reference<rdf::XMetadatable> xMeta(
        i_xTextContent, uno::UNO_QUERY);
    if (!xMeta.is() || !xMeta->getMetadataReference().Second.getLength())
    {
        return; // no xml:id => no RDFa
    }

    if (!mpImpl->mpRDFaHelper.get())
    {
        mpImpl->mpRDFaHelper.reset( new ::xmloff::RDFaExportHelper(*this) );
    }
    mpImpl->mpRDFaHelper->AddRDFa(xMeta);
}

sal_Bool SvXMLExport::exportTextNumberElement() const
{
    return mpImpl->mbExportTextNumberElement;
}

sal_Bool SvXMLExport::SetNullDateOnUnitConverter()
{
    // if the null date has already been set, don't set it again (performance)
    if (!mpImpl->mbNullDateInitialized)
        mpImpl->mbNullDateInitialized = GetMM100UnitConverter().setNullDate(GetModel());

    return mpImpl->mbNullDateInitialized;
}


//=============================================================================


void SvXMLElementExport::StartElement(
    const sal_uInt16 nPrefixKey,
    const OUString& rLName,
    const sal_Bool bIgnoreWhitespaceOutside )
{
    maElementName = mrExport.GetNamespaceMap().GetQNameByKey(nPrefixKey, rLName);
    mrExport.StartElement(maElementName, bIgnoreWhitespaceOutside);
}

SvXMLElementExport::SvXMLElementExport(
    SvXMLExport& rExp,
    sal_uInt16 nPrefixKey,
    const sal_Char *pLName,
    sal_Bool bIWSOutside,
    sal_Bool bIWSInside )
    : mrExport( rExp )
    , maElementName()
    , mbIgnoreWhitespaceInside( bIWSInside )
    , mbDoSomething( sal_True )
{
    const OUString sLName( OUString::createFromAscii( pLName ) );
    StartElement( nPrefixKey, sLName, bIWSOutside );
}

SvXMLElementExport::SvXMLElementExport(
    SvXMLExport& rExp,
    sal_uInt16 nPrefixKey,
    const OUString& rLName,
    sal_Bool bIWSOutside,
    sal_Bool bIWSInside )
    : mrExport( rExp )
    , maElementName()
    , mbIgnoreWhitespaceInside( bIWSInside )
    , mbDoSomething( sal_True )
{
    StartElement( nPrefixKey, rLName, bIWSOutside );
}

SvXMLElementExport::SvXMLElementExport(
    SvXMLExport& rExp,
    sal_uInt16 nPrefixKey,
    enum XMLTokenEnum eLName,
    sal_Bool bIWSOutside,
    sal_Bool bIWSInside )
    : mrExport( rExp )
    , maElementName()
    , mbIgnoreWhitespaceInside( bIWSInside )
    , mbDoSomething( sal_True )
{
    StartElement( nPrefixKey, GetXMLToken(eLName), bIWSOutside );
}

SvXMLElementExport::SvXMLElementExport(
    SvXMLExport& rExp,
    sal_Bool bDoSth,
    sal_uInt16 nPrefixKey,
    const sal_Char *pLName,
    sal_Bool bIWSOutside,
    sal_Bool bIWSInside )
    : mrExport( rExp )
    , maElementName()
    , mbIgnoreWhitespaceInside( bIWSInside )
    , mbDoSomething( bDoSth )
{
    if ( mbDoSomething )
    {
        const OUString sLName( OUString::createFromAscii( pLName ) );
        StartElement( nPrefixKey, sLName, bIWSOutside );
    }
}

SvXMLElementExport::SvXMLElementExport(
    SvXMLExport& rExp,
    sal_Bool bDoSth,
    sal_uInt16 nPrefixKey,
    const OUString& rLName,
    sal_Bool bIWSOutside,
    sal_Bool bIWSInside )
    : mrExport( rExp )
    , maElementName()
    , mbIgnoreWhitespaceInside( bIWSInside )
    , mbDoSomething( bDoSth )
{
    if ( mbDoSomething )
        StartElement( nPrefixKey, rLName, bIWSOutside );
}

SvXMLElementExport::SvXMLElementExport(
    SvXMLExport& rExp,
    sal_Bool bDoSth,
    sal_uInt16 nPrefixKey,
    enum XMLTokenEnum eLName,
    sal_Bool bIWSOutside,
    sal_Bool bIWSInside )
    : mrExport( rExp )
    , maElementName()
    , mbIgnoreWhitespaceInside( bIWSInside )
    , mbDoSomething( bDoSth )
{
    if ( mbDoSomething )
        StartElement( nPrefixKey, GetXMLToken( eLName ), bIWSOutside );
}

SvXMLElementExport::SvXMLElementExport(
    SvXMLExport& rExp,
    const OUString& rQName,
    sal_Bool bIWSOutside,
    sal_Bool bIWSInside )
    : mrExport( rExp )
    , maElementName()
    , mbIgnoreWhitespaceInside( bIWSInside )
    , mbDoSomething( sal_True )
{
    maElementName = rQName;
    rExp.StartElement( rQName, bIWSOutside );
}

SvXMLElementExport::~SvXMLElementExport()
{
    if ( mbDoSomething )
    {
        mrExport.EndElement( maElementName, mbIgnoreWhitespaceInside );
    }
}

