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


#ifndef SC_VBA_FONT_HXX
#define SC_VBA_FONT_HXX

#include <cppuhelper/implbase1.hxx>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <ooo/vba/excel/XFont.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <vbahelper/vbafontbase.hxx>
#include "vbapalette.hxx"

class ScTableSheetsObj;
class ScCellRangeObj;

typedef cppu::ImplInheritanceHelper1< VbaFontBase, ov::excel::XFont > ScVbaFont_BASE;

class ScVbaFont : public ScVbaFont_BASE
{
    ScVbaPalette mPalette;
    ScCellRangeObj* mpRangeObj;
    SfxItemSet*  GetDataSet();
public:
	ScVbaFont(
        const css::uno::Reference< ov::XHelperInterface >& xParent,
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const ScVbaPalette& dPalette,
        const css::uno::Reference< css::beans::XPropertySet >& xPropertySet,
        ScCellRangeObj* pRangeObj = 0, bool bFormControl = false );
	virtual ~ScVbaFont();// {}

	// Attributes
    virtual css::uno::Any SAL_CALL getSize();
    virtual css::uno::Any SAL_CALL getStandardFontSize();
    virtual void SAL_CALL setStandardFontSize( const css::uno::Any& _standardfontsize );
    virtual css::uno::Any SAL_CALL getStandardFont();
    virtual void SAL_CALL setStandardFont( const css::uno::Any& _standardfont );
    virtual css::uno::Any SAL_CALL getFontStyle();
    virtual void SAL_CALL setFontStyle( const css::uno::Any& _fontstyle );
    virtual css::uno::Any SAL_CALL getColorIndex();
    virtual void SAL_CALL setColorIndex( const css::uno::Any& _colorindex );
    virtual css::uno::Any SAL_CALL getBold();
    virtual css::uno::Any SAL_CALL getUnderline();
    virtual void SAL_CALL setUnderline( const css::uno::Any& _underline );
    virtual css::uno::Any SAL_CALL getStrikethrough();
    virtual css::uno::Any SAL_CALL getShadow();
    virtual css::uno::Any SAL_CALL getItalic();
    virtual css::uno::Any SAL_CALL getSubscript();
    virtual void SAL_CALL setSubscript( const css::uno::Any& _subscript );
    virtual css::uno::Any SAL_CALL getSuperscript();
    virtual void SAL_CALL setSuperscript( const css::uno::Any& _superscript );
    virtual css::uno::Any SAL_CALL getName();
    virtual css::uno::Any SAL_CALL getColor() ;
    virtual css::uno::Any SAL_CALL getOutlineFont() ;
    virtual void SAL_CALL setOutlineFont( const css::uno::Any& _outlinefont ) ;
    // XHelperInterface
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();

};



#endif /* SC_VBA_FONT_HXX */
