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



#ifndef _FILEPICKER_HXX_
#define _FILEPICKER_HXX_

//_______________________________________________________________________________________________________________________
//	includes of other projects
//_______________________________________________________________________________________________________________________


#include <cppuhelper/compbase10.hxx>
#include <osl/mutex.hxx>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/ui/dialogs/XFilePicker2.hpp>
#include <com/sun/star/ui/dialogs/XFilePickerNotifier.hpp>
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#include <com/sun/star/ui/dialogs/XFilterGroupManager.hpp>
#include <com/sun/star/ui/dialogs/XFilePickerControlAccess.hpp>
#include <com/sun/star/ui/dialogs/XFilePreview.hpp>
#include <com/sun/star/util/XCancellable.hpp>
#include "asynceventnotifier.hxx"
#include "eventnotification.hxx"

#include <memory>

//----------------------------------------------------------
// Implementation class for the XFilePicker Interface
//----------------------------------------------------------

//----------------------------------------------------------
// forward declarations
//----------------------------------------------------------

class CWinFileOpenImpl;

//----------------------------------------------------------
// class declaration
//----------------------------------------------------------

class CFilePickerDummy
{
protected:
	osl::Mutex	m_aMutex;
	osl::Mutex	m_rbHelperMtx;
};

class CFilePicker :
	public CFilePickerDummy,
	public cppu::WeakComponentImplHelper10<
        ::com::sun::star::ui::dialogs::XFilterManager,
        ::com::sun::star::ui::dialogs::XFilterGroupManager,
        ::com::sun::star::ui::dialogs::XFilePickerControlAccess,
        ::com::sun::star::ui::dialogs::XFilePickerNotifier,
        ::com::sun::star::ui::dialogs::XFilePreview,
        ::com::sun::star::ui::dialogs::XFilePicker2,
		::com::sun::star::lang::XInitialization,
        ::com::sun::star::util::XCancellable,
		::com::sun::star::lang::XEventListener,
		::com::sun::star::lang::XServiceInfo >
{
public:

	// ctor
	CFilePicker( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceMgr );

	//------------------------------------------------------------------------------------
	// XFilePickerNotifier
	//------------------------------------------------------------------------------------

    virtual void SAL_CALL addFilePickerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XFilePickerListener >& xListener );
    virtual void SAL_CALL removeFilePickerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XFilePickerListener >& xListener );

	//------------------------------------------------------------------------------------
	// XExecutableDialog functions
	//------------------------------------------------------------------------------------

	virtual void SAL_CALL setTitle( const ::rtl::OUString& aTitle );

	virtual sal_Int16 SAL_CALL execute(  );

	//------------------------------------------------------------------------------------
	// XFilePicker functions
	//------------------------------------------------------------------------------------

	virtual void SAL_CALL setMultiSelectionMode( sal_Bool bMode );

    virtual void SAL_CALL setDefaultName( const ::rtl::OUString& aName );

    virtual void SAL_CALL setDisplayDirectory( const ::rtl::OUString& aDirectory );

	virtual ::rtl::OUString SAL_CALL getDisplayDirectory(  );

    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getFiles(  );

	//------------------------------------------------------------------------------------
	// XFilePicker2 functions
	//------------------------------------------------------------------------------------
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSelectedFiles(  );

	//------------------------------------------------------------------------------------
	// XFilterManager functions
	//------------------------------------------------------------------------------------

	virtual void SAL_CALL appendFilter( const ::rtl::OUString& aTitle, const ::rtl::OUString& aFilter );

    virtual void SAL_CALL setCurrentFilter( const ::rtl::OUString& aTitle );

    virtual ::rtl::OUString SAL_CALL getCurrentFilter(  );

    //------------------------------------------------------------------------------------
	// XFilterGroupManager functions
	//------------------------------------------------------------------------------------

    virtual void SAL_CALL appendFilterGroup( const ::rtl::OUString& sGroupTitle, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::StringPair >& aFilters );

	//------------------------------------------------------------------------------------
	// XFilePickerControlAccess functions
	//------------------------------------------------------------------------------------

	virtual void SAL_CALL setValue( sal_Int16 aControlId, sal_Int16 aControlAction, const ::com::sun::star::uno::Any& aValue );

    virtual ::com::sun::star::uno::Any SAL_CALL getValue( sal_Int16 aControlId, sal_Int16 aControlAction );

    virtual void SAL_CALL enableControl( sal_Int16 aControlId, sal_Bool bEnable );

	virtual void SAL_CALL setLabel( sal_Int16 aControlId, const ::rtl::OUString& aLabel );

    virtual ::rtl::OUString SAL_CALL getLabel( sal_Int16 aControlId );

    //------------------------------------------------
    // XFilePreview
    //------------------------------------------------

    virtual ::com::sun::star::uno::Sequence< sal_Int16 > SAL_CALL getSupportedImageFormats(  );

    virtual sal_Int32 SAL_CALL getTargetColorDepth(  );

    virtual sal_Int32 SAL_CALL getAvailableWidth(  );

    virtual sal_Int32 SAL_CALL getAvailableHeight(  );

    virtual void SAL_CALL setImage( sal_Int16 aImageFormat, const ::com::sun::star::uno::Any& aImage );

    virtual sal_Bool SAL_CALL setShowState( sal_Bool bShowState );

    virtual sal_Bool SAL_CALL getShowState(  );

	//------------------------------------------------
	// XInitialization
	//------------------------------------------------

	virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

    //------------------------------------------------
	// XCancellable
	//------------------------------------------------

    virtual void SAL_CALL cancel( );

	//------------------------------------------------
    // XEventListener
    //------------------------------------------------

    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& aEvent );

	//------------------------------------------------
	// XServiceInfo
	//------------------------------------------------

	virtual ::rtl::OUString SAL_CALL getImplementationName(	 );

	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );

	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

	//------------------------------------------------------------------------------------
	// FilePicker Event functions
	//------------------------------------------------------------------------------------

	void SAL_CALL fileSelectionChanged( ::com::sun::star::ui::dialogs::FilePickerEvent aEvent );
	void SAL_CALL directoryChanged( ::com::sun::star::ui::dialogs::FilePickerEvent aEvent );
    rtl::OUString SAL_CALL helpRequested( ::com::sun::star::ui::dialogs::FilePickerEvent aEvent ) const;
	void SAL_CALL controlStateChanged( ::com::sun::star::ui::dialogs::FilePickerEvent aEvent );
    void SAL_CALL dialogSizeChanged( );

    bool startupEventNotification(bool bStartupSuspended);
    void shutdownEventNotification();
    void suspendEventNotification();
    void resumeEventNotification();

private:
	// prevent copy and assignment
	CFilePicker( const CFilePicker& );
	CFilePicker& operator=( const CFilePicker& );

    using WeakComponentImplHelperBase::disposing;

private:
	::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xServiceMgr;   // to instantiate own services
    CAsyncEventNotifier				                                               m_aAsyncEventNotifier;
	std::auto_ptr<CWinFileOpenImpl>	    										   m_pImpl;
};

#endif
