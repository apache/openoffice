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
#include <uielement/toolbarsmenucontroller.hxx>

#include <algorithm>

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________
#include <threadhelp/resetableguard.hxx>
#include "services.h"
#ifndef __FRAMEWORK_CLASSES_RESOURCE_HRC_
#include <classes/resource.hrc>
#endif
#include <classes/fwkresid.hxx>
#include <uiconfiguration/windowstateconfiguration.hxx>
#include <framework/imageproducer.hxx>
#include <framework/sfxhelperfunctions.hxx>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/awt/XDevice.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/awt/MenuItemStyle.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XLayoutManager.hpp>
#include <com/sun/star/ui/XUIElementSettings.hpp>
#include <com/sun/star/ui/XModuleUIConfigurationManagerSupplier.hpp>
#include <com/sun/star/ui/XUIConfigurationManagerSupplier.hpp>
#include <com/sun/star/ui/UIElementType.hpp>

//_________________________________________________________________________________________________________________
//	includes of other projects
//_________________________________________________________________________________________________________________

#ifndef _VCL_MENU_HXX_
#include <vcl/menu.hxx>
#endif
#include <vcl/svapp.hxx>
#include <vcl/i18nhelp.hxx>
#include <vcl/image.hxx>
#include <tools/urlobj.hxx>
#include <rtl/ustrbuf.hxx>
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/helper/vclunohelper.hxx>
#endif
#include <vcl/window.hxx>
#include <svtools/menuoptions.hxx>
#include <unotools/cmdoptions.hxx>
#include <dispatch/uieventloghelper.hxx>
#include <rtl/logfile.hxx>

//_________________________________________________________________________________________________________________
//	Defines
//_________________________________________________________________________________________________________________
//

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::ui;

static const char CONFIGURE_TOOLBARS_CMD[]      = "ConfigureDialog";
static const char CONFIGURE_TOOLBARS[]          = ".uno:ConfigureDialog";
static const char CMD_COLORBAR[]                = ".uno:ColorControl";
static const char CMD_HYPERLINKBAR[]            = ".uno:InsertHyperlink";
static const char CMD_FORMULABAR[]              = ".uno:InsertFormula";
static const char CMD_INPUTLINEBAR[]            = ".uno:InputLineVisible";
static const char CMD_RESTOREVISIBILITY[]       = ".cmd:RestoreVisibility";
static const char ITEM_DESCRIPTOR_RESOURCEURL[] = "ResourceURL";
static const char ITEM_DESCRIPTOR_UINAME[]		= "UIName";
static const char STATIC_PRIVATE_TB_RESOURCE[]  = "private:resource/toolbar/";

static const char STATIC_CMD_PART[]             = ".uno:AvailableToolbars?Toolbar:string=";
static const char STATIC_INTERNAL_CMD_PART[]    = ".cmd:";

namespace framework
{

typedef std::hash_map< rtl::OUString, rtl::OUString, rtl::OUStringHash, ::std::equal_to< ::rtl::OUString > > ToolbarHashMap;

struct ToolBarEntry
{
    rtl::OUString           aUIName;
    rtl::OUString           aCommand;
    sal_Bool                bVisible;
    sal_Bool                bContextSensitive;
    const CollatorWrapper*  pCollatorWrapper;
};

sal_Bool CompareToolBarEntry( const ToolBarEntry& aOne, const ToolBarEntry& aTwo )
{
    sal_Int32 nComp = aOne.pCollatorWrapper->compareString( aOne.aUIName, aTwo.aUIName );

    if ( nComp < 0 )
		return sal_True;
	else if ( nComp > 0 )
		return sal_False;
	else
		return sal_False;
}

Reference< XLayoutManager > getLayoutManagerFromFrame( const Reference< XFrame >& rFrame )
{
    Reference< XPropertySet >   xPropSet( rFrame, UNO_QUERY );
    Reference< XLayoutManager > xLayoutManager;

    try
    {
	    xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "LayoutManager" ))) >>= xLayoutManager;
    }
    catch ( UnknownPropertyException& )
    {
    }

    return xLayoutManager;
}

struct ToolBarInfo
{
    rtl::OUString aToolBarResName;
    rtl::OUString aToolBarUIName;
};

DEFINE_XSERVICEINFO_MULTISERVICE        (   ToolbarsMenuController				    ,
                                            OWeakObject                             ,
                                            SERVICENAME_POPUPMENUCONTROLLER		    ,
											IMPLEMENTATIONNAME_TOOLBARSMENUCONTROLLER
										)

