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



#ifndef REPORTDESIGN_API_REPORTDEFINITION_HXX
#define REPORTDESIGN_API_REPORTDEFINITION_HXX

#include "dllapi.h"

#include "ReportHelperDefines.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/datatransfer/XTransferable.hpp>
#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>
#include <com/sun/star/document/XUndoManagerSupplier.hpp>
#include <com/sun/star/frame/XModule.hpp>
#include <com/sun/star/frame/XTitle.hpp>
#include <com/sun/star/frame/XTitleChangeBroadcaster.hpp>
#include <com/sun/star/frame/XUntitledNumbers.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/report/XReportDefinition.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
/** === end UNO includes === **/

#include <comphelper/embeddedobjectcontainer.hxx>
#include <comphelper/uno3.hxx>
#include <cppuhelper/basemutex.hxx>
#include <cppuhelper/propertysetmixin.hxx>
#include <svx/unomod.hxx>

#if !defined(INCLUDED_COMPHELPER_IMPLBASE_VAR_HXX_13)
#define INCLUDED_COMPHELPER_IMPLBASE_VAR_HXX_13
#define COMPHELPER_IMPLBASE_INTERFACE_NUMBER 13
#include <comphelper/implbase_var.hxx>
#endif

#include <boost/shared_ptr.hpp>


namespace rptui
{
    class OReportModel;
}
namespace comphelper
{
    class MediaDescriptor;
}
namespace reportdesign
{
    class OReportComponentProperties;
    typedef ::comphelper::WeakComponentImplHelper13 <   ::com::sun::star::report::XReportDefinition
                                                    ,   ::com::sun::star::document::XEventBroadcaster
                                                    ,   ::com::sun::star::lang::XServiceInfo
                                                    ,   ::com::sun::star::frame::XModule
                                                    ,   ::com::sun::star::lang::XUnoTunnel
                                                    ,   ::com::sun::star::util::XNumberFormatsSupplier
                                                    ,   ::com::sun::star::frame::XTitle
                                                    ,   ::com::sun::star::frame::XTitleChangeBroadcaster
                                                    ,   ::com::sun::star::frame::XUntitledNumbers
                                                    ,   ::com::sun::star::document::XDocumentPropertiesSupplier
                                                    ,   ::com::sun::star::datatransfer::XTransferable
                                                    ,   ::com::sun::star::document::XUndoManagerSupplier
                                                    ,   SvxUnoDrawMSFactory
                                                    >   ReportDefinitionBase;

    typedef ::cppu::PropertySetMixin< ::com::sun::star::report::XReportDefinition > ReportDefinitionPropertySet;

