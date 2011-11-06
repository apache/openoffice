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


#ifndef _CHXCHARTDRAWPAGE_HXX_
#define _CHXCHARTDRAWPAGE_HXX_

#ifndef _SVX_UNOWPAGE_HXX
#include <bf_svx/unopage.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

// header for SvxItemPropertySet
#ifndef SVX_UNOPROV_HXX
#include <bf_svx/unoprov.hxx>
#endif
namespace binfilter {

class ChartModel;

class ChXChartDrawPage :
	public ::com::sun::star::beans::XPropertySet,
	public SvxDrawPage
{
private:
	ChartModel*			mpModel;
	SvxItemPropertySet	maPropSet;

	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > maTypeSequence;

public:
	ChXChartDrawPage( ChartModel* pModel );
	virtual ~ChXChartDrawPage() throw();

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType )
		throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL acquire() throw();
    virtual void SAL_CALL release() throw();

	// XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes()
		throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId()
		throw( ::com::sun::star::uno::RuntimeException );

	// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo()
		throw( ::com::sun::star::uno::RuntimeException );
	virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName,
											const ::com::sun::star::uno::Any& aValue )
		throw( ::com::sun::star::beans::UnknownPropertyException,
			   ::com::sun::star::beans::PropertyVetoException,
			   ::com::sun::star::lang::IllegalArgumentException,
			   ::com::sun::star::lang::WrappedTargetException,
			   ::com::sun::star::uno::RuntimeException );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName )
		throw( ::com::sun::star::beans::UnknownPropertyException,
			   ::com::sun::star::lang::WrappedTargetException,
			   ::com::sun::star::uno::RuntimeException );
	virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
				const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener )
		throw( ::com::sun::star::beans::UnknownPropertyException,
			   ::com::sun::star::lang::WrappedTargetException,
			   ::com::sun::star::uno::RuntimeException );
	virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
				const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener )
		throw( ::com::sun::star::beans::UnknownPropertyException,
			   ::com::sun::star::lang::WrappedTargetException,
			   ::com::sun::star::uno::RuntimeException );
	virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName,
				const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener )
		throw( ::com::sun::star::beans::UnknownPropertyException,
			   ::com::sun::star::lang::WrappedTargetException,
			   ::com::sun::star::uno::RuntimeException );
	virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
				const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener )
		throw( ::com::sun::star::beans::UnknownPropertyException,
			   ::com::sun::star::lang::WrappedTargetException,
			   ::com::sun::star::uno::RuntimeException );
};

} //namespace binfilter
#endif	// _CHXCHARTDRAWPAGE_HXX_
