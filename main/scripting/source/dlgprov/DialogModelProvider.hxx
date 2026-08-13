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


#include "sal/config.h"
#include "cppuhelper/factory.hxx"
#include "cppuhelper/implbase4.hxx"
#include "com/sun/star/lang/XInitialization.hpp"
#include "com/sun/star/container/XNameContainer.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/beans/XPropertySet.hpp"

/// anonymous implementation namespace
namespace dlgprov{

namespace css = ::com::sun::star;

class DialogModelProvider:
    public ::cppu::WeakImplHelper4<
        css::lang::XInitialization,
        css::container::XNameContainer,
        css::beans::XPropertySet,
        css::lang::XServiceInfo>
{
public:
    explicit DialogModelProvider(css::uno::Reference< css::uno::XComponentContext > const & context);
private:
    // ::com::sun::star::lang::XInitialization:
    virtual void SAL_CALL initialize(const css::uno::Sequence< ::com::sun::star::uno::Any > & aArguments);

    // ::com::sun::star::container::XElementAccess:
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
    virtual ::sal_Bool SAL_CALL hasElements();

    // ::com::sun::star::container::XNameAccess:
    virtual ::com::sun::star::uno::Any SAL_CALL getByName(const ::rtl::OUString & aName);
    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
    virtual ::sal_Bool SAL_CALL hasByName(const ::rtl::OUString & aName);

    // ::com::sun::star::container::XNameReplace:
    virtual void SAL_CALL replaceByName(const ::rtl::OUString & aName, const ::com::sun::star::uno::Any & aElement);

    // ::com::sun::star::container::XNameContainer:
    virtual void SAL_CALL insertByName(const ::rtl::OUString & aName, const ::com::sun::star::uno::Any & aElement);
    virtual void SAL_CALL removeByName(const ::rtl::OUString & Name);

    // ::com::sun::star::lang::XServiceInfo:
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual ::sal_Bool SAL_CALL supportsService(const ::rtl::OUString & ServiceName);
    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

private:
    DialogModelProvider(const DialogModelProvider &); // not defined
    DialogModelProvider& operator=(const DialogModelProvider &); // not defined

    // destructor is private and will be called indirectly by the release call    virtual ~DialogModelProvider() {}

    css::uno::Reference< css::uno::XComponentContext >      m_xContext;
    css::uno::Reference< css::container::XNameContainer>    m_xDialogModel;
    css::uno::Reference< css::beans::XPropertySet>          m_xDialogModelProp;
};
} // closing anonymous implementation namespace
