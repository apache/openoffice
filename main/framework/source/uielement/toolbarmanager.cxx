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
#include <uielement/toolbarmanager.hxx>

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________


#ifndef __FRAMEWORK_UIELEMENT_TOOLBAR_HXX
#include <uielement/toolbar.hxx>
#endif
#ifndef __FRAMEWORK_UIELEMENT_GENERICTOOLBARCONTROLLER_HXX
#include <uielement/generictoolbarcontroller.hxx>
#endif
#include <threadhelp/resetableguard.hxx>
#include "services.h"
#include "general.h"
#include "properties.h"
#include <framework/imageproducer.hxx>
#include <framework/sfxhelperfunctions.hxx>
#include <classes/fwkresid.hxx>
#ifndef __FRAMEWORK_CLASES_RESOURCE_HRC_
#include <classes/resource.hrc>
#endif
#include <framework/addonsoptions.hxx>
#include <uielement/toolbarmerger.hxx>
#include <framework/acceleratorinfo.hxx>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/ui/ItemType.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/awt/XDockableWindow.hpp>
#include <com/sun/star/frame/XLayoutManager.hpp>
#ifndef _COM_SUN_STAR_UI_XDOCKINGAREA_HPP_
#include <com/sun/star/ui/DockingArea.hpp>
#endif
#include <com/sun/star/graphic/XGraphic.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/frame/XModuleManager.hpp>
#include <com/sun/star/frame/ToolbarControllerFactory.hpp>
#include <com/sun/star/ui/XUIElementSettings.hpp>
#include <com/sun/star/ui/XUIConfigurationPersistence.hpp>
#include <com/sun/star/ui/XModuleUIConfigurationManagerSupplier.hpp>
#include <com/sun/star/ui/XUIConfigurationManagerSupplier.hpp>
#include <com/sun/star/ui/ImageType.hpp>
#include <com/sun/star/ui/UIElementType.hpp>
#include <comphelper/sequence.hxx>
#include <com/sun/star/frame/status/Visibility.hpp>
#include <com/sun/star/lang/DisposedException.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <svtools/imgdef.hxx>
#include <svtools/toolboxcontroller.hxx>
#include <unotools/cmdoptions.hxx>
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/helper/vclunohelper.hxx>
#endif
#include <comphelper/mediadescriptor.hxx>
#include <comphelper/processfactory.hxx>
#include <svtools/miscopt.hxx>
#include <svl/imageitm.hxx>
#include <svtools/framestatuslistener.hxx>
#include <vcl/svapp.hxx>
#include <vcl/menu.hxx>
#include <vcl/syswin.hxx>
#include <vcl/taskpanelist.hxx>
#include <rtl/logfile.hxx>
#include <svtools/menuoptions.hxx>
#include <unotools/cmdoptions.hxx>
#include <boost/bind.hpp>
#include <svtools/acceleratorexecute.hxx>

//_________________________________________________________________________________________________________________
//	namespaces
//_________________________________________________________________________________________________________________

using rtl::OUString;

using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::graphic;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::ui;
using namespace ::com::sun::star;

namespace framework
{

static const char ITEM_DESCRIPTOR_COMMANDURL[]	= "CommandURL";
static const char ITEM_DESCRIPTOR_HELPURL[]		= "HelpURL";
static const char ITEM_DESCRIPTOR_TOOLTIP[]		= "Tooltip";
static const char ITEM_DESCRIPTOR_CONTAINER[]	= "ItemDescriptorContainer";
static const char ITEM_DESCRIPTOR_LABEL[]		= "Label";
static const char ITEM_DESCRIPTOR_TYPE[]		= "Type";
static const char ITEM_DESCRIPTOR_VISIBLE[]		= "IsVisible";
static const char ITEM_DESCRIPTOR_WIDTH[]		= "Width";
static const char ITEM_DESCRIPTOR_STYLE[]		= "Style";

static const sal_Int32 ITEM_DESCRIPTOR_COMMANDURL_LEN	= 10;
static const sal_Int32 ITEM_DESCRIPTOR_HELPURL_LEN		= 7;
static const sal_Int32 ITEM_DESCRIPTOR_TOOLTIP_LEN		= 7;
static const sal_Int32 ITEM_DESCRIPTOR_CONTAINER_LEN	= 23;
static const sal_Int32 ITEM_DESCRIPTOR_LABEL_LEN		= 5;
static const sal_Int32 ITEM_DESCRIPTOR_TYPE_LEN			= 4;
static const sal_Int32 ITEM_DESCRIPTOR_VISIBLE_LEN		= 9;
static const sal_Int32 ITEM_DESCRIPTOR_WIDTH_LEN		= 5;
static const sal_Int32 ITEM_DESCRIPTOR_STYLE_LEN		= 5;

static const char	HELPID_PREFIX[]					= "helpid:";
static const char	HELPID_PREFIX_TESTTOOL[]		= ".HelpId:";
//static sal_Int32	HELPID_PREFIX_LENGTH			= 7;
static const sal_uInt16 STARTID_CUSTOMIZE_POPUPMENU	= 1000;

#define MENUPREFIX "private:resource/menubar/"

class ImageOrientationListener : public svt::FrameStatusListener
{
	public:
		ImageOrientationListener( const Reference< XStatusListener > rReceiver,
								  const Reference< XMultiServiceFactory > rServiceManager,
								  const Reference< XFrame > rFrame );
		virtual ~ImageOrientationListener();

		virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event ) throw ( ::com::sun::star::uno::RuntimeException );

	private:
		Reference< XStatusListener > m_xReceiver;
};

ImageOrientationListener::ImageOrientationListener(
	const Reference< XStatusListener > rReceiver,
	const Reference< XMultiServiceFactory > rServiceManager,
	const Reference< XFrame > rFrame ) :
	FrameStatusListener( rServiceManager, rFrame ),
	m_xReceiver( rReceiver )
{
}

ImageOrientationListener::~ImageOrientationListener()
{
}

void SAL_CALL ImageOrientationListener::statusChanged( const FeatureStateEvent& Event )
throw ( RuntimeException )
{
	if ( m_xReceiver.is() )
		m_xReceiver->statusChanged( Event );
}

//*****************************************************************************************************************

static sal_Int16 getImageTypeFromBools( sal_Bool bBig, sal_Bool bHighContrast )
{
	sal_Int16 n( 0 );
	if ( bBig )
		n |= ::com::sun::star::ui::ImageType::SIZE_LARGE;
	if ( bHighContrast )
		n |= ::com::sun::star::ui::ImageType::COLOR_HIGHCONTRAST;
	return n;
}

static ::com::sun::star::uno::Reference< ::com::sun::star::frame::XLayoutManager > getLayoutManagerFromFrame(
	::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame )
{
	::com::sun::star::uno::Reference< ::com::sun::star::frame::XLayoutManager > xLayoutManager;

	Reference< XPropertySet > xPropSet( rFrame, UNO_QUERY );
	if ( xPropSet.is() )
	{
		try
		{
			xPropSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "LayoutManager" ))) >>= xLayoutManager;
		}
		catch ( RuntimeException& )
		{
			throw;
		}
		catch ( Exception& )
		{
		}
	}

	return xLayoutManager;
}

//*****************************************************************************************************************
//	XInterface, XTypeProvider, XServiceInfo
//*****************************************************************************************************************
DEFINE_XINTERFACE_6		(   ToolBarManager																								 ,
							OWeakObject																									 ,
							DIRECT_INTERFACE( ::com::sun::star::lang::XTypeProvider														),
							DIRECT_INTERFACE( ::com::sun::star::lang::XComponent														),
							DIRECT_INTERFACE( ::com::sun::star::frame::XFrameActionListener												),
							DIRECT_INTERFACE( ::com::sun::star::ui::XUIConfigurationListener											),
							DIRECT_INTERFACE( ::com::sun::star::frame::XStatusListener													),
							DERIVED_INTERFACE( ::com::sun::star::lang::XEventListener, ::com::sun::star::frame::XFrameActionListener	)
						)

DEFINE_XTYPEPROVIDER_6	(   ToolBarManager									,
							::com::sun::star::lang::XTypeProvider			,
							::com::sun::star::lang::XComponent				,
							::com::sun::star::ui::XUIConfigurationListener	,
							::com::sun::star::frame::XFrameActionListener	,
							::com::sun::star::frame::XStatusListener		,
							::com::sun::star::lang::XEventListener
						)

ToolBarManager::ToolBarManager( const Reference< XMultiServiceFactory >& rServiceManager,
								const Reference< XFrame >& rFrame,
								const rtl::OUString& rResourceName,
								ToolBar* pToolBar ) :
	ThreadHelpBase( &Application::GetSolarMutex() ),
	OWeakObject(),
	m_bDisposed( sal_False ),
	m_bIsHiContrast( pToolBar->GetSettings().GetStyleSettings().GetHighContrastMode() ),
	m_bSmallSymbols( !SvtMiscOptions().AreCurrentSymbolsLarge() ),
	m_bModuleIdentified( sal_False ),
	m_bAddedToTaskPaneList( sal_True ),
	m_bFrameActionRegistered( sal_False ),
	m_bUpdateControllers( sal_False ),
	m_bImageOrientationRegistered( sal_False ),
	m_bImageMirrored( sal_False ),
	m_bCanBeCustomized( sal_True ),
	m_lImageRotation( 0 ),
	m_pToolBar( pToolBar ),
	m_aResourceName( rResourceName ),
	m_xFrame( rFrame ),
	m_aListenerContainer( m_aLock.getShareableOslMutex() ),
	m_xServiceManager( rServiceManager ),
	m_nSymbolsStyle( SvtMiscOptions().GetCurrentSymbolsStyle() ),
	m_bAcceleratorCfg( sal_False )
{
	Window* pWindow = m_pToolBar;
	while ( pWindow && !pWindow->IsSystemWindow() )
		pWindow = pWindow->GetParent();

	if ( pWindow )
		((SystemWindow *)pWindow)->GetTaskPaneList()->AddWindow( m_pToolBar );

	Reference< XComponentContext > xContext(::comphelper::getProcessComponentContext());
	if ( xContext.is() )
	{
		m_xToolbarControllerFactory = frame::ToolbarControllerFactory::create( xContext );

		m_xURLTransformer.set( xContext->getServiceManager()->createInstanceWithContext(
			SERVICENAME_URLTRANSFORMER, xContext), UNO_QUERY );
	}

	m_pToolBar->SetSelectHdl( LINK( this, ToolBarManager, Select) );
	m_pToolBar->SetActivateHdl( LINK( this, ToolBarManager, Activate) );
	m_pToolBar->SetDeactivateHdl( LINK( this, ToolBarManager, Deactivate) );
	m_pToolBar->SetClickHdl( LINK( this, ToolBarManager, Click ) );
	m_pToolBar->SetDropdownClickHdl( LINK( this, ToolBarManager, DropdownClick ) );
	m_pToolBar->SetDoubleClickHdl( LINK( this, ToolBarManager, DoubleClick ) );
	m_pToolBar->SetStateChangedHdl( LINK( this, ToolBarManager, StateChanged ) );
	m_pToolBar->SetDataChangedHdl( LINK( this, ToolBarManager, DataChanged ) );
	m_pToolBar->SetToolboxButtonSize( m_bSmallSymbols ? TOOLBOX_BUTTONSIZE_SMALL : TOOLBOX_BUTTONSIZE_LARGE );

	// enables a menu for clipped items and customization
	SvtCommandOptions aCmdOptions;
	sal_uInt16 nMenuType = TOOLBOX_MENUTYPE_CLIPPEDITEMS;
	if ( !aCmdOptions.Lookup( SvtCommandOptions::CMDOPTION_DISABLED, ::rtl::OUString::createFromAscii( "CreateDialog" )))
		 nMenuType |= TOOLBOX_MENUTYPE_CUSTOMIZE;
	//added for issue33668 by shizhoubo
	m_pToolBar->SetCommandHdl( LINK( this, ToolBarManager, Command ) );
	//end
	m_pToolBar->SetMenuType( nMenuType );
	m_pToolBar->SetMenuButtonHdl( LINK( this, ToolBarManager, MenuButton ) );
	m_pToolBar->GetMenu()->SetSelectHdl( LINK( this, ToolBarManager, MenuSelect ) );
	m_pToolBar->GetMenu()->SetDeactivateHdl( LINK( this, ToolBarManager, MenuDeactivate ) );

	// set name for testtool, the useful part is after the last '/'
	sal_Int32 idx = rResourceName.lastIndexOf('/');
	idx++; // will become 0 if '/' not found: use full string
	::rtl::OString aHelpIdAsString( HELPID_PREFIX_TESTTOOL );
	::rtl::OUString aToolbarName = rResourceName.copy( idx );
	aHelpIdAsString += rtl::OUStringToOString( aToolbarName, RTL_TEXTENCODING_UTF8 );;
	m_pToolBar->SetHelpId( aHelpIdAsString );

	m_aAsyncUpdateControllersTimer.SetTimeout( 50 );
	m_aAsyncUpdateControllersTimer.SetTimeoutHdl( LINK( this, ToolBarManager, AsyncUpdateControllersHdl ) );
}

