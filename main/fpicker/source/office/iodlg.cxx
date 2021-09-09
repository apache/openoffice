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
#include "precompiled_fpicker.hxx"

// includes --------------------------------------------------------------

#include "iodlg.hxx"
#include <tools/stream.hxx>
#include <tools/urlobj.hxx>
#include <vcl/fixed.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/svapp.hxx>
#include <vcl/timer.hxx>
#include <unotools/ucbhelper.hxx>
#include <ucbhelper/contentbroker.hxx>
#include "svtools/ehdl.hxx"
#include "svl/urihelper.hxx"
#include "unotools/pathoptions.hxx"
#include "unotools/viewoptions.hxx"
#include "svtools/fileview.hxx"
#include "unotools/inetoptions.hxx"
#include "svtools/sfxecode.hxx"
#include "svl/svarray.hxx"
#include "svtools/svtabbx.hxx"

#define _SVSTDARR_USHORTS
#define _SVSTDARR_STRINGSDTOR
#include "svl/svstdarr.hxx"
#include <toolkit/helper/vclunohelper.hxx>
#include <unotools/localfilehelper.hxx>

#ifndef _SVTOOLS_HRC
#include "svtools/svtools.hrc"
#endif
#ifndef _SVT_HELPID_HRC
#include "svtools/helpid.hrc"
#endif
#ifndef _SVTOOLS_IODLGIMPL_HRC
#include "iodlg.hrc"
#endif
#include "rtl/instance.hxx"
#include "asyncfilepicker.hxx"
#include "iodlgimp.hxx"
#include "svtools/inettbc.hxx"
#include "unotools/syslocale.hxx"
#include "svtools/QueryFolderName.hxx"
#ifndef  _RTL_USTRING_HXX
#include <rtl/ustring.hxx>
#endif
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/ucb/XContentProviderManager.hpp>
#include <com/sun/star/ui/dialogs/CommonFilePickerElementIds.hpp>
#include <com/sun/star/ui/dialogs/ExtendedFilePickerElementIds.hpp>
#include <com/sun/star/ui/dialogs/ControlActions.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/sdbc/XResultSet.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/util/URL.hpp>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#ifndef _UNOTOOLS_PROCESSFACTORY_HXX
#include <comphelper/processfactory.hxx>
#endif
#include <osl/file.h>
#include <vcl/waitobj.hxx>

// #97148# ------------------------------------
#include <com/sun/star/task/XInteractionHandler.hpp>
#include "com/sun/star/ucb/InteractiveAugmentedIOException.hpp"
#include "fpinteraction.hxx"
#include <osl/process.h>
#include <comphelper/interaction.hxx>
#include <vcl/dibtools.hxx>

#include <algorithm>
#include <functional>

//#define AUTOSELECT_USERFILTER
	// define this for the experimental feature of user-filter auto selection
	// means if the user enters e.g. *.doc<enter>, and there is a filter which is responsible for *.doc files (only),
	// then this filter is selected automatically

using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::ui::dialogs;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::ucb;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::sdbc;
using namespace ::utl;
using namespace ::svt;

using namespace ExtendedFilePickerElementIds;
using namespace CommonFilePickerElementIds;
using namespace InternalFilePickerElementIds;

#define IODLG_CONFIGNAME		String(RTL_CONSTASCII_USTRINGPARAM("FileDialog"))
#define IMPGRF_CONFIGNAME		String(RTL_CONSTASCII_USTRINGPARAM("ImportGraphicDialog"))

#define GET_DECODED_NAME(aObj) \
	aObj.getName( INetURLObject::LAST_SEGMENT, true, INetURLObject::DECODE_WITH_CHARSET )

// Zeit die beim Traveln in der Filterbox gewartet wird,
// bis in der Browsebox gefiltert wird ( in ms ).
#define TRAVELFILTER_TIMEOUT	750

#define WIDTH_ADDITION	15

// functions -------------------------------------------------------------

namespace
{

	//-----------------------------------------------------------------------------
	String getMostCurrentFilter( SvtExpFileDlg_Impl* pImpl )
	{
		DBG_ASSERT( pImpl, "invalid impl pointer" );
		const SvtFileDialogFilter_Impl* pFilter = pImpl->_pUserFilter;

		if ( !pFilter )
			pFilter = pImpl->GetCurFilter();

		// Filtern.
		if ( !pFilter )
			return String();

		return pFilter->GetType();
	}

	//-----------------------------------------------------------------------------
	sal_Bool restoreCurrentFilter( SvtExpFileDlg_Impl* _pImpl )
	{
		DBG_ASSERT( _pImpl->GetCurFilter(), "restoreCurrentFilter: no current filter!" );
		DBG_ASSERT( _pImpl->GetCurFilterDisplayName().Len(), "restoreCurrentFilter: no current filter (no display name)!" );

		_pImpl->SelectFilterListEntry( _pImpl->GetCurFilterDisplayName() );

#ifdef DBG_UTIL
		String sSelectedDisplayName;
		DBG_ASSERT(	( _pImpl->GetSelectedFilterEntry( sSelectedDisplayName ) == _pImpl->GetCurFilter() )
				&&	( sSelectedDisplayName == _pImpl->GetCurFilterDisplayName() ),
			"restoreCurrentFilter: inconsistence!" );
#endif
		return _pImpl->m_bNeedDelayedFilterExecute;
	}

	//-----------------------------------------------------------------------------
	String GetFsysExtension_Impl( const String& rFile, const String& rLastFilterExt )
	{
		xub_StrLen nDotPos = rFile.SearchBackward( '.' );
		if ( nDotPos != STRING_NOTFOUND )
		{
			if ( rLastFilterExt.Len() )
			{
				if ( rFile.Copy( nDotPos + 1 ).EqualsIgnoreCaseAscii( rLastFilterExt ) )
					return String( rLastFilterExt );
			}
			else
				return String( rFile.Copy( nDotPos ) );
		}
		return String();
	}

	//-----------------------------------------------------------------------------
	void SetFsysExtension_Impl( String& rFile, const String& rExtension )
	{
		const sal_Unicode* p0 = rFile.GetBuffer();
		const sal_Unicode* p1 = p0 + rFile.Len() - 1;
		while ( p1 >= p0 && *p1 != sal_Unicode( '.' ) )
			p1--;
		if ( p1 >= p0 )
			// remove old extension
			rFile.Erase(
				sal::static_int_cast< xub_StrLen >(
					p1 - p0 + 1 - ( rExtension.Len() > 0 ? 0 : 1 ) ) );
		else if ( rExtension.Len() )
			// no old extension
			rFile += sal_Unicode( '.' );
		rFile += rExtension;
	}

	//-----------------------------------------------------------------------------
	// move the control with the given offset
	void lcl_MoveControl( Control* _pControl, sal_Int32 _nDeltaX, sal_Int32 _nDeltaY, sal_Int32* _pMaxY = NULL )
	{
		if ( _pControl )
		{
			Point aNewPos = _pControl->GetPosPixel();

			// adjust the vertical position
			aNewPos.Y() += _nDeltaY;
			if ( _pMaxY && ( aNewPos.Y() > *_pMaxY ) )
				*_pMaxY = aNewPos.Y();

			// adjust the horizontal position
			aNewPos.X() += _nDeltaX;

			_pControl->SetPosPixel( aNewPos );
		}
	}

	//-------------------------------------------------------------------------
	void lcl_autoUpdateFileExtension( SvtFileDialog* _pDialog, const String& _rLastFilterExt )
	{
		// if auto extension is enabled ....
		if ( _pDialog->isAutoExtensionEnabled() )
		{
			// automatically switch to the extension of the (maybe just newly selected) extension
			String aNewFile = _pDialog->getCurrentFileText( );
			String aExt = GetFsysExtension_Impl( aNewFile, _rLastFilterExt );

			// but only if there already is an extension
			if ( aExt.Len() )
			{
				// check if it is a real file extension, and not only the "post-dot" part in
				// a directory name
				// 28.03.2002 - 98337 - fs@openoffice.org
				sal_Bool bRealExtensions = sal_True;
				if ( STRING_NOTFOUND != aExt.Search( '/' ) )
					bRealExtensions = sal_False;
				else if ( STRING_NOTFOUND != aExt.Search( '\\' ) )
					bRealExtensions = sal_False;
				else
				{
					// no easy way to tell, because the part containing the dot already is the last
					// segment of the complete file name
					// So we have to check if the file name denotes a folder or a file.
					// For performance reasons, we do this for file urls only
					INetURLObject aURL( aNewFile );
					if ( INET_PROT_NOT_VALID == aURL.GetProtocol() )
					{
						String sURL;
						if ( ::utl::LocalFileHelper::ConvertPhysicalNameToURL( aNewFile, sURL ) )
							aURL = INetURLObject( sURL );
					}
					if ( INET_PROT_FILE == aURL.GetProtocol() )
					{
						// #97148# & #102204# -----
						try
						{
							bRealExtensions = !_pDialog->ContentIsFolder( aURL.GetMainURL( INetURLObject::NO_DECODE ) );
						}
						catch( ::com::sun::star::uno::Exception& )
						{
							DBG_WARNING( "Exception in lcl_autoUpdateFileExtension" );
						}
					}
				}

				if ( bRealExtensions )
				{
					SetFsysExtension_Impl( aNewFile, _pDialog->GetDefaultExt() );
					_pDialog->setCurrentFileText( aNewFile );
				}
			}
		}
	}

	//-------------------------------------------------------------------------
	sal_Bool lcl_getHomeDirectory( const String& _rForURL, String& /* [out] */ _rHomeDir )
	{
		_rHomeDir.Erase();

		// now ask the content broker for a provider for this scheme
		//=================================================================
		try
		{
			// get the content provider manager
			::ucbhelper::ContentBroker* pBroker = ::ucbhelper::ContentBroker::get();
			Reference< XContentProviderManager > xProviderManager;
			if ( pBroker )
				xProviderManager = pBroker->getContentProviderManagerInterface();

			//=================================================================
			// get the provider for the current scheme
			Reference< XContentProvider > xProvider;
			if ( xProviderManager.is() )
				xProvider = xProviderManager->queryContentProvider( _rForURL );

			DBG_ASSERT( xProvider.is(), "lcl_getHomeDirectory: could not find a (valid) content provider for the current URL!" );
			Reference< XPropertySet > xProviderProps( xProvider, UNO_QUERY );
			if ( xProviderProps.is() )
			{
				Reference< XPropertySetInfo > xPropInfo = xProviderProps->getPropertySetInfo();
				const ::rtl::OUString sHomeDirPropertyName( RTL_CONSTASCII_USTRINGPARAM( "HomeDirectory" ) );
				if ( !xPropInfo.is() || xPropInfo->hasPropertyByName( sHomeDirPropertyName ) )
				{
					::rtl::OUString sHomeDirectory;
					xProviderProps->getPropertyValue( sHomeDirPropertyName ) >>= sHomeDirectory;
					_rHomeDir = sHomeDirectory;
				}
			}
		}
		catch( const Exception& )
		{
			DBG_ERROR( "lcl_getHomeDirectory: caught an exception!" );
		}
		return 0 < _rHomeDir.Len();
	}

	//---------------------------------------------------------------------
	static String lcl_ensureFinalSlash( const String& _rDir )
	{
		INetURLObject aWorkPathObj( _rDir, INET_PROT_FILE );
		aWorkPathObj.setFinalSlash();
		return aWorkPathObj.GetMainURL( INetURLObject::NO_DECODE );
	}

	//---------------------------------------------------------------------
	void convertStringListToUrls( const String& _rColonSeparatedList, ::std::vector< String >& _rTokens, bool _bFinalSlash )
	{
		const sal_Unicode s_cSeparator =
#if defined(WNT) || defined(OS2)
			';'
#else
			':'
#endif
			;
		xub_StrLen nTokens = _rColonSeparatedList.GetTokenCount( s_cSeparator );
		_rTokens.resize( 0 ); _rTokens.reserve( nTokens );
		for ( xub_StrLen i=0; i<nTokens; ++i )
		{
			// the current token in the list
			String sCurrentToken = _rColonSeparatedList.GetToken( i, s_cSeparator );
			if ( !sCurrentToken.Len() )
				continue;

			INetURLObject aCurrentURL;

			String sURL;
			if ( ::utl::LocalFileHelper::ConvertPhysicalNameToURL( sCurrentToken, sURL ) )
				aCurrentURL = INetURLObject( sURL );
			else
			{
				// smart URL parsing, assuming FILE protocol
				aCurrentURL = INetURLObject( sCurrentToken, INET_PROT_FILE );
			}

			if ( _bFinalSlash )
				aCurrentURL.setFinalSlash( );
			else
				aCurrentURL.removeFinalSlash( );
			_rTokens.push_back( aCurrentURL.GetMainURL( INetURLObject::NO_DECODE ) );
		}
	}

	//---------------------------------------------------------------------
	struct RemoveFinalSlash : public ::std::unary_function< String, void >
	{
		void operator()( String& _rURL )
		{
			INetURLObject aURL( _rURL );
#if defined(WNT) || defined(OS2)
			if ( aURL.getSegmentCount() > 1 )
#endif
				aURL.removeFinalSlash( );
			_rURL = aURL.GetMainURL( INetURLObject::NO_DECODE );
		}
	};

	// -----------------------------------------------------------------------
	/** retrieves the value of an environment variable
		@return <TRUE/> if and only if the retrieved string value is not empty
	*/
	bool getEnvironmentValue( const sal_Char* _pAsciiEnvName, ::rtl::OUString& _rValue )
	{
		_rValue = ::rtl::OUString();
		::rtl::OUString sEnvName = ::rtl::OUString::createFromAscii( _pAsciiEnvName );
		osl_getEnvironment( sEnvName.pData, &_rValue.pData );
		return _rValue.getLength() != 0;
	}
}

//***************************************************************************
// ControlChain_Impl
//***************************************************************************

struct ControlChain_Impl
{
	Window* 		   _pControl;
	ControlChain_Impl* _pNext;
	sal_Bool		   _bHasOwnerShip;

	ControlChain_Impl( Window* pControl, ControlChain_Impl* pNext );
	~ControlChain_Impl();
};

//***************************************************************************

ControlChain_Impl::ControlChain_Impl
(
	Window* pControl,
	ControlChain_Impl* pNext
)
	: _pControl( pControl ),
	  _pNext( pNext ),
	  _bHasOwnerShip( sal_True )
{
}

//***************************************************************************

ControlChain_Impl::~ControlChain_Impl()
{
	if ( _bHasOwnerShip )
	{
		delete _pControl;
	}
	delete _pNext;
}

//*****************************************************************************
// ResMgrHolder
//*****************************************************************************
namespace
{
	struct ResMgrHolder
	{
		ResMgr * operator ()()
		{
			return ResMgr::CreateResMgr (CREATEVERSIONRESMGR_NAME(fps_office));
		}

		static ResMgr * getOrCreate()
		{
			return rtl_Instance<
				ResMgr, ResMgrHolder,
				osl::MutexGuard, osl::GetGlobalMutex >::create (
					ResMgrHolder(), osl::GetGlobalMutex());
		}
	};

	struct SvtResId : public ResId
	{
		SvtResId (sal_uInt16 nId) : ResId (nId, *ResMgrHolder::getOrCreate()) {}
	};
}

//*****************************************************************************
// SvtFileDialog
//*****************************************************************************
SvtFileDialog::SvtFileDialog
(
	Window* _pParent,
	WinBits nBits,
	WinBits nExtraBits
) :
	ModalDialog( _pParent, SvtResId( DLG_SVT_EXPLORERFILE ) )

	,_pUserControls( NULL )
	,_pCbReadOnly( NULL )
	,_pCbLinkBox( NULL)
	,_pCbPreviewBox( NULL )
	,_pCbSelection( NULL )
	,_pPbPlay( NULL )
	,_pPrevWin( NULL )
	,_pPrevBmp( NULL )
	,_pFileView( NULL )
	,_pFileNotifier( NULL )
	,_pImp( new SvtExpFileDlg_Impl( nBits ) )
	,_nExtraBits( nExtraBits )
	,_bIsInExecute( sal_False )
	,m_bInExecuteAsync( false )
	,m_bHasFilename( false )
{
	Init_Impl( nBits );
}

//*****************************************************************************

