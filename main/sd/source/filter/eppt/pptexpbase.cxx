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

#include "pptexpbase.hxx"
#include "epptdef.hxx"
#include <vcl/outdev.hxx>
#include <vcl/virdev.hxx>
#include <vcl/gradient.hxx>

#ifndef _CPPUHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _CPPUHELPER_PROPTYPEHLP_HXX_
#include <cppuhelper/proptypehlp.hxx>
#endif


#include <com/sun/star/view/PaperOrientation.hpp>
#include <com/sun/star/view/PaperFormat.hpp>
#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>

#include <com/sun/star/geometry/RealPoint2D.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/animations/TransitionType.hpp>
#include <com/sun/star/animations/TransitionSubType.hpp>
#include <com/sun/star/awt/FontDescriptor.hpp>
#include <com/sun/star/awt/FontFamily.hpp>
#include <com/sun/star/awt/FontPitch.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/presentation/XPresentationPage.hpp>
#include <com/sun/star/text/XSimpleText.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>

using namespace com::sun::star;

using namespace ::com::sun::star::animations;
using namespace ::com::sun::star::awt::FontFamily;
using namespace ::com::sun::star::awt::FontPitch;
using namespace ::com::sun::star::presentation;

using ::com::sun::star::awt::FontDescriptor;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::container::XNameAccess;
using ::com::sun::star::container::XNamed;
using ::com::sun::star::drawing::XDrawPagesSupplier;
using ::com::sun::star::drawing::XMasterPagesSupplier;
using ::com::sun::star::drawing::XShapes;
using ::com::sun::star::drawing::XMasterPageTarget;
using ::com::sun::star::drawing::XDrawPage;
using ::com::sun::star::frame::XModel;
using ::com::sun::star::style::XStyleFamiliesSupplier;
using ::com::sun::star::style::XStyle;
using ::com::sun::star::task::XStatusIndicator;
using ::com::sun::star::text::XSimpleText;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Exception;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::UNO_QUERY;

#define PPT_EXPORT_BASE_INIT_VALUES \
    maFraction              ( 1, 576 ), \
    maMapModeSrc            ( MAP_100TH_MM ), \
    maMapModeDest           ( MAP_INCH, Point(), maFraction, maFraction ), \
    meLatestPageType        ( NORMAL )


PPTEXPBase::PPTEXPBase() :
PPT_EXPORT_BASE_INIT_VALUES
{

}

PPTEXPBase::PPTEXPBase( const Reference< XModel > & rXModel,
                       const Reference< XStatusIndicator > & rXStatInd ) :
mXModel                 ( rXModel ),
mXStatusIndicator       ( rXStatInd ),
mbStatusIndicator       ( false ),
PPT_EXPORT_BASE_INIT_VALUES
{
}

// ---------------------------------------------------------------------------------------------

PPTEXPBase::~PPTEXPBase()
{
    if ( mbStatusIndicator )
        mXStatusIndicator->end();
}

// ---------------------------------------------------------------------------------------------

void PPTEXPBase::exportPPT()
{
    if ( !InitUnoModel() )
        return;

    if ( !ImplGetPageByIndex( 0, NOTICE ) )
        return;
    sal_Int32 nWidth = 21000;
    if ( ImplGetPropertyValue( mXPagePropSet, String( RTL_CONSTASCII_USTRINGPARAM(  "Width" ) ) ) )
        mAny >>= nWidth;
    sal_Int32 nHeight = 29700;
    if ( ImplGetPropertyValue( mXPagePropSet, String( RTL_CONSTASCII_USTRINGPARAM( "Height" ) ) ) )
        mAny >>= nHeight;

    maNotesPageSize = ImplMapSize( ::com::sun::star::awt::Size( nWidth, nHeight ) );

    if ( !ImplGetPageByIndex( 0, MASTER ) )
        return;
    nWidth = 28000;
    if ( ImplGetPropertyValue( mXPagePropSet, String( RTL_CONSTASCII_USTRINGPARAM( "Width" ) ) ) )
        mAny >>= nWidth;
    nHeight = 21000;
    if ( ImplGetPropertyValue( mXPagePropSet, String( RTL_CONSTASCII_USTRINGPARAM( "Height" ) ) ) )
        mAny >>= nHeight;
    maDestPageSize = ImplMapSize( ::com::sun::star::awt::Size( nWidth, nHeight ) );

    exportPPTPre();

    if ( !ImplGetStyleSheets() )
        return;

    if ( !ImplCreateDocument() )
        return;

    sal_uInt32 i;
    for ( i = 0; i < mnPages; i++ )
    {
        if ( ImplGetPageByIndex( i, NORMAL ) ) 
        {
            sal_uInt32 nMasterNum = ImplGetMasterIndex( NORMAL );
            ImplWriteLayout( GetLayoutOffset( mXPagePropSet ), nMasterNum );
        }
    }

    for ( i = 0; i < mnMasterPages; i++ )
    {
        if ( !ImplCreateSlideMaster( i ) )
            return;
    }

    if ( !CreateMainNotes() )
        return;
    maTextRuleList.First();                        

    for ( i = 0; i < mnPages; i++ )
    {

        if ( !ImplCreateSlide( i ) )
            return;
    }

    for ( i = 0; i < mnPages; i++ )
    {
        if ( !ImplCreateNotes( i ) )
            return;
    }

    exportPPTPost();
}

