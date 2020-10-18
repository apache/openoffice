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



#ifndef __FRAMEWORK_SERVICES_FRAME_HXX_
#define __FRAMEWORK_SERVICES_FRAME_HXX_

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________

#include <classes/framecontainer.hxx>
#include <classes/propertysethelper.hxx>
#include <threadhelp/resetableguard.hxx>
#include <threadhelp/writeguard.hxx>
#include <threadhelp/readguard.hxx>
#include <threadhelp/threadhelpbase.hxx>

#ifndef __FRAMEWORK_THREADHELP_TRANSACTIONBASE_HXX_
#include <threadhelp/transactionbase.hxx>
#endif
#include <macros/generic.hxx>
#include <macros/debug.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>
#include <general.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/frame/XDispatchInformationProvider.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XDispatchRecorderSupplier.hpp>
#include <com/sun/star/frame/XDispatchProviderInterception.hpp>
#include <com/sun/star/frame/XDispatchProviderInterceptor.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XFrameActionListener.hpp>
#include <com/sun/star/frame/XFrames.hpp>
#include <com/sun/star/frame/XFramesSupplier.hpp>
#include <com/sun/star/frame/XTitle.hpp>
#include <com/sun/star/frame/XTitleChangeBroadcaster.hpp>
#include <com/sun/star/task/XStatusIndicator.hpp>
#include <com/sun/star/task/XStatusIndicatorFactory.hpp>
#include <com/sun/star/awt/XTopWindowListener.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XTopWindow.hpp>
#include <com/sun/star/awt/XWindowListener.hpp>
#include <com/sun/star/awt/XFocusListener.hpp>
#include <com/sun/star/awt/FocusEvent.hpp>
#include <com/sun/star/datatransfer/dnd/XDropTargetListener.hpp>
#include <com/sun/star/document/XActionLockable.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/frame/XLayoutManager.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <unotools/cmdoptions.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <cppuhelper/weak.hxx>
#include <rtl/string.hxx>

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________
//	exported const
//_________________________________________________________________________________________________________________

// This enum can be used to set different active states of frames
enum EActiveState
{
	E_INACTIVE		,	// I am not a member of active path in tree and I don't have the focus.
	E_ACTIVE		,	// I am in the middle of an active path in tree and I don't have the focus.
	E_FOCUS				// I have the focus now. I must be a member of an active path!
};

//_________________________________________________________________________________________________________________
//	exported definitions
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
	@short		implements a normal frame of hierarchy
	@descr		An instance of this class can be a normal node in frame tree. A frame support influencing of his
				subtree, find of subframes, activate- and deactivate-mechanism as well as
				set/get of a frame window, component or controller.

	@attention	This implementation supports three states: a)uninitialized, b)working, c)disposed
				If you call wrong methods in modes a) or c) ... you will get some exceptions.
				How you should work with this service:
					i) 		create it by using "xServiceManager->createInstance(...)"
					ii)		call XInitialization::initialize() with a valid window reference or use createInstanceWithArguments() at i)
					iii)	work with object
					iv)		dispose object by calling XComponent::dispose()
				After iv) all further requests are rejected by exceptions! (DisposedException)

	@base		ThreadHelpBase
					help to guarantee correct initialized lock member at startup
	@base		TransactionBase
					help to implement unbreakable interface calls
	@base		OBroadcastHelper
				OPropertySetHelper
					implements the property set
	@base		OWeakObject
					provides the refcount and XInterface, XWeak

	@devstatus	ready to use
	@threadsafe	yes
	@modified   04.10.2004 10:47, as96863
