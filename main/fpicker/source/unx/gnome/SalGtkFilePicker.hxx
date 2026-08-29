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



#ifndef _SALGTKFILEPICKER_HXX_
#define _SALGTKFILEPICKER_HXX_

//_______________________________________________________________________________________________________________________
//	includes of other projects
//_______________________________________________________________________________________________________________________

#include <cppuhelper/compbase10.hxx>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/ui/dialogs/XFilePickerNotifier.hpp>
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#include <com/sun/star/ui/dialogs/XFilterGroupManager.hpp>
#include <com/sun/star/ui/dialogs/XFilePickerControlAccess.hpp>
#include <com/sun/star/ui/dialogs/XFilePreview.hpp>
#include <com/sun/star/beans/StringPair.hpp>

#ifndef _SALGTKPICKER_HXX_
#include "SalGtkPicker.hxx"
#endif

#include <memory>

#ifndef _RTL_USTRING_H_
#include <rtl/ustring.hxx>
#endif

#include <list>

//----------------------------------------------------------
// Implementation class for the XFilePicker Interface
//----------------------------------------------------------

//----------------------------------------------------------
// forward declarations
//----------------------------------------------------------

using namespace rtl;

struct FilterEntry;
struct ElementEntry_Impl;

typedef ::std::list < FilterEntry >		FilterList;
typedef ::std::list < ElementEntry_Impl >	ElementList;
typedef ::com::sun::star::beans::StringPair	UnoFilterEntry;
typedef ::com::sun::star::uno::Sequence< UnoFilterEntry >	UnoFilterList;	// can be transported more effectively


//----------------------------------------------------------
// class declaration
//----------------------------------------------------------

class SalGtkFilePicker :
        public SalGtkPicker,
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

		// constructor
		SalGtkFilePicker( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceMgr );

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

		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSelectedFiles();

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

		virtual void SAL_CALL setValue( sal_Int16 nControlId, sal_Int16 nControlAction, const ::com::sun::star::uno::Any& aValue );

		virtual ::com::sun::star::uno::Any SAL_CALL getValue( sal_Int16 aControlId, sal_Int16 aControlAction );

		virtual void SAL_CALL enableControl( sal_Int16 nControlId, sal_Bool bEnable );

		virtual void SAL_CALL setLabel( sal_Int16 nControlId, const ::rtl::OUString& aLabel );

		virtual ::rtl::OUString SAL_CALL getLabel( sal_Int16 nControlId );

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

        using cppu::WeakComponentImplHelperBase::disposing;
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

	private:
		// prevent copy and assignment
		SalGtkFilePicker( const SalGtkFilePicker& );
		SalGtkFilePicker& operator=( const SalGtkFilePicker& );

		sal_Bool FilterNameExists( const ::rtl::OUString& rTitle );
		sal_Bool FilterNameExists( const UnoFilterList& _rGroupedFilters );

		void ensureFilterList( const ::rtl::OUString& _rInitialCurrentFilter );

		// to instantiate own services
		::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xServiceMgr;

	private:
        ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XFilePickerListener >
            m_xListener;
        ::rtl::OUString msPlayLabel;
		FilterList *m_pFilterList;
		GtkWidget  *m_pVBox;

        GtkWidget  *m_pFilterExpander;
        GtkWidget  *m_pFilterView;
        GtkListStore *m_pFilterStore;

		enum {
			AUTOEXTENSION,
			PASSWORD,
			FILTEROPTIONS,
			READONLY,
			LINK,
			PREVIEW,
			SELECTION,
			TOGGLE_LAST
		      };

		GtkWidget  *m_pToggles[ TOGGLE_LAST ];

		bool mbToggleVisibility[TOGGLE_LAST];
		bool mbToggleChecked[TOGGLE_LAST];

		static const rtl::OString m_ToggleLabels[TOGGLE_LAST];

		enum {
			PLAY,
			BUTTON_LAST };

		GtkWidget  *m_pButtons[ BUTTON_LAST ];

		enum {
			VERSION,
			TEMPLATE,
			IMAGE_TEMPLATE,
			LIST_LAST
		      };

		GtkWidget *m_pHBoxs[ LIST_LAST ];
        GtkWidget *m_pAligns[ LIST_LAST ];
		GtkWidget *m_pLists[ LIST_LAST ];
		GtkWidget *m_pListLabels[ LIST_LAST ];
		bool mbListVisibility[ LIST_LAST ];
		bool mbButtonVisibility[ BUTTON_LAST ];
        gulong mnHID_FolderChange;
        gulong mnHID_SelectionChange;

		::rtl::OUString	m_aCurrentFilter;
		::rtl::OUString	m_aInitialFilter;

        bool bVersionWidthUnset;
        sal_Bool mbPreviewState;
        gulong mHID_Preview;
        GtkWidget* m_pPreview;
		GtkFileFilter* m_pPseudoFilter;
        sal_Int32 m_PreviewImageWidth;
        sal_Int32 m_PreviewImageHeight;

        GtkWidget  *getWidget( sal_Int16 nControlId, GType *pType = NULL);

		void SetCurFilter( const OUString& rFilter );
		void SetFilters();
        void UpdateFilterfromUI();

        void implChangeType( GtkTreeSelection *selection );
		GtkFileFilter * implAddFilter( const OUString& rFilter, const OUString& rType );
		void implAddFilterGroup( const OUString& rFilter,
					 const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::StringPair>& _rFilters );
        void updateCurrentFilterFromName(const gchar* filtername);
        void unselect_type();
        void InitialMapping();

		void HandleSetListValue(GtkComboBox *pWidget, sal_Int16 nControlAction,
            const ::com::sun::star::uno::Any& rValue);
		::com::sun::star::uno::Any HandleGetListValue(GtkComboBox *pWidget, sal_Int16 nControlAction) const;

        static void expander_changed_cb( GtkExpander *expander, SalGtkFilePicker *pobjFP );
		static void preview_toggled_cb (GtkObject *cb, SalGtkFilePicker *pobjFP);
		static void filter_changed_cb (GtkFileChooser *file_chooser, GParamSpec *pspec, SalGtkFilePicker *pobjFP);
        static void type_changed_cb( GtkTreeSelection *selection, SalGtkFilePicker *pobjFP );
		static void folder_changed_cb (GtkFileChooser *file_chooser, SalGtkFilePicker *pobjFP);
		static void selection_changed_cb (GtkFileChooser *file_chooser, SalGtkFilePicker *pobjFP);
		static void update_preview_cb (GtkFileChooser *file_chooser, SalGtkFilePicker *pobjFP);
        static void dialog_mapped_cb(GtkWidget *widget, SalGtkFilePicker *pobjFP);
	public:
		 virtual ~SalGtkFilePicker();

};
/* vi:set tabstop=4 shiftwidth=4 expandtab: */
#endif // _SALGTKFILEPICKER_HXX_