SvtFileDialog::SvtFileDialog ( Window* _pParent, WinBits nBits )
	:ModalDialog( _pParent, SvtResId( DLG_SVT_EXPLORERFILE ) )
	,_pUserControls( NULL )
	,_pCbReadOnly( NULL )
	,_pCbLinkBox( NULL)
	,_pCbPreviewBox( NULL )
	,_pCbSelection( NULL )
	,_pPbPlay( NULL )
	,_pPrevWin( NULL )
	,_pPrevBmp( NULL )
	,_pFileView( NULL )
	,_pFileNotifier( NULL )
	,_pImp( new SvtExpFileDlg_Impl( nBits ) )
	,_nExtraBits( 0L )
	,_bIsInExecute( sal_False )
	,m_bHasFilename( false )
{
	Init_Impl( nBits );
}

//*****************************************************************************

SvtFileDialog::~SvtFileDialog()
{
	if ( _pImp->_aIniKey.Len() )
	{
		// save window state
		SvtViewOptions aDlgOpt( E_DIALOG, _pImp->_aIniKey );
		aDlgOpt.SetWindowState( String( GetWindowState(), osl_getThreadTextEncoding() ) );
		String sUserData = _pFileView->GetConfigString();
		aDlgOpt.SetUserItem( ::rtl::OUString::createFromAscii( "UserData" ),
							 makeAny( ::rtl::OUString( sUserData ) ) );
	}

	_pFileView->SetSelectHdl( Link() );

	delete _pImp;
	delete _pFileView;

	delete _pCbReadOnly;
	delete _pCbLinkBox;
	delete _pCbPreviewBox;
	delete _pCbSelection;
	delete _pPbPlay;
	delete _pPrevWin;
	delete _pPrevBmp;

	delete _pUserControls;
}

//*****************************************************************************

void SvtFileDialog::Init_Impl
(
	WinBits nStyle
)
{
	sal_Bool bIsHighContrast = GetSettings().GetStyleSettings().GetHighContrastMode();
	m_aImages = ImageList( SvtResId( bIsHighContrast ? RID_FILEPICKER_IMAGES_HC : RID_FILEPICKER_IMAGES ) );

	_pImp->_nStyle = nStyle;
	_pImp->_a6Size = LogicToPixel( Size( 6, 6 ), MAP_APPFONT );
	_pImp->_eMode = ( nStyle & WB_SAVEAS ) ? FILEDLG_MODE_SAVE : FILEDLG_MODE_OPEN;
	_pImp->_eDlgType = FILEDLG_TYPE_FILEDLG;

	if ( ( nStyle & SFXWB_PATHDIALOG ) == SFXWB_PATHDIALOG )
		_pImp->_eDlgType = FILEDLG_TYPE_PATHDLG;

	// Set the directory for the "back to the default dir" button
	INetURLObject aStdDirObj( SvtPathOptions().GetWorkPath() );
	SetStandardDir( aStdDirObj.GetMainURL( INetURLObject::NO_DECODE ) );

	// Reichweite bestimmen.
	if ( !( nStyle & SFXWB_NOREMOTE ) )
	{
		_pImp->_nState |= FILEDLG_STATE_REMOTE;
	}

	// Kontrollelement erzeugen, wobei die Reihenfolge die Tab-Steuerung
	// bestimmt.
	_pImp->_pFtFileName = new FixedText( this, SvtResId( FT_EXPLORERFILE_FILENAME ) );

	SvtURLBox* pURLBox = new SvtURLBox( this );
	pURLBox->SetUrlFilter( &m_aURLFilter );
	_pImp->_pEdFileName = pURLBox;

	Edit aDummy( this, SvtResId( ED_EXPLORERFILE_FILENAME ) );
	_pImp->_pEdFileName->SetPosSizePixel( aDummy.GetPosPixel(), aDummy.GetSizePixel() );
	_pImp->_pEdFileName->Show();
	pURLBox->SetSelectHdl( LINK( this, SvtFileDialog, EntrySelectHdl_Impl ) );
	pURLBox->SetOpenHdl( STATIC_LINK( this, SvtFileDialog, OpenHdl_Impl ) );

	// in folder picker mode, only auto-complete directories (no files)
	bool bIsFolderPicker = ( _pImp->_eDlgType == FILEDLG_TYPE_PATHDLG );
	pURLBox->SetOnlyDirectories( bIsFolderPicker );

	// in save mode, don't use the autocompletion as selection in the edit part
	bool bSaveMode = ( FILEDLG_MODE_SAVE == _pImp->_eMode );
	pURLBox->SetNoURLSelection( bSaveMode );

	_pImp->_pEdFileName->SetHelpId( HID_FILEDLG_AUTOCOMPLETEBOX );

	_pImp->_pFtFileType = new FixedText( this, SvtResId( FT_EXPLORERFILE_FILETYPE ) );
	_pImp->CreateFilterListControl( this, SvtResId( LB_EXPLORERFILE_FILETYPE ) );

	// move the filter listbox to the space occupied by the version listbox
	// if that box isn't needed
	if ( !( _nExtraBits & SFX_EXTRA_SHOWVERSIONS ) &&
		 !( _nExtraBits & SFX_EXTRA_TEMPLATES ) &&
		 !( _nExtraBits & SFX_EXTRA_IMAGE_TEMPLATE ) )
	{
		{
			FixedText aSharedListBoxLabel( this, SvtResId( FT_EXPLORERFILE_SHARED_LISTBOX ) );
			_pImp->_pFtFileType->SetPosPixel( aSharedListBoxLabel.GetPosPixel() );
		}

		{
			ListBox aSharedListBox( this, SvtResId( LB_EXPLORERFILE_SHARED_LISTBOX ) );
			_pImp->GetFilterListControl()->SetPosPixel( aSharedListBox.GetPosPixel() );
		}
	}

	_pImp->_pFtCurrentPath = new FixedText( this, SvtResId( FT_EXPLORERFILE_CURRENTPATH ) );
	WinBits nTmpStyle = _pImp->_pFtCurrentPath->GetStyle();
	nTmpStyle |= WB_PATHELLIPSIS;
	_pImp->_pFtCurrentPath->SetStyle( nTmpStyle );

	_pImp->_pBtnFileOpen = new PushButton( this, SvtResId( BTN_EXPLORERFILE_OPEN ) );
	_pImp->_pBtnCancel = new CancelButton( this, SvtResId( BTN_EXPLORERFILE_CANCEL ) );
	_pImp->_pBtnHelp = new HelpButton( this, SvtResId( BTN_EXPLORERFILE_HELP ) );

	_pImp->_pBtnUp = new SvtUpButton_Impl( this, SvtResId( BTN_EXPLORERFILE_UP ) );
	_pImp->_pBtnNewFolder = new ImageButton( this, SvtResId( BTN_EXPLORERFILE_NEWFOLDER ) );
	_pImp->_pBtnNewFolder->SetStyle( _pImp->_pBtnNewFolder->GetStyle() | WB_NOPOINTERFOCUS );
	_pImp->_pBtnStandard = new SvtTravelButton_Impl( this, SvtResId( BTN_EXPLORERFILE_STANDARD ) );

	_pImp->_pBtnUp->SetAccessibleName( _pImp->_pBtnUp->GetQuickHelpText() );
	_pImp->_pBtnNewFolder->SetAccessibleName( _pImp->_pBtnNewFolder->GetQuickHelpText() );
	_pImp->_pBtnStandard->SetAccessibleName( _pImp->_pBtnStandard->GetQuickHelpText() );

	if ( ( nStyle & SFXWB_MULTISELECTION ) == SFXWB_MULTISELECTION )
		_pImp->_bMultiSelection = sal_True;

	_pFileView = new SvtFileView( this, SvtResId( CTL_EXPLORERFILE_FILELIST ),
									   FILEDLG_TYPE_PATHDLG == _pImp->_eDlgType,
									   _pImp->_bMultiSelection );
	_pFileView->SetUrlFilter( &m_aURLFilter );
	_pFileView->EnableAutoResize();

	_pFileView->SetHelpId( HID_FILEDLG_STANDARD );
	_pFileView->SetStyle( _pFileView->GetStyle() | WB_TABSTOP );

	// Positionen und Groessen der Knoepfe bestimmen.
	Image aNewFolderImg( GetButtonImage( IMG_FILEDLG_CREATEFOLDER ) );
	_pImp->_pBtnNewFolder->SetModeImage( aNewFolderImg );

	Size aSize( aNewFolderImg.GetSizePixel() );
	aSize.Width() += FILEDIALOG_DEF_IMAGEBORDER;
	aSize.Height() += FILEDIALOG_DEF_IMAGEBORDER;
	_pImp->_pBtnNewFolder->SetSizePixel( aSize );
	_pImp->_pBtnUp->SetSizePixel( aSize );
	_pImp->_pBtnStandard->SetSizePixel( aSize );

	Size aDlgSize = GetOutputSizePixel();
	long n6AppFontInPixel =
			LogicToPixel( Size( 6, 0 ), MAP_APPFONT ).Width();
	long n3AppFontInPixel =
			LogicToPixel( Size( 3, 0 ), MAP_APPFONT ).Width();

	// calculate the length of all buttons
	const sal_uInt16 nBtnCount = 3; // "previous level", "new folder" and "standard dir"
	long nDelta = n6AppFontInPixel; // right border
	nDelta += ( nBtnCount * aSize.Width() ); // button count * button width
	nDelta += ( n3AppFontInPixel + n3AppFontInPixel / 2 ); // spacing 1*big 1*small

	Point aPos(
		aDlgSize.Width() - nDelta,
		_pImp->_pBtnUp->GetPosPixel().Y()
	);
	Size aCurPathSize(
		aPos.X() - n6AppFontInPixel,
		_pImp->_pFtCurrentPath->GetOutputSizePixel().Height()
	);
	_pImp->_pFtCurrentPath->SetOutputSizePixel( aCurPathSize );
	_pImp->_pBtnUp->SetPosPixel( aPos );
	aPos.X() += aSize.Width();
	aPos.X() += n3AppFontInPixel;
	_pImp->_pBtnNewFolder->SetPosPixel( aPos );
	aPos.X() += aSize.Width();
	aPos.X() += n3AppFontInPixel / 2;
	_pImp->_pBtnStandard->SetPosPixel( aPos );
	nDelta = aSize.Height();
	nDelta -= aCurPathSize.Height();
	nDelta /= 2;
	Point aCurPathPos = _pImp->_pFtCurrentPath->GetPosPixel();
	aCurPathPos.Y() += nDelta;
	_pImp->_pFtCurrentPath->SetPosPixel( aCurPathPos );

	if ( nStyle & SFXWB_READONLY )
	{
		_pCbReadOnly = new CheckBox( this, SvtResId( CB_EXPLORERFILE_READONLY ) );
		_pCbReadOnly->SetHelpId( HID_FILEOPEN_READONLY );
		_pCbReadOnly->SetText( SvtResId( STR_SVT_FILEPICKER_READONLY ) );
		AddControl( _pCbReadOnly );
		ReleaseOwnerShip( _pCbReadOnly );
		_pCbReadOnly->SetClickHdl( LINK( this, SvtFileDialog, ClickHdl_Impl ) );
	}

	if ( nStyle & SFXWB_PASSWORD )
	{
		_pImp->_pCbPassword = new CheckBox( this, SvtResId( CB_EXPLORERFILE_PASSWORD ) );
		_pImp->_pCbPassword->SetText( SvtResId( STR_SVT_FILEPICKER_PASSWORD ) );
		AddControl( _pImp->_pCbPassword );
		ReleaseOwnerShip( _pImp->_pCbPassword );
		_pImp->_pCbPassword->SetClickHdl( LINK( this, SvtFileDialog, ClickHdl_Impl ) );
	}

	// set the ini file for extracting the size
	_pImp->_aIniKey = IODLG_CONFIGNAME;

	AddControls_Impl( );

	// Zahl der Pixel bestimmen, um die die anderen Elemente in der Position
	// angepasst werden muessen.
	aPos.Y() += aSize.Height();
	aPos.Y() += LogicToPixel( Size( 0, 6 ), MAP_APPFONT ).Height();
	long nYOffset = aPos.Y();
	aPos = _pFileView->GetPosPixel();
	nYOffset -= aPos.Y();

	// Positionen der uebrigen Elemente anpassen.
	aPos.Y() += nYOffset;
	_pFileView->SetPosPixel( aPos );

	lcl_MoveControl( _pImp->_pFtFileName, 0, nYOffset );
	lcl_MoveControl( _pImp->_pEdFileName, 0, nYOffset );

	lcl_MoveControl( _pImp->_pFtFileVersion, 0, nYOffset );
	lcl_MoveControl( _pImp->_pLbFileVersion, 0, nYOffset );

	lcl_MoveControl( _pImp->_pFtTemplates, 0, nYOffset );
	lcl_MoveControl( _pImp->_pLbTemplates, 0, nYOffset );

	lcl_MoveControl( _pImp->_pFtImageTemplates, 0, nYOffset );
	lcl_MoveControl( _pImp->_pLbImageTemplates, 0, nYOffset );

	lcl_MoveControl( _pImp->_pFtFileType, 0, nYOffset );
	lcl_MoveControl( _pImp->GetFilterListControl(), 0, nYOffset );

	lcl_MoveControl( _pImp->_pBtnFileOpen, 0, nYOffset );
	lcl_MoveControl( _pImp->_pBtnCancel, 0, nYOffset );

	lcl_MoveControl( _pImp->_pBtnHelp, 0, nYOffset + 3 );
	// a little more spacing between Cancel- and HelpButton

	// Groesse des Dialoges anpassen.
	aSize = GetSizePixel();
	aSize.Height() += nYOffset;
	SetSizePixel( aSize );

	// Beschriftungen dem Modus anpassen.
	sal_uInt16 nResId = STR_EXPLORERFILE_OPEN;
	sal_uInt16 nButtonResId = 0;

	if ( nStyle & WB_SAVEAS )
	{
		nResId = STR_EXPLORERFILE_SAVE;
		nButtonResId = STR_EXPLORERFILE_BUTTONSAVE;
	}

	if ( ( nStyle & SFXWB_PATHDIALOG ) == SFXWB_PATHDIALOG )
	{
		_pImp->_pFtFileName->SetText( SvtResId( STR_PATHNAME ) );
		nResId = STR_PATHSELECT;
		nButtonResId = STR_BUTTONSELECT;
	}

	SetText( SvtResId( nResId ) );

	if ( nButtonResId )
		_pImp->_pBtnFileOpen->SetText( SvtResId( nButtonResId ) );

	if ( FILEDLG_TYPE_FILEDLG != _pImp->_eDlgType )
	{
		_pImp->_pFtFileType->Hide();
		_pImp->GetFilterListControl()->Hide();
	}

	// Einstellungen der Steuerelemente vornehmen.
	_pImp->_pBtnNewFolder->SetClickHdl( STATIC_LINK( this, SvtFileDialog, NewFolderHdl_Impl ) );
	_pImp->_pBtnFileOpen->SetClickHdl( STATIC_LINK( this, SvtFileDialog, OpenHdl_Impl ) );
	_pImp->_pBtnCancel->SetClickHdl( LINK( this, SvtFileDialog, CancelHdl_Impl ) );
	_pImp->SetFilterListSelectHdl( STATIC_LINK( this, SvtFileDialog, FilterSelectHdl_Impl ) );
	_pImp->_pEdFileName->SetGetFocusHdl( STATIC_LINK( this, SvtFileDialog, FileNameGetFocusHdl_Impl ) );
	_pImp->_pEdFileName->SetModifyHdl( STATIC_LINK( this, SvtFileDialog, FileNameModifiedHdl_Impl ) );
	_pFileView->SetSelectHdl( LINK( this, SvtFileDialog, SelectHdl_Impl ) );
	_pFileView->SetDoubleClickHdl( LINK( this, SvtFileDialog, DblClickHdl_Impl ) );
	_pFileView->SetOpenDoneHdl( LINK( this, SvtFileDialog, OpenDoneHdl_Impl ) );

	// Resourcen freigeben.
	FreeResource();

	// Timer fuer Filterbox Travel setzen
	_pImp->_aFilterTimer.SetTimeout( TRAVELFILTER_TIMEOUT );
	_pImp->_aFilterTimer.SetTimeoutHdl( STATIC_LINK( this, SvtFileDialog, FilterSelectHdl_Impl ) );

	if ( WB_SAVEAS & nStyle )
	{
		// different help ids if in save-as mode
		// 90744 - 09.08.2001 - frank.schoenheit@sun.com
		SetHelpId( HID_FILESAVE_DIALOG );

		_pImp->_pEdFileName->SetHelpId( HID_FILESAVE_FILEURL );
		_pImp->_pBtnFileOpen->SetHelpId( HID_FILESAVE_DOSAVE );
		_pImp->_pBtnNewFolder->SetHelpId( HID_FILESAVE_CREATEDIRECTORY );
		_pImp->_pBtnStandard->SetHelpId( HID_FILESAVE_DEFAULTDIRECTORY );
		_pImp->_pBtnUp->SetHelpId( HID_FILESAVE_LEVELUP );
		_pImp->GetFilterListControl()->SetHelpId( HID_FILESAVE_FILETYPE );
		_pFileView->SetHelpId( HID_FILESAVE_FILEVIEW );

		// formerly, there was only _pLbFileVersion, which was used for 3 different
		// use cases. For reasons of maintainability, I introduced extra members (_pLbTemplates, _pLbImageTemplates)
		// for the extra use cases, and separated _pLbFileVersion
		// I did not find out in which cases the help ID is really needed HID_FILESAVE_TEMPLATE - all
		// tests I made lead to a dialog where _no_ of the three list boxes was present.
		// 96930 - 15.08.2002 - fs@openoffice.org
		if ( _pImp->_pLbFileVersion )
			_pImp->_pLbFileVersion->SetHelpId( HID_FILESAVE_TEMPLATE );
		if ( _pImp->_pLbTemplates )
			_pImp->_pLbTemplates->SetHelpId( HID_FILESAVE_TEMPLATE );
		if ( _pImp->_pLbImageTemplates )
			_pImp->_pLbImageTemplates->SetHelpId( HID_FILESAVE_TEMPLATE );

		if ( _pImp->_pCbPassword ) _pImp->_pCbPassword->SetHelpId( HID_FILESAVE_SAVEWITHPASSWORD );
		if ( _pImp->_pCbAutoExtension ) _pImp->_pCbAutoExtension->SetHelpId( HID_FILESAVE_AUTOEXTENSION );
		if ( _pImp->_pCbOptions ) _pImp->_pCbOptions->SetHelpId( HID_FILESAVE_CUSTOMIZEFILTER );
		if ( _pCbSelection ) _pCbSelection->SetHelpId( HID_FILESAVE_SELECTION );
	}

	// correct the z-order of the controls
	implArrangeControls();

	// special URLs, such as favorites and "restricted" paths
	implInitializeSpecialURLLists( );

	/// read our settings from the configuration
	m_aConfiguration = OConfigurationTreeRoot::createWithServiceFactory(
		::comphelper::getProcessServiceFactory(),
		::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "/org.openoffice.Office.UI/FilePicker" ) )
	);
}

