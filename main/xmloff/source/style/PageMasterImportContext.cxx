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

#include "PageMasterImportContext.hxx"
#include "xmloff/xmlnmspe.hxx"
#include <xmloff/xmltoken.hxx>
#include "PageMasterPropHdl.hxx"
#include "PagePropertySetContext.hxx"
#include "PageHeaderFooterContext.hxx"
#include "PageMasterPropMapper.hxx"
#include "PageMasterImportPropMapper.hxx"
#include <xmloff/PageMasterStyleMap.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

//UUUU
#include <xmlsdtypes.hxx>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/drawing/FillStyle.hpp>
#include <xmloff/xmlerror.hxx>

using namespace ::com::sun::star;
using namespace ::xmloff::token;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;

//UUUU
using namespace ::com::sun::star::beans;

void PageStyleContext::SetAttribute( sal_uInt16 nPrefixKey,
                                        const rtl::OUString& rLocalName,
                                        const rtl::OUString& rValue )
{
    // TODO: use a map here
    if( XML_NAMESPACE_STYLE == nPrefixKey && IsXMLToken( rLocalName, XML_PAGE_USAGE ) )
    {
        sPageUsage = rValue;
    }
    else
    {
        XMLPropStyleContext::SetAttribute( nPrefixKey, rLocalName, rValue );
    }
}

TYPEINIT1( PageStyleContext, XMLPropStyleContext );

PageStyleContext::PageStyleContext( SvXMLImport& rImport,
        sal_uInt16 nPrfx, const rtl::OUString& rLName,
        const uno::Reference< xml::sax::XAttributeList > & xAttrList,
        SvXMLStylesContext& rStyles,
        sal_Bool bDefaultStyle) :
    XMLPropStyleContext( rImport, nPrfx, rLName, xAttrList, rStyles, XML_STYLE_FAMILY_PAGE_MASTER, bDefaultStyle),
    sPageUsage(),
    m_bIsFillStyleAlreadyConverted(false) //UUUU
{
}

PageStyleContext::~PageStyleContext()
{
}

SvXMLImportContext *PageStyleContext::CreateChildContext(
        sal_uInt16 nPrefix,
        const rtl::OUString& rLocalName,
        const uno::Reference< xml::sax::XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = NULL;

    if( XML_NAMESPACE_STYLE == nPrefix &&
        ((IsXMLToken(rLocalName, XML_HEADER_STYLE )) ||
         (IsXMLToken(rLocalName, XML_FOOTER_STYLE )) ) )
    {
        sal_Bool bHeader = IsXMLToken(rLocalName, XML_HEADER_STYLE);
        UniReference < SvXMLImportPropertyMapper > xImpPrMap =
            GetStyles()->GetImportPropertyMapper( GetFamily() );
        if( xImpPrMap.is() )
        {
            const UniReference< XMLPropertySetMapper >& rMapper = xImpPrMap->getPropertySetMapper();
            sal_Int32 nFlag;
            if (bHeader)
                nFlag = CTF_PM_HEADERFLAG;
            else
                nFlag = CTF_PM_FOOTERFLAG;
            sal_Int32 nStartIndex (-1);
            sal_Int32 nEndIndex (-1);
            sal_Bool bFirst(sal_False);
            sal_Bool bEnd(sal_False);
            sal_Int32 nIndex = 0;
            while ( nIndex < rMapper->GetEntryCount() && !bEnd)
            {
                if ((rMapper->GetEntryContextId( nIndex ) & CTF_PM_FLAGMASK) == nFlag)
                {
                    if (!bFirst)
                    {
                        bFirst = sal_True;
                        nStartIndex = nIndex;
                    }
                }
                else if (bFirst)
                {
                    bEnd = sal_True;
                    nEndIndex = nIndex;
                }
                nIndex++;
            }
            if (!bEnd)
                nEndIndex = nIndex;
            pContext = new PageHeaderFooterContext(GetImport(), nPrefix, rLocalName,
                            xAttrList, GetProperties(), xImpPrMap, nStartIndex, nEndIndex, bHeader);
        }
    }
    if( XML_NAMESPACE_STYLE == nPrefix &&
        IsXMLToken(rLocalName, XML_PAGE_LAYOUT_PROPERTIES) )
    {
        UniReference < SvXMLImportPropertyMapper > xImpPrMap =
            GetStyles()->GetImportPropertyMapper( GetFamily() );
        if( xImpPrMap.is() )
        {
            const UniReference< XMLPropertySetMapper >& rMapper = xImpPrMap->getPropertySetMapper();
            sal_Int32 nEndIndex (-1);
            sal_Bool bEnd(sal_False);
            sal_Int32 nIndex = 0;
            sal_Int16 nContextID;
            while ( nIndex < rMapper->GetEntryCount() && !bEnd)
            {
                nContextID = rMapper->GetEntryContextId( nIndex );
                if (nContextID && ((nContextID & CTF_PM_FLAGMASK) != XML_PM_CTF_START))
                {
                    nEndIndex = nIndex;
                    bEnd = sal_True;
                }
                nIndex++;
            }
            if (!bEnd)
                nEndIndex = nIndex;
            PageContextType aType = Page;
            pContext = new PagePropertySetContext( GetImport(), nPrefix,
                                                    rLocalName, xAttrList,
													XML_TYPE_PROP_PAGE_LAYOUT,
                                                    GetProperties(),
                                                    xImpPrMap, 0, nEndIndex, aType);
        }
    }

    if (!pContext)
        pContext = XMLPropStyleContext::CreateChildContext( nPrefix, rLocalName,
                                                           xAttrList );
    return pContext;
}

