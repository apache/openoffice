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

#ifndef PPTEXP_BASE_HXX
#define PPTEXP_BASE_HXX

#include "eppt.hxx"
#include <vector>
#include <vcl/mapmod.hxx>
#include <tools/list.hxx>
#include <tools/string.hxx>
#include <tools/stream.hxx>
#include <tools/gen.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>
#include <com/sun/star/drawing/XMasterPagesSupplier.hpp>
#include <com/sun/star/drawing/XMasterPageTarget.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/presentation/XPresentationSupplier.hpp>
#include <com/sun/star/presentation/FadeEffect.hpp>
#include <com/sun/star/task/XStatusIndicatorSupplier.hpp>

class PPTEXPBase : public PropValue, public GroupTable
{
protected:
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >                 mXModel;
    ::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator >        mXStatusIndicator;

    sal_Bool            mbStatusIndicator;

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPagesSupplier >   mXDrawPagesSupplier;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XMasterPagesSupplier > mXMasterPagesSupplier;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPages >           mXDrawPages;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >            mXDrawPage;
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >           mXPagePropSet;
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >           mXBackgroundPropSet;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >              mXShapes;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >               mXShape;
    ::com::sun::star::awt::Size         maSize;
    ::com::sun::star::awt::Point        maPosition;
    Rectangle           maRect;
    ByteString          mType;
    sal_Bool            mbPresObj;
    sal_Bool            mbEmptyPresObj;
    sal_Int32           mnAngle;

    sal_uInt32          mnPages;            // number of Slides ( w/o master pages & notes & handout )
    sal_uInt32          mnMasterPages;

    Fraction                        maFraction;
    MapMode                         maMapModeSrc;
    MapMode                         maMapModeDest;
    ::com::sun::star::awt::Size     maDestPageSize;
    ::com::sun::star::awt::Size     maNotesPageSize;

    PageType                        meLatestPageType;
    std::vector< PPTExStyleSheet* > maStyleSheetList;
    PPTExStyleSheet*                mpStyleSheet;

    FontCollection      maFontCollection;

    List                maTextRuleList;     // TextRuleEntry's

    virtual void ImplWriteSlide( sal_uInt32 /* nPageNum */, sal_uInt32 /* nMasterNum */, sal_uInt16 /* nMode */,
                                 sal_Bool /* bHasBackground */, ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > /* aXBackgroundPropSet */ ) {}
    virtual void ImplWriteNotes( sal_uInt32 nPageNum ) = 0;
    virtual void ImplWriteSlideMaster( sal_uInt32 /* nPageNum */, ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > /* aXBackgroundPropSet */ ) {}
    virtual void ImplWriteLayout( sal_Int32 /* nOffset */, sal_uInt32 /* nMasterNum */ ) {}

    virtual void exportPPTPre() {}
    virtual void exportPPTPost() {}

    virtual sal_Bool ImplCreateDocument()=0;
    virtual sal_Bool ImplCreateMainNotes()=0;

    sal_Bool ImplGetStyleSheets();
    sal_Bool ImplGetShapeByIndex( sal_uInt32 nIndex, sal_Bool bGroup = sal_False );

    sal_Bool CreateMainNotes();

    ::com::sun::star::awt::Size   ImplMapSize( const ::com::sun::star::awt::Size& );
    ::com::sun::star::awt::Point  ImplMapPoint( const ::com::sun::star::awt::Point& );
    Rectangle                     ImplMapRectangle( const ::com::sun::star::awt::Rectangle& );

    sal_Bool ContainsOtherShapeThanPlaceholders( sal_Bool bForOOMLX );

public:
    PPTEXPBase();
    PPTEXPBase( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > & rModel,
                   const ::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator > & rStatInd );

    ~PPTEXPBase();
    
    sal_Bool InitUnoModel();
    void exportPPT();
   	
    sal_Bool ImplGetPageByIndex( sal_uInt32 nIndex, PageType );
    sal_uInt32 ImplGetMasterIndex( PageType ePageType );
    sal_Bool ImplSetCurrentStyleSheet( sal_uInt32 nPageNum );

    sal_Bool GetPresObj() { return mbPresObj; }

    PHLayout& GetLayout( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& rXPropSet ) const;
    PHLayout& GetLayout( sal_Int32 nOffset ) const;
    sal_Int32 GetLayoutOffset( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& rXPropSet ) const;
    sal_Int32 GetLayoutOffsetFixed( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& rXPropSet ) const;

    sal_Bool ImplCreateSlide( sal_uInt32 nPageNum );
    sal_Bool ImplCreateSlideMaster( sal_uInt32 nPageNum );
    sal_Bool ImplCreateNotes( sal_uInt32 nPageNum );

    static sal_Int8 GetTransition( sal_Int16 nTransitionType, sal_Int16 nTransitionSubtype, ::com::sun::star::presentation::FadeEffect eEffect, sal_uInt8& nDirection );
    static sal_Int8 GetTransition( ::com::sun::star::presentation::FadeEffect eEffect, sal_uInt8& nDirection );
};


#endif
