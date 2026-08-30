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


#ifndef SC_VBA_COMMANDBAR_HXX
#define SC_VBA_COMMANDBAR_HXX

#include <ooo/vba/XCommandBar.hpp>
#include <com/sun/star/ui/XUIConfigurationManager.hpp>
#include <com/sun/star/ui/XUIConfigurationPersistence.hpp>
#include <com/sun/star/container/XIndexContainer.hpp>
#include <com/sun/star/beans/PropertyValues.hpp>

#include <vbahelper/vbahelperinterface.hxx>
#include "vbacommandbarhelper.hxx"

#include <map>

typedef InheritedHelperInterfaceImpl1< ov::XCommandBar > CommandBar_BASE;

class ScVbaCommandBar : public CommandBar_BASE
{
private:
    VbaCommandBarHelperRef pCBarHelper;
    css::uno::Reference< css::container::XIndexAccess > m_xBarSettings;
    rtl::OUString   m_sResourceUrl;
    sal_Bool        m_bIsMenu;
    sal_Bool        m_bTemporary;

public:
    ScVbaCommandBar( const css::uno::Reference< ov::XHelperInterface > xParent, const css::uno::Reference< css::uno::XComponentContext > xContext, VbaCommandBarHelperRef pHelper, const css::uno::Reference< css::container::XIndexAccess >& xBarSettings, const rtl::OUString& sResourceUrl, sal_Bool bIsMenu, sal_Bool bTemporary = sal_True );

    sal_Bool IsMenu() { return m_bIsMenu; }

    // Attributes
    virtual ::rtl::OUString SAL_CALL getName();
    virtual void SAL_CALL setName( const ::rtl::OUString& _name );
    virtual ::sal_Bool SAL_CALL getVisible();
    virtual void SAL_CALL setVisible( ::sal_Bool _visible );
    virtual ::sal_Bool SAL_CALL getEnabled();
    virtual void SAL_CALL setEnabled( ::sal_Bool _enabled );

    // Methods
    virtual void SAL_CALL Delete(  );
    virtual css::uno::Any SAL_CALL Controls( const css::uno::Any& aIndex );
    virtual sal_Int32 SAL_CALL Type(  );
    virtual css::uno::Any SAL_CALL FindControl( const css::uno::Any& aType, const css::uno::Any& aId, const css::uno::Any& aTag, const css::uno::Any& aVisible, const css::uno::Any& aRecursive );

	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

/** Dummy command bar implementation. Does nothing but provide its name. */
class VbaDummyCommandBar : public CommandBar_BASE
{
public:
    VbaDummyCommandBar(
            const css::uno::Reference< ov::XHelperInterface > xParent,
            const css::uno::Reference< css::uno::XComponentContext > xContext,
            const ::rtl::OUString& rName,
            sal_Int32 nType );

    // Attributes
    virtual ::rtl::OUString SAL_CALL getName();
    virtual void SAL_CALL setName( const ::rtl::OUString& _name );
    virtual ::sal_Bool SAL_CALL getVisible();
    virtual void SAL_CALL setVisible( ::sal_Bool _visible );
    virtual ::sal_Bool SAL_CALL getEnabled();
    virtual void SAL_CALL setEnabled( ::sal_Bool _enabled );

    // Methods
    virtual void SAL_CALL Delete(  );
    virtual css::uno::Any SAL_CALL Controls( const css::uno::Any& aIndex );
    virtual sal_Int32 SAL_CALL Type(  );
    virtual css::uno::Any SAL_CALL FindControl( const css::uno::Any& aType, const css::uno::Any& aId, const css::uno::Any& aTag, const css::uno::Any& aVisible, const css::uno::Any& aRecursive );

	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();

private:
    ::rtl::OUString maName;
    sal_Int32 mnType;
};

#endif//SC_VBA_COMMANDBAR_HXX
