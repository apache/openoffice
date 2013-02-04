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



#ifndef _RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif

#include <sot/formats.hxx>
#include <bf_svtools/itempool.hxx>
#include <bf_svtools/style.hxx>
#include <bf_svtools/sfxecode.hxx>
#include <bf_svtools/saveopt.hxx>
#include <bf_sfx2/docfile.hxx>
#include <bf_sfx2/progress.hxx>

#include "bf_sd/docshell.hxx"


#ifndef _XDEF_HXX
#include <bf_svx/xdef.hxx>
#endif

#ifndef _SVX_XFLGRIT_HXX
#include <bf_svx/xflgrit.hxx>
#endif
#ifndef _SVX_XBTMPIT_HXX
#include <bf_svx/xbtmpit.hxx>
#endif
#ifndef _SVX_XLNDSIT_HXX
#include <bf_svx/xlndsit.hxx>
#endif
#ifndef _SVX_XLNSTIT_HXX
#include <bf_svx/xlnstit.hxx>
#endif
#ifndef _SVX_XFLFTRIT_HXX
#include <bf_svx/xflftrit.hxx>
#endif
#ifndef _SVX_XFLHTIT_HXX
#include <bf_svx/xflhtit.hxx>
#endif
#ifndef _SVX_XLNEDIT_HXX
#include <bf_svx/xlnedit.hxx>
#endif
#ifndef _SVDOTEXT_HXX
#include <bf_svx/svdotext.hxx>
#endif
#ifndef _EEITEM_HXX
#include <bf_svx/eeitem.hxx>
#endif
#ifndef _SVDITER_HXX
#include <bf_svx/svditer.hxx>
#endif

