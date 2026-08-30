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



#ifndef EXTENSIONS_SOURCE_PROPCTRLR_PROPERTYCOMPOSER_HXX
#define EXTENSIONS_SOURCE_PROPCTRLR_PROPERTYCOMPOSER_HXX

#include "pcrcommon.hxx"
#include "formbrowsertools.hxx"
#include "composeduiupdate.hxx"
#include "formbrowsertools.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/inspection/XPropertyHandler.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
/** === end UNO includes === **/
#include <cppuhelper/compbase2.hxx>
#include <comphelper/broadcasthelper.hxx>
#include <comphelper/listenernotification.hxx>

#include <vector>
#include <set>

//........................................................................
namespace pcr
{
//........................................................................

	//====================================================================
	//= PropertyComposer
	//====================================================================
    typedef ::cppu::WeakComponentImplHelper2    <   ::com::sun::star::inspection::XPropertyHandler
                                                ,   ::com::sun::star::beans::XPropertyChangeListener
                                                >   PropertyComposer_Base;
    /** implements an <type>XPropertyHandler</type> which composes it's information
        from a set of other property handlers
    */
    class PropertyComposer  :public PropertyComposer_Base
                            ,public ::comphelper::OBaseMutex
                            ,public IPropertyExistenceCheck
	{
    public:
        typedef ::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XPropertyHandler > >
                                                            HandlerArray;

    private:
        HandlerArray                                    m_aSlaveHandlers;
        ::std::auto_ptr< ComposedPropertyUIUpdate >     m_pUIRequestComposer;
        PropertyChangeListeners                         m_aPropertyListeners;
        bool                                            m_bSupportedPropertiesAreKnown;
        PropertyBag                                     m_aSupportedProperties;

    public:
        /** constructs an <type>XPropertyHandler</type> which composes it's information from a set
            of other property handlers

            @param _rSlaveHandlers
                the set of slave handlers to invoke. Must not be <NULL/>
        */
        PropertyComposer( const ::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XPropertyHandler > >& _rSlaveHandlers );

    public:
        // XPropertyHandler overridables
        virtual void                                SAL_CALL inspect( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxIntrospectee );
        virtual ::com::sun::star::uno::Any          SAL_CALL getPropertyValue( const ::rtl::OUString& _rPropertyName );
        virtual void                                SAL_CALL setPropertyValue( const ::rtl::OUString& _rPropertyName, const ::com::sun::star::uno::Any& _rValue );
        virtual ::com::sun::star::uno::Any SAL_CALL convertToPropertyValue( const ::rtl::OUString& _rPropertyName, const ::com::sun::star::uno::Any& _rControlValue );
        virtual ::com::sun::star::uno::Any SAL_CALL convertToControlValue( const ::rtl::OUString& _rPropertyName, const ::com::sun::star::uno::Any& _rPropertyValue, const ::com::sun::star::uno::Type& _rControlValueType );
        virtual ::com::sun::star::beans::PropertyState
                                                    SAL_CALL getPropertyState( const ::rtl::OUString& _rPropertyName );
        virtual void                                SAL_CALL addPropertyChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& _rxListener );
        virtual void                                SAL_CALL removePropertyChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& _rxListener );
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >
                                                    SAL_CALL getSupportedProperties();
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >
                                                    SAL_CALL getSupersededProperties( );
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >
                                                    SAL_CALL getActuatingProperties( );
        virtual ::com::sun::star::inspection::LineDescriptor
                                                    SAL_CALL describePropertyLine( const ::rtl::OUString& _rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XPropertyControlFactory >& _rxControlFactory );
        virtual ::sal_Bool                          SAL_CALL isComposable( const ::rtl::OUString& _rPropertyName );
        virtual ::com::sun::star::inspection::InteractiveSelectionResult
                                                    SAL_CALL onInteractivePropertySelection( const ::rtl::OUString& _rPropertyName, sal_Bool _bPrimary, ::com::sun::star::uno::Any& _rData, const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XObjectInspectorUI >& _rxInspectorUI );
        virtual void                                SAL_CALL actuatingPropertyChanged( const ::rtl::OUString& _rActuatingPropertyName, const ::com::sun::star::uno::Any& _rNewValue, const ::com::sun::star::uno::Any& _rOldValue, const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XObjectInspectorUI >& _rxInspectorUI, sal_Bool _bFirstTimeInit );
	    virtual sal_Bool                            SAL_CALL suspend( sal_Bool _bSuspend );

        // XComponent
        DECLARE_XCOMPONENT()
        virtual void                                SAL_CALL disposing();

        // XPropertyChangeListener
        virtual void SAL_CALL propertyChange( const ::com::sun::star::beans::PropertyChangeEvent& evt );

        // XEventListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source );

        // IPropertyExistenceCheck
        virtual ::sal_Bool SAL_CALL hasPropertyByName( const ::rtl::OUString& _rName );

    private:
        /** ensures that m_pUIRequestComposer exists
        */
        void    impl_ensureUIRequestComposer( const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XObjectInspectorUI >& _rxInspectorUI );

        /** checks whether a given property exists in <member>m_aSupportedProperties</member>
        */
        bool    impl_isSupportedProperty_nothrow( const ::rtl::OUString& _rPropertyName )
        {
            ::com::sun::star::beans::Property aDummy; aDummy.Name = _rPropertyName;
            return m_aSupportedProperties.find( aDummy ) != m_aSupportedProperties.end();
        }

    private:
        class MethodGuard;
        friend class MethodGuard;
        class MethodGuard : public ::osl::MutexGuard
        {
        public:
            MethodGuard( PropertyComposer& _rInstance )
                : ::osl::MutexGuard( _rInstance.m_aMutex )
            {
                if ( _rInstance.m_aSlaveHandlers.empty() )
                    throw ::com::sun::star::lang::DisposedException( ::rtl::OUString(), *(&_rInstance) );
            }
        };
    };

//........................................................................
} // namespace pcr
//........................................................................

#endif // EXTENSIONS_SOURCE_PROPCTRLR_PROPERTYCOMPOSER_HXX
