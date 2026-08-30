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



#ifndef SW_UNOSECTION_HXX
#define SW_UNOSECTION_HXX

#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/text/XTextContent.hpp>
#include <com/sun/star/text/XTextSection.hpp>

#include <cppuhelper/implbase7.hxx>

#include <sfx2/Metadatable.hxx>

#include <unobaseclass.hxx>


class SwSectionFmt;


/*-----------------09.03.98 13:57-------------------

--------------------------------------------------*/


struct SwTextSectionProperties_Impl;

typedef ::cppu::ImplInheritanceHelper7
<   ::sfx2::MetadatableMixin
,   ::com::sun::star::lang::XUnoTunnel
,   ::com::sun::star::lang::XServiceInfo
,   ::com::sun::star::beans::XPropertySet
,   ::com::sun::star::beans::XPropertyState
,   ::com::sun::star::beans::XMultiPropertySet
,   ::com::sun::star::container::XNamed
,   ::com::sun::star::text::XTextSection
> SwXTextSection_Base;

class SwXTextSection
    : public SwXTextSection_Base
{

private:

    class Impl;
    ::sw::UnoImplPtr<Impl> m_pImpl;

    SwXTextSection(SwSectionFmt *const pFmt, const bool bIndexHeader = false);

    virtual ~SwXTextSection();

public:

    SwSectionFmt*   GetFmt() const;

    static ::com::sun::star::uno::Reference<
            ::com::sun::star::text::XTextSection >
        CreateXTextSection(SwSectionFmt *const pFmt = 0,
                const bool bIndexHeader = false);

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

    // XNamed
    virtual ::rtl::OUString SAL_CALL getName();
    virtual void SAL_CALL setName(const ::rtl::OUString& rName);

    // XTextContent
    virtual void SAL_CALL attach(
            const ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > & xTextRange);
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextRange > SAL_CALL getAnchor();

    // XTextSection
    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextSection > SAL_CALL
        getParentSection();
    virtual ::com::sun::star::uno::Sequence<
                ::com::sun::star::uno::Reference<
                    ::com::sun::star::text::XTextSection >  > SAL_CALL
        getChildSections();

};

#endif // SW_UNOSECTION_HXX
