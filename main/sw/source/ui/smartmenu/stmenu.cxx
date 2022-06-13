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



// SMARTTAGS

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"
#include <stmenu.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <svl/eitem.hxx>
#include <sfx2/dispatch.hxx>

#include <SwSmartTagMgr.hxx>

#ifndef _STMENU_HRC
#include <stmenu.hrc>
#endif
#ifndef _VIEW_HXX
#include <view.hxx>
#endif
#include <breakit.hxx>

#define C2U(cChar) rtl::OUString::createFromAscii(cChar)

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

SwSmartTagPopup::SwSmartTagPopup( SwView* pSwView,
								  Sequence< rtl::OUString >& rSmartTagTypes,
								  Sequence< Reference< container::XStringKeyMap > >& rStringKeyMaps,
								  Reference< text::XTextRange > xTextRange ) :
	PopupMenu( SW_RES(MN_SMARTTAG_POPUP) ),
	mpSwView ( pSwView ),
	mxTextRange( xTextRange )
{
	//CreateAutoMnemonics();

	Reference <frame::XController> xController = mpSwView->GetController();
	const lang::Locale aLocale( SW_BREAKITER()->GetLocale( (LanguageType)GetAppLanguage() ) );

	sal_uInt16 nMenuPos = 0;
	sal_uInt16 nSubMenuPos = 0;
	sal_uInt16 nMenuId = 1;
	sal_uInt16 nSubMenuId = MN_ST_INSERT_START;

	const rtl::OUString aRangeText = mxTextRange->getString();

	SmartTagMgr& rSmartTagMgr = SwSmartTagMgr::Get();
	const rtl::OUString aApplicationName( rSmartTagMgr.GetApplicationName() );

	Sequence < Sequence< Reference< smarttags::XSmartTagAction > > > aActionComponentsSequence;
	Sequence < Sequence< sal_Int32 > > aActionIndicesSequence;

	rSmartTagMgr.GetActionSequences( rSmartTagTypes,
									 aActionComponentsSequence,
									 aActionIndicesSequence );

	InsertSeparator(0);

	for ( sal_uInt16 j = 0; j < aActionComponentsSequence.getLength(); ++j )
	{
		Reference< container::XStringKeyMap > xSmartTagProperties = rStringKeyMaps[j];

		// Get all actions references associated with the current smart tag type:
		const Sequence< Reference< smarttags::XSmartTagAction > >& rActionComponents = aActionComponentsSequence[j];
		const Sequence< sal_Int32 >& rActionIndices = aActionIndicesSequence[j];

		if ( 0 == rActionComponents.getLength() || 0 == rActionIndices.getLength() )
			continue;

		// Ask first entry for the smart tag type caption:
		Reference< smarttags::XSmartTagAction > xAction = rActionComponents[0];

		if ( !xAction.is() )
			continue;

		const sal_Int32 nSmartTagIndex = rActionIndices[0];
		const rtl::OUString aSmartTagType = xAction->getSmartTagName( nSmartTagIndex );
		const rtl::OUString aSmartTagCaption = xAction->getSmartTagCaption( nSmartTagIndex, aLocale );

		// no sub-menus if there's only one smart tag type listed:
		PopupMenu* pSbMenu = this;
		if ( 1 < aActionComponentsSequence.getLength() )
		{
			InsertItem( nMenuId, aSmartTagCaption, 0, nMenuPos++);
			pSbMenu = new PopupMenu;
			SetPopupMenu( nMenuId++, pSbMenu );
		}

		// sub-menu starts with smart tag caption and separator
		const rtl::OUString aSmartTagCaption2 = aSmartTagCaption + C2U(": ") + aRangeText;
		nSubMenuPos = 0;
		pSbMenu->InsertItem( nMenuId++, aSmartTagCaption2, MIB_NOSELECT, nSubMenuPos++ );
		pSbMenu->InsertSeparator( nSubMenuPos++ );

		// Add subitem for every action reference for the current smart tag type:
		for ( sal_uInt16 i = 0; i < rActionComponents.getLength(); ++i )
		{
			xAction = rActionComponents[i];

			for ( sal_Int32 k = 0; k < xAction->getActionCount( aSmartTagType, xController, xSmartTagProperties ); ++k )
			{
				const sal_uInt32 nActionID = xAction->getActionID( aSmartTagType, k, xController );
				rtl::OUString aActionCaption = xAction->getActionCaptionFromID( nActionID,
																				aApplicationName,
																				aLocale,
																				xSmartTagProperties,
																				aRangeText,
																				rtl::OUString(),
																				xController,
																				mxTextRange );

				pSbMenu->InsertItem( nSubMenuId++, aActionCaption, 0, nSubMenuPos++ );
				InvokeAction aEntry( xAction, xSmartTagProperties, nActionID );
				maInvokeActions.push_back( aEntry );
			}
		}
	}
}

/** Function: Execute

	executes actions by calling the invoke function of the appropriate
	smarttag library.

*/
sal_uInt16 SwSmartTagPopup::Execute( const Rectangle& rWordPos, Window* pWin )
{
	sal_uInt16 nId = PopupMenu::Execute(pWin, pWin->LogicToPixel(rWordPos));

	if ( nId == MN_SMARTTAG_OPTIONS )
	{
		SfxBoolItem aBool(SID_OPEN_SMARTTAGOPTIONS, sal_True);
		mpSwView->GetViewFrame()->GetDispatcher()->Execute( SID_AUTO_CORRECT_DLG, SFX_CALLMODE_ASYNCHRON, &aBool, 0L );
	}

	if ( nId < MN_ST_INSERT_START) return nId;
	nId -= MN_ST_INSERT_START;

	// compute smarttag lib index and action index
	if ( nId < maInvokeActions.size() )
	{
		Reference< smarttags::XSmartTagAction > xSmartTagAction = maInvokeActions[ nId ].mxAction;

		// execute action
		if ( xSmartTagAction.is() )
		{
			SmartTagMgr& rSmartTagMgr = SwSmartTagMgr::Get();

			xSmartTagAction->invokeAction( maInvokeActions[ nId ].mnActionID,
										   rSmartTagMgr.GetApplicationName(),
										   mpSwView->GetController(),
										   mxTextRange,
										   maInvokeActions[ nId ].mxSmartTagProperties,
										   mxTextRange->getString(),
										   rtl::OUString(),
										   SW_BREAKITER()->GetLocale( (LanguageType)GetAppLanguage() ) );
		}
	}

	return nId;
}
