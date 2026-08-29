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

#ifndef VBAHELPER_FONTBASE_HXX
#define VBAHELPER_FONTBASE_HXX

#include <cppuhelper/implbase1.hxx>

#include <ooo/vba/XFontBase.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <vbahelper/vbahelperinterface.hxx>

// use local constants there is no need to expose these constants
// externally. Looking at the Format->Character dialog it seem that
// these may in fact be even be calculated. Leave hardcoded for now
// #FIXEME #TBD investigate the code for dialog mentioned above

// The font baseline is not specified.
const short NORMAL = 0;

// specifies a superscripted.
const short SUPERSCRIPT = 33;

// specifies a subscripted.
const short SUBSCRIPT = -33;

// specifies a height of superscripted font
 const sal_Int8 SUPERSCRIPTHEIGHT = 58;

// specifies a height of subscripted font
const sal_Int8 SUBSCRIPTHEIGHT = 58;

// specifies a height of normal font
const short NORMALHEIGHT = 100;

typedef InheritedHelperInterfaceImpl1< ov::XFontBase > VbaFontBase_BASE;

class VBAHELPER_DLLPUBLIC VbaFontBase : public VbaFontBase_BASE
{
protected:
	css::uno::Reference< css::beans::XPropertySet > mxFont;
	css::uno::Reference< css::container::XIndexAccess > mxPalette;
	bool mbFormControl;

public:
	VbaFontBase(
		const css::uno::Reference< ov::XHelperInterface >& xParent,
		const css::uno::Reference< css::uno::XComponentContext >& xContext,
		const css::uno::Reference< css::container::XIndexAccess >& xPalette,
		const css::uno::Reference< css::beans::XPropertySet >& xPropertySet,
		bool bFormControl = false );
	virtual ~VbaFontBase();// {}

	// Attributes
	virtual css::uno::Any SAL_CALL getSize();
	virtual void SAL_CALL setSize( const css::uno::Any& _size );
	virtual css::uno::Any SAL_CALL getColorIndex();
	virtual void SAL_CALL setColorIndex( const css::uno::Any& _colorindex );
	virtual css::uno::Any SAL_CALL getBold();
	virtual void SAL_CALL setBold( const css::uno::Any& _bold );
	virtual css::uno::Any SAL_CALL getUnderline() = 0;
	virtual void SAL_CALL setUnderline( const css::uno::Any& _underline ) = 0;
	virtual css::uno::Any SAL_CALL getStrikethrough();
	virtual void SAL_CALL setStrikethrough( const css::uno::Any& _strikethrough );
	virtual css::uno::Any SAL_CALL getShadow();
	virtual void SAL_CALL setShadow( const css::uno::Any& _shadow );
	virtual css::uno::Any SAL_CALL getItalic();
	virtual void SAL_CALL setItalic( const css::uno::Any& _italic );
	virtual css::uno::Any SAL_CALL getSubscript();
	virtual void SAL_CALL setSubscript( const css::uno::Any& _subscript );
	virtual css::uno::Any SAL_CALL getSuperscript();
	virtual void SAL_CALL setSuperscript( const css::uno::Any& _superscript );
	virtual css::uno::Any SAL_CALL getName();
	virtual void SAL_CALL setName( const css::uno::Any& _name );
	virtual css::uno::Any SAL_CALL getColor() ;
	virtual void SAL_CALL setColor( const css::uno::Any& _color ) ;
};

#endif

/* vim: set noet sw=4 ts=4: */
