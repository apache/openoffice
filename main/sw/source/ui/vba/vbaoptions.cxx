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


#include "vbaoptions.hxx"
#include <vbahelper/vbahelper.hxx>
#include <ooo/vba/word/WdDefaultFilePath.hpp>
#include <ooo/vba/word/WdLineStyle.hpp>
#include <ooo/vba/word/WdLineWidth.hpp>
#include <ooo/vba/word/WdColorIndex.hpp>
#include <com/sun/star/util/XStringSubstitution.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <osl/file.hxx>

using namespace ::ooo::vba;
using namespace ::com::sun::star;

SwVbaOptions::SwVbaOptions( uno::Reference<uno::XComponentContext >& xContext ) : SwVbaOptions_BASE( uno::Reference< XHelperInterface >(), xContext )
{
    mxFactory.set( comphelper::getProcessServiceFactory(), uno::UNO_QUERY_THROW );
}

SwVbaOptions::~SwVbaOptions()
{
}

uno::Any SAL_CALL
SwVbaOptions::DefaultFilePath( sal_Int32 _path )
{
    switch( _path )
    {
        case word::WdDefaultFilePath::wdDocumentsPath:
        {
            msDefaultFilePath = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Work") );
            break;
        }
        case word::WdDefaultFilePath::wdPicturesPath:
        {
            msDefaultFilePath = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Gallery") );
            break;
        }
        case word::WdDefaultFilePath::wdUserTemplatesPath:
        case word::WdDefaultFilePath::wdWorkgroupTemplatesPath:
        {
            msDefaultFilePath = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Template") );
            break;
        }
        case word::WdDefaultFilePath::wdStartupPath:
        {
            msDefaultFilePath = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Addin") );
            break;
        }
        case word::WdDefaultFilePath::wdUserOptionsPath:
        {
            msDefaultFilePath = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("UserConfig") );
            break;
        }
        case word::WdDefaultFilePath::wdToolsPath:
        case word::WdDefaultFilePath::wdProgramPath:
        {
            msDefaultFilePath = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Module") );
            break;
        }
        case word::WdDefaultFilePath::wdTempFilePath:
        {
            msDefaultFilePath = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Temp") );
            break;
        }
        default:
        {
            DebugHelper::exception( SbERR_NOT_IMPLEMENTED, rtl::OUString() );
            break;
        }
    }
    return uno::makeAny( uno::Reference< XPropValue > ( new ScVbaPropValue( this ) ) );
}

void SwVbaOptions::setValueEvent( const uno::Any& value )
{
    rtl::OUString sNewPath;
    value >>= sNewPath;
    rtl::OUString sNewPathUrl;
    ::osl::File::getFileURLFromSystemPath( sNewPath, sNewPathUrl );
    uno::Reference< beans::XPropertySet > xPathSettings( mxFactory->createInstance( rtl::OUString::createFromAscii("com.sun.star.util.PathSettings") ), uno::UNO_QUERY_THROW );
    rtl::OUString sOldPathUrl;
    xPathSettings->getPropertyValue( msDefaultFilePath ) >>= sOldPathUrl;
    // path could be a multipath, Microsoft doesn't support this feature in Word currently
    // only the last path is from interest.
    sal_Int32 nIndex = sOldPathUrl.lastIndexOf( sal_Unicode(';') );
    if( nIndex != -1 )
    {
        sNewPathUrl = sOldPathUrl.copy( 0, nIndex + 1 ).concat( sNewPathUrl );
    }
    xPathSettings->setPropertyValue( msDefaultFilePath, uno::makeAny( sNewPathUrl ) );
}

uno::Any SwVbaOptions::getValueEvent()
{
    uno::Reference< beans::XPropertySet > xPathSettings( mxFactory->createInstance( rtl::OUString::createFromAscii("com.sun.star.util.PathSettings") ), uno::UNO_QUERY_THROW );
    rtl::OUString sPathUrl;
    xPathSettings->getPropertyValue( msDefaultFilePath ) >>= sPathUrl;
    // path could be a multipath, Microsoft doesn't support this feature in Word currently
    // only the last path is from interest.
    sal_Int32 nIndex = sPathUrl.lastIndexOf( sal_Unicode(';') );
    if( nIndex != -1 )
    {
        sPathUrl = sPathUrl.copy( nIndex + 1 );
    }
    rtl::OUString sPath;
    ::osl::File::getSystemPathFromFileURL( sPathUrl, sPath );
    return uno::makeAny( sPath );
}