void PageStyleContext::FillPropertySet(const uno::Reference<beans::XPropertySet > & rPropSet)
{
    //UUUU need to filter out old fill definitions when the new ones are used. The new
    // ones are used when a FillStyle is defined
    if(!m_bIsFillStyleAlreadyConverted && GetProperties().size())
    {
        static ::rtl::OUString s_FillStyle(RTL_CONSTASCII_USTRINGPARAM("FillStyle"));
        static ::rtl::OUString s_HeaderFillStyle(RTL_CONSTASCII_USTRINGPARAM("HeaderFillStyle"));
        static ::rtl::OUString s_FooterFillStyle(RTL_CONSTASCII_USTRINGPARAM("FooterFillStyle"));

        if(doNewDrawingLayerFillStyleDefinitionsExist(s_FillStyle))
        {
            deactivateOldFillStyleDefinitions(getStandardSet());
        }

        if(doNewDrawingLayerFillStyleDefinitionsExist(s_HeaderFillStyle))
        {
            deactivateOldFillStyleDefinitions(getHeaderSet());
        }

        if(doNewDrawingLayerFillStyleDefinitionsExist(s_FooterFillStyle))
        {
            deactivateOldFillStyleDefinitions(getFooterSet());
        }

        m_bIsFillStyleAlreadyConverted = true;
    }

    //UUUU do not use XMLPropStyleContext::FillPropertySet, we need to handle this ourselves since
    // we have properties which use the MID_FLAG_NO_PROPERTY_IMPORT flag since they need some special
    // handling
    UniReference < SvXMLImportPropertyMapper > xImpPrMap = GetStyles()->GetImportPropertyMapper(GetFamily());

    if(xImpPrMap.is())
    {
        // properties that need special handling because they need the used name to be translated first
        struct _ContextID_Index_Pair aContextIDs[] =
        {
            { CTF_PM_FILLGRADIENTNAME, -1 },
            { CTF_PM_FILLTRANSNAME, -1 },
            { CTF_PM_FILLHATCHNAME, -1 },
            { CTF_PM_FILLBITMAPNAME, -1 },

            // also need to special handling for header entries
            { CTF_PM_HEADERFILLGRADIENTNAME, -1 },
            { CTF_PM_HEADERFILLTRANSNAME, -1 },
            { CTF_PM_HEADERFILLHATCHNAME, -1 },
            { CTF_PM_HEADERFILLBITMAPNAME, -1 },

            // also need to special handling for footer entries
            { CTF_PM_FOOTERFILLGRADIENTNAME, -1 },
            { CTF_PM_FOOTERFILLTRANSNAME, -1 },
            { CTF_PM_FOOTERFILLHATCHNAME, -1 },
            { CTF_PM_FOOTERFILLBITMAPNAME, -1 },

            {-1, -1}
        };

        // the style families associated with the same index modulo 4
        static sal_uInt16 aFamilies[] =
        {
            XML_STYLE_FAMILY_SD_GRADIENT_ID,
            XML_STYLE_FAMILY_SD_GRADIENT_ID,
            XML_STYLE_FAMILY_SD_HATCH_ID,
            XML_STYLE_FAMILY_SD_FILL_IMAGE_ID
        };

        //UUUU Fill PropertySet, but let it handle special properties not itself
        xImpPrMap->FillPropertySet(GetProperties(), rPropSet, aContextIDs);

        // get property set mapper
        const UniReference< XMLPropertySetMapper >& rMapper = xImpPrMap->getPropertySetMapper();
        Reference< XPropertySetInfo > xInfo;

        //UUUU handle special attributes which have MID_FLAG_NO_PROPERTY_IMPORT set
        for(sal_uInt16 i = 0; aContextIDs[i].nContextID != -1; i++)
        {
            sal_Int32 nIndex = aContextIDs[i].nIndex;

            if(nIndex != -1)
            {
                switch(aContextIDs[i].nContextID)
                {
                    case CTF_PM_FILLGRADIENTNAME:
                    case CTF_PM_FILLTRANSNAME:
                    case CTF_PM_FILLHATCHNAME:
                    case CTF_PM_FILLBITMAPNAME:

                    case CTF_PM_HEADERFILLGRADIENTNAME:
                    case CTF_PM_HEADERFILLTRANSNAME:
                    case CTF_PM_HEADERFILLHATCHNAME:
                    case CTF_PM_HEADERFILLBITMAPNAME:

                    case CTF_PM_FOOTERFILLGRADIENTNAME:
                    case CTF_PM_FOOTERFILLTRANSNAME:
                    case CTF_PM_FOOTERFILLHATCHNAME:
                    case CTF_PM_FOOTERFILLBITMAPNAME:
                    {
                        struct XMLPropertyState& rState = GetProperties()[nIndex];
                        rtl::OUString sStyleName;
                        rState.maValue >>= sStyleName;

                        //UUUU translate the used name from ODF intern to the name used in the Model
                        sStyleName = GetImport().GetStyleDisplayName(aFamilies[i%4], sStyleName);

                        try
                        {
                            // set property
                            const rtl::OUString& rPropertyName = rMapper->GetEntryAPIName(rState.mnIndex);

                            if(!xInfo.is())
                            {
                                xInfo = rPropSet->getPropertySetInfo();
                            }

                            if(xInfo->hasPropertyByName(rPropertyName))
                            {
                                rPropSet->setPropertyValue(rPropertyName,Any(sStyleName));
                            }
                        }
                        catch(::com::sun::star::lang::IllegalArgumentException& e)
                        {
                            Sequence< rtl::OUString > aSeq(1);
                            aSeq[0] = sStyleName;
                            GetImport().SetError(
                                XMLERROR_STYLE_PROP_VALUE | XMLERROR_FLAG_WARNING,
                                aSeq,e.Message,NULL);
                        }
                        break;
                    }
                }
            }
        }
    }
    else
    {
        OSL_ENSURE(xImpPrMap.is(), "Got no SvXMLImportPropertyMapper (!)");
    }

    //UUUU old code, replaced by above stuff
    // XMLPropStyleContext::FillPropertySet(rPropSet);

    if (sPageUsage.getLength())
    {
        uno::Any aPageUsage;
        XMLPMPropHdl_PageStyleLayout aPageUsageHdl;
        if (aPageUsageHdl.importXML(sPageUsage, aPageUsage, GetImport().GetMM100UnitConverter()))
            rPropSet->setPropertyValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PageStyleLayout")), aPageUsage);
    }
}

