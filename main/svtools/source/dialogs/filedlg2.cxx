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
#include "precompiled_svtools.hxx"
#include <vcl/svapp.hxx>
#include <vcl/button.hxx>
#include <vcl/fixed.hxx>
#include <vcl/edit.hxx>
#include <vcl/lstbox.hxx>
#include <svtools/svtdata.hxx>
#include <filedlg2.hxx>
#include <svtools/filedlg.hxx>
#include <svtools/filedlg2.hrc>
#include <vcl/msgbox.hxx>
#include <vos/security.hxx>
#include <com/sun/star/i18n/XCollator.hpp>

#include <svtools/stdctrl.hxx>

#ifdef _MSC_VER
#pragma optimize ("", off)
#endif

#include <svtools/helpid.hrc>

using namespace com::sun::star;
using namespace com::sun::star::uno;


DECLARE_LIST( UniStringList, UniString* )

#define STD_BTN_WIDTH	80
#define STD_BTN_HEIGHT	26

#ifndef UNX
	#define ALLFILES				"*.*"
#else
	#define ALLFILES				"*"
#endif
//		#define STD_BTN_WIDTH	90
//		#define STD_BTN_HEIGHT	35

#define INITCONTROL( p, ControlClass, nBits, aPos, aSize, aTitel, rHelpId ) \
	p = new ControlClass( GetPathDialog(), WinBits( nBits ) ); \
	p->SetHelpId( rHelpId ); \
	p->SetPosSizePixel( aPos, aSize ); \
	p->SetText( aTitel ); \
	p->Show();


inline sal_Bool IsPrintable( sal_Unicode c )
{
	return c >= 32 && c != 127 ? sal_True : sal_False;
}

long
KbdListBox::PreNotify( NotifyEvent& rNEvt )
{
	if ( rNEvt.GetType() == EVENT_KEYINPUT )
	{
		KeyEvent aKeyEvt	= *rNEvt.GetKeyEvent();
		sal_Unicode  cCharCode	= aKeyEvt.GetCharCode();

		if ( IsPrintable ( cCharCode ) )
		{
			sal_uInt16 nCurrentPos = GetSelectEntryPos();
			sal_uInt16 nEntries    = GetEntryCount();

			for ( sal_uInt16 i = 1; i < nEntries; i++ )
			{
				UniString aEntry = GetEntry ( (i + nCurrentPos) % nEntries );
				aEntry.EraseLeadingChars( ' ' );
				aEntry.ToUpperAscii();
				UniString aCompare( cCharCode );
				aCompare.ToUpperAscii();

				if ( aEntry.CompareTo( aCompare, 1 ) == COMPARE_EQUAL )
				{
					SelectEntryPos ( (i + nCurrentPos) % nEntries );
					break;
				}
			}
		}
		else
		if ( aKeyEvt.GetKeyCode().GetCode() == KEY_RETURN )
		{
			DoubleClick();
		}
	}

	return ListBox::PreNotify ( rNEvt );
}

ImpPathDialog::ImpPathDialog( PathDialog* pDlg, RESOURCE_TYPE nType, sal_Bool bCreateDir )
{
	pSvPathDialog = pDlg;
	nDirCount = 0;

	// initialize Controls if not used as a base class
	if ( nType == WINDOW_PATHDIALOG )
	{
		InitControls();
		if( pNewDirBtn )
			pNewDirBtn->Enable( bCreateDir );
	}

	pDlg->SetHelpId( HID_FILEDLG_PATHDLG );

    lang::Locale aLocale = Application::GetSettings().GetLocale(); 
    xCollator = ::vcl::unohelper::CreateCollator();
    if( xCollator.is() )
        xCollator->loadDefaultCollator( aLocale, 1 );
    DBG_ASSERT( xCollator.is(), "not collator service for path dialog" );
}

ImpPathDialog::~ImpPathDialog()
{
	delete pEdit;
	delete pDirTitel;
	delete pDirList;
	delete pDirPath;
	delete pDriveList;
	delete pDriveTitle;
	delete pLoadBtn;
	delete pOkBtn;
	delete pCancelBtn;
	delete pNewDirBtn;
#	if defined(UNX) || defined(OS2)
	delete pHomeBtn;
#	endif
}