sal_Bool PPTEXPBase::InitUnoModel()
{
    while( sal_True )
    {
        mXDrawPagesSupplier = Reference< XDrawPagesSupplier >( mXModel, UNO_QUERY );
        if ( !mXDrawPagesSupplier.is() )
            break;

        mXMasterPagesSupplier = Reference< XMasterPagesSupplier >( mXModel, UNO_QUERY );
        if ( !mXMasterPagesSupplier.is() )
            break;
        mXDrawPages = mXMasterPagesSupplier->getMasterPages();
        if ( !mXDrawPages.is() )
            break;
        mnMasterPages = mXDrawPages->getCount();
        mXDrawPages = mXDrawPagesSupplier->getDrawPages();
        if( !mXDrawPages.is() )
            break;
        mnPages =  mXDrawPages->getCount();
        if ( !ImplGetPageByIndex( 0, NORMAL ) )
            break;

        return sal_True;
    }
    return sal_False;

}
sal_Bool PPTEXPBase::ImplCreateSlide( sal_uInt32 nPageNum )
{
    Any aAny;

    if ( !ImplGetPageByIndex( nPageNum, NORMAL ) )
        return sal_False;

    sal_uInt32 nMasterNum = ImplGetMasterIndex( NORMAL );
    ImplSetCurrentStyleSheet( nMasterNum );

    Reference< XPropertySet > aXBackgroundPropSet;
    sal_Bool bHasBackground = GetPropertyValue( aAny, mXPagePropSet, String( RTL_CONSTASCII_USTRINGPARAM( "Background" ) ) );
    if ( bHasBackground )
        bHasBackground = ( aAny >>= aXBackgroundPropSet );

    sal_uInt16 nMode = 7;   // Bit 1: Follow master objects, Bit 2: Follow master scheme, Bit 3: Follow master background
    if ( bHasBackground )
        nMode &=~4;

    if ( GetPropertyValue( aAny, mXPagePropSet, String( RTL_CONSTASCII_USTRINGPARAM( "IsBackgroundObjectsVisible" ) ) ) )
    {
        sal_Bool bBackgroundObjectsVisible = sal_False;
        if ( aAny >>= bBackgroundObjectsVisible )
        {
            if ( !bBackgroundObjectsVisible )
                nMode &= ~1;
        }
    }

    ImplWriteSlide( nPageNum, nMasterNum, nMode, bHasBackground, aXBackgroundPropSet );

    return sal_True;
}

// ---------------------------------------------------------------------------------------------

sal_Bool PPTEXPBase::ImplCreateNotes( sal_uInt32 nPageNum )
{
    if ( !ImplGetPageByIndex( nPageNum, NOTICE ) )
        return sal_False;
    ImplSetCurrentStyleSheet( ImplGetMasterIndex( NORMAL ) );

    ImplWriteNotes( nPageNum );

    return sal_True;
}


