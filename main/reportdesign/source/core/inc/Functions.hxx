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


#ifndef REPORTDESIGN_API_FUNCTIONS_HXX
#define REPORTDESIGN_API_FUNCTIONS_HXX

#include <com/sun/star/report/XFunctions.hpp>
#include <com/sun/star/report/XFunctionsSupplier.hpp>
#include <cppuhelper/compbase1.hxx>
#include <comphelper/broadcasthelper.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <list>


namespace reportdesign
{
	typedef ::cppu::WeakComponentImplHelper1< com::sun::star::report::XFunctions> FunctionsBase;
    /** \class OFunctions Defines the implementation of a \interface com:::sun::star::report::XFunctions
	 * \ingroup reportdesign_api
	 *
	 */
	class OFunctions : public comphelper::OBaseMutex,
					public FunctionsBase
	{
		typedef ::std::list< ::com::sun::star::uno::Reference< ::com::sun::star::report::XFunction > >	TFunctions;
		::cppu::OInterfaceContainerHelper														m_aContainerListeners;
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >			m_xContext;
		::com::sun::star::uno::WeakReference< ::com::sun::star::report::XFunctionsSupplier >    m_xParent;
		TFunctions																				m_aFunctions;
	private:
		OFunctions& operator=(const OFunctions&);
		OFunctions(const OFunctions&);
		void checkIndex(sal_Int32 _nIndex);
	protected:
        // TODO: VirtualFunctionFinder: This is virtual function!
        //
        virtual ~OFunctions();

        /** this function is called upon disposing the component
        */
        // TODO: VirtualFunctionFinder: This is virtual function!
        //
        virtual void SAL_CALL disposing();
	public:
		explicit OFunctions( const ::com::sun::star::uno::Reference< ::com::sun::star::report::XFunctionsSupplier >& _xParent
				,const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >& context);

	// XFunctions
		// Methods
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XFunction > SAL_CALL createFunction(  );
	// XIndexContainer
		virtual void SAL_CALL insertByIndex( ::sal_Int32 Index, const ::com::sun::star::uno::Any& Element );
		virtual void SAL_CALL removeByIndex( ::sal_Int32 Index );
	// XIndexReplace
		virtual void SAL_CALL replaceByIndex( ::sal_Int32 Index, const ::com::sun::star::uno::Any& Element );
	// XIndexAccess
		virtual ::sal_Int32 SAL_CALL getCount(  );
		virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( ::sal_Int32 Index );
	// XElementAccess
		virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
		virtual ::sal_Bool SAL_CALL hasElements(  );
	// XChild
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getParent(  );
		virtual void SAL_CALL setParent( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& Parent );
	// XContainer
		virtual void SAL_CALL addContainerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& xListener );
		virtual void SAL_CALL removeContainerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& xListener );

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
#endif // REPORTDESIGN_API_FUNCTIONS_HXX
