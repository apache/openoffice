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



#ifndef _UTL_PROPERTYSETHELPER_HXX_
#define _UTL_PROPERTYSETHELPER_HXX_

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>

//=========================================================================
//= property helper classes
//=========================================================================

//... namespace utl .......................................................
namespace utl
{
//.........................................................................

class PropertySetInfo;
struct PropertyMapEntry;
class PropertySetHelperImpl;

class PropertySetHelper : public ::com::sun::star::beans::XPropertySet,
						  public ::com::sun::star::beans::XPropertyState,
						  public ::com::sun::star::beans::XMultiPropertySet
{
private:
	PropertySetHelperImpl* mp;

protected:
	virtual void _setPropertyValues( const utl::PropertyMapEntry** ppEntries, const ::com::sun::star::uno::Any* pValues ) = 0;
	virtual void _getPropertyValues( const utl::PropertyMapEntry** ppEntries, ::com::sun::star::uno::Any* pValue ) = 0;

	virtual void _getPropertyStates( const utl::PropertyMapEntry** ppEntries, ::com::sun::star::beans::PropertyState* pStates );
	virtual void _setPropertyToDefault( const utl::PropertyMapEntry* pEntry );
	virtual ::com::sun::star::uno::Any _getPropertyDefault( const utl::PropertyMapEntry* pEntry );

public:
	PropertySetHelper( utl::PropertySetInfo* pInfo ) throw();
	virtual ~PropertySetHelper() throw();

	// XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	// XMultiPropertySet
//  virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );
    virtual void SAL_CALL addPropertiesChangeListener( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
    virtual void SAL_CALL removePropertiesChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
    virtual void SAL_CALL firePropertiesChangeEvent( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );

    // XPropertyState
    virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL getPropertyStates( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyName );
    virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName );
};

//.........................................................................
}
//... namespace utl .......................................................

#endif // _UTL_PROPERTYSETHELPER_HXX_