DEFINE_INIT_SERVICE                     (   ToolbarsMenuController, {} )

ToolbarsMenuController::ToolbarsMenuController( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager ) :
	svt::PopupMenuControllerBase( xServiceManager ),
    m_aPropUIName( RTL_CONSTASCII_USTRINGPARAM( "UIName" )),
    m_aPropResourceURL( RTL_CONSTASCII_USTRINGPARAM( "ResourceURL" )),
    m_bModuleIdentified( sal_False ),
    m_bResetActive( sal_False ),
    m_aIntlWrapper( xServiceManager, Application::GetSettings().GetLocale() )
{
}

ToolbarsMenuController::~ToolbarsMenuController()
{
}

void ToolbarsMenuController::addCommand(
    Reference< css::awt::XPopupMenu >& rPopupMenu, const rtl::OUString& rCommandURL, const rtl::OUString& rLabel )
{
    sal_uInt16        nItemId    = m_xPopupMenu->getItemCount()+1;

    rtl::OUString aLabel;
    if ( rLabel.getLength() == 0 )
        aLabel = getUINameFromCommand( rCommandURL );
    else
        aLabel = rLabel;

	rPopupMenu->insertItem( nItemId, aLabel, 0, nItemId );
    rPopupMenu->setCommand( nItemId, rCommandURL );

    bool bInternal = ( rCommandURL.indexOf( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( STATIC_INTERNAL_CMD_PART ))) == 0);
    if ( !bInternal )
    {
        if ( !getDispatchFromCommandURL( rCommandURL ).is() )
            m_xPopupMenu->enableItem( nItemId, sal_False );
    }

    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

    Image                aImage;
	const StyleSettings& rSettings = Application::GetSettings().GetStyleSettings();

    if ( rSettings.GetUseImagesInMenus() )
        aImage = GetImageFromURL( m_xFrame, rCommandURL, sal_False, rSettings.GetHighContrastMode() );

    VCLXPopupMenu* pPopupMenu = (VCLXPopupMenu *)VCLXPopupMenu::GetImplementation( rPopupMenu );
    if ( pPopupMenu )
    {
        PopupMenu* pVCLPopupMenu = (PopupMenu *)pPopupMenu->GetMenu();
        if ( !!aImage )
            pVCLPopupMenu->SetItemImage( nItemId, aImage );
    }

    m_aCommandVector.push_back( rCommandURL );
}

Reference< XDispatch > ToolbarsMenuController::getDispatchFromCommandURL( const rtl::OUString& rCommandURL )
{
    URL                          aTargetURL;
	Sequence<PropertyValue>	     aArgs;
    Reference< XURLTransformer > xURLTransformer;
    Reference< XFrame >          xFrame;

    {
        vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
        xURLTransformer = m_xURLTransformer;
        xFrame = m_xFrame;
    }

    aTargetURL.Complete = rCommandURL;
    xURLTransformer->parseStrict( aTargetURL );
	Reference< XDispatchProvider > xDispatchProvider( xFrame, UNO_QUERY );
    if ( xDispatchProvider.is() )
        return xDispatchProvider->queryDispatch( aTargetURL, ::rtl::OUString(), 0 );
    else
        return Reference< XDispatch >();
}

// private function
rtl::OUString ToolbarsMenuController::getUINameFromCommand( const rtl::OUString& rCommandURL )
{
    rtl::OUString aLabel;

    if ( !m_bModuleIdentified  )
    {
		try
        {
            Reference< XModuleManager > xModuleManager( m_xServiceManager->createInstance( SERVICENAME_MODULEMANAGER ), UNO_QUERY_THROW );
            m_aModuleIdentifier = xModuleManager->identify( m_xFrame );
            Reference< XNameAccess > xNameAccess( m_xServiceManager->createInstance(
                                                                    SERVICENAME_UICOMMANDDESCRIPTION ),
                                                                UNO_QUERY );
            xNameAccess->getByName( m_aModuleIdentifier ) >>= m_xUICommandDescription;
        }
        catch ( Exception& )
        {
        }
    }

    if ( m_xUICommandDescription.is() )
    {
        try
        {
            Sequence< PropertyValue > aPropSeq;
            rtl::OUString             aStr;
            if ( m_xUICommandDescription->getByName( rCommandURL ) >>= aPropSeq )
            {
                for ( sal_Int32 i = 0; i < aPropSeq.getLength(); i++ )
                {
                    if ( aPropSeq[i].Name.equalsAscii( "Label" ))
                    {
                        aPropSeq[i].Value >>= aStr;
                        break;
                    }
                }
            }
            aLabel = aStr;
        }
        catch ( Exception& )
        {
        }
    }

    return aLabel;
}