ToolBarManager::~ToolBarManager()
{
	OSL_ASSERT( m_pToolBar == 0 );
	OSL_ASSERT( !m_bAddedToTaskPaneList );
}

void ToolBarManager::Destroy()
{
	ResetableGuard aGuard( m_aLock );
	if ( m_bAddedToTaskPaneList )
	{
		Window* pWindow = m_pToolBar;
		while ( pWindow && !pWindow->IsSystemWindow() )
			pWindow = pWindow->GetParent();

		if ( pWindow )
			((SystemWindow *)pWindow)->GetTaskPaneList()->RemoveWindow( m_pToolBar );
		m_bAddedToTaskPaneList = sal_False;
	}

	// Delete the additional add-ons data
	for ( sal_uInt16 i = 0; i < m_pToolBar->GetItemCount(); i++ )
	{
		sal_uInt16 nItemId = m_pToolBar->GetItemId( i );
		if ( nItemId > 0 )
			delete static_cast< AddonsParams* >( m_pToolBar->GetItemData( nItemId ));
	}

	// Hide toolbar as lazy delete can destroy the toolbar much later.
	m_pToolBar->Hide();
	/* #i99167# removed change for i93173 since there is some weird crash */
		// #i93173# delete toolbar lazily as we can still be in one of its handlers
	m_pToolBar->doLazyDelete();

	Link aEmpty;
	m_pToolBar->SetSelectHdl( aEmpty );
	m_pToolBar->SetActivateHdl( aEmpty );
	m_pToolBar->SetDeactivateHdl( aEmpty );
	m_pToolBar->SetClickHdl( aEmpty );
	m_pToolBar->SetDropdownClickHdl( aEmpty );
	m_pToolBar->SetDoubleClickHdl( aEmpty );
	m_pToolBar->SetStateChangedHdl( aEmpty );
	m_pToolBar->SetDataChangedHdl( aEmpty );

//	delete m_pToolBar;
	m_pToolBar = 0;
}

ToolBox* ToolBarManager::GetToolBar() const
{
	ResetableGuard aGuard( m_aLock );
	return m_pToolBar;
}

void ToolBarManager::CheckAndUpdateImages()
{
	ResetableGuard aGuard( m_aLock );
	sal_Bool bRefreshImages = sal_False;

	// Check if high contrast/normal mode have changed
	if ( m_pToolBar->GetSettings().GetStyleSettings().GetHighContrastMode() )
	{
		if ( !m_bIsHiContrast )
		{
			bRefreshImages = sal_True;
			m_bIsHiContrast = sal_True;
		}
	}
	else if ( m_bIsHiContrast )
	{
		bRefreshImages = sal_True;
		m_bIsHiContrast = sal_False;
	}

	SvtMiscOptions aMiscOptions;
	bool bCurrentSymbolsSmall = !aMiscOptions.AreCurrentSymbolsLarge();
	if ( m_bSmallSymbols != bCurrentSymbolsSmall )
	{
		bRefreshImages = sal_True;
		m_bSmallSymbols = bCurrentSymbolsSmall;
	}

	sal_Int16 nCurrentSymbolsStyle = aMiscOptions.GetCurrentSymbolsStyle();
	if ( m_nSymbolsStyle != nCurrentSymbolsStyle )
	{
		bRefreshImages = sal_True;
		m_nSymbolsStyle = nCurrentSymbolsStyle;
	}

	// Refresh images if requested
	if ( bRefreshImages )
		RefreshImages();
}

void ToolBarManager::RefreshImages()
{
	ResetableGuard aGuard( m_aLock );

	sal_Bool bBigImages( SvtMiscOptions().AreCurrentSymbolsLarge() );
	for ( sal_uInt16 nPos = 0; nPos < m_pToolBar->GetItemCount(); nPos++ )
	{
		sal_uInt16 nId( m_pToolBar->GetItemId( nPos ) );

		if ( nId > 0 )
		{
			::rtl::OUString aCommandURL = m_pToolBar->GetItemCommand( nId );
			Image aImage = GetImageFromURL( m_xFrame, aCommandURL, bBigImages, m_bIsHiContrast );
			// Try also to query for add-on images before giving up and use an
			// empty image.
			if ( !aImage )
				aImage = QueryAddonsImage( aCommandURL, bBigImages, m_bIsHiContrast );
			m_pToolBar->SetItemImage( nId, aImage );
		}
	}

	m_pToolBar->SetToolboxButtonSize( bBigImages ? TOOLBOX_BUTTONSIZE_LARGE : TOOLBOX_BUTTONSIZE_SMALL );
	::Size aSize = m_pToolBar->CalcWindowSizePixel();
	m_pToolBar->SetOutputSizePixel( aSize );
}

void ToolBarManager::UpdateImageOrientation()
{
	ResetableGuard aGuard( m_aLock );

	if ( m_xUICommandLabels.is() )
	{
		sal_Int32 i;
		Sequence< rtl::OUString > aSeqMirrorCmd;
		Sequence< rtl::OUString > aSeqRotateCmd;
		m_xUICommandLabels->getByName(
			rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( UICOMMANDDESCRIPTION_NAMEACCESS_COMMANDMIRRORIMAGELIST ))) >>= aSeqMirrorCmd;
		m_xUICommandLabels->getByName(
			rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( UICOMMANDDESCRIPTION_NAMEACCESS_COMMANDROTATEIMAGELIST ))) >>= aSeqRotateCmd;

		CommandToInfoMap::iterator pIter;
		for ( i = 0; i < aSeqMirrorCmd.getLength(); i++ )
		{
			rtl::OUString aMirrorCmd = aSeqMirrorCmd[i];
			pIter = m_aCommandMap.find( aMirrorCmd );
			if ( pIter != m_aCommandMap.end() )
				pIter->second.bMirrored = sal_True;
		}
		for ( i = 0; i < aSeqRotateCmd.getLength(); i++ )
		{
			rtl::OUString aRotateCmd = aSeqRotateCmd[i];
			pIter = m_aCommandMap.find( aRotateCmd );
			if ( pIter != m_aCommandMap.end() )
				pIter->second.bRotated = sal_True;
		}
	}

	for ( sal_uInt16 nPos = 0; nPos < m_pToolBar->GetItemCount(); nPos++ )
	{
		sal_uInt16 nId = m_pToolBar->GetItemId( nPos );
		if ( nId > 0 )
		{
			rtl::OUString aCmd = m_pToolBar->GetItemCommand( nId );

			CommandToInfoMap::const_iterator pIter = m_aCommandMap.find( aCmd );
			if ( pIter != m_aCommandMap.end() )
			{
				if ( pIter->second.bRotated )
				{
					m_pToolBar->SetItemImageMirrorMode( nId, sal_False );
					m_pToolBar->SetItemImageAngle( nId, m_lImageRotation );
				}
				if ( pIter->second.bMirrored )
					m_pToolBar->SetItemImageMirrorMode( nId, m_bImageMirrored );
			}
		}
	}
}

void ToolBarManager::UpdateControllers()
{
	RTL_LOGFILE_CONTEXT( aLog, "framework (cd100003) ::ToolBarManager::UpdateControllers" );

	if ( !m_bUpdateControllers )
	{
		m_bUpdateControllers = sal_True;
		ToolBarControllerMap::iterator pIter = m_aControllerMap.begin();

		while ( pIter != m_aControllerMap.end() )
		{
			try
			{
				Reference< XUpdatable > xUpdatable( pIter->second, UNO_QUERY );
				if ( xUpdatable.is() )
					xUpdatable->update();
			}
			catch ( Exception& )
			{
			}
			++pIter;
		}
	}
	m_bUpdateControllers = sal_False;
}
//for update toolbar controller via Support Visible by shizhoubo
void ToolBarManager::UpdateController( ::com::sun::star::uno::Reference< ::com::sun::star::frame::XToolbarController > xController)
{
	RTL_LOGFILE_CONTEXT( aLog, "framework (cd100003) ::ToolBarManager::UpdateControllers" );

	if ( !m_bUpdateControllers )
	{
		m_bUpdateControllers = sal_True;
		try
		{   if(xController.is())
			{
				Reference< XUpdatable > xUpdatable( xController, UNO_QUERY );
				if ( xUpdatable.is() )
					xUpdatable->update();
			}
		}
		catch ( Exception& )
		{
		}

		/* m_bUpdateControllers = sal_True;
		ToolBarControllerMap::iterator pIter = m_aControllerMap.begin();

		while ( pIter != m_aControllerMap.end() )
		{
			try
			{
				Reference< XUpdatable > xUpdatable( pIter->second, UNO_QUERY );
				if ( xUpdatable.is() )
					xUpdatable->update();
			}
			catch ( Exception& )
			{
			}
			++pIter;
		}*/

	}
	m_bUpdateControllers = sal_False;
}
//end
void ToolBarManager::frameAction( const FrameActionEvent& Action )
throw ( RuntimeException )
{
	ResetableGuard aGuard( m_aLock );
	if ( Action.Action == FrameAction_CONTEXT_CHANGED )
		 m_aAsyncUpdateControllersTimer.Start();
}

void SAL_CALL ToolBarManager::statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event )
throw ( ::com::sun::star::uno::RuntimeException )
{
	ResetableGuard aGuard( m_aLock );
	if ( m_bDisposed )
		return;

	if ( Event.FeatureURL.Complete.equalsAscii( ".uno:ImageOrientation" ))
	{
		SfxImageItem aItem( 1, 0 );
		aItem.PutValue( Event.State );

		m_lImageRotation = aItem.GetRotation();
		m_bImageMirrored = aItem.IsMirrored();
		UpdateImageOrientation();
	}
}

