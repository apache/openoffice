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



#ifndef _UCBSTORE_HXX
#define _UCBSTORE_HXX

#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/ucb/XPropertySetRegistryFactory.hpp>
#include <com/sun/star/ucb/XPropertySetRegistry.hpp>
#include <com/sun/star/ucb/XPersistentPropertySet.hpp>
#include <com/sun/star/beans/XPropertyContainer.hpp>
#include <com/sun/star/beans/XPropertySetInfoChangeNotifier.hpp>
#include <com/sun/star/beans/XPropertyAccess.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <cppuhelper/weak.hxx>
#include <ucbhelper/macros.hxx>

//=========================================================================

#define STORE_SERVICE_NAME 			"com.sun.star.ucb.Store"
#define PROPSET_REG_SERVICE_NAME	"com.sun.star.ucb.PropertySetRegistry"
#define PERS_PROPSET_SERVICE_NAME	"com.sun.star.ucb.PersistentPropertySet"

//=========================================================================

struct UcbStore_Impl;

class UcbStore :
				public cppu::OWeakObject,
				public com::sun::star::lang::XTypeProvider,
				public com::sun::star::lang::XServiceInfo,
				public com::sun::star::ucb::XPropertySetRegistryFactory,
				public com::sun::star::lang::XInitialization
{
	com::sun::star::uno::Reference<
				com::sun::star::lang::XMultiServiceFactory > m_xSMgr;
	UcbStore_Impl* m_pImpl;

public:
	UcbStore(
		const com::sun::star::uno::Reference<
				com::sun::star::lang::XMultiServiceFactory >& rXSMgr );
	virtual ~UcbStore();

	// XInterface
	XINTERFACE_DECL()

	// XTypeProvider
	XTYPEPROVIDER_DECL()

    // XServiceInfo
	XSERVICEINFO_DECL()

	// XPropertySetRegistryFactory
    virtual com::sun::star::uno::Reference<
				com::sun::star::ucb::XPropertySetRegistry > SAL_CALL
	createPropertySetRegistry( const rtl::OUString& URL );

	// XInitialization
    virtual void SAL_CALL
	initialize( const ::com::sun::star::uno::Sequence<
						::com::sun::star::uno::Any >& aArguments );

	const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >&
	getInitArgs() const;
};

//=========================================================================

struct PropertySetRegistry_Impl;
class PersistentPropertySet;

class PropertySetRegistry :
				public cppu::OWeakObject,
				public com::sun::star::lang::XTypeProvider,
				public com::sun::star::lang::XServiceInfo,
				public com::sun::star::ucb::XPropertySetRegistry,
				public com::sun::star::container::XNameAccess
{
	friend class PersistentPropertySet;

	com::sun::star::uno::Reference<
				com::sun::star::lang::XMultiServiceFactory > m_xSMgr;
	PropertySetRegistry_Impl* m_pImpl;

private:
	com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >
	getConfigProvider();

	void add   ( PersistentPropertySet* pSet );
	void remove( PersistentPropertySet* pSet );

	void renamePropertySet( const rtl::OUString& rOldKey,
							const rtl::OUString& rNewKey );

public:
	PropertySetRegistry(
		const com::sun::star::uno::Reference<
				com::sun::star::lang::XMultiServiceFactory >& rXSMgr,
		const ::com::sun::star::uno::Sequence<
                                                ::com::sun::star::uno::Any >& rInitArgs);
	virtual ~PropertySetRegistry();

	// XInterface
	XINTERFACE_DECL()

	// XTypeProvider
	XTYPEPROVIDER_DECL()

    // XServiceInfo
	XSERVICEINFO_NOFACTORY_DECL()

	// XPropertySetRegistry
    virtual com::sun::star::uno::Reference<
				com::sun::star::ucb::XPersistentPropertySet > SAL_CALL
	openPropertySet( const rtl::OUString& key, sal_Bool create );
    virtual void SAL_CALL
	removePropertySet( const rtl::OUString& key );

	// XElementAccess ( XNameAccess is derived from it )
    virtual com::sun::star::uno::Type SAL_CALL
	getElementType();
    virtual sal_Bool SAL_CALL
	hasElements();

    // XNameAccess
    virtual com::sun::star::uno::Any SAL_CALL
	getByName( const rtl::OUString& aName );
    virtual com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL
	getElementNames();
    virtual sal_Bool SAL_CALL
	hasByName( const rtl::OUString& aName );

	// Non-interface methods
	com::sun::star::uno::Reference< com::sun::star::uno::XInterface >
	getRootConfigReadAccess();
	com::sun::star::uno::Reference< com::sun::star::uno::XInterface >
	getConfigWriteAccess( const rtl::OUString& rPath );
};