void ImpPathDialog::InitControls()
{
	PathDialog* pDlg = GetPathDialog();
	pDlg->SetText( UniString( SvtResId( STR_FILEDLG_SELECT ) ) );

	Size a3Siz = pDlg->LogicToPixel( Size( 3, 3 ), MAP_APPFONT );
	Size a6Siz = pDlg->LogicToPixel( Size( 6, 6 ), MAP_APPFONT );
	Size aBtnSiz = pDlg->LogicToPixel( Size( 50, 14 ), MAP_APPFONT );
	Size aFTSiz = pDlg->LogicToPixel( Size( 142, 10 ), MAP_APPFONT );
	Size aEDSiz = pDlg->LogicToPixel( Size( 142, 12 ), MAP_APPFONT );
	Point aPnt( a6Siz.Width(), a6Siz.Height() );
	long nLbH1 = pDlg->LogicToPixel( Size( 0, 93 ), MAP_APPFONT ).Height();
	long nH = 0;
	UniString aEmptyStr;

	INITCONTROL( pDirTitel, FixedText, 0,
				 aPnt, aFTSiz, UniString( SvtResId( STR_FILEDLG_DIR ) ), HID_FILEDLG_DIR );
	aPnt.Y() += aFTSiz.Height() + a3Siz.Height();

	INITCONTROL( pEdit, Edit, WB_BORDER, aPnt, aEDSiz, aPath.GetFull(), HID_FILEDLG_EDIT );

	aPnt.Y() += aEDSiz.Height() + a3Siz.Height();
#ifndef UNX
	long nLbH2 = pDlg->LogicToPixel( Size( 0, 60 ), MAP_APPFONT ).Height();
	INITCONTROL( pDirList, KbdListBox, WB_AUTOHSCROLL | WB_BORDER,
		aPnt, Size( aEDSiz.Width(), nLbH1 ), aEmptyStr, HID_FILEDLG_DIRS );
	aPnt.Y() += nLbH1 + a6Siz.Height();
	INITCONTROL( pDriveTitle, FixedText, 0,
				 aPnt, aFTSiz, UniString( SvtResId( STR_FILEDLG_DRIVES ) ), HID_FILEDLG_DRIVE );
	aPnt.Y() += aFTSiz.Height() + a3Siz.Height();
	INITCONTROL( pDriveList, ListBox, WB_DROPDOWN,
				 aPnt, Size( aEDSiz.Width(), nLbH2 ), aEmptyStr, HID_FILEDLG_DRIVES );
	nH = aPnt.Y() + aEDSiz.Height() + a6Siz.Height();
#else
	long nNewH = nLbH1 + 3 * a3Siz.Height() +
				 aFTSiz.Height() + aEDSiz.Height();
	INITCONTROL( pDirList, KbdListBox, WB_AUTOHSCROLL | WB_BORDER,
				 aPnt, Size( aEDSiz.Width(), nNewH ), aEmptyStr, HID_FILEDLG_DIRS );
	nH = aPnt.Y() + nNewH + a6Siz.Height();
	pDriveTitle = NULL;
	pDriveList = NULL;
#endif

    long nExtraWidth = pDlg->GetTextWidth( String( RTL_CONSTASCII_USTRINGPARAM( "(W)" ) ) )+10;
    String aOkStr = Button::GetStandardText( BUTTON_OK );
    long nTextWidth = pDlg->GetTextWidth( aOkStr )+nExtraWidth;
    if( nTextWidth > aBtnSiz.Width() )
        aBtnSiz.Width() = nTextWidth;

    String aCancelStr = Button::GetStandardText( BUTTON_CANCEL );
    nTextWidth = pDlg->GetTextWidth( aCancelStr )+nExtraWidth;
    if( nTextWidth > aBtnSiz.Width() )
        aBtnSiz.Width() = nTextWidth;

    String aNewDirStr( SvtResId( STR_FILEDLG_NEWDIR ) );
    nTextWidth = pDlg->GetTextWidth( aNewDirStr )+nExtraWidth;
    if( nTextWidth > aBtnSiz.Width() )
        aBtnSiz.Width() = nTextWidth;
#if defined(UNX) || defined(OS2)
    String aHomeDirStr( SvtResId( STR_FILEDLG_HOME ) );
    nTextWidth = pDlg->GetTextWidth( aHomeDirStr )+nExtraWidth;
    if( nTextWidth > aBtnSiz.Width() )
        aBtnSiz.Width() = nTextWidth;
#endif

	aPnt.X() = 2 * a6Siz.Width() + aEDSiz.Width();
	aPnt.Y() = a6Siz.Height();
	INITCONTROL( pOkBtn, PushButton, WB_DEFBUTTON,
				 aPnt, aBtnSiz, aOkStr, "" );
	aPnt.Y() += aBtnSiz.Height() + a3Siz.Height();
	INITCONTROL( pCancelBtn, CancelButton, 0,
				 aPnt, aBtnSiz, aCancelStr, "" );
	aPnt.Y() += aBtnSiz.Height() + a3Siz.Height();
	INITCONTROL( pNewDirBtn, PushButton, WB_DEFBUTTON,
				 aPnt, aBtnSiz, aNewDirStr, HID_FILEDLG_NEWDIR );
#if defined(UNX) || defined(OS2)
	aPnt.Y() += aBtnSiz.Height() + a3Siz.Height();
	INITCONTROL( pHomeBtn, PushButton, WB_DEFBUTTON,
				 aPnt, aBtnSiz, aHomeDirStr, HID_FILEDLG_HOME );
#else
	pHomeBtn = NULL;
#endif

	pDirPath = 0;
	pLoadBtn = 0;
	// Dialogbreite == OKBtn-Position + OKBtn-Breite + Rand
	long nW = aPnt.X() + aBtnSiz.Width() + a6Siz.Width();

	pDlg->SetOutputSizePixel( Size( nW, nH ) );  // Groesse ggf. auch Resource wird geplaettet?

	if (pDirList)
		pDirList->SetDoubleClickHdl(LINK( this, ImpPathDialog, DblClickHdl) );

	if (pDirList)
		pDirList->SetSelectHdl( LINK( this, ImpPathDialog, SelectHdl ) );

	if (pDriveList)
		pDriveList->SetSelectHdl( LINK( this, ImpPathDialog, SelectHdl ) );

	if (pOkBtn)
		pOkBtn->SetClickHdl( LINK( this, ImpPathDialog, ClickHdl) );

	if (pCancelBtn)
		pCancelBtn->SetClickHdl( LINK( this, ImpPathDialog, ClickHdl) );

	if (pHomeBtn)
		pHomeBtn->SetClickHdl( LINK( this, ImpPathDialog, ClickHdl) );

	if (pNewDirBtn)
		pNewDirBtn->SetClickHdl( LINK( this, ImpPathDialog, ClickHdl) );

	nOwnChilds = pDlg->GetChildCount();
}



IMPL_LINK( ImpPathDialog, SelectHdl, ListBox *, p )
{
	if( p == pDriveList )
	{
		UniString aDrive( pDriveList->GetSelectEntry(), 0, 2);
		aDrive += '\\';
		SetPath( aDrive );
	}
	else
	if( p == pDirList )
	{
		// isolate the pure name of the entry
		// removing trailing stuff and leading spaces
		UniString aEntry( pDirList->GetSelectEntry() );

		aEntry.EraseLeadingChars( ' ' );
		sal_uInt16 nPos = aEntry.Search( '/' );
		aEntry.Erase( nPos );

		// build the absolute path to the selected item
		DirEntry aNewPath;
		aNewPath.ToAbs();

		sal_uInt16 nCurPos = pDirList->GetSelectEntryPos();

		// Wird nach oben gewechselt
		if( nCurPos < nDirCount )
			aNewPath = aNewPath[nDirCount-nCurPos-1];
		else
			aNewPath += aEntry;

		pEdit->SetText( aNewPath.GetFull() );
	}

	return 0;
}