#include "sdoutl.hxx"
#include "sdpage.hxx"
#include "drawdoc.hxx"
#include "strmname.h"
#include "sdbinfilter.hxx"
namespace binfilter {

// -----------
// - Defines -
// -----------

#define POOL_BUFFER_SIZE		32768
#define BASIC_BUFFER_SIZE		8192
#define DOCUMENT_BUFFER_SIZE	32768

// --------------
// - Namespaces -
// --------------

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::frame;

// ---------------
// - SdBINFilter -
// ---------------

SdBINFilter::SdBINFilter( SfxMedium& rMedium, SdDrawDocShell& rDocShell, sal_Bool bShowProgress ) :
	SdFilter( rMedium, rDocShell, bShowProgress )
{
}

// -----------------------------------------------------------------------------

static void ImplPostLoadCheckStyleSheetPool( SfxStyleSheetPool* pStyleSheetPool, SdDrawDocument* pDoc )
{
	if( pStyleSheetPool )
	{
		SfxStyleSheetBase* pStyleSheet = pStyleSheetPool->First();
		while( pStyleSheet )
		{
			SfxItemSet& rSet = pStyleSheet->GetItemSet();

			const SfxPoolItem *pItem;

			// check fill gradient item for valid name
			if( SFX_ITEM_SET == rSet.GetItemState( XATTR_FILLGRADIENT, sal_False, &pItem ) )
			{
				XFillGradientItem* pNewItem = ((XFillGradientItem*)pItem)->checkForUniqueItem( pDoc );
				if( pNewItem != pItem )
				{
					rSet.Put( *pNewItem, XATTR_FILLGRADIENT );
					delete pNewItem;
				}
			}

			// check fill bitmap item for valid name
			if( SFX_ITEM_SET == rSet.GetItemState( XATTR_FILLBITMAP, sal_False, &pItem ) )
			{
				XFillBitmapItem* pNewItem = ((XFillBitmapItem*)pItem)->checkForUniqueItem( pDoc );
				if( pNewItem != pItem )
				{
					rSet.Put( *pNewItem, XATTR_FILLBITMAP );
					delete pNewItem;
				}
			}

			// check line dash item for valid name
			if( SFX_ITEM_SET == rSet.GetItemState( XATTR_LINEDASH, sal_False, &pItem ) )
			{
				XLineDashItem* pNewItem = ((XLineDashItem*)pItem)->checkForUniqueItem( pDoc );
				if( pNewItem != pItem )
				{
					rSet.Put( *pNewItem, XATTR_LINEDASH );
					delete pNewItem;
				}
			}

			// check line start item for valid name
			if( SFX_ITEM_SET == rSet.GetItemState( XATTR_LINESTART, sal_False, &pItem ) )
			{
				XLineStartItem* pNewItem = ((XLineStartItem*)pItem)->checkForUniqueItem( pDoc );
				if( pNewItem != pItem )
				{
					rSet.Put( *pNewItem, XATTR_LINESTART );
					delete pNewItem;
				}
			}

			// check line end item for valid name
			if( SFX_ITEM_SET == rSet.GetItemState( XATTR_LINEEND, sal_False, &pItem ) )
			{
				XLineEndItem* pNewItem = ((XLineEndItem*)pItem)->checkForUniqueItem( pDoc );
				if( pNewItem != pItem )
				{
					rSet.Put( *pNewItem, XATTR_LINEEND );
					delete pNewItem;
				}
			}

			// check fill float transparence item for valid name
			if( SFX_ITEM_SET == rSet.GetItemState( XATTR_FILLFLOATTRANSPARENCE, sal_False, &pItem ) )
			{
				XFillFloatTransparenceItem* pNewItem = ((XFillFloatTransparenceItem*)pItem)->checkForUniqueItem( pDoc );
				if( pNewItem != pItem )
				{
					rSet.Put( *pNewItem, XATTR_FILLFLOATTRANSPARENCE );
					delete pNewItem;
				}
			}

			// check fill hatch item for valid name
			if( SFX_ITEM_SET == rSet.GetItemState( XATTR_FILLHATCH, sal_False, &pItem ) )
			{
				XFillHatchItem* pNewItem = ((XFillHatchItem*)pItem)->checkForUniqueItem( pDoc );
				if( pNewItem != pItem )
				{
					rSet.Put( *pNewItem, XATTR_FILLHATCH );
					delete pNewItem;
				}
			}
/* sample code for fixing bullet problems. Didn't need it yet
			if( SFX_ITEM_SET == rSet.GetItemState( EE_PARA_NUMBULLET, sal_False, &pItem ) )
			{
				SvxNumRule* pRule = ((SvxNumBulletItem*)rSet.GetItem( EE_PARA_NUMBULLET ))->GetNumRule();

				const USHORT nLevelCount = pRule->GetLevelCount();
				USHORT nLevel;
				BOOL bNeedsFix = FALSE;
				for( nLevel = 0; !bNeedsFix && (nLevel < nLevelCount); nLevel++ )
				{
					const SvxNumberFormat& rLevel = pRule->GetLevel(nLevel);
					bNeedsFix = (rLevel.GetNumberingType() == ::com::sun::star::style::NumberingType::CHAR_SPECIAL) &&
								(rLevel.GetBulletChar() == 0 );
				}

				if( bNeedsFix )
				{
					SvxNumRule aNewRule( pRule->GetFeatureFlags(), nLevelCount, FALSE, pRule->GetNumRuleType() );

					for( nLevel = 0; !bNeedsFix && (nLevel < nLevelCount); nLevel++ )
					{
						SvxNumberFormat aLevel(pRule->GetLevel(nLevel)) ;
						if( (aLevel.GetNumberingType() == ::com::sun::star::style::NumberingType::CHAR_SPECIAL) && (aLevel.GetBulletChar() == 0 ) )
							aLevel.SetNumberingType( ::com::sun::star::style::NumberingType::NUMBER_NONE );

						aNewRule.SetLevel(nLevel, aLevel);
					}

					rSet.Put( SvxNumBulletItem( aNewRule, EE_PARA_NUMBULLET ) );

				}
			}
*/
			pStyleSheet = pStyleSheetPool->Next();
		}
	}
}

/** this checks for the given text object if all paragraphs have the
	same stylesheet as the object itself. If not, they will be fixed
*/
static void ImplPostLoadFixTextStyles( SdDrawDocument* pDoc, SdrTextObj* pText )
{
	SfxStyleSheet* pObjectStyleSheet = pText->GetStyleSheet();
	if( pObjectStyleSheet )
	{
		sal_Bool bDataChanged = sal_False;

		SdOutliner* pOutliner = pDoc->GetInternalOutliner(TRUE);

		pOutliner->SetText( *pText->GetOutlinerParaObject() );

		const ULONG nParaCount = pOutliner->GetParagraphCount();
		ULONG nPara;
		for( nPara = 0; nPara < nParaCount; nPara++ )
		{
			SfxStyleSheet* pStyleSheet = pOutliner->GetStyleSheet( nPara );
			if( pStyleSheet && (pStyleSheet != pObjectStyleSheet) )
			{
				SfxItemSet& rStyleSet = pStyleSheet->GetItemSet();
				SfxItemSet& rObjStyleSet = pObjectStyleSheet->GetItemSet();

				SfxItemSet aSet( pOutliner->GetParaAttribs( nPara ) );

				USHORT nWhich;
				for( nWhich = EE_ITEMS_START; nWhich <= EE_CHAR_END; nWhich++)
				{
					if( aSet.GetItemState( nWhich ) == SFX_ITEM_OFF )
					{
						const SfxPoolItem* pSourceItem = rStyleSet.GetItem( nWhich );
						const SfxPoolItem* pDestItem = rObjStyleSet.GetItem( nWhich );

						if( (*pSourceItem) != (*pDestItem) )
							aSet.Put( *pSourceItem );
					}
				}
				pOutliner->SetStyleSheet( nPara, pObjectStyleSheet );
				pOutliner->SetParaAttribs( nPara, aSet );
				bDataChanged = sal_True;
			}
		}

		if( bDataChanged )
			pText->SetOutlinerParaObject( pOutliner->CreateParaObject() );

		pOutliner->Clear();
	}
}

static void ImplPostLoadFixTextStyles( SdDrawDocument* pDoc, SdrObjList* pList)
{
	SdrObjListIter aIter( *pList, IM_DEEPNOGROUPS );
	while( aIter.IsMore() )
	{
		SdrObject* pObj = aIter.Next();
		SdrTextObj* pText = PTR_CAST( SdrTextObj, pObj );
		if( pText && pText->HasText() )
		{
			SdPage* pPage = PTR_CAST(SdPage,pList);
			if( pPage && (pPage->GetPresObjKind(pObj) == PRESOBJ_NONE) )
				ImplPostLoadFixTextStyles( pDoc, pText );
		}
	}
}

/** this method checks all SdrObjects inside the document if the paragraphs have the
	same style as the shape */
static void ImplPostLoadFixTextStyles( SdDrawDocument* pDoc )
{
	USHORT nPageCount = pDoc->GetPageCount();
	USHORT nPage;
	for( nPage = 0; nPage < nPageCount; nPage++ )
	{
		ImplPostLoadFixTextStyles( pDoc, pDoc->GetPage(nPage) );
	}

	nPageCount = pDoc->GetMasterPageCount();
	for( nPage = 0; nPage < nPageCount; nPage++ )
	{
		ImplPostLoadFixTextStyles( pDoc, pDoc->GetMasterPage(nPage) );
	}
}

// -----------------------------------------------------------------------------

sal_Bool SdBINFilter::Import()
{
    RTL_LOGFILE_CONTEXT_AUTHOR ( aLog, "sd", "cl93746", "SdBINFilter::Import" );
#ifdef TIMELOG
	ByteString aFile( mrMedium.GetName(), RTL_TEXTENCODING_ASCII_US );
	RTL_LOGFILE_CONTEXT_TRACE1( aLog, "importing %s", aFile.GetBuffer() );
#endif

	SvStorage*	pStore = mrMedium.GetStorage();
	ULONG		nStorFmt = pStore->GetFormat();
	sal_Bool	bRet = sal_False;

	const ByteString aStrKey = pStore->GetKey();
	const sal_Bool	 bKeySet = (aStrKey.Len() > 0);

	if ( nStorFmt == SOT_FORMATSTR_ID_STARIMPRESS_50 ||
		 nStorFmt == SOT_FORMATSTR_ID_STARDRAW_50    ||
		 nStorFmt == SOT_FORMATSTR_ID_STARDRAW_40    ||
		 nStorFmt == SOT_FORMATSTR_ID_STARDRAW )
	{
		StreamMode	eStmMode = STREAM_STD_READWRITE;
        const BOOL  bProgress = ( mbShowProgress && ( mrDocShell.GetCreateMode() != SFX_CREATE_MODE_EMBEDDED ) );
		BOOL		bIsEmptyStreams = FALSE;

		if( bProgress )
		{
			// create progress
			CreateProgress();

			if( mpProgress )
				mrDocument.SetIOProgressHdl( LINK( this, SdBINFilter, IOProgressHdl ) );
		}

		if( pStore->IsStream( pSfxStyleSheets ) )
		{
			SvStorageStreamRef xPoolStm( pStore->OpenStream( pSfxStyleSheets, STREAM_STD_READWRITE ) );

			if( !( bRet = ( xPoolStm.Is() && ( xPoolStm->GetError() == 0 ) ) ) )
			{
				// try to open as read only
				xPoolStm.Clear();
				xPoolStm = pStore->OpenStream( pSfxStyleSheets, eStmMode = STREAM_STD_READ );
				bRet = ( xPoolStm.Is() && xPoolStm->GetError() == 0 );
			}

			if( bRet )
			{
				// load ItemPool
				xPoolStm->SetVersion( pStore->GetVersion() );
				mrDocShell.GetPool().SetFileFormatVersion( (USHORT) pStore->GetVersion() );
				xPoolStm->SetBufferSize( POOL_BUFFER_SIZE );
				mrDocShell.GetPool().Load( *xPoolStm );
				bRet = ( xPoolStm->GetError() == 0 );
				DBG_ASSERT( bRet, "Error while loading ItemPool" );
			}

			if( bRet )
			{
				// load StyleSheet pool
				( (SfxStyleSheetPool*) mrDocShell.GetStyleSheetPool() )->Load( *xPoolStm );
				xPoolStm->SetBufferSize( 0 );
				bRet = ( xPoolStm->GetError() == 0 );
				DBG_ASSERT( bRet, "Error while loading StyleSheetPool" );
			}

			if( !bRet || xPoolStm->GetErrorCode() )
				mrDocShell.SetError( xPoolStm->GetErrorCode() );
		}
		else
		{
			bIsEmptyStreams = TRUE;
			bRet = TRUE;
		}

		if( bRet )
		{
			// load drawing model
			SvStorageStreamRef	xDocStm;
			sal_Bool			bAcceptableStreamFound = sal_False;

			if( pStore->IsStream( pStarDrawDoc ) )
			{
				bAcceptableStreamFound = sal_True;
				xDocStm = pStore->OpenStream( pStarDrawDoc, eStmMode );
			}
			else if( pStore->IsStream( pStarDrawDoc3 ))
			{
				bAcceptableStreamFound = sal_True;
				xDocStm = pStore->OpenStream( pStarDrawDoc3, eStmMode );
			}

			if( xDocStm.Is() && bAcceptableStreamFound )
			{
				xDocStm->SetVersion( pStore->GetVersion() );
				if( bKeySet )
					xDocStm->SetKey( aStrKey );

				if( bRet = ( xDocStm->GetError() == 0 ) )
				{
					xDocStm->SetBufferSize( DOCUMENT_BUFFER_SIZE );
					if( bKeySet )
						xDocStm->SetKey( aStrKey );
					mrDocument.SetModelStorage( pStore );
					*xDocStm >> mrDocument;
					mrDocument.SetModelStorage( NULL );

					if( !( bRet = ( xDocStm->GetError() == 0 ) ) )
					{
						if( pStore->GetKey().Len() == 0 )
							mrDocShell.SetError( ERRCODE_SFX_DOLOADFAILED );  // no password? --> file defect
						else
							mrDocShell.SetError( ERRCODE_SFX_WRONGPASSWORD ); // wrong password
					}
					else if ( xDocStm->GetErrorCode() )
						mrDocShell.SetError( xDocStm->GetErrorCode() );

					xDocStm->SetBufferSize( 0 );
				}

				bIsEmptyStreams = FALSE;
			}
		}

		if( bRet )
		{
			if( bIsEmptyStreams )
				mrDocument.NewOrLoadCompleted( NEW_DOC );
			else
			{
				mrDocument.NewOrLoadCompleted( DOC_LOADED );
				ImplPostLoadCheckStyleSheetPool( (SfxStyleSheetPool*) mrDocShell.GetStyleSheetPool(), &mrDocument );
				ImplPostLoadFixTextStyles( &mrDocument );
			}
		}

        if( bProgress )
        {
            const Link aNullLink;
            mrDocument.SetIOProgressHdl( aNullLink );
        }
	}
	else
		pStore->SetError( SVSTREAM_WRONGVERSION );

	return bRet;
}



// -----------------------------------------------------------------------------

IMPL_LINK( SdBINFilter, IOProgressHdl, USHORT*, pPercent )
{
	if( mpProgress && pPercent )
		mpProgress->SetState( *pPercent );

	return 0;
}
}
