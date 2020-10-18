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
#include "precompiled_filter.hxx"

#define ITEMID_FIELD 0
 
#include "svgwriter.hxx"
#include "svgfontexport.hxx"
#include "svgfilter.hxx"
#include "impsvgdialog.hxx"

#include <com/sun/star/graphic/XPrimitiveFactory2D.hpp>

#include <svx/unopage.hxx>
#include <svx/unoshape.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdoutl.hxx>
#include <editeng/outliner.hxx>
#include <editeng/flditem.hxx>
#include <editeng/numitem.hxx>

using namespace ::com::sun::star::graphic;
using namespace ::com::sun::star::geometry;
using ::rtl::OUString;

// -------------
// - SVGExport -
// -------------

SVGExport::SVGExport( 
	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
	const Reference< XDocumentHandler >& rxHandler,
    const Sequence< PropertyValue >& rFilterData )
: SvXMLExport( xServiceFactory, MAP_100TH_MM )
, mbTinyProfile			( sal_True )
, mbTSpans				( sal_True )
, mbEmbedFonts			( sal_False )
, mbNativeTextDecoration( sal_True )
, mbOpacity				( sal_True )
, mbGradient			( sal_True )
{
	for ( sal_Int32 i = 0, nCount = rFilterData.getLength(); i < nCount; i++ )
	{
		if ( rFilterData[ i ].Name == B2UCONST( SVG_PROP_TINYPROFILE ) )
			rFilterData[ i ].Value >>= mbTinyProfile;
		else if ( rFilterData[ i ].Name == B2UCONST( "TSpans" ) )
			rFilterData[ i ].Value >>= mbTSpans;
		else if ( rFilterData[ i ].Name == B2UCONST( SVG_PROP_EMBEDFONTS ) )
			rFilterData[ i ].Value >>= mbEmbedFonts;
		else if ( rFilterData[ i ].Name == B2UCONST( SVG_PROP_NATIVEDECORATION ) )
			rFilterData[ i ].Value >>= mbNativeTextDecoration;
		else if ( rFilterData[ i ].Name == B2UCONST( "GlyphPlacement" ) )
			rFilterData[ i ].Value >>= maGlyphPlacement;
		else if ( rFilterData[ i ].Name == B2UCONST( SVG_PROP_OPACITY ) )
			rFilterData[ i ].Value >>= mbOpacity;
		else if ( rFilterData[ i ].Name == B2UCONST( SVG_PROP_GRADIENT ) )
			rFilterData[ i ].Value >>= mbGradient;
	}
	if ( mbTinyProfile )
		mbNativeTextDecoration = sal_False;
	else
	{
		mbTSpans = sal_False;
		mbOpacity = sal_True;
		mbGradient = sal_True;
	}
	SetDocHandler( rxHandler );
	GetDocHandler()->startDocument();
}

// -----------------------------------------------------------------------------

SVGExport::~SVGExport()
{
	GetDocHandler()->endDocument();
}

// -----------------------------------------------------------------------------

void SVGExport::pushClip( const ::basegfx::B2DPolyPolygon& rPolyPoly )
{
    maClipList.push_front( ::basegfx::tools::correctOrientations( rPolyPoly ) );
}
    
// -----------------------------------------------------------------------------

void SVGExport::popClip()
{
    if( !maClipList.empty() )
        maClipList.pop_front();
}
    
// -----------------------------------------------------------------------------

sal_Bool SVGExport::hasClip() const
{
    return( !maClipList.empty() );
}
    
// -----------------------------------------------------------------------------
    
const ::basegfx::B2DPolyPolygon* SVGExport::getCurClip() const
{
    return( maClipList.empty() ? NULL : &( *maClipList.begin() ) );
}

// ------------------------
// - ObjectRepresentation -
// ------------------------

ObjectRepresentation::ObjectRepresentation() :
	mpMtf( NULL )
{
}

// -----------------------------------------------------------------------------

ObjectRepresentation::ObjectRepresentation( const Reference< XInterface >& rxObject, 
											const GDIMetaFile& rMtf ) :
	mxObject( rxObject ),
	mpMtf( new GDIMetaFile( rMtf ) )
{
}

// -----------------------------------------------------------------------------

ObjectRepresentation::ObjectRepresentation( const ObjectRepresentation& rPresentation ) :
	mxObject( rPresentation.mxObject ),
	mpMtf( rPresentation.mpMtf ? new GDIMetaFile( *rPresentation.mpMtf ) : NULL )
{
}

// -----------------------------------------------------------------------------

ObjectRepresentation::~ObjectRepresentation()
{
	delete mpMtf;
}

// -----------------------------------------------------------------------------
									
ObjectRepresentation& ObjectRepresentation::operator=( const ObjectRepresentation& rPresentation )
{
	mxObject = rPresentation.mxObject;
	delete mpMtf, ( mpMtf = rPresentation.mpMtf ? new GDIMetaFile( *rPresentation.mpMtf ) : NULL );

	return *this;
}

// -----------------------------------------------------------------------------

sal_Bool ObjectRepresentation::operator==( const ObjectRepresentation& rPresentation ) const
{
	return( ( mxObject == rPresentation.mxObject ) &&
			( *mpMtf == *rPresentation.mpMtf ) );
}

// -------------
// - SVGFilter -
// -------------