sal_Bool PPTEXPBase::CreateMainNotes()
{
    if ( !ImplGetPageByIndex( 0, NOTICE ) )
        return sal_False;
    ImplSetCurrentStyleSheet( 0 );

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XMasterPageTarget >
        aXMasterPageTarget( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );

    if ( !aXMasterPageTarget.is() )
        return sal_False;

    mXDrawPage = aXMasterPageTarget->getMasterPage();
    if ( !mXDrawPage.is() )
        return sal_False;

    mXPropSet = ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet >
        ( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );
    if ( !mXPropSet.is() )
        return sal_False;

    mXShapes = ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShapes >
        ( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );
    if ( !mXShapes.is() )
        return sal_False;

    return ImplCreateMainNotes();
}


sal_Bool PPTEXPBase::ImplCreateSlideMaster( sal_uInt32 nPageNum )
{
    if ( !ImplGetPageByIndex( nPageNum, MASTER ) )
        return sal_False;
    ImplSetCurrentStyleSheet( nPageNum );

    if ( !ImplGetPropertyValue( mXPagePropSet, String( RTL_CONSTASCII_USTRINGPARAM( "Background" ) ) ) )              
        return sal_False;
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > aXBackgroundPropSet;
    if ( !( mAny >>= aXBackgroundPropSet ) )
        return sal_False;

    ImplWriteSlideMaster( nPageNum, aXBackgroundPropSet );

    return sal_True;
}


sal_Bool PPTEXPBase::ImplGetPageByIndex( sal_uInt32 nIndex, PageType ePageType )
{
    while( true )
    {
        if ( ePageType != meLatestPageType )
        {
            switch( ePageType )
            {
            case NORMAL :
            case NOTICE :
                {
                    mXDrawPages = mXDrawPagesSupplier->getDrawPages();
                    if( !mXDrawPages.is() )
                        return sal_False;
                }
                break;

            case MASTER :
                {
                    mXDrawPages = mXMasterPagesSupplier->getMasterPages();
                    if( !mXDrawPages.is() )
                        return sal_False;
                }
                break;
            default:
                break;
            }
            meLatestPageType = ePageType;
        }
        ::com::sun::star::uno::Any aAny( mXDrawPages->getByIndex( nIndex ) );
        aAny >>= mXDrawPage;
        if ( !mXDrawPage.is() )
            break;
        if ( ePageType == NOTICE )
        {
            ::com::sun::star::uno::Reference< ::com::sun::star::presentation::XPresentationPage >
                aXPresentationPage( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );
            if ( !aXPresentationPage.is() )
                break;
            mXDrawPage = aXPresentationPage->getNotesPage();
            if ( !mXDrawPage.is() )
                break;
        }
        mXPagePropSet = ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
            ( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );
        if ( !mXPagePropSet.is() )
            break;

        mXShapes = ::com::sun::star::uno::Reference<
            ::com::sun::star::drawing::XShapes >
            ( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );
        if ( !mXShapes.is() )
            break;

        /* try to get the "real" background PropertySet. If the normal page is not supporting this property, it is
        taken the property from the master */
        sal_Bool bHasBackground = GetPropertyValue( aAny, mXPagePropSet, String( RTL_CONSTASCII_USTRINGPARAM( "Background" ) ), sal_True );
        if ( bHasBackground )
            bHasBackground = ( aAny >>= mXBackgroundPropSet );
        if ( !bHasBackground )
        {
            ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XMasterPageTarget >
                aXMasterPageTarget( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );
            if ( aXMasterPageTarget.is() )
            {
                ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage > aXMasterDrawPage;
                aXMasterDrawPage = aXMasterPageTarget->getMasterPage();
                if ( aXMasterDrawPage.is() )
                {
                    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > aXMasterPagePropSet;
                    aXMasterPagePropSet = ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
                        ( aXMasterDrawPage, ::com::sun::star::uno::UNO_QUERY );
                    if ( aXMasterPagePropSet.is() )
                    {
                        sal_Bool bBackground = GetPropertyValue( aAny, aXMasterPagePropSet,
                            String( RTL_CONSTASCII_USTRINGPARAM( "Background" ) ) );
                        if ( bBackground )
                        {
                            aAny >>= mXBackgroundPropSet;
                        }
                    }
                }
            }
        }
        return sal_True;
    }
    return sal_False;
}

