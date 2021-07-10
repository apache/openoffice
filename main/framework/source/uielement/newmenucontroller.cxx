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
#include <uielement/newmenucontroller.hxx>

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________
#include <threadhelp/resetableguard.hxx>
#include "services.h"
#ifndef __FRAMEWORK_CLASSES_RESOURCE_HRC_
#include <classes/resource.hrc>
#endif
#include <classes/fwkresid.hxx>
#include <framework/bmkmenu.hxx>
#include <framework/imageproducer.hxx>
#include <framework/menuconfiguration.hxx>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/awt/XDevice.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/awt/MenuItemStyle.hpp>
#include <com/sun/star/ui/XModuleUIConfigurationManagerSupplier.hpp>
#include <com/sun/star/ui/XUIConfigurationManagerSupplier.hpp>
#include <com/sun/star/frame/XModuleManager.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/document/CorruptedFilterConfigurationException.hpp>

//_________________________________________________________________________________________________________________
//	includes of other projects
//_________________________________________________________________________________________________________________
#include <vcl/svapp.hxx>
#include <vcl/i18nhelp.hxx>
#include <tools/urlobj.hxx>
#include <rtl/ustrbuf.hxx>
#include <cppuhelper/implbase1.hxx>
#include <osl/file.hxx>
#include <svtools/menuoptions.hxx>
#include <svtools/acceleratorexecute.hxx>
#include <unotools/moduleoptions.hxx>
#include <dispatch/uieventloghelper.hxx>
#include <vos/mutex.hxx>

//_________________________________________________________________________________________________________________
//	Defines
//_________________________________________________________________________________________________________________
//

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::frame;
using namespace com::sun::star::beans;
using namespace com::sun::star::util;
using namespace com::sun::star::container;
using namespace com::sun::star::ui;

static const char SFX_REFERER_USER[] = "private:user";

