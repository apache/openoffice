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

#include <uielement/recentfilesmenucontroller.hxx>
#include <threadhelp/resetableguard.hxx>
#include <classes/resource.hrc>
#include <classes/fwkresid.hxx>

#include <com/sun/star/util/XStringWidth.hpp>

#include <cppuhelper/implbase1.hxx>
#include <dispatch/uieventloghelper.hxx>
#include <osl/file.hxx>
#include <tools/urlobj.hxx>
#include <unotools/historyoptions.hxx>
#include <vcl/menu.hxx>
#include <vcl/svapp.hxx>
#include <vos/mutex.hxx>

using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::frame;
using namespace com::sun::star::beans;
using namespace com::sun::star::util;

#define MAX_STR_WIDTH   46
#define MAX_MENU_ITEMS  99

static const char SFX_REFERER_USER[] = "private:user";
static const char CMD_CLEAR_LIST[]   = ".uno:ClearRecentFileList";
static const char CMD_PREFIX[]       = "vnd.sun.star.popup:RecentFileList?entry=";
static const char MENU_SHORTCUT[]    = "~N: ";

namespace framework
{

class RecentFilesStringLength : public ::cppu::WeakImplHelper1< ::com::sun::star::util::XStringWidth >
{
	public:
		RecentFilesStringLength() {}
		virtual ~RecentFilesStringLength() {}