::com::sun::star::awt::Point PPTEXPBase::ImplMapPoint( const ::com::sun::star::awt::Point& rPoint )
{
    Point aRet( OutputDevice::LogicToLogic( Point( rPoint.X, rPoint.Y ), maMapModeSrc, maMapModeDest ) );
    return ::com::sun::star::awt::Point( aRet.X(), aRet.Y() );
}


::com::sun::star::awt::Size PPTEXPBase::ImplMapSize( const ::com::sun::star::awt::Size& rSize )
{
    Size aRetSize( OutputDevice::LogicToLogic( Size( rSize.Width, rSize.Height ), maMapModeSrc, maMapModeDest ) );

    if ( !aRetSize.Width() )
        aRetSize.Width()++;
    if ( !aRetSize.Height() )
        aRetSize.Height()++;
    return ::com::sun::star::awt::Size( aRetSize.Width(), aRetSize.Height() );
}


Rectangle PPTEXPBase::ImplMapRectangle( const ::com::sun::star::awt::Rectangle& rRect )
{
    ::com::sun::star::awt::Point    aPoint( rRect.X, rRect.Y );
    ::com::sun::star::awt::Size     aSize( rRect.Width, rRect.Height );
    ::com::sun::star::awt::Point    aP( ImplMapPoint( aPoint ) );
    ::com::sun::star::awt::Size     aS( ImplMapSize( aSize ) );
    return Rectangle( Point( aP.X, aP.Y ), Size( aS.Width, aS.Height ) );
}