static void fillHashMap( const Sequence< Sequence< ::com::sun::star::beans::PropertyValue > >& rSeqToolBars,
                         ToolbarHashMap& rHashMap )
{
    for ( sal_Int32 i = 0; i < rSeqToolBars.getLength(); i++ )
    {
        rtl::OUString aResourceURL;
        rtl::OUString aUIName;
        const PropertyValue* pProperties = rSeqToolBars[i].getConstArray();
        for ( sal_Int32 j = 0; j < rSeqToolBars[i].getLength(); j++ )
		{
			if ( pProperties[j].Name.equalsAscii( ITEM_DESCRIPTOR_RESOURCEURL) )
				pProperties[j].Value >>= aResourceURL;
			else if ( pProperties[j].Name.equalsAscii( ITEM_DESCRIPTOR_UINAME) )
                pProperties[j].Value >>= aUIName;
        }

        if ( aResourceURL.getLength() > 0 &&
             rHashMap.find( aResourceURL ) == rHashMap.end() )
            rHashMap.insert( ToolbarHashMap::value_type( aResourceURL, aUIName ));
    }
}

// private function
Sequence< Sequence< com::sun::star::beans::PropertyValue > > ToolbarsMenuController::getLayoutManagerToolbars( const Reference< ::com::sun::star::frame::XLayoutManager >& rLayoutManager )
{
    std::vector< ToolBarInfo > aToolBarArray;
    Sequence< Reference< XUIElement > > aUIElements = rLayoutManager->getElements();
    for ( sal_Int32 i = 0; i < aUIElements.getLength(); i++ )
    {
        Reference< XUIElement > xUIElement( aUIElements[i] );
        Reference< XPropertySet > xPropSet( aUIElements[i], UNO_QUERY );
        if ( xPropSet.is() && xUIElement.is() )
        {
            try
            {
                rtl::OUString   aResName;
                sal_Int16       nType( -1 );
                xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Type" ))) >>= nType;
                xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ResourceURL" ))) >>= aResName;

                if (( nType == ::com::sun::star::ui::UIElementType::TOOLBAR ) &&
                    ( aResName.getLength() > 0 ))
                {
		            ToolBarInfo aToolBarInfo;

                    aToolBarInfo.aToolBarResName = aResName;

                    vos::OGuard	aGuard( Application::GetSolarMutex() );
                    Reference< css::awt::XWindow > xWindow( xUIElement->getRealInterface(), UNO_QUERY );
                    Window* pWindow = VCLUnoHelper::GetWindow( xWindow );
                    if ( pWindow )
                        aToolBarInfo.aToolBarUIName = pWindow->GetText();

                    aToolBarArray.push_back( aToolBarInfo );
                }
            }
            catch ( Exception& )
            {
            }
        }
    }

    Sequence< com::sun::star::beans::PropertyValue > aTbSeq( 2 );
    aTbSeq[0].Name = m_aPropUIName;
    aTbSeq[1].Name = m_aPropResourceURL;

    Sequence< Sequence< com::sun::star::beans::PropertyValue > > aSeq( aToolBarArray.size() );
    const sal_uInt32 nCount = aToolBarArray.size();
    for ( sal_uInt32 i = 0; i < nCount; i++ )
    {
        aTbSeq[0].Value <<= aToolBarArray[i].aToolBarUIName;
        aTbSeq[1].Value <<= aToolBarArray[i].aToolBarResName;
        aSeq[i] = aTbSeq;
    }

    return aSeq;
}

sal_Bool ToolbarsMenuController::isContextSensitiveToolbarNonVisible()
{
    return m_bResetActive;
}