//=========================================================================

struct PersistentPropertySet_Impl;

class PersistentPropertySet :
				public cppu::OWeakObject,
				public com::sun::star::lang::XTypeProvider,
				public com::sun::star::lang::XServiceInfo,
				public com::sun::star::lang::XComponent,
				public com::sun::star::ucb::XPersistentPropertySet,
				public com::sun::star::container::XNamed,
				public com::sun::star::beans::XPropertyContainer,
				public com::sun::star::beans::XPropertySetInfoChangeNotifier,
				public com::sun::star::beans::XPropertyAccess
{
	com::sun::star::uno::Reference<
			com::sun::star::lang::XMultiServiceFactory > m_xSMgr;
	PersistentPropertySet_Impl* m_pImpl;

private:
	void notifyPropertyChangeEvent(
		const com::sun::star::beans::PropertyChangeEvent& rEvent ) const;
	void notifyPropertySetInfoChange(
		const com::sun::star::beans::PropertySetInfoChangeEvent& evt ) const;

public:
	PersistentPropertySet(
		const com::sun::star::uno::Reference<
				com::sun::star::lang::XMultiServiceFactory >& rXSMgr,
		PropertySetRegistry& rCreator,
		const rtl::OUString& rKey );
	virtual ~PersistentPropertySet();

	// XInterface
	XINTERFACE_DECL()

	// XTypeProvider
	XTYPEPROVIDER_DECL()

    // XServiceInfo
	XSERVICEINFO_NOFACTORY_DECL()

	// XComponent
    virtual void SAL_CALL
	dispose();
    virtual void SAL_CALL
	addEventListener( const com::sun::star::uno::Reference<
							com::sun::star::lang::XEventListener >& Listener );
    virtual void SAL_CALL
	removeEventListener( const com::sun::star::uno::Reference<
							com::sun::star::lang::XEventListener >& Listener );

	// XPropertySet
    virtual com::sun::star::uno::Reference<
				com::sun::star::beans::XPropertySetInfo > SAL_CALL
	getPropertySetInfo();
    virtual void SAL_CALL
	setPropertyValue( const rtl::OUString& aPropertyName,
					  const com::sun::star::uno::Any& aValue );
    virtual com::sun::star::uno::Any SAL_CALL
	getPropertyValue( const rtl::OUString& PropertyName );
    virtual void SAL_CALL
	addPropertyChangeListener( const rtl::OUString& aPropertyName,
							   const com::sun::star::uno::Reference<
							   	com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL
	removePropertyChangeListener( const rtl::OUString& aPropertyName,
								  const com::sun::star::uno::Reference<
								  	com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL
	addVetoableChangeListener( const rtl::OUString& PropertyName,
							   const com::sun::star::uno::Reference<
							   	com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL
	removeVetoableChangeListener( const rtl::OUString& PropertyName,
								  const com::sun::star::uno::Reference<
								  	com::sun::star::beans::XVetoableChangeListener >& aListener );

	// XPersistentPropertySet
    virtual com::sun::star::uno::Reference<
				com::sun::star::ucb::XPropertySetRegistry > SAL_CALL
	getRegistry();
    virtual rtl::OUString SAL_CALL
	getKey();

	// XNamed
    virtual rtl::OUString SAL_CALL
	getName();
    virtual void SAL_CALL
	setName( const ::rtl::OUString& aName );

	// XPropertyContainer
	virtual void SAL_CALL
	addProperty( const rtl::OUString& Name,
				 sal_Int16 Attributes,
				 const com::sun::star::uno::Any& DefaultValue );
    virtual void SAL_CALL
	removeProperty( const rtl::OUString& Name );

	// XPropertySetInfoChangeNotifier
    virtual void SAL_CALL
	addPropertySetInfoChangeListener( const com::sun::star::uno::Reference<
			com::sun::star::beans::XPropertySetInfoChangeListener >& Listener );
    virtual void SAL_CALL
	removePropertySetInfoChangeListener( const com::sun::star::uno::Reference<
			com::sun::star::beans::XPropertySetInfoChangeListener >& Listener );

	// XPropertyAccess
    virtual com::sun::star::uno::Sequence<
				com::sun::star::beans::PropertyValue > SAL_CALL
	getPropertyValues();
    virtual void SAL_CALL
	setPropertyValues( const com::sun::star::uno::Sequence<
							com::sun::star::beans::PropertyValue >& aProps );

	// Non-interface methods.
	PropertySetRegistry& getPropertySetRegistry();
	const rtl::OUString& getFullKey();
};

#endif /* !_UCBSTORE_HXX */
