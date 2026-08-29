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



#ifndef _SD_STLFAMILY_HXX
#define _SD_STLFAMILY_HXX

#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/container/XNamed.hpp>

#include <rtl/ref.hxx>

#include <cppuhelper/implbase7.hxx>

#include <svl/style.hxx>

#include <editeng/unoipset.hxx>

class SdStyleSheet;
class SdPage;
struct SdStyleFamilyImpl;

namespace css = ::com::sun::star;

class SdStyleFamily : public ::cppu::WeakImplHelper7< css::container::XNameContainer, css::container::XNamed, css::container::XIndexAccess, css::lang::XSingleServiceFactory,  css::lang::XServiceInfo, css::lang::XComponent, css::beans::XPropertySet >
{
public:
	/// creates the style family for the given SfxStyleFamily
	SdStyleFamily( const rtl::Reference< SfxStyleSheetPool >& xPool, SfxStyleFamily nFamily );

	/// creates the presentation family for the given masterpage
	SdStyleFamily( const rtl::Reference< SfxStyleSheetPool >& xPool, const SdPage* pMasterPage );

	virtual ~SdStyleFamily();

	// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XNamed
    virtual ::rtl::OUString SAL_CALL getName(  );
    virtual void SAL_CALL setName( const ::rtl::OUString& aName );

	// XNameAccess
    virtual css::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
    virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

	// XElementAccess
    virtual css::uno::Type SAL_CALL getElementType();
    virtual sal_Bool SAL_CALL hasElements();

	// XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() ;
    virtual css::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

	// XNameContainer
	virtual void SAL_CALL insertByName( const ::rtl::OUString& aName, const css::uno::Any& aElement );
	virtual void SAL_CALL removeByName( const ::rtl::OUString& Name );

	// XNameReplace
    virtual void SAL_CALL replaceByName( const ::rtl::OUString& aName, const css::uno::Any& aElement );

	// XSingleServiceFactory
    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstance(  );
    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstanceWithArguments( const css::uno::Sequence< css::uno::Any >& aArguments );

	// XComponent
    virtual void SAL_CALL dispose(  );
    virtual void SAL_CALL addEventListener( const css::uno::Reference< css::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const css::uno::Reference< css::lang::XEventListener >& aListener );

    // XPropertySet
	virtual css::uno::Reference<css::beans::XPropertySetInfo> SAL_CALL getPropertySetInfo();
	virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const css::uno::Any& aValue );
	virtual css::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
	virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const css::uno::Reference<css::beans::XPropertyChangeListener>& xListener );
	virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const css::uno::Reference<css::beans::XPropertyChangeListener>& aListener );
    virtual void SAL_CALL addVetoableChangeListener(const ::rtl::OUString& PropertyName, const css::uno::Reference<css::beans::XVetoableChangeListener>& aListener );
    virtual void SAL_CALL removeVetoableChangeListener(const ::rtl::OUString& PropertyName,const css::uno::Reference<css::beans::XVetoableChangeListener>&aListener );

private:
	void throwIfDisposed() const;
	SdStyleSheet* GetValidNewSheet( const css::uno::Any& rElement );
	SdStyleSheet* GetSheetByName( const ::rtl::OUString& rName );

	SfxStyleFamily mnFamily;
	rtl::Reference< SfxStyleSheetPool > mxPool;
	SdStyleFamilyImpl*	mpImpl;
};

typedef rtl::Reference< SdStyleFamily > SdStyleFamilyRef;

#endif	   // _SD_STLFAMILY_HXX