sal_Bool SVGFilter::implExport( const Sequence< PropertyValue >& rDescriptor )
	throw (RuntimeException)
{
    Reference< XMultiServiceFactory >	xServiceFactory( ::comphelper::getProcessServiceFactory() ) ;
    Reference< XOutputStream >          xOStm;
    SvStream*                           pOStm = NULL;
	sal_Int32							nLength = rDescriptor.getLength();
    sal_Int32                           nPageToExport = SVG_EXPORT_ALLPAGES;
	const PropertyValue*				pValue = rDescriptor.getConstArray();
	sal_Bool							bRet = sal_False;
 
    mnMasterSlideId = mnSlideId = mnDrawingGroupId = mnDrawingId = 0;
    maFilterData.realloc( 0 );
 
	for ( sal_Int32 i = 0 ; i < nLength; ++i)
    {
		if( pValue[ i ].Name.equalsAscii( "OutputStream" ) )
			pValue[ i ].Value >>= xOStm;
        else if( pValue[ i ].Name.equalsAscii( "FileName" ) )
        {
            ::rtl::OUString aFileName;

            pValue[ i ].Value >>= aFileName;
            pOStm = ::utl::UcbStreamHelper::CreateStream( aFileName, STREAM_WRITE | STREAM_TRUNC );

            if( pOStm )
                xOStm = Reference< XOutputStream >( new ::utl::OOutputStreamWrapper ( *pOStm ) );
        }
        else if( pValue[ i ].Name.equalsAscii( "PagePos" ) )
        {
            pValue[ i ].Value >>= nPageToExport;
        }
        else if( pValue[ i ].Name.equalsAscii( "FilterData" ) )
        {
            pValue[ i ].Value >>= maFilterData;
        }
        else if( pValue[ i ].Name.equalsAscii( "ShapeSelection" ) )
        {
            // #124608# read selection if given
            pValue[ i ].Value >>= maShapeSelection;
        }
    }
    
    // if no filter data is given use stored/prepared ones
    if( !maFilterData.getLength() )
    {
#ifdef _SVG_USE_CONFIG
        FilterConfigItem aCfgItem( String( RTL_CONSTASCII_USTRINGPARAM( SVG_EXPORTFILTER_CONFIGPATH ) ) );
        
        aCfgItem.ReadBool( String( RTL_CONSTASCII_USTRINGPARAM( SVG_PROP_TINYPROFILE ) ), sal_True );
        aCfgItem.ReadBool( String( RTL_CONSTASCII_USTRINGPARAM( SVG_PROP_EMBEDFONTS ) ), sal_True );
        aCfgItem.ReadBool( String( RTL_CONSTASCII_USTRINGPARAM( SVG_PROP_NATIVEDECORATION ) ), sal_False );
        aCfgItem.ReadString( String( RTL_CONSTASCII_USTRINGPARAM( SVG_PROP_NATIVEDECORATION ) ), B2UCONST( "xlist" ) );
        aCfgItem.ReadString( String( RTL_CONSTASCII_USTRINGPARAM( SVG_PROP_OPACITY ) ), sal_True );
        aCfgItem.ReadString( String( RTL_CONSTASCII_USTRINGPARAM( SVG_PROP_GRADIENT ) ), sal_True );
        
        maFilterData = aCfgItem.GetFilterData();
#else
        maFilterData.realloc( 6 );
        
        maFilterData[ 0 ].Name = B2UCONST( SVG_PROP_TINYPROFILE );
        maFilterData[ 0 ].Value <<= (sal_Bool) sal_True;
        
        // font embedding
        const char* pSVGDisableFontEmbedding = getenv( "SVG_DISABLE_FONT_EMBEDDING" );
        
        maFilterData[ 1 ].Name = B2UCONST( SVG_PROP_EMBEDFONTS );
        maFilterData[ 1 ].Value <<= (sal_Bool) ( pSVGDisableFontEmbedding ? sal_False : sal_True );
        
        // Native decoration
        maFilterData[ 2 ].Name = B2UCONST( SVG_PROP_NATIVEDECORATION );
        maFilterData[ 2 ].Value <<= (sal_Bool) sal_False;

        // glyph placement
        const char* pSVGGlyphPlacement = getenv( "SVG_GLYPH_PLACEMENT" );
        
        maFilterData[ 3 ].Name = B2UCONST( SVG_PROP_GLYPHPLACEMENT );

        if( pSVGGlyphPlacement )
            maFilterData[ 3 ].Value <<= ::rtl::OUString::createFromAscii( pSVGGlyphPlacement );
        else
            maFilterData[ 3 ].Value <<= B2UCONST( "xlist" );

        // Tiny Opacity
        maFilterData[ 4 ].Name = B2UCONST( SVG_PROP_OPACITY );
        maFilterData[ 4 ].Value <<= (sal_Bool) sal_True;

        // Tiny Gradient
        maFilterData[ 5 ].Name = B2UCONST( SVG_PROP_GRADIENT );
        maFilterData[ 5 ].Value <<= (sal_Bool) sal_False;
#endif
    }
   
    if( xOStm.is() && xServiceFactory.is() )
    {
		Reference< XMasterPagesSupplier >	xMasterPagesSupplier( mxSrcDoc, UNO_QUERY );
		Reference< XDrawPagesSupplier >		xDrawPagesSupplier( mxSrcDoc, UNO_QUERY );
	
		if( xMasterPagesSupplier.is() && xDrawPagesSupplier.is() )
		{
			Reference< XDrawPages >   xMasterPages( xMasterPagesSupplier->getMasterPages(), UNO_QUERY );
			Reference< XDrawPages >   xDrawPages( xDrawPagesSupplier->getDrawPages(), UNO_QUERY );

			if( xMasterPages.is() && xMasterPages->getCount() && xDrawPages.is() && xDrawPages->getCount() )
			{
                Reference< XDocumentHandler > xDocHandler( implCreateExportDocumentHandler( xOStm ) );

                if( xDocHandler.is() )
                {
					mbPresentation = Reference< XPresentationSupplier >( mxSrcDoc, UNO_QUERY ).is();
					mpObjects = new ObjectMap;

					// #110680#
					// mpSVGExport = new SVGExport( xDocHandler );
                    mpSVGExport = new SVGExport( xServiceFactory, xDocHandler, maFilterData );

                    if( nPageToExport < 0 || nPageToExport >= xDrawPages->getCount() )
                        nPageToExport = SVG_EXPORT_ALLPAGES;

					try
					{
    					const sal_Int32 nDefaultPage = ( ( SVG_EXPORT_ALLPAGES == nPageToExport ) ? 0 : nPageToExport );
						
						xDrawPages->getByIndex( nDefaultPage ) >>= mxDefaultPage;
						
						if( mxDefaultPage.is() )
						{
							SvxDrawPage* pSvxDrawPage = SvxDrawPage::getImplementation( mxDefaultPage );

							if( pSvxDrawPage )
							{
								mpDefaultSdrPage = pSvxDrawPage->GetSdrPage();
								mpSdrModel = mpDefaultSdrPage->GetModel();
								
								if( mpSdrModel )
								{
									SdrOutliner& rOutl = mpSdrModel->GetDrawOutliner(NULL);
									
									maOldFieldHdl = rOutl.GetCalcFieldValueHdl();
									rOutl.SetCalcFieldValueHdl( LINK( this, SVGFilter, CalcFieldHdl) );
								}
							}
							
							if( implCreateObjects( xMasterPages, xDrawPages, nPageToExport ) )
							{
								ObjectMap::const_iterator				aIter( mpObjects->begin() );
								::std::vector< ObjectRepresentation >	aObjects( mpObjects->size() );
								sal_uInt32								nPos = 0;
	
								while( aIter != mpObjects->end() )
								{
									aObjects[ nPos++ ] = (*aIter).second;
									++aIter;
								}
	
								mpSVGFontExport = new SVGFontExport( *mpSVGExport, aObjects );
								mpSVGWriter = new SVGActionWriter( *mpSVGExport, *mpSVGFontExport );
								
								bRet = implExportDocument( xMasterPages, xDrawPages, nPageToExport );
							}
						}
					}
					catch( ... )
					{
						delete mpSVGDoc, mpSVGDoc = NULL;
						DBG_ERROR( "Exception caught" );
					}
					
					if( mpSdrModel )
						mpSdrModel->GetDrawOutliner( NULL ).SetCalcFieldValueHdl( maOldFieldHdl );
                    
					delete mpSVGWriter, mpSVGWriter = NULL;
                    delete mpSVGExport, mpSVGExport = NULL;
					delete mpSVGFontExport, mpSVGFontExport = NULL;
					delete mpObjects, mpObjects = NULL;
                    mbPresentation = sal_False;
                }
			}
		}
	}

    delete pOStm;
    
	return bRet;
}

