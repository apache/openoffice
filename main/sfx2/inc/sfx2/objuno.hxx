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


#ifndef _SFX_OBJUNO_HXX
#define _SFX_OBJUNO_HXX

#include <com/sun/star/document/XDocumentInfo.hpp>
#include <com/sun/star/document/XStandaloneDocumentInfo.hpp>
#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>
#include <com/sun/star/util/XModifyListener.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/XFastPropertySet.hpp>
#include <com/sun/star/beans/XVetoableChangeListener.hpp>
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#include <com/sun/star/beans/XPropertyContainer.hpp>
#include <com/sun/star/beans/XPropertyAccess.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/embed/XStorage.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/util/XCloneable.hpp>

#include <com/sun/star/io/IOException.hpp>

#include <svl/itemprop.hxx>
#include <cppuhelper/implbase10.hxx>

#include "sfxuno.hxx"


// this is now just a wrapper around a XDocumentProperties instance

class SAL_DLLPRIVATE SfxDocumentInfoObject: public ::cppu::WeakImplHelper10<
        ::com::sun::star::document::XDocumentInfo,
		::com::sun::star::lang::XComponent,
		::com::sun::star::beans::XPropertySet,
		::com::sun::star::beans::XFastPropertySet,
		::com::sun::star::beans::XPropertyAccess,
		::com::sun::star::beans::XPropertyContainer,
        ::com::sun::star::document::XDocumentPropertiesSupplier,
        ::com::sun::star::util::XModifyBroadcaster,
		::com::sun::star::lang::XInitialization,
		::com::sun::star::util::XCloneable>
{
protected:
    struct SfxDocumentInfoObject_Impl* _pImp;

public:
    SfxDocumentInfoObject();
	~SfxDocumentInfoObject();

	// XComponent
	virtual void SAL_CALL dispose();
	virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener);
	virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener);

	// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo();
	virtual void SAL_CALL setPropertyValue(const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue);
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(const ::rtl::OUString& aPropertyName);
	virtual void SAL_CALL addPropertyChangeListener(const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener > & aListener);
	virtual void SAL_CALL removePropertyChangeListener(const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener > & aListener);
	virtual void SAL_CALL addVetoableChangeListener(const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener > & aListener);
	virtual void SAL_CALL removeVetoableChangeListener(const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener > & aListener);

	// ::com::sun::star::beans::XFastPropertySet
	virtual void SAL_CALL setFastPropertyValue(sal_Int32 nHandle, const ::com::sun::star::uno::Any& aValue);
	virtual ::com::sun::star::uno::Any SAL_CALL getFastPropertyValue(sal_Int32 nHandle);

    // ::com::sun::star::beans::XPropertyAccess
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getPropertyValues();
    virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aProps );

    // ::com::sun::star::beans::XPropertyContainer
    virtual void SAL_CALL addProperty( const ::rtl::OUString& Name, ::sal_Int16 Attributes, const ::com::sun::star::uno::Any& DefaultValue );
    virtual void SAL_CALL removeProperty( const ::rtl::OUString& Name );

	// ::com::sun::star::document::XDocumentInfo
	virtual sal_Int16 SAL_CALL getUserFieldCount();
	virtual ::rtl::OUString SAL_CALL getUserFieldName(sal_Int16 nIndex);
	virtual ::rtl::OUString SAL_CALL getUserFieldValue(sal_Int16 nIndex);
	virtual void SAL_CALL setUserFieldName(sal_Int16 nIndex, const ::rtl::OUString& aName );
	virtual void SAL_CALL setUserFieldValue(sal_Int16 nIndex, const ::rtl::OUString& aValue );

    // ::com::sun::star::document::XDocumentPropertiesSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::document::XDocumentProperties >
        SAL_CALL getDocumentProperties();

    // ::com::sun::star::util::XModifiable
	virtual sal_Bool SAL_CALL isModified();
    virtual void SAL_CALL setModified( sal_Bool bModified );

    // ::com::sun::star::util::XModifyBroadcaster
    virtual void SAL_CALL addModifyListener( const com::sun::star::uno::Reference< com::sun::star::util::XModifyListener >& xListener ) ;
    virtual void SAL_CALL removeModifyListener( const com::sun::star::uno::Reference< com::sun::star::util::XModifyListener > & xListener) ;

    // ::com::sun::star::lang::XInitialization:
    virtual void SAL_CALL initialize(
        const com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > & aArguments);

    // ::com::sun::star::util::XCloneable:
    virtual com::sun::star::uno::Reference<com::sun::star::util::XCloneable> SAL_CALL createClone();

    const SfxDocumentInfoObject& operator=( const SfxDocumentInfoObject & rOther);
};

class SfxStandaloneDocumentInfoObject: public SfxDocumentInfoObject,
                                       public ::com::sun::star::lang::XServiceInfo,
									   public ::com::sun::star::document::XStandaloneDocumentInfo
{
    ::com::sun::star::uno::Reference < ::com::sun::star::lang::XMultiServiceFactory > _xFactory;

public:
	SfxStandaloneDocumentInfoObject( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xFactory );
	virtual ~SfxStandaloneDocumentInfoObject();

	void Clear();

	// XInterface, XTypeProvider, XServiceInfo
	SFX_DECL_XINTERFACE_XTYPEPROVIDER_XSERVICEINFO

	// ::com::sun::star::document::XDocumentInfo
	virtual sal_Int16 SAL_CALL getUserFieldCount();
	virtual ::rtl::OUString SAL_CALL getUserFieldName(sal_Int16 nIndex);
	virtual ::rtl::OUString SAL_CALL getUserFieldValue(sal_Int16 nIndex);
	virtual void SAL_CALL setUserFieldName(sal_Int16 nIndex, const ::rtl::OUString& aName );
	virtual void SAL_CALL setUserFieldValue(sal_Int16 nIndex, const ::rtl::OUString& aValue );

	// ::com::sun::star::document::XStandaloneDocumentInfo
    virtual void SAL_CALL loadFromURL(const ::rtl::OUString& aURL);
    virtual void SAL_CALL storeIntoURL(const ::rtl::OUString& aURL);
};

#endif