void ToolbarsMenuController::fillPopupMenu( Reference< css::awt::XPopupMenu >& rPopupMenu )
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
    resetPopupMenu( rPopupMenu );

    m_aCommandVector.clear();

    // Retrieve layout manager for additional information
    rtl::OUString aEmptyString;
    Reference< XLayoutManager > xLayoutManager( getLayoutManagerFromFrame( m_xFrame ));

    m_bResetActive = sal_False;
    if ( xLayoutManager.is() )
    {
        ToolbarHashMap aToolbarHashMap;

        if ( m_xDocCfgMgr.is() )
        {
            Sequence< Sequence< com::sun::star::beans::PropertyValue > > aSeqDocToolBars =
                m_xDocCfgMgr->getUIElementsInfo( UIElementType::TOOLBAR );
            fillHashMap( aSeqDocToolBars, aToolbarHashMap );
        }

        if ( m_xModuleCfgMgr.is() )
        {
            Sequence< Sequence< com::sun::star::beans::PropertyValue > > aSeqToolBars =
                m_xModuleCfgMgr->getUIElementsInfo( UIElementType::TOOLBAR );
            fillHashMap( aSeqToolBars, aToolbarHashMap );
        }

        std::vector< ToolBarEntry > aSortedTbs;
        rtl::OUString               aStaticCmdPart( RTL_CONSTASCII_USTRINGPARAM( STATIC_CMD_PART ));

        Sequence< Sequence< com::sun::star::beans::PropertyValue > > aSeqFrameToolBars = getLayoutManagerToolbars( xLayoutManager );
        fillHashMap( aSeqFrameToolBars, aToolbarHashMap );

        ToolbarHashMap::const_iterator pIter = aToolbarHashMap.begin();
        while ( pIter != aToolbarHashMap.end() )
        {
            rtl::OUString aUIName = pIter->second;
            sal_Bool      bHideFromMenu( sal_False );
            sal_Bool      bContextSensitive( sal_False );
            sal_Bool      bVisible( sal_False );
            if ( aUIName.getLength() == 0 &&
                 m_xPersistentWindowState.is() )
            {
                try
                {
                    Sequence< PropertyValue > aWindowState;
                    Any                       a( m_xPersistentWindowState->getByName( pIter->first ));

                    if ( a >>= aWindowState )
                    {
                        for ( sal_Int32 i = 0; i < aWindowState.getLength(); i++ )
                        {
                            if ( aWindowState[i].Name.equalsAscii( WINDOWSTATE_PROPERTY_UINAME ))
                                aWindowState[i].Value >>= aUIName;
                            else if ( aWindowState[i].Name.equalsAscii( WINDOWSTATE_PROPERTY_HIDEFROMENU ))
                                aWindowState[i].Value >>= bHideFromMenu;
                            else if ( aWindowState[i].Name.equalsAscii( WINDOWSTATE_PROPERTY_CONTEXT ))
                                aWindowState[i].Value >>= bContextSensitive;
                            else if ( aWindowState[i].Name.equalsAscii( WINDOWSTATE_PROPERTY_VISIBLE ))
                                aWindowState[i].Value >>= bVisible;
                        }
                    }
                }
                catch ( Exception& )
                {
                }

                // Check if we have to enable/disable "Reset" menu item
                if ( bContextSensitive && !bVisible )
                    m_bResetActive = sal_True;

            }

            if (( aUIName.getLength() > 0 ) && ( !bHideFromMenu ))
            {
                ToolBarEntry aTbEntry;
                aTbEntry.aUIName = aUIName;
                aTbEntry.aCommand = pIter->first;
                aTbEntry.bVisible = xLayoutManager->isElementVisible( pIter->first );
                aTbEntry.bContextSensitive = bContextSensitive;
                aTbEntry.pCollatorWrapper = m_aIntlWrapper.getCaseCollator();
                aSortedTbs.push_back( aTbEntry );
            }
            pIter++;
        }

        // sort toolbars
        std::sort( aSortedTbs.begin(), aSortedTbs.end(), CompareToolBarEntry );

        sal_Int16 nIndex( 1 );
        const sal_uInt32 nCount = aSortedTbs.size();
        for ( sal_uInt32 i = 0; i < nCount; i++ )
        {
            sal_uInt16 nItemCount = m_xPopupMenu->getItemCount();
            m_xPopupMenu->insertItem( nIndex, aSortedTbs[i].aUIName, css::awt::MenuItemStyle::CHECKABLE, nItemCount );
            if ( aSortedTbs[i].bVisible )
                m_xPopupMenu->checkItem( nIndex, sal_True );

            {
                vos::OGuard	aGuard( Application::GetSolarMutex() );
                VCLXPopupMenu* pXPopupMenu = (VCLXPopupMenu *)VCLXMenu::GetImplementation( m_xPopupMenu );
                PopupMenu*     pVCLPopupMenu = (PopupMenu *)pXPopupMenu->GetMenu();

                pVCLPopupMenu->SetUserValue( nIndex, sal_uIntPtr( aSortedTbs[i].bContextSensitive ? 1L : 0L ));
            }

            // use VCL popup menu pointer to set vital information that are not part of the awt implementation
            rtl::OUStringBuffer aStrBuf( aStaticCmdPart );

            sal_Int32 n = aSortedTbs[i].aCommand.lastIndexOf( '/' );
            if (( n > 0 ) && (( n+1 ) < aSortedTbs[i].aCommand.getLength() ))
                aStrBuf.append( aSortedTbs[i].aCommand.copy( n+1 ));

            rtl::OUString aCmd( aStrBuf.makeStringAndClear() );

            // Store complete uno-command so it can also be dispatched. This is necessary to support
            // the test tool!
		    rPopupMenu->setCommand( nIndex, aCmd );
            ++nIndex;
        }

        // Create commands for non-toolbars
        if ( m_aModuleIdentifier.equalsAscii( "com.sun.star.text.TextDocument" ) ||
             m_aModuleIdentifier.equalsAscii( "com.sun.star.text.WebDocument" ) ||
             m_aModuleIdentifier.equalsAscii( "com.sun.star.text.GlobalDocument" ) ||
             m_aModuleIdentifier.equalsAscii( "com.sun.star.drawing.DrawingDocument" ) ||
             m_aModuleIdentifier.equalsAscii( "com.sun.star.presentation.PresentationDocument" ) ||
             m_aModuleIdentifier.equalsAscii( "com.sun.star.sheet.SpreadsheetDocument" ))
        {
            addCommand( m_xPopupMenu, rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( CMD_HYPERLINKBAR )), aEmptyString );
            if ( m_aModuleIdentifier.equalsAscii( "com.sun.star.drawing.DrawingDocument" ) ||
                 m_aModuleIdentifier.equalsAscii( "com.sun.star.presentation.PresentationDocument" ))
                addCommand( m_xPopupMenu, rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( CMD_COLORBAR )), aEmptyString );
            else if ( m_aModuleIdentifier.equalsAscii( "com.sun.star.sheet.SpreadsheetDocument" ))
                addCommand( m_xPopupMenu, rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( CMD_INPUTLINEBAR )), aEmptyString );
            else
                addCommand( m_xPopupMenu, rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( CMD_FORMULABAR )), aEmptyString );
        }

        sal_Bool          bAddCommand( sal_True );
        SvtCommandOptions aCmdOptions;
        rtl::OUString     aConfigureToolbar( RTL_CONSTASCII_USTRINGPARAM( CONFIGURE_TOOLBARS ));

        if ( aCmdOptions.HasEntries( SvtCommandOptions::CMDOPTION_DISABLED ))
        {
            if ( aCmdOptions.Lookup( SvtCommandOptions::CMDOPTION_DISABLED,
                                     rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( CONFIGURE_TOOLBARS_CMD ))))
                bAddCommand = sal_False;
        }

        if ( bAddCommand )
        {
            // Create command for configure
            if ( m_xPopupMenu->getItemCount() > 0 )
            {
                sal_uInt16        nItemCount = m_xPopupMenu->getItemCount();
                m_xPopupMenu->insertSeparator( nItemCount+1 );
            }

            addCommand( m_xPopupMenu, aConfigureToolbar, aEmptyString );
        }

        // Add separator if no configure has been added
        if ( !bAddCommand )
        {
            // Create command for configure
            if ( m_xPopupMenu->getItemCount() > 0 )
            {
                sal_uInt16        nItemCount = m_xPopupMenu->getItemCount();
                m_xPopupMenu->insertSeparator( nItemCount+1 );
            }
        }

        String aLabelStr = String( FwkResId( STR_RESTORE_TOOLBARS ));
        rtl::OUString aRestoreCmd( RTL_CONSTASCII_USTRINGPARAM( CMD_RESTOREVISIBILITY ));
        addCommand( m_xPopupMenu, aRestoreCmd, aLabelStr );
    }
}

