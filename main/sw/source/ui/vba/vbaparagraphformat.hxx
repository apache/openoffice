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


#ifndef SW_VBA_PARAGRAPHFORMAT_HXX
#define SW_VBA_PARAGRAPHFORMAT_HXX

#include <ooo/vba/word/XParagraphFormat.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/style/LineSpacing.hpp>

typedef InheritedHelperInterfaceImpl1< ooo::vba::word::XParagraphFormat > SwVbaParagraphFormat_BASE;

class SwVbaParagraphFormat : public SwVbaParagraphFormat_BASE
{
private:
    css::uno::Reference< css::text::XTextDocument > mxTextDocument;
    css::uno::Reference< css::beans::XPropertySet > mxParaProps;

private:
    css::style::LineSpacing getOOoLineSpacing( float _lineSpace, sal_Int16 mode );
    css::style::LineSpacing getOOoLineSpacingFromRule( sal_Int32 _linespacingrule );
    float getMSWordLineSpacing( css::style::LineSpacing& rLineSpacing );
    sal_Int32 getMSWordLineSpacingRule( css::style::LineSpacing& rLineSpacing );
    sal_Int16 getCharHeight();
    sal_Int32 getOOoAlignment( sal_Int32 _alignment );
    sal_Int32 getMSWordAlignment( sal_Int32 _alignment );

public:
	SwVbaParagraphFormat( const css::uno::Reference< ooo::vba::XHelperInterface >& rParent, const css::uno::Reference< css::uno::XComponentContext >& rContext, const css::uno::Reference< css::text::XTextDocument >& rTextDocument, const css::uno::Reference< css::beans::XPropertySet >& rParaProps );
	virtual ~SwVbaParagraphFormat();

    // Attributes
    virtual ::sal_Int32 SAL_CALL getAlignment();
    virtual void SAL_CALL setAlignment( ::sal_Int32 _alignment );
    virtual float SAL_CALL getFirstLineIndent();
    virtual void SAL_CALL setFirstLineIndent( float _firstlineindent );
    virtual css::uno::Any SAL_CALL getKeepTogether();
    virtual void SAL_CALL setKeepTogether( const css::uno::Any& _keeptogether );
    virtual css::uno::Any SAL_CALL getKeepWithNext();
    virtual void SAL_CALL setKeepWithNext( const css::uno::Any& _keepwithnext );
    virtual css::uno::Any SAL_CALL getHyphenation();
    virtual void SAL_CALL setHyphenation( const css::uno::Any& _hyphenation );
    virtual float SAL_CALL getLineSpacing();
    virtual void SAL_CALL setLineSpacing( float _linespacing );
    virtual ::sal_Int32 SAL_CALL getLineSpacingRule();
    virtual void SAL_CALL setLineSpacingRule( ::sal_Int32 _linespacingrule );
    virtual css::uno::Any SAL_CALL getNoLineNumber();
    virtual void SAL_CALL setNoLineNumber( const css::uno::Any& _nolinenumber );
    virtual ::sal_Int32 SAL_CALL getOutlineLevel();
    virtual void SAL_CALL setOutlineLevel( ::sal_Int32 _outlinelevel );
    virtual css::uno::Any SAL_CALL getPageBreakBefore();
    virtual void SAL_CALL setPageBreakBefore( const css::uno::Any& _pagebreakbefore );
    virtual float SAL_CALL getSpaceBefore();
    virtual void SAL_CALL setSpaceBefore( float _spacebefore );
    virtual float SAL_CALL getSpaceAfter();
    virtual void SAL_CALL setSpaceAfter( float _spaceafter );
    virtual float SAL_CALL getLeftIndent();
    virtual void SAL_CALL setLeftIndent( float _leftindent );
    virtual float SAL_CALL getRightIndent();
    virtual void SAL_CALL setRightIndent( float _rightindent );
    virtual css::uno::Any SAL_CALL getTabStops();
    virtual void SAL_CALL setTabStops( const css::uno::Any& _tabstops );
    virtual css::uno::Any SAL_CALL getWidowControl();
    virtual void SAL_CALL setWidowControl( const css::uno::Any& _widowcontrol );

	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SW_VBA_PARAGRAPHFORMAT_HXX */