namespace framework
{

DEFINE_XSERVICEINFO_MULTISERVICE        (   NewMenuController                           ,
                                            OWeakObject                                 ,
                                            SERVICENAME_POPUPMENUCONTROLLER		        ,
											IMPLEMENTATIONNAME_NEWMENUCONTROLLER
										)

DEFINE_INIT_SERVICE                     (   NewMenuController, {} )

void NewMenuController::setMenuImages( PopupMenu* pPopupMenu, sal_Bool bSetImages, sal_Bool bHiContrast )
{
	sal_uInt16 nItemCount = pPopupMenu->GetItemCount();
    Image               aImage;
    Reference< XFrame > xFrame( m_xFrame );

    for ( sal_uInt16 i = 0; i < nItemCount; i++ )
    {
        sal_uInt16 nItemId = pPopupMenu->GetItemId( sal::static_int_cast<sal_uInt16>( i ));
        if ( nItemId != 0 )
        {
            if ( bSetImages )
            {
                sal_Bool        bImageSet( sal_False );
		        ::rtl::OUString aImageId;

                AddInfoForId::const_iterator pInfo = m_aAddInfoForItem.find( nItemId );
                if ( pInfo != m_aAddInfoForItem.end() )
				    aImageId = pInfo->second.aImageId; // Retrieve image id for menu item

			    if ( aImageId.getLength() > 0 )
			    {
                    aImage = GetImageFromURL( xFrame, aImageId, sal_False, bHiContrast );
				    if ( !!aImage )
				    {
					    bImageSet = sal_True;
					    pPopupMenu->SetItemImage( nItemId, aImage );
				    }
			    }

			    if ( !bImageSet )
                {
                    String aCmd( pPopupMenu->GetItemCommand( nItemId ) );
                    if ( aCmd.Len() )
                        aImage = GetImageFromURL( xFrame, aCmd, sal_False, bHiContrast );

                    if ( !!aImage )
                        pPopupMenu->SetItemImage( nItemId, aImage );
                }
            }
            else
                pPopupMenu->SetItemImage( nItemId, aImage );
        }
    }
}

void NewMenuController::determineAndSetNewDocAccel( PopupMenu* pPopupMenu, const KeyCode& rKeyCode )
{
    sal_uInt16        nCount( pPopupMenu->GetItemCount() );
    sal_uInt16        nId( 0 );
    sal_Bool      bFound( sal_False );
    rtl::OUString aCommand;

    if ( m_aEmptyDocURL.getLength() > 0 )
    {
        // Search for the empty document URL

        for ( sal_uInt32 i = 0; i < sal_uInt32( nCount ); i++ )
        {
            nId = pPopupMenu->GetItemId( sal_uInt16( i ));
            if ( nId != 0 && pPopupMenu->GetItemType( nId ) != MENUITEM_SEPARATOR )
            {
                aCommand = pPopupMenu->GetItemCommand( nId );
                if ( aCommand.indexOf( m_aEmptyDocURL ) == 0 )
                {
                    pPopupMenu->SetAccelKey( nId, rKeyCode );
                    bFound = sal_True;
                    break;
                }
            }
        }
    }

    if ( !bFound )
    {
        // Search for the default module name
        rtl::OUString aDefaultModuleName( SvtModuleOptions().GetDefaultModuleName() );
        if ( aDefaultModuleName.getLength() > 0 )
        {
            for ( sal_uInt32 i = 0; i < sal_uInt32( nCount ); i++ )
            {
                nId = pPopupMenu->GetItemId( sal_uInt16( i ));
                if ( nId != 0 && pPopupMenu->GetItemType( nId ) != MENUITEM_SEPARATOR )
                {
                    aCommand = pPopupMenu->GetItemCommand( nId );
                    if ( aCommand.indexOf( aDefaultModuleName ) >= 0 )
                    {
                        pPopupMenu->SetAccelKey( nId, rKeyCode );
                        break;
                    }
                }
            }
        }
    }
}

void NewMenuController::setAccelerators( PopupMenu* pPopupMenu )
{
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
                Reference< XUIConfigurationManager > xUICfgMgr = xModuleCfgMgrSupplier->getUIConfigurationManager( m_aModuleIdentifier );
                if ( xUICfgMgr.is() )
                {
                    xModuleAccelCfg = Reference< XAcceleratorConfiguration >( xUICfgMgr->getShortCutManager(), UNO_QUERY );
                    m_xModuleAcceleratorManager = xModuleAccelCfg;
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

        KeyCode                         aEmptyKeyCode;
        sal_uInt32                      nItemCount( pPopupMenu->GetItemCount() );
        std::vector< KeyCode >          aMenuShortCuts;
        std::vector< rtl::OUString >    aCmds;
        std::vector< sal_uInt32 >       aIds;
        for ( sal_uInt32 i = 0; i < nItemCount; i++ )
        {
            sal_uInt16 nId( pPopupMenu->GetItemId( sal_uInt16( i )));
            if ( nId & ( pPopupMenu->GetItemType( nId ) != MENUITEM_SEPARATOR ))
            {
                aIds.push_back( nId );
                aMenuShortCuts.push_back( aEmptyKeyCode );
                aCmds.push_back( pPopupMenu->GetItemCommand( nId ));
            }
        }

        sal_uInt32 nSeqCount( aIds.size() );

        if ( m_bNewMenu )
            nSeqCount+=1;

        Sequence< rtl::OUString > aSeq( nSeqCount );

        // Add a special command for our "New" menu.
        if ( m_bNewMenu )
        {
            aSeq[nSeqCount-1] = m_aCommandURL;
            aMenuShortCuts.push_back( aEmptyKeyCode );
        }

        const sal_uInt32 nCount = aCmds.size();
        for ( sal_uInt32 i = 0; i < nCount; i++ )
            aSeq[i] = aCmds[i];

        if ( m_xGlobalAcceleratorManager.is() )
            retrieveShortcutsFromConfiguration( xGlobalAccelCfg, aSeq, aMenuShortCuts );
        if ( m_xModuleAcceleratorManager.is() )
            retrieveShortcutsFromConfiguration( xModuleAccelCfg, aSeq, aMenuShortCuts );
        if ( m_xDocAcceleratorManager.is() )
            retrieveShortcutsFromConfiguration( xGlobalAccelCfg, aSeq, aMenuShortCuts );

        const sal_uInt32 nCount2 = aIds.size();
        for ( sal_uInt32 i = 0; i < nCount2; i++ )
            pPopupMenu->SetAccelKey( sal_uInt16( aIds[i] ), aMenuShortCuts[i] );

        // Special handling for "New" menu short-cut should be set at the
        // document which will be opened using it.
        if ( m_bNewMenu )
        {
            if ( aMenuShortCuts[nSeqCount-1] != aEmptyKeyCode )
                determineAndSetNewDocAccel( pPopupMenu, aMenuShortCuts[nSeqCount-1] );
        }
    }
}

void NewMenuController::retrieveShortcutsFromConfiguration(
    const Reference< XAcceleratorConfiguration >& rAccelCfg,
    const Sequence< rtl::OUString >& rCommands,
    std::vector< KeyCode >& aMenuShortCuts )
{
    if ( rAccelCfg.is() )
    {
        try
        {
            com::sun::star::awt::KeyEvent aKeyEvent;
            Sequence< Any > aSeqKeyCode = rAccelCfg->getPreferredKeyEventsForCommandList( rCommands );
            for ( sal_Int32 i = 0; i < aSeqKeyCode.getLength(); i++ )
            {
                if ( aSeqKeyCode[i] >>= aKeyEvent )
                    aMenuShortCuts[i] = svt::AcceleratorExecute::st_AWTKey2VCLKey( aKeyEvent );
            }
        }
        catch ( IllegalArgumentException& )
        {
        }
    }
}

NewMenuController::NewMenuController( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager ) :
	svt::PopupMenuControllerBase( xServiceManager ),
    m_bShowImages( sal_True ),
    m_bHiContrast( sal_False ),
    m_bNewMenu( sal_False ),
    m_bModuleIdentified( sal_False ),
    m_bAcceleratorCfg( sal_False ),
    m_aTargetFrame( RTL_CONSTASCII_USTRINGPARAM( "_default" ))
{
}

NewMenuController::~NewMenuController()
{
}

// private function
void NewMenuController::fillPopupMenu( Reference< css::awt::XPopupMenu >& rPopupMenu )
{
    VCLXPopupMenu* pPopupMenu    = (VCLXPopupMenu *)VCLXMenu::GetImplementation( rPopupMenu );
    PopupMenu*     pVCLPopupMenu = 0;

    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

    resetPopupMenu( rPopupMenu );
    if ( pPopupMenu )
        pVCLPopupMenu = (PopupMenu *)pPopupMenu->GetMenu();

    if ( pVCLPopupMenu )
    {
        MenuConfiguration aMenuCfg( m_xServiceManager );
        BmkMenu* pSubMenu( 0 );

        if ( m_bNewMenu )
            pSubMenu = (BmkMenu*)aMenuCfg.CreateBookmarkMenu( m_xFrame, BOOKMARK_NEWMENU );
        else
            pSubMenu = (BmkMenu*)aMenuCfg.CreateBookmarkMenu( m_xFrame, BOOKMARK_WIZARDMENU );

        // copy entries as we have to use the provided popup menu
        *pVCLPopupMenu = *pSubMenu;

        Image           aImage;
        AddInfo         aAddInfo;

        // retrieve additional parameters from bookmark menu and
        // store it in a hash_map.
        for ( sal_uInt16 i = 0; i < pSubMenu->GetItemCount(); i++ )
        {
		    sal_uInt16 nItemId = pSubMenu->GetItemId( sal::static_int_cast<sal_uInt16>( i ) );
            if (( nItemId != 0 ) &&
                ( pSubMenu->GetItemType( nItemId ) != MENUITEM_SEPARATOR ))
            {
				MenuConfiguration::Attributes* pBmkAttributes = (MenuConfiguration::Attributes *)(pSubMenu->GetUserValue( nItemId ));
                if ( pBmkAttributes != 0 )
                {
                    aAddInfo.aTargetFrame = pBmkAttributes->aTargetFrame;
                    aAddInfo.aImageId     = pBmkAttributes->aImageId;

                    m_aAddInfoForItem.insert( AddInfoForId::value_type( nItemId, aAddInfo ));
                }
            }
        }

        if ( m_bShowImages )
            setMenuImages( pVCLPopupMenu, m_bShowImages, m_bHiContrast );

        delete pSubMenu;
    }
}

// XEventListener
void SAL_CALL NewMenuController::disposing( const EventObject& ) throw ( RuntimeException )
{
    Reference< css::awt::XMenuListener > xHolder(( OWeakObject *)this, UNO_QUERY );

    osl::MutexGuard aLock( m_aMutex );
    m_xFrame.clear();
    m_xDispatch.clear();
    m_xServiceManager.clear();

    if ( m_xPopupMenu.is() )
        m_xPopupMenu->removeMenuListener( Reference< css::awt::XMenuListener >(( OWeakObject *)this, UNO_QUERY ));
    m_xPopupMenu.clear();
}

// XStatusListener
void SAL_CALL NewMenuController::statusChanged( const FeatureStateEvent& ) throw ( RuntimeException )
{
}

// XMenuListener
void SAL_CALL NewMenuController::itemSelected( const css::awt::MenuEvent& rEvent ) throw (RuntimeException)
{
    Reference< css::awt::XPopupMenu > xPopupMenu;
    Reference< XDispatch >            xDispatch;
    Reference< XDispatchProvider >    xDispatchProvider;
    Reference< XMultiServiceFactory > xServiceManager;
    Reference< XURLTransformer >      xURLTransformer;

    osl::ClearableMutexGuard aLock( m_aMutex );
    xPopupMenu          = m_xPopupMenu;
    xDispatchProvider   = Reference< XDispatchProvider >( m_xFrame, UNO_QUERY );
    xServiceManager     = m_xServiceManager;
    xURLTransformer     = m_xURLTransformer;
    aLock.clear();

    css::util::URL aTargetURL;
    Sequence< PropertyValue > aArgsList( 1 );

    if ( xPopupMenu.is() && xDispatchProvider.is() )
    {
        VCLXPopupMenu* pPopupMenu = (VCLXPopupMenu *)VCLXPopupMenu::GetImplementation( xPopupMenu );
        if ( pPopupMenu )
        {
            {
                vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
                PopupMenu* pVCLPopupMenu = (PopupMenu *)pPopupMenu->GetMenu();
                aTargetURL.Complete = pVCLPopupMenu->GetItemCommand( rEvent.MenuId );
            }

            xURLTransformer->parseStrict( aTargetURL );

            aArgsList[0].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Referer" ));
	        aArgsList[0].Value = makeAny( ::rtl::OUString::createFromAscii( SFX_REFERER_USER ));

            rtl::OUString aTargetFrame( m_aTargetFrame );
            AddInfoForId::const_iterator pItem = m_aAddInfoForItem.find( rEvent.MenuId );
            if ( pItem != m_aAddInfoForItem.end() )
                aTargetFrame = pItem->second.aTargetFrame;

            xDispatch = xDispatchProvider->queryDispatch( aTargetURL, aTargetFrame, 0 );
        }
    }

    if ( xDispatch.is() )
    {
        // Call dispatch asynchronously as we can be destroyed while dispatch is
        // executed. VCL is not able to survive this as it wants to call listeners
        // after select!!!
        NewDocument* pNewDocument = new NewDocument;
        pNewDocument->xDispatch  = xDispatch;
        pNewDocument->aTargetURL = aTargetURL;
        pNewDocument->aArgSeq    = aArgsList;
        if(::comphelper::UiEventsLogger::isEnabled()) //#i88653#
            UiEventLogHelper(::rtl::OUString::createFromAscii("NewMenuController")).log(m_xServiceManager, m_xFrame, aTargetURL, aArgsList);
        Application::PostUserEvent( STATIC_LINK(0, NewMenuController, ExecuteHdl_Impl), pNewDocument );
    }
}

void SAL_CALL NewMenuController::itemActivated( const css::awt::MenuEvent& ) throw (RuntimeException)
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
    if ( m_xFrame.is() && m_xPopupMenu.is() )
    {
        VCLXPopupMenu* pPopupMenu = (VCLXPopupMenu *)VCLXPopupMenu::GetImplementation( m_xPopupMenu );
        if ( pPopupMenu )
        {
            const StyleSettings& rSettings = Application::GetSettings().GetStyleSettings();
            sal_Bool bShowImages( rSettings.GetUseImagesInMenus() );
            sal_Bool bHiContrast( rSettings.GetHighContrastMode() );

            PopupMenu* pVCLPopupMenu = (PopupMenu *)pPopupMenu->GetMenu();

            if (( m_bShowImages != bShowImages ) ||
                ( m_bHiContrast != bHiContrast ))
            {
                m_bShowImages = bShowImages;
                m_bHiContrast = bHiContrast;

                setMenuImages( pVCLPopupMenu, m_bShowImages, m_bHiContrast );
            }

            setAccelerators( pVCLPopupMenu );
        }
    }
}