//*****************************************************************************

IMPL_STATIC_LINK( SvtFileDialog, NewFolderHdl_Impl, PushButton*, EMPTYARG )
{
	pThis->_pFileView->EndInplaceEditing( false );

	INetURLObject aObj( pThis->_pFileView->GetViewURL() );
	String sFolderName = aObj.getName( INetURLObject::LAST_SEGMENT, true, INetURLObject::DECODE_WITH_CHARSET, RTL_TEXTENCODING_UTF8 );
	svtools::QueryFolderNameDialog aDlg( pThis, sFolderName, String( SvtResId( STR_SVT_NEW_FOLDER ) ) );
	sal_Bool bHandled = sal_False;

	while ( !bHandled )
	{
		if ( aDlg.Execute() == RET_OK )
			bHandled = pThis->_pFileView->CreateNewFolder( aDlg.GetName() );
		else
			bHandled = sal_True;
	}

	return 0;
}

//*****************************************************************************

IMPL_STATIC_LINK_NOINSTANCE( SvtFileDialog, ViewHdl_Impl, ImageButton*, EMPTYARG )
{
	return 0;
}

//*****************************************************************************
//-----------------------------------------------------------------------------
sal_Bool SvtFileDialog::createNewUserFilter( const String& _rNewFilter, sal_Bool _bAllowUserDefExt )
{
	// delete the old user filter and create a new one
	DELETEZ( _pImp->_pUserFilter );
	_pImp->_pUserFilter = new SvtFileDialogFilter_Impl( _rNewFilter, _rNewFilter );

	// remember the extension
	sal_Bool bIsAllFiles = _rNewFilter.EqualsAscii( FILEDIALOG_FILTER_ALL );
	if ( bIsAllFiles )
		EraseDefaultExt();
	else
		SetDefaultExt( _rNewFilter.Copy( 2 ) );
		// TODO: this is nonsense. In the whole file there are a lot of places where we assume that a user filter
		// is always "*.<something>". But changing this would take some more time than I have now ...
		// 05.12.2001 - 95486 - fs@openoffice.org

	// now, the default extension is set to the one of the user filter (or empty)
	// if the former is not allowed (_bAllowUserDefExt = <FALSE/>), we have to use the ext of the current filter
	// (if possible)
	sal_Bool bUseCurFilterExt = sal_True;
	String sUserFilter = _pImp->_pUserFilter->GetType();
	xub_StrLen nSepPos = sUserFilter.SearchBackward( '.' );
	if ( STRING_NOTFOUND != nSepPos )
	{
		String sUserExt = sUserFilter.Copy( nSepPos + 1 );
		if	(	( STRING_NOTFOUND == sUserExt.Search( '*' ) )
			&&	( STRING_NOTFOUND == sUserExt.Search( '?' ) )
			)
			bUseCurFilterExt = sal_False;
	}

	if ( !_bAllowUserDefExt || bUseCurFilterExt )
	{
		if ( _pImp->GetCurFilter( ) )
			SetDefaultExt( _pImp->GetCurFilter( )->GetExtension() );
		else
			EraseDefaultExt();
	}

	// outta here
	return bIsAllFiles;
}

//-----------------------------------------------------------------------------
#define FLT_NONEMPTY		0x0001
#define FLT_CHANGED			0x0002
#define FLT_USERFILTER		0x0004
#define FLT_ALLFILESFILTER	0x0008

//-----------------------------------------------------------------------------
sal_uInt16 SvtFileDialog::adjustFilter( const String& _rFilter )
{
	sal_uInt16 nReturn = 0;

	const sal_Bool bNonEmpty = ( _rFilter.Len() != 0 );
	if ( bNonEmpty )
	{
		nReturn |= FLT_NONEMPTY;

		sal_Bool bFilterChanged = sal_True;

		// search for a corresponding filter
		SvtFileDialogFilter_Impl* pFilter = FindFilter_Impl( _rFilter, sal_False, bFilterChanged );

#ifdef AUTOSELECT_USERFILTER
		// if we found a filter which without allowing multi-extensions -> select it
		if ( pFilter )
		{
			_pImp->SelectFilterListEntry( pFilter->GetName() );
			_pImp->SetCurFilter( pFilter );
		}
#endif // AUTOSELECT_USERFILTER

		// look for multi-ext filters if necessary
		if ( !pFilter )
			pFilter = FindFilter_Impl( _rFilter, sal_True, bFilterChanged );

		if ( bFilterChanged )
			nReturn |= FLT_CHANGED;

		if ( !pFilter )
		{
			nReturn |= FLT_USERFILTER;
			// no filter found : use it as user defined filter
#ifdef AUTOSELECT_USERFILTER
			if ( createNewUserFilter( _rFilter, sal_True ) )
#else
			if ( createNewUserFilter( _rFilter, sal_False ) )
#endif
			{	// it's the "all files" filter
				nReturn |= FLT_ALLFILESFILTER;

#ifdef AUTOSELECT_USERFILTER
				// select the "all files" entry
				String sAllFilesFilter( SvtResId( STR_FILTERNAME_ALL ) );
				if ( _pImp->HasFilterListEntry( sAllFilesFilter ) )
				{
					_pImp->SelectFilterListEntry( sAllFilesFilter );
					_pImp->SetCurFilter( _pImp->GetSelectedFilterEntry( sAllFilesFilter ) );
				}
				else
					_pImp->SetNoFilterListSelection( );	// there is no "all files" entry
#endif // AUTOSELECT_USERFILTER
			}
#ifdef AUTOSELECT_USERFILTER
			else
				_pImp->SetNoFilterListSelection( );
#endif // AUTOSELECT_USERFILTER
		}
	}

	return nReturn;
}

//-----------------------------------------------------------------------------
IMPL_LINK( SvtFileDialog, CancelHdl_Impl, void*, EMPTYARG )
{
	if ( m_pCurrentAsyncAction.is() )
	{
		m_pCurrentAsyncAction->cancel();
		onAsyncOperationFinished();
	}
	else
	{
		EndDialog( sal_False );
	}
	return 1L;
}

//-----------------------------------------------------------------------------
IMPL_STATIC_LINK( SvtFileDialog, OpenHdl_Impl, void*, pVoid )
{
	if ( pThis->_pImp->_bMultiSelection && pThis->_pFileView->GetSelectionCount() > 1 )
	{
		// bei Multiselektion spezielles Open
		pThis->OpenMultiSelection_Impl();
		return 0;
	}

	String aFileName;
	String aOldPath( pThis->_pFileView->GetViewURL() );
	if ( pThis->_pImp->_bDoubleClick || pThis->_pFileView->HasChildPathFocus() )
		// Selection done by doubleclicking in the view, get filename from the view
		aFileName = pThis->_pFileView->GetCurrentURL();

	if ( !aFileName.Len() )
	{
		// if an entry is selected in the view ....
		if ( pThis->_pFileView->GetSelectionCount() )
		{	// -> use this one. This will allow us to step down this folder
			// #i8928# - 2002-12-20 - fs@openoffice.org
			aFileName = pThis->_pFileView->GetCurrentURL();
		}
	}

	if ( !aFileName.Len() )
	{
		if ( pThis->_pImp->_eMode == FILEDLG_MODE_OPEN && pThis->_pImp->_pEdFileName->IsTravelSelect() )
			// OpenHdl called from URLBox; travelling through the list of URLs should not cause an opening
			return 0; // MBA->PB: seems to be called never ?!

		// get the URL from from the edit field ( if not empty )
		if ( pThis->_pImp->_pEdFileName->GetText().Len() )
		{
			String aText = pThis->_pImp->_pEdFileName->GetText();

			// did we reach the root?
			if ( !INetURLObject( aOldPath ).getSegmentCount() )
			{
				if ( ( aText.Len() == 2 && aText.EqualsAscii( ".." ) ) ||
					 ( aText.Len() == 3 && ( aText.EqualsAscii( "..\\" ) || aText.EqualsAscii( "../" ) ) ) )
					// don't go higher than the root
					return 0;
			}

#if defined( UNX ) || defined( FS_PRIV_DEBUG )
			if ( ( 1 == aText.Len() ) && ( '~' == aText.GetBuffer()[0] ) )
			{
				// go to the home directory
				if ( lcl_getHomeDirectory( pThis->_pFileView->GetViewURL(), aFileName ) )
					// in case we got a home dir, reset the text of the edit
					pThis->_pImp->_pEdFileName->SetText( String() );
			}
			if ( !aFileName.Len() )
#endif
			{
				// get url from autocomplete edit
				aFileName = pThis->_pImp->_pEdFileName->GetURL();
			}
		}
		else if ( pVoid == pThis->_pImp->_pBtnFileOpen )
			// OpenHdl was called for the "Open" Button; if edit field is empty, use selected element in the view
			aFileName = pThis->_pFileView->GetCurrentURL();
	}

	// MBA->PB: ?!
	if ( !aFileName.Len() && pVoid == pThis->_pImp->_pEdFileName && pThis->_pImp->_pUserFilter )
	{
		DELETEZ( pThis->_pImp->_pUserFilter );
		return 0;
	}

	sal_uInt16 nLen = aFileName.Len();
	if ( !nLen )
	{
		// if the dialog was opened to select a folder, the last selected folder should be selected
		if( pThis->_pImp->_eDlgType == FILEDLG_TYPE_PATHDLG )
		{
			aFileName =	pThis->_pImp->_pFtCurrentPath->GetText();
			nLen = aFileName.Len();
		}
		else
			// no file selected !
			return 0;
	}

	// mark input as selected
	pThis->_pImp->_pEdFileName->SetSelection( Selection( 0, nLen ) );

	// if a path with wildcards is given, divide the string into path and wildcards
	String aFilter;
	if ( !pThis->IsolateFilterFromPath_Impl( aFileName, aFilter ) )
		return 0;

	// if a filter was retrieved, there were wildcards !
	sal_uInt16 nNewFilterFlags = pThis->adjustFilter( aFilter );
	if ( nNewFilterFlags & FLT_CHANGED )
	{
		// cut off all text before wildcard in edit and select wildcard
		pThis->_pImp->_pEdFileName->SetText( aFilter );
		pThis->_pImp->_pEdFileName->SetSelection( Selection( 0, aFilter.Len() ) );
	}

	{
		INetURLObject aFileObject( aFileName );
		if ( ( aFileObject.GetProtocol() == INET_PROT_NOT_VALID ) && aFileName.Len() )
		{
			String sCompleted = SvtURLBox::ParseSmart( aFileName, pThis->_pFileView->GetViewURL(), SvtPathOptions().GetWorkPath() );
			if ( sCompleted.Len() )
				aFileName = sCompleted;
		}
	}

	// Pr"ufen, ob es sich um einen Ordner handelt.
	sal_Bool bIsFolder = sal_False;

	// first thing before doing anything with the content: Reset it. When the user presses "open" (or "save" or "export",
	// for that matter), s/he wants the complete handling, including all possible error messages, even if s/he
	// does the same thing for the same content twice, s/he wants both fails to be displayed.
	// Without the reset, it could be that the content cached all relevant information, and will not display any
	// error messages for the same content a second time...
	pThis->m_aContent.bindTo( ::rtl::OUString( ) );

	// #97148# & #102204# ---------
	if ( aFileName.Len() )
	{
		// Make sure we have own Interaction Handler in place. We do not need
		// to intercept interactions here, but to record the fact that there
		// was an interaction.
		SmartContent::InteractionHandlerType eInterActionHandlerType
			= pThis->m_aContent.queryCurrentInteractionHandler();
		if ( ( eInterActionHandlerType == SmartContent::IHT_NONE ) ||
			 ( eInterActionHandlerType == SmartContent::IHT_DEFAULT ) )
			 pThis->m_aContent.enableOwnInteractionHandler(
				OFilePickerInteractionHandler::E_NOINTERCEPTION );

		bIsFolder = pThis->m_aContent.isFolder( aFileName );

		// access denied to the given resource - and interaction was already
		// used => break following operations
		OFilePickerInteractionHandler* pHandler
			= pThis->m_aContent.getOwnInteractionHandler();

		OSL_ENSURE( pHandler, "Got no Interaction Handler!!!" );

		if ( pHandler->wasAccessDenied() )
			return 0;

		if ( pThis->m_aContent.isInvalid() &&
		   ( pThis->_pImp->_eMode == FILEDLG_MODE_OPEN ) )
		{
			if ( !pHandler->wasUsed() )
				ErrorHandler::HandleError( ERRCODE_IO_NOTEXISTS );

			return 0;
		}

		// restore previous Interaction Handler
		if ( eInterActionHandlerType == SmartContent::IHT_NONE )
			pThis->m_aContent.disableInteractionHandler();
		else if ( eInterActionHandlerType == SmartContent::IHT_DEFAULT )
			pThis->m_aContent.enableDefaultInteractionHandler();
 	}

	if  (   !bIsFolder										// no existent folder
		&&	pThis->_pImp->_pCbAutoExtension					// auto extension is enabled in general
		&&	pThis->_pImp->_pCbAutoExtension->IsChecked()	// auto extension is really to be used
		&&	pThis->GetDefaultExt().Len()					// there is a default extension
		&&	pThis->GetDefaultExt() != '*'					// the default extension is not "all"
		&& !(   FILEDLG_MODE_SAVE == pThis->_pImp->_eMode	// we're saving a file
			&&  pThis->_pFileView->GetSelectionCount()		// there is a selected file in the file view -> it will later on
			)												// (in SvtFileDialog::GetPathList) be taken as file to save to
															// (#114818# - 2004-03-17 - fs@openoffice.org)
		&& FILEDLG_MODE_OPEN != pThis->_pImp->_eMode		// pb: #i83408# don't append extension on open
		)
	{
		// check extension and append the default extension if necessary
		appendDefaultExtension(aFileName,
							   pThis->GetDefaultExt(),
							   pThis->_pImp->GetCurFilter()->GetType());
	}

	sal_Bool bOpenFolder = ( FILEDLG_TYPE_PATHDLG == pThis->_pImp->_eDlgType ) &&
					   !pThis->_pImp->_bDoubleClick && pVoid != pThis->_pImp->_pEdFileName;
	if ( bIsFolder )
	{
		if ( bOpenFolder )
		{
			pThis->_aPath = aFileName;
		}
		else
		{
			if ( aFileName != pThis->_pFileView->GetViewURL() )
			{
				if ( !pThis->m_aURLFilter.isUrlAllowed( aFileName ) )
				{
					pThis->simulateAccessDenied( aFileName );
					return 0;
				}

				pThis->OpenURL_Impl( aFileName );
			}
			else
			{
				if ( nNewFilterFlags & FLT_CHANGED )
					pThis->ExecuteFilter();
			}

			return 0;
		}
	}
	else if ( !( nNewFilterFlags & FLT_NONEMPTY ) )
	{
		// Ggf. URL speichern.
		pThis->_aPath = aFileName;
	}
	else
	{
		// Ggf. neu filtern.
		if ( nNewFilterFlags & FLT_CHANGED )
			pThis->ExecuteFilter();
		return 0;
	}

	INetURLObject aFileObj( aFileName );
	if ( aFileObj.HasError() )
	{
		ErrorHandler::HandleError( ERRCODE_IO_GENERAL );
		return 0;
	}

	// if restrictions for the allowed folders are in place, we need to do a check here
	if ( !pThis->m_aURLFilter.isUrlAllowed( aFileObj.GetMainURL( INetURLObject::NO_DECODE ) ) )
	{
		pThis->simulateAccessDenied( aFileName );
		return 0;
	}

	switch ( pThis->_pImp->_eMode )
	{
		case FILEDLG_MODE_SAVE:
		{
			if ( ::utl::UCBContentHelper::Exists( aFileObj.GetMainURL( INetURLObject::NO_DECODE ) ) )
			{
				QueryBox aBox( pThis, WB_YES_NO, SvtResId( STR_SVT_ALREADYEXISTOVERWRITE ) );
				if ( aBox.Execute() != RET_YES )
					return 0;
			}
			else
			{
				String aCurPath;
				if ( ::utl::LocalFileHelper::ConvertURLToSystemPath( aFileName, aCurPath ) )
				{
					// if content does not exist: at least its path must exist
					INetURLObject aPathObj = aFileObj;
					aPathObj.removeSegment();
					// #97148# & #102204# ------------
					sal_Bool bFolder = pThis->m_aContent.isFolder( aPathObj.GetMainURL( INetURLObject::NO_DECODE ) );
					if ( !bFolder )
					{
						ErrorHandler::HandleError( ERRCODE_IO_NOTEXISTSPATH );
						return 0;
					}
				}
			}
		}
		break;

		case FILEDLG_MODE_OPEN:
		{
			// do an existence check herein, again
			// 16.11.2001 - 93107 - frank.schoenheit@sun.com

			if ( INET_PROT_FILE == aFileObj.GetProtocol( ) )
			{
				sal_Bool bExists = sal_False;
				// #102204# --------------
				bExists = pThis->m_aContent.is( aFileObj.GetMainURL( INetURLObject::NO_DECODE ) );


				if ( !bExists )
				{
					String sError( SvtResId( RID_FILEOPEN_NOTEXISTENTFILE ) );

					String sInvalidFile( aFileObj.GetMainURL( INetURLObject::DECODE_TO_IURI ) );
					if ( INET_PROT_FILE == aFileObj.GetProtocol() )
					{	// if it's a file URL, transform the URL into system notation
						::rtl::OUString sURL( sInvalidFile );
						::rtl::OUString sSystem;
						osl_getSystemPathFromFileURL( sURL.pData, &sSystem.pData );
						sInvalidFile = sSystem;
					}
					sError.SearchAndReplaceAscii( "$name$", sInvalidFile );

					ErrorBox aError( pThis, WB_OK, sError );
					aError.Execute();
					return 0;
				}
			}
		}
		break;

		default:
			DBG_ERROR("SvtFileDialog, OpenHdl_Impl: invalid mode!");
	}

	// Interessenten benachrichtigen.
	long nRet;

	if ( pThis->_aOKHdl.IsSet() )
		nRet = pThis->_aOKHdl.Call( pThis );
	else
		nRet = pThis->OK();

	if ( nRet )
	{
		pThis->EndDialog( sal_True );
	}

	return nRet;
}

