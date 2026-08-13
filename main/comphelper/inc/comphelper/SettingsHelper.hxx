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



#ifndef _COMPHELPER_SETTINGSHELPER_HXX_
#define _COMPHELPER_SETTINGSHELPER_HXX_
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase3.hxx>
namespace vos
{
	class IMutex;
}

namespace comphelper
{
	class MasterPropertySet;
	class MasterPropertySetInfo;
	class ChainablePropertySet;
	class ChainablePropertySetInfo;

	typedef  cppu::WeakImplHelper3
	<
		::com::sun::star::beans::XPropertySet,
		::com::sun::star::beans::XMultiPropertySet,
		::com::sun::star::lang::XServiceInfo
	>
	HelperBaseNoState;
	template < class ComphelperBase, class ComphelperBaseInfo > class SettingsHelperNoState :
		public HelperBaseNoState,
		public ComphelperBase
	{
	public:
		SettingsHelperNoState ( ComphelperBaseInfo *pInfo, ::vos::IMutex *pMutex = NULL)
		: ComphelperBase ( pInfo, pMutex )
		{}
		virtual ~SettingsHelperNoState () throw( ) {}
		com::sun::star::uno::Any SAL_CALL queryInterface( const com::sun::star::uno::Type& aType )
		{ return HelperBaseNoState::queryInterface( aType ); }
		void SAL_CALL acquire(  ) throw ()
		{ HelperBaseNoState::acquire( ); }
		void SAL_CALL release(  ) throw ()
		{ HelperBaseNoState::release( ); }

		// XPropertySet
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  )
		{ return ComphelperBase::getPropertySetInfo(); }
		virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue )
		{ ComphelperBase::setPropertyValue ( aPropertyName, aValue ); }
		virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName )
		{ return ComphelperBase::getPropertyValue ( PropertyName ); }
		virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener )
		{ ComphelperBase::addPropertyChangeListener ( aPropertyName, xListener ); }
		virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener )
		{ ComphelperBase::removePropertyChangeListener ( aPropertyName, aListener ); }
		virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener )
		{ ComphelperBase::addVetoableChangeListener ( PropertyName, aListener ); }
		virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener )
		{ ComphelperBase::removeVetoableChangeListener ( PropertyName, aListener ); }

		// XMultiPropertySet
		virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues )
		{ ComphelperBase::setPropertyValues ( aPropertyNames, aValues ); }
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames )
		{ return ComphelperBase::getPropertyValues ( aPropertyNames ); }
		virtual void SAL_CALL addPropertiesChangeListener( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener )
		{ ComphelperBase::addPropertiesChangeListener ( aPropertyNames, xListener ); }
		virtual void SAL_CALL removePropertiesChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener )
		{ ComphelperBase::removePropertiesChangeListener ( xListener ); }
		virtual void SAL_CALL firePropertiesChangeEvent( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener )
		{ ComphelperBase::firePropertiesChangeEvent ( aPropertyNames, xListener ); }
	};
	typedef comphelper::SettingsHelperNoState
	<
		::comphelper::MasterPropertySet,
		::comphelper::MasterPropertySetInfo
	>
	MasterHelperNoState;
	typedef comphelper::SettingsHelperNoState
	<
		::comphelper::ChainablePropertySet,
		::comphelper::ChainablePropertySetInfo
	>
	ChainableHelperNoState;
}

#endif
