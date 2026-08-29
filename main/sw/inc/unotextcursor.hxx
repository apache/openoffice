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



#ifndef SW_UNOTEXTCURSOR_HXX
#define SW_UNOTEXTCURSOR_HXX

#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/beans/XMultiPropertyStates.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XContentEnumerationAccess.hpp>
#include <com/sun/star/util/XSortable.hpp>
#include <com/sun/star/document/XDocumentInsertable.hpp>
#include <com/sun/star/text/XSentenceCursor.hpp>
#include <com/sun/star/text/XWordCursor.hpp>
#include <com/sun/star/text/XParagraphCursor.hpp>
#include <com/sun/star/text/XRedline.hpp>
#include <com/sun/star/text/XMarkingAccess.hpp>

#include <cppuhelper/implbase12.hxx>

#include <comphelper/uno3.hxx>

#if ! defined(INCLUDED_COMPHELPER_IMPLBASE_VAR_HXX_13)
#define INCLUDED_COMPHELPER_IMPLBASE_VAR_HXX_13
#define COMPHELPER_IMPLBASE_INTERFACE_NUMBER 13
#include <comphelper/implbase_var.hxx>
#endif

#include <unobaseclass.hxx>
#include <TextCursorHelper.hxx>


class SwDoc;
struct SwPosition;
class SwUnoCrsr;

typedef ::comphelper::WeakImplHelper13
<   ::com::sun::star::lang::XServiceInfo
,   ::com::sun::star::beans::XPropertySet
,   ::com::sun::star::beans::XPropertyState
,   ::com::sun::star::beans::XMultiPropertyStates
,   ::com::sun::star::container::XEnumerationAccess
,   ::com::sun::star::container::XContentEnumerationAccess
,   ::com::sun::star::util::XSortable
,   ::com::sun::star::document::XDocumentInsertable
,   ::com::sun::star::text::XSentenceCursor
,   ::com::sun::star::text::XWordCursor
,   ::com::sun::star::text::XParagraphCursor
,   ::com::sun::star::text::XRedline
,   ::com::sun::star::text::XMarkingAccess
> SwXTextCursor_Base;