void SAL_CALL ToolBarManager::disposing( const EventObject& Source ) throw ( RuntimeException )
{
	{
		ResetableGuard aGuard( m_aLock );
		if ( m_bDisposed )
			return;
	}

	RemoveControllers();

	{
		ResetableGuard aGuard( m_aLock );
		if ( m_xDocImageManager.is() )
		{
			try
			{
				m_xDocImageManager->removeConfigurationListener(
					Reference< XUIConfigurationListener >(
						static_cast< OWeakObject* >( this ), UNO_QUERY ));
			}
			catch ( Exception& )
			{
			}
		}

		if ( m_xModuleImageManager.is() )
		{
			try
			{
				m_xModuleImageManager->removeConfigurationListener(
					Reference< XUIConfigurationListener >(
						static_cast< OWeakObject* >( this ), UNO_QUERY ));
			}
			catch ( Exception& )
			{
			}
		}

		if ( m_xImageOrientationListener.is() )
		{
			ImageOrientationListener* pImageOrientation =
				(ImageOrientationListener*)m_xImageOrientationListener.get();
			pImageOrientation->unbindListener();
			m_xImageOrientationListener.clear();
		}

		m_xDocImageManager.clear();
		m_xModuleImageManager.clear();

		if ( Source.Source == Reference< XInterface >( m_xFrame, UNO_QUERY ))
			m_xFrame.clear();

		m_xServiceManager.clear();
	}
}

// XComponent
void SAL_CALL ToolBarManager::dispose() throw( RuntimeException )
{
	Reference< XComponent > xThis( static_cast< OWeakObject* >(this), UNO_QUERY );

	EventObject aEvent( xThis );
	m_aListenerContainer.disposeAndClear( aEvent );

	{
		ResetableGuard aGuard( m_aLock );

		// stop timer to prevent timer events after dispose
		m_aAsyncUpdateControllersTimer.Stop();

		RemoveControllers();

		if ( m_xDocImageManager.is() )
		{
			try
			{
				m_xDocImageManager->removeConfigurationListener(
					Reference< XUIConfigurationListener >(
						static_cast< OWeakObject* >( this ), UNO_QUERY ));
			}
			catch ( Exception& )
			{
			}
		}
		m_xDocImageManager.clear();
		if ( m_xModuleImageManager.is() )
		{
			try
			{
				m_xModuleImageManager->removeConfigurationListener(
					Reference< XUIConfigurationListener >(
						static_cast< OWeakObject* >( this ), UNO_QUERY ));
			}
			catch ( Exception& )
			{
			}
		}
		m_xModuleImageManager.clear();

		ImplClearPopupMenu( m_pToolBar );

		// We have to destroy our toolbar instance now.
		Destroy();

		if ( m_bFrameActionRegistered && m_xFrame.is() )
		{
			try
			{
				m_xFrame->removeFrameActionListener( Reference< XFrameActionListener >(
														static_cast< ::cppu::OWeakObject *>( this ), UNO_QUERY ));
			}
			catch ( Exception& )
			{
			}
		}

		if ( m_xImageOrientationListener.is() )
		{
			ImageOrientationListener* pImageOrientation =
				(ImageOrientationListener*)m_xImageOrientationListener.get();
			pImageOrientation->unbindListener();
			m_xImageOrientationListener.clear();
		}

		m_xFrame.clear();
		m_xServiceManager.clear();
		Reference< XComponent > xCompGAM( m_xGlobalAcceleratorManager, UNO_QUERY );
		if ( xCompGAM.is() )
			 xCompGAM->dispose();
		m_xGlobalAcceleratorManager.clear();
		m_xModuleAcceleratorManager.clear();
		m_xDocAcceleratorManager.clear();

		m_bDisposed = sal_True;
	}
}

void SAL_CALL ToolBarManager::addEventListener( const Reference< XEventListener >& xListener ) throw( RuntimeException )
{
	ResetableGuard aGuard( m_aLock );

	/* SAFE AREA ----------------------------------------------------------------------------------------------- */
	if ( m_bDisposed )
		throw DisposedException();

	m_aListenerContainer.addInterface( ::getCppuType( ( const Reference< XEventListener >* ) NULL ), xListener );
}

void SAL_CALL ToolBarManager::removeEventListener( const Reference< XEventListener >& xListener ) throw( RuntimeException )
{
	m_aListenerContainer.removeInterface( ::getCppuType( ( const Reference< XEventListener >* ) NULL ), xListener );
}

// XUIConfigurationListener
void SAL_CALL ToolBarManager::elementInserted( const ::com::sun::star::ui::ConfigurationEvent& Event ) throw (::com::sun::star::uno::RuntimeException)
{
	impl_elementChanged(false,Event);
}

void SAL_CALL ToolBarManager::elementRemoved( const ::com::sun::star::ui::ConfigurationEvent& Event ) throw (::com::sun::star::uno::RuntimeException)
{
	impl_elementChanged(true,Event);
}
void ToolBarManager::impl_elementChanged(bool _bRemove,const ::com::sun::star::ui::ConfigurationEvent& Event )
{
	ResetableGuard aGuard( m_aLock );

	/* SAFE AREA ----------------------------------------------------------------------------------------------- */
	if ( m_bDisposed )
		return;

	Reference< XNameAccess >	xNameAccess;
	sal_Int16					nImageType = sal_Int16();
	sal_Int16					nCurrentImageType = getImageTypeFromBools(
														SvtMiscOptions().AreCurrentSymbolsLarge(),
														m_bIsHiContrast );

	if (( Event.aInfo >>= nImageType ) &&
		( nImageType == nCurrentImageType ) &&
		( Event.Element >>= xNameAccess ))
	{
		sal_Int16 nImageInfo( 1 );
		Reference< XInterface > xIfacDocImgMgr( m_xDocImageManager, UNO_QUERY );
		if ( xIfacDocImgMgr == Event.Source )
			 nImageInfo = 0;

		Sequence< rtl::OUString > aSeq = xNameAccess->getElementNames();
		for ( sal_Int32 i = 0; i < aSeq.getLength(); i++ )
		{
			CommandToInfoMap::iterator pIter = m_aCommandMap.find( aSeq[i] );
			if ( pIter != m_aCommandMap.end() && ( pIter->second.nImageInfo >= nImageInfo ))
			{
				if ( _bRemove )
				{
					Image aImage;
					if (( pIter->second.nImageInfo == 0 ) && ( pIter->second.nImageInfo == nImageInfo ))
					{
						// Special case: An image from the document image manager has been removed.
						// It is possible that we have a image at our module image manager. Before
						// we can remove our image we have to ask our module image manager.
						Sequence< rtl::OUString > aCmdURLSeq( 1 );
						Sequence< Reference< XGraphic > > aGraphicSeq;
						aCmdURLSeq[0] = pIter->first;
						aGraphicSeq = m_xModuleImageManager->getImages( nImageType, aCmdURLSeq );
						aImage = Image( aGraphicSeq[0] );
					}

					setToolBarImage(aImage,pIter);
				} // if ( _bRemove )
				else
				{
					Reference< XGraphic > xGraphic;
					if ( xNameAccess->getByName( aSeq[i] ) >>= xGraphic )
					{
						Image aImage( xGraphic );
						setToolBarImage(aImage,pIter);
					}
					pIter->second.nImageInfo = nImageInfo;
				}
			}
		}
	}
}
void ToolBarManager::setToolBarImage(const Image& _aImage,const CommandToInfoMap::const_iterator& _pIter)
{
	const ::std::vector< sal_uInt16 >& _rIDs = _pIter->second.aIds;
	m_pToolBar->SetItemImage( _pIter->second.nId, _aImage );
	::std::for_each(_rIDs.begin(),_rIDs.end(),::boost::bind(&ToolBar::SetItemImage,m_pToolBar,_1,_aImage));
}

void SAL_CALL ToolBarManager::elementReplaced( const ::com::sun::star::ui::ConfigurationEvent& Event ) throw (::com::sun::star::uno::RuntimeException)
{
	impl_elementChanged(false,Event);
}

void ToolBarManager::RemoveControllers()
{
	ResetableGuard aGuard( m_aLock );

	if ( m_bDisposed )
		return;

	m_aSubToolBarControllerMap.clear();


	// i90033
	// Remove item window pointers from the toolbar. They were
	// destroyed by the dispose() at the XComponent. This is needed
	// as VCL code later tries to access the item window data in certain
	// dtors where the item window is already invalid!
	for ( sal_uInt16 i = 0; i < m_pToolBar->GetItemCount(); i++ )
	{
		sal_uInt16 nItemId = m_pToolBar->GetItemId( i );
		if ( nItemId > 0 )
		{
			Reference< XComponent > xComponent( m_aControllerMap[ nItemId ], UNO_QUERY );
			if ( xComponent.is() )
			{
				try
				{
					xComponent->dispose();
				}
				catch ( Exception& )
				{
				}
			}
			m_pToolBar->SetItemWindow(nItemId, 0);
		}
	}
	m_aControllerMap.clear();
}

uno::Sequence< beans::PropertyValue > ToolBarManager::GetPropsForCommand( const ::rtl::OUString& rCmdURL )
{
	Sequence< PropertyValue > aPropSeq;

	// Retrieve properties for command
	try
	{
		if ( !m_bModuleIdentified )
		{
			Reference< XModuleManager > xModuleManager( m_xServiceManager->createInstance( SERVICENAME_MODULEMANAGER ), UNO_QUERY_THROW );
			Reference< XInterface > xIfac( m_xFrame, UNO_QUERY );

			m_bModuleIdentified = sal_True;
			m_aModuleIdentifier = xModuleManager->identify( xIfac );

			if ( m_aModuleIdentifier.getLength() > 0 )
			{
				Reference< XNameAccess > xNameAccess( m_xServiceManager->createInstance( SERVICENAME_UICOMMANDDESCRIPTION ), UNO_QUERY );
				if ( xNameAccess.is() )
					xNameAccess->getByName( m_aModuleIdentifier ) >>= m_xUICommandLabels;
			}
		}

		if ( m_xUICommandLabels.is() )
		{
			if ( rCmdURL.getLength() > 0 )
				 m_xUICommandLabels->getByName( rCmdURL ) >>= aPropSeq;
		}
	}
	catch ( Exception& )
	{
	}

	return aPropSeq;
}

::rtl::OUString ToolBarManager::RetrieveLabelFromCommand( const ::rtl::OUString& aCmdURL )
{
	::rtl::OUString aLabel;
	Sequence< PropertyValue > aPropSeq;

	// Retrieve popup menu labels
	aPropSeq = GetPropsForCommand( aCmdURL );
	for ( sal_Int32 i = 0; i < aPropSeq.getLength(); i++ )
	{
		if ( aPropSeq[i].Name.equalsAscii( "Name" ))
		{
			aPropSeq[i].Value >>= aLabel;
			break;
		}
	}
	return aLabel;
}