// XEventListener
void SAL_CALL ToolbarsMenuController::disposing( const EventObject& ) throw ( RuntimeException )
{
    Reference< css::awt::XMenuListener > xHolder(( OWeakObject *)this, UNO_QUERY );

    osl::MutexGuard aLock( m_aMutex );
    m_xFrame.clear();
    m_xDispatch.clear();
    m_xDocCfgMgr.clear();
    m_xModuleCfgMgr.clear();
    m_xServiceManager.clear();

    if ( m_xPopupMenu.is() )
        m_xPopupMenu->removeMenuListener( Reference< css::awt::XMenuListener >(( OWeakObject *)this, UNO_QUERY ));
    m_xPopupMenu.clear();
}

// XStatusListener
void SAL_CALL ToolbarsMenuController::statusChanged( const FeatureStateEvent& Event ) throw ( RuntimeException )
{
    rtl::OUString aFeatureURL( Event.FeatureURL.Complete );

    // All other status events will be processed here
    sal_Bool bSetCheckmark      = sal_False;
	sal_Bool bCheckmark			= sal_False;

    osl::ClearableMutexGuard aLock( m_aMutex );
    Reference< css::awt::XPopupMenu > xPopupMenu( m_xPopupMenu );
    aLock.clear();

	if ( xPopupMenu.is() )
    {
        vos::OGuard	aGuard( Application::GetSolarMutex() );
        VCLXPopupMenu* pXPopupMenu = (VCLXPopupMenu *)VCLXMenu::GetImplementation( xPopupMenu );
        PopupMenu*     pVCLPopupMenu = (PopupMenu *)pXPopupMenu->GetMenu();

        for ( sal_uInt16 i = 0; i < pVCLPopupMenu->GetItemCount(); i++ )
        {
            sal_uInt16 nId = pVCLPopupMenu->GetItemId( i );
            if ( nId == 0 )
                continue;

            rtl::OUString aCmd = pVCLPopupMenu->GetItemCommand( nId );
            if ( aCmd == aFeatureURL )
            {
                // Enable/disable item
                pVCLPopupMenu->EnableItem( nId, Event.IsEnabled );

                // Checkmark
                if ( Event.State >>= bCheckmark )
				    bSetCheckmark = sal_True;

                if ( bSetCheckmark )
                    pVCLPopupMenu->CheckItem( nId, bCheckmark );
                else
                {
                    rtl::OUString aItemText;

                    if ( Event.State >>= aItemText )
                        pVCLPopupMenu->SetItemText( nId, aItemText );
                }
            }
        }
    }
}

