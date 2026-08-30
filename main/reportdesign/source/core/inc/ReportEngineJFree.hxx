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


#ifndef REPORTDESIGN_API_REPORTENGINEJFREE_HXX
#define REPORTDESIGN_API_REPORTENGINEJFREE_HXX

#include <com/sun/star/report/XReportEngine.hpp>
#include <cppuhelper/compbase2.hxx>
#include <comphelper/broadcasthelper.hxx>
#include <comphelper/uno3.hxx>
#include <comphelper/types.hxx>
#include <cppuhelper/propertysetmixin.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/sdbc/XRowSet.hpp>
#include <comphelper/stl_types.hxx>
#include <comphelper/implementationreference.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>

namespace reportdesign
{
	typedef ::cppu::WeakComponentImplHelper2< com::sun::star::report::XReportEngine
                                             ,com::sun::star::lang::XServiceInfo> ReportEngineBase;
	typedef ::cppu::PropertySetMixin<com::sun::star::report::XReportEngine> ReportEnginePropertySet;

	class OReportEngineJFree : public comphelper::OMutexAndBroadcastHelper,
					public ReportEngineBase,
					public ReportEnginePropertySet
	{
		typedef ::std::multimap< ::rtl::OUString, ::com::sun::star::uno::Any , ::comphelper::UStringMixLess>			TComponentMap;
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >		m_xContext;
		::com::sun::star::uno::Reference< ::com::sun::star::report::XReportDefinition >		m_xReport;
		::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator>			m_StatusIndicator;
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >             m_xActiveConnection;
        ::sal_Int32 											                            m_nMaxRows;
	private:
		OReportEngineJFree(const OReportEngineJFree&);
		OReportEngineJFree& operator=(const OReportEngineJFree&);
		template <typename T> void set(	 const ::rtl::OUString& _sProperty
										,const T& _Value
										,T& _member)
		{
			BoundListeners l;
			{
				::osl::MutexGuard aGuard(m_aMutex);
				prepareSet(_sProperty, ::com::sun::star::uno::makeAny(_member), ::com::sun::star::uno::makeAny(_Value), &l);
				_member = _Value;
			}
			l.notify();
		}

        /** returns the file url for a new model
        *
        * \return The new file url.
        */
        ::rtl::OUString getNewOutputName();

	protected:
        // TODO: VirtualFunctionFinder: This is virtual function!
        //
        virtual ~OReportEngineJFree();
	public:
		typedef ::comphelper::ImplementationReference< OReportEngineJFree	,::com::sun::star::report::XReportEngine,::com::sun::star::uno::XWeak > TReportEngine;

		OReportEngineJFree(const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >& context);

		DECLARE_XINTERFACE( )
        // ::com::sun::star::lang::XServiceInfo
		virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
		virtual ::rtl::OUString SAL_CALL getImplementationName(  );
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

        static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static(void);
		static ::rtl::OUString getImplementationName_Static(void);
        static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
			create(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & xContext);
    private:
		// com::sun::star::beans::XPropertySet
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
		virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
		virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
		virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
		virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
		virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
		virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

		// XReportEngine
			// Attributes
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportDefinition > SAL_CALL getReportDefinition() ;
		virtual void SAL_CALL setReportDefinition( const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportDefinition >& _reportdefinition );
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getActiveConnection();
        virtual void SAL_CALL setActiveConnection( const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _activeconnection );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator > SAL_CALL getStatusIndicator() ;
		virtual void SAL_CALL setStatusIndicator( const ::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator >& _statusindicator ) ;
        virtual ::sal_Int32 SAL_CALL getMaxRows();
        virtual void SAL_CALL setMaxRows( ::sal_Int32 _MaxRows );
			// Methods
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > SAL_CALL createDocumentModel(  ) ;
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > SAL_CALL createDocumentAlive( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& _frame ) ;
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > SAL_CALL createDocumentAlive( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& _frame ,bool _bHidden) ;
		virtual ::com::sun::star::util::URL SAL_CALL createDocument(  ) ;
		virtual void SAL_CALL interrupt(  ) ;

		// XComponent
		virtual void SAL_CALL dispose();
		virtual void SAL_CALL addEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener)
		{
			cppu::WeakComponentImplHelperBase::addEventListener(aListener);
		}
		virtual void SAL_CALL removeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener)
		{
			cppu::WeakComponentImplHelperBase::removeEventListener(aListener);
		}
	};
}
#endif //REPORTDESIGN_API_REPORTENGINEJFREE_HXX