sal_Int32 ToolBarManager::RetrievePropertiesFromCommand( const ::rtl::OUString& aCmdURL )
{
	sal_Int32 nProperties(0);
	Sequence< PropertyValue > aPropSeq;

	// Retrieve popup menu labels
	aPropSeq = GetPropsForCommand( aCmdURL );
	for ( sal_Int32 i = 0; i < aPropSeq.getLength(); i++ )
	{
		if ( aPropSeq[i].Name.equalsAscii( "Properties" ))
		{
			aPropSeq[i].Value >>= nProperties;
			break;
		}
	}
	return nProperties;
}

void ToolBarManager::CreateControllers()
{
	RTL_LOGFILE_CONTEXT( aLog, "framework (cd100003) ::ToolBarManager::CreateControllers" );

	Reference< XComponentContext > xComponentContext;
	Reference< XPropertySet > xProps( m_xServiceManager, UNO_QUERY );
	Reference< XWindow > xToolbarWindow = VCLUnoHelper::GetInterface( m_pToolBar );

	css::util::URL		aURL;
	sal_Bool			bHasDisabledEntries = SvtCommandOptions().HasEntries( SvtCommandOptions::CMDOPTION_DISABLED );
	SvtCommandOptions	aCmdOptions;

	if ( xProps.is() )
		xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DefaultContext" ))) >>= xComponentContext;

	for ( sal_uInt16 i = 0; i < m_pToolBar->GetItemCount(); i++ )
	{
		sal_uInt16 nId = m_pToolBar->GetItemId( i );
		if ( nId == 0 )
			continue;

		rtl::OUString					aLoadURL( RTL_CONSTASCII_USTRINGPARAM( ".uno:OpenUrl" ));
		rtl::OUString					aCommandURL( m_pToolBar->GetItemCommand( nId ));
		sal_Bool						bInit( sal_True );
		sal_Bool						bCreate( sal_True );
		Reference< XStatusListener >	xController;
		CommandToInfoMap::iterator pCommandIter = m_aCommandMap.find( aCommandURL );
		sal_Int16 nWidth = ( pCommandIter != m_aCommandMap.end() ? pCommandIter->second.nWidth : 0 );

		svt::ToolboxController* pController( 0 );

		if ( bHasDisabledEntries )
		{
			aURL.Complete = aCommandURL;
			m_xURLTransformer->parseStrict( aURL );
			if ( aCmdOptions.Lookup( SvtCommandOptions::CMDOPTION_DISABLED, aURL.Path ))
			{
				m_aControllerMap[ nId ] = xController;
				m_pToolBar->HideItem( nId );
				continue;
			}
		}

		if ( m_xToolbarControllerFactory.is() &&
			m_xToolbarControllerFactory->hasController( aCommandURL, m_aModuleIdentifier ))
		{
			PropertyValue aPropValue;
			std::vector< Any > aPropertyVector;

			aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ModuleIdentifier" ));
			aPropValue.Value <<= m_aModuleIdentifier;
			aPropertyVector.push_back( makeAny( aPropValue ));
			aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Frame" ));
			aPropValue.Value <<= m_xFrame;
			aPropertyVector.push_back( makeAny( aPropValue ));
			aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ServiceManager" ));
			aPropValue.Value <<= m_xServiceManager;
			aPropertyVector.push_back( makeAny( aPropValue ));
			aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ParentWindow" ));
			aPropValue.Value <<= xToolbarWindow;
			aPropertyVector.push_back( makeAny( aPropValue ));
			aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Identifier" ));
			aPropValue.Value = uno::makeAny( nId );
			aPropertyVector.push_back( uno::makeAny( aPropValue ) );

			if ( nWidth > 0 )
			{
				aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Width" ));
				aPropValue.Value <<= nWidth;
				aPropertyVector.push_back( makeAny( aPropValue ));
			}

			Sequence< Any > aArgs( comphelper::containerToSequence( aPropertyVector ));
			xController = Reference< XStatusListener >( m_xToolbarControllerFactory->createInstanceWithArgumentsAndContext(
															aCommandURL, aArgs, xComponentContext ),
														UNO_QUERY );
			bInit = sal_False; // Initialization is done through the factory service
		}

		if (( aCommandURL == aLoadURL ) && ( !m_pToolBar->IsItemVisible(nId)))
			bCreate = sal_False;

		if ( !xController.is() && m_pToolBar && bCreate )
		{
			pController = CreateToolBoxController( m_xFrame, m_pToolBar, nId, aCommandURL );
			if ( !pController )
			{
				if ( m_pToolBar->GetItemData( nId ) != 0 )
				{
					// retrieve additional parameters
					::rtl::OUString aControlType = static_cast< AddonsParams* >( m_pToolBar->GetItemData( nId ))->aControlType;

					Reference< XStatusListener > xStatusListener(
						ToolBarMerger::CreateController( m_xServiceManager,
														 m_xFrame,
														 m_pToolBar,
														 aCommandURL,
														 nId,
														 nWidth,
														 aControlType ), UNO_QUERY );

					xController = xStatusListener;
				}
				else
				{
					MenuDescriptionMap::iterator it = m_aMenuMap.find( nId );
					if ( it == m_aMenuMap.end() )
					{
						xController = Reference< XStatusListener >(
							new GenericToolbarController( m_xServiceManager, m_xFrame, m_pToolBar, nId, aCommandURL ));

						// Accessibility support: Set toggle button role for specific commands
						sal_Int32 nProps = RetrievePropertiesFromCommand( aCommandURL );
						if ( nProps & UICOMMANDDESCRIPTION_PROPERTIES_TOGGLEBUTTON )
							m_pToolBar->SetItemBits( nId, m_pToolBar->GetItemBits( nId ) | TIB_CHECKABLE );
					}
					else
						xController = Reference< XStatusListener >(
							new MenuToolbarController( m_xServiceManager, m_xFrame, m_pToolBar, nId, aCommandURL, m_aModuleIdentifier, m_aMenuMap[ nId ] ));
				}
			}
			else if ( pController )
			{
				xController = Reference< XStatusListener >( static_cast< ::cppu::OWeakObject *>( pController ), UNO_QUERY );
			}
		}

		// Associate ID and controller to be able to retrieve
		// the controller from the ID later.
		m_aControllerMap[ nId ] = xController;

		// Fill sub-toolbars into our hash-map
		Reference< XSubToolbarController > xSubToolBar( xController, UNO_QUERY );
		if ( xSubToolBar.is() && xSubToolBar->opensSubToolbar() )
		{
			rtl::OUString aSubToolBarName = xSubToolBar->getSubToolbarName();
			if ( aSubToolBarName.getLength() != 0 )
			{
				SubToolBarToSubToolBarControllerMap::iterator pIter =
					m_aSubToolBarControllerMap.find( aSubToolBarName );
				if ( pIter == m_aSubToolBarControllerMap.end() )
				{
					SubToolBarControllerVector aSubToolBarVector;
					aSubToolBarVector.push_back( xSubToolBar );
					m_aSubToolBarControllerMap.insert(
						SubToolBarToSubToolBarControllerMap::value_type(
							aSubToolBarName, aSubToolBarVector ));
				}
				else
					pIter->second.push_back( xSubToolBar );
			}
		}

		Reference< XInitialization > xInit( xController, UNO_QUERY );
		if ( xInit.is() )
		{
			if ( bInit )
			{
				PropertyValue aPropValue;
				std::vector< Any > aPropertyVector;

				aPropValue.Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Frame" ));
				aPropValue.Value <<= m_xFrame;
				aPropertyVector.push_back( makeAny( aPropValue ));
				aPropValue.Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "CommandURL" ));
				aPropValue.Value <<= aCommandURL;
				aPropertyVector.push_back( makeAny( aPropValue ));
				aPropValue.Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ServiceManager" ));
				aPropValue.Value <<= m_xServiceManager;
				aPropertyVector.push_back( makeAny( aPropValue ));
				aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ParentWindow" ));
				aPropValue.Value <<= xToolbarWindow;
				aPropertyVector.push_back( makeAny( aPropValue ));
				aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ModuleIdentifier" ));
				aPropValue.Value <<= m_aModuleIdentifier;
				aPropertyVector.push_back( makeAny( aPropValue ));
				aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Identifier" ));
				aPropValue.Value = uno::makeAny( nId );
				aPropertyVector.push_back( uno::makeAny( aPropValue ) );

				if ( nWidth > 0 )
				{
					aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Width" ));
					aPropValue.Value <<= nWidth;
					aPropertyVector.push_back( makeAny( aPropValue ));
				}

				Sequence< Any > aArgs( comphelper::containerToSequence( aPropertyVector ));
				xInit->initialize( aArgs );
				//for Support Visiblitly by shizhoubo
				if (pController)
				{
				//	rtl::OUString aCommandURL = pController->m_aCommandURL;
					if(aCommandURL == rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:SwitchXFormsDesignMode" )) ||
					   aCommandURL == rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:ViewDataSourceBrowser" )) ||
					   aCommandURL == rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:ParaLeftToRight" )) ||
					   aCommandURL == rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:ParaRightToLeft" ))
					   )
						pController->setFastPropertyValue_NoBroadcast(1,makeAny(sal_True));
				}

				//end
			}

			// Request a item window from the toolbar controller and set it at the VCL toolbar
			Reference< XToolbarController > xTbxController( xController, UNO_QUERY );
			if ( xTbxController.is() && xToolbarWindow.is() )
			{
				Reference< XWindow > xWindow = xTbxController->createItemWindow( xToolbarWindow );
				if ( xWindow.is() )
				{
					Window* pItemWin = VCLUnoHelper::GetWindow( xWindow );
					if ( pItemWin )
					{
						WindowType nType = pItemWin->GetType();
						if ( nType == WINDOW_LISTBOX || nType == WINDOW_MULTILISTBOX || nType == WINDOW_COMBOBOX )
							pItemWin->SetAccessibleName( m_pToolBar->GetItemText( nId ) );
						m_pToolBar->SetItemWindow( nId, pItemWin );
					}
				}
			}
		}
		// for update Controller via support visible state by shizhoubo
		Reference< XPropertySet > xPropSet( xController, UNO_QUERY );
		if ( xPropSet.is() )
		{
			try
			{
				sal_Bool bSupportVisiable = sal_True;
				Any a( xPropSet->getPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "SupportsVisiable" ))) );
				a >>= bSupportVisiable;
				if ( bSupportVisiable )
				{
					Reference< XToolbarController > xTbxController( xController, UNO_QUERY );
					UpdateController(xTbxController);
				}
			}
			catch ( RuntimeException& )
			{
				throw;
			}
			catch ( Exception& )
			{
			}
		}
		//end

	}

	AddFrameActionListener();
	AddImageOrientationListener();
}

void ToolBarManager::AddFrameActionListener()
{
	if ( !m_bFrameActionRegistered && m_xFrame.is() )
	{
		m_bFrameActionRegistered = sal_True;
		m_xFrame->addFrameActionListener( Reference< XFrameActionListener >(
										  static_cast< ::cppu::OWeakObject *>( this ), UNO_QUERY ));
	}
}

