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


#ifndef REPORTDESIGN_API_SECTION_HXX
#define REPORTDESIGN_API_SECTION_HXX

#include <com/sun/star/report/XSection.hpp>
#include <cppuhelper/compbase3.hxx>
#include <comphelper/broadcasthelper.hxx>
#include <comphelper/uno3.hxx>
#include <comphelper/types.hxx>
#include <cppuhelper/propertysetmixin.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/drawing/XShapes.hpp>
#include <com/sun/star/uno/XAggregation.hpp>
#include <comphelper/stl_types.hxx>
#include <comphelper/implementationreference.hxx>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>

namespace reportdesign
{
    typedef ::cppu::WeakComponentImplHelper3<   ::com::sun::star::report::XSection
                                            ,   ::com::sun::star::lang::XServiceInfo
                                            ,	::com::sun::star::lang::XUnoTunnel> SectionBase;
	typedef ::cppu::PropertySetMixin<com::sun::star::report::XSection> SectionPropertySet;

	class OSection : public comphelper::OMutexAndBroadcastHelper,
					public SectionBase,
					public SectionPropertySet
	{
        ::cppu::OInterfaceContainerHelper													m_aContainerListeners;
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >		m_xContext;
        ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >		        m_xDrawPage;
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation >             m_xProxy;
		::com::sun::star::uno::WeakReference< ::com::sun::star::report::XGroup >			m_xGroup;
		::com::sun::star::uno::WeakReference< ::com::sun::star::report::XReportDefinition > m_xReportDefinition;
		::rtl::OUString 																	m_sName;
		::rtl::OUString 																	m_sConditionalPrintExpression;
		::sal_uInt32																		m_nHeight;
		::sal_Int32 																		m_nBackgroundColor;
		::sal_Int16 																		m_nForceNewPage;
		::sal_Int16 																		m_nNewRowOrCol;
		::sal_Bool 																			m_bKeepTogether;
		::sal_Bool 																			m_bCanGrow;
		::sal_Bool 																			m_bCanShrink;
		::sal_Bool 																			m_bRepeatSection;
		::sal_Bool																			m_bVisible;
        ::sal_Bool																			m_bBacktransparent;
        bool                                                                                m_bInRemoveNotify;
        bool                                                                                m_bInInsertNotify;

	private:
		OSection(const OSection&);
		OSection& operator=(const OSection&);

		template <typename T> void set(	 const ::rtl::OUString& _sProperty
										,const T& _Value
										,T& _member)
		{
			BoundListeners l;
			{
				::osl::MutexGuard aGuard(m_aMutex);
                if ( _member != _Value )
                {
				    prepareSet(_sProperty, ::com::sun::star::uno::makeAny(_member), ::com::sun::star::uno::makeAny(_Value), &l);
				    _member = _Value;
                }
			}
			l.notify();
		}

        /** checks if this section is either the page header or footer and if so it throws an UnknownPropertyException
        *
        */
        void checkNotPageHeaderFooter();

        void init();
	protected:
        // TODO: VirtualFunctionFinder: This is virtual function!
        //
        virtual ~OSection();

        /** this function is called upon disposing the component
        */
        // TODO: VirtualFunctionFinder: This is virtual function!
        //
        virtual void SAL_CALL disposing();
	public:
		typedef ::comphelper::ImplementationReference< OSection	,::com::sun::star::report::XSection,::com::sun::star::uno::XWeak > TSection;

		OSection(const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportDefinition >& _xParent
				,const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >& context,bool _bPageSection=false);
		OSection(const ::com::sun::star::uno::Reference< ::com::sun::star::report::XGroup >& _xParent
				,const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >& context,bool _bPageSection=false);

		DECLARE_XINTERFACE( )

		// ::com::sun::star::lang::XServiceInfo
		virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
		virtual ::rtl::OUString SAL_CALL getImplementationName(  );
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

		static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static(void);
		// com::sun::star::beans::XPropertySet
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
		virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
		virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
		virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
		virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
		virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
		virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

		// XSection
		virtual ::sal_Bool SAL_CALL getVisible();
		virtual void SAL_CALL setVisible( ::sal_Bool _visible );
		virtual ::rtl::OUString SAL_CALL getName();
		virtual void SAL_CALL setName( const ::rtl::OUString& _name );
		virtual ::sal_uInt32 SAL_CALL getHeight();
		virtual void SAL_CALL setHeight( ::sal_uInt32 _height );
		virtual ::sal_Int32 SAL_CALL getBackColor();
		virtual void SAL_CALL setBackColor( ::sal_Int32 _backgroundcolor );
        virtual ::sal_Bool SAL_CALL getBackTransparent();
        virtual void SAL_CALL setBackTransparent( ::sal_Bool _backtransparent );
		virtual ::rtl::OUString SAL_CALL getConditionalPrintExpression();
		virtual void SAL_CALL setConditionalPrintExpression( const ::rtl::OUString& _conditionalprintexpression );
		virtual ::sal_Int16 SAL_CALL getForceNewPage();
		virtual void SAL_CALL setForceNewPage( ::sal_Int16 _forcenewpage );
		virtual ::sal_Int16 SAL_CALL getNewRowOrCol();
		virtual void SAL_CALL setNewRowOrCol( ::sal_Int16 _newroworcol );
		virtual ::sal_Bool SAL_CALL getKeepTogether();
		virtual void SAL_CALL setKeepTogether( ::sal_Bool _keeptogether );
		virtual ::sal_Bool SAL_CALL getCanGrow();
		virtual void SAL_CALL setCanGrow( ::sal_Bool _cangrow );
		virtual ::sal_Bool SAL_CALL getCanShrink();
		virtual void SAL_CALL setCanShrink( ::sal_Bool _canshrink );
		virtual ::sal_Bool SAL_CALL getRepeatSection();
		virtual void SAL_CALL setRepeatSection( ::sal_Bool _repeatsection );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XGroup > SAL_CALL getGroup();
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportDefinition > SAL_CALL getReportDefinition();

		virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent > SAL_CALL createReportComponent( const ::rtl::OUString& _sReportComponentSpecifier );
	    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getAvailableReportComponentNames(  );
		// XChild
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getParent(  );
		virtual void SAL_CALL setParent( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& Parent );
		// XContainer
		virtual void SAL_CALL addContainerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& xListener );
		virtual void SAL_CALL removeContainerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& xListener );
		// XElementAccess
		virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
		virtual ::sal_Bool SAL_CALL hasElements(  );
        // XShapes
        virtual void SAL_CALL add( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape );
        virtual void SAL_CALL remove( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape );
		// XIndexAccess
		virtual ::sal_Int32 SAL_CALL getCount(  );
		virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( ::sal_Int32 Index );
		// XEnumerationAccess
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL createEnumeration(  );

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

        // com::sun::star::lang::XUnoTunnel
		virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );
        static OSection* getImplementation( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxComponent );
        static ::com::sun::star::uno::Sequence< sal_Int8 > getUnoTunnelImplementationId();

		static void lcl_copySection(const ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection>& _xSource
										 ,::com::sun::star::uno::Reference< ::com::sun::star::report::XSection>& _xDest);

        void notifyElementAdded(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape);
        void notifyElementRemoved(const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape);
	};
}
#endif //REPORTDESIGN_API_SECTION_HXX