// XPopupMenuController
void NewMenuController::impl_setPopupMenu()
{

    if ( m_xPopupMenu.is() )
        fillPopupMenu( m_xPopupMenu );

    // Identify module that we are attach to. It's our context that we need to know.
    Reference< XModuleManager > xModuleManager( m_xServiceManager->createInstance( SERVICENAME_MODULEMANAGER ),UNO_QUERY );
    if ( xModuleManager.is() )
    {
        try
        {
            m_aModuleIdentifier = xModuleManager->identify( m_xFrame );
            m_bModuleIdentified = sal_True;

            Reference< XNameAccess > xNameAccess( xModuleManager, UNO_QUERY );
            if (( m_aModuleIdentifier.getLength() > 0 ) && xNameAccess.is() )
            {
                Sequence< PropertyValue > aSeq;

                if ( xNameAccess->getByName( m_aModuleIdentifier ) >>= aSeq )
                {
                    for ( sal_Int32 y = 0; y < aSeq.getLength(); y++ )
                    {
                        if ( aSeq[y].Name.equalsAscii("ooSetupFactoryEmptyDocumentURL") )
                        {
                            aSeq[y].Value >>= m_aEmptyDocURL;
                            break;
                        }
                    }
                }
            }
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

// XInitialization
void SAL_CALL NewMenuController::initialize( const Sequence< Any >& aArguments ) throw ( Exception, RuntimeException )
{
    osl::MutexGuard aLock( m_aMutex );

    sal_Bool bInitialized( m_bInitialized );
    if ( !bInitialized )
    {
		svt::PopupMenuControllerBase::initialize( aArguments );

        if ( m_bInitialized )
        {
            const StyleSettings& rSettings = Application::GetSettings().GetStyleSettings();

            m_bShowImages   = rSettings.GetUseImagesInMenus();
            m_bHiContrast   = rSettings.GetHighContrastMode();

            m_bNewMenu      = m_aCommandURL.equalsAscii( ".uno:AddDirect" );
        }
    }
}

IMPL_STATIC_LINK_NOINSTANCE( NewMenuController, ExecuteHdl_Impl, NewDocument*, pNewDocument )
{
/*  i62706: Don't catch all exceptions. We hide all problems here and are not able
            to handle them on higher levels.
    try
    {
*/
        // Asynchronous execution as this can lead to our own destruction!
        // Framework can recycle our current frame and the layout manager disposes all user interface
        // elements if a component gets detached from its frame!
        pNewDocument->xDispatch->dispatch( pNewDocument->aTargetURL, pNewDocument->aArgSeq );
/*
    }
    catch (const ::com::sun::star::document::CorruptedFilterConfigurationException& exFilters)
    {
        throw exFilters;
    }
    catch (const Exception& )
    {
    }
*/
    delete pNewDocument;
    return 0;
}

}