// -----------------------------------------------------------------------------

Reference< XDocumentHandler > SVGFilter::implCreateExportDocumentHandler( const Reference< XOutputStream >& rxOStm )
{
	Reference< XMultiServiceFactory >   xMgr( ::comphelper::getProcessServiceFactory() );
    Reference< XDocumentHandler >       xSaxWriter;

	if( xMgr.is() && rxOStm.is() )
	{
		xSaxWriter = Reference< XDocumentHandler >( xMgr->createInstance( B2UCONST( "com.sun.star.xml.sax.Writer" ) ), UNO_QUERY );

		if( xSaxWriter.is() )
		{
			Reference< XActiveDataSource > xActiveDataSource( xSaxWriter, UNO_QUERY );

			if( xActiveDataSource.is() )
				xActiveDataSource->setOutputStream( rxOStm );
            else
                xSaxWriter = NULL;
		}
	}

    return xSaxWriter;
}

// -----------------------------------------------------------------------------

sal_Bool SVGFilter::implExportDocument( const Reference< XDrawPages >& rxMasterPages,
										const Reference< XDrawPages >& rxDrawPages,
                                        sal_Int32 nPageToExport )
{
	DBG_ASSERT( rxMasterPages.is() && rxDrawPages.is(), 
				"SVGFilter::implExportDocument: invalid parameter" );

	OUString		aAttr;
	sal_Int32		nDocX = 0, nDocY = 0; // #124608#
	sal_Int32		nDocWidth = 0, nDocHeight = 0;
	sal_Int32		nVisible = -1, nVisibleMaster = -1;
	sal_Bool 		bRet = sal_False;
	const sal_Bool	bSinglePage = ( rxDrawPages->getCount() == 1 ) || ( SVG_EXPORT_ALLPAGES != nPageToExport );
    const sal_Int32 nFirstPage = ( ( SVG_EXPORT_ALLPAGES == nPageToExport ) ? 0 : nPageToExport );
	sal_Int32       nCurPage = nFirstPage, nLastPage = ( bSinglePage ? nFirstPage : ( rxDrawPages->getCount() - 1 ) );

	const Reference< XPropertySet >             xDefaultPagePropertySet( mxDefaultPage, UNO_QUERY );
    const Reference< XExtendedDocumentHandler > xExtDocHandler( mpSVGExport->GetDocHandler(), UNO_QUERY );

    // #124608#
    mbExportSelection = maShapeSelection.is() && maShapeSelection->getCount();

    if(xDefaultPagePropertySet.is())
    {
        xDefaultPagePropertySet->getPropertyValue(B2UCONST("Width")) >>= nDocWidth;
        xDefaultPagePropertySet->getPropertyValue(B2UCONST("Height")) >>= nDocHeight;
    }

    if(mbExportSelection)
    {
        // #124608# create BoundRange and set nDocX, nDocY, nDocWidth and nDocHeight
        basegfx::B2DRange aShapeRange;

        Reference< XPrimitiveFactory2D > xPrimitiveFactory(
            mxMSF->createInstance(
                String(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.graphic.PrimitiveFactory2D"))),
                UNO_QUERY);

        // use XPrimitiveFactory2D and go the way over getting the primitives; this
        // will give better precision (doubles) and be based on the true object
        // geometry. If needed aViewInformation may be expanded to carry a view
        // resolution for which to prepare the geometry.
        if(xPrimitiveFactory.is())
        {
            Reference< XShape > xShapeCandidate; 
            const Sequence< PropertyValue > aViewInformation;
            const Sequence< PropertyValue > aParams;

            for(sal_Int32 a(0); a < maShapeSelection->getCount(); a++)
            {
                if((maShapeSelection->getByIndex(a) >>= xShapeCandidate) && xShapeCandidate.is())
                {
                    const Sequence< Reference< XPrimitive2D > > aPrimitiveSequence(
                        xPrimitiveFactory->createPrimitivesFromXShape( xShapeCandidate, aParams ));
                    const sal_Int32 nCount(aPrimitiveSequence.getLength());

                    for(sal_Int32 nIndex = 0; nIndex < nCount; nIndex++)
                    {
                        const RealRectangle2D aRect(aPrimitiveSequence[nIndex]->getRange(aViewInformation));

                        aShapeRange.expand(basegfx::B2DTuple(aRect.X1, aRect.Y1));
                        aShapeRange.expand(basegfx::B2DTuple(aRect.X2, aRect.Y2));
                    }
                }
            }
        }

        if(!aShapeRange.isEmpty())
        {
            nDocX = basegfx::fround(aShapeRange.getMinX());
            nDocY = basegfx::fround(aShapeRange.getMinY());
            nDocWidth  = basegfx::fround(aShapeRange.getWidth());
            nDocHeight = basegfx::fround(aShapeRange.getHeight());
        }
    }
      
    if( xExtDocHandler.is() && !mpSVGExport->IsUseTinyProfile() )
    {
        xExtDocHandler->unknown( SVG_DTD_STRING );
    }

    mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "version", B2UCONST( "1.2" ) );
	
    if( mpSVGExport->IsUseTinyProfile() )
         mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "baseProfile", B2UCONST( "tiny" ) );

#ifdef _SVG_WRITE_EXTENTS
	aAttr = OUString::valueOf( nDocWidth * 0.01 ); 
	aAttr += B2UCONST( "mm" );
	mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "width", aAttr );

	aAttr = OUString::valueOf( nDocHeight * 0.01 ); 
	aAttr += B2UCONST( "mm" );
	mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "height", aAttr );
