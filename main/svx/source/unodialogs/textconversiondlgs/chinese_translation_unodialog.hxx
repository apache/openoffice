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



#ifndef _TEXTCONVERSIONDLGS_CHINESE_TRANSLATION_UNODIALOG_HXX
#define _TEXTCONVERSIONDLGS_CHINESE_TRANSLATION_UNODIALOG_HXX

#include <cppuhelper/component.hxx>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <cppuhelper/implbase5.hxx>

//.............................................................................
namespace textconversiondlgs
{
//.............................................................................

//-------------------------------------------------------------------------
/** This class provides the chinese translation dialog as an uno component.

It can be created via lang::XMultiComponentFactory::createInstanceWithContext
with servicename "com.sun.star.linguistic2.ChineseTranslationDialog"
or implementation name "com.sun.star.comp.linguistic2.ChineseTranslationDialog"

It can be initialized via the XInitialization interface with the following single parameter:
PropertyValue-Parameter: Name="ParentWindow" Type="awt::XWindow".

It can be executed via the ui::dialogs::XExecutableDialog interface.

Made settings can be retrieved via beans::XPropertySet interface.
Following properties are available (read only and not bound):
1) Name="IsDirectionToSimplified" Type="sal_Bool"
2) Name="IsUseCharacterVariants" Type="sal_Bool"
3) Name="IsTranslateCommonTerms" Type="sal_Bool"

The dialog gets this information from the registry on execute and writes it back to the registry if ended with OK.
*/

class ChineseTranslationDialog;

class ChineseTranslation_UnoDialog : public ::cppu::WeakImplHelper5 <
						      ::com::sun::star::ui::dialogs::XExecutableDialog
                            , ::com::sun::star::lang::XInitialization
                            , ::com::sun::star::beans::XPropertySet
                            , ::com::sun::star::lang::XComponent
                            , ::com::sun::star::lang::XServiceInfo
                            >
                            //	,public ::com::sun::star::uno::XWeak			// implemented by WeakImplHelper(optional interface)
	                        //	,public ::com::sun::star::uno::XInterface		// implemented by WeakImplHelper(optional interface)
	                        //	,public ::com::sun::star::lang::XTypeProvider	// implemented by WeakImplHelper
{
public:
    ChineseTranslation_UnoDialog( const ::com::sun::star::uno::Reference<
        ::com::sun::star::uno::XComponentContext >& xContext );
    virtual ~ChineseTranslation_UnoDialog();

	// lang::XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	static ::rtl::OUString getImplementationName_Static();
    static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static();

    static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
	        create( ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & xContext)
    {
	    return (::cppu::OWeakObject *)new ChineseTranslation_UnoDialog( xContext );
    }

    // lang::XInitialization
	virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

    // ui::dialogs::XExecutableDialog
	virtual void SAL_CALL setTitle( const ::rtl::OUString& aTitle );
    virtual sal_Int16 SAL_CALL execute(  );

    // beans::XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

    // lang::XComponent
	virtual void SAL_CALL dispose();
	virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & xListener );
	virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & xListener );

private:
    //no default constructor
    ChineseTranslation_UnoDialog();

    void impl_DeleteDialog();

private:
    ::com::sun::star::uno::Reference<
        ::com::sun::star::uno::XComponentContext>    m_xCC;
    com::sun::star::uno::Reference<
        com::sun::star::awt::XWindow >               m_xParentWindow;

	ChineseTranslationDialog*     m_pDialog;

	sal_Bool m_bDisposed; ///Dispose call ready.
	sal_Bool m_bInDispose;///In dispose call
    osl::Mutex				        m_aContainerMutex;
    cppu::OInterfaceContainerHelper m_aDisposeEventListeners;
};

//.............................................................................
} //end namespace
//.............................................................................
#endif