//*****************************************************************************

void SvtFileDialog::EnableAutocompletion( sal_Bool _bEnable )
{
	_pImp->_pEdFileName->EnableAutocompletion( _bEnable );
}

//*****************************************************************************

IMPL_STATIC_LINK( SvtFileDialog, FilterSelectHdl_Impl, ListBox*, pBox )
{
	DBG_ASSERT( pBox, "SvtFileDialog:keine Instanz" );

	// wurde der Handler vom Travel-Timer gefeuert?
	if ( pBox == (ListBox*)&pThis->_pImp->_aFilterTimer )
	{
		// Anzeige erneut filtern.
		pThis->ExecuteFilter();
		return 0;
	}

	String sSelectedFilterDisplayName;
	SvtFileDialogFilter_Impl* pSelectedFilter = pThis->_pImp->GetSelectedFilterEntry( sSelectedFilterDisplayName );
	if ( !pSelectedFilter )
	{	// there is no current selection. This happens if for instance the user selects a group separator using
		// the keyboard, and then presses enter: When the selection happens, we immediately deselect the entry,
		// so in this situation there is no current selection.
		if ( restoreCurrentFilter( pThis->_pImp ) )
			pThis->ExecuteFilter();
	}
	else
	{
		if ( pSelectedFilter->isGroupSeparator() )
		{	// group separators can't be selected
			// return to the previously selected entry
			if ( pThis->_pImp->IsFilterListTravelSelect() )
			{
				pThis->_pImp->SetNoFilterListSelection( );

				// stop the timer for executing the filter
				if ( pThis->_pImp->_aFilterTimer.IsActive() )
					pThis->_pImp->m_bNeedDelayedFilterExecute = sal_True;
				pThis->_pImp->_aFilterTimer.Stop();
			}
			else
			{
				if ( restoreCurrentFilter( pThis->_pImp ) )
					pThis->ExecuteFilter();
			}
		}
		else if	(	( pSelectedFilter != pThis->_pImp->GetCurFilter() )
				||	pThis->_pImp->_pUserFilter
				)
		{
			// Store the old filter for the auto extension handling
			String sLastFilterExt = pThis->_pImp->GetCurFilter()->GetExtension();
			DELETEZ( pThis->_pImp->_pUserFilter );

			// Ggf. Filter des Benutzers entfernen.
			pThis->_pImp->SetCurFilter( pSelectedFilter, sSelectedFilterDisplayName );

			// Ggf. Endung anzeigen.
			pThis->SetDefaultExt( pSelectedFilter->GetExtension() );
			sal_uInt16 nSepPos = pThis->GetDefaultExt().Search( FILEDIALOG_DEF_EXTSEP );

			if ( nSepPos != STRING_NOTFOUND )
				pThis->EraseDefaultExt( nSepPos );

			// update the extension of the current file if necessary
			lcl_autoUpdateFileExtension( pThis, sLastFilterExt );

			// wenn der Benutzer schnell durch die Filterbox
			// travelt, nicht sofort Filtern
			if ( pThis->_pImp->IsFilterListTravelSelect() )
			{
				// FilterSelectHdl_Impl soll in
				// TRAVELFILTER_TIMEOUT ms neu gefeuert werden
				pThis->_pImp->_aFilterTimer.Start();
			}
			else
			{
				// evtl. vorher gestarteten Timer stoppen
				pThis->_pImp->_aFilterTimer.Stop();

				// Anzeige erneut filtern.
				pThis->ExecuteFilter();
			}
		}
	}

	return 0;
}

//*****************************************************************************

IMPL_STATIC_LINK( SvtFileDialog, FileNameGetFocusHdl_Impl, void*, EMPTYARG )
{
	pThis->_pFileView->SetNoSelection();
	pThis->_pFileView->Update();
	return 0;
}

//*****************************************************************************

IMPL_STATIC_LINK( SvtFileDialog, FileNameModifiedHdl_Impl, void*, EMPTYARG )
{
	FileNameGetFocusHdl_Impl( pThis, NULL );
	return 0;
}

//*****************************************************************************

SvtFileDialogFilter_Impl* SvtFileDialog::FindFilter_Impl
(
	const String& _rFilter,
	sal_Bool _bMultiExt,/*	sal_True - auch Filter mit mehreren Endungen
							beruecksichtigen
							sal_False - keine ...
						*/
	sal_Bool& _rFilterChanged
)

/*	[Beschreibung]

	Die Methode sucht in den eingefügten Filtern nach der
	spezifizierten Endung.
*/

{
	SvtFileDialogFilter_Impl* pFoundFilter = NULL;
	SvtFileDialogFilterList_Impl* pList = _pImp->_pFilter;
	sal_uInt16 nFilter = pList->Count();

	while ( nFilter-- )
	{
		SvtFileDialogFilter_Impl* pFilter = pList->GetObject( nFilter );
		const String& rType = pFilter->GetType();
		String aSingleType = rType;

		if ( _bMultiExt )
		{
			sal_uInt16 nIdx = 0;
			while ( !pFoundFilter && nIdx != STRING_NOTFOUND )
			{
				aSingleType = rType.GetToken( 0, FILEDIALOG_DEF_EXTSEP, nIdx );
#ifdef UNX
				if ( aSingleType.CompareTo( _rFilter ) == COMPARE_EQUAL )
#else
				if ( aSingleType.CompareIgnoreCaseToAscii( _rFilter ) == COMPARE_EQUAL )
#endif
					pFoundFilter = pFilter;
			}
		}
#ifdef UNX
		else if ( rType.CompareTo( _rFilter ) == COMPARE_EQUAL )
#else
		else if ( rType.CompareIgnoreCaseToAscii( _rFilter ) == COMPARE_EQUAL )
#endif
			pFoundFilter = pFilter;

		if ( pFoundFilter )
		{
			// activate filter
			_rFilterChanged = _pImp->_pUserFilter || ( _pImp->GetCurFilter() != pFilter );

			createNewUserFilter( _rFilter, sal_False );

			break;
		}
	}
	return pFoundFilter;
}

//*****************************************************************************

void SvtFileDialog::ExecuteFilter()
{
	_pImp->m_bNeedDelayedFilterExecute = sal_False;
	executeAsync( AsyncPickerAction::eExecuteFilter, String(), getMostCurrentFilter( _pImp ) );
}

//*****************************************************************************

void SvtFileDialog::OpenMultiSelection_Impl()

/*	[Beschreibung]

	OpenHandler für MultiSelektion
*/

{
	String aPath;
	sal_uLong nCount = _pFileView->GetSelectionCount();
	SvLBoxEntry* pEntry = nCount ? _pFileView->FirstSelected() : NULL;

	if ( nCount && pEntry )
		_aPath = _pFileView->GetURL( pEntry );

	// Interessenten benachrichtigen.
	long nRet;

	if ( _aOKHdl.IsSet() )
		nRet = _aOKHdl.Call( this );
	else
		nRet = OK();

	if ( nRet )
		EndDialog( sal_True );
}

//*****************************************************************************

void SvtFileDialog::UpdateControls( const String& rURL )
{
	_pImp->_pEdFileName->SetBaseURL( rURL );

	INetURLObject aObj( rURL );

	//=========================================================================
	{
		String sText;
		DBG_ASSERT( INET_PROT_NOT_VALID != aObj.GetProtocol(), "SvtFileDialog::UpdateControls: Invalid URL!" );

		if ( aObj.getSegmentCount() )
		{
			::utl::LocalFileHelper::ConvertURLToSystemPath( rURL, sText );
			if ( sText.Len() )
			{
				// no Fsys path for server file system ( only UCB has mountpoints! )
				if ( INET_PROT_FILE != aObj.GetProtocol() )
					sText = rURL.Copy( static_cast< sal_uInt16 >(
						INetURLObject::GetScheme( aObj.GetProtocol() ).getLength() ) );
			}

			if ( !sText.Len() && aObj.getSegmentCount() )
				sText = rURL;
		}

		// path mode ?
		if ( FILEDLG_TYPE_PATHDLG == _pImp->_eDlgType )
			// -> set new path in the edit field
			_pImp->_pEdFileName->SetText( sText );

		// in the "current path" field, truncate the trailing slash
		if ( aObj.hasFinalSlash() )
		{
			aObj.removeFinalSlash();
			String sURL( aObj.GetMainURL( INetURLObject::NO_DECODE ) );
			if ( !::utl::LocalFileHelper::ConvertURLToSystemPath( sURL, sText ) )
				sText = sURL;
		}

		if ( !sText.Len() && rURL.Len() )
			// happens, for instance, for URLs which the INetURLObject does not know to belong to a hierarchical scheme
			sText = rURL;
		_pImp->_pFtCurrentPath->SetText( sText );
	}

	//=========================================================================
	_aPath = rURL;
	if ( _pFileNotifier )
		_pFileNotifier->notify( DIRECTORY_CHANGED, 0 );
}

//*****************************************************************************

IMPL_LINK( SvtFileDialog, SelectHdl_Impl, SvTabListBox*, pBox )
{
	SvLBoxEntry* pEntry = pBox->FirstSelected();
	DBG_ASSERT( pEntry, "SelectHandler without selected entry" );
	SvtContentEntry* pUserData = (SvtContentEntry*)pEntry->GetUserData();

	if ( pUserData )
	{
		INetURLObject aObj( pUserData->maURL );
		if ( FILEDLG_TYPE_PATHDLG == _pImp->_eDlgType )
		{
			if ( aObj.GetProtocol() == INET_PROT_FILE )
			{
				if ( !pUserData->mbIsFolder )
					aObj.removeSegment();
				String aName = aObj.getFSysPath( (INetURLObject::FSysStyle)(INetURLObject::FSYS_DETECT & ~INetURLObject::FSYS_VOS) );
				_pImp->_pEdFileName->SetText( aName );
				_pImp->_pEdFileName->SetSelection( Selection( 0, aName.Len() ) );
				_aPath = pUserData->maURL;
			}
			else if ( !pUserData->mbIsFolder )
			{
				_pImp->_pEdFileName->SetText( pUserData->maURL );
				_pImp->_pEdFileName->SetSelection( Selection( 0, pUserData->maURL.Len() ) );
				_aPath = pUserData->maURL;
			}
			else
				_pImp->_pEdFileName->SetText( UniString() );
		}
		else
		{
			if ( !pUserData->mbIsFolder )
			{
				String aName = pBox->GetEntryText( pEntry, 0 );
				_pImp->_pEdFileName->SetText( aName );
				_pImp->_pEdFileName->SetSelection( Selection( 0, aName.Len() ) );
				_aPath = pUserData->maURL;
			}
		}
	}

	if ( _pImp->_bMultiSelection && _pFileView->GetSelectionCount() > 1 )
	{
		// bei Multiselektion den Datei-Edit leeren
		_pImp->_pEdFileName->SetText( String() );
	}

	FileSelect();

	return 0;
}

//*****************************************************************************

IMPL_LINK( SvtFileDialog, DblClickHdl_Impl, SvTabListBox*, EMPTYARG )
{
	_pImp->_bDoubleClick = sal_True;
	OpenHdl_Impl( this, NULL );
	_pImp->_bDoubleClick = sal_False;

	return 0;
}

//*****************************************************************************

IMPL_LINK( SvtFileDialog, EntrySelectHdl_Impl, ComboBox*, EMPTYARG )
{
	FileSelect();

	return 0;
}

//*****************************************************************************

IMPL_LINK( SvtFileDialog, OpenDoneHdl_Impl, SvtFileView*, pView )
{
	String sCurrentFolder( pView->GetViewURL() );
	// check if we can create new folders
	EnableControl( _pImp->_pBtnNewFolder, ContentCanMakeFolder( sCurrentFolder ) && m_aURLFilter.isUrlAllowed( sCurrentFolder, false ) );

	// check if we can travel one level up
	bool bCanTravelUp = ContentHasParentFolder( pView->GetViewURL() );
	if ( bCanTravelUp )
	{
		// additional check: the parent folder should not be prohibited
		INetURLObject aCurrentFolder( sCurrentFolder );
		DBG_ASSERT( INET_PROT_NOT_VALID != aCurrentFolder.GetProtocol(),
			"SvtFileDialog::OpenDoneHdl_Impl: invalid current URL!" );

		aCurrentFolder.removeSegment();
		bCanTravelUp &= m_aURLFilter.isUrlAllowed( aCurrentFolder.GetMainURL( INetURLObject::NO_DECODE ) );
	}
	EnableControl( _pImp->_pBtnUp, bCanTravelUp );

	return 0;
}

//*****************************************************************************

IMPL_LINK( SvtFileDialog, AutoExtensionHdl_Impl, CheckBox*, EMPTYARG )
{
	if ( _pFileNotifier )
		_pFileNotifier->notify( CTRL_STATE_CHANGED,
								CHECKBOX_AUTOEXTENSION );

	// update the extension of the current file if necessary
	lcl_autoUpdateFileExtension( this, _pImp->GetCurFilter()->GetExtension() );

	return 0;
}