    struct OReportDefinitionImpl;
    /** \class OReportDefinition Defines the implementation of a \interface com:::sun::star::report::XReportDefinition
	 * \ingroup reportdesign_api
	 *
	 */
    class REPORTDESIGN_DLLPUBLIC OReportDefinition  :public ::cppu::BaseMutex
								                    ,public ReportDefinitionBase
								                    ,public ReportDefinitionPropertySet
                                                    ,public ::comphelper::IEmbeddedHelper
	{
    private:
		::boost::shared_ptr<OReportComponentProperties>								m_aProps;
        ::boost::shared_ptr<OReportDefinitionImpl>                                  m_pImpl;

    private:
		OReportDefinition(const OReportDefinition&);
		OReportDefinition& operator=(const OReportDefinition&);

		void setSection(	 const ::rtl::OUString& _sProperty
							,const sal_Bool& _bOn
							,const ::rtl::OUString& _sName
							,::com::sun::star::uno::Reference< ::com::sun::star::report::XSection>& _member);

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

		/// write a single XML stream into the package
		sal_Bool WriteThroughComponent(
			/// the component we export
			const ::com::sun::star::uno::Reference<
				::com::sun::star::lang::XComponent> & xComponent,
			const sal_Char* pStreamName,		/// the stream name
			const sal_Char* pServiceName,		/// service name of the component
			/// the argument (XInitialization)
			const ::com::sun::star::uno::Sequence<
				::com::sun::star::uno::Any> & rArguments,
			/// output descriptor
			const ::com::sun::star::uno::Sequence<
				::com::sun::star::beans::PropertyValue> & rMediaDesc,
			sal_Bool bPlainStream
			, const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& _xStorageToSaveTo);			/// neither compress nor encrypt

		/// write a single output stream
		/// (to be called either directly or by WriteThroughComponent(...))
		sal_Bool WriteThroughComponent(
			const ::com::sun::star::uno::Reference<
				::com::sun::star::io::XOutputStream> & xOutputStream,
			const ::com::sun::star::uno::Reference<
				::com::sun::star::lang::XComponent> & xComponent,
			const sal_Char* pServiceName,
			const ::com::sun::star::uno::Sequence<
				::com::sun::star::uno::Any> & rArguments,
			const ::com::sun::star::uno::Sequence<
				::com::sun::star::beans::PropertyValue> & rMediaDesc);

		void notifyEvent(const ::rtl::OUString& _sEventName);
        void init();
        void fillArgs(::comphelper::MediaDescriptor& _aDescriptor);

        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XTitle >             impl_getTitleHelper_throw();
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XUntitledNumbers >   impl_getUntitledHelper_throw();

        /** loads the report definition from the given storage
            @precond
                our mutex is locked
            @throws
        */
        void impl_loadFromStorage_nolck_throw(
            const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& _rxStorage,
            const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& _rArguments
        );

	protected:
		virtual ~OReportDefinition();

        /** this function is called upon disposing the component
        */
        virtual void SAL_CALL disposing();
	public:
		explicit OReportDefinition(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & _xContext);
        explicit OReportDefinition(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & _xContext
                                  ,const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & _xFactory
                                  ,::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& _xShape);

		static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static(void);
		static ::rtl::OUString getImplementationName_Static(void);
		static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
			create(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & xContext);

        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > getContext();

        /** return the SdrModel of the real model
        *
        * \return
        */
        ::boost::shared_ptr<rptui::OReportModel> getSdrModel() const;

        static ::boost::shared_ptr<rptui::OReportModel> getSdrModel(const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportDefinition >& _xReportDefinition);
    private:
        DECLARE_XINTERFACE( )
        DECLARE_XTYPEPROVIDER( )
		// ::com::sun::star::lang::XServiceInfo
		virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
		virtual ::rtl::OUString SAL_CALL getImplementationName(  );
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

		// com::sun::star::beans::XPropertySet
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
		virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
		virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
		virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
		virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
		virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
		virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

		// XReportDefinition
        virtual ::rtl::OUString SAL_CALL getMimeType();
        virtual void SAL_CALL setMimeType( const ::rtl::OUString& _mimetype );
		virtual ::rtl::OUString SAL_CALL getCaption();
		virtual void SAL_CALL setCaption( const ::rtl::OUString& _caption );
		virtual ::sal_Int16 SAL_CALL getGroupKeepTogether();
		virtual void SAL_CALL setGroupKeepTogether( ::sal_Int16 _groupkeeptogether );
		virtual ::sal_Int16 SAL_CALL getPageHeaderOption();
		virtual void SAL_CALL setPageHeaderOption( ::sal_Int16 _pageheaderoption );
		virtual ::sal_Int16 SAL_CALL getPageFooterOption();
		virtual void SAL_CALL setPageFooterOption( ::sal_Int16 _pagefooteroption );
		virtual ::rtl::OUString SAL_CALL getCommand();
		virtual void SAL_CALL setCommand( const ::rtl::OUString& _command );
		virtual ::sal_Int32 SAL_CALL getCommandType();
		virtual void SAL_CALL setCommandType( ::sal_Int32 _commandtype );
        virtual ::rtl::OUString SAL_CALL getFilter();
        virtual void SAL_CALL setFilter( const ::rtl::OUString& _filter );
        virtual ::sal_Bool SAL_CALL getEscapeProcessing();
        virtual void SAL_CALL setEscapeProcessing( ::sal_Bool _escapeprocessing );
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getActiveConnection();
        virtual void SAL_CALL setActiveConnection( const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _activeconnection );
        virtual ::rtl::OUString SAL_CALL getDataSourceName();
        virtual void SAL_CALL setDataSourceName( const ::rtl::OUString& _datasourcename );
		virtual ::sal_Bool SAL_CALL getReportHeaderOn();
		virtual void SAL_CALL setReportHeaderOn( ::sal_Bool _reportheaderon );
		virtual ::sal_Bool SAL_CALL getReportFooterOn();
		virtual void SAL_CALL setReportFooterOn( ::sal_Bool _reportfooteron );
		virtual ::sal_Bool SAL_CALL getPageHeaderOn();
		virtual void SAL_CALL setPageHeaderOn( ::sal_Bool _pageheaderon );
		virtual ::sal_Bool SAL_CALL getPageFooterOn();
		virtual void SAL_CALL setPageFooterOn( ::sal_Bool _pagefooteron );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XGroups > SAL_CALL getGroups();
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection > SAL_CALL getReportHeader();
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection > SAL_CALL getPageHeader();
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection > SAL_CALL getDetail();
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection > SAL_CALL getPageFooter();
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XSection > SAL_CALL getReportFooter();
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::document::XEventBroadcaster > SAL_CALL getEventBroadcaster(  );
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getAvailableMimeTypes(  );

		// XReportComponent
        REPORTCOMPONENT_HEADER()

        // XShape
        SHAPE_HEADER()

        // XShapeDescriptor
        virtual ::rtl::OUString SAL_CALL getShapeType(  );

        //XFunctionsSupplier
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::report::XFunctions > SAL_CALL getFunctions();

		// XCloneable
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL createClone(  );

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

		// XCloseBroadcaster
		virtual void SAL_CALL addCloseListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloseListener >& Listener );
		virtual void SAL_CALL removeCloseListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloseListener >& Listener );

		// XCloseable
		virtual void SAL_CALL close( ::sal_Bool DeliverOwnership );

		// XModel
		virtual ::sal_Bool SAL_CALL attachResource( const ::rtl::OUString& URL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& Arguments );
		virtual ::rtl::OUString SAL_CALL getURL(  );
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getArgs(  );
		virtual void SAL_CALL connectController( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController >& Controller );
		virtual void SAL_CALL disconnectController( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController >& Controller );
		virtual void SAL_CALL lockControllers(  );
		virtual void SAL_CALL unlockControllers(  );
		virtual ::sal_Bool SAL_CALL hasControllersLocked(  );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController > SAL_CALL getCurrentController(  );
		virtual void SAL_CALL setCurrentController( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController >& Controller );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getCurrentSelection(  );

		// XStorageBasedDocument
		virtual void SAL_CALL loadFromStorage( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aMediaDescriptor );
		virtual void SAL_CALL storeToStorage( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aMediaDescriptor );
		virtual void SAL_CALL switchToStorage( const ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage >& xStorage );
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage > SAL_CALL getDocumentStorage(  );
		virtual void SAL_CALL addStorageChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::document::XStorageChangeListener >& xListener );
		virtual void SAL_CALL removeStorageChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::document::XStorageChangeListener >& xListener );

        // XViewDataSupplier
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > SAL_CALL getViewData(  );
        virtual void SAL_CALL setViewData( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& Data );

		// XLoadable
		virtual void SAL_CALL initNew(  );
		virtual void SAL_CALL load( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArguments );

		// XVisualObject
		virtual void SAL_CALL setVisualAreaSize( ::sal_Int64 nAspect, const ::com::sun::star::awt::Size& aSize );
		virtual ::com::sun::star::awt::Size SAL_CALL getVisualAreaSize( ::sal_Int64 nAspect );
		virtual ::com::sun::star::embed::VisualRepresentation SAL_CALL getPreferredVisualRepresentation( ::sal_Int64 nAspect );
		virtual ::sal_Int32 SAL_CALL getMapUnit( ::sal_Int64 nAspect );

		// XModifiable2
        virtual ::sal_Bool SAL_CALL disableSetModified(  );
        virtual ::sal_Bool SAL_CALL enableSetModified(  );
        virtual ::sal_Bool SAL_CALL isSetModifiedEnabled(  );

		// XModifiable
		virtual ::sal_Bool SAL_CALL isModified(  );
		virtual void SAL_CALL setModified( ::sal_Bool bModified );

		// XModifyBroadcaster
		virtual void SAL_CALL addModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );
		virtual void SAL_CALL removeModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );

		// document::XEventBroadcaster
		virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::document::XEventListener >& aListener );
		virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::document::XEventListener >& aListener );

		// document::XEventListener
		virtual void SAL_CALL notifyEvent( const ::com::sun::star::document::EventObject& aEvent );

        // XUIConfigurationManagerSupplier
	    virtual ::com::sun::star::uno::Reference< ::com::sun::star::ui::XUIConfigurationManager > SAL_CALL getUIConfigurationManager(  );

        // XDocumentSubStorageSupplier
	    virtual ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage > SAL_CALL getDocumentSubStorage( const ::rtl::OUString& aStorageName, sal_Int32 nMode );
	    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getDocumentSubStoragesNames(  );

        // com::sun::star::lang::XUnoTunnel
		virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );
		static ::com::sun::star::uno::Sequence< sal_Int8 > getUnoTunnelImplementationId();

        // SvxUnoDrawMSFactory
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL createInstance( const ::rtl::OUString& aServiceSpecifier );
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL createInstanceWithArguments( const ::rtl::OUString& ServiceSpecifier, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& Arguments );
        ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getAvailableServiceNames(void);

        // XStyleFamiliesSupplier
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > SAL_CALL getStyleFamilies(  );

		// XModule
		virtual void SAL_CALL setIdentifier( const ::rtl::OUString& Identifier );
		virtual ::rtl::OUString SAL_CALL getIdentifier(  );

        // XNumberFormatsSupplier
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL getNumberFormatSettings(  );
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormats > SAL_CALL getNumberFormats(  );

        // XTitle
        virtual ::rtl::OUString SAL_CALL getTitle(  );
        virtual void SAL_CALL setTitle( const ::rtl::OUString& sTitle );

        // XTitleChangeBroadcaster
        virtual void SAL_CALL addTitleChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XTitleChangeListener >& xListener );
        virtual void SAL_CALL removeTitleChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XTitleChangeListener >& xListener );

        // XUntitledNumbers
        virtual ::sal_Int32 SAL_CALL leaseNumber( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& xComponent );
        virtual void SAL_CALL releaseNumber( ::sal_Int32 nNumber );
        virtual void SAL_CALL releaseNumberForComponent( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& xComponent );
        virtual ::rtl::OUString SAL_CALL getUntitledPrefix(  );

        // XDocumentPropertiesSupplier
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::document::XDocumentProperties > SAL_CALL getDocumentProperties(  );

        // XTransferable
        virtual ::com::sun::star::uno::Any SAL_CALL getTransferData( const ::com::sun::star::datatransfer::DataFlavor& aFlavor );
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::datatransfer::DataFlavor > SAL_CALL getTransferDataFlavors(  );
        virtual ::sal_Bool SAL_CALL isDataFlavorSupported( const ::com::sun::star::datatransfer::DataFlavor& aFlavor );

        // XUndoManagerSupplier
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::document::XUndoManager > SAL_CALL getUndoManager(  );

        // comphelper::IEmbeddedHelper
        virtual com::sun::star::uno::Reference < com::sun::star::embed::XStorage > getStorage() const;
        virtual ::comphelper::EmbeddedObjectContainer& getEmbeddedObjectContainer() const;
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler > getInteractionHandler() const;
        virtual bool isEnableSetModified() const;

        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > getContext() const;
  	};
// =============================================================================
} // namespace reportdesign
// =============================================================================
#endif // REPORTDESIGN_API_REPORTDEFINITION_HXX