void PageStyleContext::FillPropertySetWithoutOldPageFill( const uno::Reference< beans::XPropertySet > & rPropSet )
{
    static ::rtl::OUString s_FillStyle(RTL_CONSTASCII_USTRINGPARAM("FillStyle"));

    // a fill of the page layout itself already switches its old fill definitions off
    if( doNewDrawingLayerFillStyleDefinitionsExist( s_FillStyle ) )
    {
        FillPropertySet( rPropSet );
        return;
    }

    // switched off for this call only: the page layout may be shared with master pages without a drawing page fill
    ::std::vector< XMLPropertyState >& rProperties = GetProperties();
    ::std::vector< ::std::pair< size_t, sal_Int32 > > aSwitchedOff;
    UniReference < SvXMLImportPropertyMapper > xImpPrMap = GetStyles()->GetImportPropertyMapper( GetFamily() );
    if( xImpPrMap.is() )
    {
        const OldFillStyleDefinitionSet& rOld = getStandardSet();
        for( size_t i = 0; i < rProperties.size(); ++i )
        {
            if( rProperties[i].mnIndex != -1 &&
                rOld.find( xImpPrMap->getPropertySetMapper()->GetEntryAPIName( rProperties[i].mnIndex ) ) != rOld.end() )
            {
                aSwitchedOff.push_back( ::std::make_pair( i, rProperties[i].mnIndex ) );
                rProperties[i].mnIndex = -1;
            }
        }
    }
    FillPropertySet( rPropSet );
    for( size_t i = 0; i < aSwitchedOff.size(); ++i )
        rProperties[aSwitchedOff[i].first].mnIndex = aSwitchedOff[i].second;
}