//*****************************************************************************

IMPL_LINK( SvtFileDialog, ClickHdl_Impl, CheckBox*, pCheckBox )
{
	if ( ! _pFileNotifier )
		return 0;

	sal_Int16 nId = -1;

	if ( pCheckBox == _pImp->_pCbOptions )
		nId = CHECKBOX_FILTEROPTIONS;
	else if ( pCheckBox == _pCbSelection )
		nId = CHECKBOX_SELECTION;
	else if ( pCheckBox == _pCbReadOnly )
		nId = CHECKBOX_READONLY;
	else if ( pCheckBox == _pImp->_pCbPassword )
		nId = CHECKBOX_PASSWORD;
	else if ( pCheckBox == _pCbLinkBox )
		nId = CHECKBOX_LINK;
	else if ( pCheckBox == _pCbPreviewBox )
		nId = CHECKBOX_PREVIEW;

	if ( nId != -1 )
		_pFileNotifier->notify( CTRL_STATE_CHANGED, nId );

	return 0;
}

//*****************************************************************************

IMPL_LINK( SvtFileDialog, PlayButtonHdl_Impl, PushButton*, EMPTYARG )
{
	if ( _pFileNotifier )
		 _pFileNotifier->notify( CTRL_STATE_CHANGED,
								 PUSHBUTTON_PLAY );

	return 0;
}

//*****************************************************************************

long SvtFileDialog::Notify( NotifyEvent& rNEvt )

/*	[Beschreibung]

	Die Methode wird gerufen, <BACKSPACE> abzufangen.
*/

{
	sal_uInt16 nType = rNEvt.GetType();
	long nRet = 0;

	if ( EVENT_KEYINPUT == nType && rNEvt.GetKeyEvent() )
	{
		const KeyCode& rKeyCode = rNEvt.GetKeyEvent()->GetKeyCode();
		sal_uInt16 nCode = rKeyCode.GetCode();

		if ( !rKeyCode.GetModifier() &&
			 KEY_BACKSPACE == nCode && !_pImp->_pEdFileName->HasChildPathFocus() )
		{
			nRet = 0; //! (long)_pFileView->DoBeamerKeyInput( *rNEvt.GetKeyEvent() );

			if ( !nRet && _pImp->_pBtnUp->IsEnabled() )
			{
				PrevLevel_Impl();
				nRet = 1;
			}
		}
//		else if ( rKeyCode.IsMod1() && ( KEY_C == nCode || KEY_V == nCode || KEY_X == nCode ) )
//		{
/* (mhu)
			String aVerb = KEY_C == nCode ? UniString(RTL_CONSTASCII_USTRINGPARAM(SVT_MENUPART_VERB_COPY)) :
				( KEY_V == nCode ? UniString(RTL_CONSTASCII_USTRINGPARAM(SVT_MENUPART_VERB_PASTE)) : UniString(RTL_CONSTASCII_USTRINGPARAM(SVT_MENUPART_VERB_CUT)) );
//(dv)			if ( !CntPopupMenu::DoVerbCommand( aVerb, _pFileView->GetView() ) )
//(dv)				Sound::Beep();
*/
//		}
	}
	return nRet ? nRet : ModalDialog::Notify( rNEvt );
}

//*****************************************************************************

long SvtFileDialog::OK()
{
	return sal_True;
}

//*****************************************************************************

class SvtDefModalDialogParent_Impl
{
private:
	Window*	_pOld;

public:
	SvtDefModalDialogParent_Impl( Window *pNew ) :
		_pOld( Application::GetDefDialogParent() )
		{ Application::SetDefDialogParent( pNew ); }

	~SvtDefModalDialogParent_Impl() { Application::SetDefDialogParent( _pOld ); }
};

//*****************************************************************************

//---------------------------------------------------------------------
void SvtFileDialog::updateListboxLabelSizes()
{
	sal_Int16 nLineControlId[5] = {
		LISTBOX_VERSION, LISTBOX_TEMPLATE, LISTBOX_IMAGE_TEMPLATE, LISTBOX_FILTER, EDIT_FILEURL
	};

	// determine the maximum width needed for the listbox labels
	long nMaxWidth = 0;
	for ( sal_Int32 i=0; i<5; ++i )
	{
		FixedText* pLabel = static_cast< FixedText* >( getControl( nLineControlId[i], sal_True ) );
		if ( !pLabel )
			continue;
		nMaxWidth = ::std::max( pLabel->GetTextWidth( pLabel->GetText() ), nMaxWidth );
	}

	// ensure that all labels are wide enough
	for ( sal_Int32 i=0; i<5; ++i )
	{
		FixedText* pLabel = static_cast< FixedText* >( getControl( nLineControlId[i], sal_True ) );
		ListBox* pListbox = static_cast< ListBox* >( getControl( nLineControlId[i], sal_False ) );
		if ( !pLabel || !pListbox )
			continue;
		Size aCurrentSize( pLabel->GetSizePixel() );
		if ( aCurrentSize.Width() >= nMaxWidth )
			continue;

		long nChange = nMaxWidth - aCurrentSize.Width();
		pLabel->SetSizePixel( Size( nMaxWidth, aCurrentSize.Height() ) );

		aCurrentSize = pListbox->GetSizePixel();
		pListbox->SetSizePixel( Size( aCurrentSize.Width() - nChange, aCurrentSize.Height() ) );
		lcl_MoveControl( pListbox, nChange, 0 );
	}
}

namespace
{

bool implIsInvalid( const String & rURL )
{
	SmartContent aContent( rURL );
	aContent.enableOwnInteractionHandler( ::svt::OFilePickerInteractionHandler::E_DOESNOTEXIST );
	aContent.isFolder();	// do this _before_ asking isInvalid! Otherwise result might be wrong.
	return aContent.isInvalid();
}

}

//---------------------------------------------------------------------
String SvtFileDialog::implGetInitialURL( const String& _rPath, const String& _rFallback )
{
	// an URL parser for the fallback
	INetURLObject aURLParser;

	// set the path
	bool bWasAbsolute = sal_False;
	aURLParser = aURLParser.smartRel2Abs( _rPath, bWasAbsolute );

	// is it a valid folder?
	m_aContent.bindTo( aURLParser.GetMainURL( INetURLObject::NO_DECODE ) );
	sal_Bool bIsFolder = m_aContent.isFolder( );	// do this _before_ asking isInvalid!
	sal_Bool bIsInvalid = m_aContent.isInvalid();

	if ( bIsInvalid && m_bHasFilename && !aURLParser.hasFinalSlash() )
	{	// check if the parent folder exists
		// #108429# - 2003-03-26 - fs@openoffice.org
		INetURLObject aParent( aURLParser );
		aParent.removeSegment( );
		aParent.setFinalSlash( );
		bIsInvalid = implIsInvalid( aParent.GetMainURL( INetURLObject::NO_DECODE ) );
	}

	if ( bIsInvalid )
	{
		INetURLObject aFallback( _rFallback );
		bIsInvalid = implIsInvalid( aFallback.GetMainURL( INetURLObject::NO_DECODE ) );

		if ( !bIsInvalid )
			aURLParser = aFallback;
	}

	if ( bIsInvalid )
	{
		INetURLObject aParent( aURLParser );
		while ( bIsInvalid && aParent.removeSegment() )
		{
			aParent.setFinalSlash( );
			bIsInvalid = implIsInvalid( aParent.GetMainURL( INetURLObject::NO_DECODE ) );
		}

		if ( !bIsInvalid )
			aURLParser = aParent;
	}

	if ( !bIsInvalid && bIsFolder )
	{
		aURLParser.setFinalSlash();
	}
	return aURLParser.GetMainURL( INetURLObject::NO_DECODE );
}

//---------------------------------------------------------------------
short SvtFileDialog::Execute()
{
	if ( !PrepareExecute() )
		return 0;

	// Start des Dialogs.
	_bIsInExecute = sal_True;
	short nResult = ModalDialog::Execute();
	_bIsInExecute = sal_False;

	DBG_ASSERT( !m_pCurrentAsyncAction.is(), "SvtFilePicker::Execute: still running an async action!" );
		// the dialog should not be cancelable while an async action is running - first, the action
		// needs to be canceled

	// letztes Verzeichnis merken
	if ( RET_OK == nResult )
	{
		INetURLObject aURL( _aPath );
		if ( aURL.GetProtocol() == INET_PROT_FILE )
		{
			// nur bei File-URL's und nicht bei virtuelle Folder
			// das ausgew"ahlte Verzeichnis merken
			sal_Int32 nLevel = aURL.getSegmentCount();
			// #97148# & #102204# ------
			sal_Bool bDir = m_aContent.isFolder( aURL.GetMainURL( INetURLObject::NO_DECODE ) );
			// sal_Bool bClassPath = ( ( _pImp->_nStyle & SFXWB_CLASSPATH ) == SFXWB_CLASSPATH );
			if ( nLevel > 1 && ( FILEDLG_TYPE_FILEDLG == _pImp->_eDlgType || !bDir ) )
				aURL.removeSegment();
		}
	}

	return nResult;
}

//---------------------------------------------------------------------
void SvtFileDialog::StartExecuteModal( const Link& rEndDialogHdl )
{
	PrepareExecute();

	// Start des Dialogs.
//	_bIsInExecute = sal_True;
	ModalDialog::StartExecuteModal( rEndDialogHdl );
}

//-----------------------------------------------------------------------------
void SvtFileDialog::onAsyncOperationStarted()
{
	EnableUI( sal_False );
	// the cancel button must be always enabled
	_pImp->_pBtnCancel->Enable( sal_True );
	_pImp->_pBtnCancel->GrabFocus();
}

//-----------------------------------------------------------------------------
void SvtFileDialog::onAsyncOperationFinished()
{
	EnableUI( sal_True );
	m_pCurrentAsyncAction = NULL;
	if ( !m_bInExecuteAsync )
		_pImp->_pEdFileName->GrabFocus();
		// (if m_bInExecuteAsync is true, then the operation was finished within the minimum wait time,
		// and to the user, the operation appears to be synchronous)
}

//-------------------------------------------------------------------------
void SvtFileDialog::displayIOException( const String& _rURL, IOErrorCode _eCode )
{
	try
	{
		// create make a human-readable string from the URL
		String sDisplayPath( _rURL );
		::utl::LocalFileHelper::ConvertURLToSystemPath( _rURL, sDisplayPath );

		// build an own exception which tells "access denied"
		InteractiveAugmentedIOException aException;
		aException.Arguments.realloc( 2 );
		aException.Arguments[ 0 ] <<= ::rtl::OUString( sDisplayPath );
		aException.Arguments[ 1 ] <<= PropertyValue(
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Uri" ) ),
			-1, aException.Arguments[ 0 ], PropertyState_DIRECT_VALUE
		);
			// (formerly, it was sufficient to put the URL first parameter. Nowadays,
			// the services expects the URL in a PropertyValue named "Uri" ...)
		aException.Code = _eCode;
		aException.Classification = InteractionClassification_ERROR;

		// let and interaction handler handle this exception
		::comphelper::OInteractionRequest* pRequest = NULL;
		Reference< ::com::sun::star::task::XInteractionRequest > xRequest = pRequest =
			new ::comphelper::OInteractionRequest( makeAny( aException ) );
		pRequest->addContinuation( new ::comphelper::OInteractionAbort( ) );

		Reference< XInteractionHandler > xHandler(
			::comphelper::getProcessServiceFactory()->createInstance(
				::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.task.InteractionHandler") )
			),
			UNO_QUERY
		);
		if ( xHandler.is() )
			 xHandler->handle( xRequest );
	}
	catch( const Exception& )
	{
		DBG_ERROR( "iodlg::displayIOException: caught an exception!" );
	}
}

//-----------------------------------------------------------------------------
void SvtFileDialog::EnableUI( sal_Bool _bEnable )
{
	Enable( _bEnable );

	if ( _bEnable )
	{
		for ( ::std::set< Control* >::iterator aLoop = m_aDisabledControls.begin();
			  aLoop != m_aDisabledControls.end();
			  ++aLoop
			)
		{
			(*aLoop)->Enable( sal_False );
		}
	}
}

//-----------------------------------------------------------------------------
void SvtFileDialog::EnableControl( Control* _pControl, sal_Bool _bEnable )
{
	if ( !_pControl )
	{
		DBG_ERRORFILE( "SvtFileDialog::EnableControl: invalid control!" );
		return;
	}

	_pControl->Enable( _bEnable );

	if ( _bEnable )
	{
		::std::set< Control* >::iterator aPos = m_aDisabledControls.find( _pControl );
		if ( m_aDisabledControls.end() != aPos )
			m_aDisabledControls.erase( aPos );
	}
	else
		m_aDisabledControls.insert( _pControl );
}

//----------------------------------------------------------------------------