void ToolBarManager::AddImageOrientationListener()
{
	if ( !m_bImageOrientationRegistered && m_xFrame.is() )
	{
		m_bImageOrientationRegistered = sal_True;
		ImageOrientationListener* pImageOrientation = new ImageOrientationListener(
			Reference< XStatusListener >( static_cast< ::cppu::OWeakObject *>( this ), UNO_QUERY ),
			m_xServiceManager,
			m_xFrame );
		m_xImageOrientationListener = Reference< XComponent >( static_cast< ::cppu::OWeakObject *>(
									  pImageOrientation ), UNO_QUERY );
		pImageOrientation->addStatusListener(
			rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:ImageOrientation" )));
		pImageOrientation->bindListener();
	}
}

sal_uInt16 ToolBarManager::ConvertStyleToToolboxItemBits( sal_Int32 nStyle )
{
	sal_uInt16 nItemBits( 0 );
	if ( nStyle & ::com::sun::star::ui::ItemStyle::RADIO_CHECK )
		nItemBits |= TIB_RADIOCHECK;
	if ( nStyle & ::com::sun::star::ui::ItemStyle::ALIGN_LEFT )
		nItemBits |= TIB_LEFT;
	if ( nStyle & ::com::sun::star::ui::ItemStyle::AUTO_SIZE )
		nItemBits |= TIB_AUTOSIZE;
	if ( nStyle & ::com::sun::star::ui::ItemStyle::DROP_DOWN )
		nItemBits |= TIB_DROPDOWN;
	if ( nStyle & ::com::sun::star::ui::ItemStyle::REPEAT )
		nItemBits |= TIB_REPEAT;
	if ( nStyle & ::com::sun::star::ui::ItemStyle::DROPDOWN_ONLY )
		nItemBits |= TIB_DROPDOWNONLY;
	if ( nStyle & ::com::sun::star::ui::ItemStyle::TEXT )
		nItemBits |= TIB_TEXT_ONLY;
	if ( nStyle & ::com::sun::star::ui::ItemStyle::ICON )
		nItemBits |= TIB_ICON_ONLY;

	return nItemBits;
}

void ToolBarManager::FillToolbar( const Reference< XIndexAccess >& rItemContainer )
{
	::rtl::OString aTbxName = rtl::OUStringToOString( m_aResourceName, RTL_TEXTENCODING_ASCII_US );

	RTL_LOGFILE_CONTEXT( aLog, "framework (cd100003) ::ToolBarManager::FillToolbar" );
	RTL_LOGFILE_CONTEXT_TRACE1( aLog, "framework (cd100003) ::ToolBarManager::FillToolbar %s", aTbxName.getStr() );

	ResetableGuard aGuard( m_aLock );

	if ( m_bDisposed )
		return;

	sal_uInt16 nId( 1 );
	::rtl::OUString aHelpIdPrefix( RTL_CONSTASCII_USTRINGPARAM( HELPID_PREFIX ));

	Reference< XModuleManager > xModuleManager( Reference< XModuleManager >(
												m_xServiceManager->createInstance( SERVICENAME_MODULEMANAGER ), UNO_QUERY ));
	if ( !m_xDocImageManager.is() )
	{
		Reference< XModel > xModel( GetModelFromFrame() );
		if ( xModel.is() )
		{
			Reference< XUIConfigurationManagerSupplier > xSupplier( xModel, UNO_QUERY );
			if ( xSupplier.is() )
			{
				m_xDocUICfgMgr.set( xSupplier->getUIConfigurationManager(), UNO_QUERY );
				m_xDocImageManager = Reference< XImageManager >( m_xDocUICfgMgr->getImageManager(), UNO_QUERY );
				m_xDocImageManager->addConfigurationListener(
										Reference< XUIConfigurationListener >(
											static_cast< OWeakObject* >( this ), UNO_QUERY ));
			}
		}
	}

	try
	{
		if ( xModuleManager.is() )
			m_aModuleIdentifier = xModuleManager->identify( Reference< XInterface >( m_xFrame, UNO_QUERY ) );
	}
	catch( Exception& )
	{
	}

	if ( !m_xModuleImageManager.is() )
	{
		Reference< XModuleUIConfigurationManagerSupplier > xModuleCfgMgrSupplier( m_xServiceManager->createInstance(
																					SERVICENAME_MODULEUICONFIGURATIONMANAGERSUPPLIER ),
																				  UNO_QUERY );
		m_xUICfgMgr = xModuleCfgMgrSupplier->getUIConfigurationManager( m_aModuleIdentifier );
		m_xModuleImageManager = Reference< XImageManager >( m_xUICfgMgr->getImageManager(), UNO_QUERY );
		m_xModuleImageManager->addConfigurationListener( Reference< XUIConfigurationListener >(
	static_cast< OWeakObject* >( this ), UNO_QUERY ));
	}

	RemoveControllers();

	// reset and fill command map
	m_pToolBar->Clear();
	m_aControllerMap.clear();
	m_aCommandMap.clear();

	m_aMenuMap.clear();

	CommandInfo aCmdInfo;
	for ( sal_Int32 n = 0; n < rItemContainer->getCount(); n++ )
	{
		Sequence< PropertyValue >	aProp;
		rtl::OUString				aCommandURL;
		rtl::OUString				aLabel;
		rtl::OUString				aHelpURL;
		rtl::OUString				aTooltip;
		sal_uInt16					nType( ::com::sun::star::ui::ItemType::DEFAULT );
		sal_uInt16					nWidth( 0 );
		sal_Bool					bIsVisible( sal_True );
		sal_uInt32					nStyle( 0 );

		Reference< XIndexAccess >	aMenuDesc;
		try
		{
			if ( rItemContainer->getByIndex( n ) >>= aProp )
			{
				for ( int i = 0; i < aProp.getLength(); i++ )
				{
					if ( aProp[i].Name.equalsAsciiL( ITEM_DESCRIPTOR_COMMANDURL, ITEM_DESCRIPTOR_COMMANDURL_LEN ))
					{
						aProp[i].Value >>= aCommandURL;
						if ( aCommandURL.compareToAscii(MENUPREFIX, RTL_CONSTASCII_LENGTH(MENUPREFIX) ) == 0 )
						{
							try
							{
								Reference< XIndexAccess > xMenuContainer;
								if ( m_xDocUICfgMgr.is() &&
									m_xDocUICfgMgr->hasSettings( aCommandURL ) )
									xMenuContainer = m_xDocUICfgMgr->getSettings( aCommandURL, sal_False );
								if ( !xMenuContainer.is() &&
									m_xUICfgMgr.is() &&
									m_xUICfgMgr->hasSettings( aCommandURL ) )
									xMenuContainer = m_xUICfgMgr->getSettings( aCommandURL, sal_False );
								if ( xMenuContainer.is() && xMenuContainer->getCount() )
								{
									Sequence< PropertyValue > aProps;
									// drop down menu info is currently
									// the first (and only) menu
									// in the menusettings container
									xMenuContainer->getByIndex(0) >>= aProps;
									for ( sal_Int32 index=0; index<aProps.getLength(); ++index )
									{
										if ( aProps[ index ].Name.equalsAsciiL( ITEM_DESCRIPTOR_CONTAINER, ITEM_DESCRIPTOR_CONTAINER_LEN ))

										{
											aProps[ index ].Value >>= aMenuDesc;
											break;
										}
									}
								}
							}
							catch( Exception& )
							{
							}
						}
					}
					else if ( aProp[i].Name.equalsAsciiL( ITEM_DESCRIPTOR_HELPURL, ITEM_DESCRIPTOR_HELPURL_LEN ))
						aProp[i].Value >>= aHelpURL;
					else if ( aProp[i].Name.equalsAsciiL( ITEM_DESCRIPTOR_TOOLTIP, ITEM_DESCRIPTOR_TOOLTIP_LEN ))
						aProp[i].Value >>= aTooltip;
					else if ( aProp[i].Name.equalsAsciiL( ITEM_DESCRIPTOR_LABEL, ITEM_DESCRIPTOR_LABEL_LEN ))
						aProp[i].Value >>= aLabel;
					else if ( aProp[i].Name.equalsAsciiL( ITEM_DESCRIPTOR_TYPE, ITEM_DESCRIPTOR_TYPE_LEN ))
						aProp[i].Value >>= nType;
					else if ( aProp[i].Name.equalsAsciiL( ITEM_DESCRIPTOR_VISIBLE, ITEM_DESCRIPTOR_VISIBLE_LEN ))
						aProp[i].Value >>= bIsVisible;
					else if ( aProp[i].Name.equalsAsciiL( ITEM_DESCRIPTOR_WIDTH, ITEM_DESCRIPTOR_WIDTH_LEN ))
						aProp[i].Value >>= nWidth;
					else if ( aProp[i].Name.equalsAsciiL( ITEM_DESCRIPTOR_STYLE, ITEM_DESCRIPTOR_STYLE_LEN ))
						aProp[i].Value >>= nStyle;
				}

				if (( nType == ::com::sun::star::ui::ItemType::DEFAULT ) && ( aCommandURL.getLength() > 0 ))
				{
					::rtl::OUString aString( RetrieveLabelFromCommand( aCommandURL ));

					sal_uInt16 nItemBits = ConvertStyleToToolboxItemBits( nStyle );
					if ( aMenuDesc.is() )
					{
						m_aMenuMap[ nId ] = aMenuDesc;
						nItemBits |= TIB_DROPDOWNONLY;
					}
					m_pToolBar->InsertItem( nId, aString, nItemBits );
					m_pToolBar->SetItemCommand( nId, aCommandURL );
					if ( aTooltip.getLength() )
					{
						m_pToolBar->SetQuickHelpText( nId, aTooltip );
					}
					else
					{
 						::rtl::OUString sQuickHelp( aString );
 						::rtl::OUString sShortCut;
 						if( RetrieveShortcut( aCommandURL, sShortCut ) )
 						{
 							sQuickHelp += rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( " (" ) );
 							sQuickHelp += sShortCut;
 							sQuickHelp += rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ")" ) );
 						}

						m_pToolBar->SetQuickHelpText( nId, sQuickHelp );
					}

					if ( aLabel.getLength() > 0 )
					{
						m_pToolBar->SetItemText( nId, aLabel );
					}
					else
					{
						m_pToolBar->SetItemText( nId, aString );
					}
					m_pToolBar->EnableItem( nId, sal_True );
					m_pToolBar->SetItemState( nId, STATE_NOCHECK );

					// Fill command map. It stores all our commands and from what
					// image manager we got our image. So we can decide if we have to use an
					// image from a notification message.
					CommandToInfoMap::iterator pIter = m_aCommandMap.find( aCommandURL );
					if ( pIter == m_aCommandMap.end())
					{
						aCmdInfo.nId = nId;
						aCmdInfo.nWidth = nWidth;
						m_aCommandMap.insert( CommandToInfoMap::value_type( aCommandURL, aCmdInfo ));
					}
					else
					{
						pIter->second.aIds.push_back( nId );
					}

					if ( !bIsVisible )
						m_pToolBar->HideItem( nId );

					++nId;
				}
				else if ( nType == ::com::sun::star::ui::ItemType::SEPARATOR_LINE )
				{
					m_pToolBar->InsertSeparator();
				}
				else if ( nType == ::com::sun::star::ui::ItemType::SEPARATOR_SPACE )
				{
					m_pToolBar->InsertSpace();
				}
				else if ( nType == ::com::sun::star::ui::ItemType::SEPARATOR_LINEBREAK )
				{
					m_pToolBar->InsertBreak();
				}
			}
		}
		catch ( ::com::sun::star::lang::IndexOutOfBoundsException& )
		{
			break;
		}
	}

	// Support add-on toolbar merging here. Working directly on the toolbar object is much
	// simpler and faster.
	const sal_uInt16 TOOLBAR_ITEM_STARTID = 1000;

	MergeToolbarInstructionContainer aMergeInstructionContainer;

	// Retrieve the toolbar name from the resource name
	::rtl::OUString aToolbarName( m_aResourceName );
	sal_Int32 nIndex = aToolbarName.lastIndexOf( '/' );
	if (( nIndex > 0 ) && ( nIndex < aToolbarName.getLength() ))
		aToolbarName = aToolbarName.copy( nIndex+1 );

	AddonsOptions().GetMergeToolbarInstructions( aToolbarName, aMergeInstructionContainer );

	if ( !aMergeInstructionContainer.empty() )
	{
		sal_uInt16 nItemId( TOOLBAR_ITEM_STARTID );
		const sal_uInt32 nCount = aMergeInstructionContainer.size();
		for ( sal_uInt32 i=0; i < nCount; i++ )
		{
			MergeToolbarInstruction& rInstruction = aMergeInstructionContainer[i];
			if ( ToolBarMerger::IsCorrectContext( rInstruction.aMergeContext, m_aModuleIdentifier ))
			{
				ReferenceToolbarPathInfo aRefPoint = ToolBarMerger::FindReferencePoint( m_pToolBar, rInstruction.aMergePoint );

				// convert the sequence< sequence< propertyvalue > > structure to
				// something we can better handle. A vector with item data
				AddonToolbarItemContainer aItems;
				ToolBarMerger::ConvertSeqSeqToVector( rInstruction.aMergeToolbarItems, aItems );

				if ( aRefPoint.bResult )
				{
					ToolBarMerger::ProcessMergeOperation( m_xFrame,
														  m_pToolBar,
														  aRefPoint.nPos,
														  nItemId,
														  m_aCommandMap,
														  m_aModuleIdentifier,
														  rInstruction.aMergeCommand,
														  rInstruction.aMergeCommandParameter,
														  aItems );
				}
				else
				{
					ToolBarMerger::ProcessMergeFallback( m_xFrame,
														 m_pToolBar,
														 aRefPoint.nPos,
														 nItemId,
														 m_aCommandMap,
														 m_aModuleIdentifier,
														 rInstruction.aMergeCommand,
														 rInstruction.aMergeFallback,
														 aItems );
				}
			}
		}
	}

	// Request images for all toolbar items. Must be done before CreateControllers as
	// some controllers need access to the image.
	RequestImages();

	// Create controllers after we set the images. There are controllers which needs
	// an image at the toolbar at creation time!
	CreateControllers();

	// Notify controllers that they are now correctly initialized and can start listening
	// toolbars that will open in popup mode will be updated immediately to avoid flickering
	if( m_pToolBar->WillUsePopupMode() )
		UpdateControllers();
	else if ( m_pToolBar->IsReallyVisible() )
		m_aAsyncUpdateControllersTimer.Start();

	// Try to retrieve UIName from the container property set and set it as the title
	// if it is not empty.
	Reference< XPropertySet > xPropSet( rItemContainer, UNO_QUERY );
	if ( xPropSet.is() )
	{
		try
		{
			rtl::OUString aUIName;
			xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "UIName" ))) >>= aUIName;
			if ( aUIName.getLength() > 0 )
				m_pToolBar->SetText( aUIName );
		}
		catch ( Exception& )
		{
		}
	}
}