IMPL_LINK( ImpPathDialog, ClickHdl, Button*, pBtn )
{
	if ( pBtn == pOkBtn || pBtn == pLoadBtn )
	{
		DirEntry aFile( pEdit->GetText() );

		// Existiert File / File ueberschreiben
		if( IsFileOk( aFile ) )
		{
			// Ja, dann kompletten Pfad mit Filenamen merken und Dialog beenden
			aPath = aFile;
			aPath.ToAbs();
			GetPathDialog()->EndDialog( sal_True );
		}
		else
		{
			DirEntry aCheck( aPath );
			aCheck += aFile;
			if( aCheck.Exists() )
			{
				aCheck.ToAbs();
				SetPath( aCheck.GetFull() );
				pEdit->SetSelection( Selection( 0x7FFFFFFF, 0x7FFFFFFF ) );
			}
		}
	}
	else
	if ( pBtn == pCancelBtn )
	{
		GetPathDialog()->EndDialog( sal_False );
	}
	else
	if ( pBtn == pHomeBtn )
	{
		::rtl::OUString aHomeDir;
		vos:: OSecurity  aSecurity;
		if ( aSecurity.getHomeDir( aHomeDir ) )
		{
			DirEntry aFile ( aHomeDir );
			if ( IsFileOk( aFile ) )
			{
				aFile.ToAbs();
				SetPath( aFile.GetFull() );
			}
		}
	}
	else
	if ( pBtn == pNewDirBtn )
	{
		DirEntry aFile( pEdit->GetText() );
		if( ! aFile.Exists() && ! FileStat( aFile ).IsKind( FSYS_KIND_WILD ) )
			aFile.MakeDir();

		if( IsFileOk ( aFile ) )
		{
			aFile.ToAbs();
			SetPath( aFile.GetFull() );
		}
	}

	return 0;
}


IMPL_LINK( ImpPathDialog, DblClickHdl, ListBox*, pBox )
{
	// isolate the pure name of the entry
	// removing trailing stuff and leading spaces
	UniString aEntry( pBox->GetSelectEntry() );

	aEntry.EraseLeadingChars( ' ' );
	sal_uInt16 nPos = aEntry.Search( '/' );
	aEntry.Erase( nPos );

	// build the absolute path to the selected item
	DirEntry aNewPath;
	aNewPath.ToAbs();
	if( pBox == pDirList )
	{
		sal_uInt16 nCurPos = pDirList->GetSelectEntryPos();

		// Wenn es schon das aktuelle ist, dann mache nichts
		if( nCurPos == nDirCount-1 )
			return 0;

		// Wird nach oben gewechselt
		if( nCurPos < nDirCount )
			aNewPath = aNewPath[nDirCount-nCurPos-1];
		else
			aNewPath += aEntry;
	}
	else
		aNewPath += aEntry;

	pSvPathDialog->EnterWait();

	if( FileStat( aNewPath ).GetKind() & FSYS_KIND_DIR )
	{
		// Neuen Pfad setzen und Listboxen updaten
		aPath = aNewPath;
		if( !aPath.SetCWD( sal_True ) )
		{
			ErrorBox aBox( GetPathDialog(),
						   WB_OK_CANCEL | WB_DEF_OK,
						   UniString( SvtResId( STR_FILEDLG_CANTCHDIR ) ) );
			if( aBox.Execute() == RET_CANCEL )
				GetPathDialog()->EndDialog( sal_False );
		}
		UpdateEntries( sal_True );
	}

	pSvPathDialog->LeaveWait();
	return 0;
}

void ImpPathDialog::UpdateEntries( const sal_Bool )
{
	UniString aTabString;
	DirEntry aTmpPath;
	aTmpPath.ToAbs();

	nDirCount = aTmpPath.Level();

	pDirList->SetUpdateMode( sal_False );
	pDirList->Clear();

	for( sal_uInt16 i = nDirCount; i > 0; i-- )
	{
		UniString aName( aTabString );
		aName += aTmpPath[i-1].GetName();
		pDirList->InsertEntry( aName );
		aTabString.AppendAscii( "  ", 2 );
	}

	// scan the directory
	DirEntry aCurrent;
	aCurrent.ToAbs();

	Dir aDir( aCurrent, FSYS_KIND_DIR|FSYS_KIND_FILE );

	sal_uInt16 nEntries = aDir.Count();
	if( nEntries )
	{
		UniStringList aSortDirList;
		for ( sal_uInt16 n = 0; n < nEntries; n++ )
		{
			DirEntry& rEntry = aDir[n];
			UniString aName( rEntry.GetName() );
			if( aName.Len() && ( aName.GetChar(0) != '.' ) && rEntry.Exists() )
			{
				if( FileStat( rEntry ).GetKind() & FSYS_KIND_DIR )
				{
					sal_uLong l = 0;
                    if( xCollator.is() )
                    {
                        for( l = 0; l < aSortDirList.Count(); l++ )
                            if( xCollator->compareString( *aSortDirList.GetObject(l), aName ) > 0 )
                                break;
                    }
					aSortDirList.Insert( new UniString( aName ), l );
				}
			}
		}

		for( sal_uLong l = 0; l < aSortDirList.Count(); l++ )
		{
			UniString aEntryStr( aTabString );
			aEntryStr += *aSortDirList.GetObject(l);
			pDirList->InsertEntry( aEntryStr );
			delete aSortDirList.GetObject(l);
		}
	}

	UpdateDirs( aTmpPath );
}

void ImpPathDialog::UpdateDirs( const DirEntry& rTmpPath )
{
	pDirList->SelectEntryPos( nDirCount-1 );
	pDirList->SetTopEntry( nDirCount > 1
						   ? nDirCount - 2
						   : nDirCount - 1 );
	pDirList->SetUpdateMode( sal_True );
	pDirList->Invalidate();
	pDirList->Update();

	UniString aDirName = rTmpPath.GetFull();
	if( pDirPath )
		pDirPath->SetText( aDirName );
	else
		pEdit->SetText( aDirName );
}