short SvtFileDialog::PrepareExecute()
{
	rtl::OUString aEnvValue;
	if ( getEnvironmentValue( "WorkDirMustContainRemovableMedia", aEnvValue ) &&
		 aEnvValue.equalsAscii( "1" ) )
	{
		try
		{
			INetURLObject aStdDir( GetStandardDir() );
			::ucbhelper::Content aCnt( rtl::OUString( aStdDir.GetMainURL(
													  INetURLObject::NO_DECODE ) ),
								 Reference< XCommandEnvironment >() );
			Sequence< rtl::OUString > aProps(2);
			aProps[0] = rtl::OUString::createFromAscii( "IsVolume" );
			aProps[1] = rtl::OUString::createFromAscii( "IsRemoveable" );

			Reference< XResultSet > xResultSet
				= aCnt.createCursor( aProps, ::ucbhelper::INCLUDE_FOLDERS_ONLY );
			if ( xResultSet.is() )
			{
				Reference< XRow > xRow( xResultSet, UNO_QUERY );

				bool bEmpty = true;
				if ( !xResultSet->next() )
				{
					// folder is empty
					bEmpty = true;
				}
				else
				{
// @@@ KSO 05/18/2006: support for removable media currently hardcoded/incomplete in OSL
//
//					do
//					{
//						// check, whether child is a removable volume
//						if ( xRow->getBoolean( 1 ) && !xRow->wasNull() )
//						{
//							if ( xRow->getBoolean( 2 ) && !xRow->wasNull() )
//							{
								bEmpty = false;
//								break;
//							}
//						}
//					}
//					while ( xResultSet->next() );
				}

				if ( bEmpty )
				{
					ErrorBox aBox( this, WB_OK, SvtResId( STR_SVT_NOREMOVABLEDEVICE ) );
					aBox.Execute();
					return 0;
				}
			}
		}
		catch ( ContentCreationException const & )
		{
		}
		catch ( CommandAbortedException const & )
		{
		}
	}

	// #102204# ---------------
	if ( ( _pImp->_nStyle & WB_SAVEAS ) && m_bHasFilename )
		// when doing a save-as, we do not want the handler to handle "this file does not exist" messages
		// - finally we're going to save that file, aren't we?
		// #105812# - 2002-12-02 - fs@openoffice.org
		m_aContent.enableOwnInteractionHandler(::svt::OFilePickerInteractionHandler::E_DOESNOTEXIST);
	else
		m_aContent.enableDefaultInteractionHandler();

	// #53016# evtl. nur ein Filename ohne Pfad?
	String aFileNameOnly;
	if( _aPath.Len() && (_pImp->_eMode == FILEDLG_MODE_SAVE)
					 && (_aPath.Search(':') == STRING_NOTFOUND)
					 && (_aPath.Search('\\') == STRING_NOTFOUND)
					 && (_aPath.Search('/') == STRING_NOTFOUND))
	{
		aFileNameOnly = _aPath;
		_aPath.Erase();
	}

	// kein Startpfad angegeben?
	if ( !_aPath.Len() )
	{
		// dann das Standard-Dir verwenden
		_aPath = lcl_ensureFinalSlash( _pImp->GetStandardDir() );

		// #53016# vorgegebener Dateiname an Pfad anh"angen
		if ( aFileNameOnly.Len() )
			_aPath += aFileNameOnly;
	}

	//.....................................................................
	_aPath = implGetInitialURL( _aPath, GetStandardDir() );

	if ( _pImp->_nStyle & WB_SAVEAS && !m_bHasFilename )
		// when doing a save-as, we do not want the handler to handle "this file does not exist" messages
		// - finally we're going to save that file, aren't we?
		m_aContent.enableOwnInteractionHandler(::svt::OFilePickerInteractionHandler::E_DOESNOTEXIST);

	//.....................................................................
	// care for possible restrictions on the paths we're allowed to show
	if ( !m_aURLFilter.isUrlAllowed( _aPath ) )
		_aPath = m_aURLFilter.getFilter()[0];

	// Ggf. Filter anzeigen.
	_pImp->InitFilterList();

	// Initialen Filter einstellen.
	sal_uInt16 nFilterCount = GetFilterCount();
	String aAll( SvtResId( STR_FILTERNAME_ALL ) );
	sal_Bool bHasAll = _pImp->HasFilterListEntry( aAll );
	if ( _pImp->GetCurFilter() || nFilterCount == 1 || ( nFilterCount == 2 && bHasAll ) )
	{
		// Ggf. einzigen Filter als aktuellen Filter setzen oder den einzigen
		// Filter, der nicht auf alle Dateien verweist.
		if ( !_pImp->GetCurFilter() )
		{
			sal_uInt16 nPos = 0;
			if ( 2 == nFilterCount && bHasAll )
			{
				nPos = nFilterCount;
				while ( nPos-- )
				{
					if ( GetFilterName( nPos ) != aAll )
						break;
				}
			}
			SvtFileDialogFilter_Impl* pNewCurFilter = _pImp->_pFilter->GetObject( nPos );
			DBG_ASSERT( pNewCurFilter, "SvtFileDialog::Execute: invalid filter pos!" );
			_pImp->SetCurFilter( pNewCurFilter, pNewCurFilter->GetName() );
		}

		// Anzeige anpassen.
		_pImp->SelectFilterListEntry( _pImp->GetCurFilter()->GetName() );
		SetDefaultExt( _pImp->GetCurFilter()->GetExtension() );
		sal_uInt16 nSepPos = GetDefaultExt().Search( FILEDIALOG_DEF_EXTSEP );
		if ( nSepPos != STRING_NOTFOUND )
			EraseDefaultExt( nSepPos );
	}
	else
	{
		// Ggf. Filter fuer alle Dateien setzen bzw. erzeugen.
		if ( !bHasAll )
		{
			SvtFileDialogFilter_Impl* pAllFilter = implAddFilter( aAll, UniString(RTL_CONSTASCII_USTRINGPARAM(FILEDIALOG_FILTER_ALL)) );
			_pImp->InsertFilterListEntry( pAllFilter );
			_pImp->SetCurFilter( pAllFilter, aAll );
		}
		_pImp->SelectFilterListEntry( aAll );
	}

	_pImp->_pDefaultFilter = _pImp->GetCurFilter();

	// HACK #50065#
	// ggf. Filter isolieren.
	String aFilter;

	if ( !IsolateFilterFromPath_Impl( _aPath, aFilter ) )
		return 0;

	sal_uInt16 nNewFilterFlags = adjustFilter( aFilter );
	if ( nNewFilterFlags & ( FLT_NONEMPTY | FLT_USERFILTER ) )
	{
		_pImp->_pEdFileName->SetText( aFilter );
	}
	// HACK #50065#

	// Instanz fuer den gesetzten Pfad erzeugen und anzeigen.
	INetURLObject aFolderURL( _aPath );
	String aFileName( aFolderURL.getName( INetURLObject::LAST_SEGMENT, false ) );
	xub_StrLen nFileNameLen = aFileName.Len();
	bool bFileToSelect = nFileNameLen != 0;
	if ( bFileToSelect && aFileName.GetChar( nFileNameLen - 1 ) != INET_PATH_TOKEN )
	{
		_pImp->_pEdFileName->SetText( GET_DECODED_NAME( aFolderURL ) );
		aFolderURL.removeSegment();
	}

	INetURLObject aObj = aFolderURL;
	if ( aObj.GetProtocol() == INET_PROT_FILE )
	{
		// Ordner als aktuelles Verzeichnis setzen.
		aObj.setFinalSlash();
	}

	UpdateControls( aObj.GetMainURL( INetURLObject::NO_DECODE ) );

	// Somebody might want to enable some controls according to the current filter
	FilterSelect();

	// Zustand der Steuerelemente anpassen.
//	EndListeningAll();

	ViewHdl_Impl( this, NULL );
	OpenURL_Impl( aObj.GetMainURL( INetURLObject::NO_DECODE ) );

	_pFileView->Show();
	SvtDefModalDialogParent_Impl aDefParent( this );

	// ggf. Gr"osse aus Ini lesen und setzen
	InitSize();

	return 1;
}

//-----------------------------------------------------------------------------
void SvtFileDialog::implInitializeSpecialURLLists( )
{
	m_aURLFilter = ::svt::RestrictedPaths();

	::std::vector< String > aFavourites;
	if ( m_aURLFilter.hasFilter() )
	{
		// if we have restrictions, then the "favourites" are the restricted folders only
		aFavourites = m_aURLFilter.getFilter();
		// for approved URLs, we needed the final slashes, for
		// favourites, we do not want to have them
		::std::for_each( aFavourites.begin(), aFavourites.end(), RemoveFinalSlash() );
	}
	else
	{
		::rtl::OUString sFavouritesList;
		if ( getEnvironmentValue( "PathFavourites", sFavouritesList ) )
			convertStringListToUrls( sFavouritesList, aFavourites, false );
	}

	DBG_ASSERT( _pImp->_pBtnStandard, "SvtFileDialog::implInitializeSpecialURLLists: how this?" );
	if ( _pImp->_pBtnStandard )
		_pImp->_pBtnStandard->SetFavouriteLocations( aFavourites );
}

//-----------------------------------------------------------------------------
void SvtFileDialog::executeAsync( ::svt::AsyncPickerAction::Action _eAction,
								  const String& _rURL, const String& _rFilter )
{
	DBG_ASSERT( !m_pCurrentAsyncAction.is(), "SvtFileDialog::executeAsync: previous async action not yet finished!" );

	m_pCurrentAsyncAction = new AsyncPickerAction( this, _pFileView, _eAction );

	bool bReallyAsync = true;
	m_aConfiguration.getNodeValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "FillAsynchronously" ) ) ) >>= bReallyAsync;

	sal_Int32 nMinTimeout = 0;
	m_aConfiguration.getNodeValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Timeout/Min" ) ) ) >>= nMinTimeout;
	sal_Int32 nMaxTimeout = 0;
	m_aConfiguration.getNodeValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Timeout/Max" ) ) ) >>= nMaxTimeout;

	m_bInExecuteAsync = true;
	m_pCurrentAsyncAction->execute( _rURL, _rFilter, bReallyAsync ? nMinTimeout : -1, nMaxTimeout, GetBlackList() );
	m_bInExecuteAsync = false;
}

//*****************************************************************************

void SvtFileDialog::FileSelect()
{
	if ( _pFileNotifier )
		_pFileNotifier->notify( FILE_SELECTION_CHANGED, 0 );
}

//*****************************************************************************

void SvtFileDialog::FilterSelect()
{
	if ( _pFileNotifier )
		_pFileNotifier->notify( CTRL_STATE_CHANGED,
								LISTBOX_FILTER );
}

//*****************************************************************************

void SvtFileDialog::SetStandardDir( const String& rStdDir )

/*	[Beschreibung]

	Die Methode setzt den Pfad f"ur den Standardknopf.
*/

{
	INetURLObject aObj( rStdDir );
	DBG_ASSERT( aObj.GetProtocol() != INET_PROT_NOT_VALID, "Invalid protocol!" );
	aObj.setFinalSlash();
	_pImp->SetStandardDir( aObj.GetMainURL( INetURLObject::NO_DECODE ) );
}

void SvtFileDialog::SetBlackList( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& rBlackList )
{
	_pImp->SetBlackList( rBlackList );
}

//*****************************************************************************

const ::com::sun::star::uno::Sequence< ::rtl::OUString >& SvtFileDialog::GetBlackList() const
{
	return _pImp->GetBlackList();
}
//*****************************************************************************

const String& SvtFileDialog::GetStandardDir() const

/*	[Beschreibung]

	Diese Methode gibt den eingestellten Standardpfad zur"uck.
*/

{
	return _pImp->GetStandardDir();
}

//*****************************************************************************

void SvtFileDialog::PrevLevel_Impl()
{
	_pFileView->EndInplaceEditing( false );

	String sDummy;
	executeAsync( AsyncPickerAction::ePrevLevel, sDummy, sDummy );
}

//*****************************************************************************

void SvtFileDialog::OpenURL_Impl( const String& _rURL )
{
	_pFileView->EndInplaceEditing( false );

	DBG_ASSERT( m_aURLFilter.isUrlAllowed( _rURL ), "SvtFileDialog::OpenURL_Impl: forbidden URL! Should have been handled by the caller!" );
	executeAsync( AsyncPickerAction::eOpenURL, _rURL, getMostCurrentFilter( _pImp ) );
}

//*****************************************************************************
SvtFileDialogFilter_Impl* SvtFileDialog::implAddFilter( const String& _rFilter, const String& _rType )
{
	SvtFileDialogFilter_Impl* pNewFilter = new SvtFileDialogFilter_Impl( _rFilter, _rType );
	_pImp->_pFilter->C40_INSERT( SvtFileDialogFilter_Impl, pNewFilter, (sal_uInt16)0 );

	if ( !_pImp->GetCurFilter() )
		_pImp->SetCurFilter( pNewFilter, _rFilter );

	return pNewFilter;
}

//*****************************************************************************

void SvtFileDialog::AddFilter( const String& _rFilter, const String& _rType )
{
	DBG_ASSERT( !IsInExecute(), "SvtFileDialog::AddFilter: currently executing!" );
	implAddFilter ( _rFilter, _rType );
}

//*****************************************************************************
void SvtFileDialog::AddFilterGroup( const String& _rFilter, const Sequence< StringPair >& _rFilters )
{
	DBG_ASSERT( !IsInExecute(), "SvtFileDialog::AddFilter: currently executing!" );

	implAddFilter( _rFilter, String() );
	const StringPair* pSubFilters		=				_rFilters.getConstArray();
	const StringPair* pSubFiltersEnd	= pSubFilters +	_rFilters.getLength();
	for ( ; pSubFilters != pSubFiltersEnd; ++pSubFilters )
		implAddFilter( pSubFilters->First, pSubFilters->Second );
}

//*****************************************************************************

//-----------------------------------------------------------------------------
void SvtFileDialog::SetCurFilter( const String& rFilter )
{
	DBG_ASSERT( !IsInExecute(), "SvtFileDialog::SetCurFilter: currently executing!" );

	// Entsprechenden Filter suchen.
	sal_uInt16 nPos = _pImp->_pFilter->Count();

	while ( nPos-- )
	{
		SvtFileDialogFilter_Impl* pFilter = _pImp->_pFilter->GetObject( nPos );
		if ( pFilter->GetName() == rFilter )
		{
			_pImp->SetCurFilter( pFilter, rFilter );
			break;
		}
	}
}

//*****************************************************************************

String SvtFileDialog::GetCurFilter() const
{
	String aFilter;

	const SvtFileDialogFilter_Impl* pCurrentFilter = _pImp->GetCurFilter();
	if ( pCurrentFilter )
		aFilter = pCurrentFilter->GetName();

	return aFilter;
}

String SvtFileDialog::getCurFilter( ) const
{
	return GetCurFilter();
}

//*****************************************************************************

sal_uInt16 SvtFileDialog::GetFilterCount() const
{
	return _pImp->_pFilter->Count();
}

//*****************************************************************************

const String& SvtFileDialog::GetFilterName( sal_uInt16 nPos ) const
{
	DBG_ASSERT( nPos < GetFilterCount(), "invalid index" );
	return _pImp->_pFilter->GetObject( nPos )->GetName();
}

//*****************************************************************************

void SvtFileDialog::InitSize()
{
	if ( ! _pImp->_aIniKey.Len() )
		return;

	Size aDlgSize = GetResizeOutputSizePixel();
	SetMinOutputSizePixel( aDlgSize );

	if ( !_pImp->_nFixDeltaHeight )
	{
		// Fixgr"ossen errechnen und merken
		Point aPnt = _pFileView->GetPosPixel();
		long nBoxH = _pFileView->GetSizePixel().Height();
		long nH = GetSizePixel().Height();
		_pImp->_nFixDeltaHeight = nH - nBoxH;
	}

	// initialize from config
	SvtViewOptions aDlgOpt( E_DIALOG, _pImp->_aIniKey );

	if ( aDlgOpt.Exists() )
	{
		SetWindowState( ByteString( String( aDlgOpt.GetWindowState() ), osl_getThreadTextEncoding() ) );

		Any aUserData = aDlgOpt.GetUserItem( ::rtl::OUString::createFromAscii( "UserData" ) );
		::rtl::OUString sCfgStr;
		if ( aUserData >>= sCfgStr )
			_pFileView->SetConfigString( String( sCfgStr ) );
	}
}

//*****************************************************************************

SvStringsDtor* SvtFileDialog::GetPathList() const
{
	SvStringsDtor*	pList = new SvStringsDtor;
	sal_uLong			nCount = _pFileView->GetSelectionCount();
	SvLBoxEntry*	pEntry = nCount ? _pFileView->FirstSelected() : NULL;

	if ( ! pEntry )
	{
		String* pURL;

		if ( _pImp->_pEdFileName->GetText().Len() && _bIsInExecute )
			pURL = new String( _pImp->_pEdFileName->GetURL() );
		else
			pURL = new String( _aPath );

		pList->Insert( pURL, pList->Count() );
	}
	else
	{
		while ( pEntry )
		{
			String* pURL = new String( _pFileView->GetURL( pEntry ) );
			pList->Insert( pURL, pList->Count() );
			pEntry = _pFileView->NextSelected( pEntry );
		}
	}

	return pList;
}

//*****************************************************************************

void SvtFileDialog::implArrangeControls()
{
	// this is the list of controls in the order they should be tabbed
	// from top left to bottom right
	// pb: #136070# new order so all LabeledBy relations are correct now
	Control* pControls[] =
	{
		_pImp->_pFtCurrentPath,
		_pImp->_pBtnUp, _pImp->_pBtnNewFolder, _pImp->_pBtnStandard,		// image buttons
		_pFileView,															// the file view
		_pImp->_pFtFileName, _pImp->_pEdFileName,
		_pImp->_pFtFileVersion, _pImp->_pLbFileVersion,
		_pImp->_pFtTemplates, _pImp->_pLbTemplates,
		_pImp->_pFtImageTemplates, _pImp->_pLbImageTemplates,
		_pImp->_pFtFileType, _pImp->GetFilterListControl(),					// edit fields/list boxes
		_pImp->_pCbPassword, _pImp->_pCbAutoExtension, _pImp->_pCbOptions,	// checkboxes
		_pCbReadOnly, _pCbLinkBox, _pCbPreviewBox, _pCbSelection, _pPbPlay,	// checkboxes (continued)
		_pImp->_pBtnFileOpen, _pImp->_pBtnCancel, _pImp->_pBtnHelp			// buttons

		// (including the FixedTexts is important - not for tabbing order (they're irrelevant there),
		// but for working keyboard shortcuts)
		// 96861 - 23.01.2002 - fs@openoffice.org
	};

	// loop through all these controls and adjust the z-order
	Window* pPreviousWin = NULL;
	Control** pCurrent = pControls;
	for ( sal_Int32 i = 0; i < sal_Int32(sizeof( pControls ) / sizeof( pControls[ 0 ] )); ++i, ++pCurrent )
	{
		if ( !*pCurrent )
			// this control is not available in the current operation mode -> skip
			continue;

		if ( pPreviousWin )
			(*pCurrent)->SetZOrder( pPreviousWin, WINDOW_ZORDER_BEHIND );
		else
			(*pCurrent)->SetZOrder( NULL, WINDOW_ZORDER_FIRST );

		pPreviousWin = *pCurrent;
	}

	// FileName edit not the first control but it should have the focus initially
	_pImp->_pEdFileName->GrabFocus();
}

//*****************************************************************************

