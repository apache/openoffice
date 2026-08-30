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


#ifndef CHART_MINMAXLINEWRAPPER_HXX
#define CHART_MINMAXLINEWRAPPER_HXX

#include "ServiceMacros.hxx"
#include "MutexContainer.hxx"
#include "WrappedIgnoreProperty.hxx"
#include <cppuhelper/interfacecontainer.hxx>


#include <com/sun/star/chart2/XDiagram.hpp>

// header for class OPropertyArrayHelper
#include <cppuhelper/propshlp.hxx>
#include <cppuhelper/implbase6.hxx>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XMultiPropertyStates.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <boost/shared_ptr.hpp>

namespace chart
{

namespace wrapper
{

class Chart2ModelContact;

class MinMaxLineWrapper : public MutexContainer
                        , public ::cppu::WeakImplHelper6
                        < ::com::sun::star::lang::XComponent
                        , ::com::sun::star::lang::XServiceInfo
                        , ::com::sun::star::beans::XPropertySet
                        , ::com::sun::star::beans::XMultiPropertySet
                        , ::com::sun::star::beans::XPropertyState
                        , ::com::sun::star::beans::XMultiPropertyStates
                        //	, ::com::sun::star::uno::XWeak			// implemented by WeakImplHelper(optional interface)
	                    //	, ::com::sun::star::uno::XInterface		// implemented by WeakImplHelper
	                    //	, ::com::sun::star::lang::XTypeProvider	// implemented by WeakImplHelper
                        >
{
public:
    MinMaxLineWrapper( ::boost::shared_ptr< Chart2ModelContact > spChart2ModelContact );
	virtual ~MinMaxLineWrapper();

    /// XServiceInfo declarations
    APPHELPER_XSERVICEINFO_DECL()

    // ____ XComponent ____
    virtual void SAL_CALL dispose();
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference<
                                            ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference<
                                               ::com::sun::star::lang::XEventListener >& aListener );

    //XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );

    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );

    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

    //XMultiPropertySet
    //getPropertySetInfo() already declared in XPropertySet
    virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );
    virtual void SAL_CALL addPropertiesChangeListener( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
    virtual void SAL_CALL removePropertiesChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
    virtual void SAL_CALL firePropertiesChangeEvent( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );

    //XPropertyState
    virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL getPropertyStates( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyName );
    virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName );

    //XMultiPropertyStates
    //getPropertyStates() already declared in XPropertyState
    virtual void SAL_CALL setAllPropertiesToDefault(  );
    virtual void SAL_CALL setPropertiesToDefault( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyDefaults( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );

private: //methods
    ::cppu::IPropertyArrayHelper&   getInfoHelper();

private: //member
    ::boost::shared_ptr< Chart2ModelContact >   m_spChart2ModelContact;
	::cppu::OInterfaceContainerHelper           m_aEventListenerContainer;

    WrappedIgnoreProperty               m_aWrappedLineJointProperty;
};

} //  namespace wrapper
} //  namespace chart

// CHART_MINMAXLINEWRAPPER_HXX
#endif