sal_Bool PPTEXPBase::ImplGetStyleSheets()
{
    int             nInstance, nLevel;
    sal_Bool        bRetValue = sal_False;
    sal_uInt32		nPageNum;

    for ( nPageNum = 0; nPageNum < mnMasterPages; nPageNum++ )
    {
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNamed >
            aXNamed;

        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >
            aXNameAccess;

        ::com::sun::star::uno::Reference< ::com::sun::star::style::XStyleFamiliesSupplier >
            aXStyleFamiliesSupplier( mXModel, ::com::sun::star::uno::UNO_QUERY );

        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
            aXPropSet( mXModel, ::com::sun::star::uno::UNO_QUERY );

        sal_uInt16 nDefaultTab = ( aXPropSet.is() && ImplGetPropertyValue( aXPropSet, String( RTL_CONSTASCII_USTRINGPARAM( "TabStop" ) ) ) )
            ? (sal_uInt16)( *(sal_Int32*)mAny.getValue() / 4.40972 )
            : 1250;

        maStyleSheetList.push_back( new PPTExStyleSheet( nDefaultTab, (PPTExBulletProvider&)*this ) );
        ImplSetCurrentStyleSheet( nPageNum );
        if ( ImplGetPageByIndex( nPageNum, MASTER ) )
            aXNamed = ::com::sun::star::uno::Reference< ::com::sun::star::container::XNamed >
            ( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );

        if ( aXStyleFamiliesSupplier.is() )
            aXNameAccess = aXStyleFamiliesSupplier->getStyleFamilies();

        bRetValue = aXNamed.is() && aXNameAccess.is() && aXStyleFamiliesSupplier.is();
        if  ( bRetValue )
        {
            // Wang Xu Ming -- 4/28/2010
            // Sym2_6917, Sym2_7468, half body style must be write for PPT table
            //for ( nInstance = EPP_TEXTTYPE_Title; nInstance <= EPP_TEXTTYPE_CenterTitle; nInstance++ )
            for ( nInstance = EPP_TEXTTYPE_Title; nInstance <= EPP_TEXTTYPE_HalfBody; nInstance++ )
                // End
            {
                String aStyle;
                String aFamily;
                switch ( nInstance )
                {
                case EPP_TEXTTYPE_CenterTitle :
                case EPP_TEXTTYPE_Title :
                    {
                        aStyle = String( RTL_CONSTASCII_USTRINGPARAM( "title" ) );
                        aFamily = aXNamed->getName();
                    }
                    break;
                    // Wang Xu Ming -- 4/28/2010
                    // Sym2_6917
                case EPP_TEXTTYPE_Body :
                case EPP_TEXTTYPE_HalfBody:
                    // End
                    {
                        aStyle = String( RTL_CONSTASCII_USTRINGPARAM( "outline1" ) );      // SD_LT_SEPARATOR
                        aFamily = aXNamed->getName();
                    }
                    break;
                case EPP_TEXTTYPE_Other :
                    {
                        aStyle = String( RTL_CONSTASCII_USTRINGPARAM( "standard" ) );
                        aFamily = String( RTL_CONSTASCII_USTRINGPARAM( "graphics" ) );
                    }
                    break;
                case EPP_TEXTTYPE_CenterBody :
                    {
                        aStyle = String( RTL_CONSTASCII_USTRINGPARAM( "subtitle" ) );
                        aFamily = aXNamed->getName();
                    }
                    break;
                }
                if ( aStyle.Len() && aFamily.Len() )
                {
                    try
                    {
                        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >xNameAccess;
                        if ( aXNameAccess->hasByName( aFamily ) )
                        {
                            ::com::sun::star::uno::Any aAny( aXNameAccess->getByName( aFamily ) );
                            if( aAny.getValue() && ::cppu::extractInterface( xNameAccess, aAny ) )
                            {
                                ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > aXFamily;
                                if ( aAny >>= aXFamily )
                                {
                                    if ( aXFamily->hasByName( aStyle ) )
                                    {
                                        ::com::sun::star::uno::Reference< ::com::sun::star::style::XStyle > xStyle;
                                        aAny = aXFamily->getByName( aStyle );
                                        if( aAny.getValue() && ::cppu::extractInterface( xStyle, aAny ) )
                                        {
                                            ::com::sun::star::uno::Reference< ::com::sun::star::style::XStyle > aXStyle;
                                            aAny >>= aXStyle;
                                            ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
                                                xPropSet( aXStyle, ::com::sun::star::uno::UNO_QUERY );
                                            if( xPropSet.is() )
                                                mpStyleSheet->SetStyleSheet( xPropSet, maFontCollection, nInstance, 0 );
                                            for ( nLevel = 1; nLevel < 5; nLevel++ )
                                            {
                                                // Wang Xu Ming -- 4/28/2010
                                                // Sym2_6917
                                                if ( nInstance == EPP_TEXTTYPE_Body || nInstance == EPP_TEXTTYPE_HalfBody )
                                                    // End
                                                {
                                                    sal_Unicode cTemp = aStyle.GetChar( aStyle.Len() - 1 );
                                                    aStyle.SetChar( aStyle.Len() - 1, ++cTemp );
                                                    if ( aXFamily->hasByName( aStyle ) )
                                                    {
                                                        aXFamily->getByName( aStyle ) >>= xStyle;
                                                        if( xStyle.is() )
                                                        {
                                                            ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
                                                                xPropertySet( xStyle, ::com::sun::star::uno::UNO_QUERY );
                                                            if ( xPropertySet.is() )
                                                                mpStyleSheet->SetStyleSheet( xPropertySet, maFontCollection, nInstance, nLevel );
                                                        }
                                                    }
                                                }
                                                else
                                                    mpStyleSheet->SetStyleSheet( xPropSet, maFontCollection, nInstance, nLevel );
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    catch( ::com::sun::star::uno::Exception& )
                    {
                        //
                    }
                }
            }
            for ( ; nInstance <= EPP_TEXTTYPE_QuarterBody; nInstance++ )
            {

            }
        }
    }
    return bRetValue;
}

sal_Bool PPTEXPBase::ImplSetCurrentStyleSheet( sal_uInt32 nPageNum )
{
    sal_Bool bRet = sal_False;
    if ( nPageNum >= maStyleSheetList.size() )
        nPageNum = 0;
    else
        bRet = sal_True;
    mpStyleSheet = maStyleSheetList[ nPageNum ];
    return bRet;
}

sal_Bool PPTEXPBase::ContainsOtherShapeThanPlaceholders( sal_Bool bForOOMLX )
{
    sal_uInt32 nShapes = mXShapes->getCount();
    sal_Bool bOtherThanPlaceHolders = sal_False;

    if ( nShapes )
        for ( sal_uInt32 nIndex = 0; ( nIndex < nShapes ) && ( bOtherThanPlaceHolders == sal_False ); nIndex++ ) {
            if ( ImplGetShapeByIndex( nIndex ) && mType != "drawing.Page" ) {
                if( bForOOMLX &&
                    ( mType == "presentation.Page" ||
                    mType == "presentation.Notes" ) ) {
                        Reference< XSimpleText > rXText( mXShape, UNO_QUERY );

                        if( rXText.is() && rXText->getString().getLength() != 0 )
                            bOtherThanPlaceHolders = sal_True;
                } else
                    bOtherThanPlaceHolders = sal_True;
            }

        }

        return bOtherThanPlaceHolders;
}

sal_Bool PPTEXPBase::ImplGetShapeByIndex( sal_uInt32 nIndex, sal_Bool bGroup )
{
    while ( true )
    {
        if ( ( bGroup == sal_False ) || ( GetCurrentGroupLevel() == 0 ) )
        {
            ::com::sun::star::uno::Any aAny( mXShapes->getByIndex( nIndex ) );
            aAny >>= mXShape;
        }
        else
        {
            ::com::sun::star::uno::Any aAny( GetCurrentGroupAccess()->getByIndex( GetCurrentGroupIndex() ) );
            aAny >>= mXShape;
        }
        if ( !mXShape.is() )
            break;

        ::com::sun::star::uno::Any aAny( mXShape->queryInterface( ::getCppuType( (const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >*) 0 ) ));
        aAny >>= mXPropSet;

        if ( !mXPropSet.is() )
            break;
        maPosition = ImplMapPoint( mXShape->getPosition() );
        maSize = ImplMapSize( mXShape->getSize() );
        maRect = Rectangle( Point( maPosition.X, maPosition.Y ), Size( maSize.Width, maSize.Height ) );
        mType = ByteString( String( mXShape->getShapeType() ), RTL_TEXTENCODING_UTF8 );
        mType.Erase( 0, 13 );                                   // "com.sun.star." entfernen
        sal_uInt16 nPos = mType.Search( (const char*)"Shape" );
        mType.Erase( nPos, 5 );

        mbPresObj = mbEmptyPresObj = sal_False;
        if ( ImplGetPropertyValue( String( RTL_CONSTASCII_USTRINGPARAM( "IsPresentationObject" ) ) ) )
            mAny >>= mbPresObj;

        if ( mbPresObj && ImplGetPropertyValue( String( RTL_CONSTASCII_USTRINGPARAM( "IsEmptyPresentationObject" ) ) ) )
            mAny >>= mbEmptyPresObj;

        mnAngle = ( PropValue::GetPropertyValue( aAny,
            mXPropSet, String( RTL_CONSTASCII_USTRINGPARAM( "RotateAngle" ) ), sal_True ) )
            ? *((sal_Int32*)aAny.getValue() )
            : 0;

        return sal_True;
    }
    return sal_False;
}


sal_uInt32 PPTEXPBase::ImplGetMasterIndex( PageType ePageType )
{
    sal_uInt32 nRetValue = 0;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XMasterPageTarget >
        aXMasterPageTarget( mXDrawPage, ::com::sun::star::uno::UNO_QUERY );

    if ( aXMasterPageTarget.is() )
    {
        ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >
            aXDrawPage = aXMasterPageTarget->getMasterPage();
        if ( aXDrawPage.is() )
        {
            ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
                aXPropertySet( aXDrawPage, ::com::sun::star::uno::UNO_QUERY );

            if ( aXPropertySet.is() )
            {
                if ( ImplGetPropertyValue( aXPropertySet, String( RTL_CONSTASCII_USTRINGPARAM( "Number" ) ) ) )
                    nRetValue |= *(sal_Int16*)mAny.getValue();
                if ( nRetValue & 0xffff )           // ueberlauf vermeiden
                    nRetValue--;
            }
        }
    }
    if ( ePageType == NOTICE )
        nRetValue += mnMasterPages;
    return nRetValue;

}

sal_Int32 PPTEXPBase::GetLayoutOffset( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& rXPropSet ) const
{
    ::com::sun::star::uno::Any aAny;
    sal_Int32 nLayout = 20;
    if ( GetPropertyValue( aAny, rXPropSet, String( RTL_CONSTASCII_USTRINGPARAM( "Layout" ) ) ), sal_True )
        aAny >>= nLayout;

    return nLayout;
}