void ToolBarManager::RequestImages()
{
	RTL_LOGFILE_CONTEXT( aLog, "framework (cd100003) ::ToolBarManager::RequestImages" );

	// Request images from image manager
	Sequence< rtl::OUString > aCmdURLSeq( m_aCommandMap.size() );
	Sequence< Reference< XGraphic > > aDocGraphicSeq;
	Sequence< Reference< XGraphic > > aModGraphicSeq;

	sal_uInt32 i = 0;
	CommandToInfoMap::iterator pIter = m_aCommandMap.begin();
	CommandToInfoMap::iterator pEnd = m_aCommandMap.end();
	while ( pIter != pEnd )
	{
		aCmdURLSeq[i++] = pIter->first;
		++pIter;
	}

	sal_Bool bBigImages( SvtMiscOptions().AreCurrentSymbolsLarge() );
	m_bIsHiContrast = m_pToolBar->GetSettings().GetStyleSettings().GetHighContrastMode();
	sal_Int16 p = getImageTypeFromBools( SvtMiscOptions().AreCurrentSymbolsLarge(), m_bIsHiContrast );

	if ( m_xDocImageManager.is() )
		aDocGraphicSeq = m_xDocImageManager->getImages( p, aCmdURLSeq );
	aModGraphicSeq = m_xModuleImageManager->getImages( p, aCmdURLSeq );

	i = 0;
	pIter = m_aCommandMap.begin();
	while ( pIter != pEnd )
	{
		rtl::OUString aCommandURL = aCmdURLSeq[i];

		Image aImage;
		if ( aDocGraphicSeq.getLength() > 0 )
			aImage = Image( aDocGraphicSeq[i] );
		if ( !aImage )
		{
			aImage = Image( aModGraphicSeq[i] );
			// Try also to query for add-on images before giving up and use an
			// empty image.
			if ( !aImage )
				aImage = QueryAddonsImage( aCmdURLSeq[i], bBigImages, m_bIsHiContrast );

			pIter->second.nImageInfo = 1; // mark image as module based
		}
		else
		{
			pIter->second.nImageInfo = 0; // mark image as document based
		}
		setToolBarImage(aImage,pIter);
		++pIter;
		++i;
	}
}

void ToolBarManager::notifyRegisteredControllers( const rtl::OUString& aUIElementName, const rtl::OUString& aCommand )
{
	ResetableGuard aGuard( m_aLock );
	if ( !m_aSubToolBarControllerMap.empty() )
	{
		SubToolBarToSubToolBarControllerMap::const_iterator pIter =
			m_aSubToolBarControllerMap.find( aUIElementName );

		if ( pIter != m_aSubToolBarControllerMap.end() )
		{
			const SubToolBarControllerVector& rSubToolBarVector = pIter->second;
			if ( !rSubToolBarVector.empty() )
			{
				SubToolBarControllerVector aNotifyVector = rSubToolBarVector;
				aGuard.unlock();

				const sal_uInt32 nCount = aNotifyVector.size();
				for ( sal_uInt32 i=0; i < nCount; i++ )
				{
					try
					{
						Reference< XSubToolbarController > xController = aNotifyVector[i];
						if ( xController.is() )
							xController->functionSelected( aCommand );
					}
					catch ( RuntimeException& e )
					{
						throw e;
					}
					catch ( Exception& )
					{
					}
				}
			}
		}
	}
}
long ToolBarManager::HandleClick(void ( SAL_CALL XToolbarController::*_pClick )())
{
	ResetableGuard aGuard( m_aLock );

	if ( m_bDisposed )
		return 1;

	sal_uInt16 nId( m_pToolBar->GetCurItemId() );
	ToolBarControllerMap::const_iterator pIter = m_aControllerMap.find( nId );
	if ( pIter != m_aControllerMap.end() )
	{
		Reference< XToolbarController > xController( pIter->second, UNO_QUERY );

		if ( xController.is() )
			(xController.get()->*_pClick)( );
	} // if ( pIter != m_aControllerMap.end() )
	return 1;
}

IMPL_LINK( ToolBarManager, Click, ToolBox*, EMPTYARG )
{
	return HandleClick(&XToolbarController::click);
}

IMPL_LINK( ToolBarManager, DropdownClick, ToolBox*, EMPTYARG )
{
	ResetableGuard aGuard( m_aLock );

	if ( m_bDisposed )
		return 1;

	sal_uInt16 nId( m_pToolBar->GetCurItemId() );
	ToolBarControllerMap::const_iterator pIter = m_aControllerMap.find( nId );
	if ( pIter != m_aControllerMap.end() )
	{
		Reference< XToolbarController > xController( pIter->second, UNO_QUERY );

		if ( xController.is() )
		{
			Reference< XWindow > xWin = xController->createPopupWindow();
			if ( xWin.is() )
				xWin->setFocus();
		}
	}
	return 1;
}

IMPL_LINK( ToolBarManager, DoubleClick, ToolBox*, EMPTYARG )
{
	return HandleClick(&XToolbarController::doubleClick);
}

void ToolBarManager::ImplClearPopupMenu( ToolBox *pToolBar )
{
	if ( m_bDisposed )
		return;

	PopupMenu *pMenu = pToolBar->GetMenu();

	// remove config entries from menu, so we have a clean menu to start with
	// remove submenu first
	PopupMenu* pItemMenu = pMenu->GetPopupMenu( 1 );
	if( pItemMenu )
	{
		pItemMenu->Clear();
		delete pItemMenu;
		pItemMenu = NULL;
		pMenu->SetPopupMenu( 1, pItemMenu );
	}

	// remove all items that were not added by the toolbar itself
	sal_uInt16 i;
	for( i=0; i<pMenu->GetItemCount(); )
	{
		if( pMenu->GetItemId( i ) < TOOLBOX_MENUITEM_START )
			pMenu->RemoveItem( i );
		else
			i++;
	}
}

IMPL_LINK( ToolBarManager, MenuDeactivate, Menu*, pMenu )
{
	ResetableGuard aGuard( m_aLock );

	if ( m_bDisposed )
		return 1;

	if( pMenu != m_pToolBar->GetMenu() )
		return 1;

	ImplClearPopupMenu( m_pToolBar );

	return 0;
}

Reference< XModel > ToolBarManager::GetModelFromFrame() const
{
	Reference< XController > xController = m_xFrame->getController();
	Reference< XModel > xModel;
	if ( xController.is() )
		xModel = xController->getModel();

	return xModel;
}

sal_Bool ToolBarManager::IsPluginMode() const
{
	sal_Bool bPluginMode( sal_False );

	if ( m_xFrame.is() )
	{
		Reference< XModel > xModel = GetModelFromFrame();
		if ( xModel.is() )
		{
			Sequence< PropertyValue > aSeq = xModel->getArgs();
			comphelper::MediaDescriptor aMediaDescriptor( aSeq );
			bPluginMode = aMediaDescriptor.getUnpackedValueOrDefault< sal_Bool >(
							comphelper::MediaDescriptor::PROP_VIEWONLY(), sal_False );
		}
	}

	return bPluginMode;
}

bool ToolBarManager::MenuItemAllowed( sal_uInt16 ) const
{
	return true;
}

