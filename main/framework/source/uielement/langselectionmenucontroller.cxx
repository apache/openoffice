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

#include <uielement/langselectionmenucontroller.hxx>

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________
#include <threadhelp/resetableguard.hxx>
#include "services.h"

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/awt/XDevice.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/awt/MenuItemStyle.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>

//_________________________________________________________________________________________________________________
//	includes of other projects
//_________________________________________________________________________________________________________________

#ifndef _VCL_MENU_HXX_
#include <vcl/menu.hxx>
#endif
#include <vcl/svapp.hxx>
#include <vcl/i18nhelp.hxx>
#include <tools/urlobj.hxx>
#include <rtl/ustrbuf.hxx>
#ifndef _VCL_MNEMONIC_HXX_
#include <vcl/mnemonic.hxx>
#endif
#include <comphelper/processfactory.hxx>

#include <com/sun/star/document/XDocumentLanguages.hpp>
#include <com/sun/star/frame/XPopupMenuController.hpp>
#include <com/sun/star/linguistic2/XLanguageGuessing.hpp>

#include <i18npool/mslangid.hxx>
#include <svl/languageoptions.hxx>
#include <com/sun/star/awt/MenuItemStyle.hpp>
#include <svtools/langtab.hxx>
#include <classes/fwlresid.hxx>

#ifndef __FRAMEWORK_CLASSES_RESOURCE_HRC_
#include <classes/resource.hrc>
#endif
#include <dispatch/uieventloghelper.hxx>

#include "helper/mischelper.hxx"
#include <vos/mutex.hxx>

#include <map>
#include <set>

//_________________________________________________________________________________________________________________
//	Defines
//_________________________________________________________________________________________________________________
//
using namespace ::com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::frame;
using namespace com::sun::star::beans;
using namespace com::sun::star::util;

using ::rtl::OUString;