// XMenuListener
void SAL_CALL ToolbarsMenuController::itemSelected( const css::awt::MenuEvent& rEvent ) throw (RuntimeException)
{
    Reference< css::awt::XPopupMenu >   xPopupMenu;
    Reference< XMultiServiceFactory >   xServiceManager;
    Reference< XURLTransformer >        xURLTransformer;
    Reference< XFrame >                 xFrame;
    Reference< XNameAccess >            xPersistentWindowState;

    osl::ClearableMutexGuard aLock( m_aMutex );
    xPopupMenu             = m_xPopupMenu;
    xServiceManager        = m_xServiceManager;
    xURLTransformer        = m_xURLTransformer;
    xFrame                 = m_xFrame;
    xPersistentWindowState = m_xPersistentWindowState;
    aLock.clear();

    if ( xPopupMenu.is() )
    {
        VCLXPopupMenu* pPopupMenu = (VCLXPopupMenu *)VCLXPopupMenu::GetImplementation( xPopupMenu );
        if ( pPopupMenu )
        {
            vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );
            PopupMenu* pVCLPopupMenu = (PopupMenu *)pPopupMenu->GetMenu();

            rtl::OUString aCmd( pVCLPopupMenu->GetItemCommand( rEvent.MenuId ));
            if ( aCmd.indexOf( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( STATIC_INTERNAL_CMD_PART ))) == 0 )
            {
                // Command to restore the visibility of all context sensitive toolbars
                Reference< XNameReplace > xNameReplace( xPersistentWindowState, UNO_QUERY );
                if ( xPersistentWindowState.is() && xNameReplace.is() )
                {
                    try
                    {
                        Sequence< rtl::OUString > aElementNames = xPersistentWindowState->getElementNames();
                        sal_Int32 nCount = aElementNames.getLength();
                        bool      bRefreshToolbars( false );

                        for ( sal_Int32 i = 0; i < nCount; i++ )
                        {
                            try
                            {
                                rtl::OUString aElementName = aElementNames[i];
                                Sequence< PropertyValue > aWindowState;

                                if ( xPersistentWindowState->getByName( aElementName ) >>= aWindowState )
                                {
                                    sal_Bool  bVisible( sal_False );
                                    sal_Bool  bContextSensitive( sal_False );
                                    sal_Int32 nVisibleIndex( -1 );
                                    for ( sal_Int32 j = 0; j < aWindowState.getLength(); j++ )
                                    {
                                        if ( aWindowState[j].Name.equalsAscii( WINDOWSTATE_PROPERTY_VISIBLE ))
                                        {
                                            aWindowState[j].Value >>= bVisible;
                                            nVisibleIndex = j;
                                        }
                                        else if ( aWindowState[j].Name.equalsAscii( WINDOWSTATE_PROPERTY_CONTEXT ))
                                            aWindowState[j].Value >>= bContextSensitive;
                                    }

                                    if ( !bVisible && bContextSensitive && nVisibleIndex >= 0 )
                                    {
                                        // Default is: Every context sensitive toolbar is visible
                                        aWindowState[nVisibleIndex].Value <<= sal_True;
                                        xNameReplace->replaceByName( aElementName, makeAny( aWindowState ));
                                        bRefreshToolbars = true;
                                    }
                                }
                            }
                            catch ( NoSuchElementException& )
                            {
                            }
                        }

                        if ( bRefreshToolbars )
                        {
                            Reference< XLayoutManager > xLayoutManager( getLayoutManagerFromFrame( xFrame ));
                            if ( xLayoutManager.is() )
                            {
                                Reference< XPropertySet > xPropSet( xLayoutManager, UNO_QUERY );
                                if ( xPropSet.is() )
                                {
                                    try
                                    {
                                        xPropSet->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "RefreshContextToolbarVisibility" )), makeAny( sal_True ));
                                    }
                                    catch ( RuntimeException& )
                                    {
                                    }
                                    catch ( Exception& )
                                    {
                                    }
                                }
                            }
                            RefreshToolbars( xFrame );
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
            }
            else if ( aCmd.indexOf( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( STATIC_CMD_PART ))) < 0 )
            {
                URL                     aTargetURL;
                Sequence<PropertyValue>	aArgs;

                aTargetURL.Complete = aCmd;
                xURLTransformer->parseStrict( aTargetURL );
		        Reference< XDispatchProvider > xDispatchProvider( m_xFrame, UNO_QUERY );
                if ( xDispatchProvider.is() )
                {
		            Reference< XDispatch > xDispatch = xDispatchProvider->queryDispatch(
														    aTargetURL, ::rtl::OUString(), 0 );

                    ExecuteInfo* pExecuteInfo = new ExecuteInfo;
                    pExecuteInfo->xDispatch     = xDispatch;
                    pExecuteInfo->aTargetURL    = aTargetURL;
                    pExecuteInfo->aArgs         = aArgs;
                    if(::comphelper::UiEventsLogger::isEnabled()) //#i88653#
                        UiEventLogHelper(::rtl::OUString::createFromAscii("ToolbarsMenuController")).log(m_xServiceManager, m_xFrame, aTargetURL, aArgs);
                    Application::PostUserEvent( STATIC_LINK(0, ToolbarsMenuController, ExecuteHdl_Impl), pExecuteInfo );
                }
            }
            else
            {
                Reference< XLayoutManager > xLayoutManager( getLayoutManagerFromFrame( xFrame ));
                if ( xLayoutManager.is() )
                {
                    // Extract toolbar name from the combined uno-command.
                    sal_Int32 nIndex = aCmd.indexOf( '=' );
                    if (( nIndex > 0 ) && (( nIndex+1 ) < aCmd.getLength() ))
                    {
                        rtl::OUStringBuffer aBuf( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( STATIC_PRIVATE_TB_RESOURCE )));
                        aBuf.append( aCmd.copy( nIndex+1 ));

                        sal_Bool      bShow( !pVCLPopupMenu->IsItemChecked( rEvent.MenuId ));
                        rtl::OUString aToolBarResName( aBuf.makeStringAndClear() );
                        if ( bShow )
                        {
                            xLayoutManager->createElement( aToolBarResName );
                            xLayoutManager->showElement( aToolBarResName );
                        }
                        else
                        {
                            // closing means:
                            // hide and destroy element
                            xLayoutManager->hideElement( aToolBarResName );
                            xLayoutManager->destroyElement( aToolBarResName );
                        }
                    }
                }
            }
        }
    }
}