sal_Bool ImpPathDialog::IsFileOk( const DirEntry& rDirEntry )
{
	if( FileStat( rDirEntry ).GetKind() & (FSYS_KIND_WILD | FSYS_KIND_DEV) )
		return sal_False;
	else
	{
		// Datei vorhanden ?
		if( ! rDirEntry.Exists() )
		{
			UniString aQueryTxt( SvtResId( STR_FILEDLG_ASKNEWDIR ) );
			aQueryTxt.SearchAndReplaceAscii( "%s", rDirEntry.GetFull() );
			QueryBox aQuery( GetPathDialog(),
							 WB_YES_NO | WB_DEF_YES,
							 aQueryTxt	);
			if( aQuery.Execute() == RET_YES )
				rDirEntry.MakeDir();
			else
				return sal_False;
		}
		if( !FileStat( rDirEntry ).IsKind( FSYS_KIND_DIR ) )
		{
			UniString aBoxText( SvtResId( STR_FILEDLG_CANTOPENDIR ) );
			aBoxText.AppendAscii( "\n[" );
			aBoxText += rDirEntry.GetFull();
			aBoxText.AppendAscii( "]" );
			InfoBox aBox( GetPathDialog(), aBoxText );
			aBox.Execute();
			return sal_False;
		}
	}
	return GetPathDialog()->OK() != 0;
}


void ImpPathDialog::PreExecute()
{
	// Neues Verzeichnis setzen und Listboxen updaten
	aPath.SetCWD( sal_True );
	UpdateEntries( sal_True );

	// Zusaetzliche Buttons anordnen
	Point	aPos;
	Size	aSize;
	long	nDY;
	if( pLoadBtn )
	{
		aPos  = pLoadBtn->GetPosPixel();
		aSize = pLoadBtn->GetSizePixel();
		nDY   = pLoadBtn->GetSizePixel().Height() * 2;
	}
	else
	{
		aPos  = pCancelBtn->GetPosPixel();
		aSize = pCancelBtn->GetSizePixel();
		nDY   = pCancelBtn->GetPosPixel().Y() - pOkBtn->GetPosPixel().Y();
	}

	// Standard-Controls anpassen
	long nMaxWidth = 0;

	// Maximale Breite ermitteln
	sal_uInt16 nChilds = GetPathDialog()->GetChildCount();
	sal_uInt16 n;
	for ( n = nOwnChilds; n < nChilds; n++ )
	{
		Window* pChild = GetPathDialog()->GetChild( n );
		pChild = pChild->GetWindow( WINDOW_CLIENT );
		if( pChild->GetType() != WINDOW_WINDOW )
		{
			long nWidth = pChild->GetTextWidth( pChild->GetText() ) + 12;
			if( nMaxWidth < nWidth )
				nMaxWidth = nWidth;
			nWidth = pChild->GetSizePixel().Width();
			if( nMaxWidth < nWidth )
				nMaxWidth = nWidth;
		}
	}

	if( nMaxWidth > aSize.Width() )
	{
		Size aDlgSize = GetPathDialog()->GetOutputSizePixel();
		GetPathDialog()->SetOutputSizePixel( Size( aDlgSize.Width()+nMaxWidth-aSize.Width(), aDlgSize.Height() ) );
		aSize.Width() = nMaxWidth;

		if( pOkBtn )
			pOkBtn->SetSizePixel( aSize );
		if( pCancelBtn )
			pCancelBtn->SetSizePixel( aSize );
		if( pLoadBtn )
			pLoadBtn->SetSizePixel( aSize );
	}
	else
		nMaxWidth = aSize.Width();

	for ( n = nOwnChilds; n < nChilds; n++ )
	{
		Window* pChild = GetPathDialog()->GetChild( n );
		pChild = pChild->GetWindow( WINDOW_CLIENT );
		if( pChild->GetType() != WINDOW_WINDOW )
		{
			aPos.Y() += nDY;
			pChild->SetPosSizePixel( aPos, aSize );
		}
		else
		{
			Size aDlgSize = GetPathDialog()->GetOutputSizePixel();
			long nExtra = Min( aDlgSize.Height(), (long)160);
			GetPathDialog()->SetOutputSizePixel( Size( aDlgSize.Width()+nExtra, aDlgSize.Height() ) );
			Size aSz( nExtra, nExtra );
			aSz.Width() -= 8;
			aSz.Height() -= 8;
			Point aCtrlPos( aDlgSize.Width() + 2, (aDlgSize.Height()-aSz.Height())/2 );
			pChild->SetPosSizePixel( aCtrlPos, aSz );
		}
	}

	// Laufwerke-LB fuellen
	if( pDriveList )
	{
		DirEntry aTmpDirEntry;
		Dir aDir( aTmpDirEntry, FSYS_KIND_BLOCK );

		sal_uInt16 nCount = aDir.Count(), i;
		for( i = 0; i < nCount; ++i )
		{
			DirEntry& rEntry = aDir[i];
			UniString aStr	  = rEntry.GetFull( FSYS_STYLE_HOST, sal_False );

			UniString aVolume = rEntry.GetVolume() ;
			aStr.ToUpperAscii();
			if ( aVolume.Len() )
			{
				aStr += ' ';
				aStr += aVolume;
			}
			pDriveList->InsertEntry( aStr );

		}
		UniString aPathStr = aPath.GetFull();

		for ( i = 0; i < pDriveList->GetEntryCount(); ++i )
		{
			UniString aEntry = pDriveList->GetEntry(i);
			xub_StrLen nLen   = aEntry.Len();
			nLen = nLen > 2 ? 2 : nLen;
			if ( aEntry.CompareIgnoreCaseToAscii( aPathStr, nLen ) == COMPARE_EQUAL )
			{
				pDriveList->SelectEntryPos(i);
				break;
			}
		}
	}
}

void ImpPathDialog::PostExecute()
{
}

void ImpPathDialog::SetPath( UniString const & rPath )
{
	aPath = DirEntry( rPath );

	pSvPathDialog->EnterWait();

	DirEntry aFile( rPath );
	// Falls der Pfad eine Wildcard oder einen Filenamen enthaelt
	// -> abschneiden und merken
	if( FileStat( aFile ).GetKind() & (FSYS_KIND_FILE | FSYS_KIND_WILD) || !aFile.Exists() )
		aFile.CutName();

	// Neue Maske und neues Verzeichnis setzen, und Listboxen updaten
	pEdit->SetText( rPath );
	aFile.SetCWD( sal_True );
	UpdateEntries( sal_True );

	pSvPathDialog->LeaveWait();
}

