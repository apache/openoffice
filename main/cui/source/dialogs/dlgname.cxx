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
#include "precompiled_cui.hxx"

#include <tools/ref.hxx>
#include <tools/list.hxx>
#include <tools/shl.hxx>
#include <tools/debug.hxx>
#include <vcl/msgbox.hxx>

#include <cuires.hrc>

#include "dlgname.hxx"
#include "defdlgname.hxx" //CHINA001
#include "dlgname.hrc"
#include <dialmgr.hxx>

#define MAX_DESCRIPTION_LINES   ((long)5)

/*************************************************************************
|* Dialog zum Editieren eines Namens
\************************************************************************/

SvxNameDialog::SvxNameDialog( Window* pWindow, const String& rName, const String& rDesc ) :
	ModalDialog		( pWindow, CUI_RES( RID_SVXDLG_NAME ) ),
	aFtDescription		( this, CUI_RES( FT_DESCRIPTION ) ),
	aEdtName			( this, CUI_RES( EDT_STRING ) ),
	aBtnOK				( this, CUI_RES( BTN_OK ) ),
	aBtnCancel			( this, CUI_RES( BTN_CANCEL ) ),
	aBtnHelp			( this, CUI_RES( BTN_HELP ) )
{
	FreeResource();

	aFtDescription.SetText( rDesc );
	aEdtName.SetText( rName );
	aEdtName.SetSelection(Selection(SELECTION_MIN, SELECTION_MAX));
	ModifyHdl(&aEdtName);
	aEdtName.SetModifyHdl(LINK(this, SvxNameDialog, ModifyHdl));

	// dynamic height of the description field
	Size aSize = aFtDescription.GetSizePixel();
	long nTxtWidth = aFtDescription.GetCtrlTextWidth( rDesc );
	if ( nTxtWidth > aSize.Width() )
	{
		long nLines = Min( ( nTxtWidth / aSize.Width() + 1 ), MAX_DESCRIPTION_LINES );
		long nHeight = aSize.Height();
		aSize.Height() = nHeight * nLines;
		aFtDescription.SetSizePixel( aSize );
		Point aPnt = aEdtName.GetPosPixel();
		aPnt.Y() += ( aSize.Height() - nHeight );
		aEdtName.SetPosPixel( aPnt );
	}
}

/* -----------------------------27.02.2002 15:22------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK(SvxNameDialog, ModifyHdl, Edit*, EMPTYARG)
{
	if(aCheckNameHdl.IsSet())
		aBtnOK.Enable(aCheckNameHdl.Call(this) > 0);
	return 0;
}

// #i68101#
// Dialog for editing Object Name
// plus uniqueness-callback-linkHandler

SvxObjectNameDialog::SvxObjectNameDialog(
	Window* pWindow,
	const String& rName)
:	ModalDialog(pWindow, CUI_RES(RID_SVXDLG_OBJECT_NAME)),
	aFtName(this, CUI_RES(NTD_FT_NAME)),
	aEdtName(this, CUI_RES(NTD_EDT_NAME)),
	aFlSeparator(this, CUI_RES(FL_SEPARATOR_A)),
	aBtnHelp(this, CUI_RES(BTN_HELP)),
	aBtnOK(this, CUI_RES(BTN_OK)),
	aBtnCancel(this, CUI_RES(BTN_CANCEL))
{
	FreeResource();

	// set name
	aEdtName.SetText(rName);

	// activate name
	aEdtName.SetSelection(Selection(SELECTION_MIN, SELECTION_MAX));
	ModifyHdl(&aEdtName);
	aEdtName.SetModifyHdl(LINK(this, SvxObjectNameDialog, ModifyHdl));
}

IMPL_LINK(SvxObjectNameDialog, ModifyHdl, Edit*, EMPTYARG)
{
	if(aCheckNameHdl.IsSet())
	{
		aBtnOK.Enable(aCheckNameHdl.Call(this) > 0);
	}

	return 0;
}

// #i68101#
// Dialog for editing Object Title and Description

SvxObjectTitleDescDialog::SvxObjectTitleDescDialog(
	Window* pWindow,
	const String& rTitle,
	const String& rDescription)
:	ModalDialog(pWindow, CUI_RES(RID_SVXDLG_OBJECT_TITLE_DESC)),
	aFtTitle(this, CUI_RES(NTD_FT_TITLE)),
	aEdtTitle(this, CUI_RES(NTD_EDT_TITLE)),
	aFtDescription(this, CUI_RES(NTD_FT_DESC)),
	aEdtDescription(this, CUI_RES(NTD_EDT_DESC)),
	aFlSeparator(this, CUI_RES(FL_SEPARATOR_B)),
	aBtnHelp(this, CUI_RES(BTN_HELP)),
	aBtnOK(this, CUI_RES(BTN_OK)),
	aBtnCancel(this, CUI_RES(BTN_CANCEL))
{
	FreeResource();

	// set title & desc
	aEdtTitle.SetText(rTitle);
	aEdtDescription.SetText(rDescription);

	// activate title
	aEdtTitle.SetSelection(Selection(SELECTION_MIN, SELECTION_MAX));
}

/*************************************************************************
|* Dialog zum Abbrechen, Speichern oder Hinzufügen
\************************************************************************/