// text grid enhancement for better CJK support
//set default page layout style
void PageStyleContext::SetDefaults( )
{
	Reference < XMultiServiceFactory > xFactory ( GetImport().GetModel(), UNO_QUERY);
	if (xFactory.is())
	{
		Reference < XInterface > xInt = xFactory->createInstance (
	    rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.text.Defaults" ) ) );
	    Reference < beans::XPropertySet > xProperties ( xInt, UNO_QUERY );
	    if ( xProperties.is() )
	    	FillPropertySet ( xProperties );
	}
}

TYPEINIT1( DrawingPageStyleContext, XMLPropStyleContext );

DrawingPageStyleContext::DrawingPageStyleContext( SvXMLImport& rImport,
        sal_uInt16 nPrfx, const rtl::OUString& rLName,
        const uno::Reference< xml::sax::XAttributeList > & xAttrList,
        SvXMLStylesContext& rStyles ) :
    XMLPropStyleContext( rImport, nPrfx, rLName, xAttrList, rStyles, XML_STYLE_FAMILY_SD_DRAWINGPAGE_ID ),
    mbChecked( false ),
    mbUsable( false )
{
}

DrawingPageStyleContext::~DrawingPageStyleContext()
{
}

static bool lcl_FillNameExists( SvXMLImport& rImport, const Reference< container::XNameContainer >& rTable,
                                sal_uInt16 nFamily, const rtl::OUString& rName )
{
    return rName.getLength() && rTable.is() && rTable->hasByName( rImport.GetStyleDisplayName( nFamily, rName ) );
}

// An incomplete fill is ignored as a whole, so the fill of the page layout stays.
void DrawingPageStyleContext::CheckFill()
{
    UniReference < SvXMLImportPropertyMapper > xImpPrMap = GetStyles()->GetImportPropertyMapper( GetFamily() );
    if( !mbChecked && xImpPrMap.is() )
    {
        mbChecked = true;
        const UniReference< XMLPropertySetMapper >& rMapper = xImpPrMap->getPropertySetMapper();
        ::std::vector< XMLPropertyState >& rProperties = GetProperties();
        drawing::FillStyle eStyle = drawing::FillStyle_NONE;
        bool bColor = false;
        rtl::OUString aGradient, aHatch, aBitmap, aTrans;
        XMLPropertyState* pTrans = NULL;

        for( ::std::vector< XMLPropertyState >::iterator a = rProperties.begin(); a != rProperties.end(); ++a )
        {
            if( a->mnIndex == -1 )
                continue;
            const rtl::OUString& rName = rMapper->GetEntryAPIName( a->mnIndex );
            if( rName.equalsAscii( "FillStyle" ) )
            {
                sal_Int32 nValue = 0;
                if( !( a->maValue >>= eStyle ) && ( a->maValue >>= nValue ) )
                    eStyle = static_cast< drawing::FillStyle >( nValue );
            }
            else if( rName.equalsAscii( "FillColor" ) )
                bColor = true;
            else if( rName.equalsAscii( "FillGradientName" ) )
                a->maValue >>= aGradient;
            else if( rName.equalsAscii( "FillHatchName" ) )
                a->maValue >>= aHatch;
            else if( rName.equalsAscii( "FillBitmapName" ) )
                a->maValue >>= aBitmap;
            else if( rName.equalsAscii( "FillTransparenceGradientName" ) )
            {
                a->maValue >>= aTrans;
                pTrans = &(*a);
            }
        }

        SvXMLImport& rImport = GetImport();
        switch( eStyle )
        {
            case drawing::FillStyle_SOLID:
                mbUsable = bColor;
                break;
            case drawing::FillStyle_GRADIENT:
                mbUsable = lcl_FillNameExists( rImport, rImport.GetGradientHelper(), XML_STYLE_FAMILY_SD_GRADIENT_ID, aGradient );
                break;
            case drawing::FillStyle_HATCH:
                mbUsable = lcl_FillNameExists( rImport, rImport.GetHatchHelper(), XML_STYLE_FAMILY_SD_HATCH_ID, aHatch );
                break;
            case drawing::FillStyle_BITMAP:
                mbUsable = lcl_FillNameExists( rImport, rImport.GetBitmapHelper(), XML_STYLE_FAMILY_SD_FILL_IMAGE_ID, aBitmap );
                break;
            default:
                break;
        }

        if( mbUsable && pTrans && !lcl_FillNameExists( rImport, rImport.GetTransGradientHelper(), XML_STYLE_FAMILY_SD_GRADIENT_ID, aTrans ) )
            pTrans->mnIndex = -1;
    }
}

void DrawingPageStyleContext::FillPropertySet( const uno::Reference< beans::XPropertySet > & rPropSet )
{
    UniReference < SvXMLImportPropertyMapper > xImpPrMap = GetStyles()->GetImportPropertyMapper( GetFamily() );
    if( !xImpPrMap.is() || !HasFill() )
        return;
    const UniReference< XMLPropertySetMapper >& rMapper = xImpPrMap->getPropertySetMapper();
    ::std::vector< XMLPropertyState >& rProperties = GetProperties();

    // as in PageStyleContext::FillPropertySet: names set in the bulk call left gradient and hatch at their defaults
    struct _ContextID_Index_Pair aContextIDs[] =
    {
        { CTF_PM_FILLGRADIENTNAME, -1 },
        { CTF_PM_FILLTRANSNAME, -1 },
        { CTF_PM_FILLHATCHNAME, -1 },
        { CTF_PM_FILLBITMAPNAME, -1 },
        { -1, -1 }
    };
    static const sal_uInt16 aFamilies[] =
    {
        XML_STYLE_FAMILY_SD_GRADIENT_ID,
        XML_STYLE_FAMILY_SD_GRADIENT_ID,
        XML_STYLE_FAMILY_SD_HATCH_ID,
        XML_STYLE_FAMILY_SD_FILL_IMAGE_ID
    };

    xImpPrMap->FillPropertySet( rProperties, rPropSet, aContextIDs );

    Reference< XPropertySetInfo > xInfo( rPropSet->getPropertySetInfo() );
    for( sal_uInt16 i = 0; aContextIDs[i].nContextID != -1; i++ )
    {
        if( aContextIDs[i].nIndex == -1 )
            continue;
        const XMLPropertyState& rState = rProperties[aContextIDs[i].nIndex];
        const rtl::OUString& rPropertyName = rMapper->GetEntryAPIName( rState.mnIndex );
        rtl::OUString sStyleName;
        rState.maValue >>= sStyleName;
        sStyleName = GetImport().GetStyleDisplayName( aFamilies[i], sStyleName );
        try
        {
            if( xInfo->hasPropertyByName( rPropertyName ) )
                rPropSet->setPropertyValue( rPropertyName, Any( sStyleName ) );
        }
        catch( lang::IllegalArgumentException& e )
        {
            Sequence< rtl::OUString > aSeq( 1 );
            aSeq[0] = sStyleName;
            GetImport().SetError( XMLERROR_STYLE_PROP_VALUE | XMLERROR_FLAG_WARNING, aSeq, e.Message, NULL );
        }
    }
}

//eof