void ImpPathDialog::SetPath( Edit const & rEdit )
{
	UniString aPresetText = rEdit.GetText();
	if( aPresetText.Len() )
		SetPath( aPresetText );
}


UniString ImpPathDialog::GetPath() const
{
	DirEntry aFile( pEdit->GetText() );
	aFile.ToAbs();
	return aFile.GetFull();
}


ImpFileDialog::ImpFileDialog( PathDialog* pDlg, WinBits nWinBits, RESOURCE_TYPE nType ) :
  ImpPathDialog( pDlg, nType, sal_False )
{
	bOpen = (nWinBits & WB_SAVEAS) == 0;

	SvtResId aSvtResId = bOpen ? STR_FILEDLG_OPEN : STR_FILEDLG_SAVE;

	// Titel setzen
	GetFileDialog()->SetText( UniString( aSvtResId ) );
	nDirCount = 0;

	// initialize Controls if not used as a base class
	if ( nType == WINDOW_FILEDIALOG )
		InitControls();

	pDlg->SetHelpId( HID_FILEDLG_OPENDLG );

}

ImpFileDialog::~ImpFileDialog()
{
	ImpFilterItem* pItem = aFilterList.First();
	while( pItem )
	{
		delete pItem;
		pItem = aFilterList.Next();
	}

	delete pFileTitel;
	if (pFileList && ( pFileList != pDirList ) )
		delete pFileList;

	delete pTypeTitel;
	delete pTypeList;
}

void ImpFileDialog::InitControls()
{
	UniString aEmptyStr;

	const int nW = 160;
	const int nH = 48; // Um den Dialog in eine akzeptable Form zu bringen

	INITCONTROL( pFileTitel, FixedText, 0,
		Point(10, 12), Size(nW, 18), UniString( SvtResId( STR_FILEDLG_FILE ) ), HID_FILEDLG_FILE );
	INITCONTROL( pEdit, Edit, WB_BORDER,
		Point(10, 31), Size(nW, 20), aEmptyStr, HID_FILEDLG_EDIT ); // aMask()
	INITCONTROL( pFileList, ListBox, WB_SORT | WB_AUTOHSCROLL | WB_BORDER,
		Point(10, 58), Size(nW, 180-nH), aEmptyStr, HID_FILEDLG_FILES );

	INITCONTROL( pDirTitel, FixedText, 0,
		Point(nW+20, 12), Size(nW, 18), UniString( SvtResId( STR_FILEDLG_DIR ) ), HID_FILEDLG_DIR );
	INITCONTROL( pDirPath, FixedInfo, WB_PATHELLIPSIS,
		Point(nW+20, 33), Size(nW, 20), aPath.GetFull(), HID_FILEDLG_PATH );
	INITCONTROL( pDirList, KbdListBox, WB_AUTOHSCROLL | WB_BORDER,
		Point(nW+20, 58), Size(nW, 180-nH ), aEmptyStr, HID_FILEDLG_DIRS );

	INITCONTROL( pTypeTitel, FixedText, 0,
		Point(10, 246-nH), Size(nW, 18), UniString( SvtResId( STR_FILEDLG_TYPE ) ), HID_FILEDLG_TYPE );

#ifndef UNX
	INITCONTROL( pTypeList, ListBox, WB_DROPDOWN,
		Point(10, 265-nH ), Size(nW, 100 ), aEmptyStr, HID_FILEDLG_TYPES );

	INITCONTROL( pDriveTitle, FixedText, 0,
		Point(nW+20, 246-nH), Size(nW, 18), UniString( SvtResId( STR_FILEDLG_DRIVES ) ), HID_FILEDLG_DRIVE );
	INITCONTROL( pDriveList, ListBox, WB_DROPDOWN,
		Point(nW+20, 265-nH ), Size(nW, 100 ), aEmptyStr, HID_FILEDLG_DRIVES );
	pNewDirBtn = NULL;
	pHomeBtn   = NULL;
#else
	INITCONTROL( pTypeList, ListBox, WB_DROPDOWN,
		Point(10, 265-nH ), Size(2*nW+20, 100 ), aEmptyStr, HID_FILEDLG_TYPES );

	pDriveTitle = NULL;
	pDriveList = NULL;
	pNewDirBtn = NULL;
	pHomeBtn   = NULL;
#endif

	const long nButtonStartX = 2*nW+20+15;
	INITCONTROL( pOkBtn, PushButton, WB_DEFBUTTON,
		Point(nButtonStartX, 10), Size(STD_BTN_WIDTH, STD_BTN_HEIGHT),
		Button::GetStandardText( BUTTON_OK ), "" );
	INITCONTROL( pCancelBtn, CancelButton, 0,
		Point(nButtonStartX, 45 ), Size(STD_BTN_WIDTH, STD_BTN_HEIGHT),
		Button::GetStandardText( BUTTON_CANCEL ), "" );

	pLoadBtn = 0;

	GetFileDialog()->SetOutputSizePixel( Size(nButtonStartX+STD_BTN_WIDTH+10, 298-nH) );

	nOwnChilds = GetPathDialog()->GetChildCount();

	// Handler setzen
	if (pDriveList)
		pDriveList->SetSelectHdl( LINK( this, ImpFileDialog, SelectHdl ) );

	if (pDirList)
		pDirList->SetDoubleClickHdl(LINK( this, ImpFileDialog, DblClickHdl) );

	if (pOkBtn)
		pOkBtn->SetClickHdl( LINK( this, ImpFileDialog, ClickHdl) );

	if (pCancelBtn)
		pCancelBtn->SetClickHdl( LINK( this, ImpFileDialog, ClickHdl) );

	if( pFileList )
	{
		pFileList->SetSelectHdl( LINK( this, ImpFileDialog, SelectHdl ) );
		pFileList->SetDoubleClickHdl( LINK( this, ImpFileDialog, DblClickHdl ) );
	}

	if( pTypeList )
		pTypeList->SetSelectHdl( LINK( this, ImpFileDialog, DblClickHdl ) );
}