void SAL_CALL ToolbarsMenuController::itemActivated( const css::awt::MenuEvent& ) throw (RuntimeException)
{
    std::vector< rtl::OUString >   aCmdVector;
    Reference< XDispatchProvider > xDispatchProvider( m_xFrame, UNO_QUERY );
    Reference< XURLTransformer >   xURLTransformer( m_xURLTransformer );
    {
        osl::MutexGuard aLock( m_aMutex );
        fillPopupMenu( m_xPopupMenu );
        aCmdVector = m_aCommandVector;
    }

    // Update status for all commands inside our toolbars popup menu
    const sal_uInt32 nCount = aCmdVector.size();
    for ( sal_uInt32 i = 0; i < nCount; i++ )
    {
        bool bInternal = ( aCmdVector[i].indexOf( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( STATIC_INTERNAL_CMD_PART ))) == 0);

        if ( !bInternal )
        {
            URL aTargetURL;
            aTargetURL.Complete = aCmdVector[i];
            xURLTransformer->parseStrict( aTargetURL );
            Reference< XDispatch > xDispatch = xDispatchProvider->queryDispatch( aTargetURL, ::rtl::OUString(), 0 );
            if ( xDispatch.is() )
            {
			    xDispatch->addStatusListener( SAL_STATIC_CAST( XStatusListener*, this ), aTargetURL );
			    xDispatch->removeStatusListener( SAL_STATIC_CAST( XStatusListener*, this ), aTargetURL );
            }
        }
        else if ( aCmdVector[i].equalsAscii( CMD_RESTOREVISIBILITY ))
        {
            // Special code to determine the enable/disable state of this command
            FeatureStateEvent aFeatureStateEvent;
            aFeatureStateEvent.FeatureURL.Complete = aCmdVector[i];
            aFeatureStateEvent.IsEnabled = isContextSensitiveToolbarNonVisible();
            statusChanged( aFeatureStateEvent );
        }
    }
}

