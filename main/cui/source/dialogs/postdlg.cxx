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

// include ---------------------------------------------------------------
#include <tools/shl.hxx>
#include <tools/date.hxx>
#include <tools/time.hxx>
#include <vcl/svapp.hxx>
#include <vcl/msgbox.hxx>
#include <svl/itempool.hxx>
#include <svl/itemset.hxx>
#include <unotools/useroptions.hxx>
#include <unotools/localedatawrapper.hxx>
#include <comphelper/processfactory.hxx>
#include <svx/svxids.hrc> // SID_ATTR_...
#include <svx/dialogs.hrc> // RID_SVXDLG_POSTIT

#define _SVX_POSTDLG_CXX

#include <cuires.hrc>
#include "postdlg.hrc"
#include <svx/postattr.hxx>
#include "postdlg.hxx"
#include <dialmgr.hxx>

#include "helpid.hrc"

// static ----------------------------------------------------------------

static sal_uInt16 pRanges[] =
{
	SID_ATTR_POSTIT_AUTHOR,
	SID_ATTR_POSTIT_TEXT,
	0
};

// class SvxPostItDialog -------------------------------------------------

SvxPostItDialog::SvxPostItDialog( Window* pParent,
								  const SfxItemSet& rCoreSet,
								  sal_Bool bPrevNext,
								  sal_Bool bRedline ) :

	SfxModalDialog( pParent, CUI_RES( RID_SVXDLG_POSTIT ) ),

	aPostItFL		( this, CUI_RES( FL_POSTIT ) ),
	aLastEditLabelFT( this, CUI_RES( FT_LASTEDITLABEL ) ),
	aLastEditFT 	( this, CUI_RES( FT_LASTEDIT ) ),
	aEditFT 		( this, CUI_RES( FT_EDIT ) ),
	aEditED 		( this, CUI_RES( ED_EDIT ) ),
	aAuthorFT		( this, CUI_RES( FT_AUTHOR) ),
	aAuthorBtn		( this, CUI_RES( BTN_AUTHOR ) ),
	aOKBtn			( this, CUI_RES( BTN_POST_OK ) ),
	aCancelBtn		( this, CUI_RES( BTN_POST_CANCEL ) ),
	aHelpBtn		( this, CUI_RES( BTN_POST_HELP ) ),
	aPrevBtn		( this, CUI_RES( BTN_PREV ) ),
	aNextBtn		( this, CUI_RES( BTN_NEXT ) ),

	rSet		( rCoreSet ),
	pOutSet 	( 0 )

{
	if (bRedline)	// HelpIDs for Redlining
	{
		SetHelpId(HID_REDLINING_DLG);
		aEditED.SetHelpId(HID_REDLINING_EDIT);
		aPrevBtn.SetHelpId(HID_REDLINING_PREV);
		aNextBtn.SetHelpId(HID_REDLINING_NEXT);
	}

	aPrevBtn.SetClickHdl( LINK( this, SvxPostItDialog, PrevHdl ) );
	aNextBtn.SetClickHdl( LINK( this, SvxPostItDialog, NextHdl ) );
	aAuthorBtn.SetClickHdl( LINK( this, SvxPostItDialog, Stamp ) );
	aOKBtn.SetClickHdl( LINK( this, SvxPostItDialog, OKHdl ) );

	Font aFont( aEditED.GetFont() );
	aFont.SetWeight( WEIGHT_LIGHT );
	aEditED.SetFont( aFont );

	sal_Bool bNew = sal_True;
	sal_uInt16 nWhich			 = 0;

	if ( !bPrevNext )
	{
		aPrevBtn.Disable();
		aNextBtn.Disable();
	}

	nWhich = rSet.GetPool()->GetWhich( SID_ATTR_POSTIT_AUTHOR );
	String aAuthorStr, aDateStr, aTextStr;

	if ( rSet.GetItemState( nWhich, sal_True ) >= SFX_ITEM_AVAILABLE )
	{
		bNew = sal_False;
		const SvxPostItAuthorItem& rAuthor =
			(const SvxPostItAuthorItem&)rSet.Get( nWhich );
		aAuthorStr = rAuthor.GetValue();
	}
	else
		aAuthorStr = SvtUserOptions().GetID();

	nWhich = rSet.GetPool()->GetWhich( SID_ATTR_POSTIT_DATE );

	if ( rSet.GetItemState( nWhich, sal_True ) >= SFX_ITEM_AVAILABLE )
	{
		const SvxPostItDateItem& rDate =
			(const SvxPostItDateItem&)rSet.Get( nWhich );
		aDateStr = rDate.GetValue();
	}
	else
	{
		LocaleDataWrapper aLocaleWrapper( ::comphelper::getProcessServiceFactory(), Application::GetSettings().GetLocale() );
		aDateStr = aLocaleWrapper.getDate( Date() );
	}

	nWhich = rSet.GetPool()->GetWhich( SID_ATTR_POSTIT_TEXT );

	if ( rSet.GetItemState( nWhich, sal_True ) >= SFX_ITEM_AVAILABLE )
	{
		const SvxPostItTextItem& rText =
			(const SvxPostItTextItem&)rSet.Get( nWhich );
		aTextStr = rText.GetValue();
	}

	ShowLastAuthor(aAuthorStr, aDateStr);
	aEditED.SetText( aTextStr.ConvertLineEnd() );

	if ( !bNew )
		SetText( CUI_RESSTR( STR_NOTIZ_EDIT ) );
	else
		// neu anlegen
		SetText( CUI_RESSTR( STR_NOTIZ_INSERT ) );

	FreeResource();

	aEditED.SetAccessibleRelationLabeledBy(&aEditFT);
	aEditED.SetAccessibleRelationMemberOf(&aPostItFL);
	aAuthorBtn.SetAccessibleRelationMemberOf(&aPostItFL);
}