sal_Bool SvtFileDialog::IsolateFilterFromPath_Impl( String& rPath, String& rFilter )
{
	String aEmpty;
	String aReversePath( rPath );
	aReversePath.Reverse();
	sal_uInt16 nQuestionMarkPos = rPath.Search( '?' );

	if ( nQuestionMarkPos != STRING_NOTFOUND )
	{
		// Fragezeichen als Wildcard nur bei Files
		INetProtocol eProt = INetURLObject::CompareProtocolScheme( rPath );

		if ( INET_PROT_NOT_VALID != eProt && INET_PROT_FILE != eProt )
			nQuestionMarkPos = STRING_NOTFOUND;
	}
	sal_uInt16 nWildCardPos = Min( rPath.Search( FILEDIALOG_DEF_WILDCARD ), nQuestionMarkPos );
	rFilter = aEmpty;

	if ( nWildCardPos != STRING_NOTFOUND )
	{
		sal_uInt16 nPathTokenPos = aReversePath.Search( INET_PATH_TOKEN );

		if ( nPathTokenPos == STRING_NOTFOUND )
		{
			String aDelim(
#if defined(WNT) || defined(OS2)
					'\\'
#else
					'/'
#endif
			);

			nPathTokenPos = aReversePath.Search( aDelim );
#if defined(OS2)
			if ( nPathTokenPos == STRING_NOTFOUND )
			{
				nPathTokenPos = aReversePath.Search( '/' );
			}
#endif
#if !defined( UNX )
			if ( nPathTokenPos == STRING_NOTFOUND )
			{
				nPathTokenPos = aReversePath.Search( ':' );
			}
#endif
		}

		// Syntax pr"ufen.
		if ( nPathTokenPos != STRING_NOTFOUND )
		{
			if ( nPathTokenPos < (rPath.Len() - nWildCardPos - 1) )
			{
				ErrorHandler::HandleError( ERRCODE_SFX_INVALIDSYNTAX );
				return sal_False;
			}

			// Filter abschneiden.
			rFilter = aReversePath;
			rFilter.Erase( nPathTokenPos );
			rFilter.Reverse();

			// Ordner bestimmen.
			rPath = aReversePath;
			rPath.Erase( 0, nPathTokenPos );
			rPath.Reverse();
		}
		else
		{
			rFilter = rPath;
			rPath = aEmpty;
		}
	}

	return sal_True;
}

//*****************************************************************************

//-----------------------------------------------------------------------------
void SvtFileDialog::implUpdateImages( )
{
	// determine high contrast mode
	{
		sal_Bool bIsHighContrast = GetSettings().GetStyleSettings().GetHighContrastMode();
		m_aImages = ImageList( SvtResId( bIsHighContrast ? RID_FILEPICKER_IMAGES_HC : RID_FILEPICKER_IMAGES ) );
	}

	// set the appropriate images on the buttons
	if ( _pImp->_pBtnUp )
		_pImp->_pBtnUp->SetModeImage( GetButtonImage( IMG_FILEDLG_BTN_UP ) );

	if ( _pImp->_pBtnStandard )
		_pImp->_pBtnStandard->SetModeImage( GetButtonImage( IMG_FILEDLG_BTN_STD ) );

	if ( _pImp->_pBtnNewFolder )
		_pImp->_pBtnNewFolder->SetModeImage( GetButtonImage( IMG_FILEDLG_CREATEFOLDER ) );
}

//-----------------------------------------------------------------------------
void SvtFileDialog::DataChanged( const DataChangedEvent& _rDCEvt )
{
	if ( DATACHANGED_SETTINGS == _rDCEvt.GetType() )
		implUpdateImages( );

	ModalDialog::DataChanged( _rDCEvt );
}

//-----------------------------------------------------------------------------
void SvtFileDialog::Resize()
{
	if ( IsRollUp() )
		return;

	Size aDlgSize = GetResizeOutputSizePixel();
	Size aOldSize = _pImp->_aDlgSize;
	_pImp->_aDlgSize = aDlgSize;
	long nWinDeltaW = 0;

	if ( _pPrevWin &&
		 _pPrevWin->GetPosPixel().X() > _pFileView->GetPosPixel().X() )
	{
		nWinDeltaW = _pPrevWin->GetOutputSizePixel().Width() + _pImp->_a6Size.Width();
	}

	Size aNewSize = _pFileView->GetSizePixel();
	Point aBoxPos( _pFileView->GetPosPixel() );
	long nDeltaY = aNewSize.Height();
	long nDeltaX = aNewSize.Width();
	aNewSize.Height() = aDlgSize.Height() - _pImp->_nFixDeltaHeight;
	aNewSize.Width() = aDlgSize.Width() - aBoxPos.X() - _pImp->_a6Size.Width() - nWinDeltaW;
	if ( aOldSize.Height() )
		nDeltaY = _pImp->_aDlgSize.Height() - aOldSize.Height();
	else
		nDeltaY = aNewSize.Height() - nDeltaY;
	nDeltaX = aNewSize.Width() - nDeltaX;

	if ( nWinDeltaW )
		nWinDeltaW = nDeltaX * 2 / 3;
	aNewSize.Width() -= nWinDeltaW;
	nDeltaX -= nWinDeltaW;

	_pFileView->SetSizePixel( aNewSize );

	if ( !nDeltaY && !nDeltaX )
		// Dieses Resize wurde nur zum Ein - oder Ausblenden des Indicators aufgerufen
		return;

	// -------------
	// move controls

	// controls to move vertically
	{
		Control* aMoveControlsVert[] =
		{
			_pImp->_pFtFileName, _pImp->_pEdFileName, _pImp->_pFtFileVersion, _pImp->_pLbFileVersion,
			_pImp->_pFtTemplates, _pImp->_pLbTemplates, _pImp->_pFtImageTemplates, _pImp->_pLbImageTemplates,
			_pImp->_pFtFileType, _pImp->GetFilterListControl(), _pCbReadOnly, _pCbLinkBox, _pCbPreviewBox,
			_pPbPlay, _pImp->_pCbPassword, _pImp->_pCbAutoExtension, _pImp->_pCbOptions, _pCbSelection
		};
		Control** ppMoveControls = aMoveControlsVert;
		Control** ppMoveControlsEnd = ppMoveControls + sizeof( aMoveControlsVert ) / sizeof( aMoveControlsVert[0] );
		for ( ; ppMoveControls != ppMoveControlsEnd; ++ppMoveControls )
			lcl_MoveControl( *ppMoveControls, 0, nDeltaY );
	}

	// controls to move vertically and horizontally
	{
		Control* aMoveControlsBoth[] =
		{
			_pImp->_pBtnFileOpen, _pImp->_pBtnCancel, _pImp->_pBtnHelp
		};
		Control** ppMoveControls = aMoveControlsBoth;
		Control** ppMoveControlsEnd = ppMoveControls + sizeof( aMoveControlsBoth ) / sizeof( aMoveControlsBoth[0] );
		for ( ; ppMoveControls != ppMoveControlsEnd; ++ppMoveControls )
			lcl_MoveControl( *ppMoveControls, nDeltaX, nDeltaY );
	}

	// controls to move horizontally
	{
		Control* aMoveControlsHor[] =
		{
			_pImp->_pBtnUp, _pImp->_pBtnNewFolder, _pImp->_pBtnStandard
		};
		Control** ppMoveControls = aMoveControlsHor;
		Control** ppMoveControlsEnd = ppMoveControls + sizeof( aMoveControlsHor ) / sizeof( aMoveControlsHor[0] );
		for ( ; ppMoveControls != ppMoveControlsEnd; ++ppMoveControls )
			lcl_MoveControl( *ppMoveControls, nDeltaX, 0 );
	}

	// ---------------
	// resize controls
	{
		Control* aSizeControls[] =
		{
			_pImp->_pEdFileName, _pImp->_pLbFileVersion, _pImp->_pLbTemplates, _pImp->_pLbImageTemplates,
			_pImp->GetFilterListControl(), _pImp->_pFtCurrentPath,
		};
		sal_Int32 nSizeControls = sizeof( aSizeControls ) / sizeof( aSizeControls[0] );
		Control** ppSizeControls = aSizeControls;
		for ( sal_Int32 j=0; j<nSizeControls; ++j, ++ppSizeControls )
		{
			if ( *ppSizeControls )
			{
				aNewSize = (*ppSizeControls)->GetSizePixel();
				aNewSize.Width() += nDeltaX;
				(*ppSizeControls)->SetSizePixel( aNewSize );
			}
		}
	}

	// zus"atzliche Controls ausrichten
	if ( _pPrevWin &&
		 _pPrevWin->GetPosPixel().X() > _pFileView->GetPosPixel().X() )
	{
		// Controls vom Typ Window speziell ausrichten
		// auch die Gr"osse anpassen
		Point aNewPos = _pPrevWin->GetPosPixel();
		aNewPos.X() += nDeltaX;
		_pPrevWin->SetPosPixel( aNewPos );
		_pPrevBmp->SetPosPixel( aNewPos );
		aNewSize = _pPrevWin->GetOutputSizePixel();
		aNewSize.Width() += nWinDeltaW;
		aNewSize.Height() += nDeltaY;
		if ( !aOldSize.Height() )
			aNewSize.Height() -= ( _pImp->_a6Size.Height() / 2 );
		_pPrevWin->SetOutputSizePixel( aNewSize );
		_pPrevBmp->SetOutputSizePixel( aNewSize );
		_pPrevBmp->Invalidate();
	}

	if ( _pFileNotifier )
		_pFileNotifier->notify( DIALOG_SIZE_CHANGED, 0 );
}

//*****************************************************************************

//-----------------------------------------------------------------------------
Control* SvtFileDialog::getControl( sal_Int16 _nControlId, sal_Bool _bLabelControl ) const
{
	Control* pReturn = NULL;

	switch ( _nControlId )
	{
		case CONTROL_FILEVIEW:
			pReturn = _bLabelControl ? NULL : static_cast< Control* >( _pFileView );
			break;

		case EDIT_FILEURL:
			pReturn =	_bLabelControl
					?	static_cast< Control* >( _pImp->_pFtFileName )
					:	static_cast< Control* >( _pImp->_pEdFileName );
			break;

		case EDIT_FILEURL_LABEL:
			pReturn = static_cast< Control* >( _pImp->_pFtFileName );
			break;

		case CHECKBOX_AUTOEXTENSION:
			pReturn = _pImp->_pCbAutoExtension;
			break;

		case CHECKBOX_PASSWORD:
			pReturn = _pImp->_pCbPassword;
			break;

		case CHECKBOX_FILTEROPTIONS:
			pReturn = _pImp->_pCbOptions;
			break;

		case CHECKBOX_READONLY:
			pReturn = _pCbReadOnly;
			break;

		case CHECKBOX_LINK:
			pReturn = _pCbLinkBox;
			break;

		case CHECKBOX_PREVIEW:
			pReturn = _pCbPreviewBox;
			break;

		case CHECKBOX_SELECTION:
			pReturn = _pCbSelection;
			break;

		case LISTBOX_FILTER:
			pReturn = _bLabelControl ? _pImp->_pFtFileType : _pImp->GetFilterListControl();
			break;

		case LISTBOX_FILTER_LABEL:
			pReturn = _pImp->_pFtFileType;
			break;

		case FIXEDTEXT_CURRENTFOLDER:
			pReturn = _pImp->_pFtCurrentPath;
			break;

		case LISTBOX_VERSION:
			pReturn =	_bLabelControl
					?	static_cast< Control* >( _pImp->_pFtFileVersion )
					:	static_cast< Control* >( _pImp->_pLbFileVersion );
			break;

		case LISTBOX_TEMPLATE:
			pReturn =	_bLabelControl
					?	static_cast< Control* >( _pImp->_pFtTemplates )
					:	static_cast< Control* >( _pImp->_pLbTemplates );
			break;

		case LISTBOX_IMAGE_TEMPLATE:
			pReturn =	_bLabelControl
					?	static_cast< Control* >( _pImp->_pFtImageTemplates )
					:	static_cast< Control* >( _pImp->_pLbImageTemplates );
			break;

		case LISTBOX_VERSION_LABEL:
			pReturn = _pImp->_pFtFileVersion;
			break;

		case LISTBOX_TEMPLATE_LABEL:
			pReturn = _pImp->_pFtTemplates;
			break;

		case LISTBOX_IMAGE_TEMPLATE_LABEL:
			pReturn = _pImp->_pFtImageTemplates;
			break;

		case PUSHBUTTON_OK:
			pReturn = _pImp->_pBtnFileOpen;
			break;

		case PUSHBUTTON_CANCEL:
			pReturn = _pImp->_pBtnCancel;
			break;

		case PUSHBUTTON_PLAY:
			pReturn = _pPbPlay;
			break;

		case PUSHBUTTON_HELP:
			pReturn = _pImp->_pBtnHelp;
			break;

		case TOOLBOXBUTOON_DEFAULT_LOCATION:
			pReturn = _pImp->_pBtnStandard;
			break;

		case TOOLBOXBUTOON_LEVEL_UP:
			pReturn = _pImp->_pBtnUp;
			break;

		case TOOLBOXBUTOON_NEW_FOLDER:
			pReturn = _pImp->_pBtnNewFolder;
			break;

		case LISTBOX_FILTER_SELECTOR:
			// only exists on SalGtkFilePicker
			break;

		default:
			DBG_ERRORFILE( "SvtFileDialog::getControl: invalid id!" );
	}
	return pReturn;
}

// -----------------------------------------------------------------------
void SvtFileDialog::enableControl( sal_Int16 _nControlId, sal_Bool _bEnable )
{
	Control* pControl = getControl( _nControlId, sal_False );
	if ( pControl )
		EnableControl( pControl, _bEnable );
	Control* pLabel = getControl( _nControlId, sal_True );
	if ( pLabel )
		EnableControl( pLabel, _bEnable );
}

// -----------------------------------------------------------------------
void SvtFileDialog::AddControls_Impl( )
{
	// create the "insert as link" checkbox, if needed
	if ( _nExtraBits & SFX_EXTRA_INSERTASLINK )
	{
		_pCbLinkBox = new CheckBox( this );
		_pCbLinkBox ->SetText( SvtResId( STR_SVT_FILEPICKER_INSERT_AS_LINK ) );
		_pCbLinkBox ->SetHelpId( HID_FILEDLG_LINK_CB );
		AddControl( _pCbLinkBox );
		ReleaseOwnerShip( _pCbLinkBox );
		_pCbLinkBox->SetClickHdl( LINK( this, SvtFileDialog, ClickHdl_Impl ) );
	}

	// create the "show preview" checkbox ( and the preview window, too ), if needed
	if ( _nExtraBits & SFX_EXTRA_SHOWPREVIEW )
	{
		_pImp->_aIniKey = IMPGRF_CONFIGNAME;
		// because the "<All Formats> (*.bmp,*...)" entry is to wide,
		// we need to disable the auto width feature of the filter box
		_pImp->DisableFilterBoxAutoWidth();

		// "Vorschau"
		_pCbPreviewBox = new CheckBox( this );
		_pCbPreviewBox->SetText( SvtResId( STR_SVT_FILEPICKER_SHOW_PREVIEW ) );
		_pCbPreviewBox->SetHelpId( HID_FILEDLG_PREVIEW_CB );
		AddControl( _pCbPreviewBox );
		ReleaseOwnerShip( _pCbPreviewBox );
		_pCbPreviewBox->SetClickHdl( LINK( this, SvtFileDialog, ClickHdl_Impl ) );

		// Preview-Fenster erst hier erzeugen
		_pPrevWin = new Window( this, WinBits( WB_BORDER ) );
		AddControl( _pPrevWin );
		ReleaseOwnerShip( _pPrevWin );
		_pPrevWin->Hide();

		_pPrevBmp = new FixedBitmap( this, WinBits( WB_BORDER ) );
		_pPrevBmp->SetBackground( Wallpaper( Color( COL_WHITE ) ) );
		_pPrevBmp->Show();
		_pPrevBmp->SetAccessibleName(SvtResId(STR_PREVIEW));
	}

	if ( _nExtraBits & SFX_EXTRA_AUTOEXTENSION )
	{
		_pImp->_pCbAutoExtension = new CheckBox( this, SvtResId( CB_AUTO_EXTENSION ) );
		_pImp->_pCbAutoExtension->SetText( SvtResId( STR_SVT_FILEPICKER_AUTO_EXTENSION ) );
		_pImp->_pCbAutoExtension->Check( sal_True );
		AddControl( _pImp->_pCbAutoExtension );
		ReleaseOwnerShip( _pImp->_pCbAutoExtension );
		_pImp->_pCbAutoExtension->SetClickHdl( LINK( this, SvtFileDialog, AutoExtensionHdl_Impl ) );
	}

	if ( _nExtraBits & SFX_EXTRA_FILTEROPTIONS )
	{
		_pImp->_pCbOptions = new CheckBox( this, SvtResId( CB_OPTIONS ) );
		_pImp->_pCbOptions->SetText( SvtResId( STR_SVT_FILEPICKER_FILTER_OPTIONS ) );
		AddControl( _pImp->_pCbOptions );
		ReleaseOwnerShip( _pImp->_pCbOptions );
		_pImp->_pCbOptions->SetClickHdl( LINK( this, SvtFileDialog, ClickHdl_Impl ) );
	}

	if ( _nExtraBits & SFX_EXTRA_SELECTION )
	{
		_pCbSelection = new CheckBox( this, SvtResId( CB_OPTIONS ) );
		_pCbSelection->SetText( SvtResId( STR_SVT_FILEPICKER_SELECTION ) );
		AddControl( _pCbSelection );
		ReleaseOwnerShip( _pCbSelection );
		_pCbSelection->SetClickHdl( LINK( this, SvtFileDialog, ClickHdl_Impl ) );
	}

	if ( _nExtraBits & SFX_EXTRA_PLAYBUTTON )
	{
		_pPbPlay = new PushButton( this );
		_pPbPlay->SetText( SvtResId( STR_SVT_FILEPICKER_PLAY ) );
		_pPbPlay->SetHelpId( HID_FILESAVE_DOPLAY );
		AddControl( _pPbPlay );
		ReleaseOwnerShip( _pPbPlay );
		_pPbPlay->SetClickHdl( LINK( this, SvtFileDialog, PlayButtonHdl_Impl ) );
	}

	if ( _nExtraBits & SFX_EXTRA_SHOWVERSIONS )
	{
		_pImp->_pFtFileVersion = new FixedText(	this, SvtResId( FT_EXPLORERFILE_SHARED_LISTBOX ) );
		_pImp->_pFtFileVersion->SetText( SvtResId( STR_SVT_FILEPICKER_VERSION ) );

		_pImp->_pLbFileVersion = new ListBox( this, SvtResId( LB_EXPLORERFILE_SHARED_LISTBOX ) );
		_pImp->_pLbFileVersion->SetHelpId( HID_FILEOPEN_VERSION );
	}
	else if ( _nExtraBits & SFX_EXTRA_TEMPLATES )
	{
		_pImp->_pFtTemplates = new FixedText( this, SvtResId( FT_EXPLORERFILE_SHARED_LISTBOX ) );
		_pImp->_pFtTemplates->SetText( SvtResId( STR_SVT_FILEPICKER_TEMPLATES ) );

		_pImp->_pLbTemplates = new ListBox( this, SvtResId( LB_EXPLORERFILE_SHARED_LISTBOX ) );
		_pImp->_pLbTemplates->SetHelpId( HID_FILEOPEN_VERSION );
			// This is strange. During the re-factoring during 96930, I discovered that this help id
			// is set in the "Templates mode". This was hidden in the previous implementation.
			// Shouldn't this be a more meaningful help id.
			// 96930 - 15.08.2002 - fs@openoffice.org
	}
	else if ( _nExtraBits & SFX_EXTRA_IMAGE_TEMPLATE )
	{
		_pImp->_pFtImageTemplates = new FixedText( this, SvtResId( FT_EXPLORERFILE_SHARED_LISTBOX ) );
		_pImp->_pFtImageTemplates->SetText( SvtResId( STR_SVT_FILEPICKER_IMAGE_TEMPLATE ) );

		_pImp->_pLbImageTemplates = new ListBox( this, SvtResId( LB_EXPLORERFILE_SHARED_LISTBOX ) );
		_pImp->_pLbImageTemplates->SetHelpId( HID_FILEOPEN_IMAGE_TEMPLATE );
	}
}