SvxMessDialog::SvxMessDialog( Window* pWindow, const String& rText, const String& rDesc, Image* pImg ) :
	ModalDialog		( pWindow, CUI_RES( RID_SVXDLG_MESSBOX ) ),
	aFtDescription		( this, CUI_RES( FT_DESCRIPTION ) ),
	aBtn1				( this, CUI_RES( BTN_1 ) ),
	aBtn2				( this, CUI_RES( BTN_2 ) ),
	aBtnCancel			( this, CUI_RES( BTN_CANCEL ) ),
	aFtImage			( this )
{
	FreeResource();

	if( pImg )
	{
		pImage = new Image( *pImg );
		aFtImage.SetImage( *pImage );
		aFtImage.SetStyle( ( aFtImage.GetStyle()/* | WB_NOTABSTOP */) & ~WB_3DLOOK );
		aFtImage.SetPosSizePixel( LogicToPixel( Point( 3, 6 ), MAP_APPFONT ),
								  aFtImage.GetImage().GetSizePixel() );
		aFtImage.Show();
	}

	SetText( rText );
	aFtDescription.SetText( rDesc );

	aBtn1.SetClickHdl( LINK( this, SvxMessDialog, Button1Hdl ) );
	aBtn2.SetClickHdl( LINK( this, SvxMessDialog, Button2Hdl ) );
}

SvxMessDialog::~SvxMessDialog()
{
	if( pImage )
		delete pImage;
}

/*************************************************************************/

IMPL_LINK_INLINE_START( SvxMessDialog, Button1Hdl, Button *, EMPTYARG )
{
	EndDialog( RET_BTN_1 );
	return 0;
}
IMPL_LINK_INLINE_END( SvxMessDialog, Button1Hdl, Button *, EMPTYARG )

/*************************************************************************/

IMPL_LINK_INLINE_START( SvxMessDialog, Button2Hdl, Button *, EMPTYARG )
{
	EndDialog( RET_BTN_2 );
	return 0;
}
IMPL_LINK_INLINE_END( SvxMessDialog, Button2Hdl, Button *, EMPTYARG )

/*************************************************************************/

void SvxMessDialog::SetButtonText( sal_uInt16 nBtnId, const String& rNewTxt )
{
	switch ( nBtnId )
	{
		case MESS_BTN_1:
			aBtn1.SetText( rNewTxt );
			break;

		case MESS_BTN_2:
			aBtn2.SetText( rNewTxt );
			break;

		default:
			DBG_ERROR( "Wrong button number!" );
	}
}

/* vim: set noet sw=4 ts=4: */