class SwXTextCursor
    : public SwXTextCursor_Base
    , public OTextCursorHelper
{

private:

    class Impl;
    ::sw::UnoImplPtr<Impl> m_pImpl;

    virtual ~SwXTextCursor();

public:

    SwXTextCursor(
            SwDoc & rDoc,
            ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XText > const& xParent,
            const enum CursorType eType,
            SwPosition const& rPos,
            SwPosition const*const pMark = 0);
    SwXTextCursor(
            ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XText > const& xParent,
            SwPaM const& rSourceCursor,
            const enum CursorType eType = CURSOR_ALL);

          SwUnoCrsr *   GetCursor();
    const SwUnoCrsr *   GetCursor() const;

    bool IsAtEndOfMeta() const;

    void DeleteAndInsert(::rtl::OUString const& rText,
                const bool bForceExpandHints);

    // OTextCursorHelper
    virtual const SwPaM*        GetPaM() const;
    virtual SwPaM*              GetPaM();
    virtual const SwDoc*        GetDoc() const;
    virtual SwDoc*              GetDoc();

    DECLARE_XINTERFACE()

    static const ::com::sun::star::uno::Sequence< sal_Int8 >& getUnoTunnelId();

    // XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething(
            const ::com::sun::star::uno::Sequence< sal_Int8 >& rIdentifier);

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService(
            const ::rtl::OUString& rServiceName);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
        getSupportedServiceNames();

    // XPropertySet
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XPropertySetInfo > SAL_CALL
        getPropertySetInfo();
    virtual void SAL_CALL setPropertyValue(
            const ::rtl::OUString& rPropertyName,
            const ::com::sun::star::uno::Any& rValue);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
            const ::rtl::OUString& rPropertyName);
    virtual void SAL_CALL addPropertyChangeListener(
            const ::rtl::OUString& rPropertyName,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XPropertyChangeListener >& xListener);
    virtual void SAL_CALL removePropertyChangeListener(
            const ::rtl::OUString& rPropertyName,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XPropertyChangeListener >& xListener);
    virtual void SAL_CALL addVetoableChangeListener(
            const ::rtl::OUString& rPropertyName,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XVetoableChangeListener >& xListener);
    virtual void SAL_CALL removeVetoableChangeListener(
            const ::rtl::OUString& rPropertyName,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XVetoableChangeListener >& xListener);

    // XPropertyState
    virtual ::com::sun::star::beans::PropertyState SAL_CALL
        getPropertyState(const ::rtl::OUString& rPropertyName);
    virtual ::com::sun::star::uno::Sequence<
            ::com::sun::star::beans::PropertyState > SAL_CALL
        getPropertyStates(
            const ::com::sun::star::uno::Sequence<
                ::rtl::OUString >& rPropertyNames);
    virtual void SAL_CALL setPropertyToDefault(
            const ::rtl::OUString& rPropertyName);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault(
            const ::rtl::OUString& rPropertyName);

    // XMultiPropertyStates
    virtual void SAL_CALL setAllPropertiesToDefault();
    virtual void SAL_CALL setPropertiesToDefault(
            const ::com::sun::star::uno::Sequence< ::rtl::OUString >&
                rPropertyNames);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >
        SAL_CALL getPropertyDefaults(
            const ::com::sun::star::uno::Sequence< ::rtl::OUString >&
                rPropertyNames);

    // XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
    virtual sal_Bool SAL_CALL hasElements();

    // XEnumerationAccess
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::container::XEnumeration >  SAL_CALL
        createEnumeration();

    // XContentEnumerationAccess
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::container::XEnumeration > SAL_CALL
        createContentEnumeration(const ::rtl::OUString& rServiceName);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
        getAvailableServiceNames();

    // XSortable
    virtual ::com::sun::star::uno::Sequence<
                ::com::sun::star::beans::PropertyValue > SAL_CALL
        createSortDescriptor();
    virtual void SAL_CALL sort(
            const ::com::sun::star::uno::Sequence<
                ::com::sun::star::beans::PropertyValue >& xDescriptor);

    // XDocumentInsertable
    virtual void SAL_CALL insertDocumentFromURL(
            const ::rtl::OUString& rURL,
            const ::com::sun::star::uno::Sequence<
                ::com::sun::star::beans::PropertyValue >& rOptions);

    // XTextRange
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XText >
        SAL_CALL getText();
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > SAL_CALL getStart();
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > SAL_CALL getEnd();
    virtual ::rtl::OUString SAL_CALL getString();
    virtual void SAL_CALL setString(const ::rtl::OUString& rString);

    // XTextCursor
    virtual void SAL_CALL collapseToStart();
    virtual void SAL_CALL collapseToEnd();
    virtual sal_Bool SAL_CALL isCollapsed();
    virtual sal_Bool SAL_CALL goLeft(sal_Int16 nCount, sal_Bool bExpand);
    virtual sal_Bool SAL_CALL goRight(sal_Int16 nCount, sal_Bool bExpand);
    virtual void SAL_CALL gotoStart(sal_Bool bExpand);
    virtual void SAL_CALL gotoEnd(sal_Bool bExpand);
    virtual void SAL_CALL gotoRange(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange >& xRange,
            sal_Bool bExpand);

    // XWordCursor
    virtual sal_Bool SAL_CALL isStartOfWord();
    virtual sal_Bool SAL_CALL isEndOfWord();
    virtual sal_Bool SAL_CALL gotoNextWord(sal_Bool bExpand);
    virtual sal_Bool SAL_CALL gotoPreviousWord(sal_Bool bExpand);
    virtual sal_Bool SAL_CALL gotoEndOfWord(sal_Bool bExpand);
    virtual sal_Bool SAL_CALL gotoStartOfWord(sal_Bool bExpand);

    // XSentenceCursor
    virtual sal_Bool SAL_CALL isStartOfSentence();
    virtual sal_Bool SAL_CALL isEndOfSentence();
    virtual sal_Bool SAL_CALL gotoNextSentence(sal_Bool Expand);
    virtual sal_Bool SAL_CALL gotoPreviousSentence(sal_Bool Expand);
    virtual sal_Bool SAL_CALL gotoStartOfSentence(sal_Bool Expand);
    virtual sal_Bool SAL_CALL gotoEndOfSentence(sal_Bool Expand);

    // XParagraphCursor
    virtual sal_Bool SAL_CALL isStartOfParagraph();
    virtual sal_Bool SAL_CALL isEndOfParagraph();
    virtual sal_Bool SAL_CALL gotoStartOfParagraph(sal_Bool Expand);
    virtual sal_Bool SAL_CALL gotoEndOfParagraph(sal_Bool Expand);
    virtual sal_Bool SAL_CALL gotoNextParagraph(sal_Bool Expand);
    virtual sal_Bool SAL_CALL gotoPreviousParagraph(sal_Bool Expand);

    // XRedline
    virtual void SAL_CALL makeRedline(
            const ::rtl::OUString& rRedlineType,
            const ::com::sun::star::uno::Sequence<
                ::com::sun::star::beans::PropertyValue >& RedlineProperties);

    //XMarkingAccess
    virtual void SAL_CALL invalidateMarkings(::sal_Int32 nType);

};

#endif // SW_UNOTEXTCURSOR_HXX