// added for i33668 by shizhoubo : 200804
PopupMenu * ToolBarManager::GetToolBarCustomMeun(ToolBox* pToolBar)
{
	PopupMenu *pMenu = pToolBar->GetMenu();
	// remove all entries before inserting new ones
	ImplClearPopupMenu( pToolBar );
	// No config menu entries if command ".uno:ConfigureDialog" is not enabled
	Reference< XDispatch > xDisp;
	com::sun::star::util::URL aURL;
	if ( m_xFrame.is() )
	{
		Reference< XDispatchProvider > xProv( m_xFrame, UNO_QUERY );
		aURL.Complete = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:ConfigureDialog" ));
		m_xURLTransformer->parseStrict( aURL );
		if ( xProv.is() )
			xDisp = xProv->queryDispatch( aURL, ::rtl::OUString(), 0 );

		if ( !xDisp.is() || IsPluginMode() )
			return 0;
	}

	// popup menu for quick customization
	sal_Bool bHideDisabledEntries = !SvtMenuOptions().IsEntryHidingEnabled();
	PopupMenu aPopupMenu( FwkResId( POPUPMENU_TOOLBAR_QUICKCUSTOMIZATION ));

	if ( m_pToolBar->IsCustomize() )
	{
		sal_uInt16 nPos( 0 );
		PopupMenu* pItemMenu( aPopupMenu.GetPopupMenu( 1 ));

		sal_Bool bIsFloating( sal_False );

		DockingManager* pDockMgr = Window::GetDockingManager();
		if ( pDockMgr )
			bIsFloating = pDockMgr->IsFloating( m_pToolBar );

		if ( !bIsFloating )
		{
			aPopupMenu.EnableItem( MENUITEM_TOOLBAR_DOCKTOOLBAR, sal_False );
			aPopupMenu.EnableItem( MENUITEM_TOOLBAR_DOCKALLTOOLBAR, sal_False );
			Reference< XDockableWindow > xDockable( VCLUnoHelper::GetInterface( m_pToolBar ), UNO_QUERY );
			if( xDockable.is() )
				aPopupMenu.CheckItem( MENUITEM_TOOLBAR_LOCKTOOLBARPOSITION, xDockable->isLocked() );
		}
		else
			aPopupMenu.EnableItem( MENUITEM_TOOLBAR_LOCKTOOLBARPOSITION, sal_False );

		if ( !m_bCanBeCustomized )
		{
			// Non-configurable toolbars should disable configuration menu items
			aPopupMenu.EnableItem( MENUITEM_TOOLBAR_VISIBLEBUTTON, sal_False );
			aPopupMenu.EnableItem( MENUITEM_TOOLBAR_CUSTOMIZETOOLBAR, sal_False );
		}

		// Disable menu item CLOSE if the toolbar has no closer
		// added for issue 64028 by shizhoubo
		if( !(pToolBar->GetFloatStyle() & WB_CLOSEABLE) )
			aPopupMenu.EnableItem(MENUITEM_TOOLBAR_CLOSE, sal_False);
		//end

		pItemMenu->SetMenuFlags (pItemMenu->GetMenuFlags () |
								 MENU_FLAG_SHOWCHECKIMAGES);

		for ( nPos = 0; nPos < m_pToolBar->GetItemCount(); ++nPos )
		{
			if ( m_pToolBar->GetItemType(nPos) == TOOLBOXITEM_BUTTON )
			{
				sal_uInt16 nId = m_pToolBar->GetItemId(nPos);
				::rtl::OUString aCommandURL = m_pToolBar->GetItemCommand( nId );
				pItemMenu->InsertItem( STARTID_CUSTOMIZE_POPUPMENU+nPos, m_pToolBar->GetItemText( nId ), MIB_CHECKABLE );
				pItemMenu->CheckItem( STARTID_CUSTOMIZE_POPUPMENU+nPos, m_pToolBar->IsItemVisible( nId ) );
				pItemMenu->SetItemCommand( STARTID_CUSTOMIZE_POPUPMENU+nPos, aCommandURL );
				pItemMenu->SetItemImage( STARTID_CUSTOMIZE_POPUPMENU+nPos,
										 GetImageFromURL( m_xFrame,
														  aCommandURL,
														  sal_False,
														  m_bIsHiContrast ));
			}
			else
			{
				pItemMenu->InsertSeparator();
			}
		}
	}
	else
	{
		sal_uInt16 nPos = aPopupMenu.GetItemPos( MENUITEM_TOOLBAR_CUSTOMIZETOOLBAR );
		if ( nPos != MENU_ITEM_NOTFOUND )
			 aPopupMenu.RemoveItem( nPos );
	}

	// copy all menu items to the toolbar menu
	if( pMenu->GetItemCount() )
		pMenu->InsertSeparator();

	sal_uInt16 i;
	for( i=0; i< aPopupMenu.GetItemCount(); i++)
	{
		sal_uInt16 nId = aPopupMenu.GetItemId( i );
		if ( MenuItemAllowed( nId ))
			 pMenu->CopyItem( aPopupMenu, i, MENU_APPEND );
	}

	// set submenu to toolbar menu
	if( aPopupMenu.GetPopupMenu( 1 ) )
	{
		// create an own submenu to avoid auto-delete when resource menu is deleted
		PopupMenu *pItemMenu = new PopupMenu();

		pItemMenu->SetMenuFlags (pItemMenu->GetMenuFlags () |
								 MENU_FLAG_SHOWCHECKIMAGES);

		for( i=0; i< aPopupMenu.GetPopupMenu( 1 )->GetItemCount(); i++)
			pItemMenu->CopyItem( *aPopupMenu.GetPopupMenu( 1 ), i, MENU_APPEND );

		pMenu->SetPopupMenu( 1, pItemMenu );
	}

	if ( bHideDisabledEntries )
		pMenu->RemoveDisabledEntries();

	return pMenu;
}

// added for 33668 by shizhoubo
IMPL_LINK( ToolBarManager, Command, CommandEvent*, pCmdEvt )
{
	ResetableGuard aGuard( m_aLock );

	if ( m_bDisposed )
		return 1;
	if ( pCmdEvt->GetCommand() != COMMAND_CONTEXTMENU )
		return 0;

	PopupMenu * pMenu = GetToolBarCustomMeun(m_pToolBar);
	if (pMenu)
	{
		// make sure all disabled entries will be shown
		pMenu->SetMenuFlags( pMenu->GetMenuFlags() | MENU_FLAG_ALWAYSSHOWDISABLEDENTRIES );
		::Point aPoint( pCmdEvt->GetMousePosPixel() );
		pMenu->Execute( m_pToolBar, aPoint );
	}

	return 0;
}
//end

IMPL_LINK( ToolBarManager, MenuButton, ToolBox*, pToolBar )
{
	ResetableGuard aGuard( m_aLock );

	if ( m_bDisposed )
		return 1;
	// modify for i33668 by shizhoubo:2008:04
	GetToolBarCustomMeun(pToolBar);
	//end
	return 0;
 }