*//*-*************************************************************************************************************/
class Frame	:	// interfaces
				public	css::lang::XTypeProvider					,
				public	css::lang::XServiceInfo						,
				public	css::frame::XFramesSupplier					,	// => XFrame => XComponent
				public	css::frame::XDispatchProvider				,
				public	css::frame::XDispatchProviderInterception	,
				public	css::frame::XDispatchInformationProvider	,
				public	css::task::XStatusIndicatorFactory			,
				public	css::awt::XWindowListener					,	// => XEventListener
				public	css::awt::XTopWindowListener				,
				public	css::awt::XFocusListener					,
				public  css::document::XActionLockable				,
				public  css::util::XCloseable						,   // => XCloseBroadcaster
				public  css::frame::XComponentLoader				,
				public  css::frame::XTitle							,
				public  css::frame::XTitleChangeBroadcaster			,
				// base classes
				// Order is necessary for right initialization of this class!
				public  ThreadHelpBase								,
				public  TransactionBase								,
				public  PropertySetHelper							,   // helper implements ThreadHelpbase, TransactionBase, XPropertySet, XPropertySetInfo
				public	::cppu::OWeakObject								// helper implements XInterface, XWeak
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------

	public:

		//---------------------------------------------------------------------------------------------------------
		//	constructor / destructor
		//---------------------------------------------------------------------------------------------------------
	 			 Frame( const css::uno::Reference< css::lang::XMultiServiceFactory >& xFactory	);
		virtual	~Frame(																			);

		//---------------------------------------------------------------------------------------------------------
		//	XInterface, XTypeProvider, XServiceInfo
		//---------------------------------------------------------------------------------------------------------
		FWK_DECLARE_XINTERFACE
		FWK_DECLARE_XTYPEPROVIDER
		DECLARE_XSERVICEINFO

        //---------------------------------------------------------------------------------------------------------
        //  XComponentLoader
        //---------------------------------------------------------------------------------------------------------
        virtual css::uno::Reference< css::lang::XComponent >        SAL_CALL loadComponentFromURL               ( const ::rtl::OUString&                                                        sURL                ,
                                                                                                                  const ::rtl::OUString&                                                        sTargetFrameName    ,
                                                                                                                        sal_Int32                                                               nSearchFlags        ,
                                                                                                                  const css::uno::Sequence< css::beans::PropertyValue >&                        lArguments          ) throw( css::io::IOException                ,
                                                                                                                                                                                                                             css::lang::IllegalArgumentException ,
                                                                                                                                                                                                                             css::uno::RuntimeException          );


		//---------------------------------------------------------------------------------------------------------
		//	XFramesSupplier
		//---------------------------------------------------------------------------------------------------------
    	virtual css::uno::Reference< css::frame::XFrames >			SAL_CALL getFrames							(																									) throw( css::uno::RuntimeException );
    	virtual css::uno::Reference< css::frame::XFrame > 			SAL_CALL getActiveFrame						(																									) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL setActiveFrame						(	const	css::uno::Reference< css::frame::XFrame >&							xFrame				) throw( css::uno::RuntimeException );

		//---------------------------------------------------------------------------------------------------------
		//	XFrame
		//---------------------------------------------------------------------------------------------------------
    	virtual void												SAL_CALL initialize							(	const	css::uno::Reference< css::awt::XWindow >&							xWindow				) throw( css::uno::RuntimeException );
    	virtual css::uno::Reference< css::awt::XWindow >			SAL_CALL getContainerWindow					(																									) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL setCreator							(	const	css::uno::Reference< css::frame::XFramesSupplier >&					xCreator			) throw( css::uno::RuntimeException );
    	virtual css::uno::Reference< css::frame::XFramesSupplier >	SAL_CALL getCreator							(																									) throw( css::uno::RuntimeException );
    	virtual ::rtl::OUString										SAL_CALL getName							(																									) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL setName							(	const	::rtl::OUString&													sName				) throw( css::uno::RuntimeException );
    	virtual css::uno::Reference< css::frame::XFrame >			SAL_CALL findFrame							(	const	::rtl::OUString&													sTargetFrameName	,
																															sal_Int32															nSearchFlags		) throw( css::uno::RuntimeException );
    	virtual sal_Bool											SAL_CALL isTop								(																									) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL activate							(																									) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL deactivate							(																									) throw( css::uno::RuntimeException );
    	virtual sal_Bool											SAL_CALL isActive							(																									) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL contextChanged						(																									) throw( css::uno::RuntimeException );
    	virtual sal_Bool											SAL_CALL setComponent						(	const	css::uno::Reference< css::awt::XWindow >&							xComponentWindow	,
																													const	css::uno::Reference< css::frame::XController >&						xController			) throw( css::uno::RuntimeException );
    	virtual css::uno::Reference< css::awt::XWindow >			SAL_CALL getComponentWindow					(																									) throw( css::uno::RuntimeException );
    	virtual css::uno::Reference< css::frame::XController >		SAL_CALL getController						(																									) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL addFrameActionListener				(	const	css::uno::Reference< css::frame::XFrameActionListener >&			xListener			) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL removeFrameActionListener			(	const	css::uno::Reference< css::frame::XFrameActionListener >&			xListener			) throw( css::uno::RuntimeException );

		//---------------------------------------------------------------------------------------------------------
		//	XComponent
		//---------------------------------------------------------------------------------------------------------
    	virtual void												SAL_CALL dispose							(																									) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL addEventListener					(	const	css::uno::Reference< css::lang::XEventListener >&					xListener			) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL removeEventListener				(	const	css::uno::Reference< css::lang::XEventListener >&					xListener			) throw( css::uno::RuntimeException );

		//---------------------------------------------------------------------------------------------------------
		//	XStatusIndicatorFactory
		//---------------------------------------------------------------------------------------------------------
	    virtual css::uno::Reference< css::task::XStatusIndicator >	SAL_CALL createStatusIndicator				(																									) throw( css::uno::RuntimeException );

		//---------------------------------------------------------------------------------------------------------
		//	XDispatchProvider
		//---------------------------------------------------------------------------------------------------------
    	virtual css::uno::Reference< css::frame::XDispatch >		SAL_CALL queryDispatch						(	const	css::util::URL&														aURL				,
																													const	::rtl::OUString&													sTargetFrameName	,
																															sal_Int32															nSearchFlags		) throw( css::uno::RuntimeException );
    	virtual css::uno::Sequence<
					css::uno::Reference< css::frame::XDispatch > >	SAL_CALL queryDispatches					(	const	css::uno::Sequence< css::frame::DispatchDescriptor >&				lDescriptor	   		) throw( css::uno::RuntimeException );

		//---------------------------------------------------------------------------------------------------------
		//	XDispatchProviderInterception
		//---------------------------------------------------------------------------------------------------------
    	virtual void												SAL_CALL registerDispatchProviderInterceptor(	const	css::uno::Reference< css::frame::XDispatchProviderInterceptor >&	xInterceptor		) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL releaseDispatchProviderInterceptor	(	const	css::uno::Reference< css::frame::XDispatchProviderInterceptor >&	xInterceptor		) throw( css::uno::RuntimeException );

		//---------------------------------------------------------------------------------------------------------
		//	XDispatchInformationProvider
		//---------------------------------------------------------------------------------------------------------
        virtual css::uno::Sequence< sal_Int16 >                       SAL_CALL getSupportedCommandGroups         (                       ) throw (css::uno::RuntimeException);
        virtual css::uno::Sequence< css::frame::DispatchInformation > SAL_CALL getConfigurableDispatchInformation(sal_Int16 nCommandGroup) throw (css::uno::RuntimeException);

		//---------------------------------------------------------------------------------------------------------
		//	XWindowListener
        //  Attention: windowResized() and windowShown() are implement only! All other are empty!
		//---------------------------------------------------------------------------------------------------------
    	virtual void												SAL_CALL windowResized						(	const	css::awt::WindowEvent&												aEvent				) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL windowMoved						(	const	css::awt::WindowEvent&												/*aEvent*/     		) throw( css::uno::RuntimeException ) {};
        virtual void                                                SAL_CALL windowShown                        (   const   css::lang::EventObject&                                             aEvent              ) throw( css::uno::RuntimeException );
        virtual void                                                SAL_CALL windowHidden                       (   const   css::lang::EventObject&                                             aEvent              ) throw( css::uno::RuntimeException );

		//---------------------------------------------------------------------------------------------------------
		//	XFocusListener
		//	Attention: focusLost() not implemented yet!
		//---------------------------------------------------------------------------------------------------------
		virtual void												SAL_CALL focusGained						(	const	css::awt::FocusEvent&												aEvent				) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL focusLost							(	const	css::awt::FocusEvent&												/*aEvent*/			) throw( css::uno::RuntimeException ) {};

		//---------------------------------------------------------------------------------------------------------
		//	XTopWindowListener
        //  Attention: windowActivated(), windowDeactivated() and windowClosing() are implement only! All other are empty!
		//---------------------------------------------------------------------------------------------------------
    	virtual void												SAL_CALL windowActivated					(	const	css::lang::EventObject&												aEvent				) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL windowDeactivated					(	const	css::lang::EventObject&												aEvent				) throw( css::uno::RuntimeException );
    	virtual void												SAL_CALL windowOpened						(	const	css::lang::EventObject&												/*aEvent*/			) throw( css::uno::RuntimeException ) {};
        virtual void                                                SAL_CALL windowClosing                      (   const   css::lang::EventObject&                                             aEvent              ) throw( css::uno::RuntimeException );
        virtual void                                                SAL_CALL windowClosed                       (   const   css::lang::EventObject&                                             /*aEvent*/          ) throw( css::uno::RuntimeException ) {};
    	virtual void												SAL_CALL windowMinimized					(	const	css::lang::EventObject&												/*aEvent*/     		) throw( css::uno::RuntimeException ) {};
    	virtual void												SAL_CALL windowNormalized					(	const	css::lang::EventObject&												/*aEvent*/			) throw( css::uno::RuntimeException ) {};

		//---------------------------------------------------------------------------------------------------------
        //  XEventListener
		//---------------------------------------------------------------------------------------------------------
	    virtual void												SAL_CALL disposing							(	const	css::lang::EventObject&												aEvent				) throw( css::uno::RuntimeException );

		//---------------------------------------------------------------------------------------------------------
        //  XActionLockable
		//---------------------------------------------------------------------------------------------------------
        virtual sal_Bool    SAL_CALL isActionLocked  (                 ) throw( css::uno::RuntimeException );
        virtual void        SAL_CALL addActionLock   (                 ) throw( css::uno::RuntimeException );
        virtual void        SAL_CALL removeActionLock(                 ) throw( css::uno::RuntimeException );
        virtual void        SAL_CALL setActionLocks  ( sal_Int16 nLock ) throw( css::uno::RuntimeException );
        virtual sal_Int16   SAL_CALL resetActionLocks(                 ) throw( css::uno::RuntimeException );

		//---------------------------------------------------------------------------------------------------------
        //  XCloseable
		//---------------------------------------------------------------------------------------------------------
        virtual void SAL_CALL close( sal_Bool bDeliverOwnerShip ) throw( css::util::CloseVetoException,
                                                                         css::uno::RuntimeException   );

		//---------------------------------------------------------------------------------------------------------
        //  XCloseBroadcaster
		//---------------------------------------------------------------------------------------------------------
        virtual void SAL_CALL addCloseListener   ( const css::uno::Reference< css::util::XCloseListener >& xListener ) throw (css::uno::RuntimeException);
        virtual void SAL_CALL removeCloseListener( const css::uno::Reference< css::util::XCloseListener >& xListener ) throw (css::uno::RuntimeException);

		//---------------------------------------------------------------------------------------------------------
        //  XTitle
		//---------------------------------------------------------------------------------------------------------
        virtual ::rtl::OUString SAL_CALL getTitle(                               ) throw (css::uno::RuntimeException);
        virtual void            SAL_CALL setTitle( const ::rtl::OUString& sTitle ) throw (css::uno::RuntimeException);

		//---------------------------------------------------------------------------------------------------------
        //  XTitleChangeBroadcaster
		//---------------------------------------------------------------------------------------------------------
        virtual void SAL_CALL addTitleChangeListener   ( const css::uno::Reference< css::frame::XTitleChangeListener >& xListener) throw (css::uno::RuntimeException);
        virtual void SAL_CALL removeTitleChangeListener( const css::uno::Reference< css::frame::XTitleChangeListener >& xListenr ) throw (css::uno::RuntimeException);

		//---------------------------------------------------------------------------------------------------------
		//	PropertySetHelper => XPropertySet, XPropertySetInfo
		//---------------------------------------------------------------------------------------------------------
    private:

        void impl_initializePropInfo();

        virtual void SAL_CALL impl_setPropertyValue(const ::rtl::OUString& sProperty,
                                                          sal_Int32        nHandle  ,
                                                    const css::uno::Any&   aValue   );

        virtual css::uno::Any SAL_CALL impl_getPropertyValue(const ::rtl::OUString& sProperty,
                                                                   sal_Int32        nHandle  );

	//-------------------------------------------------------------------------------------------------------------
	//	private methods
	//-------------------------------------------------------------------------------------------------------------

	private:

		/*-****************************************************************************************************//**
			@short		helper methods
			@descr		Follow methods are needed at different points of our code (more than once!).

            @attention  Threadsafe methods are signed by "implts_..."!
		*//*-*****************************************************************************************************/

        // threadsafe
        void                                                    implts_sendFrameActionEvent     ( const css::frame::FrameAction&                        aAction          );
        void                                                    implts_resizeComponentWindow    (                                                                        );
        void                                                    implts_setIconOnWindow          (                                                                        );
        void                                                    implts_startWindowListening     (                                                                        );
        void                                                    implts_stopWindowListening      (                                                                        );
        void                                                    implts_saveWindowAttributes     (                                                                        );
        void                                                    implts_checkSuicide             (                                                                        );
		void													implts_forgetSubFrames			(																		 );
        DECL_LINK( implts_windowClosing, void* );
        css::uno::Reference< css::awt::XTopWindow >             impl_searchTopWindow            ( const css::uno::Reference< css::awt::XWindow >&       xWindow          );

        // non threadsafe
        void                                                    impl_checkMenuCloser            (                                                                        );
        void                                                    impl_setCloser                  ( const css::uno::Reference< css::frame::XFrame >&      xFrame           ,
                                                                                                        sal_Bool                                        bState           );
        void                                                    impl_disposeContainerWindow     (       css::uno::Reference< css::awt::XWindow >&       xWindow          );
        static const css::uno::Sequence< css::beans::Property > impl_getStaticPropertyDescriptor(                                                                        );

	//-------------------------------------------------------------------------------------------------------------
	//	debug methods
	//	(should be private everyway!)
	//-------------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		debug-method to check incoming parameter of some other methods of this class
			@descr		The following methods are used to check parameters for other methods
						of this class. The return value is used directly for an ASSERT(...).

			@attention	These methods are static and can't use our member directly! It's better for threadsafe code...
						because we call it with references or pointer to check variables... and must make it safe
						by himself!

			@seealso	ASSERTs in implementation!

			@param		references to checking variables
			@return		sal_True  ,on invalid parameter
			@return		sal_False ,otherwise

			@onerror	We return sal_True
		*//*-*****************************************************************************************************/

	#ifdef ENABLE_ASSERTIONS

	private:

        static sal_Bool implcp_ctor                                (   const   css::uno::Reference< css::lang::XMultiServiceFactory >&             xFactory            );
        static sal_Bool implcp_setActiveFrame                      (   const   css::uno::Reference< css::frame::XFrame >&                          xFrame              );
        static sal_Bool implcp_addFrameActionListener              (   const   css::uno::Reference< css::frame::XFrameActionListener >&            xListener           );
        static sal_Bool implcp_removeFrameActionListener           (   const   css::uno::Reference< css::frame::XFrameActionListener >&            xListener           );
        static sal_Bool implcp_addEventListener                    (   const   css::uno::Reference< css::lang::XEventListener >&                   xListener           );
        static sal_Bool implcp_removeEventListener                 (   const   css::uno::Reference< css::lang::XEventListener >&                   xListener           );
        static sal_Bool implcp_windowResized                       (   const   css::awt::WindowEvent&                                              aEvent              );
        static sal_Bool implcp_focusGained                         (   const   css::awt::FocusEvent&                                               aEvent              );
        static sal_Bool implcp_windowActivated                     (   const   css::lang::EventObject&                                             aEvent              );
        static sal_Bool implcp_windowDeactivated                   (   const   css::lang::EventObject&                                             aEvent              );
        static sal_Bool implcp_disposing                           (   const   css::lang::EventObject&                                             aEvent              );

	#endif	// #ifdef ENABLE_ASSERTIONS

	//*************************************************************************************************************
	//	variables
	//	-threadsafe by own read/write lock "m_aLock"
	//*************************************************************************************************************
	private:

        css::uno::Reference< css::lang::XMultiServiceFactory >                  m_xFactory                          ;   /// reference to factory, which has create this instance
        css::uno::Reference< css::task::XStatusIndicatorFactory >               m_xIndicatorFactoryHelper           ;   /// reference to factory helper to create status indicator objects
        css::uno::WeakReference< css::task::XStatusIndicator >                  m_xIndicatorInterception            ;   /// points to an external set progress, which should be used instead of the internal one.
        css::uno::Reference< css::frame::XDispatchProvider >                    m_xDispatchHelper                   ;   /// helper for XDispatch/Provider and interception interfaces
        css::uno::Reference< css::frame::XFrames >                              m_xFramesHelper                     ;   /// helper for XFrames, XIndexAccess and XElementAccess interfaces
        ::cppu::OMultiTypeInterfaceContainerHelper                              m_aListenerContainer                ;   /// container for ALL Listener
        css::uno::Reference< css::frame::XFramesSupplier >                      m_xParent                           ;   /// parent of this frame
        css::uno::Reference< css::awt::XWindow >                                m_xContainerWindow                  ;   /// containerwindow of this frame for embedded components
        css::uno::Reference< css::awt::XWindow >                                m_xComponentWindow                  ;   /// window of the actual component
        css::uno::Reference< css::frame::XController >                          m_xController                       ;   /// controller of the actual frame
        css::uno::Reference< css::datatransfer::dnd::XDropTargetListener >      m_xDropTargetListener               ;   /// listen to drag & drop
        EActiveState                                                            m_eActiveState                      ;   /// state, if I am a member of active path in tree or I have the focus or ...
        ::rtl::OUString                                                         m_sName                             ;   /// name of this frame
        sal_Bool                                                                m_bIsFrameTop                       ;   /// frame has no parent or the parent is a task on the desktop
        sal_Bool                                                                m_bConnected                        ;   /// due to FrameActionEvent
        sal_Int16                                                               m_nExternalLockCount                ;
        css::uno::Reference< css::frame::XDispatchRecorderSupplier >            m_xDispatchRecorderSupplier         ;   /// is used for dispatch recording and will be set/get from outside. Frame provide it only!
        SvtCommandOptions                                                       m_aCommandOptions                   ;   /// ref counted class to support disabling commands defined by configuration file
        sal_Bool                                                                m_bSelfClose                        ;   /// in case of CloseVetoException on method close() was thrown by ourself - we must close ourself later if no internal processes are running
        sal_Bool                                                                m_bIsHidden                         ;   /// indicates, if this frame is used in hidden mode or not
        static css::uno::WeakReference< css::frame::XFrame >                    m_xCloserFrame                      ;   /// holds the only frame, which must show the special closer menu item (can be NULL!)
        css::uno::Reference< ::com::sun::star::frame::XLayoutManager >    m_xLayoutManager                    ;   /// is used to layout the child windows of the frame.
        css::uno::Reference< css::frame::XDispatchInformationProvider >         m_xDispatchInfoHelper               ;
        css::uno::Reference< css::frame::XTitle >                               m_xTitleHelper                      ;
	     void*                                                                   mp_WindowCommandDispatch            ;   /// holds the window command dispatch

    protected:

        FrameContainer                                                          m_aChildFrameContainer              ;   /// array of child frames

        inline css::uno::Reference< css::lang::XMultiServiceFactory > impl_getFactory()
        {
            ReadGuard aReadLock( m_aLock );
            return m_xFactory;
        }

        inline ::rtl::OUString impl_getName()
        {
            ReadGuard aReadLock( m_aLock );
            return m_sName;
        }

        inline css::uno::Reference< css::awt::XWindow > impl_getContainerWindow()
        {
            ReadGuard aReadLock( m_aLock );
            return m_xContainerWindow;
        }

        inline css::uno::Reference< css::frame::XDispatchProvider > impl_getDispatchHelper()
        {
            ReadGuard aReadLock( m_aLock );
            return m_xDispatchHelper;
        }

        inline css::uno::Reference< css::frame::XFramesSupplier > impl_getParent()
        {
            ReadGuard aReadLock( m_aLock );
            return m_xParent;
        }

};		// class Frame

}		// namespace framework

#endif	// #ifndef __FRAMEWORK_SERVICES_FRAME_HXX_
