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


#ifndef RPTUI_TOOLBOXCONTROLLER_HXX
#define RPTUI_TOOLBOXCONTROLLER_HXX

#include <svtools/toolboxcontroller.hxx>
#include <comphelper/stl_types.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/frame/XSubToolbarController.hpp>
#include <cppuhelper/implbase2.hxx>
#include <comphelper/implementationreference.hxx>

class SfxToolBoxControl;
namespace rptui
{
	typedef ::cppu::ImplHelper2	<	::com::sun::star::lang::XServiceInfo,
                                    ::com::sun::star::frame::XSubToolbarController> TToolboxController_BASE;
    typedef ::comphelper::ImplementationReference<SfxToolBoxControl,::com::sun::star::frame::XToolbarController> TToolbarHelper;

	class OToolboxController : public ::svt::ToolboxController
							  ,public TToolboxController_BASE
	{
		DECLARE_STL_USTRINGACCESS_MAP(sal_Bool,TCommandState);
		TCommandState	m_aStates;
		TToolbarHelper  m_pToolbarController;
		sal_uInt16		m_nToolBoxId;
		sal_uInt16		m_nSlotId;
        OToolboxController(const OToolboxController&);
        void operator =(const OToolboxController&);
	public:
		OToolboxController(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB);
		virtual ~OToolboxController();

		// XInterface
		virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
		virtual void SAL_CALL acquire() throw ();
		virtual void SAL_CALL release() throw ();
		// XServiceInfo
		virtual ::rtl::OUString SAL_CALL getImplementationName();
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL getSupportedServiceNames();
		// need by registration
		static ::rtl::OUString getImplementationName_Static();
		static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static(void);
		static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
			create(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & xContext);
		virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );

		// XInitialization
        virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );
		// XStatusListener
		virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event );
		// XToolbarController
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > SAL_CALL createPopupWindow();

        // XSubToolbarController
        virtual ::sal_Bool SAL_CALL opensSubToolbar(  );
        virtual ::rtl::OUString SAL_CALL getSubToolbarName(  );
        virtual void SAL_CALL functionSelected( const ::rtl::OUString& aCommand );
        virtual void SAL_CALL updateImage(  );
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > SAL_CALL createItemWindow( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& Parent );
	};
//..........................................................................
} // rptui
//..........................................................................
#endif //RPTUI_TOOLBOXCONTROLLER_HXX