IMPL_LINK( ImpFileDialog, SelectHdl, ListBox *, p )
{
	if( p == pDriveList )
	{
		UniString aDrive ( pDriveList->GetSelectEntry(), 0, 2);
		aDrive += '\\';
		SetPath( aDrive );
	}
	else if (p == pFileList)
	{
		// Ausgewaehltes File in das Edit stellen
		pEdit->SetText( pFileList->GetSelectEntry() );
		GetFileDialog()->FileSelect();
	}
	return 0;
}


IMPL_LINK( ImpFileDialog, DblClickHdl, ListBox *, pBox )
{
  // isolate the pure name of the entry
  // removing trailing stuff and leading spaces
	UniString aEntry( pBox->GetSelectEntry() );

	aEntry.EraseLeadingChars( ' ' );
	sal_uInt16 nPos = aEntry.Search( '/' );
	aEntry.Erase( nPos );

	// build the absolute path to the selected item
	DirEntry aNewPath;
	aNewPath.ToAbs();

	if( ( pDirList != pFileList ) && ( pBox == pDirList ) )
	{
		// SVLOOK
		sal_uInt16 nCurPos = pDirList->GetSelectEntryPos();

		// Wenn es schon das aktuelle ist, dann mache nichts
		if( nCurPos == nDirCount-1 )
			return 0;

		// Wird nach oben gewechselt
		if( nCurPos < nDirCount )
			aNewPath = aNewPath[nDirCount-nCurPos-1];
		else
			aNewPath += aEntry;
	}
	else
	{
		// non-SVLOOK
		if( aEntry == UniString( SvtResId( STR_FILEDLG_GOUP ) ) )
			aEntry.AssignAscii( ".." );
		aNewPath += aEntry;
	}

	if( pBox == pFileList )
	{
		DirEntry aFile( aEntry );

		// Abfrage, ob File ueberschrieben werden soll...
		if( !FileStat(aFile).IsKind(FSYS_KIND_DIR) && IsFileOk( aFile ) )
		{
			// dann kompletten Pfad mit Filenamen merken und Dialog beenden
			aPath = aNewPath;
			GetFileDialog()->EndDialog( sal_True );
		}
	}

	GetFileDialog()->EnterWait();

	UniString aFull = aNewPath.GetFull();

	if( ( ( pBox == pDirList ) && ( pDirList != pFileList ) ) ||
		( ( pDirList == pFileList ) && FileStat( aNewPath ).GetKind() & FSYS_KIND_DIR ) )
	{
		// Neuen Pfad setzen und Listboxen updaten
		aPath = aNewPath;
		if( !aPath.SetCWD( sal_True ) )
		{
			if( ErrorBox( GetFileDialog(), WB_OK_CANCEL|WB_DEF_OK,
								UniString( SvtResId( STR_FILEDLG_CANTCHDIR ) ) ).Execute() == RET_CANCEL )
			{
				GetFileDialog()->EndDialog( sal_False );
			}
		}
		UpdateEntries( sal_True );
		GetFileDialog()->FileSelect();
	}

	if( pBox == pTypeList )
	{
		// Neue Maske setzen, und Listboxen updaten
		sal_uInt16 nCurPos = pTypeList->GetSelectEntryPos();
		if( nCurPos+1 > (sal_uInt16)aFilterList.Count() )
			aMask = UniString::CreateFromAscii( ALLFILES );
		else
		{
			UniString aFilterListMask = aFilterList.GetObject( nCurPos )->aMask;
//						if( aFilterListMask.Search( ';' ) == STRING_NOTFOUND ) // kein ; in der Maske
//								aMask = WildCard( aFilterListMask, '\0' );
//						else // ; muss beruecksichtigt werden
				aMask = WildCard( aFilterListMask, ';' );
		}

		pEdit->SetText( aMask() );
		UpdateEntries( sal_False );
		GetFileDialog()->FilterSelect();
	}

  GetFileDialog()->LeaveWait();

  return 0;
}

IMPL_LINK( ImpFileDialog, ClickHdl, Button*, pBtn )
{
	if( ( pBtn == pOkBtn ) || ( pBtn == pLoadBtn ) )
	{
		DirEntry aFile( pEdit->GetText() );

		// Existiert File / File ueberschreiben
		if( IsFileOk( aFile ) )
		{
			// Ja, dann kompletten Pfad mit Filenamen merken und Dialog beenden
			aPath = aFile;
			aPath.ToAbs();
			GetFileDialog()->EndDialog( sal_True );
		}
		else
		{
			GetFileDialog()->EnterWait();

			// Falls der Pfad eine Wildcard oder einen Filenamen enthaelt
			// -> abschneiden und merken
			if( FileStat( aFile ).GetKind() & (FSYS_KIND_FILE | FSYS_KIND_WILD) || !aFile.Exists() )
			{
				aMask = aFile.CutName();
			}

			// Neue Maske und neues Verzeichnis setzen, und Listboxen updaten
			pEdit->SetText( aMask() );
			aFile.SetCWD( sal_True );
			UpdateEntries( sal_True );

			GetFileDialog()->LeaveWait();
		}
	}
	else if( pBtn == pCancelBtn )
		GetFileDialog()->EndDialog( sal_False );

	return 0;
}