		// XStringWidth
		sal_Int32 SAL_CALL queryStringWidth( const ::rtl::OUString& aString )
			throw (::com::sun::star::uno::RuntimeException)
		{
			return aString.getLength();
		}
};

DEFINE_XSERVICEINFO_MULTISERVICE		(	RecentFilesMenuController					,
											OWeakObject									,
											SERVICENAME_POPUPMENUCONTROLLER				,
											IMPLEMENTATIONNAME_RECENTFILESMENUCONTROLLER
										)

DEFINE_INIT_SERVICE						(   RecentFilesMenuController, {} )

RecentFilesMenuController::RecentFilesMenuController( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager ) :
	svt::PopupMenuControllerBase( xServiceManager ),
	m_bDisabled( sal_False )
{
}

RecentFilesMenuController::~RecentFilesMenuController()
{
}

// private function
void RecentFilesMenuController::fillPopupMenu( Reference< css::awt::XPopupMenu >& rPopupMenu )
{
	VCLXPopupMenu*	pPopupMenu = (VCLXPopupMenu *)VCLXMenu::GetImplementation( rPopupMenu );
	PopupMenu*		pVCLPopupMenu = 0;

	vos::OGuard aSolarMutexGuard( Application::GetSolarMutex() );

	resetPopupMenu( rPopupMenu );
	if ( pPopupMenu )
		pVCLPopupMenu = (PopupMenu *)pPopupMenu->GetMenu();

	if ( pVCLPopupMenu )
	{
		Sequence< Sequence< PropertyValue > > aHistoryList = SvtHistoryOptions().GetList( ePICKLIST );
		Reference< XStringWidth > xStringLength( new RecentFilesStringLength );

		int nPickListMenuItems = ( aHistoryList.getLength() > MAX_MENU_ITEMS ) ? MAX_MENU_ITEMS : aHistoryList.getLength();

		m_aRecentFilesItems.clear();
		if (( nPickListMenuItems > 0 ) && !m_bDisabled )
		{
			for ( int i = 0; i < nPickListMenuItems; i++ )
			{
				Sequence< PropertyValue >& rPickListEntry = aHistoryList[i];
				RecentFile aRecentFile;

				for ( int j = 0; j < rPickListEntry.getLength(); j++ )
				{
					Any a = rPickListEntry[j].Value;

					if ( rPickListEntry[j].Name == HISTORY_PROPERTYNAME_URL )
						a >>= aRecentFile.aURL;
					else if ( rPickListEntry[j].Name == HISTORY_PROPERTYNAME_FILTER )
						a >>= aRecentFile.aFilter;
					else if ( rPickListEntry[j].Name == HISTORY_PROPERTYNAME_TITLE )
						a >>= aRecentFile.aTitle;
					else if ( rPickListEntry[j].Name == HISTORY_PROPERTYNAME_PASSWORD )
						a >>= aRecentFile.aPassword;
				}

				m_aRecentFilesItems.push_back( aRecentFile );
			}
		}

		if ( !m_aRecentFilesItems.empty() )
		{
			const sal_uInt32 nCount = m_aRecentFilesItems.size();
			for ( sal_uInt32 i = 0; i < nCount; i++ )
			{
				rtl::OUStringBuffer aMenuShortCut;
				if ( i <= 9 )
				{
					if ( i == 9 )
						aMenuShortCut.appendAscii( RTL_CONSTASCII_STRINGPARAM( "1~0: " ) );
					else
					{
						aMenuShortCut.appendAscii( RTL_CONSTASCII_STRINGPARAM( MENU_SHORTCUT ) );
						aMenuShortCut.setCharAt( 1, sal_Unicode( i + '1' ) );
					}
				}
				else
				{
					aMenuShortCut.append( sal_Int32( i + 1 ) );
					aMenuShortCut.appendAscii( RTL_CONSTASCII_STRINGPARAM( ": " ) );
				}

				rtl::OUStringBuffer aStrBuffer;
				aStrBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( CMD_PREFIX ) );
				aStrBuffer.append( sal_Int32( i ) );
				rtl::OUString  aURLString( aStrBuffer.makeStringAndClear() );

				// Abbreviate URL
				rtl::OUString	aTipHelpText;
				rtl::OUString	aMenuTitle;
				INetURLObject	aURL( m_aRecentFilesItems[i].aURL );

				if ( aURL.GetProtocol() == INET_PROT_FILE )
				{
					// Do handle file URL differently => convert it to a system
					// path and abbreviate it with a special function:
					rtl::OUString aSystemPath( aURL.getFSysPath( INetURLObject::FSYS_DETECT ) );
					aTipHelpText = aSystemPath;

					::rtl::OUString	aCompactedSystemPath;
					if ( osl_abbreviateSystemPath( aSystemPath.pData, &aCompactedSystemPath.pData, MAX_STR_WIDTH, NULL ) == osl_File_E_None )
						aMenuTitle = aCompactedSystemPath;
					else
						aMenuTitle = aSystemPath;
				}
				else
				{
					// Use INetURLObject to abbreviate all other URLs
					aMenuTitle   = aURL.getAbbreviated( xStringLength, MAX_STR_WIDTH, INetURLObject::DECODE_UNAMBIGUOUS );
					aTipHelpText = aURLString;
				}

				aMenuShortCut.append( aMenuTitle );

				pVCLPopupMenu->InsertItem( sal_uInt16( i+1 ), aMenuShortCut.makeStringAndClear() );
				pVCLPopupMenu->SetTipHelpText( sal_uInt16( i+1 ), aTipHelpText );
				pVCLPopupMenu->SetItemCommand( sal_uInt16( i+1 ), aURLString );
			}

			pVCLPopupMenu->InsertSeparator();
			// Clear List menu entry
			pVCLPopupMenu->InsertItem( sal_uInt16( nCount + 1 ),
									   String( FwkResId( STR_CLEAR_RECENT_FILES ) ) );
			pVCLPopupMenu->SetItemCommand( sal_uInt16( nCount + 1 ),
										   rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( CMD_CLEAR_LIST ) ) );
			pVCLPopupMenu->SetHelpText( sal_uInt16( nCount + 1 ),
										String( FwkResId( STR_CLEAR_RECENT_FILES_HELP ) ) );
		}
		else
		{
			// No recent documents => insert "no document" string
			pVCLPopupMenu->InsertItem( 1, String( FwkResId( STR_NODOCUMENT ) ) );
			// Do not disable it, otherwise the Toolbar controller and MenuButton
			// will display SV_RESID_STRING_NOSELECTIONPOSSIBLE instead of STR_NODOCUMENT
			pVCLPopupMenu->SetItemBits( 1, pVCLPopupMenu->GetItemBits( 1 ) | MIB_NOSELECT );
		}
	}
}