#endif

    // #124608# set viewBox explicitly to the exported content
    if(mbExportSelection)
    {
        aAttr = OUString::valueOf( nDocX );
        aAttr += B2UCONST( " " );
        aAttr += OUString::valueOf( nDocY );
        aAttr += B2UCONST( " " );
    }
    else
    {
        aAttr = B2UCONST( "0 0 " );
    }

    aAttr += OUString::valueOf( nDocWidth );
    aAttr += B2UCONST( " " );
    aAttr += OUString::valueOf( nDocHeight );
	mpSVGExport->SetViewBox( Rectangle( Point(nDocX, nDocY), Size( nDocWidth, nDocHeight ) ) );
    mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "viewBox", aAttr );
	mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "preserveAspectRatio", B2UCONST( "xMidYMid" ) );
	mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "fill-rule", B2UCONST( "evenodd" ) );

    // standard line width is based on 1 pixel on a 90 DPI device (0.28222mmm)
    mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "stroke-width", OUString::valueOf( 28.222 ) );
    mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "stroke-linejoin", B2UCONST( "round" ) );

	if( !bSinglePage )
	{
		mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "xmlns:ooo", B2UCONST( "http://xml.openoffice.org/svg/export" ) );
		mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "onclick", B2UCONST( "onClick(evt)" ) );
		mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "onkeypress", B2UCONST( "onKeyPress(evt)" ) );
	}

	mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "xmlns", B2UCONST( "http://www.w3.org/2000/svg" ) );
	mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "xmlns:xlink", B2UCONST( "http://www.w3.org/1999/xlink" ) );
 
    mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "xml:space", B2UCONST( "preserve" ) );

	mpSVGDoc = new SvXMLElementExport( *mpSVGExport, XML_NAMESPACE_NONE, "svg", sal_True, sal_True );

	while( ( nCurPage <= nLastPage ) && ( -1 == nVisible ) )
	{
		Reference< XDrawPage > xDrawPage;

		rxDrawPages->getByIndex( nCurPage ) >>= xDrawPage;

		if( xDrawPage.is() )
		{
			Reference< XPropertySet > xPropSet( xDrawPage, UNO_QUERY );

			if( xPropSet.is() )
			{
				sal_Bool bVisible = sal_False;

				if( !mbPresentation || bSinglePage ||
					( ( xPropSet->getPropertyValue( B2UCONST( "Visible" ) ) >>= bVisible ) && bVisible ) )
				{
					Reference< XMasterPageTarget > xMasterTarget( xDrawPage, UNO_QUERY );

					if( xMasterTarget.is() )
					{
						Reference< XDrawPage > xMasterPage( xMasterTarget->getMasterPage() );

						nVisible = nCurPage;

						for( sal_Int32 nMaster = 0, nMasterCount = rxMasterPages->getCount(); 
							 ( nMaster < nMasterCount ) && ( -1 == nVisibleMaster ); 
							 ++nMaster )
						{
							Reference< XDrawPage > xMasterTestPage;
							
							rxMasterPages->getByIndex( nMaster ) >>= xMasterTestPage;

							if( xMasterTestPage == xMasterPage )
								nVisibleMaster = nMaster;
						}
					}
				}
			}
		}

		++nCurPage;
	}
 
    if( mpSVGExport->IsEmbedFonts() )
    {
        mpSVGFontExport->EmbedFonts();
    }

	if( -1 != nVisible )
	{
        if(bSinglePage)
        {
            if(mbExportSelection)
            {
                // #124608# export a given object selection, so no MasterPage export at all
            }
            else
            {
                implExportPages(rxMasterPages,nVisibleMaster,nVisibleMaster,nVisibleMaster,sal_True);
            }
        }
        else
        {
			implGenerateMetaData( rxMasterPages, rxDrawPages );
			implGenerateScript( rxMasterPages, rxDrawPages );
			implExportPages( rxMasterPages, 0, rxMasterPages->getCount() - 1, nVisibleMaster, sal_True );
		}

		implExportPages( rxDrawPages, nFirstPage, nLastPage, nVisible, sal_False );

		delete mpSVGDoc, mpSVGDoc = NULL;
		bRet = sal_True;
	}

    return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool SVGFilter::implGenerateMetaData( const Reference< XDrawPages >& /* rxMasterPages */, 
										  const Reference< XDrawPages >& rxDrawPages )
{
	sal_Bool bRet = sal_False;

	if( rxDrawPages->getCount() )
	{
		mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "id", B2UCONST( "meta_slides" ) );
		mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "numberOfSlides", OUString::valueOf( rxDrawPages->getCount() ) );

		{
			SvXMLElementExport	aExp( *mpSVGExport, XML_NAMESPACE_NONE, "ooo:slidesInfo", sal_True, sal_True );
			const OUString		aId( B2UCONST( "meta_slide" ) );

			for( sal_Int32 i = 0, nCount = rxDrawPages->getCount(); i < nCount; ++i )
			{
				OUString						aSlideId( aId );
				Reference< XDrawPage >			xDrawPage( rxDrawPages->getByIndex( i ), UNO_QUERY );
				Reference< XMasterPageTarget >	xMasterPageTarget( xDrawPage, UNO_QUERY );
				Reference< XDrawPage >			xMasterPage( xMasterPageTarget->getMasterPage(), UNO_QUERY );
                sal_Bool                        bMasterVisible = sal_True;
                OUString                        aMasterVisibility;

				aSlideId += OUString::valueOf( i );

                if( mbPresentation )
                {
                    Reference< XPropertySet > xPropSet( xDrawPage, UNO_QUERY );

                    if( xPropSet.is() )
                        xPropSet->getPropertyValue( B2UCONST( "Background" ) )  >>= bMasterVisible;
                }

                if( bMasterVisible )
                    aMasterVisibility = B2UCONST( "visible" );
                else
                    aMasterVisibility = B2UCONST( "hidden" );
				
                mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "id", aSlideId );	
				mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "slide", implGetValidIDFromInterface( xDrawPage ) );
				mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "master", implGetValidIDFromInterface( xMasterPage ) );
				mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "master-visibility", aMasterVisibility );

				{
					SvXMLElementExport aExp2( *mpSVGExport, XML_NAMESPACE_NONE, "ooo:slideInfo", sal_True, sal_True );
				}
			}
		}
		

		bRet =sal_True;
	}

	return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool SVGFilter::implGenerateScript( const Reference< XDrawPages >& /* rxMasterPages */, 
                                        const Reference< XDrawPages >& /* rxDrawPages */ )
{
    mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "type", B2UCONST( "text/ecmascript" ) );

    {
        SvXMLElementExport	                    aExp( *mpSVGExport, XML_NAMESPACE_NONE, "script", sal_True, sal_True );
        Reference< XExtendedDocumentHandler >	xExtDocHandler( mpSVGExport->GetDocHandler(), UNO_QUERY );

        if( xExtDocHandler.is() )
        {
            xExtDocHandler->unknown( OUString::createFromAscii( aSVGScript1 ) );
            xExtDocHandler->unknown( OUString::createFromAscii( aSVGScript2 ) );
        }
    }

    return sal_True;
}

