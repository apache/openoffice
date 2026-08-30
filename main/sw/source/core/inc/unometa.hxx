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



#ifndef SW_UNOMETA_HXX
#define SW_UNOMETA_HXX

#include <deque>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/text/XTextContent.hpp>
#include <com/sun/star/text/XTextField.hpp>

#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase6.hxx>

#include <sfx2/Metadatable.hxx>

#include <unobaseclass.hxx>


typedef ::std::deque<
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > >
    TextRangeList_t;

class SwPaM;
class SwTxtNode;

namespace sw {
    class Meta;
    class MetaField;
}

typedef ::cppu::ImplInheritanceHelper6
<   ::sfx2::MetadatableMixin
,   ::com::sun::star::lang::XUnoTunnel
,   ::com::sun::star::lang::XServiceInfo
,   ::com::sun::star::container::XChild
,   ::com::sun::star::container::XEnumerationAccess
,   ::com::sun::star::text::XTextContent
,   ::com::sun::star::text::XText
> SwXMeta_Base;

class SwXMeta
    : public SwXMeta_Base
    , private ::boost::noncopyable
{

public:

    class Impl;

protected:

    ::sw::UnoImplPtr<Impl> m_pImpl;

    virtual void SAL_CALL AttachImpl(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > & xTextRange,
            const sal_uInt16 nWhich);

    virtual ~SwXMeta();

    /// @param pDoc and pMeta != 0, but not & because of ImplInheritanceHelper
    SwXMeta(SwDoc *const pDoc, ::sw::Meta *const pMeta,
        ::com::sun::star::uno::Reference< ::com::sun::star::text::XText> const&
            xParentText,
        TextRangeList_t const*const pPortions);

public:

    SwXMeta(SwDoc *const pDoc);

    static ::com::sun::star::uno::Reference<
            ::com::sun::star::rdf::XMetadatable >
        CreateXMeta(
            ::sw::Meta & rMeta,
            ::com::sun::star::uno::Reference< ::com::sun::star::text::XText>
                const& xParentText = 0,
            ::std::auto_ptr<TextRangeList_t const> pPortions =
                ::std::auto_ptr<TextRangeList_t const>(0));

    /// init params with position of the attribute content (w/out CH_TXTATR)
    bool SetContentRange(
            SwTxtNode *& rpNode, xub_StrLen & rStart, xub_StrLen & rEnd) const;
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XText >
        GetParentText() const;

    bool CheckForOwnMemberMeta(const SwPaM & rPam, const bool bAbsorb);

    // MetadatableMixin
    virtual ::sfx2::Metadatable * GetCoreObject();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >
        GetModel();

    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

    // XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething(
            const ::com::sun::star::uno::Sequence< sal_Int8 >& Identifier );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService(
            const ::rtl::OUString& rServiceName);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
        getSupportedServiceNames();

    // XComponent
    virtual void SAL_CALL dispose();
    virtual void SAL_CALL addEventListener(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::lang::XEventListener > & xListener);
    virtual void SAL_CALL removeEventListener(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::lang::XEventListener > & xListener);

    // XChild
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XInterface > SAL_CALL
        getParent();
    virtual void SAL_CALL setParent(
            ::com::sun::star::uno::Reference<
                ::com::sun::star::uno::XInterface> const& xParent);

    // XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
    virtual sal_Bool SAL_CALL hasElements();

    // XEnumerationAccess
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::container::XEnumeration >  SAL_CALL
        createEnumeration();

    // XTextContent
    virtual void SAL_CALL attach(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > & xTextRange);
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > SAL_CALL getAnchor();

    // XTextRange
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XText >
        SAL_CALL getText();
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > SAL_CALL getStart();
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > SAL_CALL getEnd();
    virtual rtl::OUString SAL_CALL getString();
    virtual void SAL_CALL setString(const rtl::OUString& rString);

    // XSimpleText
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextCursor >  SAL_CALL
        createTextCursor();
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextCursor >  SAL_CALL
        createTextCursorByRange(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > & xTextPosition);
    virtual void SAL_CALL insertString(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > & xRange,
            const rtl::OUString& aString, sal_Bool bAbsorb);
    virtual void SAL_CALL insertControlCharacter(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > & xRange,
            sal_Int16 nControlCharacter, sal_Bool bAbsorb);

    // XText
    virtual void SAL_CALL insertTextContent(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > & xRange,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextContent > & xContent,
            sal_Bool bAbsorb);
    virtual void SAL_CALL removeTextContent(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextContent > & xContent);

};


typedef ::cppu::ImplInheritanceHelper2
<   SwXMeta
,   ::com::sun::star::beans::XPropertySet
,   ::com::sun::star::text::XTextField
> SwXMetaField_Base;

class SwXMetaField
    : public SwXMetaField_Base
{

private:

    virtual ~SwXMetaField();

    friend ::com::sun::star::uno::Reference<
            ::com::sun::star::rdf::XMetadatable >
        SwXMeta::CreateXMeta(::sw::Meta &,
            ::com::sun::star::uno::Reference< ::com::sun::star::text::XText>
                const&,
            ::std::auto_ptr<TextRangeList_t const> pPortions);

    SwXMetaField(SwDoc *const pDoc, ::sw::Meta *const pMeta,
        ::com::sun::star::uno::Reference< ::com::sun::star::text::XText> const&
            xParentText,
        TextRangeList_t const*const pPortions);

public:

    SwXMetaField(SwDoc *const pDoc);

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService(
            const ::rtl::OUString& ServiceName);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
        getSupportedServiceNames( );

    // XComponent
    virtual void SAL_CALL dispose();
    virtual void SAL_CALL addEventListener(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::lang::XEventListener > & xListener);
    virtual void SAL_CALL removeEventListener(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::lang::XEventListener > & xListener);

    // XPropertySet
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::beans::XPropertySetInfo > SAL_CALL
        getPropertySetInfo();
    virtual void SAL_CALL setPropertyValue(
            const ::rtl::OUString& rPropertyName,
            const ::com::sun::star::uno::Any& rValue);
    virtual ::com::sun::star::uno::Any SAL_CALL
        getPropertyValue(const ::rtl::OUString& rPropertyName);
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

    // XTextContent
    virtual void SAL_CALL attach(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > & xTextRange);
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::text::XTextRange > SAL_CALL getAnchor();

    // XTextField
    virtual rtl::OUString SAL_CALL getPresentation(sal_Bool bShowCommand);

};

/// get prefix/suffix from the RDF repository. @throws RuntimeException
void getPrefixAndSuffix(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::frame::XModel>& xModel,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::rdf::XMetadatable>& xMetaField,
        ::rtl::OUString *const o_pPrefix, ::rtl::OUString *const o_pSuffix);

#endif // SW_UNOMETA_HXX