// -----------------------------------------------------------------------

SvxPostItDialog::~SvxPostItDialog()
{
	delete pOutSet;
	pOutSet = 0;
}

// -----------------------------------------------------------------------

void SvxPostItDialog::ShowLastAuthor(const String& rAuthor, const String& rDate)
{
	String sTxt( rAuthor );
	sTxt.AppendAscii( RTL_CONSTASCII_STRINGPARAM( ", " ) );
	sTxt += rDate;
	aLastEditFT.SetText( sTxt );
}

// -----------------------------------------------------------------------

sal_uInt16* SvxPostItDialog::GetRanges()
{
	return pRanges;
}

// -----------------------------------------------------------------------

void SvxPostItDialog::EnableTravel(sal_Bool bNext, sal_Bool bPrev)
{
	aPrevBtn.Enable(bPrev);
	aNextBtn.Enable(bNext);
}

// -----------------------------------------------------------------------

IMPL_LINK_INLINE_START( SvxPostItDialog, PrevHdl, Button *, EMPTYARG )
{
	aPrevHdlLink.Call( this );
	return 0;
}
IMPL_LINK_INLINE_END( SvxPostItDialog, PrevHdl, Button *, EMPTYARG )

// -----------------------------------------------------------------------

IMPL_LINK_INLINE_START( SvxPostItDialog, NextHdl, Button *, EMPTYARG )
{
	aNextHdlLink.Call( this );
	return 0;
}
IMPL_LINK_INLINE_END( SvxPostItDialog, NextHdl, Button *, EMPTYARG )

// -----------------------------------------------------------------------

IMPL_LINK( SvxPostItDialog, Stamp, Button *, EMPTYARG )
{
	Date aDate;
	Time aTime;
	String aTmp( SvtUserOptions().GetID() );
	LocaleDataWrapper aLocaleWrapper( ::comphelper::getProcessServiceFactory(), Application::GetSettings().GetLocale() );
	String aStr( aEditED.GetText() );
	aStr.AppendAscii( RTL_CONSTASCII_STRINGPARAM( "\n---- " ) );

	if ( aTmp.Len() > 0 )
	{
		aStr += aTmp;
		aStr.AppendAscii( RTL_CONSTASCII_STRINGPARAM( ", " ) );
	}
	aStr += aLocaleWrapper.getDate(aDate);
	aStr.AppendAscii( RTL_CONSTASCII_STRINGPARAM( ", " ) );
	aStr += aLocaleWrapper.getTime(aTime, sal_False, sal_False);
	aStr.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " ----\n" ) );


	aEditED.SetText( aStr.ConvertLineEnd() );
	xub_StrLen nLen = aStr.Len();
	aEditED.GrabFocus();
	aEditED.SetSelection( Selection( nLen, nLen ) );
	return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( SvxPostItDialog, OKHdl, Button *, EMPTYARG )
{
	LocaleDataWrapper aLocaleWrapper( ::comphelper::getProcessServiceFactory(), Application::GetSettings().GetLocale() );
	pOutSet = new SfxItemSet( rSet );
	pOutSet->Put( SvxPostItAuthorItem( SvtUserOptions().GetID(),
									   rSet.GetPool()->GetWhich( SID_ATTR_POSTIT_AUTHOR ) ) );
	pOutSet->Put( SvxPostItDateItem( aLocaleWrapper.getDate( Date() ),
									 rSet.GetPool()->GetWhich( SID_ATTR_POSTIT_DATE ) ) );
	pOutSet->Put( SvxPostItTextItem( aEditED.GetText(),
									 rSet.GetPool()->GetWhich( SID_ATTR_POSTIT_TEXT ) ) );
	EndDialog( RET_OK );
	return 0;
}

/* vim: set noet sw=4 ts=4: */
