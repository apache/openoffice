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



#ifndef RPT_FIXEDTEXT_HXX
#define RPT_FIXEDTEXT_HXX

#include <cppuhelper/propertysetmixin.hxx>
#include <com/sun/star/report/XFixedText.hpp>
#include "ReportControlModel.hxx"
#include <cppuhelper/compbase2.hxx>
#include <comphelper/broadcasthelper.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include "ReportHelperDefines.hxx"


namespace reportdesign
{
	typedef ::cppu::PropertySetMixin<		 com::sun::star::report::XFixedText	> FixedTextPropertySet;
	typedef ::cppu::WeakComponentImplHelper2<    com::sun::star::report::XFixedText
												,com::sun::star::lang::XServiceInfo	> FixedTextBase;

    /** \class OFixedText Defines the implementation of a \interface com:::sun::star::report::XFixedText
	 * \ingroup reportdesign_api
	 *
	 */
	class OFixedText :	public comphelper::OBaseMutex,
							public FixedTextBase,
							public FixedTextPropertySet
	{
        friend class OShapeHelper;
		OReportControlModel		            m_aProps;
		::rtl::OUString			            m_sLabel;
	private:
		OFixedText(const OFixedText&);
		OFixedText& operator=(const OFixedText&);

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
        void checkIndex(sal_Int32 _nIndex);
	protected:
		virtual ~OFixedText();
	public:
		explicit OFixedText(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & _xContext);
        explicit OFixedText(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & _xContext
                            ,const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & _xFactory
                            ,::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& _xShape);

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

		// XReportComponent
		REPORTCOMPONENT_HEADER()
        // XShape
        SHAPE_HEADER()

        // XShapeDescriptor
        virtual ::rtl::OUString SAL_CALL getShapeType(  );

		// XReportControlModel
		REPORTCONTROLMODEL_HEADER()

		// XCloneable
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL createClone(  );

		// XFixedText
		virtual ::rtl::OUString SAL_CALL getLabel();
		virtual void SAL_CALL setLabel( const ::rtl::OUString& _label );

        // ::com::sun::star::report::XReportControlFormat
        REPORTCONTROLFORMAT_HEADER()

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
        // XContainer
		virtual void SAL_CALL addContainerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& xListener );
		virtual void SAL_CALL removeContainerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& xListener );

		// XElementAccess
		virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
		virtual ::sal_Bool SAL_CALL hasElements(  );

		// XIndexReplace
		virtual void SAL_CALL replaceByIndex( ::sal_Int32 Index, const ::com::sun::star::uno::Any& Element );

		// XIndexContainer
		virtual void SAL_CALL insertByIndex( ::sal_Int32 Index, const ::com::sun::star::uno::Any& Element );
		virtual void SAL_CALL removeByIndex( ::sal_Int32 Index );

		// XIndexAccess
		virtual ::sal_Int32 SAL_CALL getCount(  );
		virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( ::sal_Int32 Index );
	};
}
#endif //RPT_FIXEDTEXT_HXX
