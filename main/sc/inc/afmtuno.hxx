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



#ifndef SC_AFMTUNO_HXX
#define SC_AFMTUNO_HXX

#include <svl/lstner.hxx>
#include <svl/itemprop.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/container/XContainer.hpp>
#include <com/sun/star/container/XIndexContainer.hpp>
#include <com/sun/star/container/XNameReplace.hpp>
#include <com/sun/star/container/XContainerListener.hpp>
#include <com/sun/star/container/XSet.hpp>
#include <com/sun/star/container/ContainerEvent.hpp>
#include <com/sun/star/container/XIndexReplace.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase4.hxx>
#include <cppuhelper/implbase6.hxx>

class ScAutoFormatFieldObj;
class ScAutoFormatObj;


#define SC_AFMTOBJ_INVALID	USHRT_MAX


::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
	ScAutoFormatsObj_CreateInstance(
		const ::com::sun::star::uno::Reference<
			::com::sun::star::lang::XMultiServiceFactory >& );


class ScAutoFormatsObj : public ::cppu::WeakImplHelper4<
							::com::sun::star::container::XNameContainer,
							::com::sun::star::container::XEnumerationAccess,
							::com::sun::star::container::XIndexAccess,
							::com::sun::star::lang::XServiceInfo >
{
private:
	ScAutoFormatObj*		GetObjectByIndex_Impl(sal_uInt16 nIndex);
	ScAutoFormatObj*		GetObjectByName_Impl(const ::rtl::OUString& aName);

public:
							ScAutoFormatsObj();
	virtual					~ScAutoFormatsObj();

	static ::rtl::OUString	getImplementationName_Static();
	static ::com::sun::star::uno::Sequence< ::rtl::OUString> getSupportedServiceNames_Static();

							// XNameContainer
	virtual void SAL_CALL	insertByName( const ::rtl::OUString& aName,
								const ::com::sun::star::uno::Any& aElement );
	virtual void SAL_CALL	removeByName( const ::rtl::OUString& Name );

							// XNameReplace
	virtual void SAL_CALL	replaceByName( const ::rtl::OUString& aName,
								const ::com::sun::star::uno::Any& aElement );

							// XNameAccess
	virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
	virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

							// XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
							createEnumeration();

							// XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScAutoFormatObj : public ::cppu::WeakImplHelper6<
							::com::sun::star::container::XIndexAccess,
							::com::sun::star::container::XEnumerationAccess,
							::com::sun::star::container::XNamed,
							::com::sun::star::beans::XPropertySet,
							::com::sun::star::lang::XUnoTunnel,
							::com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	SfxItemPropertySet		aPropSet;
	sal_uInt16					nFormatIndex;

	ScAutoFormatFieldObj*	GetObjectByIndex_Impl(sal_uInt16 nIndex);

public:
							ScAutoFormatObj(sal_uInt16 nIndex);
	virtual					~ScAutoFormatObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// per getImplementation gerufen:
	sal_Bool				IsInserted() const		{ return nFormatIndex != SC_AFMTOBJ_INVALID; }
	void					InitFormat( sal_uInt16 nNewIndex );

							// XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

							// XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
							createEnumeration();

							// XNamed
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();
	virtual void SAL_CALL	setPropertyValue( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Any& aValue );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
									const ::rtl::OUString& PropertyName );
	virtual void SAL_CALL	addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& xListener );
	virtual void SAL_CALL	removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& aListener );
	virtual void SAL_CALL	addVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );
	virtual void SAL_CALL	removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

							// XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence<
									sal_Int8 >& aIdentifier );

	static const ::com::sun::star::uno::Sequence<sal_Int8>& getUnoTunnelId();
	static ScAutoFormatObj* getImplementation( const ::com::sun::star::uno::Reference<
									::com::sun::star::uno::XInterface> xObj );
};


class ScAutoFormatFieldObj : public ::cppu::WeakImplHelper2<
								::com::sun::star::beans::XPropertySet,
								::com::sun::star::lang::XServiceInfo >,
							public SfxListener
{
private:
	SfxItemPropertySet		aPropSet;
	sal_uInt16					nFormatIndex;
	sal_uInt16					nFieldIndex;

public:
							ScAutoFormatFieldObj(sal_uInt16 nFormat, sal_uInt16 nField);
	virtual					~ScAutoFormatFieldObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();
	virtual void SAL_CALL	setPropertyValue( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Any& aValue );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
									const ::rtl::OUString& PropertyName );
	virtual void SAL_CALL	addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& xListener );
	virtual void SAL_CALL	removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& aListener );
	virtual void SAL_CALL	addVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );
	virtual void SAL_CALL	removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


#endif