// -----------------------------------------------------------------------
sal_Int32 SvtFileDialog::getTargetColorDepth()
{
	if ( _pPrevBmp )
		return _pPrevBmp->GetBitCount();
	else
		return 0;
}

// -----------------------------------------------------------------------
sal_Int32 SvtFileDialog::getAvailableWidth()
{
	if ( _pPrevBmp )
		return _pPrevBmp->GetOutputSizePixel().Width();
	else
		return 0;
}

// -----------------------------------------------------------------------
sal_Int32 SvtFileDialog::getAvailableHeight()
{
	if ( _pPrevBmp )
		return _pPrevBmp->GetOutputSizePixel().Height();
	else
		return 0;
}

// -----------------------------------------------------------------------
void SvtFileDialog::setImage( sal_Int16 /*aImageFormat*/, const Any& rImage )
{
	if ( ! _pPrevBmp || ! _pPrevBmp->IsVisible() )
		return;

	Sequence < sal_Int8 > aBmpSequence;

	if ( rImage >>= aBmpSequence )
	{
		Bitmap			aBmp;
		SvMemoryStream	aData( aBmpSequence.getArray(),
							   aBmpSequence.getLength(),
							   STREAM_READ );
		ReadDIB(aBmp, aData, true);

		_pPrevBmp->SetBitmap( aBmp );
	}
	else
	{
		Bitmap aEmpty;
		_pPrevBmp->SetBitmap( aEmpty );
	}
}

// -----------------------------------------------------------------------
sal_Bool SvtFileDialog::setShowState( sal_Bool /*bShowState*/ )
{
	// #97633 for the system filedialog it's
	// useful to make the preview switchable
	// because the preview occupies
	// half of the size of the file listbox
	// which is not the case here,
	// so we (TRA/FS) decided not to make
	// the preview window switchable because
	// else we would have to change the layout
	// of the file dialog dynamically
	// support for set/getShowState is optionally
	// see com::sun::star::ui::dialogs::XFilePreview
	/*
	if ( _pPrevBmp )
	{
		_pPrevBmp->Show( bShowState );
		return sal_True;
	}
	else
		return sal_False;
	*/

	return sal_False;
}

// -----------------------------------------------------------------------
String SvtFileDialog::getCurrentFileText( ) const
{
	String sReturn;
	if ( _pImp && _pImp->_pEdFileName )
		sReturn = _pImp->_pEdFileName->GetText();
	return sReturn;
}

// -----------------------------------------------------------------------
void SvtFileDialog::setCurrentFileText( const String& _rText, bool _bSelectAll )
{
	if ( _pImp && _pImp->_pEdFileName )
	{
		_pImp->_pEdFileName->SetText( _rText );
		if ( _bSelectAll )
			_pImp->_pEdFileName->SetSelection( Selection( 0, _rText.Len() ) );
	}
}

// -----------------------------------------------------------------------
sal_Bool SvtFileDialog::isAutoExtensionEnabled()
{
	return _pImp->_pCbAutoExtension && _pImp->_pCbAutoExtension->IsChecked();
}

// -----------------------------------------------------------------------
sal_Bool SvtFileDialog::getShowState()
{
	if ( _pPrevBmp )
		return _pPrevBmp->IsVisible();
	else
		return sal_False;
}

// -----------------------------------------------------------------------
void SvtFileDialog::ReleaseOwnerShip( Window* pUserControl )

/*
  [Beschreibung]
  Die Methode sorgt dafuer das das spezifizierte Element nicht mehr im Besitz
  der Instanz ist.
*/

{
	ControlChain_Impl* pElement = _pUserControls;
	while ( pElement )
	{
		if ( pElement->_pControl == pUserControl )
		{
			pElement->_bHasOwnerShip = sal_False;
			break;
		}
		pElement = pElement->_pNext;
	}
}

//***************************************************************************

sal_Bool SvtFileDialog::AddControl( Window* pControl, sal_Bool bNewLine )
{
	// control already exists
	ControlChain_Impl* pElement = _pUserControls;
	while ( pElement )
	{
		if ( pElement->_pControl == pControl )
			return sal_False;
		pElement = pElement->_pNext;
	}

	// Check if controls have already been added.
	Size aNewControlSize( pControl->GetOutputSizePixel() );
	Size aDlgSize( GetOutputSizePixel() );
	WindowType nType = pControl->GetType();
	if ( !aNewControlSize.Height() )
	{
		// Detect a size.
		Size aSize( 0, 10 );
		if ( nType == WINDOW_PUSHBUTTON )
		{
			Size aDefSiz = LogicToPixel( Size( 50, 14 ), MAP_APPFONT );
			long nTextWidth = pControl->GetTextWidth( pControl->GetText() );
			aSize.Width() = nTextWidth + WIDTH_ADDITION;

			// PushButton:	Mindestbreite 50 logische Einheiten,
			//				H"ohe immer 14 logische Einheiten
			if ( aDefSiz.Width() > aSize.Width() )
				aSize.Width() = aDefSiz.Width();
			aSize.Height() = aDefSiz.Height();
			aNewControlSize = aSize;
		}
		else
			aNewControlSize = LogicToPixel( aSize, MAP_APPFONT );
		if ( nType != WINDOW_PUSHBUTTON )
			aNewControlSize.Width() = pControl->GetTextWidth( pControl->GetText() ) + WIDTH_ADDITION;
		if ( nType == WINDOW_CHECKBOX )
			aNewControlSize.Width() += WIDTH_ADDITION;
		if ( nType == WINDOW_WINDOW )
		{
			aNewControlSize.Height() = GetOutputSizePixel().Height() - 18;
			aNewControlSize.Width() = 200;
			aDlgSize.Width() += 210;
			SetOutputSizePixel( aDlgSize );
		}
		pControl->SetOutputSizePixel( aNewControlSize );
	}
	Point aNewControlPos;
	Size* pNewDlgSize = NULL;
	sal_Bool bNewRow = bNewLine;
	sal_Bool bFirstNewRow = sal_False;

	if ( nType == WINDOW_WINDOW )
	{
		aNewControlPos.X() = aDlgSize.Width() - 210;
		aNewControlPos.Y() = 8;
	}
	else if ( _pUserControls )
	{
		Point aNewControlRange( _pUserControls->_pControl->GetPosPixel() );
		long nPrevControlHeight = _pUserControls->_pControl->GetSizePixel().Height();
		aNewControlRange +=
			Point( _pUserControls->_pControl->GetOutputSizePixel().Width(), 0 );
		aNewControlPos = aNewControlRange;
		if ( nPrevControlHeight > aNewControlSize.Height() )
		{
			long nY = nPrevControlHeight;
			nY -= aNewControlSize.Height();
			nY /= 2;
			aNewControlPos.Y() += nY;
		}
		aNewControlPos += LogicToPixel( Point( 3, 0 ), MAP_APPFONT );
		aNewControlRange += LogicToPixel( Point( 9, 0 ), MAP_APPFONT );
		aNewControlRange += Point( aNewControlSize.Width(), 0 );

		// Check if a new row has to be created.
		if ( aNewControlRange.X() > aDlgSize.Width() )
			bNewRow = sal_True;
	}
	else
	{
		// Create a new row if there was no usercontrol before.
		bNewRow = sal_True;
		bFirstNewRow = sal_True;
	}

	// Check if a new row has to be created.
	Size aBorderSize = LogicToPixel( Size( 6, 6 ), MAP_APPFONT );
	long nLeftBorder = aBorderSize.Width();
	long nLowerBorder = aBorderSize.Height();
	if ( bNewRow )
	{
		// Set control at the beginning of a new line.
		long nSmallBorderHeight = nLowerBorder / 2;
		aNewControlPos = Point( nLeftBorder, 0 );
		aNewControlPos += Point( 0, aDlgSize.Height() );
		aNewControlPos.Y() -= nSmallBorderHeight;
		// Set new size.
		pNewDlgSize = new Size( aDlgSize );
		pNewDlgSize->Height() -= nSmallBorderHeight;
		pNewDlgSize->Height() += aNewControlSize.Height();
		pNewDlgSize->Height() += nLowerBorder;
	}
	else
	{
		// Check if the window has to be resized.
		Size aNewControlRange( 0, aNewControlPos.Y() );
		aNewControlRange.Height() += aNewControlSize.Height();
		aNewControlRange.Height() += nLowerBorder;
		if ( aNewControlRange.Height() > aDlgSize.Height() )
			pNewDlgSize = new Size( aDlgSize.Width(), aNewControlRange.Height() );
	}

	// Update view.
	if ( pNewDlgSize )
	{
		SetOutputSizePixel( *pNewDlgSize );
		delete pNewDlgSize;
	}
	pControl->SetPosPixel( aNewControlPos );
	pControl->Show();
	_pUserControls = new ControlChain_Impl( pControl, _pUserControls );

	return sal_True;
}

sal_Bool SvtFileDialog::ContentHasParentFolder( const rtl::OUString& rURL )
{
	m_aContent.bindTo( rURL );

	if ( m_aContent.isInvalid() )
		return sal_False;

	return m_aContent.hasParentFolder( ) && m_aContent.isValid();
}

sal_Bool SvtFileDialog::ContentCanMakeFolder( const rtl::OUString& rURL )
{
	m_aContent.bindTo( rURL );

	if ( m_aContent.isInvalid() )
		return sal_False;

	return m_aContent.canCreateFolder( ) && m_aContent.isValid();
}

sal_Bool SvtFileDialog::ContentGetTitle( const rtl::OUString& rURL, String& rTitle )
{
	m_aContent.bindTo( rURL );

	if ( m_aContent.isInvalid() )
		return sal_False;

	::rtl::OUString sTitle;
	m_aContent.getTitle( sTitle );
	rTitle = sTitle;

	return m_aContent.isValid();
}

void SvtFileDialog::appendDefaultExtension(String& _rFileName,
										   const String& _rFilterDefaultExtension,
										   const String& _rFilterExtensions)
{
	String aTemp(_rFileName);
	aTemp.ToLowerAscii();
	String aType(_rFilterExtensions);
	aType.ToLowerAscii();

	if ( ! aType.EqualsAscii(FILEDIALOG_FILTER_ALL) )
	{
		sal_uInt16 nWildCard = aType.GetTokenCount( FILEDIALOG_DEF_EXTSEP );
		sal_uInt16 nIndex, nPos = 0;

		for ( nIndex = 0; nIndex < nWildCard; nIndex++ )
		{
			String aExt(aType.GetToken( 0, FILEDIALOG_DEF_EXTSEP, nPos ));
			// take care of a leading *
			sal_uInt16 nExtOffset = (aExt.GetBuffer()[0] == '*' ? 1 : 0);
			sal_Unicode* pExt = aExt.GetBufferAccess() + nExtOffset;
			xub_StrLen nExtLen = aExt.Len() - nExtOffset;
			xub_StrLen nOffset = aTemp.Len() - nExtLen;
			// minimize search by starting at last possible index
			if ( aTemp.Search(pExt, nOffset) == nOffset )
				break;
		}

		if ( nIndex >= nWildCard )
		{
			_rFileName += '.';
			_rFileName += _rFilterDefaultExtension;
		}
	}
}

// -----------------------------------------------------------------------

// QueryFolderNameDialog -------------------------------------------------------

namespace svtools {

QueryFolderNameDialog::QueryFolderNameDialog
(
	Window* _pParent,
	const String& rTitle,
	const String& rDefaultText,
	String* pGroupName
) :
	ModalDialog( _pParent, SvtResId( DLG_SVT_QUERYFOLDERNAME ) ),

	aNameText	( this, SvtResId( FT_SVT_QUERYFOLDERNAME_DLG_NAME ) ),
	aNameEdit	( this, SvtResId( ED_SVT_QUERYFOLDERNAME_DLG_NAME ) ),
	aNameLine	( this, SvtResId( FL_SVT_QUERYFOLDERNAME_DLG_NAME ) ),
	aOKBtn		( this, SvtResId( BT_SVT_QUERYFOLDERNAME_DLG_OK ) ),
	aCancelBtn	( this, SvtResId( BT_SVT_QUERYFOLDERNAME_DLG_CANCEL ) )
{
	FreeResource();
	SetText( rTitle );
	aNameEdit.SetText( rDefaultText );
	aNameEdit.SetSelection( Selection( 0, rDefaultText.Len() ) );
	aOKBtn.SetClickHdl( LINK( this, QueryFolderNameDialog, OKHdl ) );
	aNameEdit.SetModifyHdl( LINK( this, QueryFolderNameDialog, NameHdl ) );

	if ( pGroupName )
		aNameLine.SetText( *pGroupName );
};

// -----------------------------------------------------------------------
IMPL_LINK( QueryFolderNameDialog, OKHdl, Button *, EMPTYARG )
{
	// trim the strings
	aNameEdit.SetText( aNameEdit.GetText().EraseLeadingChars().EraseTrailingChars() );
	EndDialog( RET_OK );
	return 1;
}

// -----------------------------------------------------------------------
IMPL_LINK( QueryFolderNameDialog, NameHdl, Edit *, EMPTYARG )
{
	// trim the strings
	String aName = aNameEdit.GetText();
	aName.EraseLeadingChars().EraseTrailingChars();
	if ( aName.Len() )
	{
		if ( !aOKBtn.IsEnabled() )
			aOKBtn.Enable( sal_True );
	}
	else
	{
		if ( aOKBtn.IsEnabled() )
			aOKBtn.Enable( sal_False );
	}

	return 0;
}

}
