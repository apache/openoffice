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


#ifndef SW_VBA_OPTIONS_HXX
#define SW_VBA_OPTIONS_HXX

#include <ooo/vba/word/XOptions.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <cppuhelper/implbase1.hxx>
#include <vbahelper/vbapropvalue.hxx>
#include <comphelper/processfactory.hxx>

typedef InheritedHelperInterfaceImpl1< ooo::vba::word::XOptions > SwVbaOptions_BASE;

class SwVbaOptions : public SwVbaOptions_BASE,
                    public PropListener
{
private:
    rtl::OUString msDefaultFilePath;
    css::uno::Reference< css::lang::XMultiServiceFactory > mxFactory;
public:
	SwVbaOptions( css::uno::Reference< css::uno::XComponentContext >& m_xContext );
	virtual ~SwVbaOptions();

    // Attributes
    virtual ::sal_Int32 SAL_CALL getDefaultBorderLineStyle();
    virtual void SAL_CALL setDefaultBorderLineStyle( ::sal_Int32 _defaultborderlinestyle );
    virtual ::sal_Int32 SAL_CALL getDefaultBorderLineWidth();
    virtual void SAL_CALL setDefaultBorderLineWidth( ::sal_Int32 _defaultborderlinewidth );
    virtual ::sal_Int32 SAL_CALL getDefaultBorderColorIndex();
    virtual void SAL_CALL setDefaultBorderColorIndex( ::sal_Int32 _defaultbordercolorindex );
    virtual ::sal_Bool SAL_CALL getReplaceSelection();
    virtual void SAL_CALL setReplaceSelection( ::sal_Bool _replaceselection );
    virtual ::sal_Bool SAL_CALL getMapPaperSize();
    virtual void SAL_CALL setMapPaperSize( ::sal_Bool _mappapersize );
    virtual ::sal_Bool SAL_CALL getAutoFormatAsYouTypeApplyHeadings();
    virtual void SAL_CALL setAutoFormatAsYouTypeApplyHeadings( ::sal_Bool _autoformatasyoutypeapplyheadings );
    virtual ::sal_Bool SAL_CALL getAutoFormatAsYouTypeApplyBulletedLists();
    virtual void SAL_CALL setAutoFormatAsYouTypeApplyBulletedLists( ::sal_Bool _autoformatasyoutypeapplybulletedlists );
    virtual ::sal_Bool SAL_CALL getAutoFormatAsYouTypeApplyNumberedLists();
    virtual void SAL_CALL setAutoFormatAsYouTypeApplyNumberedLists( ::sal_Bool _autoformatasyoutypeapplynumberedlists );
    virtual ::sal_Bool SAL_CALL getAutoFormatAsYouTypeFormatListItemBeginning();
    virtual void SAL_CALL setAutoFormatAsYouTypeFormatListItemBeginning( ::sal_Bool _autoformatasyoutypeformatlistitembeginning );
    virtual ::sal_Bool SAL_CALL getAutoFormatAsYouTypeDefineStyles();
    virtual void SAL_CALL setAutoFormatAsYouTypeDefineStyles( ::sal_Bool _autoformatasyoutypedefinestyles );
    virtual ::sal_Bool SAL_CALL getAutoFormatApplyHeadings();
    virtual void SAL_CALL setAutoFormatApplyHeadings( ::sal_Bool _autoformatapplyheadings );
    virtual ::sal_Bool SAL_CALL getAutoFormatApplyLists();
    virtual void SAL_CALL setAutoFormatApplyLists( ::sal_Bool _autoformatapplylists );
    virtual ::sal_Bool SAL_CALL getAutoFormatApplyBulletedLists();
    virtual void SAL_CALL setAutoFormatApplyBulletedLists( ::sal_Bool _autoformatapplybulletedlists );

    // Methods
    virtual css::uno::Any SAL_CALL DefaultFilePath( sal_Int32 _path );

    //PropListener
    virtual void setValueEvent( const css::uno::Any& value );
    virtual css::uno::Any getValueEvent();

	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SW_VBA_OPTIONS_HXX */