// XPopupMenuController
void SAL_CALL ToolbarsMenuController::setPopupMenu( const Reference< css::awt::XPopupMenu >& xPopupMenu ) throw ( RuntimeException )
{
    osl::MutexGuard aLock( m_aMutex );

    throwIfDisposed();

    if ( m_xFrame.is() && !m_xPopupMenu.is() )
    {
        // Create popup menu on demand
        vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

        m_xPopupMenu = xPopupMenu;
	    m_xPopupMenu->addMenuListener( Reference< css::awt::XMenuListener >( (OWeakObject*)this, UNO_QUERY ));
        fillPopupMenu( m_xPopupMenu );
    }
}

// XInitialization
void SAL_CALL ToolbarsMenuController::initialize( const Sequence< Any >& aArguments ) throw ( Exception, RuntimeException )
{
    osl::MutexGuard aLock( m_aMutex );
    sal_Bool bInitialized( m_bInitialized );
    if ( !bInitialized )
    {
		svt::PopupMenuControllerBase::initialize(aArguments);

        if ( m_bInitialized )
        {
            Reference< XModuleManager > xModuleManager( m_xServiceManager->createInstance(
                                                            SERVICENAME_MODULEMANAGER ),
                                                        UNO_QUERY );
            Reference< XNameAccess > xPersistentWindowStateSupplier( m_xServiceManager->createInstance(
                                                                        SERVICENAME_WINDOWSTATECONFIGURATION ),
                                                                     UNO_QUERY );

            // Retrieve persistent window state reference for our module
            if ( xPersistentWindowStateSupplier.is() && xModuleManager.is() )
            {
                rtl::OUString aModuleIdentifier;
                try
                {
                    aModuleIdentifier = xModuleManager->identify( m_xFrame );
                    xPersistentWindowStateSupplier->getByName( aModuleIdentifier ) >>= m_xPersistentWindowState;

                    Reference< XModuleUIConfigurationManagerSupplier > xModuleCfgSupplier;
                    if ( m_xServiceManager.is() )
                        xModuleCfgSupplier = Reference< XModuleUIConfigurationManagerSupplier >(
                            m_xServiceManager->createInstance( SERVICENAME_MODULEUICONFIGURATIONMANAGERSUPPLIER ), UNO_QUERY );
                    m_xModuleCfgMgr = xModuleCfgSupplier->getUIConfigurationManager( aModuleIdentifier );

                    Reference< XController > xController = m_xFrame->getController();
                    Reference< XModel >      xModel;
                    if ( xController.is() )
                        xModel = xController->getModel();
                    if ( xModel.is() )
                    {
                        Reference< XUIConfigurationManagerSupplier > xUIConfigurationManagerSupplier( xModel, UNO_QUERY );
                        if ( xUIConfigurationManagerSupplier.is() )
                            m_xDocCfgMgr = xUIConfigurationManagerSupplier->getUIConfigurationManager();
                    }
                    m_aModuleIdentifier = aModuleIdentifier;
                }
                catch ( Exception& )
                {
                }
            }
        }
    }
}

IMPL_STATIC_LINK_NOINSTANCE( ToolbarsMenuController, ExecuteHdl_Impl, ExecuteInfo*, pExecuteInfo )
{
    try
    {
        // Asynchronous execution as this can lead to our own destruction!
        // Framework can recycle our current frame and the layout manager disposes all user interface
        // elements if a component gets detached from its frame!
        if ( pExecuteInfo->xDispatch.is() )
        {
            pExecuteInfo->xDispatch->dispatch( pExecuteInfo->aTargetURL, pExecuteInfo->aArgs );
        }
    }
    catch ( Exception& )
    {
    }

    delete pExecuteInfo;
    return 0;
}

}
