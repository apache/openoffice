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



#ifndef TOOLKIT_DIALOG_CONTROL_HXX
#define TOOLKIT_DIALOG_CONTROL_HXX

#include <toolkit/controls/controlmodelcontainerbase.hxx>
#include <com/sun/star/awt/XTopWindow.hpp>
#include <com/sun/star/awt/XDialog2.hpp>
#include <com/sun/star/resource/XStringResourceResolver.hpp>
#include "toolkit/helper/servicenames.hxx"
#include "toolkit/helper/macros.hxx"
#include <toolkit/controls/unocontrolcontainer.hxx>
#include <cppuhelper/basemutex.hxx>
#include <cppuhelper/implbase3.hxx>
#include <list>

//	----------------------------------------------------
//	class UnoControlDialogModel
//	----------------------------------------------------

class UnoControlDialogModel :	public ControlModelContainerBase
{
protected:
	::com::sun::star::uno::Any			ImplGetDefaultValue( sal_uInt16 nPropId ) const;
	::cppu::IPropertyArrayHelper&		SAL_CALL getInfoHelper();
public:
						UnoControlDialogModel( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& i_factory );
						UnoControlDialogModel( const UnoControlDialogModel& rModel );
						~UnoControlDialogModel();

    UnoControlModel*	Clone() const;
	// ::com::sun::star::beans::XMultiPropertySet
	::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );

	// ::com::sun::star::io::XPersistObject
    ::rtl::OUString SAL_CALL getServiceName();

	// XServiceInfo
	DECLIMPL_SERVICEINFO_DERIVED( UnoControlDialogModel, ControlModelContainerBase, szServiceName2_UnoControlDialogModel )

};

typedef ::cppu::AggImplInheritanceHelper3   <   ControlContainerBase
                                            ,   ::com::sun::star::awt::XTopWindow
                                            ,   ::com::sun::star::awt::XDialog2
                                            ,   ::com::sun::star::awt::XWindowListener
                                            >   UnoDialogControl_Base;
class UnoDialogControl : public UnoDialogControl_Base
{
private:
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMenuBar >			mxMenuBar;
	TopWindowListenerMultiplexer	                                            maTopWindowListeners;
    bool                                                                        mbWindowListener;

public:

								UnoDialogControl( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& i_factory );
								~UnoDialogControl();
	::rtl::OUString				GetComponentServiceName();

    void SAL_CALL createPeer( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XToolkit >& Toolkit, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer >& Parent );
    void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source );
    void SAL_CALL dispose();

    // ::com::sun::star::awt::XTopWindow
    void SAL_CALL addTopWindowListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTopWindowListener >& xListener );
    void SAL_CALL removeTopWindowListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTopWindowListener >& xListener );
    void SAL_CALL toFront(  );
    void SAL_CALL toBack(  );
    void SAL_CALL setMenuBar( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMenuBar >& xMenu );

    // ::com::sun::star::awt::XWindowListener
    virtual void SAL_CALL windowResized( const ::com::sun::star::awt::WindowEvent& e );
    virtual void SAL_CALL windowMoved( const ::com::sun::star::awt::WindowEvent& e );
    virtual void SAL_CALL windowShown( const ::com::sun::star::lang::EventObject& e );
    virtual void SAL_CALL windowHidden( const ::com::sun::star::lang::EventObject& e );

    // ::com::sun::star::awt::XDialog2
    virtual void SAL_CALL endDialog( ::sal_Int32 Result );
    virtual void SAL_CALL setHelpId( const rtl::OUString& Id );

	// ::com::sun::star::awt::XDialog
    void SAL_CALL setTitle( const ::rtl::OUString& Title );
	::rtl::OUString SAL_CALL getTitle();
	sal_Int16 SAL_CALL execute();
	void SAL_CALL endExecute();

	// ::com::sun::star::awt::XControl
    sal_Bool SAL_CALL setModel( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >& Model );

    // XModifyListener
    virtual void SAL_CALL modified( const ::com::sun::star::lang::EventObject& aEvent );

    // ::com::sun::star::lang::XServiceInfo
	DECLIMPL_SERVICEINFO( UnoDialogControl, szServiceName2_UnoControlDialog )

protected:
   // virtual void ImplModelPropertiesChanged( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyChangeEvent >& rEvents ) throw(::com::sun::star::uno::RuntimeException);
	virtual void PrepareWindowDescriptor( ::com::sun::star::awt::WindowDescriptor& rDesc );
    virtual void ImplModelPropertiesChanged( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyChangeEvent >& rEvents );
protected:
};

#endif // TOOLKIT_DIALOG_CONTROL_HXX
