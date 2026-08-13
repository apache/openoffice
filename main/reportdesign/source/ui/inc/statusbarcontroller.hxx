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


#ifndef RPTUI_STATUSBARCONTROLLER_HXX
#define RPTUI_STATUSBARCONTROLLER_HXX

#include <svtools/statusbarcontroller.hxx>
#include <comphelper/uno3.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase1.hxx>

class SfxStatusBarControl;
namespace rptui
{
    typedef com::sun::star::uno::Reference< com::sun::star::frame::XStatusbarController > TStatusbarRef;

	typedef ::cppu::ImplHelper1	<	::com::sun::star::lang::XServiceInfo> OStatusbarController_BASE;
	class OStatusbarController : public ::svt::StatusbarController,
								 public OStatusbarController_BASE
	{
        TStatusbarRef   m_rController;
        sal_uInt16      m_nSlotId;
        sal_uInt16      m_nId;
	public:
		OStatusbarController(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB);

        static ::rtl::OUString getImplementationName_Static();
	    static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static(void);
	    static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
		    create(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & xContext);

    private:
        void SAL_CALL dispose();
		// XInterface
        DECLARE_XINTERFACE( )
	    // XServiceInfo
	    virtual ::rtl::OUString SAL_CALL getImplementationName();
	    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL getSupportedServiceNames();
	    // need by registration

	    virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );

        // XInitialization
        virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

        // XUpdatable
        virtual void SAL_CALL update();

        // XStatusListener
	    virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event );

        // XStatusbarController
        virtual ::sal_Bool SAL_CALL mouseButtonDown( const ::com::sun::star::awt::MouseEvent& aMouseEvent );
        virtual ::sal_Bool SAL_CALL mouseMove( const ::com::sun::star::awt::MouseEvent& aMouseEvent );
        virtual ::sal_Bool SAL_CALL mouseButtonUp( const ::com::sun::star::awt::MouseEvent& aMouseEvent );
        virtual void SAL_CALL command( const ::com::sun::star::awt::Point& aPos,
                                       ::sal_Int32 nCommand,
                                       ::sal_Bool bMouseEvent,
                                       const ::com::sun::star::uno::Any& aData );
        virtual void SAL_CALL paint( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XGraphics >& xGraphics,
                                     const ::com::sun::star::awt::Rectangle& rOutputRectangle,
                                     ::sal_Int32 nStyle );
        virtual void SAL_CALL click( const ::com::sun::star::awt::Point& aPos );
        virtual void SAL_CALL doubleClick( const ::com::sun::star::awt::Point& aPos );
	};
}
#endif // DBAUI_STATUSBARCONTROLLER_HXX