sal_Int32 SAL_CALL SwVbaOptions::getDefaultBorderLineStyle()
{
    return word::WdLineStyle::wdLineStyleSingle;
}

void SAL_CALL SwVbaOptions::setDefaultBorderLineStyle( ::sal_Int32 /*_defaultborderlinestyle*/ )
{
    // not support in Writer
}

sal_Int32 SAL_CALL SwVbaOptions::getDefaultBorderLineWidth()
{
    return word::WdLineWidth::wdLineWidth050pt;
}

void SAL_CALL SwVbaOptions::setDefaultBorderLineWidth( ::sal_Int32 /*_defaultborderlinewidth*/ )
{
    // not support in Writer
}

sal_Int32 SAL_CALL SwVbaOptions::getDefaultBorderColorIndex()
{
    return word::WdColorIndex::wdAuto;
}

void SAL_CALL SwVbaOptions::setDefaultBorderColorIndex( ::sal_Int32 /*_defaultbordercolorindex*/ )
{
    // not support in Writer
}

::sal_Bool SAL_CALL SwVbaOptions::getReplaceSelection()
{
    return sal_True;
}

void SAL_CALL SwVbaOptions::setReplaceSelection( ::sal_Bool /*_replaceselection*/ )
{
    // not support in Writer
}

::sal_Bool SAL_CALL SwVbaOptions::getMapPaperSize()
{
    return sal_False;
}

void SAL_CALL SwVbaOptions::setMapPaperSize( ::sal_Bool /*_mappapersize*/ )
{
    // not support in Writer
}

::sal_Bool SAL_CALL SwVbaOptions::getAutoFormatAsYouTypeApplyHeadings()
{
    return sal_False;
}

void SAL_CALL SwVbaOptions::setAutoFormatAsYouTypeApplyHeadings( ::sal_Bool /*_autoformatasyoutypeapplyheadings*/ )
{
    // not support in Writer
}

::sal_Bool SAL_CALL SwVbaOptions::getAutoFormatAsYouTypeApplyBulletedLists()
{
    return sal_False;
}

void SAL_CALL SwVbaOptions::setAutoFormatAsYouTypeApplyBulletedLists( ::sal_Bool /*_autoformatasyoutypeapplybulletedlists*/ )
{
    // not support in Writer
}

::sal_Bool SAL_CALL SwVbaOptions::getAutoFormatAsYouTypeApplyNumberedLists()
{
    return sal_False;
}

void SAL_CALL SwVbaOptions::setAutoFormatAsYouTypeApplyNumberedLists( ::sal_Bool /*_autoformatasyoutypeapplynumberedlists*/ )
{
    // not support in Writer
}

::sal_Bool SAL_CALL SwVbaOptions::getAutoFormatAsYouTypeFormatListItemBeginning()
{
    return sal_False;
}

void SAL_CALL SwVbaOptions::setAutoFormatAsYouTypeFormatListItemBeginning( ::sal_Bool /*_autoformatasyoutypeformatlistitembeginning*/ )
{
    // not support in Writer
}

::sal_Bool SAL_CALL SwVbaOptions::getAutoFormatAsYouTypeDefineStyles()
{
    return sal_False;
}

void SAL_CALL SwVbaOptions::setAutoFormatAsYouTypeDefineStyles( ::sal_Bool /*_autoformatasyoutypedefinestyles*/ )
{
    // not support in Writer
}

::sal_Bool SAL_CALL SwVbaOptions::getAutoFormatApplyHeadings()
{
    return sal_False;
}

void SAL_CALL SwVbaOptions::setAutoFormatApplyHeadings( ::sal_Bool /*_autoformatapplyheadings*/ )
{
    // not support in Writer
}

::sal_Bool SAL_CALL SwVbaOptions::getAutoFormatApplyLists()
{
    return sal_False;
}

void SAL_CALL SwVbaOptions::setAutoFormatApplyLists( ::sal_Bool /*_autoformatapplylists*/ )
{
    // not support in Writer
}

::sal_Bool SAL_CALL SwVbaOptions::getAutoFormatApplyBulletedLists()
{
    return sal_False;
}

void SAL_CALL SwVbaOptions::setAutoFormatApplyBulletedLists( ::sal_Bool /*_autoformatapplybulletedlists*/ )
{
    // not support in Writer
}


rtl::OUString&
SwVbaOptions::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("SwVbaOptions") );
	return sImplName;
}

uno::Sequence< rtl::OUString >
SwVbaOptions::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		aServiceNames.realloc( 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.Options" ) );
	}
	return aServiceNames;
}