void ImpFileDialog::UpdateEntries( const sal_Bool bWithDirs )
{
	GetFileDialog()->EnterWait();

	UniString aTabString;
	DirEntry aTmpPath;
	aTmpPath.ToAbs();
	nDirCount = aTmpPath.Level();

	if( pFileList )
	{
	pFileList->SetUpdateMode( sal_False );
		pFileList->Clear();
	}

	if( bWithDirs && (pDirList != pFileList) )
	{
		pDirList->SetUpdateMode( sal_False );
		pDirList->Clear();

		for( sal_uInt16 i = nDirCount; i > 0; i-- )
		{
			UniString aEntryStr( aTabString );
			aEntryStr += aTmpPath[i-1].GetName();
			pDirList->InsertEntry( aEntryStr );
			aTabString.AppendAscii( "  ", 2 );
		}
	}

	// for the combined box insert a '..'
	// (this happens only if WB_3DLOOK is not set)

	if( pDirList == pFileList && nDirCount != 1 )
		pFileList->InsertEntry( UniString( SvtResId( STR_FILEDLG_GOUP ) ) );

	// scan the directory
	DirEntry aCurrent;
	aCurrent.ToAbs();
	Dir aDir( aCurrent, FSYS_KIND_DIR|FSYS_KIND_FILE );
	sal_uInt16 nEntries = aDir.Count();

	// TempMask, weil Vergleich case-sensitiv
	sal_Bool bMatchCase = sal_False; //aCurrent.IsCaseSensitive();
	UniString aWildCard( aMask.GetWildCard() );
	if ( !bMatchCase )
		aWildCard.ToLowerAscii();
	WildCard aTmpMask( aWildCard, ';' );
	if ( nEntries )
	{
		UniStringList	aSortDirList;
        for ( sal_uInt16 n = 0; n < nEntries; n++ )
		{
			DirEntry& rEntry = aDir[n];
            UniString aName( rEntry.GetName() );

			if( aName.Len() &&
				( ( ( aName.GetChar(0) != '.' ) ||
				  ( ( aName.GetChar(0) == '.' ) && ( aMask.GetWildCard() ).GetChar(0) == '.' ) )
						&& rEntry.Exists() ) )
			{
				FileStat aFileStat( rEntry );
				UniString aTmpName( aName );
				if ( !bMatchCase )
					aTmpName.ToLowerAscii();
				if( ( aFileStat.GetKind() & FSYS_KIND_FILE ) && aTmpMask.Matches( aTmpName ) )
				{
					if( pFileList )
                        pFileList->InsertEntry( aName );
				}
                else if( bWithDirs && ( aFileStat.GetKind() & FSYS_KIND_DIR ) )
				{
					if( pDirList == pFileList )
					{
						UniString aEntryStr( aName );
						aEntryStr += '/';
						pDirList->InsertEntry( aEntryStr );
					}
					else
					{
                        sal_uLong l = 0;
                        if( xCollator.is() )
                        {
                            for( l = 0; l < aSortDirList.Count(); l++ )
                                if( xCollator->compareString( *aSortDirList.GetObject(l), aName ) > 0 )
                                    break;
                        }
                        aSortDirList.Insert( new UniString( aName ), l );
			}
		}
		}
	}
	for( sal_uLong l = 0; l < aSortDirList.Count(); l++ )
		{
			UniString aEntryStr( aTabString );
			aEntryStr += *aSortDirList.GetObject(l);
		pDirList->InsertEntry( aEntryStr );
		delete aSortDirList.GetObject(l);
	}
	}

	if( bWithDirs )
		UpdateDirs( aTmpPath );

	if( pFileList )
	{
	if ( pDirList == pFileList && nDirCount > 1 )
		pFileList->SelectEntryPos( 1 );
	else
		pFileList->SetNoSelection();
	pFileList->SetUpdateMode( sal_True );
	pFileList->Invalidate();
	pFileList->Update();
	}

	if( pDriveList )
	{
		if( pDirList->GetEntryCount() > 0 )
		{
			UniString aStr( pDirList->GetEntry( 0 ) );
			aStr.Erase( 2 );
			aStr.ToLowerAscii();
			pDriveList->SelectEntry( aStr );
		}
	}

  GetFileDialog()->LeaveWait();
}

sal_Bool ImpFileDialog::IsFileOk( const DirEntry& rDirEntry )
{
	if( FileStat( rDirEntry ).GetKind() & (FSYS_KIND_WILD | FSYS_KIND_DEV) )
	return sal_False;
	if( FileStat( rDirEntry ).GetKind() & FSYS_KIND_DIR )
	{
		if( pFileList )
			return sal_False;
	}
	else if( bOpen )
	{
	// Datei vorhanden ?
	if( !FileStat( rDirEntry ).IsKind( FSYS_KIND_FILE ) )
		{
			UniString aErrorString( SvtResId( STR_FILEDLG_CANTOPENFILE ) );
			aErrorString.AppendAscii( "\n[" );
			aErrorString += rDirEntry.GetFull();
			aErrorString += ']';
			InfoBox aBox( GetFileDialog(),
						  aErrorString );
			aBox.Execute();
		return sal_False;
	}
	}
	else
	{
	// Datei vorhanden ?
		if( FileStat( ExtendFileName( rDirEntry ) ).IsKind( FSYS_KIND_FILE ) )
		{
			UniString aQueryString( SvtResId( STR_FILEDLG_OVERWRITE ) );
			aQueryString.AppendAscii( "\n[" );
			aQueryString += rDirEntry.GetFull();
			aQueryString += ']';
			QueryBox aBox( GetFileDialog(),
						   WinBits( WB_YES_NO | WB_DEF_NO ),
						   aQueryString );
			if( aBox.Execute() != RET_YES )
				return sal_False;
	}
	}
	return GetFileDialog()->OK() != 0;
}

void ImpFileDialog::SetPath( UniString const & rPath )
{
	aPath = DirEntry( rPath );
	GetFileDialog()->EnterWait();

	DirEntry aFile( rPath );

	// Falls der Pfad eine Wildcard oder einen Filenamen enthaelt
	// -> abschneiden und merken
	if( FileStat( aFile ).GetKind() & (FSYS_KIND_FILE | FSYS_KIND_WILD) 	|| !aFile.Exists() )
	{
		aMask = aFile.CutName();

		// Neue Maske und neues Verzeichnis setzen, und Listboxen updaten
		if( pDirList )
		{
			UniString aWildCard( aMask.GetWildCard() );
			pEdit->SetText( aWildCard );
		}
		else
			pEdit->SetText( rPath );
	}

	aFile.SetCWD( sal_True );

	UpdateEntries( sal_True );

	GetFileDialog()->LeaveWait();
}