void RecentFilesMenuController::executeEntry( sal_Int32 nIndex )
{
	static int NUM_OF_PICKLIST_ARGS = 3;

	Reference< XDispatch >			xDispatch;
	Reference< XDispatchProvider >	xDispatchProvider;
	css::util::URL					aTargetURL;
	Sequence< PropertyValue >		aArgsList;

	osl::ClearableMutexGuard aLock( m_aMutex );
	xDispatchProvider = Reference< XDispatchProvider >( m_xFrame, UNO_QUERY );
	aLock.clear();

	if (( nIndex >= 0 ) &&
		( nIndex < sal::static_int_cast<sal_Int32>( m_aRecentFilesItems.size() )))
	{
		const RecentFile& rRecentFile = m_aRecentFilesItems[ nIndex ];

		aTargetURL.Complete = rRecentFile.aURL;
		m_xURLTransformer->parseStrict( aTargetURL );

		aArgsList.realloc( NUM_OF_PICKLIST_ARGS );
		aArgsList[0].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Referer" ));
		aArgsList[0].Value = makeAny( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( SFX_REFERER_USER )));

		// documents in the picklist will never be opened as templates
		aArgsList[1].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "AsTemplate" ));
		aArgsList[1].Value = makeAny( (sal_Bool) sal_False );

		::rtl::OUString  aFilter( rRecentFile.aFilter );
		sal_Int32 nPos = aFilter.indexOf( '|' );
		if ( nPos >= 0 )
		{
			::rtl::OUString aFilterOptions;

			if ( nPos < ( aFilter.getLength() - 1 ) )
				aFilterOptions = aFilter.copy( nPos+1 );

			aArgsList[2].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "FilterOptions" ));
			aArgsList[2].Value <<= aFilterOptions;

			aFilter = aFilter.copy( 0, nPos-1 );
			aArgsList.realloc( ++NUM_OF_PICKLIST_ARGS );
		}

		aArgsList[NUM_OF_PICKLIST_ARGS-1].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "FilterName" ));
		aArgsList[NUM_OF_PICKLIST_ARGS-1].Value <<= aFilter;

		xDispatch = xDispatchProvider->queryDispatch( aTargetURL, ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "_default" ) ), 0 );
	}

	if ( xDispatch.is() )
	{
		// Call dispatch asynchronously as we can be destroyed while dispatch is
		// executed. VCL is not able to survive this as it wants to call listeners
		// after select!!!
		LoadRecentFile* pLoadRecentFile = new LoadRecentFile;
		pLoadRecentFile->xDispatch  = xDispatch;
		pLoadRecentFile->aTargetURL = aTargetURL;
		pLoadRecentFile->aArgSeq    = aArgsList;

		if(::comphelper::UiEventsLogger::isEnabled()) //#i88653#
			UiEventLogHelper(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("RecentFilesMenuController"))).log(m_xServiceManager, m_xFrame, aTargetURL, aArgsList);

		Application::PostUserEvent( STATIC_LINK(0, RecentFilesMenuController, ExecuteHdl_Impl), pLoadRecentFile );
	}
}

// XEventListener
void SAL_CALL RecentFilesMenuController::disposing( const EventObject& ) throw ( RuntimeException )
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
void SAL_CALL RecentFilesMenuController::statusChanged( const FeatureStateEvent& Event ) throw ( RuntimeException )
{
	osl::MutexGuard aLock( m_aMutex );
	m_bDisabled = !Event.IsEnabled;
}