IMPL_LINK( ToolBarManager, MenuSelect, Menu*, pMenu )
{
	// We have to hold a reference to ourself as it is possible that we will be disposed and
	// our refcount could be zero (destruction) otherwise.
	Reference< XInterface > xInterface( static_cast< OWeakObject* >( this ), UNO_QUERY );

	{
		// The guard must be in its own context as the we can get destroyed when our
		// own xInterface reference get destroyed!
		ResetableGuard aGuard( m_aLock );

		if ( m_bDisposed )
			return 1;

		switch ( pMenu->GetCurItemId() )
		{
			case MENUITEM_TOOLBAR_CUSTOMIZETOOLBAR:
			{
				Reference< XDispatch > xDisp;
				com::sun::star::util::URL aURL;
				if ( m_xFrame.is() )
				{
					Reference< XDispatchProvider > xProv( m_xFrame, UNO_QUERY );
					aURL.Complete = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:ConfigureDialog" ));
					m_xURLTransformer->parseStrict( aURL );
					if ( xProv.is() )
						xDisp = xProv->queryDispatch( aURL, ::rtl::OUString(), 0 );
				}

				if ( xDisp.is() )
				{
					Sequence< PropertyValue > aPropSeq( 1 );

					aPropSeq[ 0 ].Name =
						rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ResourceURL"));
					aPropSeq[ 0 ].Value <<= m_aResourceName;

					xDisp->dispatch( aURL, aPropSeq );
				}
				break;
			}

			case MENUITEM_TOOLBAR_DOCKTOOLBAR:
			{
				ExecuteInfo* pExecuteInfo = new ExecuteInfo;

				pExecuteInfo->aToolbarResName	= m_aResourceName;
				pExecuteInfo->nCmd				= EXEC_CMD_DOCKTOOLBAR;
				pExecuteInfo->xLayoutManager	= getLayoutManagerFromFrame( m_xFrame );

				Application::PostUserEvent( STATIC_LINK(0, ToolBarManager, ExecuteHdl_Impl), pExecuteInfo );
				break;
			}

			case MENUITEM_TOOLBAR_DOCKALLTOOLBAR:
			{
				ExecuteInfo* pExecuteInfo = new ExecuteInfo;

				pExecuteInfo->aToolbarResName	= m_aResourceName;
				pExecuteInfo->nCmd				= EXEC_CMD_DOCKALLTOOLBARS;
				pExecuteInfo->xLayoutManager	= getLayoutManagerFromFrame( m_xFrame );

				Application::PostUserEvent( STATIC_LINK(0, ToolBarManager, ExecuteHdl_Impl), pExecuteInfo );
				break;
			}

			case MENUITEM_TOOLBAR_LOCKTOOLBARPOSITION:
			{
				Reference< XLayoutManager > xLayoutManager = getLayoutManagerFromFrame( m_xFrame );
				if ( xLayoutManager.is() )
				{
					Reference< XDockableWindow > xDockable( VCLUnoHelper::GetInterface( m_pToolBar ), UNO_QUERY );

					if( xDockable->isLocked() )
						xLayoutManager->unlockWindow( m_aResourceName );
					else
						xLayoutManager->lockWindow( m_aResourceName );
				}
				break;
			}

			case MENUITEM_TOOLBAR_CLOSE:
			{
				ExecuteInfo* pExecuteInfo = new ExecuteInfo;

				pExecuteInfo->aToolbarResName	= m_aResourceName;
				pExecuteInfo->nCmd				= EXEC_CMD_CLOSETOOLBAR;
				pExecuteInfo->xLayoutManager	= getLayoutManagerFromFrame( m_xFrame );
				pExecuteInfo->xWindow			= VCLUnoHelper::GetInterface( m_pToolBar );

				Application::PostUserEvent( STATIC_LINK(0, ToolBarManager, ExecuteHdl_Impl), pExecuteInfo );
			}

			default:
			{
				sal_uInt16 nId = pMenu->GetCurItemId();
				if(( nId > 0 ) && ( nId < TOOLBOX_MENUITEM_START ))
				{
					// toggle toolbar button visibility
					rtl::OUString aCommand = pMenu->GetItemCommand( nId );

					Reference< XLayoutManager > xLayoutManager = getLayoutManagerFromFrame( m_xFrame );
					if ( xLayoutManager.is() )
					{
						Reference< XUIElementSettings > xUIElementSettings( xLayoutManager->getElement( m_aResourceName ), UNO_QUERY );
						if ( xUIElementSettings.is() )
						{
							Reference< XIndexContainer > xItemContainer( xUIElementSettings->getSettings( sal_True ), UNO_QUERY );
							sal_Int32 nCount = xItemContainer->getCount();
							for ( sal_Int32 i = 0; i < nCount; i++ )
							{
								Sequence< PropertyValue >	aProp;
								sal_Int32					nVisibleIndex( -1 );
								rtl::OUString				aCommandURL;
								sal_Bool					bVisible( sal_False );

								if ( xItemContainer->getByIndex( i ) >>= aProp )
								{
									for ( sal_Int32 j = 0; j < aProp.getLength(); j++ )
									{
										if ( aProp[j].Name.equalsAscii( ITEM_DESCRIPTOR_COMMANDURL ))
										{
											aProp[j].Value >>= aCommandURL;
										}
										else if ( aProp[j].Name.equalsAscii( ITEM_DESCRIPTOR_VISIBLE ))
										{
											aProp[j].Value >>= bVisible;
											nVisibleIndex = j;
										}
									}

									if (( aCommandURL == aCommand ) && ( nVisibleIndex >= 0 ))
									{
										// We have found the requested item, toggle the visible flag
										// and write back the configuration settings to the toolbar
										aProp[nVisibleIndex].Value = makeAny( !bVisible );
										try
										{
											xItemContainer->replaceByIndex( i, makeAny( aProp ));
											xUIElementSettings->setSettings( Reference< XIndexAccess >( xItemContainer, UNO_QUERY ));
											Reference< XPropertySet > xPropSet( xUIElementSettings, UNO_QUERY );
											if ( xPropSet.is() )
											{
												Reference< XUIConfigurationPersistence > xUICfgMgr;
												if (( xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ConfigurationSource" ))) >>= xUICfgMgr ) && ( xUICfgMgr.is() ))
													xUICfgMgr->store();
											}
										}
										catch ( Exception& )
										{
										}

										break;
									}
								}
							}
						}
					}
				}
				break;
			}
		}

		// remove all entries - deactivate is not reliable
		// The method checks if we are already disposed and in that case does nothing!
		ImplClearPopupMenu( m_pToolBar );
	}

	return 1;
}

IMPL_LINK( ToolBarManager, Select, ToolBox*, EMPTYARG )
{
	if ( m_bDisposed )
		return 1;

	sal_Int16 nKeyModifier( (sal_Int16)m_pToolBar->GetModifier() );
	sal_uInt16 nId( m_pToolBar->GetCurItemId() );

	ToolBarControllerMap::const_iterator pIter = m_aControllerMap.find( nId );
	if ( pIter != m_aControllerMap.end() )
	{
		Reference< XToolbarController > xController( pIter->second, UNO_QUERY );

		if ( xController.is() )
			xController->execute( nKeyModifier );
	}

	return 1;
}

IMPL_LINK( ToolBarManager, Highlight, ToolBox*, EMPTYARG )
{
	return 1;
}

IMPL_LINK( ToolBarManager, Activate, ToolBox*, EMPTYARG )
{
	return 1;
}

IMPL_LINK( ToolBarManager, Deactivate, ToolBox*, EMPTYARG )
{
	return 1;
}

IMPL_LINK( ToolBarManager, StateChanged, StateChangedType*, pStateChangedType )
{
	if ( m_bDisposed )
		return 1;

	if ( *pStateChangedType == STATE_CHANGE_CONTROLBACKGROUND )
	{
		// Check if we need to get new images for normal/high contrast mode
		CheckAndUpdateImages();
	}
	else if ( *pStateChangedType == STATE_CHANGE_VISIBLE )
	{
		if ( m_pToolBar->IsReallyVisible() )
			 m_aAsyncUpdateControllersTimer.Start();
	}
	else if ( *pStateChangedType == STATE_CHANGE_INITSHOW )
	{
		m_aAsyncUpdateControllersTimer.Start();
	}
	return 1;
}

IMPL_LINK( ToolBarManager, DataChanged, DataChangedEvent*, pDataChangedEvent )
{
	if ((( pDataChangedEvent->GetType() == DATACHANGED_SETTINGS	)	||
		(  pDataChangedEvent->GetType() == DATACHANGED_DISPLAY	))	&&
		(  pDataChangedEvent->GetFlags() & SETTINGS_STYLE		))
	{
		// Check if we need to get new images for normal/high contrast mode
		CheckAndUpdateImages();
	}

	for ( sal_uInt16 nPos = 0; nPos < m_pToolBar->GetItemCount(); ++nPos )
	{
		const sal_uInt16 nId = m_pToolBar->GetItemId(nPos);
		Window* pWindow = m_pToolBar->GetItemWindow( nId );
		if ( pWindow )
		{
			const DataChangedEvent& rDCEvt( *pDataChangedEvent );
			pWindow->DataChanged( rDCEvt );
		}
	}

	if ( !m_pToolBar->IsFloatingMode() &&
		 m_pToolBar->IsVisible() )
	{
		// Resize toolbar, layout manager is resize listener and will calc
		// the layout automatically.
		::Size aSize( m_pToolBar->CalcWindowSizePixel() );
		m_pToolBar->SetOutputSizePixel( aSize );
	}

	return 1;
}

IMPL_LINK( ToolBarManager, AsyncUpdateControllersHdl, Timer *, EMPTYARG )
{
	// The guard must be in its own context as the we can get destroyed when our
	// own xInterface reference get destroyed!
	Reference< XComponent > xThis( static_cast< OWeakObject* >(this), UNO_QUERY );

	ResetableGuard aGuard( m_aLock );

	if ( m_bDisposed )
		return 1;

	// Request to update our controllers
	m_aAsyncUpdateControllersTimer.Stop();
	UpdateControllers();

	return 0;
}

IMPL_STATIC_LINK_NOINSTANCE( ToolBarManager, ExecuteHdl_Impl, ExecuteInfo*, pExecuteInfo )
{
	try
	{
		// Asynchronous execution as this can lead to our own destruction!
		if (( pExecuteInfo->nCmd == EXEC_CMD_CLOSETOOLBAR ) &&
			( pExecuteInfo->xLayoutManager.is() ) &&
			( pExecuteInfo->xWindow.is() ))
		{
			// Use docking window close to close the toolbar. The layout manager is
			// listener and will react correctly according to the context sensitive
			// flag of our toolbar.
			Window* pWin = VCLUnoHelper::GetWindow( pExecuteInfo->xWindow );
			DockingWindow* pDockWin = dynamic_cast< DockingWindow* >( pWin );
			if ( pDockWin )
				 pDockWin->Close();
		}
		else if (( pExecuteInfo->nCmd == EXEC_CMD_DOCKTOOLBAR ) &&
				 ( pExecuteInfo->xLayoutManager.is() ))
		{
			::com::sun::star::awt::Point aPoint;
			aPoint.X = aPoint.Y = SAL_MAX_INT32;
			pExecuteInfo->xLayoutManager->dockWindow( pExecuteInfo->aToolbarResName,
													  DockingArea_DOCKINGAREA_DEFAULT,
													  aPoint );
		}
		else if (( pExecuteInfo->nCmd == EXEC_CMD_DOCKALLTOOLBARS ) &&
				 ( pExecuteInfo->xLayoutManager.is() ))
		{
			pExecuteInfo->xLayoutManager->dockAllWindows( UIElementType::TOOLBAR );
		}
	}
	catch ( Exception& )
	{
	}

	delete pExecuteInfo;
	return 0;
}

Image ToolBarManager::QueryAddonsImage( const ::rtl::OUString& aCommandURL, bool bBigImages, bool bHiContrast )
{
	Image aImage = framework::AddonsOptions().GetImageFromURL( aCommandURL, bBigImages, bHiContrast );
	return aImage;
}

bool ToolBarManager::impl_RetrieveShortcutsFromConfiguration(
	const Reference< XAcceleratorConfiguration >& rAccelCfg,
	const rtl::OUString& rCommand,
	rtl::OUString& rShortCut )
{
	RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "framework", "Ocke.Janssen@sun.com", "ToolBarManager::impl_RetrieveShortcutsFromConfiguration" );
	if ( rAccelCfg.is() )
	{
		try
		{
			com::sun::star::awt::KeyEvent aKeyEvent;
			Sequence< OUString > aCommands(1);
			aCommands[0] = rCommand;

			Sequence< Any > aSeqKeyCode( rAccelCfg->getPreferredKeyEventsForCommandList( aCommands ) );
			if( aSeqKeyCode.getLength() == 1 )
			{
				if ( aSeqKeyCode[0] >>= aKeyEvent )
				{
					rShortCut = svt::AcceleratorExecute::st_AWTKey2VCLKey( aKeyEvent ).GetName();
					return true;
				}
			}
		}
		catch ( IllegalArgumentException& )
		{
		}
	}

	return false;
}

bool ToolBarManager::RetrieveShortcut( const rtl::OUString& rCommandURL, rtl::OUString& rShortCut )
{
	RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "framework", "Ocke.Janssen@sun.com", "ToolBarManager::RetrieveShortcuts" );
	if ( m_bModuleIdentified )
	{
		Reference< XAcceleratorConfiguration > xDocAccelCfg( m_xDocAcceleratorManager );
		Reference< XAcceleratorConfiguration > xModuleAccelCfg( m_xModuleAcceleratorManager );
		Reference< XAcceleratorConfiguration > xGlobalAccelCfg( m_xGlobalAcceleratorManager );

		if ( !m_bAcceleratorCfg )
		{
			// Retrieve references on demand
			m_bAcceleratorCfg = sal_True;
			if ( !xDocAccelCfg.is() )
			{
				Reference< XController > xController = m_xFrame->getController();
				Reference< XModel > xModel;
				if ( xController.is() )
				{
					xModel = xController->getModel();
					if ( xModel.is() )
					{
						Reference< XUIConfigurationManagerSupplier > xSupplier( xModel, UNO_QUERY );
						if ( xSupplier.is() )
						{
							Reference< XUIConfigurationManager > xDocUICfgMgr( xSupplier->getUIConfigurationManager(), UNO_QUERY );
							if ( xDocUICfgMgr.is() )
							{
								xDocAccelCfg = Reference< XAcceleratorConfiguration >( xDocUICfgMgr->getShortCutManager(), UNO_QUERY );
								m_xDocAcceleratorManager = xDocAccelCfg;
							}
						}
					}
				}
			}

			if ( !xModuleAccelCfg.is() )
			{
				Reference< XModuleUIConfigurationManagerSupplier > xModuleCfgMgrSupplier( m_xServiceManager->createInstance(
																							SERVICENAME_MODULEUICONFIGURATIONMANAGERSUPPLIER ),
																						  UNO_QUERY );
				try
				{
					Reference< XUIConfigurationManager > xUICfgMgr = xModuleCfgMgrSupplier->getUIConfigurationManager( m_aModuleIdentifier );
					if ( xUICfgMgr.is() )
					{
						xModuleAccelCfg = Reference< XAcceleratorConfiguration >( xUICfgMgr->getShortCutManager(), UNO_QUERY );
						m_xModuleAcceleratorManager = xModuleAccelCfg;
					}
				}
				catch ( RuntimeException& )
				{
					throw;
				}
				catch ( Exception& )
				{
				}
			}

			if ( !xGlobalAccelCfg.is() )
			{
				xGlobalAccelCfg = Reference< XAcceleratorConfiguration >( m_xServiceManager->createInstance(
																			SERVICENAME_GLOBALACCELERATORCONFIGURATION ),
																		  UNO_QUERY );
				m_xGlobalAcceleratorManager = xGlobalAccelCfg;
			}
		}

		bool bFound = false;

		if ( m_xGlobalAcceleratorManager.is() )
			bFound = impl_RetrieveShortcutsFromConfiguration( xGlobalAccelCfg, rCommandURL, rShortCut );
		if ( !bFound && m_xModuleAcceleratorManager.is() )
			bFound = impl_RetrieveShortcutsFromConfiguration( xModuleAccelCfg, rCommandURL, rShortCut );
		if ( !bFound && m_xDocAcceleratorManager.is() )
			impl_RetrieveShortcutsFromConfiguration( xGlobalAccelCfg, rCommandURL, rShortCut );

		if( bFound )
			return true;
	}
	return false;
}

}