namespace framework
{

DEFINE_XSERVICEINFO_MULTISERVICE        (   LanguageSelectionMenuController			,
                                            OWeakObject                             ,
                                            SERVICENAME_POPUPMENUCONTROLLER			,
											IMPLEMENTATIONNAME_LANGUAGESELECTIONMENUCONTROLLER
										)

DEFINE_INIT_SERVICE                     (   LanguageSelectionMenuController, {} )

LanguageSelectionMenuController::LanguageSelectionMenuController( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager ) :
	svt::PopupMenuControllerBase( xServiceManager ),
	m_bShowMenu( sal_True ),
    m_aLangGuessHelper( xServiceManager )
{
}

LanguageSelectionMenuController::~LanguageSelectionMenuController()
{
}

// XEventListener
void SAL_CALL LanguageSelectionMenuController::disposing( const EventObject& ) throw ( RuntimeException )
{
    Reference< css::awt::XMenuListener > xHolder(( OWeakObject *)this, UNO_QUERY );

    osl::MutexGuard aLock( m_aMutex );
    m_xFrame.clear();
    m_xDispatch.clear();
    m_xLanguageDispatch.clear();
    m_xServiceManager.clear();

    if ( m_xPopupMenu.is() )
        m_xPopupMenu->removeMenuListener( Reference< css::awt::XMenuListener >(( OWeakObject *)this, UNO_QUERY ));
    m_xPopupMenu.clear();
}

// XStatusListener
void SAL_CALL LanguageSelectionMenuController::statusChanged( const FeatureStateEvent& Event ) throw ( RuntimeException )
{
    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

    if (rBHelper.bDisposed || rBHelper.bInDispose)
        return;

	m_bShowMenu = sal_True;
	m_nScriptType = LS_SCRIPT_LATIN | LS_SCRIPT_ASIAN | LS_SCRIPT_COMPLEX;  //set the default value

    OUString               aStrValue;
    Sequence< OUString > aSeq;

    if ( Event.State >>= aSeq )
    {
        if ( aSeq.getLength() == 4 )
        {
            // Retrieve all other values from the sequence and
            // store it members!
            m_aCurLang          = aSeq[0];
            m_nScriptType       = static_cast< sal_Int16 >(aSeq[1].toInt32());
            m_aKeyboardLang     = aSeq[2];
            m_aGuessedTextLang  = aSeq[3];
        }
    }
    else if ( !Event.State.hasValue() )
	{
        m_bShowMenu = sal_False;	// no language -> no sub-menu entries -> disable menu
	}
}

// XMenuListener
void LanguageSelectionMenuController::impl_select(const Reference< XDispatch >& _xDispatch,const ::com::sun::star::util::URL& aTargetURL)
{
    Reference< XDispatch > xDispatch = _xDispatch;

	if ( aTargetURL.Complete == m_aMenuCommandURL_Font )
	{	//open format/character dialog for current selection
		xDispatch = m_xMenuDispatch_Font;
	}
	else if ( aTargetURL.Complete == m_aMenuCommandURL_Lang )
	{	//open language tab-page in tools/options dialog
		xDispatch = m_xMenuDispatch_Lang;
	}
	else if ( aTargetURL.Complete == m_aMenuCommandURL_CharDlgForParagraph )
	{	//open format/character dialog for current selection
		xDispatch = m_xMenuDispatch_CharDlgForParagraph;
	}

	if ( !xDispatch.is() )
	{
		Reference< XDispatchProvider > xDispatchProvider( m_xFrame, UNO_QUERY );
		if ( xDispatchProvider.is() )
			xDispatch = xDispatchProvider->queryDispatch( aTargetURL, OUString(), 0 );
	}

	if ( xDispatch.is() )
	{	
        Sequence<PropertyValue>	     aArgs;
        if(::comphelper::UiEventsLogger::isEnabled()) //#i88653#
            UiEventLogHelper( OUString::createFromAscii("LanguageSelectionMenuController")).log( m_xServiceManager, m_xFrame, aTargetURL, aArgs );
        xDispatch->dispatch( aTargetURL, aArgs );
	}
}

// XPopupMenuController
void LanguageSelectionMenuController::impl_setPopupMenu()
{
    Reference< XDispatchProvider > xDispatchProvider( m_xFrame, UNO_QUERY );

    com::sun::star::util::URL aTargetURL;

    // Register for language updates
    aTargetURL.Complete = m_aLangStatusCommandURL;
    m_xURLTransformer->parseStrict( aTargetURL );
    m_xLanguageDispatch = xDispatchProvider->queryDispatch( aTargetURL, OUString(), 0 );

	// Register for setting languages and opening language dialog
    aTargetURL.Complete = m_aMenuCommandURL_Lang;
    m_xURLTransformer->parseStrict( aTargetURL );
    m_xMenuDispatch_Lang = xDispatchProvider->queryDispatch( aTargetURL, OUString(), 0 );

	// Register for opening character dialog
    aTargetURL.Complete = m_aMenuCommandURL_Font;
    m_xURLTransformer->parseStrict( aTargetURL );
    m_xMenuDispatch_Font = xDispatchProvider->queryDispatch( aTargetURL, OUString(), 0 );

	// Register for opening character dialog with preselected paragraph
    aTargetURL.Complete = m_aMenuCommandURL_CharDlgForParagraph;
    m_xURLTransformer->parseStrict( aTargetURL );
    m_xMenuDispatch_CharDlgForParagraph = xDispatchProvider->queryDispatch( aTargetURL, OUString(), 0 );
}

void LanguageSelectionMenuController::fillPopupMenu( Reference< css::awt::XPopupMenu >& rPopupMenu , const Mode eMode )
{
    VCLXPopupMenu* pVCLPopupMenu = (VCLXPopupMenu *)VCLXMenu::GetImplementation( rPopupMenu );
    PopupMenu*     pPopupMenu    = 0;

    vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

    resetPopupMenu( rPopupMenu );
	if (!m_bShowMenu)
		return;

    if ( pVCLPopupMenu )
        pPopupMenu = (PopupMenu *)pVCLPopupMenu->GetMenu();

	String aCmd;
	String aCmd_Dialog;
	String aCmd_Language;
    if( eMode == MODE_SetLanguageSelectionMenu )
	{
        aCmd_Dialog.AppendAscii(".uno:FontDialog?Language:string=*");
		aCmd_Language.AppendAscii(".uno:LanguageStatus?Language:string=Current_");
	}
	else if ( eMode == MODE_SetLanguageParagraphMenu )
	{
		aCmd_Dialog.AppendAscii(".uno:FontDialogForParagraph");
		aCmd_Language.AppendAscii(".uno:LanguageStatus?Language:string=Paragraph_");
	}
    else if ( eMode == MODE_SetLanguageAllTextMenu )
    {
        aCmd_Dialog.AppendAscii(".uno:LanguageStatus?Language:string=*");
        aCmd_Language.AppendAscii(".uno:LanguageStatus?Language:string=Default_");
    }

    SvtLanguageTable    aLanguageTable;

    // get languages to be displayed in the menu
    std::set< OUString > aLangItems;
    FillLangItems( aLangItems, aLanguageTable, m_xFrame, m_aLangGuessHelper, 
            m_nScriptType, m_aCurLang, m_aKeyboardLang, m_aGuessedTextLang );

    //
    // now add menu entries
    // the different menues purpose will be handled by the different string
    // for aCmd_Dialog and aCmd_Language
    //

    sal_Int16 nItemId = 1;  // in this control the item id is not important for executing the command
    const OUString sAsterix(RTL_CONSTASCII_USTRINGPARAM("*"));  // multiple languages in current selection
    const OUString sEmpty;  // 'no language found' from language guessing
    std::map< sal_Int16, OUString > aLangMap;
    std::set< OUString >::const_iterator it;
    for (it = aLangItems.begin(); it != aLangItems.end(); ++it)
	{
        const OUString & rStr( *it );
        if (rStr != OUString( aLanguageTable.GetString( LANGUAGE_NONE ) )&&
            rStr != sAsterix &&
            rStr != sEmpty)
		{
			pPopupMenu->InsertItem( nItemId, rStr );
			aCmd = aCmd_Language;
			aCmd += String( rStr );
			pPopupMenu->SetItemCommand( nItemId, aCmd );
            if (rStr == m_aCurLang && eMode == MODE_SetLanguageSelectionMenu )
			{
				//make a sign for the current language
				pPopupMenu->CheckItem( nItemId, sal_True );
			}
            aLangMap[ nItemId ] = rStr;
            ++nItemId;
		}
	}

    // entry for LANGUAGE_NONE
	++nItemId;
	pPopupMenu->InsertItem( nItemId, String(FwlResId( STR_LANGSTATUS_NONE )) );
	aCmd=aCmd_Language;
	aCmd.AppendAscii("LANGUAGE_NONE");
	pPopupMenu->SetItemCommand( nItemId, aCmd );

    // entry for 'Reset to default language'
	++nItemId;
	pPopupMenu->InsertItem( nItemId, String(FwlResId( STR_RESET_TO_DEFAULT_LANGUAGE )) );
	aCmd=aCmd_Language;
	aCmd.AppendAscii("RESET_LANGUAGES");
	pPopupMenu->SetItemCommand( nItemId, aCmd );

    // entry for opening the Format/Character dialog
	++nItemId;
	pPopupMenu->InsertItem( nItemId, String(FwlResId( STR_LANGSTATUS_MORE )));
    pPopupMenu->SetItemCommand( nItemId, aCmd_Dialog );
}


void SAL_CALL LanguageSelectionMenuController::updatePopupMenu() throw ( ::com::sun::star::uno::RuntimeException )
{
	svt::PopupMenuControllerBase::updatePopupMenu();

    // Force status update to get information about the current languages
    osl::ClearableMutexGuard aLock( m_aMutex );
    Reference< XDispatch > xDispatch( m_xLanguageDispatch );
    com::sun::star::util::URL aTargetURL;
    aTargetURL.Complete = m_aLangStatusCommandURL;
    m_xURLTransformer->parseStrict( aTargetURL );
    aLock.clear();

    if ( xDispatch.is() )
    {
        xDispatch->addStatusListener( SAL_STATIC_CAST( XStatusListener*, this ), aTargetURL );
        xDispatch->removeStatusListener( SAL_STATIC_CAST( XStatusListener*, this ), aTargetURL );
    }

    // TODO: Fill menu with the information retrieved by the status update

    if( m_aCommandURL.equalsAscii( ".uno:SetLanguageSelectionMenu" ))
	{
        fillPopupMenu(m_xPopupMenu, MODE_SetLanguageSelectionMenu );
	}
	else if( m_aCommandURL.equalsAscii( ".uno:SetLanguageParagraphMenu" ))
	{
		fillPopupMenu(m_xPopupMenu, MODE_SetLanguageParagraphMenu );
	}
    else if( m_aCommandURL.equalsAscii( ".uno:SetLanguageAllTextMenu" ))
	{
        fillPopupMenu(m_xPopupMenu, MODE_SetLanguageAllTextMenu );
	}
}

// XInitialization
void SAL_CALL LanguageSelectionMenuController::initialize( const Sequence< Any >& aArguments ) throw ( Exception, RuntimeException )
{
    osl::MutexGuard aLock( m_aMutex );

    sal_Bool bInitialized( m_bInitialized );
    if ( !bInitialized )
    {
		svt::PopupMenuControllerBase::initialize(aArguments);

        if ( m_bInitialized )
        {
			m_aLangStatusCommandURL				  = OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:LanguageStatus" ));
			m_aMenuCommandURL_Lang				  = m_aLangStatusCommandURL;
			m_aMenuCommandURL_Font			      = OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:FontDialog" ));
			m_aMenuCommandURL_CharDlgForParagraph = OUString( RTL_CONSTASCII_USTRINGPARAM( ".uno:FontDialogForParagraph" ));
        }
    }
}

}

