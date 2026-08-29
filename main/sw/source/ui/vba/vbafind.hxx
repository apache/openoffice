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


#ifndef SW_VBA_FIND_HXX
#define SW_VBA_FIND_HXX

#include <ooo/vba/word/XFind.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/text/XTextRange.hpp>
#include <com/sun/star/util/XReplaceable.hpp>
#include <com/sun/star/util/XPropertyReplace.hpp>
#include <com/sun/star/text/XTextViewCursor.hpp>
#include <com/sun/star/text/XTextCursor.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>

typedef InheritedHelperInterfaceImpl1< ooo::vba::word::XFind > SwVbaFind_BASE;

class SwVbaFind : public SwVbaFind_BASE
{
private:
    css::uno::Reference< css::frame::XModel > mxModel;
    css::uno::Reference< css::text::XTextRange > mxTextRange;
    css::uno::Reference< css::util::XReplaceable > mxReplaceable;
    css::uno::Reference< css::util::XPropertyReplace> mxPropertyReplace;
    css::uno::Reference< css::text::XTextViewCursor> mxTVC;
    css::uno::Reference< css::view::XSelectionSupplier> mxSelSupp;
    sal_Bool mbReplace;
    sal_Int32 mnReplaceType;
    sal_Int32 mnWrap;

private:
    sal_Bool InRange( const css::uno::Reference< css::text::XTextRange >& xCurrentRange );
    sal_Bool InEqualRange( const css::uno::Reference< css::text::XTextRange >& xCurrentRange );
    void SetReplace( sal_Int32 type );
    void SetReplaceWith( const rtl::OUString& rText );
    rtl::OUString GetReplaceWith();
#ifdef TOMORROW
    rtl::OUString ReplaceWildcards( const rtl::OUString& rText );
#endif
    css::uno::Reference< css::text::XTextRange > FindOneElement();
    sal_Bool SearchReplace();

public:
	SwVbaFind( const css::uno::Reference< ooo::vba::XHelperInterface >& rParent, const css::uno::Reference< css::uno::XComponentContext >& rContext, const css::uno::Reference< css::frame::XModel >& xModel, const css::uno::Reference< css::text::XTextRange >& xTextRange );
	virtual ~SwVbaFind();

    // Attributes
    virtual ::rtl::OUString SAL_CALL getText();
    virtual void SAL_CALL setText( const ::rtl::OUString& _text );
    virtual css::uno::Any SAL_CALL getReplacement();
    virtual void SAL_CALL setReplacement( const css::uno::Any& _replacement );
    virtual ::sal_Bool SAL_CALL getForward();
    virtual void SAL_CALL setForward( ::sal_Bool _forward );
    virtual ::sal_Int32 SAL_CALL getWrap();
    virtual void SAL_CALL setWrap( ::sal_Int32 _wrap );
    virtual ::sal_Bool SAL_CALL getFormat();
    virtual void SAL_CALL setFormat( ::sal_Bool _format );
    virtual ::sal_Bool SAL_CALL getMatchCase();
    virtual void SAL_CALL setMatchCase( ::sal_Bool _matchcase );
    virtual ::sal_Bool SAL_CALL getMatchWholeWord();
    virtual void SAL_CALL setMatchWholeWord( ::sal_Bool _matchwholeword );
    virtual ::sal_Bool SAL_CALL getMatchWildcards();
    virtual void SAL_CALL setMatchWildcards( ::sal_Bool _matchwildcards );
    virtual ::sal_Bool SAL_CALL getMatchSoundsLike();
    virtual void SAL_CALL setMatchSoundsLike( ::sal_Bool _matchsoundslike );
    virtual ::sal_Bool SAL_CALL getMatchAllWordForms();
    virtual void SAL_CALL setMatchAllWordForms( ::sal_Bool _matchallwordforms );
    virtual css::uno::Any SAL_CALL getStyle();
    virtual void SAL_CALL setStyle( const css::uno::Any& _style );

    // Methods
    virtual ::sal_Bool SAL_CALL Execute( const css::uno::Any& FindText, const css::uno::Any& MatchCase, const css::uno::Any& MatchWholeWord, const css::uno::Any& MatchWildcards, const css::uno::Any& MatchSoundsLike, const css::uno::Any& MatchAllWordForms, const css::uno::Any& Forward, const css::uno::Any& Wrap, const css::uno::Any& Format, const css::uno::Any& ReplaceWith, const css::uno::Any& Replace, const css::uno::Any& MatchKashida, const css::uno::Any& MatchDiacritics, const css::uno::Any& MatchAlefHamza, const css::uno::Any& MatchControl, const css::uno::Any& MatchPrefix, const css::uno::Any& MatchSuffix, const css::uno::Any& MatchPhrase, const css::uno::Any& IgnoreSpace, const css::uno::Any& IgnorePunct );
    virtual void SAL_CALL ClearFormatting(  );

	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif /* SW_VBA_FIND_HXX */