// -----------------------------------------------------------------------------

sal_Bool SVGFilter::implExportPages( const Reference< XDrawPages >& rxPages,
                                     sal_Int32 nFirstPage, sal_Int32 nLastPage, 
									 sal_Int32 nVisiblePage, sal_Bool bMaster )
{
    DBG_ASSERT( nFirstPage <= nLastPage,
                "SVGFilter::implExportPages: nFirstPage > nLastPage" );

	sal_Bool bRet = sal_False;

	for( sal_Int32 i = nFirstPage; i <= nLastPage; ++i )
	{
		Reference< XDrawPage > xDrawPage;

		rxPages->getByIndex( i ) >>= xDrawPage;

		if( xDrawPage.is() )
		{
            Reference< XShapes > xShapes;

            if(mbExportSelection)
            {
                // #124608# export a given object selection
                xShapes = maShapeSelection;
            }
            else
            {
                xShapes = Reference< XShapes >( xDrawPage, UNO_QUERY );
            }

			if( xShapes.is() ) 
			{
                OUString aVisibility, aId, aSlideName( implGetValidIDFromInterface( xShapes, sal_True ) );

	            // add visibility attribute			
                if( i == nVisiblePage )
					aVisibility = B2UCONST( "visible" );
				else
					aVisibility = B2UCONST( "hidden" );

	            mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "visibility", aVisibility );
                
                // add id attribute         
                if( bMaster )
                    aId = ( B2UCONST( "MasterSlide_" ) ) += ::rtl::OUString::valueOf( ++mnMasterSlideId );
                else
                    aId = ( B2UCONST( "Slide_" ) ) += ::rtl::OUString::valueOf( ++mnSlideId );

                if( aSlideName.getLength() )
                    ( aId += B2UCONST( "_" ) ) += aSlideName;
                
                mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "id", aId );

				{
					SvXMLElementExport	aExp( *mpSVGExport, XML_NAMESPACE_NONE, "g", sal_True, sal_True );
					const Point			aNullPt;

                    if( mpObjects->find( xDrawPage ) != mpObjects->end() )
					{
						Reference< XExtendedDocumentHandler > xExtDocHandler( mpSVGExport->GetDocHandler(), UNO_QUERY );

						if( xExtDocHandler.is() )
						{
							SvXMLElementExport	aExp2( *mpSVGExport, XML_NAMESPACE_NONE, "desc", sal_True, sal_True );
							OUString			aDesc;

							if( bMaster )
                                aDesc = B2UCONST( "Master slide" );
							else
								aDesc = B2UCONST( "Slide" );

							xExtDocHandler->unknown( aDesc );
						}
					}

					if( bMaster )
					{
						const GDIMetaFile& rMtf = (*mpObjects)[ xDrawPage ].GetRepresentation();
						mpSVGWriter->WriteMetaFile( aNullPt, rMtf.GetPrefSize(), rMtf, SVGWRITER_WRITE_FILL );
					}

					bRet = implExportShapes( xShapes ) || bRet;
				}
			}
		}
	}

    return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool SVGFilter::implExportShapes( const Reference< XShapes >& rxShapes )
{
    Reference< XShape > xShape; 
	sal_Bool			bRet = sal_False;

    for( sal_Int32 i = 0, nCount = rxShapes->getCount(); i < nCount; ++i )
    {
        if( ( rxShapes->getByIndex( i ) >>= xShape ) && xShape.is() )
			bRet = implExportShape( xShape ) || bRet;

        xShape = NULL;
    }

	return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool SVGFilter::implExportShape( const Reference< XShape >& rxShape )
{
    Reference< XPropertySet >   xShapePropSet( rxShape, UNO_QUERY );
    sal_Bool                    bRet = sal_False;

    if( xShapePropSet.is() )
    {
        const ::rtl::OUString   aShapeType( rxShape->getShapeType() );
        sal_Bool                    bHideObj = sal_False;
        
        if( mbPresentation )
        {    
            xShapePropSet->getPropertyValue( B2UCONST( "IsEmptyPresentationObject" ) )  >>= bHideObj;
            
            if( !bHideObj )
            {
				const Reference< XPropertySet > xDefaultPagePropertySet( mxDefaultPage, UNO_QUERY );
				Reference< XPropertySetInfo > 	xPagePropSetInfo( xDefaultPagePropertySet->getPropertySetInfo() );
                
                if( xPagePropSetInfo.is() )
                {
                    const ::rtl::OUString aHeaderString( B2UCONST( "IsHeaderVisible" ) );
                    const ::rtl::OUString aFooterString( B2UCONST( "IsFooterVisible" ) );
                    const ::rtl::OUString aDateTimeString( B2UCONST( "IsDateTimeVisible" ) );
                    const ::rtl::OUString aPageNumberString( B2UCONST( "IsPageNumberVisible" ) );
                    
                    Any     aProperty;
                    sal_Bool    bValue = sal_False;
                
                    if( ( aShapeType.lastIndexOf( B2UCONST( "presentation.HeaderShape" ) ) != -1 ) &&
                        xPagePropSetInfo->hasPropertyByName( aHeaderString ) &&
                        ( ( aProperty = xDefaultPagePropertySet->getPropertyValue( aHeaderString ) ) >>= bValue ) &&
                        !bValue )
                    {
                        bHideObj = sal_True;
                    }   
                    else if( ( aShapeType.lastIndexOf( B2UCONST( "presentation.FooterShape" ) ) != -1 ) &&
                                xPagePropSetInfo->hasPropertyByName( aFooterString ) &&
                                ( ( aProperty = xDefaultPagePropertySet->getPropertyValue( aFooterString ) ) >>= bValue ) &&
                            !bValue )
                    {
                        bHideObj = sal_True;
                    }   
                    else if( ( aShapeType.lastIndexOf( B2UCONST( "presentation.DateTimeShape" ) ) != -1 ) &&
                                xPagePropSetInfo->hasPropertyByName( aDateTimeString ) &&
                                ( ( aProperty = xDefaultPagePropertySet->getPropertyValue( aDateTimeString ) ) >>= bValue ) &&
                            !bValue )
                    {
                        bHideObj = sal_True;
                    }   
                    else if( ( aShapeType.lastIndexOf( B2UCONST( "presentation.SlideNumberShape" ) ) != -1 ) &&
                                xPagePropSetInfo->hasPropertyByName( aPageNumberString ) &&
                                ( ( aProperty = xDefaultPagePropertySet->getPropertyValue( aPageNumberString ) ) >>= bValue ) &&
                            !bValue )
                    {
                        bHideObj = sal_True;
                    }   
                } 
            }
        }

        if( !bHideObj )
        {
		    if( aShapeType.lastIndexOf( B2UCONST( "drawing.GroupShape" ) ) != -1 )
		    {
			    Reference< XShapes > xShapes( rxShape, UNO_QUERY );

			    if( xShapes.is() )
			    {
                    OUString    aId( B2UCONST( "DrawingGroup_" ) );
                    OUString    aObjName( implGetValidIDFromInterface( rxShape, sal_True ) ), aObjDesc;
                    
                    aId += ::rtl::OUString::valueOf( ++mnDrawingGroupId );

                    if( aObjName.getLength() )
                        ( aId += B2UCONST( "_" ) ) += aObjName;
                
                    mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "id", aId );
				
                    {
						SvXMLElementExport aExp( *mpSVGExport, XML_NAMESPACE_NONE, "g", sal_True, sal_True );
        			    bRet = implExportShapes( xShapes );
                    }         
                }
		    }

		    if( !bRet && mpObjects->find( rxShape ) !=  mpObjects->end() )
		    {
 			    Reference< XText >                  xText( rxShape, UNO_QUERY );
				::com::sun::star::awt::Rectangle    aBoundRect;
				const GDIMetaFile&					rMtf = (*mpObjects)[ rxShape ].GetRepresentation();
            
                xShapePropSet->getPropertyValue( B2UCONST( "BoundRect" ) ) >>= aBoundRect;
                
                const Point aTopLeft( aBoundRect.X, aBoundRect.Y );
                const Size  aSize( aBoundRect.Width, aBoundRect.Height );

                if( rMtf.GetActionCount() )
                {
                    OUString aId( B2UCONST( "Drawing_" ) );
                    OUString aObjName( implGetValidIDFromInterface( rxShape, sal_True ) ), aObjDesc;

                    aId += ::rtl::OUString::valueOf( ++mnDrawingId );

                    if( aObjName.getLength() )
                        ( aId += B2UCONST( "_" ) ) += aObjName;
                
                    {
                        if( ( aShapeType.lastIndexOf( B2UCONST( "drawing.OLE2Shape" ) ) != -1 ) ||
                            ( aShapeType.lastIndexOf( B2UCONST( "drawing.GraphicObjectShape" ) ) != -1 ) )
                        {
                            mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "id", aId );
                            
                            {
                                SvXMLElementExport aExp( *mpSVGExport, XML_NAMESPACE_NONE, "g", sal_True, sal_True );
                                mpSVGWriter->WriteMetaFile( aTopLeft, aSize, rMtf, SVGWRITER_WRITE_FILL | SVGWRITER_WRITE_TEXT );
                            }
                        }
                        else 
                        {
                            if( implHasText( rMtf ) )
                            {
                                mpSVGExport->AddAttribute( XML_NAMESPACE_NONE, "id", aId );
                                
                                {
                                    SvXMLElementExport aExp( *mpSVGExport, XML_NAMESPACE_NONE, "g", sal_True, sal_True );
                                    mpSVGWriter->WriteMetaFile( aTopLeft, aSize, rMtf, SVGWRITER_WRITE_FILL );
                                    mpSVGWriter->WriteMetaFile( aTopLeft, aSize, rMtf, SVGWRITER_WRITE_TEXT );
                                }
                            }
                            else
                            {
                                SvXMLElementExport aExp( *mpSVGExport, XML_NAMESPACE_NONE, "g", sal_True, sal_True );
                                mpSVGWriter->WriteMetaFile( aTopLeft, aSize, rMtf, SVGWRITER_WRITE_FILL | SVGWRITER_WRITE_TEXT, &aId );
                            }
                        }
                    }
                }
       
				bRet = sal_True;
		    }
        }
    }

    return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool SVGFilter::implCreateObjects( const Reference< XDrawPages >& rxMasterPages,
								       const Reference< XDrawPages >& rxDrawPages,
                                       sal_Int32 nPageToExport )
{
    if( SVG_EXPORT_ALLPAGES == nPageToExport )
    {
        // export the whole document
	    sal_Int32 i, nCount;

        for( i = 0, nCount = rxMasterPages->getCount(); i < nCount; ++i )
    	{
    		Reference< XDrawPage > xMasterPage;

    		rxMasterPages->getByIndex( i ) >>= xMasterPage;

    		if( xMasterPage.is() )
    		{
    			Reference< XShapes > xShapes( xMasterPage, UNO_QUERY );

    			implCreateObjectsFromBackground( xMasterPage );

    			if( xShapes.is() )
    				implCreateObjectsFromShapes( xShapes );
    		}
    	}

    	for( i = 0, nCount = rxDrawPages->getCount(); i < nCount; ++i )
    	{
    		Reference< XDrawPage > xDrawPage;

    		rxDrawPages->getByIndex( i ) >>= xDrawPage;

    		if( xDrawPage.is() )
    		{
    			Reference< XShapes > xShapes( xDrawPage, UNO_QUERY );

    			if( xShapes.is() )
    				implCreateObjectsFromShapes( xShapes );
    		}
    	}
    }
    else
    {
        DBG_ASSERT( nPageToExport >= 0 && nPageToExport < rxDrawPages->getCount(),
                    "SVGFilter::implCreateObjects: invalid page number to export" );

        if(mbExportSelection)
        {
            // #124608# export a given object selection
            implCreateObjectsFromShapes(maShapeSelection);
        }
        else
        {
            // export a given xDrawPage
            Reference< XDrawPage > xDrawPage;

            rxDrawPages->getByIndex(nPageToExport) >>= xDrawPage;

            if(xDrawPage.is())
            {
                Reference< XMasterPageTarget > xMasterTarget(xDrawPage,UNO_QUERY);

                if(xMasterTarget.is())
                {
                    Reference< XDrawPage > xMasterPage(xMasterTarget->getMasterPage());

                    if(xMasterPage.is())
                    {
                        Reference< XShapes > xShapes(xMasterPage,UNO_QUERY);

                        implCreateObjectsFromBackground(xMasterPage);

                        if(xShapes.is())
                            implCreateObjectsFromShapes(xShapes);
                    }
                }

                Reference< XShapes > xShapes(xDrawPage,UNO_QUERY);

                if(xShapes.is())
                    implCreateObjectsFromShapes(xShapes);
            }
        }
    }

	return sal_True;
}

