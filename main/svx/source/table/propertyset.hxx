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



#ifndef _COMPHELPER_PROPERTYSET_HXX_
#define _COMPHELPER_PROPERTYSET_HXX_

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XFastPropertySet.hpp>
#include <rtl/ref.hxx>
#include <functional>
#include <hash_map>
#include <vector>

#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase3.hxx>

namespace comphelper {

// -----------------------------------------------------------------------------

typedef std::vector< ::com::sun::star::beans::Property > PropertyVector;
typedef std::hash_map< ::rtl::OUString, ::sal_uInt32, ::rtl::OUStringHash, ::std::equal_to< ::rtl::OUString > > PropertyMap;

// -----------------------------------------------------------------------------

class FastPropertySetInfo : public ::cppu::WeakAggImplHelper1< ::com::sun::star::beans::XPropertySetInfo >
{
public:
	FastPropertySetInfo();
	FastPropertySetInfo( const PropertyVector& rProps );
	virtual ~FastPropertySetInfo();

	void addProperty( const ::com::sun::star::beans::Property& rProperty );
	void addProperties( const PropertyVector& rProps );

	const ::com::sun::star::beans::Property& getProperty( const ::rtl::OUString& aName );
	const ::com::sun::star::beans::Property* hasProperty( const ::rtl::OUString& aName );

    // XPropertySetInfo
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property > SAL_CALL getProperties(  );
    virtual ::com::sun::star::beans::Property SAL_CALL getPropertyByName( const ::rtl::OUString& aName );
    virtual ::sal_Bool SAL_CALL hasPropertyByName( const ::rtl::OUString& Name );

private:
	PropertyVector maProperties;
	PropertyMap maMap;
};

// -----------------------------------------------------------------------------

class FastPropertySet : public ::cppu::WeakAggImplHelper3<  ::com::sun::star::beans::XPropertySet, ::com::sun::star::beans::XMultiPropertySet, ::com::sun::star::beans::XFastPropertySet >
{
public:
	FastPropertySet( const rtl::Reference< FastPropertySetInfo >& xInfo );
	virtual ~FastPropertySet();

	// XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

    // XMultiPropertySet
//    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );
    virtual void SAL_CALL addPropertiesChangeListener( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
    virtual void SAL_CALL removePropertiesChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
    virtual void SAL_CALL firePropertiesChangeEvent( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );

    // XFastPropertySet
    virtual void SAL_CALL setFastPropertyValue( ::sal_Int32 nHandle, const ::com::sun::star::uno::Any& aValue ) = 0;
    virtual ::com::sun::star::uno::Any SAL_CALL getFastPropertyValue( ::sal_Int32 nHandle ) = 0;

private:
	rtl::Reference< FastPropertySetInfo > mxInfo;
};

}

#endif