void SAL_CALL RecentFilesMenuController::itemSelected( const css::awt::MenuEvent& rEvent ) throw (RuntimeException)
{
	Reference< css::awt::XPopupMenu > xPopupMenu;

	osl::ClearableMutexGuard aLock( m_aMutex );
	xPopupMenu = m_xPopupMenu;
	aLock.clear();

	if ( xPopupMenu.is() )
	{
		const rtl::OUString aCommand( xPopupMenu->getCommand( rEvent.MenuId ) );
		OSL_TRACE( "RecentFilesMenuController::itemSelected() - Command : %s",
					rtl::OUStringToOString( aCommand, RTL_TEXTENCODING_UTF8 ).getStr() );

		if ( aCommand.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( CMD_CLEAR_LIST ) ) )
			SvtHistoryOptions().Clear( ePICKLIST );
		else
			executeEntry( rEvent.MenuId-1 );
	}
}

void SAL_CALL RecentFilesMenuController::itemActivated( const css::awt::MenuEvent& ) throw (RuntimeException)
{
	osl::MutexGuard aLock( m_aMutex );
	impl_setPopupMenu();
}

// XPopupMenuController
void RecentFilesMenuController::impl_setPopupMenu()
{
	if ( m_xPopupMenu.is() )
		fillPopupMenu( m_xPopupMenu );
}

// XDispatchProvider
Reference< XDispatch > SAL_CALL RecentFilesMenuController::queryDispatch(
	const URL& aURL,
	const ::rtl::OUString& /*sTarget*/,
	sal_Int32 /*nFlags*/ )
throw( RuntimeException )
{
	osl::MutexGuard aLock( m_aMutex );

	throwIfDisposed();

	if ( aURL.Complete.indexOf( m_aBaseURL ) == 0 )
		return Reference< XDispatch >( static_cast< OWeakObject* >( this ), UNO_QUERY );
	else
		return Reference< XDispatch >();
}

// XDispatch
void SAL_CALL RecentFilesMenuController::dispatch(
	const URL& aURL,
	const Sequence< PropertyValue >& /*seqProperties*/ )
throw( RuntimeException )
{
	osl::MutexGuard aLock( m_aMutex );

	throwIfDisposed();

	if ( aURL.Complete.indexOf( m_aBaseURL ) == 0 )
	{
		// Parse URL to retrieve entry argument and its value
		sal_Int32 nQueryPart = aURL.Complete.indexOf( '?', m_aBaseURL.getLength() );
		if ( nQueryPart > 0 )
		{
			const rtl::OUString aEntryArgStr( RTL_CONSTASCII_USTRINGPARAM( "entry=" ));
			sal_Int32 nEntryArg = aURL.Complete.indexOf( aEntryArgStr, nQueryPart );
			sal_Int32 nEntryPos = nEntryArg + aEntryArgStr.getLength();
			if (( nEntryArg > 0 ) && ( nEntryPos < aURL.Complete.getLength() ))
			{
				sal_Int32 nAddArgs = aURL.Complete.indexOf( '&', nEntryPos );
				rtl::OUString aEntryArg;

				if ( nAddArgs < 0 )
					aEntryArg = aURL.Complete.copy( nEntryPos );
				else
					aEntryArg = aURL.Complete.copy( nEntryPos, nAddArgs-nEntryPos );

				sal_Int32 nEntry = aEntryArg.toInt32();
				executeEntry( nEntry );
			}
		}
	}
}

IMPL_STATIC_LINK_NOINSTANCE( RecentFilesMenuController, ExecuteHdl_Impl, LoadRecentFile*, pLoadRecentFile )
{
	try
	{
		// Asynchronous execution as this can lead to our own destruction!
		// Framework can recycle our current frame and the layout manager disposes all user interface
		// elements if a component gets detached from its frame!
		pLoadRecentFile->xDispatch->dispatch( pLoadRecentFile->aTargetURL, pLoadRecentFile->aArgSeq );
	}
	catch ( Exception& )
	{
	}

	delete pLoadRecentFile;
	return 0;
}

}

/* vim: set noet sw=4 ts=4: */
