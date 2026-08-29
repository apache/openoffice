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


#ifndef RPT_FUNCTION_HXX
#define RPT_FUNCTION_HXX

#include <cppuhelper/propertysetmixin.hxx>
#ifndef _COM_SUN_STAR_REPORT_XFunction_HPP_
#include <com/sun/star/report/XFunction.hpp>
#endif
#include <cppuhelper/basemutex.hxx>
#include "ReportControlModel.hxx"
#include <cppuhelper/compbase2.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>

namespace reportdesign
{
	typedef ::cppu::PropertySetMixin<		 com::sun::star::report::XFunction	> FunctionPropertySet;
	typedef ::cppu::WeakComponentImplHelper2<    com::sun::star::report::XFunction
												,com::sun::star::lang::XServiceInfo	> FunctionBase;

    /** \class OFunction Defines the implementation of a \interface com:::sun::star::report::XFunction
	 * \ingroup reportdesign_api
	 *
	 */
	class OFunction :	public cppu::BaseMutex,
							public FunctionBase,
							public FunctionPropertySet
	{
        com::sun::star::beans::Optional< ::rtl::OUString> m_sInitialFormula;
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >  m_xContext;
        ::com::sun::star::uno::WeakReference< ::com::sun::star::report::XFunctions >  m_xParent;
        ::rtl::OUString m_sName;
        ::rtl::OUString m_sFormula;
        ::sal_Bool      m_bPreEvaluated;
        ::sal_Bool      m_bDeepTraversing;
	private:
		OFunction(const OFunction&);
		OFunction& operator=(const OFunction&);

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
	protected:
		virtual ~OFunction();
	public:
		explicit OFunction(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & _xContext);

		DECLARE_XINTERFACE( )
		// ::com::sun::star::lang::XServiceInfo
		virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
		virtual ::rtl::OUString SAL_CALL getImplementationName(  );
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

		static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static(void);
		static ::rtl::OUString getImplementationName_Static(void);
		static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
			create(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & xContext);
		// com::sun::star::beans::XPropertySet
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
		virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
		virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
		virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
		virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
		virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
		virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

		// ::com::sun::star::report::XFunction:
        virtual ::sal_Bool SAL_CALL getPreEvaluated();
        virtual void SAL_CALL setPreEvaluated(::sal_Bool the_value);
        virtual ::sal_Bool SAL_CALL getDeepTraversing();
        virtual void SAL_CALL setDeepTraversing(::sal_Bool the_value);
        virtual ::rtl::OUString SAL_CALL getName();
        virtual void SAL_CALL setName(const ::rtl::OUString & the_value);
        virtual ::rtl::OUString SAL_CALL getFormula();
        virtual void SAL_CALL setFormula(const ::rtl::OUString & the_value);
        virtual com::sun::star::beans::Optional< ::rtl::OUString> SAL_CALL getInitialFormula();
        virtual void SAL_CALL setInitialFormula(const com::sun::star::beans::Optional< ::rtl::OUString> & the_value);

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

        // XChild
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getParent(  );
		virtual void SAL_CALL setParent( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& Parent );
	};
}
#endif //RPT_FUNCTION_HXX