void ImpFileDialog::SetPath( Edit const& rEdit )
{
	UniString aPresetText = rEdit.GetText();
	if( aPresetText.Len() )
		SetPath( aPresetText );
}


void ImpFileDialog::AddFilter( const UniString& rFilter, const UniString& rMask )
{
	aFilterList.Insert( new ImpFilterItem( rFilter, rMask ), LIST_APPEND );
	if( pTypeList )
		pTypeList->InsertEntry( rFilter, LISTBOX_APPEND );

	if( !GetCurFilter().Len() )
		SetCurFilter( rFilter );
}

void ImpFileDialog::RemoveFilter( const UniString& rFilter )
{
	ImpFilterItem* pItem = aFilterList.First();
	while( pItem && pItem->aName != rFilter )
		pItem = aFilterList.Next();

	if( pItem )
	{
		delete aFilterList.Remove();
		if( pTypeList )
			pTypeList->RemoveEntry( rFilter );
	}
}

void ImpFileDialog::RemoveAllFilter()
{
	ImpFilterItem* pItem = aFilterList.First();
	while( pItem )
	{
		delete pItem;
		pItem = aFilterList.Next();
	}
	aFilterList.Clear();

	if( pTypeList )
		pTypeList->Clear();
}

void ImpFileDialog::SetCurFilter( const UniString& rFilter )
{
	if( !pTypeList )
		return;

	ImpFilterItem* pItem = aFilterList.First();
	while( pItem && pItem->aName != rFilter )
		pItem = aFilterList.Next();

	if( pItem )
		pTypeList->SelectEntryPos( (sal_uInt16)aFilterList.GetCurPos() );
	else
		pTypeList->SetNoSelection();
}

UniString ImpFileDialog::GetCurFilter() const
{
	UniString aFilter;
	if ( pTypeList )
		aFilter = pTypeList->GetSelectEntry();
	return aFilter;
}

void ImpFileDialog::PreExecute()
{
	// ListBoxen erst unmittelbar vor Execute fuellen
	// (damit vor Execute der Pfad umgesetzt werden kann, ohne das immer die
	//	Listboxen sofort upgedatet werden)

	GetFileDialog()->EnterWait();

	// Wenn kein Filter vorhanden, dann auch keine FilterBox
	if( pTypeList && !pTypeList->GetEntryCount() )
	{
		// pTypeList->InsertEntry( "* (all files)" );
		pTypeTitel->Disable();
		pTypeList->Disable();
	}

	if( pTypeList )
	{
		sal_uInt16 nCurType = pTypeList->GetSelectEntryPos();
		if( nCurType < aFilterList.Count() )
		{
			UniString aFilterListMask = aFilterList.GetObject( nCurType )->aMask;
			if( aFilterListMask.Search( ';' ) == STRING_NOTFOUND ) // kein ; in der Maske
				aMask = WildCard( aFilterListMask, '\0' );
			else // ; in der Maske, muss in der Wildcard beruecksichtigt werden
				aMask = WildCard( aFilterListMask, ';' );
		}
		else
			aMask = UniString::CreateFromAscii( ALLFILES );
	}
	else
		aMask = UniString::CreateFromAscii( ALLFILES );

	// Neue Maske setzen
	if( pEdit->GetText().Len() == 0 )
		pEdit->SetText( aMask() );

	ImpPathDialog::PreExecute();

	GetFileDialog()->LeaveWait();
}

UniString ImpFileDialog::GetPath() const
{
	DirEntry aFile( pEdit->GetText() );
	return ExtendFileName( aFile );
}

UniString ImpFileDialog::ExtendFileName( DirEntry aEntry ) const
{
	aEntry.ToAbs();
	// das ganze Theater hier ohnehin nur machen, wenn Dateiname
	// ohne Extension angegeben wurde
	if( !aEntry.GetExtension().Len() )
	{
		UniString aPostfix; // hier kommt die ausgesuchte Extension herein

		// ist ein Filter mit Extension gesetzt?
		sal_uInt16 nChosenFilterPos = pTypeList->GetSelectEntryPos();
		if( nChosenFilterPos != LISTBOX_ENTRY_NOTFOUND )
		{
			UniString aExtensionMask = GetFileDialog()->GetFilterType( nChosenFilterPos );
			// aExtension ist z.B. *.sdw, alles bis einschliesslich Punkt abschneiden
			UniString aExtension = aExtensionMask.Copy( aExtensionMask.Search( '.' )+1 );

			// hat der Filter ueberhaupt eine Extension
			if( aExtension.Len() )
			{
				// keine Wildcards enthalten?
				if( ( aExtension.Search( '*' ) == STRING_NOTFOUND ) &&
					( aExtension.Search( '?' ) == STRING_NOTFOUND ) )
				{
					// OK, Filter hat Extension ohne Wildcards -> verwenden
					aPostfix = aExtension;
				}
				else
				{
					// Filter hat Extension mit Wildcards (z.B. *.*) -> nicht verwenden
					aPostfix.Erase();
				}
			}
			else
			{
				// Filter hatte keine Extension (schwer vorstellbar) -> nichts anhaengen
				aPostfix.Erase();
			}
		}
		else
		{
			// kein Filter gefunden (merkw�rdig) -> Default-Extension anhaengen
			aPostfix = GetFileDialog()->GetDefaultExt();
		}

		// jetzt kann es mit dem Anhaengen losgehen
		const sal_Unicode* pExt = aPostfix.GetBuffer();
		while( *pExt == '*' || *pExt == '?' )
			pExt++;

		if( *pExt )
		{
			UniString aName = aEntry.GetName();
			if( *pExt != '.' )
				aName += '.';
			aName += pExt;
			aEntry.SetName( aName );
		}
	}
	return aEntry.GetFull();
}


void ImpSvFileDlg::CreateDialog( PathDialog* pSvDlg, WinBits nStyle, RESOURCE_TYPE nType, sal_Bool bCreate )
{
	delete pDlg;
	if ( nType == WINDOW_PATHDIALOG )
		pDlg = new ImpPathDialog( pSvDlg, nType, bCreate );
	else
		pDlg = new ImpFileDialog( pSvDlg, nStyle, nType );
}