// -----------------------------------------------------------------------------

sal_Bool SVGFilter::implCreateObjectsFromShapes( const Reference< XShapes >& rxShapes )
{
    Reference< XShape > xShape; 
	sal_Bool			bRet = sal_False;

    for( sal_Int32 i = 0, nCount = rxShapes->getCount(); i < nCount; ++i )
    {
        if( ( rxShapes->getByIndex( i ) >>= xShape ) && xShape.is() )
			bRet = implCreateObjectsFromShape( xShape ) || bRet;

        xShape = NULL;
    }

	return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool SVGFilter::implCreateObjectsFromShape( const Reference< XShape >& rxShape )
{
    sal_Bool bRet = sal_False;

	if( rxShape->getShapeType().lastIndexOf( B2UCONST( "drawing.GroupShape" ) ) != -1 )
	{
		Reference< XShapes > xShapes( rxShape, UNO_QUERY );

		if( xShapes.is() )
			bRet = implCreateObjectsFromShapes( xShapes );
	}
	else
	{
		SdrObject*  pObj = GetSdrObjectFromXShape( rxShape );

		if( pObj )
		{
			Graphic aGraphic( SdrExchangeView::GetObjGraphic( pObj->GetModel(), pObj ) );

			if( aGraphic.GetType() != GRAPHIC_NONE )
			{
				if( aGraphic.GetType() == GRAPHIC_BITMAP )
				{
					GDIMetaFile	aMtf;
					const Point	aNullPt;
					const Size	aSize( pObj->GetCurrentBoundRect().GetSize() );

					aMtf.AddAction( new MetaBmpExScaleAction( aNullPt, aSize, aGraphic.GetBitmapEx() ) );
					aMtf.SetPrefSize( aSize );
					aMtf.SetPrefMapMode( MAP_100TH_MM );

					(*mpObjects)[ rxShape ] = ObjectRepresentation( rxShape, aMtf );
				}
				else
					(*mpObjects)[ rxShape ] = ObjectRepresentation( rxShape, aGraphic.GetGDIMetaFile() );

				bRet = sal_True;
			}
		}
	}

    return bRet;
}

// -----------------------------------------------------------------------------

sal_Bool SVGFilter::implCreateObjectsFromBackground( const Reference< XDrawPage >& rxMasterPage )
{
	Reference< XExporter >	xExporter( mxMSF->createInstance( B2UCONST( "com.sun.star.drawing.GraphicExportFilter" ) ), UNO_QUERY );
	sal_Bool				bRet = sal_False;

	if( xExporter.is() )
	{
		GDIMetaFile				aMtf;
		Reference< XFilter >	xFilter( xExporter, UNO_QUERY );

		utl::TempFile aFile;
		aFile.EnableKillingFile();
	
		Sequence< PropertyValue > aDescriptor( 3 );
		aDescriptor[0].Name = B2UCONST( "FilterName" );
		aDescriptor[0].Value <<= B2UCONST( "SVM" );
		aDescriptor[1].Name = B2UCONST( "URL" );
		aDescriptor[1].Value <<= OUString( aFile.GetURL() );
		aDescriptor[2].Name = B2UCONST( "ExportOnlyBackground" );
		aDescriptor[2].Value <<= (sal_Bool) sal_True;
	
		xExporter->setSourceDocument( Reference< XComponent >( rxMasterPage, UNO_QUERY ) );
		xFilter->filter( aDescriptor );
		aMtf.Read( *aFile.GetStream( STREAM_READ ) );

		(*mpObjects)[ rxMasterPage ] = ObjectRepresentation( rxMasterPage, aMtf );

		bRet = sal_True;
	}

	return bRet;
}

// -----------------------------------------------------------------------------

OUString SVGFilter::implGetDescriptionFromShape( const Reference< XShape >& rxShape )
{
	OUString			aRet;
    const OUString      aShapeType( rxShape->getShapeType() );

	if( aShapeType.lastIndexOf( B2UCONST( "drawing.GroupShape" ) ) != -1 )
		aRet = B2UCONST( "Group" );
	else if( aShapeType.lastIndexOf( B2UCONST( "drawing.GraphicObjectShape" ) ) != -1 )
		aRet = B2UCONST( "Graphic" );
	else if( aShapeType.lastIndexOf( B2UCONST( "drawing.OLE2Shape" ) ) != -1 )
		aRet = B2UCONST( "OLE2" );
    else if( aShapeType.lastIndexOf( B2UCONST( "presentation.HeaderShape" ) ) != -1 )
		aRet = B2UCONST( "Header" );
    else if( aShapeType.lastIndexOf( B2UCONST( "presentation.FooterShape" ) ) != -1 )
		aRet = B2UCONST( "Footer" );
    else if( aShapeType.lastIndexOf( B2UCONST( "presentation.DateTimeShape" ) ) != -1 )
		aRet = B2UCONST( "Date/Time" );
    else if( aShapeType.lastIndexOf( B2UCONST( "presentation.SlideNumberShape" ) ) != -1 )
		aRet = B2UCONST( "Slide Number" );
	else
		aRet = B2UCONST( "Drawing" );
		
	return aRet;
}

// -----------------------------------------------------------------------------

OUString SVGFilter::implGetValidIDFromInterface( const Reference< XInterface >& rxIf, sal_Bool bUnique )
{
	Reference< XNamed > xNamed( rxIf, UNO_QUERY );
	OUString			aRet;

	if( xNamed.is() )
    {
        aRet = xNamed->getName().replace( ' ', '_' ).
               replace( ':', '_' ).
               replace( ',', '_' ).
               replace( ';', '_' ).
               replace( '&', '_' ).
               replace( '!', '_' ).
               replace( '|', '_' );
    }
 
    if( ( aRet.getLength() > 0 ) && bUnique )
    {
        while( ::std::find( maUniqueIdVector.begin(), maUniqueIdVector.end(), aRet ) != maUniqueIdVector.end() )
        {
            aRet += B2UCONST( "_" );
        }
        
        maUniqueIdVector.push_back( aRet );
    }

	return aRet;
}

// -----------------------------------------------------------------------------

sal_Bool SVGFilter::implHasText( const GDIMetaFile& rMtf ) const
{
    sal_Bool bRet = sal_False;
    
    for( sal_uInt32 nCurAction = 0, nCount = rMtf.GetActionCount(); ( nCurAction < nCount ) && !bRet; ++nCurAction )
    {
        switch( rMtf.GetAction( nCurAction )->GetType() )
        {
            case( META_TEXT_ACTION ):
            case( META_TEXTRECT_ACTION ):
            case( META_TEXTARRAY_ACTION ):
            case( META_STRETCHTEXT_ACTION ):
            {
                bRet = sal_True;
            }
            break;
            
            default:
            break;
        }
    }
    
    return bRet;
}

// -----------------------------------------------------------------------------

IMPL_LINK( SVGFilter, CalcFieldHdl, EditFieldInfo*, pInfo )
{
	OUString   aRepresentation;
    sal_Bool       bFieldProcessed = sal_False;

	if( pInfo )
	{
		const ::rtl::OUString aHeaderText( B2UCONST( "HeaderText" ) );
		const ::rtl::OUString aFooterText( B2UCONST( "FooterText" ) );
		const ::rtl::OUString aDateTimeText( B2UCONST( "DateTimeText" ) );
		const ::rtl::OUString aPageNumberText( B2UCONST( "Number" ) );
		
		const Reference< XPropertySet >	xDefaultPagePropertySet( mxDefaultPage, UNO_QUERY );
		Reference< XPropertySetInfo > 	xDefaultPagePropSetInfo( xDefaultPagePropertySet->getPropertySetInfo() );
		
		if( xDefaultPagePropSetInfo.is() )
		{
			const SvxFieldData* pField = pInfo->GetField().GetField();
			Any     			aProperty;
		
			if( pField->ISA( SvxHeaderField ) &&
				xDefaultPagePropSetInfo->hasPropertyByName( aHeaderText ) )
			{
				xDefaultPagePropertySet->getPropertyValue( aHeaderText ) >>= aRepresentation;
                bFieldProcessed = sal_True;
			}
			else if( pField->ISA( SvxFooterField ) &&
					 xDefaultPagePropSetInfo->hasPropertyByName( aFooterText ) )
			{
				xDefaultPagePropertySet->getPropertyValue( aFooterText ) >>= aRepresentation;
                bFieldProcessed = sal_True;
			}
			else if( pField->ISA( SvxDateTimeField ) &&
					 xDefaultPagePropSetInfo->hasPropertyByName( aDateTimeText ) )
			{
				xDefaultPagePropertySet->getPropertyValue( aDateTimeText ) >>= aRepresentation;
                bFieldProcessed = sal_True;
			}
			else if( pField->ISA( SvxPageField ) &&
					 xDefaultPagePropSetInfo->hasPropertyByName( aPageNumberText ) )
			{
	            String     aPageNumValue;
				sal_Int16  nPageNumber = 0;
                
                xDefaultPagePropertySet->getPropertyValue( aPageNumberText ) >>= nPageNumber;
                
                if( mpSdrModel )
                {  
                    sal_Bool bUpper = sal_False;
                
                	switch( mpSdrModel->GetPageNumType() )
                    {
                        case SVX_CHARS_UPPER_LETTER:
                            aPageNumValue += (sal_Unicode)(char)( ( nPageNumber - 1 ) % 26 + 'A' );
                            break;
                        case SVX_CHARS_LOWER_LETTER:
                            aPageNumValue += (sal_Unicode)(char)( ( nPageNumber- 1 ) % 26 + 'a' );
                            break;
                        case SVX_ROMAN_UPPER:
                            bUpper = sal_True;
                        case SVX_ROMAN_LOWER:
                            aPageNumValue += SvxNumberFormat::CreateRomanString( nPageNumber, bUpper );
                            break;
                        case SVX_NUMBER_NONE:
                            aPageNumValue.Erase();
                            aPageNumValue += sal_Unicode(' ');
                            break;
						default : break;
                    }
               	}
                
                if( !aPageNumValue.Len() )
                    aPageNumValue += String::CreateFromInt32( nPageNumber );
                
                aRepresentation = aPageNumValue;
                bFieldProcessed = sal_True;
			}
		}
	
		pInfo->SetRepresentation( aRepresentation );
	}
	
	return( bFieldProcessed ? 0 : maOldFieldHdl.Call( pInfo ) );
}

// -----------------------------------------------------------------------------

void SVGExport::writeMtf( const GDIMetaFile& rMtf )
{
	const Size									aSize( OutputDevice::LogicToLogic( rMtf.GetPrefSize(), rMtf.GetPrefMapMode(), MAP_MM ) );
	rtl::OUString							aAttr;
	REF( NMSP_SAX::XExtendedDocumentHandler )	xExtDocHandler( GetDocHandler(), NMSP_UNO::UNO_QUERY );

	if( xExtDocHandler.is() )
		xExtDocHandler->unknown( SVG_DTD_STRING );

	aAttr = rtl::OUString::valueOf( aSize.Width() ); 
	aAttr += B2UCONST( "mm" );
	AddAttribute( XML_NAMESPACE_NONE, "width", aAttr );

	aAttr = rtl::OUString::valueOf( aSize.Height() ); 
	aAttr += B2UCONST( "mm" );
	AddAttribute( XML_NAMESPACE_NONE, "height", aAttr );

	aAttr = B2UCONST( "0 0 " );
	aAttr += rtl::OUString::valueOf( aSize.Width() * 100L );
	aAttr += B2UCONST( " " );
	aAttr += rtl::OUString::valueOf( aSize.Height() * 100L );
	AddAttribute( XML_NAMESPACE_NONE, "viewBox", aAttr );
	SetViewBox( Rectangle( Point(), Size( aSize.Width() * 100, aSize.Height() * 100 ) ) );

	// standard line width is based on 1 pixel on a 90 DPI device (0.28222mmm)
	AddAttribute( XML_NAMESPACE_NONE, "stroke-width", OUString::valueOf( 28.222 ) );
	AddAttribute( XML_NAMESPACE_NONE, "stroke-linejoin", B2UCONST( "round" ) );

	AddAttribute( XML_NAMESPACE_NONE, "xml:space", B2UCONST( "preserve" ) );
	{
		SvXMLElementExport	aSVG( *this, XML_NAMESPACE_NONE, "svg", sal_True, sal_True );

		std::vector< ObjectRepresentation > aObjects;

		aObjects.push_back( ObjectRepresentation( Reference< XInterface >(), rMtf ) );
		SVGFontExport aSVGFontExport( *this, aObjects );

		Point aPoint100thmm( OutputDevice::LogicToLogic( rMtf.GetPrefMapMode().GetOrigin(), rMtf.GetPrefMapMode(), MAP_100TH_MM ) );
		Size  aSize100thmm( OutputDevice::LogicToLogic( rMtf.GetPrefSize(), rMtf.GetPrefMapMode(), MAP_100TH_MM ) );

		SVGActionWriter		aWriter( *this, aSVGFontExport );
		aWriter.WriteMetaFile( aPoint100thmm, aSize100thmm, rMtf,
			SVGWRITER_WRITE_FILL | SVGWRITER_WRITE_TEXT, NULL );
	}
}
