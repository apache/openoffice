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



#ifndef SW_UNOPARAGRAPH_HXX
#define SW_UNOPARAGRAPH_HXX

#include <memory>

#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XTolerantMultiPropertySet.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XContentEnumerationAccess.hpp>
#include <com/sun/star/text/XTextContent.hpp>
#include <com/sun/star/text/XTextRange.hpp>

#include <cppuhelper/implbase10.hxx>

#include <sfx2/Metadatable.hxx>

#include <unobaseclass.hxx>


struct SwPosition;
class SwPaM;
class SwUnoCrsr;
class SwStartNode;
class SwTxtNode;
class SwTable;
class SwXText;


/*-----------------07.04.98 08:15-------------------

--------------------------------------------------*/
typedef ::cppu::ImplInheritanceHelper10
<   ::sfx2::MetadatableMixin
,   ::com::sun::star::lang::XUnoTunnel
,   ::com::sun::star::lang::XServiceInfo
,   ::com::sun::star::beans::XPropertySet
,   ::com::sun::star::beans::XPropertyState
,   ::com::sun::star::beans::XMultiPropertySet
,   ::com::sun::star::beans::XTolerantMultiPropertySet
,   ::com::sun::star::container::XEnumerationAccess
,   ::com::sun::star::container::XContentEnumerationAccess
,   ::com::sun::star::text::XTextContent
,   ::com::sun::star::text::XTextRange
> SwXParagraph_Base;

class SwXParagraph
    : public SwXParagraph_Base
{

private:

    class Impl;
    ::sw::UnoImplPtr<Impl> m_pImpl;

    virtual ~SwXParagraph();

    SwXParagraph(::com::sun::star::uno::Reference<
                    ::com::sun::star::text::XText > const & xParent,
            SwTxtNode & rTxtNode,
            const sal_Int32 nSelStart = -1, const sal_Int32 nSelEnd = - 1);

public:

    /// descriptor
    SwXParagraph();

    static ::com::sun::star::uno::Reference<
            ::com::sun::star::text::XTextContent>
        CreateXParagraph(SwDoc & rDoc, SwTxtNode& rTxtNode,
            ::com::sun::star::uno::Reference< ::com::sun::star::text::XText>
                const& xParentText = 0,
            const sal_Int32 nSelStart = -1, const sal_Int32 nSelEnd = - 1);

    const SwTxtNode * GetTxtNode() const;
    bool            IsDescriptor() const;
    /// make rPaM select the paragraph
    bool SelectPaM(SwPaM & rPaM);
    /// for SwXText
    void attachToText(SwXText & rParent, SwTxtNode & rTxtNode);

    // MetadatableMixin
    virtual ::sfx2::Metadatable* GetCoreObject();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >
        GetModel();

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

    // XMultiPropertySet
    virtual void SAL_CALL setPropertyValues(
            const ::com::sun::star::uno::Sequence< ::rtl::OUString >&
                rPropertyNames,
            const ::com::sun::star::uno::Sequence<
                ::com::sun::star::uno::Any >& rValues);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >
        SAL_CALL getPropertyValues(
            const ::com::sun::star::uno::Sequence< ::rtl::OUString >&
                rPropertyNames);
    virtual void SAL_CALL addPropertiesChangeListener(
            const ::com::sun::star::uno::Sequence< ::rtl::OUString >&
                rPropertyNames,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XPropertiesChangeListener >& xListener);
    virtual void SAL_CALL removePropertiesChangeListener(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XPropertiesChangeListener >& xListener);
    virtual void SAL_CALL firePropertiesChangeEvent(
            const ::com::sun::star::uno::Sequence< ::rtl::OUString >&
                rPropertyNames,
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::beans::XPropertiesChangeListener >& xListener);

    // XTolerantMultiPropertySet
    virtual ::com::sun::star::uno::Sequence<
                ::com::sun::star::beans::SetPropertyTolerantFailed > SAL_CALL
        setPropertyValuesTolerant(
            const ::com::sun::star::uno::Sequence< ::rtl::OUString >&
                rPropertyNames,
            const ::com::sun::star::uno::Sequence<
                ::com::sun::star::uno::Any >& rValues);
    virtual ::com::sun::star::uno::Sequence<
                ::com::sun::star::beans::GetPropertyTolerantResult > SAL_CALL
        getPropertyValuesTolerant(
            const ::com::sun::star::uno::Sequence< ::rtl::OUString >&
                rPropertyNames);
    virtual ::com::sun::star::uno::Sequence<
            ::com::sun::star::beans::GetDirectPropertyTolerantResult > SAL_CALL
        getDirectPropertyValuesTolerant(
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
    virtual ::rtl::OUString SAL_CALL getString();
    virtual void SAL_CALL setString(const ::rtl::OUString& rString);

};


/*-----------------07.04.98 08:10-------------------

--------------------------------------------------*/
class SwXParagraphEnumeration
    : public SwSimpleEnumeration_Base
{

private:

    class Impl;
    ::sw::UnoImplPtr<Impl> m_pImpl;

    virtual ~SwXParagraphEnumeration();

public:

    /// takes ownership of cursor
    SwXParagraphEnumeration(
            ::com::sun::star::uno::Reference< ::com::sun::star::text::XText >
                const & xParent,
            ::std::auto_ptr<SwUnoCrsr> pCursor,
            const CursorType eType,
            SwStartNode const*const pStartNode = 0,
            SwTable const*const pTable = 0);

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService(
            const ::rtl::OUString& rServiceName);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
        getSupportedServiceNames();

    // XEnumeration
    virtual sal_Bool SAL_CALL hasMoreElements();
    virtual ::com::sun::star::uno::Any SAL_CALL nextElement();

};

#endif // SW_UNOPARAGRAPH_HXX
