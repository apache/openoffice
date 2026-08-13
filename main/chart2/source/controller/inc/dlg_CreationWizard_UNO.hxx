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



#ifndef _CHART2_CREATION_WIZARD_UNO_HXX
#define _CHART2_CREATION_WIZARD_UNO_HXX

#include "ServiceMacros.hxx"
#include "MutexContainer.hxx"
#include <cppuhelper/component.hxx>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XTerminateListener.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

// header for define DECL_LINK
#include <tools/link.hxx>
#include <vcl/vclevent.hxx>

//.............................................................................
namespace chart
{
//.............................................................................

class CreationWizard;
class CreationWizardUnoDlg : public MutexContainer
                            , public ::cppu::OComponentHelper
						    , public ::com::sun::star::ui::dialogs::XExecutableDialog
                            , public ::com::sun::star::lang::XServiceInfo
                            , public ::com::sun::star::lang::XInitialization
                            , public ::com::sun::star::frame::XTerminateListener
                            , public ::com::sun::star::beans::XPropertySet
{
public:
    CreationWizardUnoDlg( const ::com::sun::star::uno::Reference<
        ::com::sun::star::uno::XComponentContext >& xContext );
    virtual ~CreationWizardUnoDlg();

    // XInterface
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( ::com::sun::star::uno::Type const & rType );
    virtual void SAL_CALL acquire() throw ();
    virtual void SAL_CALL release() throw ();

	// XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	// XServiceInfo
	APPHELPER_XSERVICEINFO_DECL()
	APPHELPER_SERVICE_FACTORY_HELPER(CreationWizardUnoDlg)

    // XExecutableDialog
	virtual void SAL_CALL setTitle( const ::rtl::OUString& aTitle );
    virtual sal_Int16 SAL_CALL execute(  );

	// XInitialization
	virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

	// XTerminateListener
    virtual void SAL_CALL queryTermination( const ::com::sun::star::lang::EventObject& Event );
    virtual void SAL_CALL notifyTermination( const ::com::sun::star::lang::EventObject& Event );

    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source );

    //XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

    DECL_LINK( DialogEventHdl, VclWindowEvent* );

protected:
    // ____ OComponentHelper ____
    /// Called in dispose method after the listeners were notified.
	virtual void SAL_CALL disposing();

private:
    //no default constructor
    CreationWizardUnoDlg();
    void createDialogOnDemand();

private:
    ::com::sun::star::uno::Reference<
        ::com::sun::star::frame::XModel >           m_xChartModel;
    ::com::sun::star::uno::Reference<
        ::com::sun::star::uno::XComponentContext>    m_xCC;
    com::sun::star::uno::Reference<
        com::sun::star::awt::XWindow >               m_xParentWindow;

	CreationWizard*     m_pDialog;
    sal_Bool            m_bUnlockControllersOnExecute;
};

//.............................................................................
} //namespace chart
//.............................................................................
#endif
