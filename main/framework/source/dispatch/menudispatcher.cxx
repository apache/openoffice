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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_framework.hxx"

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________
#include <dispatch/menudispatcher.hxx>
#include <general.h>
#include <framework/menuconfiguration.hxx>
#include <framework/addonmenu.hxx>
#include <services.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#include <com/sun/star/awt/XToolkit.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/WindowDescriptor.hpp>
#include <com/sun/star/awt/PosSize.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/beans/UnknownPropertyException.hpp>
#include <com/sun/star/lang/WrappedTargetException.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XEnumeration.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>

#include <vcl/window.hxx>
#include <vcl/syswin.hxx>
#include <vcl/menu.hxx>
#include <vcl/svapp.hxx>
#include <tools/resmgr.hxx>
#include <tools/rcid.h>
#include <vos/mutex.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <rtl/logfile.hxx>

//_________________________________________________________________________________________________________________
//	includes of other projects
//_________________________________________________________________________________________________________________

#include <ucbhelper/content.hxx>

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

namespace framework{

using namespace ::com::sun::star                ;
using namespace ::com::sun::star::awt			;
using namespace ::com::sun::star::beans			;
using namespace ::com::sun::star::container		;
using namespace ::com::sun::star::frame			;
using namespace ::com::sun::star::lang			;
using namespace ::com::sun::star::uno			;
using namespace ::com::sun::star::util			;
using namespace ::cppu							;
using namespace ::osl							;
using namespace ::rtl							;
using namespace ::vos							;

//_________________________________________________________________________________________________________________
//	non exported const
//_________________________________________________________________________________________________________________

const sal_uInt16 SLOTID_MDIWINDOWLIST = 5610;

//_________________________________________________________________________________________________________________
//	non exported definitions
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	declarations
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
MenuDispatcher::MenuDispatcher(   const   uno::Reference< XMultiServiceFactory >&  xFactory    ,
									const	uno::Reference< XFrame >&				xOwner		)
		//	Init baseclasses first
        :   ThreadHelpBase          ( &Application::GetSolarMutex()  )
        ,   OWeakObject             (                                )
        // Init member
        ,   m_xOwnerWeak            ( xOwner                         )
        ,   m_xFactory              ( xFactory                       )
        ,   m_aListenerContainer    ( m_aLock.getShareableOslMutex() )
        ,   m_bAlreadyDisposed      ( sal_False                      )
        ,   m_bActivateListener     ( sal_False                      )
        ,   m_pMenuManager          ( NULL                           )
{
	// Safe impossible cases
	// We need valid informations about our owner for work.
    LOG_ASSERT( impldbg_checkParameter_MenuDispatcher( xFactory, xOwner ), "MenuDispatcher::MenuDispatcher()\nInvalid parameter detected!\n" )

	m_bActivateListener = sal_True;
	xOwner->addFrameActionListener( uno::Reference< XFrameActionListener >( (OWeakObject *)this, UNO_QUERY ));
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
MenuDispatcher::~MenuDispatcher()
{
	// Warn programmer if he forgot to dispose this instance.
	// We must release all our references ...
	// and a dtor isn't the best place to do that!
}

//*****************************************************************************************************************
//	XInterface, XTypeProvider
//*****************************************************************************************************************
DEFINE_XINTERFACE_4     (   MenuDispatcher                     ,
							OWeakObject					  		,
							DIRECT_INTERFACE(	XTypeProvider	),
							DIRECT_INTERFACE(	XDispatch		),
							DIRECT_INTERFACE(	XEventListener	),
							DERIVED_INTERFACE(	XFrameActionListener, XEventListener )
						)

DEFINE_XTYPEPROVIDER_4  (   MenuDispatcher     ,
							XTypeProvider		,
							XDispatch			,
							XEventListener		,
							XFrameActionListener
						)


//*****************************************************************************************************************
//	XDispatch
//*****************************************************************************************************************
void SAL_CALL MenuDispatcher::dispatch(    const   URL&                        /*aURL*/            ,
											const	Sequence< PropertyValue >&	/*seqProperties*/	) throw( RuntimeException )
{
}

//*****************************************************************************************************************
//	XDispatch
//*****************************************************************************************************************
void SAL_CALL MenuDispatcher::addStatusListener(   const   uno::Reference< XStatusListener >&   xControl,
													const	URL&							aURL	) throw( RuntimeException )
{
	// Ready for multithreading
	ResetableGuard aGuard( m_aLock );
	// Safe impossible cases
	// Method not defined for all incoming parameter
    LOG_ASSERT( impldbg_checkParameter_addStatusListener( xControl, aURL ), "MenuDispatcher::addStatusListener()\nInvalid parameter detected.\n" )
	// Add listener to container.
	m_aListenerContainer.addInterface( aURL.Complete, xControl );
}

//*****************************************************************************************************************
//	XDispatch
//*****************************************************************************************************************
void SAL_CALL MenuDispatcher::removeStatusListener(    const   uno::Reference< XStatusListener >&   xControl,
														const	URL&							aURL	) throw( RuntimeException )
{
	// Ready for multithreading
	ResetableGuard aGuard( m_aLock );
	// Safe impossible cases
	// Method not defined for all incoming parameter
    LOG_ASSERT( impldbg_checkParameter_removeStatusListener( xControl, aURL ), "MenuDispatcher::removeStatusListener()\nInvalid parameter detected.\n" )
	// Add listener to container.
	m_aListenerContainer.removeInterface( aURL.Complete, xControl );
}

//*****************************************************************************************************************
//	 XFrameActionListener
//*****************************************************************************************************************

void SAL_CALL MenuDispatcher::frameAction( const FrameActionEvent& aEvent ) throw ( RuntimeException )
{
	ResetableGuard aGuard( m_aLock );

	if ( m_pMenuManager && aEvent.Action == FrameAction_FRAME_UI_ACTIVATED )
	{
		MenuBar* pMenuBar = (MenuBar *)m_pMenuManager->GetMenu();
		uno::Reference< XFrame > xFrame( m_xOwnerWeak.get(), UNO_QUERY );
		aGuard.unlock();

		if ( xFrame.is() && pMenuBar )
		{
			uno::Reference< ::com::sun::star::awt::XWindow >xContainerWindow = xFrame->getContainerWindow();

			OGuard aSolarGuard( Application::GetSolarMutex() );
			{
				Window* pWindow = VCLUnoHelper::GetWindow( xContainerWindow );
				while ( pWindow && !pWindow->IsSystemWindow() )
					pWindow = pWindow->GetParent();

				if ( pWindow )
				{
					SystemWindow* pSysWindow = (SystemWindow *)pWindow;
					pSysWindow->SetMenuBar( pMenuBar );
				}
			}
		}
	}
	else if ( m_pMenuManager && aEvent.Action == css::frame::FrameAction_COMPONENT_DETACHING )
	{
		if ( m_pMenuManager )
			impl_setMenuBar( NULL );
	}
}

//*****************************************************************************************************************
//	 XEventListener
//*****************************************************************************************************************
void SAL_CALL MenuDispatcher::disposing( const EventObject& ) throw( RuntimeException )
{
	// Ready for multithreading
	ResetableGuard aGuard( m_aLock );
	// Safe impossible cases
    LOG_ASSERT( !(m_bAlreadyDisposed==sal_True), "MenuDispatcher::disposing()\nObject already disposed .. don't call it again!\n" )

	if( m_bAlreadyDisposed == sal_False )
	{
		m_bAlreadyDisposed = sal_True;

		if ( m_bActivateListener )
		{
			uno::Reference< XFrame > xFrame( m_xOwnerWeak.get(), UNO_QUERY );
			if ( xFrame.is() )
			{
				xFrame->removeFrameActionListener( uno::Reference< XFrameActionListener >( (OWeakObject *)this, UNO_QUERY ));
				m_bActivateListener = sal_False;
				if ( m_pMenuManager )
				{
					EventObject aEventObj;
					aEventObj.Source = xFrame;
					m_pMenuManager->disposing( aEventObj );
				}
			}
		}

		// Forget our factory.
		m_xFactory = uno::Reference< XMultiServiceFactory >();

		// Remove our menu from system window if it is still there!
		if ( m_pMenuManager )
			impl_setMenuBar( NULL );
	}
}

//*****************************************************************************************************************
//	private method
//
//
//*****************************************************************************************************************
void MenuDispatcher::impl_setAccelerators( Menu* pMenu, const Accelerator& aAccel )
{
	for ( sal_uInt16 nPos = 0; nPos < pMenu->GetItemCount(); ++nPos )
	{
		sal_uInt16     nId    = pMenu->GetItemId(nPos);
        PopupMenu* pPopup = pMenu->GetPopupMenu(nId);
        if ( pPopup )
            impl_setAccelerators( (Menu *)pPopup, aAccel );
        else if ( nId && !pMenu->GetPopupMenu(nId))
		{
			KeyCode aCode = aAccel.GetKeyCode( nId );
			if ( aCode.GetCode() )
			    pMenu->SetAccelKey( nId, aCode );
		}
	}
}

//*****************************************************************************************************************
//	private method
//
//
//*****************************************************************************************************************
sal_Bool MenuDispatcher::impl_setMenuBar( MenuBar* pMenuBar, sal_Bool bMenuFromResource )
{
	uno::Reference< XFrame > xFrame( m_xOwnerWeak.get(), UNO_QUERY );
	if ( xFrame.is() )
	{
		uno::Reference< ::com::sun::star::awt::XWindow >xContainerWindow = xFrame->getContainerWindow();
		Window* pWindow = NULL;

		// Use SolarMutex for threadsafe code too!
		OGuard aSolarGuard( Application::GetSolarMutex() );
		{
			pWindow = VCLUnoHelper::GetWindow( xContainerWindow );
			while ( pWindow && !pWindow->IsSystemWindow() )
				pWindow = pWindow->GetParent();
		}

		if ( pWindow )
		{
			// Ready for multithreading
			ResetableGuard aGuard( m_aLock );

			SystemWindow* pSysWindow = (SystemWindow *)pWindow;

			if ( m_pMenuManager )
			{
                // remove old menu from our system window if it was set before
                if ( m_pMenuManager->GetMenu() == (Menu *)pSysWindow->GetMenuBar() )
                    pSysWindow->SetMenuBar( NULL );

                // remove listener before we destruct ourself, so we cannot be called back afterwards
                m_pMenuManager->RemoveListener();

                SAL_STATIC_CAST( ::com::sun::star::uno::XInterface*, (OWeakObject*)m_pMenuManager )->release();

				m_pMenuManager = 0;
			}

			if ( pMenuBar != NULL )
			{
                sal_uInt16 nPos = pMenuBar->GetItemPos( SLOTID_MDIWINDOWLIST );
                if ( nPos != MENU_ITEM_NOTFOUND )
                {
                    OUString aNoContext;

                    uno::Reference< XModel >			xModel;
                    uno::Reference< XController >	xController( xFrame->getController(), UNO_QUERY );

                    if ( xController.is() )
                        xModel = uno::Reference< XModel >( xController->getModel(), UNO_QUERY );

                    // retrieve addon popup menus and add them to our menu bar
                    AddonMenuManager::MergeAddonPopupMenus( xFrame, nPos, pMenuBar, m_xFactory );

                    // retrieve addon help menu items and add them to our help menu
                    AddonMenuManager::MergeAddonHelpMenu( xFrame, pMenuBar, m_xFactory );
                }

                // set new menu on our system window and create new menu manager
                if ( bMenuFromResource )
                {
                    // #110897#
                    // m_pMenuManager = new MenuManager( xFrame, pMenuBar, sal_True, sal_False );
                    m_pMenuManager = new MenuManager( m_xFactory, xFrame, pMenuBar, sal_True, sal_False );
                }
                else
                {
                    // #110897#
                    // m_pMenuManager = new MenuManager( xFrame, pMenuBar, sal_True, sal_True );
                    m_pMenuManager = new MenuManager( m_xFactory, xFrame, pMenuBar, sal_True, sal_True );
                }

                pSysWindow->SetMenuBar( pMenuBar );
			}

			return sal_True;
		}
	}

	return sal_False;
}

IMPL_LINK( MenuDispatcher, Close_Impl, void*, EMPTYARG )
{
	css::uno::Reference < css::frame::XFrame > xFrame( m_xOwnerWeak.get(), css::uno::UNO_QUERY );
	if ( !xFrame.is() )
		return 0;

	css::util::URL aURL;
	aURL.Complete = ::rtl::OUString::createFromAscii(".uno:CloseWin");
	css::uno::Reference< css::util::XURLTransformer >  xTrans ( m_xFactory->createInstance(
						SERVICENAME_URLTRANSFORMER ), css::uno::UNO_QUERY );
	if( xTrans.is() )
	{
		// Datei laden
		xTrans->parseStrict( aURL );
		uno::Reference< XDispatchProvider > xProv( xFrame, UNO_QUERY );
		if ( xProv.is() )
		{
			css::uno::Reference < css::frame::XDispatch > aDisp = xProv->queryDispatch( aURL, ::rtl::OUString(), 0 );
			if ( aDisp.is() )
				aDisp->dispatch( aURL, css::uno::Sequence < css::beans::PropertyValue>() );
		}
	}

	return 0;
}


//_________________________________________________________________________________________________________________
//	debug methods
//_________________________________________________________________________________________________________________

/*-----------------------------------------------------------------------------------------------------------------
	The follow methods checks the parameter for other functions. If a parameter or his value is non valid,
	we return "sal_False". (else sal_True) This mechanism is used to throw an ASSERT!

	ATTENTION

		If you miss a test for one of this parameters, contact the author or add it himself !(?)
		But ... look for right testing! See using of this methods!
-----------------------------------------------------------------------------------------------------------------*/

#ifdef ENABLE_ASSERTIONS

//*****************************************************************************************************************
sal_Bool MenuDispatcher::impldbg_checkParameter_MenuDispatcher(   const   uno::Reference< XMultiServiceFactory >&  xFactory    ,
																		const	uno::Reference< XFrame >&				xOwner		)
{
	// Set default return value.
	sal_Bool bOK = sal_True;
	// Check parameter.
	if	(
			( &xFactory		==	NULL		)	||
			( &xOwner		==	NULL		)	||
			( xFactory.is()	==	sal_False	)	||
			( xOwner.is()	==	sal_False	)
		)
	{
		bOK = sal_False ;
	}
	// Return result of check.
	return bOK ;
}

//*****************************************************************************************************************
// We need a valid URL. What is meaning with "register for nothing"?!
// xControl must correct to - nobody can advised otherwise!
sal_Bool MenuDispatcher::impldbg_checkParameter_addStatusListener( const   uno::Reference< XStatusListener >&   xControl,
																		const	URL&							aURL	)
{
	// Set default return value.
	sal_Bool bOK = sal_True;
	// Check parameter.
	if	(
			( &xControl					==	NULL	)	||
			( &aURL						==	NULL	)	||
			( aURL.Complete.getLength()	<	1		)
		)
	{
		bOK = sal_False ;
	}
	// Return result of check.
	return bOK ;
}

//*****************************************************************************************************************
// The same goes for these case! We have added valid listener for correct URL only.
// We can't remove invalid listener for nothing!
sal_Bool MenuDispatcher::impldbg_checkParameter_removeStatusListener(  const   uno::Reference< XStatusListener >&   xControl,
																			const	URL&							aURL	)
{
	// Set default return value.
	sal_Bool bOK = sal_True;
	// Check parameter.
	if	(
			( &xControl					==	NULL	)	||
			( &aURL						==	NULL	)	||
			( aURL.Complete.getLength()	<	1		)
		)
	{
		bOK = sal_False ;
	}
	// Return result of check.
	return bOK ;
}

#endif	//	#ifdef ENABLE_ASSERTIONS

}		//	namespace framework
