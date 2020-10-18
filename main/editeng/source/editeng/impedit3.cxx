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
#include "precompiled_editeng.hxx"

#include <vcl/wrkwin.hxx>
#include <vcl/dialog.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/svapp.hxx>
#include <vcl/metaact.hxx>
#include <vcl/gdimtf.hxx>

#define _SVSTDARR_sal_uIt16S
#include <svl/svstdarr.hxx>

#include <vcl/wrkwin.hxx>
#include <editeng/adjitem.hxx>
#include <editeng/tstpitem.hxx>
#include <editeng/lspcitem.hxx>
#include <editeng/flditem.hxx>
#include <impedit.hxx>
#include <editeng/editeng.hxx>
#include <editeng/editview.hxx>
#include <editeng/txtrange.hxx>
#include <editeng/cscoitem.hxx>
#include <editeng/colritem.hxx>
#include <editeng/udlnitem.hxx>
#include <editeng/fhgtitem.hxx>
#include <editeng/kernitem.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/ulspitem.hxx>
#include <editeng/fontitem.hxx>
#include <editeng/wghtitem.hxx>
#include <editeng/postitem.hxx>
#include <editeng/langitem.hxx>
#include <editeng/scriptspaceitem.hxx>
#include <editeng/charscaleitem.hxx>
#include <editeng/numitem.hxx>

#include <svtools/colorcfg.hxx>
#include <svl/ctloptions.hxx>

#include <editeng/forbiddencharacterstable.hxx>

#include <unotools/localedatawrapper.hxx>

#include <editeng/unolingu.hxx>

#include <math.h>
#include <vcl/svapp.hxx>
#include <vcl/metric.hxx>
#include <com/sun/star/i18n/ScriptType.hpp>
#include <com/sun/star/text/CharacterCompressionType.hpp>
#include <vcl/pdfextoutdevdata.hxx>
#include <i18npool/mslangid.hxx>

#include <comphelper/processfactory.hxx>

using ::rtl::OUString;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::linguistic2;

SV_DECL_VARARR_SORT( SortedPositions, sal_uInt32, 16, 8 )
SV_IMPL_VARARR_SORT( SortedPositions, sal_uInt32 );

#define CH_HYPH		'-'

#define RESDIFF		10

#define WRONG_SHOW_MIN 		 5
#define WRONG_SHOW_SMALL 	11
#define WRONG_SHOW_MEDIUM 	15

struct TabInfo
{
	sal_Bool		bValid;

	SvxTabStop	aTabStop;
	xub_StrLen	nCharPos;
	sal_uInt16		nTabPortion;
	long		nStartPosX;
	long		nTabPos;

	TabInfo() { bValid = sal_False; }
};

Point Rotate( const Point& rPoint, short nOrientation, const Point& rOrigin )
{
	double nRealOrientation = nOrientation*F_PI1800;
	double nCos = cos( nRealOrientation );
	double nSin = sin( nRealOrientation );

	Point aRotatedPos;
	Point aTranslatedPos( rPoint );

	// Translation
	aTranslatedPos -= rOrigin;

	// Rotation...
	aRotatedPos.X() = (long)   ( nCos*aTranslatedPos.X() + nSin*aTranslatedPos.Y() );
	aRotatedPos.Y() = (long) - ( nSin*aTranslatedPos.X() - nCos*aTranslatedPos.Y() );
	aTranslatedPos = aRotatedPos;

	// Translation...
	aTranslatedPos += rOrigin;
	return aTranslatedPos;
}

sal_uInt8 GetCharTypeForCompression( xub_Unicode cChar )
{
    switch ( cChar )
    {
        case 0x3008: case 0x300A: case 0x300C: case 0x300E:
        case 0x3010: case 0x3014: case 0x3016: case 0x3018:
        case 0x301A: case 0x301D:
        {
            return CHAR_PUNCTUATIONRIGHT;
        }
        case 0x3001: case 0x3002: case 0x3009: case 0x300B:
        case 0x300D: case 0x300F: case 0x3011: case 0x3015:
        case 0x3017: case 0x3019: case 0x301B: case 0x301E:
        case 0x301F:
        {
            return CHAR_PUNCTUATIONLEFT;
        }
        default:
        {
            return ( ( 0x3040 <= cChar ) && ( 0x3100 > cChar ) ) ? CHAR_KANA : CHAR_NORMAL;
        }
    }
}

void lcl_DrawRedLines( 
    OutputDevice* pOutDev, 
    long nFontHeight, 
    const Point& rPnt, 
    sal_uInt16 nIndex, 
    sal_uInt16 nMaxEnd, 
    const sal_Int32* pDXArray, 
    WrongList* pWrongs, 
    short nOrientation, 
    const Point& rOrigin, 
    sal_Bool bVertical,
    sal_Bool bIsRightToLeft )
{
#ifndef SVX_LIGHT
	// Aber nur, wenn Font nicht zu klein...
	long nHght = pOutDev->LogicToPixel( Size( 0, nFontHeight ) ).Height();
	if( WRONG_SHOW_MIN < nHght )
	{
		sal_uInt16 nStyle;
		if( WRONG_SHOW_MEDIUM < nHght )
			nStyle = WAVE_NORMAL;
		else if( WRONG_SHOW_SMALL < nHght )
			nStyle = WAVE_SMALL;
		else
			nStyle = WAVE_FLAT;

		sal_uInt16 nEnd, nStart = nIndex;
		sal_Bool bWrong = pWrongs->NextWrong( nStart, nEnd );
		while ( bWrong )
		{
			if ( nStart >= nMaxEnd )
				break;

			if ( nStart < nIndex ) 	// Wurde korrigiert
				nStart = nIndex;
			if ( nEnd > nMaxEnd )
				nEnd = nMaxEnd;
			Point aPnt1( rPnt );
			if ( bVertical && ( nStyle != WAVE_FLAT ) )
			{
				// VCL doesn't know that the text is vertical, and is manipulating
				// the positions a little bit in y direction...
				long nOnePixel = pOutDev->PixelToLogic( Size( 0, 1 ) ).Height();
				long nCorrect = ( nStyle == WAVE_NORMAL ) ? 2*nOnePixel : nOnePixel;
				aPnt1.Y() -= nCorrect;
				aPnt1.X() -= nCorrect;
			}
			if ( nStart > nIndex )
			{
				if ( !bVertical )
                {
                    // since for RTL portions rPnt is on the visual right end of the portion
                    // (i.e. at the start of the first RTL char) we need to subtract the offset
                    // for RTL portions...
					aPnt1.X() += (bIsRightToLeft ? -1 : 1) * pDXArray[ nStart - nIndex - 1 ];
                }
				else
					aPnt1.Y() += pDXArray[ nStart - nIndex - 1 ];
			}
			Point aPnt2( rPnt );
			DBG_ASSERT( nEnd > nIndex, "RedLine: aPnt2?" );
			if ( !bVertical )
            {
                // since for RTL portions rPnt is on the visual right end of the portion
                // (i.e. at the start of the first RTL char) we need to subtract the offset
                // for RTL portions...
                aPnt2.X() += (bIsRightToLeft ? -1 : 1) * pDXArray[ nEnd - nIndex - 1 ];
            }
			else
				aPnt2.Y() += pDXArray[ nEnd - nIndex - 1 ];
			if ( nOrientation )
			{
				aPnt1 = Rotate( aPnt1, nOrientation, rOrigin );
				aPnt2 = Rotate( aPnt2, nOrientation, rOrigin );
			}

			pOutDev->DrawWaveLine( aPnt1, aPnt2, nStyle );

			nStart = nEnd+1;
			if ( nEnd < nMaxEnd )
				bWrong = pWrongs->NextWrong( nStart, nEnd );
			else
				bWrong = sal_False;
		}
	}
#endif // !SVX_LIGHT
}

Point lcl_ImplCalcRotatedPos( Point rPos, Point rOrigin, double nSin, double nCos )
{
	Point aRotatedPos;
	// Translation...
	Point aTranslatedPos( rPos);
	aTranslatedPos -= rOrigin;

	aRotatedPos.X() = (long)   ( nCos*aTranslatedPos.X() + nSin*aTranslatedPos.Y() );
	aRotatedPos.Y() = (long) - ( nSin*aTranslatedPos.X() - nCos*aTranslatedPos.Y() );
	aTranslatedPos = aRotatedPos;
	// Translation...
	aTranslatedPos += rOrigin;

	return aTranslatedPos;
}

sal_Bool lcl_IsLigature( xub_Unicode cCh, xub_Unicode cNextCh ) // For Kashidas from sw/source/core/text/porlay.txt
{
            // Lam + Alef
    return ( 0x644 == cCh && 0x627 == cNextCh ) ||
            // Beh + Reh
           ( 0x628 == cCh && 0x631 == cNextCh );
}

sal_Bool lcl_ConnectToPrev( xub_Unicode cCh, xub_Unicode cPrevCh )  // For Kashidas from sw/source/core/text/porlay.txt
{
    // Alef, Dal, Thal, Reh, Zain, and Waw do not connect to the left
    sal_Bool bRet = 0x627 != cPrevCh && 0x62F != cPrevCh && 0x630 != cPrevCh &&
                    0x631 != cPrevCh && 0x632 != cPrevCh && 0x648 != cPrevCh;

    // check for ligatures cPrevChar + cChar
    if ( bRet )
        bRet = ! lcl_IsLigature( cPrevCh, cCh );

    return bRet;
}


//  ----------------------------------------------------------------------
//	class ImpEditEngine
//	----------------------------------------------------------------------
void ImpEditEngine::UpdateViews( EditView* pCurView )
{
	if ( !GetUpdateMode() || IsFormatting() || aInvalidRec.IsEmpty() )
		return;

	DBG_ASSERT( IsFormatted(), "UpdateViews: Doc nicht formatiert!" );

	for ( sal_uInt16 nView = 0; nView < aEditViews.Count(); nView++ )
	{
		EditView* pView = aEditViews[nView];
		DBG_CHKOBJ( pView, EditView, 0 );
		pView->HideCursor();

		Rectangle aClipRec( aInvalidRec );
		Rectangle aVisArea( pView->GetVisArea() );
		aClipRec.Intersection( aVisArea );

		if ( !aClipRec.IsEmpty() )
		{
			// in Fensterkoordinaten umwandeln....
			aClipRec = pView->pImpEditView->GetWindowPos( aClipRec );

			if ( ( pView == pCurView )  )
				Paint( pView->pImpEditView, aClipRec, 0, sal_True );
			else
				pView->GetWindow()->Invalidate( aClipRec );
		}
	}

	if ( pCurView )
	{
		sal_Bool bGotoCursor = pCurView->pImpEditView->DoAutoScroll();
		pCurView->ShowCursor( bGotoCursor );
	}

	aInvalidRec = Rectangle();
	CallStatusHdl();
}

IMPL_LINK( ImpEditEngine, OnlineSpellHdl, Timer *, EMPTYARG )
{
	if ( !Application::AnyInput( INPUT_KEYBOARD ) && GetUpdateMode() && IsFormatted() )
		DoOnlineSpelling();
	else
		aOnlineSpellTimer.Start();

	return 0;
}

IMPL_LINK_INLINE_START( ImpEditEngine, IdleFormatHdl, Timer *, EMPTYARG )
{
	aIdleFormatter.ResetRestarts();

    // #i97146# check if that view is still available
    // else probably the idle format timer fired while we're already
    // downing
    EditView* pView = aIdleFormatter.GetView();    
    for( sal_uInt16 nView = 0; nView < aEditViews.Count(); nView++ )
    {
        if( aEditViews[nView] == pView )
        {
            FormatAndUpdate( pView );
            break;
        }
    }
	return 0;
}
IMPL_LINK_INLINE_END( ImpEditEngine, IdleFormatHdl, Timer *, EMPTYARG )

void ImpEditEngine::CheckIdleFormatter()
{
	aIdleFormatter.ForceTimeout();
	// Falls kein Idle, aber trotzdem nicht formatiert:
	if ( !IsFormatted() )
		FormatDoc();
}

void ImpEditEngine::FormatFullDoc()
{
	for ( sal_uInt16 nPortion = 0; nPortion < GetParaPortions().Count(); nPortion++ )
		GetParaPortions()[nPortion]->MarkSelectionInvalid( 0, GetParaPortions()[nPortion]->GetNode()->Len() );
	FormatDoc();
}

void ImpEditEngine::FormatDoc()
{
	if ( !GetUpdateMode() || IsFormatting() || !GetUpdateModeForAcc())
		return;

    EnterBlockNotifications();

	bIsFormatting = sal_True;

	// Dann kann ich auch den Spell-Timer starten...
	if ( GetStatus().DoOnlineSpelling() )
		StartOnlineSpellTimer();

	long nY = 0;
	sal_Bool bGrow = sal_False;

	Font aOldFont( GetRefDevice()->GetFont() );

	// Hier schon, damit nicht jedesmal in CreateLines...
	sal_Bool bMapChanged = ImpCheckRefMapMode();

	aInvalidRec = Rectangle();	// leermachen
	for ( sal_uInt16 nPara = 0; nPara < GetParaPortions().Count(); nPara++ )
	{
		ParaPortion* pParaPortion = GetParaPortions().GetObject( nPara );
		if ( pParaPortion->MustRepaint() || ( pParaPortion->IsInvalid() && pParaPortion->IsVisible() ) )
		{
			if ( pParaPortion->IsInvalid() )
			{
				sal_Bool bChangedByDerivedClass = GetEditEnginePtr()->FormattingParagraph( nPara );
				if ( bChangedByDerivedClass )
				{
					pParaPortion->GetTextPortions().Reset();
					pParaPortion->MarkSelectionInvalid( 0, pParaPortion->GetNode()->Len() );
				}
			}
			// bei MustRepaint() sollte keine Formatierung noetig sein!
			// 23.1.95: Evtl. ist sie durch eine andere Aktion aber doch
			// ungueltig geworden!
//			if ( pParaPortion->MustRepaint() || CreateLines( nPara ) )
			if ( ( pParaPortion->MustRepaint() && !pParaPortion->IsInvalid() )
					|| CreateLines( nPara, nY ) )
			{
				if ( !bGrow && GetTextRanger() )
				{
					// Bei einer Aenderung der Hoehe muss alles weiter unten
					// neu formatiert werden...
					for ( sal_uInt16 n = nPara+1; n < GetParaPortions().Count(); n++ )
					{
						ParaPortion* pPP = GetParaPortions().GetObject( n );
						pPP->MarkSelectionInvalid( 0, pPP->GetNode()->Len() );
						pPP->GetLines().Reset();
					}
				}
				bGrow = sal_True;
				if ( IsCallParaInsertedOrDeleted() )
					GetEditEnginePtr()->ParagraphHeightChanged( nPara );
				pParaPortion->SetMustRepaint( sal_False );
			}

			// InvalidRec nur einmal setzen...
			if ( aInvalidRec.IsEmpty() )
			{
				// Bei Paperwidth 0 (AutoPageSize) bleibt es sonst Empty()...
				long nWidth = Max( (long)1, ( !IsVertical() ? aPaperSize.Width() : aPaperSize.Height() ) );
				Range aInvRange( GetInvalidYOffsets( pParaPortion ) );
				aInvalidRec = Rectangle( Point( 0, nY+aInvRange.Min() ),
					Size( nWidth, aInvRange.Len() ) );
			}
			else
			{
				aInvalidRec.Bottom() = nY + pParaPortion->GetHeight();
			}
		}
		else if ( bGrow )
		{
			aInvalidRec.Bottom() = nY + pParaPortion->GetHeight();
		}
		nY += pParaPortion->GetHeight();
	}

	// Man kann auch durch UpdateMode An=>AUS=>AN in die Formatierung gelangen...
	// Optimierung erst nach Vobis-Auslieferung aktivieren...
//	if ( !aInvalidRec.IsEmpty() )
	{
		sal_uInt32 nNewHeight = CalcTextHeight();
		long nDiff = nNewHeight - nCurTextHeight;
		if ( nDiff )
			aStatus.GetStatusWord() |= !IsVertical() ? EE_STAT_TEXTHEIGHTCHANGED : EE_STAT_TEXTWIDTHCHANGED;
		if ( nNewHeight < nCurTextHeight )
		{
			aInvalidRec.Bottom() = (long)Max( nNewHeight, nCurTextHeight );
			if ( aInvalidRec.IsEmpty() )
			{
				aInvalidRec.Top() = 0;
				// Left und Right werden nicht ausgewertet, aber wegen IsEmpty gesetzt.
				aInvalidRec.Left() = 0;
				aInvalidRec.Right() = !IsVertical() ? aPaperSize.Width() : aPaperSize.Height();
			}
		}

		nCurTextHeight = nNewHeight;

		if ( aStatus.AutoPageSize() )
			CheckAutoPageSize();
		else if ( nDiff )
		{
			for ( sal_uInt16 nView = 0; nView < aEditViews.Count(); nView++ )
			{
				EditView* pView = aEditViews[nView];
				ImpEditView* pImpView = pView->pImpEditView;
				if ( pImpView->DoAutoHeight() )
				{
					Size aSz( pImpView->GetOutputArea().GetWidth(), nCurTextHeight );
					if ( aSz.Height() > aMaxAutoPaperSize.Height() )
						aSz.Height() = aMaxAutoPaperSize.Height();
					else if ( aSz.Height() < aMinAutoPaperSize.Height() )
						aSz.Height() = aMinAutoPaperSize.Height();
					pImpView->ResetOutputArea( Rectangle(
						pImpView->GetOutputArea().TopLeft(), aSz ) );
				}
			}
		}
	}

	if ( aStatus.DoRestoreFont() )
		GetRefDevice()->SetFont( aOldFont );
	bIsFormatting = sal_False;
	bFormatted = sal_True;

	if ( bMapChanged )
		GetRefDevice()->Pop();

	CallStatusHdl();	// Falls Modified...

    LeaveBlockNotifications();
}

sal_Bool ImpEditEngine::ImpCheckRefMapMode()
{
	sal_Bool bChange = sal_False;

	if ( aStatus.DoFormat100() )
	{
		MapMode aMapMode( GetRefDevice()->GetMapMode() );
		if ( aMapMode.GetScaleX().GetNumerator() != aMapMode.GetScaleX().GetDenominator() )
			bChange = sal_True;
		else if ( aMapMode.GetScaleY().GetNumerator() != aMapMode.GetScaleY().GetDenominator() )
			bChange = sal_True;

		if ( bChange )
		{
			Fraction Scale1( 1, 1 );
			aMapMode.SetScaleX( Scale1 );
			aMapMode.SetScaleY( Scale1 );
			GetRefDevice()->Push();
			GetRefDevice()->SetMapMode( aMapMode );
		}
	}

	return bChange;
}

void ImpEditEngine::CheckAutoPageSize()
{
	Size aPrevPaperSize( GetPaperSize() );
	if ( GetStatus().AutoPageWidth() )
		aPaperSize.Width() = (long) !IsVertical() ? CalcTextWidth( sal_True ) : GetTextHeight();
	if ( GetStatus().AutoPageHeight() )
		aPaperSize.Height() = (long) !IsVertical() ? GetTextHeight() : CalcTextWidth( sal_True );

	SetValidPaperSize( aPaperSize );	//Min, Max beruecksichtigen

	if ( aPaperSize != aPrevPaperSize )
	{
		if ( ( !IsVertical() && ( aPaperSize.Width() != aPrevPaperSize.Width() ) )
			 || ( IsVertical() && ( aPaperSize.Height() != aPrevPaperSize.Height() ) ) )
		{
			// Falls davor zentriert/rechts oder Tabs...
			aStatus.GetStatusWord() |= !IsVertical() ? EE_STAT_TEXTWIDTHCHANGED : EE_STAT_TEXTHEIGHTCHANGED;
			for ( sal_uInt16 nPara = 0; nPara < GetParaPortions().Count(); nPara++ )
			{
				// Es brauchen nur Absaetze neu formatiert werden,
				// die nicht linksbuendig sind.
				// Die Hoehe kann sich hier nicht mehr aendern.
				ParaPortion* pParaPortion = GetParaPortions().GetObject( nPara );
				ContentNode* pNode = pParaPortion->GetNode();
				SvxAdjust eJustification = GetJustification( nPara );
				if ( eJustification != SVX_ADJUST_LEFT )
				{
					pParaPortion->MarkSelectionInvalid( 0, pNode->Len() );
					CreateLines( nPara, 0 );	// 0: Bei AutoPageSize kein TextRange!
				}
			}
		}

		Size aInvSize = aPaperSize;
		if ( aPaperSize.Width() < aPrevPaperSize.Width() )
			aInvSize.Width() = aPrevPaperSize.Width();
		if ( aPaperSize.Height() < aPrevPaperSize.Height() )
			aInvSize.Height() = aPrevPaperSize.Height();

		Size aSz( aInvSize );
		if ( IsVertical() )
		{
			aSz.Width() = aInvSize.Height();
			aSz.Height() = aInvSize.Width();
		}
		aInvalidRec = Rectangle( Point(), aSz );


		for ( sal_uInt16 nView = 0; nView < aEditViews.Count(); nView++ )
		{
			EditView* pView = aEditViews[nView];
			pView->pImpEditView->RecalcOutputArea();
		}
	}
}

static sal_Int32 ImplCalculateFontIndependentLineSpacing( const sal_Int32 nFontHeight )
{
	return ( nFontHeight * 12 ) / 10;	// + 20%
}

sal_Bool ImpEditEngine::CreateLines( sal_uInt16 nPara, sal_uInt32 nStartPosY )
{
	ParaPortion* pParaPortion = GetParaPortions().GetObject( nPara );

	// sal_Bool: Aenderung der Hoehe des Absatzes Ja/Nein - sal_True/sal_False
	DBG_ASSERT( pParaPortion->GetNode(), "Portion ohne Node in CreateLines" );
	DBG_ASSERT( pParaPortion->IsVisible(), "Unsichtbare Absaetze nicht formatieren!" );
	DBG_ASSERT( pParaPortion->IsInvalid(), "CreateLines: Portion nicht invalid!" );

	sal_Bool bProcessingEmptyLine = ( pParaPortion->GetNode()->Len() == 0 );
	sal_Bool bEmptyNodeWithPolygon = ( pParaPortion->GetNode()->Len() == 0 ) && GetTextRanger();

	// ---------------------------------------------------------------
	// Schnelle Sonderbehandlung fuer leere Absaetze...
	// ---------------------------------------------------------------
	if ( ( pParaPortion->GetNode()->Len() == 0 ) && !GetTextRanger() )
	{
		// schnelle Sonderbehandlung...
		if ( pParaPortion->GetTextPortions().Count() )
			pParaPortion->GetTextPortions().Reset();
		if ( pParaPortion->GetLines().Count() )
			pParaPortion->GetLines().Reset();
		CreateAndInsertEmptyLine( pParaPortion, nStartPosY );
		return FinishCreateLines( pParaPortion );
	}

	// ---------------------------------------------------------------
	// Initialisierung......
	// ---------------------------------------------------------------

	// Immer fuer 100% formatieren:
	sal_Bool bMapChanged = ImpCheckRefMapMode();

	if ( pParaPortion->GetLines().Count() == 0 )
	{
		EditLine* pL = new EditLine;
		pParaPortion->GetLines().Insert( pL, 0 );
	}

	// ---------------------------------------------------------------
	// Absatzattribute holen......
	// ---------------------------------------------------------------
	ContentNode* const pNode = pParaPortion->GetNode();

    sal_Bool bRightToLeftPara = IsRightToLeft( nPara );

    SvxAdjust eJustification = GetJustification( nPara );
	sal_Bool bHyphenatePara = ((const SfxBoolItem&)pNode->GetContentAttribs().GetItem( EE_PARA_HYPHENATE )).GetValue();
    sal_Int32 nSpaceBefore      = 0;
    sal_Int32 nMinLabelWidth    = 0;
    sal_Int32 nSpaceBeforeAndMinLabelWidth = GetSpaceBeforeAndMinLabelWidth( pNode, &nSpaceBefore, &nMinLabelWidth );
	const SvxLRSpaceItem& rLRItem = GetLRSpaceItem( pNode );
	const SvxLineSpacingItem& rLSItem = (const SvxLineSpacingItem&) pNode->GetContentAttribs().GetItem( EE_PARA_SBL );
	const sal_Bool bScriptSpace = ((const SvxScriptSpaceItem&) pNode->GetContentAttribs().GetItem( EE_PARA_ASIANCJKSPACING )).GetValue();

//	const sal_uInt16 nInvalidEnd = ( pParaPortion->GetInvalidDiff() > 0 )
//		? pParaPortion->GetInvalidPosStart() + pParaPortion->GetInvalidDiff()
//		: pNode->Len();
	const short nInvalidDiff = pParaPortion->GetInvalidDiff();
	const sal_uInt16 nInvalidStart = pParaPortion->GetInvalidPosStart();
	const sal_uInt16 nInvalidEnd =  nInvalidStart + Abs( nInvalidDiff );

	sal_Bool bQuickFormat = sal_False;
	if ( !bEmptyNodeWithPolygon && !HasScriptType( nPara, i18n::ScriptType::COMPLEX ) )
	{
		if ( ( pParaPortion->IsSimpleInvalid() ) && ( nInvalidDiff > 0 ) &&
			 ( pNode->Search( CH_FEATURE, nInvalidStart ) > nInvalidEnd ) )
		{
			bQuickFormat = sal_True;
		}
		else if ( ( pParaPortion->IsSimpleInvalid() ) && ( nInvalidDiff < 0 ) )
		{
			// pruefen, ob loeschen ueber Portiongrenzen erfolgte...
			sal_uInt16 nStart = nInvalidStart;	// DOPPELT !!!!!!!!!!!!!!!
			sal_uInt16 nEnd = nStart - nInvalidDiff;  // neg.
			bQuickFormat = sal_True;
			sal_uInt16 nPos = 0;
			sal_uInt16 nPortions = pParaPortion->GetTextPortions().Count();
			for ( sal_uInt16 nTP = 0; nTP < nPortions; nTP++ )
			{
				// Es darf kein Start/Ende im geloeschten Bereich liegen.
				TextPortion* const pTP = pParaPortion->GetTextPortions()[ nTP ];
				nPos = nPos + pTP->GetLen();
				if ( ( nPos > nStart ) && ( nPos < nEnd ) )
				{
					bQuickFormat = sal_False;
					break;
				}
			}
		}
	}

    // SW disables TEXT_LAYOUT_COMPLEX_DISABLED, so maybe I have to enable it...

    // #114278# Saving both layout mode and language (since I'm
    // potentially changing both)

    GetRefDevice()->Push( PUSH_TEXTLAYOUTMODE|PUSH_TEXTLANGUAGE );

    ImplInitLayoutMode( GetRefDevice(), nPara, 0xFFFF );

	sal_uInt16 nRealInvalidStart = nInvalidStart;

    if ( bEmptyNodeWithPolygon )
	{
		TextPortion* pDummyPortion = new TextPortion( 0 );
		pParaPortion->GetTextPortions().Reset();
		pParaPortion->GetTextPortions().Insert( pDummyPortion, 0 );
	}
	else if ( bQuickFormat )
	{
		// schnellere Methode:
		RecalcTextPortion( pParaPortion, nInvalidStart, nInvalidDiff );
	}
	else	// nRealInvalidStart kann vor InvalidStart liegen, weil Portions geloescht....
    {
		CreateTextPortions( pParaPortion, nRealInvalidStart );
    }


	// ---------------------------------------------------------------
	// Zeile mit InvalidPos suchen, eine Zeile davor beginnen...
	// Zeilen flaggen => nicht removen !
	// ---------------------------------------------------------------

	sal_uInt16 nLine = pParaPortion->GetLines().Count()-1;
	for ( sal_uInt16 nL = 0; nL <= nLine; nL++ )
	{
		EditLine* pLine = pParaPortion->GetLines().GetObject( nL );
		if ( pLine->GetEnd() > nRealInvalidStart )	// nicht nInvalidStart!
		{
			nLine = nL;
			break;
		}
		pLine->SetValid();
	}
	// Eine Zeile davor beginnen...
	// Wenn ganz hinten getippt wird, kann sich die Zeile davor nicht aendern.
	if ( nLine && ( !pParaPortion->IsSimpleInvalid() || ( nInvalidEnd < pNode->Len() ) || ( nInvalidDiff <= 0 ) ) )
		nLine--;

	EditLine* pLine = pParaPortion->GetLines().GetObject( nLine );

	static Rectangle aZeroArea = Rectangle( Point(), Point() );
	Rectangle aBulletArea( aZeroArea );
	if ( !nLine )
	{
		aBulletArea = GetEditEnginePtr()->GetBulletArea( GetParaPortions().GetPos( pParaPortion ) );
		if ( aBulletArea.Right() > 0 )
			pParaPortion->SetBulletX( (sal_uInt16) GetXValue( aBulletArea.Right() ) );
		else
			pParaPortion->SetBulletX( 0 ); // Falls Bullet falsch eingestellt.
	}

	// ---------------------------------------------------------------
	// Ab hier alle Zeilen durchformatieren...
	// ---------------------------------------------------------------
	sal_uInt16 nDelFromLine = 0xFFFF;
	sal_Bool bLineBreak = sal_False;

	sal_uInt16 nIndex = pLine->GetStart();
	EditLine aSaveLine( *pLine );
	SvxFont aTmpFont( pNode->GetCharAttribs().GetDefFont() );

	sal_Bool bCalcCharPositions = sal_True;
	sal_Int32* pBuf = new sal_Int32[ pNode->Len() ];

	sal_Bool bSameLineAgain = sal_False;	// Fuer TextRanger, wenn sich die Hoehe aendert.
	TabInfo aCurrentTab;

	sal_Bool bForceOneRun = bEmptyNodeWithPolygon;
    sal_Bool bCompressedChars = sal_False;

	while ( ( nIndex < pNode->Len() ) || bForceOneRun )
	{
		bForceOneRun = sal_False;

		sal_Bool bEOL = sal_False;
		sal_Bool bEOC = sal_False;
		sal_uInt16 nPortionStart = 0;
		sal_uInt16 nPortionEnd = 0;

        long nStartX = GetXValue( rLRItem.GetTxtLeft() + nSpaceBeforeAndMinLabelWidth );
		if ( nIndex == 0 )
		{
			long nFI = GetXValue( rLRItem.GetTxtFirstLineOfst() );
            nStartX += nFI;

			if ( !nLine && ( pParaPortion->GetBulletX() > nStartX ) )
			{
// TL_NFLR				nStartX += nFI;	// Vielleicht reicht der LI?
// TL_NFLR				if ( pParaPortion->GetBulletX() > nStartX )
					nStartX = pParaPortion->GetBulletX();
			}
		}

		long nMaxLineWidth;
		if ( !IsVertical() )
			nMaxLineWidth = aStatus.AutoPageWidth() ? aMaxAutoPaperSize.Width() : aPaperSize.Width();
		else
			nMaxLineWidth = aStatus.AutoPageHeight() ? aMaxAutoPaperSize.Height() : aPaperSize.Height();

		nMaxLineWidth -= GetXValue( rLRItem.GetRight() );
		nMaxLineWidth -= nStartX;

		// Wenn PaperSize == long_max, kann ich keinen neg. Erstzeileneinzug
		// abziehen (Overflow)
		if ( ( nMaxLineWidth < 0 ) && ( nStartX < 0 ) )
			nMaxLineWidth = ( !IsVertical() ? aPaperSize.Width() : aPaperSize.Height() ) - GetXValue( rLRItem.GetRight() );

		// Wenn jetzt noch kleiner 0, kann es nur der rechte Rand sein.
		if ( nMaxLineWidth <= 0 )
			nMaxLineWidth = 1;

		// Problem: Da eine Zeile _vor_ der ungueltigen Position mit der
		// Formatierung begonnen wird, werden hier leider auch die Positionen
		// neu bestimmt...
		// Loesungsansatz:
		// Die Zeile davor kann nur groesser werden, nicht kleiner
		// => ...
		if ( bCalcCharPositions )
			pLine->GetCharPosArray().Remove( 0, pLine->GetCharPosArray().Count() );

		sal_uInt16 nTmpPos = nIndex;
		sal_uInt16 nTmpPortion = pLine->GetStartPortion();
		long nTmpWidth = 0;
		long nXWidth = nMaxLineWidth;
		if ( nXWidth <= nTmpWidth )	// while muss 1x durchlaufen werden
			nXWidth = nTmpWidth+1;

		SvLongsPtr pTextRanges = 0;
		long nTextExtraYOffset = 0;
		long nTextXOffset = 0;
		long nTextLineHeight = 0;
		if ( GetTextRanger() )
		{
			GetTextRanger()->SetVertical( IsVertical() );

			long nTextY = nStartPosY + GetEditCursor( pParaPortion, pLine->GetStart() ).Top();
			if ( !bSameLineAgain )
			{
				SeekCursor( pNode, nTmpPos+1, aTmpFont );
				aTmpFont.SetPhysFont( GetRefDevice() );
                ImplInitDigitMode( GetRefDevice(), 0, 0, 0, aTmpFont.GetLanguage() );

				if ( IsFixedCellHeight() )
					nTextLineHeight = ImplCalculateFontIndependentLineSpacing( aTmpFont.GetHeight() );
				else
					nTextLineHeight = aTmpFont.GetPhysTxtSize( GetRefDevice(), String() ).Height();
				// Metriken koennen groesser sein
				FormatterFontMetric aTempFormatterMetrics;
				RecalcFormatterFontMetrics( aTempFormatterMetrics, aTmpFont );
				sal_uInt16 nLineHeight = aTempFormatterMetrics.GetHeight();
				if ( nLineHeight > nTextLineHeight )
					nTextLineHeight = nLineHeight;
			}
			else
				nTextLineHeight = pLine->GetHeight();

			nXWidth = 0;
			while ( !nXWidth )
			{
				long nYOff = nTextY + nTextExtraYOffset;
				long nYDiff = nTextLineHeight;
				if ( IsVertical() )
				{
					long nMaxPolygonX = GetTextRanger()->GetBoundRect().Right();
					nYOff = nMaxPolygonX-nYOff;
					nYDiff = -nTextLineHeight;
				}
				pTextRanges = GetTextRanger()->GetTextRanges( Range( nYOff, nYOff + nYDiff ) );
				DBG_ASSERT( pTextRanges, "GetTextRanges?!" );
				long nMaxRangeWidth = 0;
				// Den breitesten Bereich verwenden...
				// Der breiteste Bereich koennte etwas verwirren, also
				// generell den ersten. Am besten mal richtig mit Luecken.
//				for ( sal_uInt16 n = 0; n < pTextRanges->Count(); )
				if ( pTextRanges->Count() )
				{
					sal_uInt16 n = 0;
					long nA = pTextRanges->GetObject( n++ );
					long nB = pTextRanges->GetObject( n++ );
					DBG_ASSERT( nA <= nB, "TextRange verdreht?" );
					long nW = nB - nA;
					if ( nW > nMaxRangeWidth )
					{
						nMaxRangeWidth = nW;
						nTextXOffset = nA;
					}
				}
				nXWidth = nMaxRangeWidth;
				if ( nXWidth )
					nMaxLineWidth = nXWidth - nStartX - GetXValue( rLRItem.GetRight() );
				else
				{
					// Weiter unten im Polygon versuchen.
					// Unterhalb des Polygons die Paperbreite verwenden.
					nTextExtraYOffset += Max( (long)(nTextLineHeight / 10), (long)1 );
					if ( ( nTextY + nTextExtraYOffset  ) > GetTextRanger()->GetBoundRect().Bottom() )
					{
						nXWidth = !IsVertical() ? GetPaperSize().Width() : GetPaperSize().Height();
						if ( !nXWidth )	// AutoPaperSize
							nXWidth = 0x7FFFFFFF;
					}
				}
			}
		}

		// Portion suchen, die nicht mehr in Zeile passt....
		TextPortion* pPortion = 0;
		sal_Bool bBrokenLine = sal_False;
		bLineBreak = sal_False;
		EditCharAttrib* pNextFeature = pNode->GetCharAttribs().FindFeature( pLine->GetStart() );
		while ( ( nTmpWidth < nXWidth ) && !bEOL && ( nTmpPortion < pParaPortion->GetTextPortions().Count() ) )
		{
			nPortionStart = nTmpPos;
			pPortion = pParaPortion->GetTextPortions().GetObject( nTmpPortion );
			if ( pPortion->GetKind() == PORTIONKIND_HYPHENATOR )
			{
				// Portion wegschmeissen, ggf. die davor korrigieren, wenn
				// die Hyph-Portion ein Zeichen geschluckt hat...
				pParaPortion->GetTextPortions().Remove( nTmpPortion );
				if ( nTmpPortion && pPortion->GetLen() )
				{
					nTmpPortion--;
					TextPortion* pPrev = pParaPortion->GetTextPortions().GetObject( nTmpPortion );
					DBG_ASSERT( pPrev->GetKind() == PORTIONKIND_TEXT, "Portion?!" );
					nTmpWidth -= pPrev->GetSize().Width();
					nTmpPos = nTmpPos - pPrev->GetLen();
					pPrev->SetLen( pPrev->GetLen() + pPortion->GetLen() );
					pPrev->GetSize().Width() = (-1);
				}
				delete pPortion;
				DBG_ASSERT( nTmpPortion < pParaPortion->GetTextPortions().Count(), "Keine Portion mehr da!" );
				pPortion = pParaPortion->GetTextPortions().GetObject( nTmpPortion );
			}
			DBG_ASSERT( pPortion->GetKind() != PORTIONKIND_HYPHENATOR, "CreateLines: Hyphenator-Portion!" );
			DBG_ASSERT( pPortion->GetLen() || bProcessingEmptyLine, "Leere Portion in CreateLines ?!" );
			if ( pNextFeature && ( pNextFeature->GetStart() == nTmpPos ) )
			{
				sal_uInt16 nWhich = pNextFeature->GetItem()->Which();
				switch ( nWhich )
				{
					case EE_FEATURE_TAB:
					{
						long nOldTmpWidth = nTmpWidth;

						// Tab-Pos suchen...
						long nCurPos = nTmpWidth+nStartX;
//						nCurPos -= rLRItem.GetTxtLeft();	// Tabs relativ zu LI
						// Skalierung rausrechnen
						if ( aStatus.DoStretch() && ( nStretchX != 100 ) )
							nCurPos = nCurPos*100/nStretchX;

                        short nAllSpaceBeforeText = static_cast< short >(rLRItem.GetTxtLeft()/* + rLRItem.GetTxtLeft()*/ + nSpaceBeforeAndMinLabelWidth);
                        aCurrentTab.aTabStop = pNode->GetContentAttribs().FindTabStop( nCurPos - nAllSpaceBeforeText /*rLRItem.GetTxtLeft()*/, aEditDoc.GetDefTab() );
                        aCurrentTab.nTabPos = GetXValue( (long) ( aCurrentTab.aTabStop.GetTabPos() + nAllSpaceBeforeText /*rLRItem.GetTxtLeft()*/ ) );
						aCurrentTab.bValid = sal_False;

                        // Switch direction in R2L para...
                        if ( bRightToLeftPara )
                        {
                            if ( aCurrentTab.aTabStop.GetAdjustment() == SVX_TAB_ADJUST_RIGHT )
                                aCurrentTab.aTabStop.GetAdjustment() = SVX_TAB_ADJUST_LEFT;
                            else if ( aCurrentTab.aTabStop.GetAdjustment() == SVX_TAB_ADJUST_LEFT )
                                aCurrentTab.aTabStop.GetAdjustment() = SVX_TAB_ADJUST_RIGHT;
                        }

                        if ( ( aCurrentTab.aTabStop.GetAdjustment() == SVX_TAB_ADJUST_RIGHT ) ||
							 ( aCurrentTab.aTabStop.GetAdjustment() == SVX_TAB_ADJUST_CENTER ) ||
							 ( aCurrentTab.aTabStop.GetAdjustment() == SVX_TAB_ADJUST_DECIMAL ) )
						{
							// Bei LEFT/DEFAULT wird dieses Tab nicht mehr betrachtet.
							aCurrentTab.bValid = sal_True;
							aCurrentTab.nStartPosX = nTmpWidth;
							aCurrentTab.nCharPos = nTmpPos;
							aCurrentTab.nTabPortion = nTmpPortion;
						}

						pPortion->GetKind() = PORTIONKIND_TAB;
						pPortion->SetExtraValue( aCurrentTab.aTabStop.GetFill() );
						pPortion->GetSize().Width() = aCurrentTab.nTabPos - (nTmpWidth+nStartX);

                        // #90520# Height needed...
                        SeekCursor( pNode, nTmpPos+1, aTmpFont );
                        pPortion->GetSize().Height() = aTmpFont.QuickGetTextSize( GetRefDevice(), String(), 0, 0, NULL ).Height();

						DBG_ASSERT( pPortion->GetSize().Width() >= 0, "Tab falsch berechnet!" );

						nTmpWidth = aCurrentTab.nTabPos-nStartX;

						// Wenn dies das erste Token in der Zeile ist,
						// und nTmpWidth > aPaperSize.Width, habe ich eine
						// Endlos-Schleife!
						if ( ( nTmpWidth >= nXWidth ) && ( nTmpPortion == pLine->GetStartPortion() ) )
						{
							// Aber was jetzt ?
							// Tab passend machen
							pPortion->GetSize().Width() = nXWidth-nOldTmpWidth;
							nTmpWidth = nXWidth-1;
							bEOL = sal_True;
							bBrokenLine = sal_True;
						}
						pLine->GetCharPosArray().Insert( pPortion->GetSize().Width(), nTmpPos-pLine->GetStart() );
                        bCompressedChars = sal_False;
					}
					break;
					case EE_FEATURE_LINEBR:
					{
						DBG_ASSERT( pPortion, "?!" );
						pPortion->GetSize().Width() = 0;
						bEOL = sal_True;
						bLineBreak = sal_True;
						pPortion->GetKind() = PORTIONKIND_LINEBREAK;
                        bCompressedChars = sal_False;
						pLine->GetCharPosArray().Insert( pPortion->GetSize().Width(), nTmpPos-pLine->GetStart() );
					}
					break;
					case EE_FEATURE_FIELD:
					{
//						long nCurWidth = nTmpWidth;
						SeekCursor( pNode, nTmpPos+1, aTmpFont );
						sal_Unicode cChar = 0;	// later: NBS?
						aTmpFont.SetPhysFont( GetRefDevice() );
                        ImplInitDigitMode( GetRefDevice(), 0, 0, 0, aTmpFont.GetLanguage() );

						String aFieldValue = cChar ? String(cChar) : ((EditCharAttribField*)pNextFeature)->GetFieldValue();
						if ( bCalcCharPositions || !pPortion->HasValidSize() )
						{
							pPortion->GetSize() = aTmpFont.QuickGetTextSize( GetRefDevice(), aFieldValue, 0, aFieldValue.Len(), 0 );
							// Damit kein Scrollen bei ueberlangen Feldern
							if ( pPortion->GetSize().Width() > nXWidth )
								pPortion->GetSize().Width() = nXWidth;
						}
						nTmpWidth += pPortion->GetSize().Width();
						pLine->GetCharPosArray().Insert( pPortion->GetSize().Width(), nTmpPos-pLine->GetStart() );
						pPortion->GetKind() = cChar ? PORTIONKIND_TEXT : PORTIONKIND_FIELD;
						// Wenn dies das erste Token in der Zeile ist,
						// und nTmpWidth > aPaperSize.Width, habe ich eine
						// Endlos-Schleife!
						if ( ( nTmpWidth >= nXWidth ) && ( nTmpPortion == pLine->GetStartPortion() ) )
						{
							nTmpWidth = nXWidth-1;
							bEOL = sal_True;
							bBrokenLine = sal_True;
						}
                        // Compression in Fields????
                        // I think this could be a little bit difficult and is not very useful
                        bCompressedChars = sal_False;
					}
					break;
					default:	DBG_ERROR( "Was fuer ein Feature ?" );
				}
				pNextFeature = pNode->GetCharAttribs().FindFeature( pNextFeature->GetStart() + 1  );
			}
			else
			{
				DBG_ASSERT( pPortion->GetLen() || bProcessingEmptyLine, "Empty Portion - Extra Space?!" );
				SeekCursor( pNode, nTmpPos+1, aTmpFont );
				aTmpFont.SetPhysFont( GetRefDevice() );
                ImplInitDigitMode( GetRefDevice(), 0, 0, 0, aTmpFont.GetLanguage() );

				if ( bCalcCharPositions || !pPortion->HasValidSize() )
				{
					pPortion->GetSize() = aTmpFont.QuickGetTextSize( GetRefDevice(), *pParaPortion->GetNode(), nTmpPos, pPortion->GetLen(), pBuf );

					// #i9050# Do Kerning also behind portions...
					if ( ( aTmpFont.GetFixKerning() > 0 ) && ( ( nTmpPos + pPortion->GetLen() ) < pNode->Len() ) )
						pPortion->GetSize().Width() += aTmpFont.GetFixKerning();
					if ( IsFixedCellHeight() )
						pPortion->GetSize().Height() = ImplCalculateFontIndependentLineSpacing( aTmpFont.GetHeight() );
				}
                if ( bCalcCharPositions )
				{
					sal_uInt16 nLen = pPortion->GetLen();
					// Es wird am Anfang generell das Array geplaettet
					// => Immer einfach schnelles insert.
					sal_uInt16 nPos = nTmpPos - pLine->GetStart();
					pLine->GetCharPosArray().Insert( pBuf, nLen, nPos );
				}

                // And now check for Compression:
                if ( pPortion->GetLen() && GetAsianCompressionMode() )
                    bCompressedChars |= ImplCalcAsianCompression( pNode, pPortion, nTmpPos, (sal_Int32*)pLine->GetCharPosArray().GetData() + (nTmpPos-pLine->GetStart()), 10000, sal_False );

				nTmpWidth += pPortion->GetSize().Width();

                pPortion->SetRightToLeft( GetRightToLeft( nPara, nTmpPos+1 ) );

                sal_uInt16 _nPortionEnd = nTmpPos + pPortion->GetLen();
                if( bScriptSpace && ( _nPortionEnd < pNode->Len() ) && ( nTmpWidth < nXWidth ) && IsScriptChange( EditPaM( pNode, _nPortionEnd ) ) )
				{
                    sal_Bool bAllow = sal_False;
                    sal_uInt16 nScriptTypeLeft = GetScriptType( EditPaM( pNode, _nPortionEnd ) );
                    sal_uInt16 nScriptTypeRight = GetScriptType( EditPaM( pNode, _nPortionEnd+1 ) );
                    if ( ( nScriptTypeLeft == i18n::ScriptType::ASIAN ) || ( nScriptTypeRight == i18n::ScriptType::ASIAN ) )
                        bAllow = sal_True;

                    // No spacing within L2R/R2L nesting
                    if ( bAllow )
                    {
					    long nExtraSpace = pPortion->GetSize().Height()/5;
                        nExtraSpace = GetXValue( nExtraSpace );
					    pPortion->GetSize().Width() += nExtraSpace;
					    nTmpWidth += nExtraSpace;
                    }
				}
			}

			if ( aCurrentTab.bValid && ( nTmpPortion != aCurrentTab.nTabPortion ) )
			{
				long nWidthAfterTab = 0;
				for ( sal_uInt16 n = aCurrentTab.nTabPortion+1; n <= nTmpPortion; n++  )
				{
					TextPortion* pTP = pParaPortion->GetTextPortions().GetObject( n );
					nWidthAfterTab += pTP->GetSize().Width();
				}
				long nW = nWidthAfterTab;	// Length before tab position
				if ( aCurrentTab.aTabStop.GetAdjustment() == SVX_TAB_ADJUST_RIGHT )
                {
//					nW = nWidthAfterTab;
                }
				else if ( aCurrentTab.aTabStop.GetAdjustment() == SVX_TAB_ADJUST_CENTER )
                {
					nW = nWidthAfterTab/2;
                }
				else if ( aCurrentTab.aTabStop.GetAdjustment() == SVX_TAB_ADJUST_DECIMAL )
				{
//					nW = nWidthAfterTab;
					String aText = GetSelected( EditSelection(  EditPaM( pParaPortion->GetNode(), nTmpPos ),
																EditPaM( pParaPortion->GetNode(), nTmpPos + pPortion->GetLen() ) ) );
					sal_uInt16 nDecPos = aText.Search( aCurrentTab.aTabStop.GetDecimal() );
					if ( nDecPos != STRING_NOTFOUND )
					{
						nW -= pParaPortion->GetTextPortions().GetObject( nTmpPortion )->GetSize().Width();
                        nW += aTmpFont.QuickGetTextSize( GetRefDevice(), *pParaPortion->GetNode(), nTmpPos, nDecPos, NULL ).Width();
						aCurrentTab.bValid = sal_False;
					}
				}
                else
                {
                    DBG_ERROR( "CreateLines: Tab not handled!" );
                }
				long nMaxW = aCurrentTab.nTabPos - aCurrentTab.nStartPosX - nStartX;
				if ( nW >= nMaxW )
				{
					nW = nMaxW;
					aCurrentTab.bValid = sal_False;
				}
				TextPortion* pTabPortion = pParaPortion->GetTextPortions().GetObject( aCurrentTab.nTabPortion );
				pTabPortion->GetSize().Width() = aCurrentTab.nTabPos - aCurrentTab.nStartPosX - nW - nStartX;
				nTmpWidth = aCurrentTab.nStartPosX + pTabPortion->GetSize().Width() + nWidthAfterTab;
			}

			nTmpPos = nTmpPos + pPortion->GetLen();
			nPortionEnd = nTmpPos;
			nTmpPortion++;
			if ( aStatus.OneCharPerLine() )
				bEOL = sal_True;
		}

		DBG_ASSERT( pPortion, "no portion!?" );

		aCurrentTab.bValid = sal_False;

		// das war evtl. eine Portion zu weit:
		sal_Bool bFixedEnd = sal_False;
		if ( aStatus.OneCharPerLine() )
		{
			// Zustand vor Portion: ( bis auf nTmpWidth )
			nPortionEnd = nTmpPos;
			nTmpPos -= pPortion ? pPortion->GetLen() : 0;
			nPortionStart = nTmpPos;
			nTmpPortion--;

			bEOL = sal_True;
			bEOC = sal_False;

			// Und jetzt genau ein Zeichen:
			nTmpPos++;
			nTmpPortion++;
			nPortionEnd = nTmpPortion;
			// Eine Nicht-Feature-Portion muss gebrochen werden
			if ( pPortion->GetLen() > 1 )
			{
				DBG_ASSERT( pPortion && (pPortion->GetKind() == PORTIONKIND_TEXT), "Len>1, aber keine TextPortion?" );
				nTmpWidth -= pPortion ? pPortion->GetSize().Width() : 0;
				sal_uInt16 nP = SplitTextPortion( pParaPortion, nTmpPos, pLine );
				TextPortion* p = pParaPortion->GetTextPortions().GetObject( nP );
				DBG_ASSERT( p, "Portion ?!" );
				nTmpWidth += p->GetSize().Width();
			}
		}
		else if ( nTmpWidth >= nXWidth )
		{
			nPortionEnd = nTmpPos;
			nTmpPos -= pPortion ? pPortion->GetLen() : 0;
			nPortionStart = nTmpPos;
			nTmpPortion--;
			bEOL = sal_False;
			bEOC = sal_False;
			if( pPortion ) switch ( pPortion->GetKind() )
			{
				case PORTIONKIND_TEXT:
				{
					nTmpWidth -= pPortion->GetSize().Width();
				}
				break;
				case PORTIONKIND_FIELD:
				case PORTIONKIND_TAB:
				{
					nTmpWidth -= pPortion->GetSize().Width();
					bEOL = sal_True;
					bFixedEnd = sal_True;
				}
				break;
				default:
				{
					// Ein Feature wird nicht umgebrochen:
					DBG_ASSERT( ( pPortion->GetKind() == PORTIONKIND_LINEBREAK ), "Was fuer ein Feature ?" );
					bEOL = sal_True;
					bFixedEnd = sal_True;
				}
			}
		}
		else
		{
			bEOL = sal_True;
			bEOC = sal_True;
			pLine->SetEnd( nPortionEnd );
			DBG_ASSERT( pParaPortion->GetTextPortions().Count(), "Keine TextPortions?" );
			pLine->SetEndPortion( (sal_uInt16)pParaPortion->GetTextPortions().Count() - 1 );
		}

		if ( aStatus.OneCharPerLine() )
		{
			pLine->SetEnd( nPortionEnd );
			pLine->SetEndPortion( nTmpPortion-1 );
		}
		else if ( bFixedEnd )
		{
			pLine->SetEnd( nPortionStart );
			pLine->SetEndPortion( nTmpPortion-1 );
		}
		else if ( bLineBreak || bBrokenLine )
		{
			pLine->SetEnd( nPortionStart+1 );
			pLine->SetEndPortion( nTmpPortion-1 );
			bEOC = sal_False; // wurde oben gesetzt, vielleich mal die if's umstellen?
		}
		else if ( !bEOL )
		{
			DBG_ASSERT( pPortion && ((nPortionEnd-nPortionStart) == pPortion->GetLen()), "Doch eine andere Portion?!" );
			long nRemainingWidth = nMaxLineWidth - nTmpWidth;
			sal_Bool bCanHyphenate = ( aTmpFont.GetCharSet() != RTL_TEXTENCODING_SYMBOL );
            if ( bCompressedChars && pPortion && ( pPortion->GetLen() > 1 ) && pPortion->GetExtraInfos() && pPortion->GetExtraInfos()->bCompressed )
            {
                // I need the manipulated DXArray for determining the break position...
                ImplCalcAsianCompression( pNode, pPortion, nPortionStart, const_cast<sal_Int32*>(( pLine->GetCharPosArray().GetData() + (nPortionStart-pLine->GetStart()) )), 10000, sal_True );
            }
			if( pPortion )
				ImpBreakLine( pParaPortion, pLine, pPortion, nPortionStart,
												nRemainingWidth, bCanHyphenate && bHyphenatePara );
		}

		// ------------------------------------------------------------------
		// Zeile fertig => justieren
		// ------------------------------------------------------------------

		// CalcTextSize sollte besser durch ein kontinuierliches
		// Registrieren ersetzt werden !
		Size aTextSize = pLine->CalcTextSize( *pParaPortion );

		if ( aTextSize.Height() == 0 )
		{
			SeekCursor( pNode, pLine->GetStart()+1, aTmpFont );
			aTmpFont.SetPhysFont( pRefDev );
            ImplInitDigitMode( pRefDev, 0, 0, 0, aTmpFont.GetLanguage() );

			if ( IsFixedCellHeight() )
				aTextSize.Height() = ImplCalculateFontIndependentLineSpacing( aTmpFont.GetHeight() );
			else
				aTextSize.Height() = aTmpFont.GetPhysTxtSize( pRefDev, String() ).Height();
			pLine->SetHeight( (sal_uInt16)aTextSize.Height() );
		}

		// Die Fontmetriken koennen nicht kontinuierlich berechnet werden,
		// wenn der Font sowieso eingestellt ist, weil ggf. ein grosser Font
		// erst nach dem Umbrechen ploetzlich in der naechsten Zeile landet
		// => Font-Metriken zu gross.
		FormatterFontMetric aFormatterMetrics;
		sal_uInt16 nTPos = pLine->GetStart();
		for ( sal_uInt16 nP = pLine->GetStartPortion(); nP <= pLine->GetEndPortion(); nP++ )
		{
			TextPortion* pTP = pParaPortion->GetTextPortions().GetObject( nP );
            // #95819# problem with hard font height attribute, when everything but the line break has this attribute
            if ( pTP->GetKind() != PORTIONKIND_LINEBREAK )
            {
			    SeekCursor( pNode, nTPos+1, aTmpFont );
			    aTmpFont.SetPhysFont( GetRefDevice() );
                ImplInitDigitMode( GetRefDevice(), 0, 0, 0, aTmpFont.GetLanguage() );
                RecalcFormatterFontMetrics( aFormatterMetrics, aTmpFont );
            }
			nTPos = nTPos + pTP->GetLen();
		}
		sal_uInt16 nLineHeight = aFormatterMetrics.GetHeight();
		if ( nLineHeight > pLine->GetHeight() )
			pLine->SetHeight( nLineHeight );
		pLine->SetMaxAscent( aFormatterMetrics.nMaxAscent );

		bSameLineAgain = sal_False;
		if ( GetTextRanger() && ( pLine->GetHeight() > nTextLineHeight ) )
		{
			// Nochmal mit der anderen Groesse aufsetzen!
			bSameLineAgain = sal_True;
		}


		if ( !bSameLineAgain && !aStatus.IsOutliner() )
		{
			if ( rLSItem.GetLineSpaceRule() == SVX_LINE_SPACE_MIN )
			{
				sal_uInt16 nMinHeight = GetYValue( rLSItem.GetLineHeight() );
				sal_uInt16 nTxtHeight = pLine->GetHeight();
				if ( nTxtHeight < nMinHeight )
				{
					// Der Ascent muss um die Differenz angepasst werden:
					long nDiff = nMinHeight - nTxtHeight;
					pLine->SetMaxAscent( (sal_uInt16)(pLine->GetMaxAscent() + nDiff) );
					pLine->SetHeight( nMinHeight, nTxtHeight );
				}
			}
			else if ( rLSItem.GetInterLineSpaceRule() == SVX_INTER_LINE_SPACE_PROP )
			{
				if ( nPara || IsFixedCellHeight() || pLine->GetStartPortion() ) // Nicht die aller erste Zeile
				{
                    // #100508# There are documents with PropLineSpace 0, why?
                    // (cmc: re above question :-) such documents can be seen by importing a .ppt
                    if ( rLSItem.GetPropLineSpace() && ( rLSItem.GetPropLineSpace() != 100 ) )
                    {
					    sal_uInt16 nTxtHeight = pLine->GetHeight();
					    sal_Int32 nH = nTxtHeight;
					    nH *= rLSItem.GetPropLineSpace();
					    nH /= 100;
					    // Der Ascent muss um die Differenz angepasst werden:
					    long nDiff = pLine->GetHeight() - nH;
					    if ( nDiff > pLine->GetMaxAscent() )
						    nDiff = pLine->GetMaxAscent();
					    pLine->SetMaxAscent( (sal_uInt16)(pLine->GetMaxAscent() - nDiff) );
					    pLine->SetHeight( (sal_uInt16)nH, nTxtHeight );
                    }
				}
			}
			else if( rLSItem.GetLineSpaceRule() == SVX_LINE_SPACE_FIX )
			{
				sal_uInt16 nTxtHeight = pLine->GetHeight();
				sal_uInt32 nH = rLSItem.GetLineHeight();
				if ( nH != nTxtHeight )
				{
					long nMaxAscent = pLine->GetMaxAscent() - nTxtHeight + nH;
					if ( nMaxAscent < 0 )
						nMaxAscent = 0 ;
					pLine->SetMaxAscent( (sal_uInt16)nMaxAscent );
					pLine->SetHeight( (sal_uInt16)nH, nTxtHeight );
				}
			}
		}


		// #80582# - Bullet should not influence line height
//		if ( !nLine )
//		{
//			long nBulletHeight = aBulletArea.GetHeight();
//			if ( nBulletHeight > (long)pLine->GetHeight() )
//			{
//				long nDiff =  nBulletHeight - (long)pLine->GetHeight();
//				// nDiff auf oben und unten verteilen.
//				pLine->SetMaxAscent( (sal_uInt16)(pLine->GetMaxAscent() + nDiff/2) );
//				pLine->SetHeight( (sal_uInt16)nBulletHeight );
//			}
//		}

		if ( ( !IsVertical() && aStatus.AutoPageWidth() ) ||
			 ( IsVertical() && aStatus.AutoPageHeight() ) )
		{
			// Wenn die Zeile in die aktuelle Papierbreite passt, muss
			// diese Breite fuer die Ausrichting verwendet werden.
			// Wenn sie nicht passt oder sie die Papierbreite aendert,
			// wird bei Justification != LEFT sowieso noch mal formatiert.
			long nMaxLineWidthFix = ( !IsVertical() ? aPaperSize.Width() : aPaperSize.Height() )
										- GetXValue( rLRItem.GetRight() ) - nStartX;
			if ( aTextSize.Width() < nMaxLineWidthFix )
				nMaxLineWidth = nMaxLineWidthFix;
		}

        if ( bCompressedChars )
        {
            long nRemainingWidth = nMaxLineWidth - aTextSize.Width();
            if ( nRemainingWidth > 0 )
            {
                ImplExpandCompressedPortions( pLine, pParaPortion, nRemainingWidth );
                aTextSize = pLine->CalcTextSize( *pParaPortion );
            }
        }

        if ( pLine->IsHangingPunctuation() )
		{
			// Width from HangingPunctuation was set to 0 in ImpBreakLine,
			// check for rel width now, maybe create compression...
			long n = nMaxLineWidth - aTextSize.Width();
			TextPortion* pTP = pParaPortion->GetTextPortions().GetObject( pLine->GetEndPortion() );
			sal_uInt16 nPosInArray = pLine->GetEnd()-1-pLine->GetStart();
			long nNewValue = ( nPosInArray ? pLine->GetCharPosArray()[ nPosInArray-1 ] : 0 ) + n;
			pLine->GetCharPosArray()[ nPosInArray ] = nNewValue;
			pTP->GetSize().Width() += n;
		}

        pLine->SetTextWidth( aTextSize.Width() );
        switch ( eJustification )
		{
			case SVX_ADJUST_CENTER:
			{
				long n = ( nMaxLineWidth - aTextSize.Width() ) / 2;
				n += nStartX;  // Einrueckung bleibt erhalten.
				if ( n > 0 )
					pLine->SetStartPosX( (sal_uInt16)n );
				else
					pLine->SetStartPosX( 0 );

			}
			break;
			case SVX_ADJUST_RIGHT:
			{
				// Bei automatisch umgebrochenen Zeilen, die ein Blank
				// am Ende enthalten, darf das Blank nicht ausgegeben werden!

				long n = nMaxLineWidth - aTextSize.Width();
				n += nStartX;  // Einrueckung bleibt erhalten.
				if ( n > 0 )
					pLine->SetStartPosX( (sal_uInt16)n );
				else
					pLine->SetStartPosX( 0 );
			}
			break;
			case SVX_ADJUST_BLOCK:
			{
				long nRemainingSpace = nMaxLineWidth - aTextSize.Width();
				pLine->SetStartPosX( (sal_uInt16)nStartX );
				if ( !bEOC && ( nRemainingSpace > 0 ) ) // nicht die letzte Zeile...
					ImpAdjustBlocks( pParaPortion, pLine, nRemainingSpace );
			}
			break;
			default:
			{
				pLine->SetStartPosX( (sal_uInt16)nStartX ); // FI, LI
			}
			break;
		}

		// -----------------------------------------------------------------
		// pruefen, ob die Zeile neu ausgegeben werden muss...
		// -----------------------------------------------------------------
		pLine->SetInvalid();

		// Wenn eine Portion umgebrochen wurde sind ggf. viel zu viele Positionen
		// im CharPosArray:
		if ( bCalcCharPositions )
		{
			sal_uInt16 nLen = pLine->GetLen();
			sal_uInt16 nCount = pLine->GetCharPosArray().Count();
			if ( nCount > nLen )
				pLine->GetCharPosArray().Remove( nLen, nCount-nLen );
		}

		if ( GetTextRanger() )
		{
			if ( nTextXOffset )
				pLine->SetStartPosX( (sal_uInt16) ( pLine->GetStartPosX() + nTextXOffset ) );
			if ( nTextExtraYOffset )
			{
				pLine->SetHeight( (sal_uInt16) ( pLine->GetHeight() + nTextExtraYOffset ), 0, pLine->GetHeight() );
				pLine->SetMaxAscent( (sal_uInt16) ( pLine->GetMaxAscent() + nTextExtraYOffset ) );
			}
		}

		// Fuer kleiner 0 noch ueberlegen!
		if ( pParaPortion->IsSimpleInvalid() /* && ( nInvalidDiff > 0 ) */ )
		{
			// Aenderung durch einfache Textaenderung...
			// Formatierung nicht abbrechen, da Portions evtl. wieder
			// gesplittet werden muessen!
			// Wenn irgendwann mal abbrechbar, dann fogende Zeilen Validieren!
			// Aber ggf. als Valid markieren, damit weniger Ausgabe...
			if ( pLine->GetEnd() < nInvalidStart )
			{
				if ( *pLine == aSaveLine )
				{
					pLine->SetValid();
				}
			}
			else
			{
				sal_uInt16 nStart = pLine->GetStart();
				sal_uInt16 nEnd = pLine->GetEnd();

				if ( nStart > nInvalidEnd )
				{
					if ( ( ( nStart-nInvalidDiff ) == aSaveLine.GetStart() ) &&
							( ( nEnd-nInvalidDiff ) == aSaveLine.GetEnd() ) )
					{
						pLine->SetValid();
						if ( bCalcCharPositions && bQuickFormat )
						{
							bCalcCharPositions = sal_False;
							bLineBreak = sal_False;
							pParaPortion->CorrectValuesBehindLastFormattedLine( nLine );
							break;
						}
					}
				}
				else if ( bCalcCharPositions && bQuickFormat && ( nEnd > nInvalidEnd) )
				{
					// Wenn die ungueltige Zeile so endet, dass die naechste an
					// der 'gleichen' Textstelle wie vorher beginnt, also nicht
					// anders umgebrochen wird, brauche ich dort auch nicht die
					// textbreiten neu bestimmen:
					if ( nEnd == ( aSaveLine.GetEnd() + nInvalidDiff ) )
					{
						bCalcCharPositions = sal_False;
						bLineBreak = sal_False;
						pParaPortion->CorrectValuesBehindLastFormattedLine( nLine );
						break;
					}
				}
			}
		}

		if ( !bSameLineAgain )
		{
			nIndex = pLine->GetEnd();	// naechste Zeile Start = letzte Zeile Ende
										// weil nEnd hinter das letzte Zeichen zeigt!

			sal_uInt16 nEndPortion = pLine->GetEndPortion();

			// Naechste Zeile oder ggf. neue Zeile....
			pLine = 0;
			if ( nLine < pParaPortion->GetLines().Count()-1 )
				pLine = pParaPortion->GetLines().GetObject( ++nLine );
			if ( pLine && ( nIndex >= pNode->Len() ) )
			{
				nDelFromLine = nLine;
				break;
			}
			if ( !pLine )
			{
				if ( nIndex < pNode->Len() )
				{
					pLine = new EditLine;
					pParaPortion->GetLines().Insert( pLine, ++nLine );
				}
				else if ( nIndex && bLineBreak && GetTextRanger() )
				{
					// normally CreateAndInsertEmptyLine would be called, but I want to use
					// CreateLines, so I need Polygon code only here...
					TextPortion* pDummyPortion = new TextPortion( 0 );
					pParaPortion->GetTextPortions().Insert( pDummyPortion, pParaPortion->GetTextPortions().Count() );
					pLine = new EditLine;
					pParaPortion->GetLines().Insert( pLine, ++nLine );
					bForceOneRun = sal_True;
					bProcessingEmptyLine = sal_True;
				}
			}
			if ( pLine )
			{
				aSaveLine = *pLine;
				pLine->SetStart( nIndex );
				pLine->SetEnd( nIndex );
				pLine->SetStartPortion( nEndPortion+1 );
				pLine->SetEndPortion( nEndPortion+1 );
			}
		}
	}	// while ( Index < Len )

	if ( nDelFromLine != 0xFFFF )
		pParaPortion->GetLines().DeleteFromLine( nDelFromLine );

	DBG_ASSERT( pParaPortion->GetLines().Count(), "Keine Zeile nach CreateLines!" );

	if ( bLineBreak == sal_True )
		CreateAndInsertEmptyLine( pParaPortion, nStartPosY );

	delete[] pBuf;

	sal_Bool bHeightChanged = FinishCreateLines( pParaPortion );

	if ( bMapChanged )
		GetRefDevice()->Pop();

    GetRefDevice()->Pop();

	return bHeightChanged;
}

void ImpEditEngine::CreateAndInsertEmptyLine( ParaPortion* pParaPortion, sal_uInt32 )
{
	DBG_ASSERT( !GetTextRanger(), "Don't use CreateAndInsertEmptyLine with a polygon!" );

	EditLine* pTmpLine = new EditLine;
	pTmpLine->SetStart( pParaPortion->GetNode()->Len() );
	pTmpLine->SetEnd( pParaPortion->GetNode()->Len() );
	pParaPortion->GetLines().Insert( pTmpLine, pParaPortion->GetLines().Count() );

	sal_Bool bLineBreak = pParaPortion->GetNode()->Len() ? sal_True : sal_False;
    sal_Int32 nSpaceBefore = 0;
    sal_Int32 nSpaceBeforeAndMinLabelWidth = GetSpaceBeforeAndMinLabelWidth( pParaPortion->GetNode(), &nSpaceBefore );
	const SvxLRSpaceItem& rLRItem = GetLRSpaceItem( pParaPortion->GetNode() );
	const SvxLineSpacingItem& rLSItem = (const SvxLineSpacingItem&)pParaPortion->GetNode()->GetContentAttribs().GetItem( EE_PARA_SBL );
    short nStartX = GetXValue( (short)(rLRItem.GetTxtLeft() + rLRItem.GetTxtFirstLineOfst() + nSpaceBefore));

	Rectangle aBulletArea = Rectangle( Point(), Point() );
	if ( bLineBreak == sal_True )
	{
        nStartX = (short)GetXValue( rLRItem.GetTxtLeft() + rLRItem.GetTxtFirstLineOfst() + nSpaceBeforeAndMinLabelWidth );
	}
	else
	{
		aBulletArea = GetEditEnginePtr()->GetBulletArea( GetParaPortions().GetPos( pParaPortion ) );
		if ( aBulletArea.Right() > 0 )
			pParaPortion->SetBulletX( (sal_uInt16) GetXValue( aBulletArea.Right() ) );
		else
			pParaPortion->SetBulletX( 0 ); // Falls Bullet falsch eingestellt.
		if ( pParaPortion->GetBulletX() > nStartX )
		{
            nStartX = (short)GetXValue( rLRItem.GetTxtLeft() + rLRItem.GetTxtFirstLineOfst() + nSpaceBeforeAndMinLabelWidth );
			if ( pParaPortion->GetBulletX() > nStartX )
				nStartX = pParaPortion->GetBulletX();
		}
	}

	SvxFont aTmpFont;
	SeekCursor( pParaPortion->GetNode(), bLineBreak ? pParaPortion->GetNode()->Len() : 0, aTmpFont );
	aTmpFont.SetPhysFont( pRefDev );

	TextPortion* pDummyPortion = new TextPortion( 0 );
	pDummyPortion->GetSize() = aTmpFont.GetPhysTxtSize( pRefDev, String() );
	if ( IsFixedCellHeight() )
		pDummyPortion->GetSize().Height() = ImplCalculateFontIndependentLineSpacing( aTmpFont.GetHeight() );
	pParaPortion->GetTextPortions().Insert( pDummyPortion, pParaPortion->GetTextPortions().Count() );
	FormatterFontMetric aFormatterMetrics;
	RecalcFormatterFontMetrics( aFormatterMetrics, aTmpFont );
	pTmpLine->SetMaxAscent( aFormatterMetrics.nMaxAscent );
	pTmpLine->SetHeight( (sal_uInt16) pDummyPortion->GetSize().Height() );
	sal_uInt16 nLineHeight = aFormatterMetrics.GetHeight();
	if ( nLineHeight > pTmpLine->GetHeight() )
		pTmpLine->SetHeight( nLineHeight );

	if ( !aStatus.IsOutliner() )
	{
        sal_uInt16 nPara = GetParaPortions().GetPos( pParaPortion );
		SvxAdjust eJustification = GetJustification( nPara );
		long nMaxLineWidth = !IsVertical() ? aPaperSize.Width() : aPaperSize.Height();
		nMaxLineWidth -= GetXValue( rLRItem.GetRight() );
		long nTextXOffset = 0;
		if ( nMaxLineWidth < 0 )
			nMaxLineWidth = 1;
		if ( eJustification ==  SVX_ADJUST_CENTER )
			nStartX = sal::static_int_cast< short >(nMaxLineWidth / 2);
		else if ( eJustification ==  SVX_ADJUST_RIGHT )
			nStartX = sal::static_int_cast< short >(nMaxLineWidth);

		nStartX = sal::static_int_cast< short >(nStartX + nTextXOffset);
	}

	pTmpLine->SetStartPosX( nStartX );

	if ( !aStatus.IsOutliner() )
	{
		if ( rLSItem.GetLineSpaceRule() == SVX_LINE_SPACE_MIN )
		{
			sal_uInt16 nMinHeight = rLSItem.GetLineHeight();
			sal_uInt16 nTxtHeight = pTmpLine->GetHeight();
			if ( nTxtHeight < nMinHeight )
			{
				// Der Ascent muss um die Differenz angepasst werden:
				long nDiff = nMinHeight - nTxtHeight;
				pTmpLine->SetMaxAscent( (sal_uInt16)(pTmpLine->GetMaxAscent() + nDiff) );
				pTmpLine->SetHeight( nMinHeight, nTxtHeight );
			}
		}
		else if ( rLSItem.GetInterLineSpaceRule() == SVX_INTER_LINE_SPACE_PROP )
		{
            sal_uInt16 nPara = GetParaPortions().GetPos( pParaPortion );
			if ( nPara || IsFixedCellHeight() || pTmpLine->GetStartPortion() ) // Nicht die aller erste Zeile
			{
                // #100508# There are documents with PropLineSpace 0, why?
                // (cmc: re above question :-) such documents can be seen by importing a .ppt
                if ( rLSItem.GetPropLineSpace() && ( rLSItem.GetPropLineSpace() != 100 ) )
                {
				    sal_uInt16 nTxtHeight = pTmpLine->GetHeight();
				    sal_Int32 nH = nTxtHeight;
				    nH *= rLSItem.GetPropLineSpace();
				    nH /= 100;
				    // Der Ascent muss um die Differenz angepasst werden:
				    long nDiff = pTmpLine->GetHeight() - nH;
				    if ( nDiff > pTmpLine->GetMaxAscent() )
					    nDiff = pTmpLine->GetMaxAscent();
				    pTmpLine->SetMaxAscent( (sal_uInt16)(pTmpLine->GetMaxAscent() - nDiff) );
				    pTmpLine->SetHeight( (sal_uInt16)nH, nTxtHeight );
                }
			}
		}
		else if( rLSItem.GetLineSpaceRule() == SVX_LINE_SPACE_FIX )
		{
			sal_uInt16 nTxtHeight = pTmpLine->GetHeight();
			sal_uInt32 nH = rLSItem.GetLineHeight();
			if ( nH != nTxtHeight )
			{
				long nMaxAscent = pTmpLine->GetMaxAscent() - nTxtHeight + nH;
				if ( nMaxAscent < 0 )
					nMaxAscent = 0 ;
				pTmpLine->SetMaxAscent( (sal_uInt16)nMaxAscent );
				pTmpLine->SetHeight( (sal_uInt16)nH, nTxtHeight );
			}
		}
	}

	if ( !bLineBreak )
	{
		long nMinHeight = aBulletArea.GetHeight();
		if ( nMinHeight > (long)pTmpLine->GetHeight() )
		{
			long nDiff = nMinHeight - (long)pTmpLine->GetHeight();
			// nDiff auf oben und unten verteilen.
			pTmpLine->SetMaxAscent( (sal_uInt16)(pTmpLine->GetMaxAscent() + nDiff/2) );
			pTmpLine->SetHeight( (sal_uInt16)nMinHeight );
		}
	}
	else
	{
		// -2: Die neue ist bereits eingefuegt.
#ifdef DBG_UTIL
		EditLine* pLastLine = pParaPortion->GetLines().GetObject( pParaPortion->GetLines().Count()-2 );
		DBG_ASSERT( pLastLine, "Weicher Umbruch, keine Zeile ?!" );
		DBG_ASSERT( pLastLine->GetEnd() == pParaPortion->GetNode()->Len(), "Doch anders?" );
#endif
//		pTmpLine->SetStart( pLastLine->GetEnd() );
//		pTmpLine->SetEnd( pLastLine->GetEnd() );
		sal_uInt16 nPos = (sal_uInt16) pParaPortion->GetTextPortions().Count() - 1 ;
		pTmpLine->SetStartPortion( nPos );
		pTmpLine->SetEndPortion( nPos );
	}
}

sal_Bool ImpEditEngine::FinishCreateLines( ParaPortion* pParaPortion )
{
//	CalcCharPositions( pParaPortion );
	pParaPortion->SetValid();
	long nOldHeight = pParaPortion->GetHeight();
//	sal_uInt16 nPos = GetParaPortions().GetPos( pParaPortion );
//	DBG_ASSERT( nPos != USHRT_MAX, "FinishCreateLines: Portion nicht in Liste!" );
//	ParaPortion* pPrev = nPos ? GetParaPortions().GetObject( nPos-1 ) : 0;
	CalcHeight( pParaPortion );

	DBG_ASSERT( pParaPortion->GetTextPortions().Count(), "FinishCreateLines: Keine Text-Portion?" );
	sal_Bool bRet = ( pParaPortion->GetHeight() != nOldHeight );
	return bRet;
}

void ImpEditEngine::ImpBreakLine( ParaPortion* pParaPortion, EditLine* pLine, TextPortion* pPortion, sal_uInt16 nPortionStart, long nRemainingWidth, sal_Bool bCanHyphenate )
{
	ContentNode* const pNode = pParaPortion->GetNode();

	sal_uInt16 nBreakInLine = nPortionStart - pLine->GetStart();
	sal_uInt16 nMax = nBreakInLine + pPortion->GetLen();
	while ( ( nBreakInLine < nMax ) && ( pLine->GetCharPosArray()[nBreakInLine] < nRemainingWidth ) )
		nBreakInLine++;

    sal_uInt16 nMaxBreakPos = nBreakInLine + pLine->GetStart();
   	sal_uInt16 nBreakPos = 0xFFFF;

    sal_Bool bCompressBlank = sal_False;
    sal_Bool bHyphenated = sal_False;
    sal_Bool bHangingPunctuation = sal_False;
	sal_Unicode cAlternateReplChar = 0;
	sal_Unicode cAlternateExtraChar = 0;

    if ( ( nMaxBreakPos < ( nMax + pLine->GetStart() ) ) && ( pNode->GetChar( nMaxBreakPos ) == ' ' ) )
    {
        // Break behind the blank, blank will be compressed...
        nBreakPos = nMaxBreakPos + 1;
        bCompressBlank = sal_True;
    }
    else
    {
	    sal_uInt16 nMinBreakPos = pLine->GetStart();
	    sal_uInt16 nAttrs = pNode->GetCharAttribs().GetAttribs().Count();
	    for ( sal_uInt16 nAttr = nAttrs; nAttr; )
	    {
		    EditCharAttrib* pAttr = pNode->GetCharAttribs().GetAttribs()[--nAttr];
		    if ( pAttr->IsFeature() && ( pAttr->GetEnd() > nMinBreakPos ) && ( pAttr->GetEnd() <= nMaxBreakPos ) )
		    {
			    nMinBreakPos = pAttr->GetEnd();
			    break;
		    }
	    }

	    lang::Locale aLocale = GetLocale( EditPaM( pNode, nMaxBreakPos ) );

	    Reference < i18n::XBreakIterator > _xBI( ImplGetBreakIterator() );
	    OUString aText( *pNode );
	    Reference< XHyphenator > xHyph;
	    if ( bCanHyphenate )
		    xHyph = GetHyphenator();
	    i18n::LineBreakHyphenationOptions aHyphOptions( xHyph, Sequence< PropertyValue >(), 1 );
	    i18n::LineBreakUserOptions aUserOptions;

	    const i18n::ForbiddenCharacters* pForbidden = GetForbiddenCharsTable()->GetForbiddenCharacters( SvxLocaleToLanguage( aLocale ), sal_True );
	    aUserOptions.forbiddenBeginCharacters = pForbidden->beginLine;
	    aUserOptions.forbiddenEndCharacters = pForbidden->endLine;
	    aUserOptions.applyForbiddenRules = ((const SfxBoolItem&)pNode->GetContentAttribs().GetItem( EE_PARA_FORBIDDENRULES )).GetValue();
	    aUserOptions.allowPunctuationOutsideMargin = ((const SfxBoolItem&)pNode->GetContentAttribs().GetItem( EE_PARA_HANGINGPUNCTUATION )).GetValue();
	    aUserOptions.allowHyphenateEnglish = sal_False;

	    i18n::LineBreakResults aLBR = _xBI->getLineBreak( *pNode, nMaxBreakPos, aLocale, nMinBreakPos, aHyphOptions, aUserOptions );
	    nBreakPos = (sal_uInt16)aLBR.breakIndex;

	    // BUG in I18N - under special condition (break behind field, #87327#) breakIndex is < nMinBreakPos
        if ( nBreakPos < nMinBreakPos )
        {
            nBreakPos = nMinBreakPos;
        }
        else if ( ( nBreakPos > nMaxBreakPos ) && !aUserOptions.allowPunctuationOutsideMargin )
        {
            DBG_ERROR( "I18N: XBreakIterator::getLineBreak returns position > Max" );
            nBreakPos = nMaxBreakPos;
        }

        // #101795# nBreakPos can never be outside the portion, even not with hangig punctuation
        if ( nBreakPos > nMaxBreakPos )
            nBreakPos = nMaxBreakPos;

        // BUG in I18N - the japanese dot is in the next line!
        // !!!	Testen!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        if ( (nBreakPos + ( aUserOptions.allowPunctuationOutsideMargin ? 0 : 1 ) ) <= nMaxBreakPos )
	    {
		    sal_Unicode cFirstInNextLine = ( (nBreakPos+1) < pNode->Len() ) ? pNode->GetChar( nBreakPos ) : 0;
		    if ( cFirstInNextLine == 12290 )
		        nBreakPos++;
        }

	    bHangingPunctuation = ( nBreakPos > nMaxBreakPos ) ? sal_True : sal_False;
	    pLine->SetHangingPunctuation( bHangingPunctuation );

    #ifndef SVX_LIGHT
	    // Egal ob Trenner oder nicht: Das Wort nach dem Trenner durch
	    // die Silbentrennung jagen...
	    // nMaxBreakPos ist das letzte Zeichen was in die Zeile passt,
	    // nBreakPos ist der Wort-Anfang
	    // Ein Problem gibt es, wenn das Dok so schmal ist, dass ein Wort
	    // auf mehr als Zwei Zeilen gebrochen wird...
	    if ( !bHangingPunctuation && bCanHyphenate && GetHyphenator().is() )
	    {
			i18n::Boundary aBoundary = _xBI->getWordBoundary( *pNode, nBreakPos, GetLocale( EditPaM( pNode, nBreakPos ) ), ::com::sun::star::i18n::WordType::DICTIONARY_WORD, sal_True );
//		    sal_uInt16 nWordStart = nBreakPos;
//		    sal_uInt16 nBreakPos_OLD = nBreakPos;
		    sal_uInt16 nWordStart = nBreakPos;
            sal_uInt16 nWordEnd = (sal_uInt16) aBoundary.endPos;
            DBG_ASSERT( nWordEnd > nWordStart, "ImpBreakLine: Start >= End?" );

            sal_uInt16 nWordLen = nWordEnd - nWordStart;
		    if ( ( nWordEnd >= nMaxBreakPos ) && ( nWordLen > 3 ) )
		    {
                // #104415# May happen, because getLineBreak may differ from getWordBoudary with DICTIONARY_WORD
			    // DBG_ASSERT( nWordEnd >= nMaxBreakPos, "Hyph: Break?" );
		        String aWord( *pNode, nWordStart, nWordLen );
			    sal_uInt16 nMinTrail = nWordEnd-nMaxBreakPos+1; 	//+1: Vor dem angeknacksten Buchstaben
			    Reference< XHyphenatedWord > xHyphWord;
			    if (xHyphenator.is())
				    xHyphWord = xHyphenator->hyphenate( aWord, aLocale, aWord.Len() - nMinTrail, Sequence< PropertyValue >() );
			    if (xHyphWord.is())
			    {
				    sal_Bool bAlternate = xHyphWord->isAlternativeSpelling();
				    sal_uInt16 _nWordLen = 1 + xHyphWord->getHyphenPos();

				    if ( ( _nWordLen >= 2 ) && ( (nWordStart+_nWordLen) >= (pLine->GetStart() + 2 ) ) )
				    {
					    if ( !bAlternate )
					    {
						    bHyphenated = sal_True;
						    nBreakPos = nWordStart + _nWordLen;
					    }
					    else
					    {
						    String aAlt( xHyphWord->getHyphenatedWord() );

						    // Wir gehen von zwei Faellen aus, die nun
						    // vorliegen koennen:
						    // 1) packen wird zu pak-ken
						    // 2) Schiffahrt wird zu Schiff-fahrt
						    // In Fall 1 muss ein Zeichen ersetzt werden,
						    // in Fall 2 wird ein Zeichen hinzugefuegt.
						    // Die Identifikation wird erschwert durch Worte wie
						    // "Schiffahrtsbrennesseln", da der Hyphenator alle
						    // Position des Wortes auftrennt und "Schifffahrtsbrennnesseln"
						    // ermittelt. Wir koennen also eigentlich nicht unmittelbar vom
						    // Index des AlternativWord auf aWord schliessen.

						    // Das ganze geraffel wird durch eine Funktion am
						    // Hyphenator vereinfacht werden, sobald AMA sie einbaut...
						    sal_uInt16 nAltStart = _nWordLen - 1;
						    sal_uInt16 nTxtStart = nAltStart - (aAlt.Len() - aWord.Len());
						    sal_uInt16 nTxtEnd = nTxtStart;
						    sal_uInt16 nAltEnd = nAltStart;

						    // Die Bereiche zwischen den nStart und nEnd ist
						    // die Differenz zwischen Alternativ- und OriginalString.
						    while( nTxtEnd < aWord.Len() && nAltEnd < aAlt.Len() &&
							       aWord.GetChar(nTxtEnd) != aAlt.GetChar(nAltEnd) )
						    {
							    ++nTxtEnd;
							    ++nAltEnd;
						    }

						    // Wenn ein Zeichen hinzugekommen ist, dann bemerken wir es jetzt:
						    if( nAltEnd > nTxtEnd && nAltStart == nAltEnd &&
							    aWord.GetChar( nTxtEnd ) == aAlt.GetChar(nAltEnd) )
						    {
							    ++nAltEnd;
							    ++nTxtStart;
							    ++nTxtEnd;
						    }

						    DBG_ASSERT( ( nAltEnd - nAltStart ) == 1, "Alternate: Falsche Annahme!" );

						    if ( nTxtEnd > nTxtStart )
							    cAlternateReplChar = aAlt.GetChar( nAltStart );
						    else
							    cAlternateExtraChar = aAlt.GetChar( nAltStart );

						    bHyphenated = sal_True;
						    nBreakPos = nWordStart + nTxtStart;
						    if ( cAlternateReplChar )
							    nBreakPos++;
					    }
				    }
			    }
		    }
	    }

    #endif // !SVX_LIGHT

	    if ( nBreakPos <= pLine->GetStart() )
	    {
		    // keine Trenner in Zeile => abhacken !
		    nBreakPos = nMaxBreakPos;
		    // MT: I18N nextCharacters !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		    if ( nBreakPos <= pLine->GetStart() )
			    nBreakPos = pLine->GetStart() + 1; 	// Sonst Endlosschleife!
	    }
    }

    // die angeknackste Portion ist die End-Portion
	pLine->SetEnd( nBreakPos );

    sal_uInt16 nEndPortion = SplitTextPortion( pParaPortion, nBreakPos, pLine );

	if ( !bCompressBlank && !bHangingPunctuation )
	{
		// #96187# When justification is not SVX_ADJUST_LEFT, it's important to compress
		// the trailing space even if there is enough room for the space...
		// Don't check for SVX_ADJUST_LEFT, doesn't matter to compress in this case too...
		DBG_ASSERT( nBreakPos > pLine->GetStart(), "ImpBreakLines - BreakPos not expected!" );
		if ( pNode->GetChar( nBreakPos-1 ) == ' ' )
			bCompressBlank = sal_True;
	}

	if ( bCompressBlank || bHangingPunctuation )
	{
		TextPortion* pTP = pParaPortion->GetTextPortions().GetObject( nEndPortion );
		DBG_ASSERT( pTP->GetKind() == PORTIONKIND_TEXT, "BlankRubber: Keine TextPortion!" );
		DBG_ASSERT( nBreakPos > pLine->GetStart(), "SplitTextPortion am Anfang der Zeile?" );
		sal_uInt16 nPosInArray = nBreakPos - 1 - pLine->GetStart();
		pTP->GetSize().Width() = ( nPosInArray && ( pTP->GetLen() > 1 ) ) ? pLine->GetCharPosArray()[ nPosInArray-1 ] : 0;
        pLine->GetCharPosArray()[ nPosInArray ] = pTP->GetSize().Width();
	}
	else if ( bHyphenated )
	{
		// Eine Portion fuer den Trenner einbauen...
		TextPortion* pHyphPortion = new TextPortion( 0 );
		pHyphPortion->GetKind() = PORTIONKIND_HYPHENATOR;
		String aHyphText( CH_HYPH );
		if ( cAlternateReplChar )
		{
			TextPortion* pPrev = pParaPortion->GetTextPortions().GetObject( nEndPortion );
			DBG_ASSERT( pPrev && pPrev->GetLen(), "Hyphenate: Prev portion?!" );
			pPrev->SetLen( pPrev->GetLen() - 1 );
			pHyphPortion->SetLen( 1 );
			pHyphPortion->SetExtraValue( cAlternateReplChar );
			// Breite der Portion davor korrigieren:
			pPrev->GetSize().Width() =
				pLine->GetCharPosArray()[ nBreakPos-1 - pLine->GetStart() - 1 ];
		}
		else if ( cAlternateExtraChar )
		{
			pHyphPortion->SetExtraValue( cAlternateExtraChar );
			aHyphText.Insert( cAlternateExtraChar, 0 );
		}

		// Breite der Hyph-Portion ermitteln:
		SvxFont aFont;
		SeekCursor( pParaPortion->GetNode(), nBreakPos, aFont );
		aFont.SetPhysFont( GetRefDevice() );
		pHyphPortion->GetSize().Height() = GetRefDevice()->GetTextHeight();
		pHyphPortion->GetSize().Width() = GetRefDevice()->GetTextWidth( aHyphText );

		pParaPortion->GetTextPortions().Insert( pHyphPortion, ++nEndPortion );
	}
	pLine->SetEndPortion( nEndPortion );
}

void ImpEditEngine::ImpAdjustBlocks( ParaPortion* pParaPortion, EditLine* pLine, long nRemainingSpace )
{
	DBG_ASSERT( nRemainingSpace > 0, "AdjustBlocks: Etwas zuwenig..." );
	DBG_ASSERT( pLine, "AdjustBlocks: Zeile ?!" );
	if ( ( nRemainingSpace < 0 ) || pLine->IsEmpty() )
		return ;

	const sal_uInt16 nFirstChar = pLine->GetStart();
	const sal_uInt16 nLastChar = pLine->GetEnd() -1;	// Last zeigt dahinter
    ContentNode* pNode = pParaPortion->GetNode();

	DBG_ASSERT( nLastChar < pNode->Len(), "AdjustBlocks: Out of range!" );

	// Search blanks or Kashidas...
    SvUShorts aPositions;
	sal_uInt16 nChar;
	for ( nChar = nFirstChar; nChar <= nLastChar; nChar++ )
	{
		if ( pNode->GetChar(nChar) == ' ' )
        {
            // Don't use blank if language is arabic
            LanguageType eLang = GetLanguage( EditPaM( pNode, nChar ) );
            if ( MsLangId::getPrimaryLanguage( eLang) != LANGUAGE_ARABIC_PRIMARY_ONLY )
                aPositions.Insert( nChar, aPositions.Count() );
        }
	}

    // Kashidas ?
    ImpFindKashidas( pNode, nFirstChar, nLastChar, aPositions );


	if ( !aPositions.Count() )
		return;

	// Wenn das letzte Zeichen ein Blank ist, will ich es nicht haben!
	// Die Breite muss auf die Blocker davor verteilt werden...
	// Aber nicht, wenn es das einzige ist
	if ( ( pNode->GetChar( nLastChar ) == ' ' ) && ( aPositions.Count() > 1 ) && ( MsLangId::getPrimaryLanguage( GetLanguage( EditPaM( pNode, nLastChar ) ) ) != LANGUAGE_ARABIC_PRIMARY_ONLY ) )
	{
        aPositions.Remove( aPositions.Count()-1, 1 );
		sal_uInt16 nPortionStart, nPortion;
		nPortion = pParaPortion->GetTextPortions().FindPortion( nLastChar+1, nPortionStart );
		TextPortion* pLastPortion = pParaPortion->GetTextPortions()[ nPortion ];
		long nRealWidth = pLine->GetCharPosArray()[nLastChar-nFirstChar];
		long nBlankWidth = nRealWidth;
		if ( nLastChar > nPortionStart )
			nBlankWidth -= pLine->GetCharPosArray()[nLastChar-nFirstChar-1];
		// Evtl. ist das Blank schon in ImpBreakLine abgezogen worden:
		if ( nRealWidth == pLastPortion->GetSize().Width() )
		{
			// Beim letzten Zeichen muss die Portion hinter dem Blank aufhoeren
			// => Korrektur vereinfachen:
			DBG_ASSERT( ( nPortionStart + pLastPortion->GetLen() ) == ( nLastChar+1 ), "Blank doch nicht am Portion-Ende?!" );
			pLastPortion->GetSize().Width() -= nBlankWidth;
			nRemainingSpace += nBlankWidth;
		}
		pLine->GetCharPosArray()[nLastChar-nFirstChar] -= nBlankWidth;
	}

    sal_uInt16 nGaps = aPositions.Count();
	const long nMore4Everyone = nRemainingSpace / nGaps;
	long nSomeExtraSpace = nRemainingSpace - nMore4Everyone*nGaps;

	DBG_ASSERT( nSomeExtraSpace < (long)nGaps, "AdjustBlocks: ExtraSpace zu gross" );
	DBG_ASSERT( nSomeExtraSpace >= 0, "AdjustBlocks: ExtraSpace < 0 " );

	// Die Positionen im Array und die Portion-Breiten korrigieren:
	// Letztes Zeichen wird schon nicht mehr beachtet...
    for ( sal_uInt16 n = 0; n < aPositions.Count(); n++ )
	{
        nChar = aPositions[n];
		if ( nChar < nLastChar )
		{
			sal_uInt16 nPortionStart, nPortion;
			nPortion = pParaPortion->GetTextPortions().FindPortion( nChar, nPortionStart );
			TextPortion* pLastPortion = pParaPortion->GetTextPortions()[ nPortion ];

			// Die Breite der Portion:
			pLastPortion->GetSize().Width() += nMore4Everyone;
			if ( nSomeExtraSpace )
				pLastPortion->GetSize().Width()++;

			// Correct positions in array
            // Even for kashidas just change positions, VCL will then draw the kashida automatically
			sal_uInt16 nPortionEnd = nPortionStart + pLastPortion->GetLen();
			for ( sal_uInt16 _n = nChar; _n < nPortionEnd; _n++ )
			{
				pLine->GetCharPosArray()[_n-nFirstChar] += nMore4Everyone;
				if ( nSomeExtraSpace )
					pLine->GetCharPosArray()[_n-nFirstChar]++;
			}

			if ( nSomeExtraSpace )
				nSomeExtraSpace--;
		}
	}

    // Now the text width contains the extra width...
    pLine->SetTextWidth( pLine->GetTextWidth() + nRemainingSpace );
}

void ImpEditEngine::ImpFindKashidas( ContentNode* pNode, sal_uInt16 nStart, sal_uInt16 nEnd, SvUShorts& rArray )
{
    // the search has to be performed on a per word base

    EditSelection aWordSel( EditPaM( pNode, nStart ) );
    aWordSel = SelectWord( aWordSel, ::com::sun::star::i18n::WordType::DICTIONARY_WORD );
    if ( aWordSel.Min().GetIndex() < nStart )
       aWordSel.Min().GetIndex() = nStart;

    while ( ( aWordSel.Min().GetNode() == pNode ) && ( aWordSel.Min().GetIndex() < nEnd ) )
    {
        sal_uInt16 nSavPos = aWordSel.Max().GetIndex();
        if ( aWordSel.Max().GetIndex() > nEnd )
           aWordSel.Max().GetIndex() = nEnd;

        String aWord = GetSelected( aWordSel );

        // restore selection for proper iteration at the end of the function
        aWordSel.Max().GetIndex() = nSavPos;

        xub_StrLen nIdx = 0;
        xub_StrLen nKashidaPos = STRING_LEN;
        xub_Unicode cCh;
        xub_Unicode cPrevCh = 0;

        while ( nIdx < aWord.Len() )
        {
            cCh = aWord.GetChar( nIdx );

            // 1. Priority:
            // after user inserted kashida
            if ( 0x640 == cCh )
            {
                nKashidaPos = aWordSel.Min().GetIndex() + nIdx;
                break;
            }

            // 2. Priority:
            // after a Seen or Sad
            if ( nIdx + 1 < aWord.Len() &&
                 ( 0x633 == cCh || 0x635 == cCh ) )
            {
                nKashidaPos = aWordSel.Min().GetIndex() + nIdx;
                break;
            }

            // 3. Priority:
            // before final form of Teh Marbuta, Hah, Dal
            // 4. Priority:
            // before final form of Alef, Lam or Kaf
            if ( nIdx && nIdx + 1 == aWord.Len() &&
                 ( 0x629 == cCh || 0x62D == cCh || 0x62F == cCh ||
                   0x627 == cCh || 0x644 == cCh || 0x643 == cCh ) )
            {
                DBG_ASSERT( 0 != cPrevCh, "No previous character" );

                // check if character is connectable to previous character,
                if ( lcl_ConnectToPrev( cCh, cPrevCh ) )
                {
                    nKashidaPos = aWordSel.Min().GetIndex() + nIdx - 1;
                    break;
                }
            }

            // 5. Priority:
            // before media Bah
            if ( nIdx && nIdx + 1 < aWord.Len() && 0x628 == cCh )
            {
                DBG_ASSERT( 0 != cPrevCh, "No previous character" );

                // check if next character is Reh, Yeh or Alef Maksura
                xub_Unicode cNextCh = aWord.GetChar( nIdx + 1 );

                if ( 0x631 == cNextCh || 0x64A == cNextCh ||
                     0x649 == cNextCh )
                {
                    // check if character is connectable to previous character,
                    if ( lcl_ConnectToPrev( cCh, cPrevCh ) )
                        nKashidaPos = aWordSel.Min().GetIndex() + nIdx - 1;
                }
            }

            // 6. Priority:
            // other connecting possibilities
            if ( nIdx && nIdx + 1 == aWord.Len() &&
                 0x60C <= cCh && 0x6FE >= cCh )
            {
                DBG_ASSERT( 0 != cPrevCh, "No previous character" );

                // check if character is connectable to previous character,
                if ( lcl_ConnectToPrev( cCh, cPrevCh ) )
                {
                    // only choose this position if we did not find
                    // a better one:
                    if ( STRING_LEN == nKashidaPos )
                        nKashidaPos = aWordSel.Min().GetIndex() + nIdx - 1;
                    break;
                }
            }

            // Do not consider Fathatan, Dammatan, Kasratan, Fatha,
            // Damma, Kasra, Shadda and Sukun when checking if
            // a character can be connected to previous character.
            if ( cCh < 0x64B || cCh > 0x652 )
                cPrevCh = cCh;

            ++nIdx;
        } // end of current word

        if ( STRING_LEN != nKashidaPos )
            rArray.Insert( nKashidaPos, rArray.Count() );

        aWordSel = WordRight( aWordSel.Max(), ::com::sun::star::i18n::WordType::DICTIONARY_WORD );
        aWordSel = SelectWord( aWordSel, ::com::sun::star::i18n::WordType::DICTIONARY_WORD );
    }
}

sal_uInt16 ImpEditEngine::SplitTextPortion( ParaPortion* pPortion, sal_uInt16 nPos, EditLine* pCurLine )
{
	DBG_ASSERT( pPortion, "SplitTextPortion: Welche ?" );

	// Die Portion bei nPos wird geplittet, wenn bei nPos nicht
	// sowieso ein Wechsel ist
	if ( nPos == 0 )
		return 0;

	sal_uInt16 nSplitPortion;
	sal_uInt16 nTmpPos = 0;
	TextPortion* pTextPortion = 0;
	sal_uInt16 nPortions = pPortion->GetTextPortions().Count();
	for ( nSplitPortion = 0; nSplitPortion < nPortions; nSplitPortion++ )
	{
		TextPortion* pTP = pPortion->GetTextPortions().GetObject(nSplitPortion);
		nTmpPos = nTmpPos + pTP->GetLen();
		if ( nTmpPos >= nPos )
		{
			if ( nTmpPos == nPos )	// dann braucht nichts geteilt werden
			{
				// Skip Portions with ExtraSpace
//				while ( ( (nSplitPortion+1) < nPortions ) && (pPortion->GetTextPortions().GetObject(nSplitPortion+1)->GetKind() == PORTIONKIND_EXTRASPACE ) )
//					nSplitPortion++;

				return nSplitPortion;
			}
			pTextPortion = pTP;
			break;
		}
	}

	DBG_ASSERT( pTextPortion, "Position ausserhalb des Bereichs!" );
	DBG_ASSERT( pTextPortion->GetKind() == PORTIONKIND_TEXT, "SplitTextPortion: Keine TextPortion!" );

	sal_uInt16 nOverlapp = nTmpPos - nPos;
	pTextPortion->GetLen() = pTextPortion->GetLen() - nOverlapp;
	TextPortion* pNewPortion = new TextPortion( nOverlapp );
	pPortion->GetTextPortions().Insert( pNewPortion, nSplitPortion+1 );
	// Groessen setzen:
	if ( pCurLine )
	{
		// Kein neues GetTextSize, sondern Werte aus Array verwenden:
		DBG_ASSERT( nPos > pCurLine->GetStart(), "SplitTextPortion am Anfang der Zeile?" );
		pTextPortion->GetSize().Width() = pCurLine->GetCharPosArray()[ nPos-pCurLine->GetStart()-1 ];

        if ( pTextPortion->GetExtraInfos() && pTextPortion->GetExtraInfos()->bCompressed )
        {
            // We need the original size from the portion
            sal_uInt16 nTxtPortionStart = pPortion->GetTextPortions().GetStartPos( nSplitPortion );
           	SvxFont aTmpFont( pPortion->GetNode()->GetCharAttribs().GetDefFont() );
		    SeekCursor( pPortion->GetNode(), nTxtPortionStart+1, aTmpFont );
		    aTmpFont.SetPhysFont( GetRefDevice() );
            GetRefDevice()->Push( PUSH_TEXTLANGUAGE );
            ImplInitDigitMode( GetRefDevice(), 0, 0, 0, aTmpFont.GetLanguage() );
            Size aSz = aTmpFont.QuickGetTextSize( GetRefDevice(), *pPortion->GetNode(), nTxtPortionStart, pTextPortion->GetLen(), NULL );
            GetRefDevice()->Pop();
            pTextPortion->GetExtraInfos()->nOrgWidth = aSz.Width();
        }
	}
	else
		pTextPortion->GetSize().Width() = (-1);

	return nSplitPortion;
}

void ImpEditEngine::CreateTextPortions( ParaPortion* pParaPortion, sal_uInt16& rStart /* , sal_Bool bCreateBlockPortions */ )
{
	sal_uInt16 nStartPos = rStart;
	ContentNode* pNode = pParaPortion->GetNode();
	DBG_ASSERT( pNode->Len(), "CreateTextPortions sollte nicht fuer leere Absaetze verwendet werden!" );

	SortedPositions aPositions;
	aPositions.Insert( (sal_uInt32) 0 );

	sal_uInt16 nAttr = 0;
	EditCharAttrib* pAttrib = GetAttrib( pNode->GetCharAttribs().GetAttribs(), nAttr );
	while ( pAttrib )
	{
		// Start und Ende in das Array eintragen...
		// Die InsertMethode laesst keine doppelten Werte zu....
		aPositions.Insert( pAttrib->GetStart() );
		aPositions.Insert( pAttrib->GetEnd() );
		nAttr++;
		pAttrib = GetAttrib( pNode->GetCharAttribs().GetAttribs(), nAttr );
	}
    aPositions.Insert( pNode->Len() );

    if ( pParaPortion->aScriptInfos.empty() )
		((ImpEditEngine*)this)->InitScriptTypes( GetParaPortions().GetPos( pParaPortion ) );

	const ScriptTypePosInfos& rTypes = pParaPortion->aScriptInfos;
	for ( size_t nT = 0; nT < rTypes.size(); nT++ )
		aPositions.Insert( rTypes[nT].nStartPos );

    const WritingDirectionInfos& rWritingDirections = pParaPortion->aWritingDirectionInfos;
	for ( size_t nD = 0; nD < rWritingDirections.size(); nD++ )
		aPositions.Insert( rWritingDirections[nD].nStartPos );

	if ( mpIMEInfos && mpIMEInfos->nLen && mpIMEInfos->pAttribs && ( mpIMEInfos->aPos.GetNode() == pNode ) )
	{
		sal_uInt16 nLastAttr = 0xFFFF;
		for( sal_uInt16 n = 0; n < mpIMEInfos->nLen; n++ )
		{
			if ( mpIMEInfos->pAttribs[n] != nLastAttr )
			{
				aPositions.Insert( mpIMEInfos->aPos.GetIndex() + n );
				nLastAttr = mpIMEInfos->pAttribs[n];
			}
		}
		aPositions.Insert( mpIMEInfos->aPos.GetIndex() + mpIMEInfos->nLen );
	}

	// Ab ... loeschen:
	// Leider muss die Anzahl der TextPortions mit aPositions.Count()
	// nicht uebereinstimmen, da evtl. Zeilenumbrueche...
	sal_uInt16 nPortionStart = 0;
	sal_uInt16 nInvPortion = 0;
    sal_uInt16 nP;
	for ( nP = 0; nP < pParaPortion->GetTextPortions().Count(); nP++ )
	{
		TextPortion* pTmpPortion = pParaPortion->GetTextPortions().GetObject(nP);
		nPortionStart = nPortionStart + pTmpPortion->GetLen();
		if ( nPortionStart >= nStartPos )
		{
			nPortionStart = nPortionStart - pTmpPortion->GetLen();
			rStart = nPortionStart;
			nInvPortion = nP;
			break;
		}
	}
	DBG_ASSERT( nP < pParaPortion->GetTextPortions().Count() || !pParaPortion->GetTextPortions().Count(), "Nichts zum loeschen: CreateTextPortions" );
	if ( nInvPortion && ( nPortionStart+pParaPortion->GetTextPortions().GetObject(nInvPortion)->GetLen() > nStartPos ) )
	{
		// lieber eine davor...
		// Aber nur wenn es mitten in der Portion war, sonst ist es evtl.
		// die einzige in der Zeile davor !
		nInvPortion--;
		nPortionStart = nPortionStart - pParaPortion->GetTextPortions().GetObject(nInvPortion)->GetLen();
	}
	pParaPortion->GetTextPortions().DeleteFromPortion( nInvPortion );

	// Eine Portion kann auch durch einen Zeilenumbruch entstanden sein:
	aPositions.Insert( nPortionStart );

	sal_uInt16 nInvPos;
#ifdef DBG_UTIL
	sal_Bool bFound =
#endif
						aPositions.Seek_Entry( nPortionStart, &nInvPos );

	DBG_ASSERT( bFound && ( nInvPos < (aPositions.Count()-1) ), "InvPos ?!" );
	for ( sal_uInt16 i = nInvPos+1; i < aPositions.Count(); i++ )
	{
        TextPortion* pNew = new TextPortion( (sal_uInt16)aPositions[i] - (sal_uInt16)aPositions[i-1] );
        pParaPortion->GetTextPortions().Insert( pNew, pParaPortion->GetTextPortions().Count());
	}

	DBG_ASSERT( pParaPortion->GetTextPortions().Count(), "Keine Portions?!" );
#ifdef EDITDEBUG
	DBG_ASSERT( pParaPortion->DbgCheckTextPortions(), "Portions kaputt?" );
#endif
}

void ImpEditEngine::RecalcTextPortion( ParaPortion* pParaPortion, sal_uInt16 nStartPos, short nNewChars )
{
	DBG_ASSERT( pParaPortion->GetTextPortions().Count(), "Keine Portions!" );
	DBG_ASSERT( nNewChars, "RecalcTextPortion mit Diff == 0" );

	ContentNode* const pNode = pParaPortion->GetNode();
	if ( nNewChars > 0 )
	{
		// Wenn an nStartPos ein Attribut beginnt/endet, faengt eine neue Portion
		// an, ansonsten wird die Portion an nStartPos erweitert.

		if ( pNode->GetCharAttribs().HasBoundingAttrib( nStartPos ) || IsScriptChange( EditPaM( pNode, nStartPos ) ) )
		{
			sal_uInt16 nNewPortionPos = 0;
			if ( nStartPos )
				nNewPortionPos = SplitTextPortion( pParaPortion, nStartPos ) + 1;

			// Eine leere Portion kann hier stehen, wenn der Absatz leer war,
			// oder eine Zeile durch einen harten Zeilenumbruch entstanden ist.
			if ( ( nNewPortionPos < pParaPortion->GetTextPortions().Count() ) &&
					!pParaPortion->GetTextPortions()[nNewPortionPos]->GetLen() )
			{
				DBG_ASSERT( pParaPortion->GetTextPortions()[nNewPortionPos]->GetKind() == PORTIONKIND_TEXT, "Leere Portion war keine TextPortion!" );
                sal_uInt16 & r =
                    pParaPortion->GetTextPortions()[nNewPortionPos]->GetLen();
				r = r + nNewChars;
			}
			else
			{
				TextPortion* pNewPortion = new TextPortion( nNewChars );
				pParaPortion->GetTextPortions().Insert( pNewPortion, nNewPortionPos );
			}
		}
		else
		{
			sal_uInt16 nPortionStart;
			const sal_uInt16 nTP = pParaPortion->GetTextPortions().
				FindPortion( nStartPos, nPortionStart );
			TextPortion* const pTP = pParaPortion->GetTextPortions()[ nTP ];
			DBG_ASSERT( pTP, "RecalcTextPortion: Portion nicht gefunden"  );
			pTP->GetLen() = pTP->GetLen() + nNewChars;
			pTP->GetSize().Width() = (-1);
		}
	}
	else
	{
		// Portion schrumpfen oder ggf. entfernen.
		// Vor Aufruf dieser Methode muss sichergestellt sein, dass
		// keine Portions in dem geloeschten Bereich lagen!

		// Es darf keine reinragende oder im Bereich startende Portion geben,
		// also muss nStartPos <= nPos <= nStartPos - nNewChars(neg.) sein
		sal_uInt16 nPortion = 0;
		sal_uInt16 nPos = 0;
		sal_uInt16 nEnd = nStartPos-nNewChars;
		sal_uInt16 nPortions = pParaPortion->GetTextPortions().Count();
		TextPortion* pTP = 0;
		for ( nPortion = 0; nPortion < nPortions; nPortion++ )
		{
			pTP = pParaPortion->GetTextPortions()[ nPortion ];
			if ( ( nPos+pTP->GetLen() ) > nStartPos )
			{
				DBG_ASSERT( nPos <= nStartPos, "Start falsch!" );
				DBG_ASSERT( nPos+pTP->GetLen() >= nEnd, "End falsch!" );
				break;
			}
			nPos = nPos + pTP->GetLen();
		}
		DBG_ASSERT( pTP, "RecalcTextPortion: Portion nicht gefunden" );
		if ( ( nPos == nStartPos ) && ( (nPos+pTP->GetLen()) == nEnd ) )
		{
			// Portion entfernen;
			sal_uInt8 nType = pTP->GetKind();
			pParaPortion->GetTextPortions().Remove( nPortion );
			delete pTP;
			if ( nType == PORTIONKIND_LINEBREAK )
			{
				TextPortion* pNext = pParaPortion->GetTextPortions()[ nPortion ];
				if ( pNext && !pNext->GetLen() )
				{
					// Dummy-Portion entfernen
					pParaPortion->GetTextPortions().Remove( nPortion );
					delete pNext;
				}
			}
		}
		else
		{
			DBG_ASSERT( pTP->GetLen() > (-nNewChars), "Portion zu klein zum schrumpfen!" );
			pTP->GetLen() = pTP->GetLen() + nNewChars;
		}

		// ganz am Schluss darf keine HYPHENATOR-Portion stehen bleiben...
		DBG_ASSERT( pParaPortion->GetTextPortions().Count(), "RecalcTextPortions: Keine mehr da!" );
		sal_uInt16 nLastPortion = pParaPortion->GetTextPortions().Count() - 1;
		pTP = pParaPortion->GetTextPortions().GetObject( nLastPortion );
		if ( pTP->GetKind() == PORTIONKIND_HYPHENATOR )
		{
			// Portion wegschmeissen, ggf. die davor korrigieren, wenn
			// die Hyph-Portion ein Zeichen geschluckt hat...
			pParaPortion->GetTextPortions().Remove( nLastPortion );
			if ( nLastPortion && pTP->GetLen() )
			{
				TextPortion* pPrev = pParaPortion->GetTextPortions().GetObject( nLastPortion - 1 );
				DBG_ASSERT( pPrev->GetKind() == PORTIONKIND_TEXT, "Portion?!" );
				pPrev->SetLen( pPrev->GetLen() + pTP->GetLen() );
				pPrev->GetSize().Width() = (-1);
			}
			delete pTP;
		}
	}
#ifdef EDITDEBUG
	DBG_ASSERT( pParaPortion->DbgCheckTextPortions(), "Portions kaputt?" );
#endif
}

void ImpEditEngine::SetTextRanger( TextRanger* pRanger )
{
	if ( pTextRanger != pRanger )
	{
		delete pTextRanger;
		pTextRanger = pRanger;

		for ( sal_uInt16 nPara = 0; nPara < GetParaPortions().Count(); nPara++ )
		{
			ParaPortion* pParaPortion = GetParaPortions().GetObject( nPara );
			pParaPortion->MarkSelectionInvalid( 0, pParaPortion->GetNode()->Len() );
			pParaPortion->GetLines().Reset();
		}

		FormatFullDoc();
		UpdateViews( GetActiveView() );
		if ( GetUpdateMode() && GetActiveView() )
			pActiveView->ShowCursor( sal_False, sal_False );
	}
}

void ImpEditEngine::SetVertical( sal_Bool bVertical )
{
	if ( IsVertical() != bVertical )
	{
		GetEditDoc().SetVertical( bVertical );
		sal_Bool bUseCharAttribs = ( aStatus.GetControlWord() & EE_CNTRL_USECHARATTRIBS ) ? sal_True : sal_False;
		GetEditDoc().CreateDefFont( bUseCharAttribs );
		if ( IsFormatted() )
		{
			FormatFullDoc();
			UpdateViews( GetActiveView() );
		}
	}
}

void ImpEditEngine::SetFixedCellHeight( sal_Bool bUseFixedCellHeight )
{
	if ( IsFixedCellHeight() != bUseFixedCellHeight )
	{
		GetEditDoc().SetFixedCellHeight( bUseFixedCellHeight );
		if ( IsFormatted() )
		{
			FormatFullDoc();
			UpdateViews( GetActiveView() );
		}
	}
}

void ImpEditEngine::SeekCursor( ContentNode* pNode, sal_uInt16 nPos, SvxFont& rFont, OutputDevice* pOut, sal_uInt16 nIgnoreWhich )
{
	// Es war mal geplant, SeekCursor( nStartPos, nEndPos, ... ), damit nur
	// ab der StartPosition neu gesucht wird.
	// Problem: Es mussten zwei Listen beruecksichtigt/gefuehrt werden:
	// OrderedByStart,OrderedByEnd.

	if ( nPos > pNode->Len() )
		nPos = pNode->Len();

	rFont = pNode->GetCharAttribs().GetDefFont();

	short nScriptType = GetScriptType( EditPaM( pNode, nPos ) );
	if ( ( nScriptType == i18n::ScriptType::ASIAN ) || ( nScriptType == i18n::ScriptType::COMPLEX ) )
	{
		const SvxFontItem& rFontItem = (const SvxFontItem&)pNode->GetContentAttribs().GetItem( GetScriptItemId( EE_CHAR_FONTINFO, nScriptType ) );
		rFont.SetName( rFontItem.GetFamilyName() );
		rFont.SetFamily( rFontItem.GetFamily() );
		rFont.SetPitch( rFontItem.GetPitch() );
		rFont.SetCharSet( rFontItem.GetCharSet() );
		Size aSz( rFont.GetSize() );
		aSz.Height() = ((const SvxFontHeightItem&)pNode->GetContentAttribs().GetItem( GetScriptItemId( EE_CHAR_FONTHEIGHT, nScriptType ) ) ).GetHeight();
		rFont.SetSize( aSz );
		rFont.SetWeight( ((const SvxWeightItem&)pNode->GetContentAttribs().GetItem( GetScriptItemId( EE_CHAR_WEIGHT, nScriptType ))).GetWeight() );
		rFont.SetItalic( ((const SvxPostureItem&)pNode->GetContentAttribs().GetItem( GetScriptItemId( EE_CHAR_ITALIC, nScriptType ))).GetPosture() );
		rFont.SetLanguage( ((const SvxLanguageItem&)pNode->GetContentAttribs().GetItem( GetScriptItemId( EE_CHAR_LANGUAGE, nScriptType ))).GetLanguage() );
	}

	sal_uInt16 nRelWidth = ((const SvxCharScaleWidthItem&)pNode->GetContentAttribs().GetItem( EE_CHAR_FONTWIDTH)).GetValue();

	if ( pOut )
	{
		const SvxUnderlineItem& rTextLineColor = (const SvxUnderlineItem&)pNode->GetContentAttribs().GetItem( EE_CHAR_UNDERLINE );
		if ( rTextLineColor.GetColor() != COL_TRANSPARENT )
			pOut->SetTextLineColor( rTextLineColor.GetColor() );
		else
			pOut->SetTextLineColor();
	}

	if ( pOut )
	{
		const SvxOverlineItem& rOverlineColor = (const SvxOverlineItem&)pNode->GetContentAttribs().GetItem( EE_CHAR_OVERLINE );
		if ( rOverlineColor.GetColor() != COL_TRANSPARENT )
			pOut->SetOverlineColor( rOverlineColor.GetColor() );
		else
			pOut->SetOverlineColor();
	}

	const SvxLanguageItem* pCJKLanguageItem = NULL;

	if ( aStatus.UseCharAttribs() )
	{
		const CharAttribArray& rAttribs = pNode->GetCharAttribs().GetAttribs();
		sal_uInt16 nAttr = 0;
		EditCharAttrib* pAttrib = GetAttrib( rAttribs, nAttr );
		while ( pAttrib && ( pAttrib->GetStart() <= nPos ) )
		{
			// Beim Seeken nicht die Attr beruecksichtigen, die dort beginnen!
			// Leere Attribute werden beruecksichtigt( verwendet), da diese
			// gerade eingestellt wurden.
			// 12.4.95: Doch keine Leeren Attribute verwenden:
			// - Wenn gerade eingestellt und leer => keine Auswirkung auf Font
			// In einem leeren Absatz eingestellte Zeichen werden sofort wirksam.
			if ( ( pAttrib->Which() != nIgnoreWhich ) &&
				 ( ( ( pAttrib->GetStart() < nPos ) && ( pAttrib->GetEnd() >= nPos ) )
					 || ( !pNode->Len() ) ) )
			{
				DBG_ASSERT( ( pAttrib->Which() >= EE_CHAR_START ) && ( pAttrib->Which() <= EE_FEATURE_END ), "Unglueltiges Attribut in Seek() " );
				if ( IsScriptItemValid( pAttrib->Which(), nScriptType ) )
                {
					pAttrib->SetFont( rFont, pOut );
                    // #i1550# hard color attrib should win over text color from field
                    if ( pAttrib->Which() == EE_FEATURE_FIELD )
                    {
                        EditCharAttrib* pColorAttr = pNode->GetCharAttribs().FindAttrib( EE_CHAR_COLOR, nPos );
                        if ( pColorAttr )
					        pColorAttr->SetFont( rFont, pOut );
                    }
                }
				if ( pAttrib->Which() == EE_CHAR_FONTWIDTH )
					nRelWidth = ((const SvxCharScaleWidthItem*)pAttrib->GetItem())->GetValue();
				if ( pAttrib->Which() == EE_CHAR_LANGUAGE_CJK )
					pCJKLanguageItem = (const SvxLanguageItem*) pAttrib->GetItem();
			}
			pAttrib = GetAttrib( rAttribs, ++nAttr );
		}
	}

	if ( !pCJKLanguageItem )
		pCJKLanguageItem = (const SvxLanguageItem*) &pNode->GetContentAttribs().GetItem( EE_CHAR_LANGUAGE_CJK );

	rFont.SetCJKContextLanguage( pCJKLanguageItem->GetLanguage() );

	if ( rFont.GetKerning() && IsKernAsianPunctuation() && ( nScriptType == i18n::ScriptType::ASIAN ) )
		rFont.SetKerning( rFont.GetKerning() | KERNING_ASIAN );

	if ( aStatus.DoNotUseColors() )
	{
		// Hack fuer DL,weil JOE staendig die Pooldefaults verbiegt!
		// const SvxColorItem& rColorItem = (const SvxColorItem&)aEditDoc.GetItemPool().GetDefaultItem( EE_CHAR_COLOR );
		rFont.SetColor( /* rColorItem.GetValue() */ COL_BLACK );
	}

	if ( aStatus.DoStretch() || ( nRelWidth != 100 ) )
	{
		// Fuer das aktuelle Ausgabegeraet, weil es sonst bei einem
		// Drucker als RefDev auf dem Bildschirm #?!@' aussieht!
		OutputDevice* pDev = pOut ? pOut : GetRefDevice();
		rFont.SetPhysFont( pDev );
		FontMetric aMetric( pDev->GetFontMetric() );
		// Fuer die Hoehe nicht die Metriken nehmen, da das bei
		// Hoch-/Tiefgestellt schief geht.
		Size aRealSz( aMetric.GetSize().Width(), rFont.GetSize().Height() );
		if ( aStatus.DoStretch() )
		{
			if ( nStretchY != 100 )
			{
				aRealSz.Height() *= nStretchY;
				aRealSz.Height() /= 100;
			}
			if ( nStretchX != 100 )
			{
				aRealSz.Width() *= nStretchX;
				aRealSz.Width() /= 100;

				// Auch das Kerning: (long wegen Zwischenergebnis)
				long nKerning = rFont.GetFixKerning();
/*
				Die Ueberlegung war: Wenn neg. Kerning, aber StretchX = 200
				=> Nicht das Kerning verdoppelt, also die Buchstaben weiter
				zusammenziehen
				---------------------------
				Kern	StretchX	=>Kern
				---------------------------
				 >0		<100		< (Proportional)
				 <0		<100		< (Proportional)
				 >0		>100		> (Proportional)
				 <0		>100		< (Der Betrag, also Antiprop)
*/
				if ( ( nKerning < 0  ) && ( nStretchX > 100 ) )
				{
					// Antiproportional
					nKerning *= 100;
					nKerning /= nStretchX;
				}
				else if ( nKerning )
				{
					// Proportional
					nKerning *= nStretchX;
					nKerning /= 100;
				}
				rFont.SetFixKerning( (short)nKerning );
			}
		}
		if ( nRelWidth != 100 )
		{
			aRealSz.Width() *= nRelWidth;
			aRealSz.Width() /= 100;
		}
		rFont.SetSize( aRealSz );
		// Font wird nicht restauriert...
	}

	if ( ( ( rFont.GetColor() == COL_AUTO ) || ( IsForceAutoColor() ) ) && pOut )
	{
        // #i75566# Do not use AutoColor when printing OR Pdf export
        const bool bPrinting(OUTDEV_PRINTER == pOut->GetOutDevType());
        const bool bPDFExporting(0 != pOut->GetPDFWriter());

        if ( IsAutoColorEnabled() && !bPrinting && !bPDFExporting)
        {
            // Never use WindowTextColor on the printer
	        rFont.SetColor( GetAutoColor() );
        }
        else
        {
	        if ( ( GetBackgroundColor() != COL_AUTO ) && GetBackgroundColor().IsDark() )
                rFont.SetColor( COL_WHITE );
            else
                rFont.SetColor( COL_BLACK );
        }
	}

	if ( mpIMEInfos && mpIMEInfos->pAttribs && ( mpIMEInfos->aPos.GetNode() == pNode ) &&
		( nPos > mpIMEInfos->aPos.GetIndex() ) && ( nPos <= ( mpIMEInfos->aPos.GetIndex() + mpIMEInfos->nLen ) ) )
	{
		sal_uInt16 nAttr = mpIMEInfos->pAttribs[ nPos - mpIMEInfos->aPos.GetIndex() - 1 ];
		if ( nAttr & EXTTEXTINPUT_ATTR_UNDERLINE )
			rFont.SetUnderline( UNDERLINE_SINGLE );
		else if ( nAttr & EXTTEXTINPUT_ATTR_BOLDUNDERLINE )
			rFont.SetUnderline( UNDERLINE_BOLD );
		else if ( nAttr & EXTTEXTINPUT_ATTR_DOTTEDUNDERLINE )
			rFont.SetUnderline( UNDERLINE_DOTTED );
		else if ( nAttr & EXTTEXTINPUT_ATTR_DASHDOTUNDERLINE )
			rFont.SetUnderline( UNDERLINE_DOTTED );
		else if ( nAttr & EXTTEXTINPUT_ATTR_REDTEXT )
			rFont.SetColor( Color( COL_RED ) );
		else if ( nAttr & EXTTEXTINPUT_ATTR_HALFTONETEXT )
			rFont.SetColor( Color( COL_LIGHTGRAY ) );
		if ( nAttr & EXTTEXTINPUT_ATTR_HIGHLIGHT )
		{
			const StyleSettings& rStyleSettings = Application::GetSettings().GetStyleSettings();
			rFont.SetColor( rStyleSettings.GetHighlightTextColor() );
			rFont.SetFillColor( rStyleSettings.GetHighlightColor() );
			rFont.SetTransparent( sal_False );
		}
		else if ( nAttr & EXTTEXTINPUT_ATTR_GRAYWAVELINE )
		{
			rFont.SetUnderline( UNDERLINE_WAVE );
			if( pOut )
				pOut->SetTextLineColor( Color( COL_LIGHTGRAY ) );
		}
	}
}

void ImpEditEngine::RecalcFormatterFontMetrics( FormatterFontMetric& rCurMetrics, SvxFont& rFont )
{
	// Fuer Zeilenhoehe bei Hoch/Tief erstmal ohne Propr!
	sal_uInt16 nPropr = rFont.GetPropr();
	DBG_ASSERT( ( nPropr == 100 ) || rFont.GetEscapement(), "Propr ohne Escape?!" );
	if ( nPropr != 100 )
	{
		rFont.SetPropr( 100 );
		rFont.SetPhysFont( pRefDev );
	}
	sal_uInt16 nAscent, nDescent;

	FontMetric aMetric( pRefDev->GetFontMetric() );
	nAscent = (sal_uInt16)aMetric.GetAscent();
	if ( IsAddExtLeading() )
		nAscent = sal::static_int_cast< sal_uInt16 >(
            nAscent + aMetric.GetExtLeading() );
	nDescent = (sal_uInt16)aMetric.GetDescent();

	if ( IsFixedCellHeight() )
	{
/*	creating correct proportional ascent and descent values lead to problems if different fonts are used
    in the same portion, it results in a bigger linespacing.
		sal_Int32 f = nAscent + nDescent;
		if ( f )
		{
			sal_Int32 nHeight = ImplCalculateFontIndependentLineSpacing( rFont.GetHeight() );
			nAscent  = (sal_Int16)(( nHeight * nAscent ) / f );
			nDescent = (sal_Int16)(nHeight - nAscent);
		}
*/
		nAscent = sal::static_int_cast< sal_uInt16 >( rFont.GetHeight() );
		nDescent= sal::static_int_cast< sal_uInt16 >( ImplCalculateFontIndependentLineSpacing( rFont.GetHeight() ) - nAscent );
	}
	else
	{
		sal_uInt16 nIntLeading = ( aMetric.GetIntLeading() > 0 ) ? (sal_uInt16)aMetric.GetIntLeading() : 0;
		// Fonts ohne Leading bereiten Probleme
		if ( ( nIntLeading == 0 ) && ( pRefDev->GetOutDevType() == OUTDEV_PRINTER ) )
		{
			// Da schaun wir mal, was fuer eine Leading ich auf dem
			// Bildschirm erhalte
			VirtualDevice* pVDev = GetVirtualDevice( pRefDev->GetMapMode(), pRefDev->GetDrawMode() );
			rFont.SetPhysFont( pVDev );
			aMetric = pVDev->GetFontMetric();

			// Damit sich die Leading nicht wieder rausrechnet,
			// wenn die ganze Zeile den Font hat, nTmpLeading.

			// 4/96: Kommt bei HP Laserjet 4V auch nicht hin
			// => Werte komplett vom Bildschirm holen.
	//		sal_uInt16 nTmpLeading = (sal_uInt16)aMetric.GetLeading();
	//		nAscent += nTmpLeading;
			nAscent = (sal_uInt16)aMetric.GetAscent();
			nDescent = (sal_uInt16)aMetric.GetDescent();
	//		nLeading = (sal_uInt16)aMetric.GetLeading();
		}
	}
	if ( nAscent > rCurMetrics.nMaxAscent )
		rCurMetrics.nMaxAscent = nAscent;
	if ( nDescent > rCurMetrics.nMaxDescent )
		rCurMetrics.nMaxDescent= nDescent;
	// Sonderbehandlung Hoch/Tief:
	if ( rFont.GetEscapement() )
	{
		// Jetzt unter Beruecksichtigung von Escape/Propr
		// Ascent oder Descent ggf vergroessern
		short nDiff = (short)(rFont.GetSize().Height()*rFont.GetEscapement()/100L);
		if ( rFont.GetEscapement() > 0 )
		{
			nAscent = (sal_uInt16) (((long)nAscent)*nPropr/100 + nDiff);
			if ( nAscent > rCurMetrics.nMaxAscent )
				rCurMetrics.nMaxAscent = nAscent;
		}
		else	// muss < 0 sein
		{
			nDescent = (sal_uInt16) (((long)nDescent)*nPropr/100 - nDiff);
			if ( nDescent > rCurMetrics.nMaxDescent )
				rCurMetrics.nMaxDescent= nDescent;
		}
	}
}

void ImpEditEngine::Paint( OutputDevice* pOutDev, Rectangle aClipRec, Point aStartPos, sal_Bool bStripOnly, short nOrientation )
{
	if ( !GetUpdateMode() && !bStripOnly )
		return;

	if ( !IsFormatted() )
		FormatDoc();

	long nFirstVisXPos = - pOutDev->GetMapMode().GetOrigin().X();
	long nFirstVisYPos = - pOutDev->GetMapMode().GetOrigin().Y();

	EditLine* pLine;
	Point aTmpPos;
    Point aRedLineTmpPos;
	DBG_ASSERT( GetParaPortions().Count(), "Keine ParaPortion?!" );
	SvxFont aTmpFont( GetParaPortions()[0]->GetNode()->GetCharAttribs().GetDefFont() );
	Font aOldFont( pOutDev->GetFont() );
	vcl::PDFExtOutDevData* pPDFExtOutDevData = PTR_CAST( vcl::PDFExtOutDevData, pOutDev->GetExtOutDevData() );

	// Bei gedrehtem Text wird aStartPos als TopLeft angesehen, da andere
	// Informationen fehlen, und sowieso das ganze Object ungescrollt
	// dargestellt wird.
	// Das Rechteck ist unendlich gross.
	Point aOrigin( aStartPos );
	double nCos = 0.0, nSin = 0.0;
	if ( nOrientation )
	{
		double nRealOrientation = nOrientation*F_PI1800;
		nCos = cos( nRealOrientation );
		nSin = sin( nRealOrientation );
	}

    // #110496# Added some more optional metafile comments. This
    // change: factored out some duplicated code.
    GDIMetaFile* pMtf = pOutDev->GetConnectMetaFile();
    const bool bMetafileValid( pMtf != NULL );

	// Fuer OnlineSpelling:
//	EditPaM aCursorPos;
//	if( GetStatus().DoOnlineSpelling() && pActiveView )
//		aCurPos = pActiveView->pImpEditView->GetEditSelections().Max();

	// --------------------------------------------------
	// Ueber alle Absaetze...
	// --------------------------------------------------
	for ( sal_uInt16 n = 0; n < GetParaPortions().Count(); n++ )
	{
		ParaPortion* pPortion = GetParaPortions().GetObject( n );
		DBG_ASSERT( pPortion, "NULL-Pointer in TokenList in Paint" );
		// falls beim Tippen Idle-Formatierung, asynchrones Paint.
		// Unsichtbare Portions koennen ungueltig sein.
		if ( pPortion->IsVisible() && pPortion->IsInvalid() )
			return;

		if ( pPDFExtOutDevData )
			pPDFExtOutDevData->BeginStructureElement( vcl::PDFWriter::Paragraph );

		long nParaHeight = pPortion->GetHeight();
		sal_uInt16 nIndex = 0;
		if ( pPortion->IsVisible() && (
				( !IsVertical() && ( ( aStartPos.Y() + nParaHeight ) > aClipRec.Top() ) ) ||
				( IsVertical() && ( ( aStartPos.X() - nParaHeight ) < aClipRec.Right() ) ) ) )

		{
			// --------------------------------------------------
			// Ueber die Zeilen des Absatzes...
			// --------------------------------------------------
			sal_uInt16 nLines = pPortion->GetLines().Count();
			sal_uInt16 nLastLine = nLines-1;

            // #108052#
            bool bEndOfParagraphWritten(false);

			if ( !IsVertical() )
				aStartPos.Y() += pPortion->GetFirstLineOffset();
			else
				aStartPos.X() -= pPortion->GetFirstLineOffset();

            Point aParaStart( aStartPos );

            const SvxLineSpacingItem& rLSItem = ((const SvxLineSpacingItem&)pPortion->GetNode()->GetContentAttribs().GetItem( EE_PARA_SBL ));
			sal_uInt16 nSBL = ( rLSItem.GetInterLineSpaceRule() == SVX_INTER_LINE_SPACE_FIX )
								? GetYValue( rLSItem.GetInterLineSpace() ) : 0;
            bool bPaintBullet (false);

            for ( sal_uInt16 nLine = 0; nLine < nLines; nLine++ )
			{
				pLine = pPortion->GetLines().GetObject(nLine);
				DBG_ASSERT( pLine, "NULL-Pointer im Zeileniterator in UpdateViews" );
				aTmpPos = aStartPos;
				if ( !IsVertical() )
				{
					aTmpPos.X() += pLine->GetStartPosX();
					aTmpPos.Y() += pLine->GetMaxAscent();
					aStartPos.Y() += pLine->GetHeight();
				}
				else
				{
					aTmpPos.Y() += pLine->GetStartPosX();
					aTmpPos.X() -= pLine->GetMaxAscent();
					aStartPos.X() -= pLine->GetHeight();
				}

				if ( ( !IsVertical() && ( aStartPos.Y() > aClipRec.Top() ) )
					|| ( IsVertical() && aStartPos.X() < aClipRec.Right() ) )
				{
                    bPaintBullet = false;

                    // Why not just also call when stripping portions? This will give the correct values
                    // and needs no position corrections in OutlinerEditEng::DrawingText which tries to call
                    // PaintBullet correctly; exactly what GetEditEnginePtr()->PaintingFirstLine
                    // does, too. No change for not-layouting (painting).
					if(0 == nLine) // && !bStripOnly)
					{
						// VERT???
						GetEditEnginePtr()->PaintingFirstLine( n, aParaStart, aTmpPos.Y(), aOrigin, nOrientation, pOutDev );

                        // Remember whether a bullet was painted.
                        const SfxBoolItem& rBulletState = static_cast<const SfxBoolItem&>(
                            pEditEngine->GetParaAttrib(n, EE_PARA_BULLETSTATE));
                        bPaintBullet = rBulletState.GetValue() ? true : false;
					}

                    // --------------------------------------------------
					// Ueber die Portions der Zeile...
					// --------------------------------------------------
					nIndex = pLine->GetStart();

					for ( sal_uInt16 y = pLine->GetStartPortion(); y <= pLine->GetEndPortion(); y++ )
					{
						DBG_ASSERT( pPortion->GetTextPortions().Count(), "Zeile ohne Textportion im Paint!" );
						TextPortion* pTextPortion = pPortion->GetTextPortions().GetObject( y );
						DBG_ASSERT( pTextPortion, "NULL-Pointer im Portioniterator in UpdateViews" );

                        long nPortionXOffset = GetPortionXOffset( pPortion, pLine, y );
				        if ( !IsVertical() )
                        {
					        aTmpPos.X() = aStartPos.X() + nPortionXOffset;
                            if ( aTmpPos.X() > aClipRec.Right() )
                                break;	// Keine weitere Ausgabe in Zeile noetig
                        }
				        else
                        {
					        aTmpPos.Y() = aStartPos.Y() + nPortionXOffset;
                            if ( aTmpPos.Y() > aClipRec.Bottom() )
                                break;	// Keine weitere Ausgabe in Zeile noetig
                        }

                        // R2L replaces with obove...
                        // New position after processing R2L text...
// R2L                        if ( nR2LWidth && !pTextPortion->GetRightToLeft() )
// R2L                        {
// R2L							if ( !IsVertical() )
// R2L								aTmpPos.X() += nR2LWidth;
// R2L							else
// R2L								aTmpPos.Y() += nR2LWidth;
// R2L
// R2L                            nR2LWidth = 0;
// R2L                        }

						switch ( pTextPortion->GetKind() )
						{
							case PORTIONKIND_TEXT:
							case PORTIONKIND_FIELD:
							case PORTIONKIND_HYPHENATOR:
                            {
								SeekCursor( pPortion->GetNode(), nIndex+1, aTmpFont, pOutDev );

                                sal_Bool bDrawFrame = sal_False;

                                if ( ( pTextPortion->GetKind() == PORTIONKIND_FIELD ) && !aTmpFont.IsTransparent() &&
                                     ( GetBackgroundColor() != COL_AUTO ) && GetBackgroundColor().IsDark() &&
                                     ( IsAutoColorEnabled() && ( pOutDev->GetOutDevType() != OUTDEV_PRINTER ) ) )
	                            {
                                    aTmpFont.SetTransparent( sal_True );
                                    pOutDev->SetFillColor();
                                    pOutDev->SetLineColor( GetAutoColor() );
                                    bDrawFrame = sal_True;
	                            }

#ifdef EDITDEBUG
								if ( pTextPortion->GetKind() == PORTIONKIND_HYPHENATOR )
								{
									aTmpFont.SetFillColor( COL_LIGHTGRAY );
									aTmpFont.SetTransparent( sal_False );
								}
                                if ( pTextPortion->GetRightToLeft()  )
								{
									aTmpFont.SetFillColor( COL_LIGHTGRAY );
									aTmpFont.SetTransparent( sal_False );
								}
                                else if ( GetScriptType( EditPaM( pPortion->GetNode(), nIndex+1 ) ) == i18n::ScriptType::COMPLEX )
								{
									aTmpFont.SetFillColor( COL_LIGHTCYAN );
									aTmpFont.SetTransparent( sal_False );
								}
#endif
								aTmpFont.SetPhysFont( pOutDev );

                                // #114278# Saving both layout mode and language (since I'm
                                // potentially changing both)
                                pOutDev->Push( PUSH_TEXTLAYOUTMODE|PUSH_TEXTLANGUAGE );
                                ImplInitLayoutMode( pOutDev, n, nIndex );
                                ImplInitDigitMode( pOutDev, 0, 0, 0, aTmpFont.GetLanguage() );

								XubString aText;
                                sal_uInt16 nTextStart = 0;
                                sal_uInt16 nTextLen = 0;
								const sal_Int32* pDXArray = 0;
								sal_Int32* pTmpDXArray = 0;

                                if ( pTextPortion->GetKind() == PORTIONKIND_TEXT )
								{
                                    aText = *pPortion->GetNode();
                                    nTextStart = nIndex;
                                    nTextLen = pTextPortion->GetLen();
									pDXArray = pLine->GetCharPosArray().GetData()+( nIndex-pLine->GetStart() );

                                    // --> FME 2005-10-18 #i55716# Paint control characters
                                    if ( aStatus.MarkFields() )
                                    {
                                        xub_StrLen nTmpIdx;
                                        const xub_StrLen nTmpEnd = nTextStart + pTextPortion->GetLen();

                                        for ( nTmpIdx = nTextStart; nTmpIdx <= nTmpEnd ; ++nTmpIdx )
                                        {
                                            const sal_Unicode cChar = ( nTmpIdx != aText.Len() && ( nTmpIdx != nTextStart || 0 == nTextStart ) ) ?
                                                                        aText.GetChar( nTmpIdx ) :
                                                                        0;

                                            if ( 0x200B == cChar || 0x2060 == cChar )
                                            {
                                                const String aBlank( ' ' );
                                                long nHalfBlankWidth = aTmpFont.QuickGetTextSize( pOutDev, aBlank, 0, 1, 0 ).Width() / 2;

                                                const long nAdvanceX = ( nTmpIdx == nTmpEnd ?
                                                                         pTextPortion->GetSize().Width() :
                                                                         pDXArray[ nTmpIdx - nTextStart ] ) - nHalfBlankWidth;
                                                const long nAdvanceY = -pLine->GetMaxAscent();

                                                Point aTopLeftRectPos( aTmpPos );
                                                if ( !IsVertical() )
                                                {
                                                    aTopLeftRectPos.X() += nAdvanceX;
                                                    aTopLeftRectPos.Y() += nAdvanceY;
                                                }
                                                else
                                                {
                                                    aTopLeftRectPos.Y() += nAdvanceX;
                                                    aTopLeftRectPos.X() -= nAdvanceY;
                                                }

                                                Point aBottomRightRectPos( aTopLeftRectPos );
                                                if ( !IsVertical() )
                                                {
                                                    aBottomRightRectPos.X() += 2 * nHalfBlankWidth;
                                                    aBottomRightRectPos.Y() += pLine->GetHeight();
                                                }
                                                else
                                                {
                                                    aBottomRightRectPos.X() -= pLine->GetHeight();
                                                    aBottomRightRectPos.Y() += 2 * nHalfBlankWidth;
                                                }

                                                pOutDev->Push( PUSH_FILLCOLOR );
                                                pOutDev->Push( PUSH_LINECOLOR );
                                                pOutDev->SetFillColor( COL_LIGHTGRAY );
                                                pOutDev->SetLineColor( COL_LIGHTGRAY );

                                                const Rectangle aBackRect( aTopLeftRectPos, aBottomRightRectPos );
                                                pOutDev->DrawRect( aBackRect );

                                                pOutDev->Pop();
                                                pOutDev->Pop();

                                                if ( 0x200B == cChar )
                                                {
                                                    const String aSlash( '/' );
                                                    const short nOldEscapement = aTmpFont.GetEscapement();
                                                    const sal_uInt8 nOldPropr = aTmpFont.GetPropr();

                                                    aTmpFont.SetEscapement( -20 );
                                                    aTmpFont.SetPropr( 25 );
                                                    aTmpFont.SetPhysFont( pOutDev );

                                                    const Size aSlashSize = aTmpFont.QuickGetTextSize( pOutDev, aSlash, 0, 1, 0 );
                                                    Point aSlashPos( aTmpPos );
                                                    const long nAddX = nHalfBlankWidth - aSlashSize.Width() / 2;
                                                    if ( !IsVertical() )
                                                    {
                                                        aSlashPos.X() = aTopLeftRectPos.X() + nAddX;
                                                    }
                                                    else
                                                    {
                                                        aSlashPos.Y() = aTopLeftRectPos.Y() + nAddX;
                                                    }

                                                    aTmpFont.QuickDrawText( pOutDev, aSlashPos, aSlash, 0, 1, 0 );

                                                    aTmpFont.SetEscapement( nOldEscapement );
                                                    aTmpFont.SetPropr( nOldPropr );
                                                    aTmpFont.SetPhysFont( pOutDev );
                                                }
                                            }
                                        }
                                    }
                                    // <--
                                }
                                else if ( pTextPortion->GetKind() == PORTIONKIND_FIELD )
								{
									EditCharAttrib* pAttr = pPortion->GetNode()->GetCharAttribs().FindFeature( nIndex );
									DBG_ASSERT( pAttr, "Feld nicht gefunden" );
									DBG_ASSERT( pAttr && pAttr->GetItem()->ISA( SvxFieldItem ), "Feld vom falschen Typ!" );
									aText = ((EditCharAttribField*)pAttr)->GetFieldValue();
                                    nTextStart = 0;
                                    nTextLen = aText.Len();

									pTmpDXArray = new sal_Int32[ aText.Len() ];
									pDXArray = pTmpDXArray;
									Font _aOldFont( GetRefDevice()->GetFont() );
									aTmpFont.SetPhysFont( GetRefDevice() );
									aTmpFont.QuickGetTextSize( GetRefDevice(), aText, 0, aText.Len(), pTmpDXArray );
									if ( aStatus.DoRestoreFont() )
										GetRefDevice()->SetFont( _aOldFont );

									// add a meta file comment if we record to a metafile
								    if( bMetafileValid )
									{
										SvxFieldItem* pFieldItem = PTR_CAST( SvxFieldItem, pAttr->GetItem() );
										if( pFieldItem )
										{
											const SvxFieldData* pFieldData = pFieldItem->GetField();
											if( pFieldData )
												pMtf->AddAction( pFieldData->createBeginComment() );
										}
									}

								}
								else if ( pTextPortion->GetKind() == PORTIONKIND_HYPHENATOR )
								{
									if ( pTextPortion->GetExtraValue() )
										aText = pTextPortion->GetExtraValue();
									aText += CH_HYPH;
                                    nTextStart = 0;
                                    nTextLen = aText.Len();

									// #b6668980# crash when accessing 0 pointer in pDXArray
									pTmpDXArray = new sal_Int32[ aText.Len() ];
									pDXArray = pTmpDXArray;
									Font _aOldFont( GetRefDevice()->GetFont() );
									aTmpFont.SetPhysFont( GetRefDevice() );
									aTmpFont.QuickGetTextSize( GetRefDevice(), aText, 0, aText.Len(), pTmpDXArray );
									if ( aStatus.DoRestoreFont() )
										GetRefDevice()->SetFont( _aOldFont );
								}

								long nTxtWidth = pTextPortion->GetSize().Width();

							    Point aOutPos( aTmpPos );
                                aRedLineTmpPos = aTmpPos;
                                // In RTL portions spell markup pos should be at the start of the
                                // first chara as well. That is on the right end of the portion
                                if (pTextPortion->IsRightToLeft())
                                    aRedLineTmpPos.X() += pTextPortion->GetSize().Width();

//L2R                                if ( pTextPortion->GetRightToLeft() )
//L2R                                {
//L2R                                    sal_uInt16 nNextPortion = y+1;
//L2R                                    while ( nNextPortion <= pLine->GetEndPortion() )
//L2R                                    {
//L2R						                TextPortion* pNextTextPortion = pPortion->GetTextPortions().GetObject( nNextPortion );
//L2R                                        if ( pNextTextPortion->GetRightToLeft() )
//L2R                                        {
//L2R			                                if ( !IsVertical() )
//L2R                                                aOutPos.X() += pNextTextPortion->GetSize().Width();
//L2R			                                else
//L2R                                                aOutPos.Y() += pNextTextPortion->GetSize().Width();
//L2R                                        }
//L2R                                        else
//L2R                                            break;
//L2R                                        nNextPortion++;
//L2R                                    }
//L2R                                }
								if ( bStripOnly )
								{
                                    EEngineData::WrongSpellVector aWrongSpellVector;

                                    if(GetStatus().DoOnlineSpelling() && pTextPortion->GetLen())
                                    {
                                        WrongList* pWrongs = pPortion->GetNode()->GetWrongList();

                                        if(pWrongs && pWrongs->HasWrongs())
                                        {
                                    		sal_uInt16 nStart(nIndex);
                                    		sal_uInt16 nEnd(0);
                                    		sal_Bool bWrong(pWrongs->NextWrong(nStart, nEnd));
                                            const sal_uInt16 nMaxEnd(nIndex + pTextPortion->GetLen());

                                            while(bWrong)
                                            {
			                                    if(nStart >= nMaxEnd)
                                                {
				                                    break;
                                                }

			                                    if(nStart < nIndex)
                                                {
				                                    nStart = nIndex;
                                                }

			                                    if(nEnd > nMaxEnd)
                                                {
				                                    nEnd = nMaxEnd;
                                                }

                                                // add to vector
                                                aWrongSpellVector.push_back(EEngineData::WrongSpellClass(nStart, nEnd));

                                                // goto next index
			                                    nStart = nEnd + 1;

                                                if(nEnd < nMaxEnd)
                                                {
				                                    bWrong = pWrongs->NextWrong(nStart, nEnd);
                                                }
			                                    else
                                                {
				                                    bWrong = sal_False;
                                                }
                                            }
                                        }
                                    }

                                    const SvxFieldData* pFieldData = 0;

									if(PORTIONKIND_FIELD == pTextPortion->GetKind())
									{
										EditCharAttrib* pAttr = pPortion->GetNode()->GetCharAttribs().FindFeature(nIndex);
										SvxFieldItem* pFieldItem = PTR_CAST(SvxFieldItem, pAttr->GetItem());

                                        if(pFieldItem)
										{
											pFieldData = pFieldItem->GetField();
                                        }
                                    }

                                    // support for EOC, EOW, EOS TEXT comments. To support that,
                                    // the locale is needed. With the locale and a XBreakIterator it is
                                    // possible to re-create the text marking info on primitive level
                                    const lang::Locale aLocale(GetLocale(EditPaM(pPortion->GetNode(), nIndex + 1)));

									// create EOL and EOP bools
									const bool bEndOfLine(y == pLine->GetEndPortion());
									const bool bEndOfParagraph(bEndOfLine && nLine + 1 == nLines);

                                    // get Overline color (from ((const SvxOverlineItem*)GetItem())->GetColor() in
                                    // consequence, but also already set at pOutDev)
                                    const Color aOverlineColor(pOutDev->GetOverlineColor());

                                    // get TextLine color (from ((const SvxUnderlineItem*)GetItem())->GetColor() in
                                    // consequence, but also already set at pOutDev)
                                    const Color aTextLineColor(pOutDev->GetTextLineColor());

									// Unicode code points conversion according to ctl text numeral setting
                                    ImplInitDigitMode( 0, &aText, nTextStart, nTextLen, aTmpFont.GetLanguage() );

									// StripPortions() data callback
                                    GetEditEnginePtr()->DrawingText( aOutPos, aText, nTextStart, nTextLen, pDXArray,
                                        aTmpFont, n, nIndex, pTextPortion->GetRightToLeft(),
                                        aWrongSpellVector.size() ? &aWrongSpellVector : 0,
                                        pFieldData,
                                        bEndOfLine, bEndOfParagraph, false, // support for EOL/EOP TEXT comments
                                        &aLocale,
                                        aOverlineColor,
                                        aTextLineColor);
                                    
                                    // #108052# remember that EOP is written already for this ParaPortion
                                    if(bEndOfParagraph)
                                    {
                                        bEndOfParagraphWritten = true;
                                    }
								}
								else
								{
									short nEsc = aTmpFont.GetEscapement();
									if ( nOrientation )
									{
										// Bei Hoch/Tief selbst Hand anlegen:
										if ( aTmpFont.GetEscapement() )
										{
                                            long nDiff = aTmpFont.GetSize().Height() * aTmpFont.GetEscapement() / 100L;
                                            if ( !IsVertical() )
											    aOutPos.Y() -= nDiff;
                                            else
                                                aOutPos.X() += nDiff;
                                            aRedLineTmpPos = aOutPos;
											aTmpFont.SetEscapement( 0 );
										}

                                        aOutPos = lcl_ImplCalcRotatedPos( aOutPos, aOrigin, nSin, nCos );
										aTmpFont.SetOrientation( aTmpFont.GetOrientation()+nOrientation );
										aTmpFont.SetPhysFont( pOutDev );

									}
									// nur ausgeben, was im sichtbaren Bereich beginnt:
									// Wichtig, weil Bug bei einigen Grafikkarten bei transparentem Font, Ausgabe bei neg.
									if ( nOrientation || ( !IsVertical() && ( ( aTmpPos.X() + nTxtWidth ) >= nFirstVisXPos ) )
											|| ( IsVertical() && ( ( aTmpPos.Y() + nTxtWidth ) >= nFirstVisYPos ) ) )
									{
										if ( nEsc && ( ( aTmpFont.GetUnderline() != UNDERLINE_NONE ) ) )
										{
											// Das Hoch/Tief ohne Underline malen, das Underline
											// auf der BaseLine der Original-Fonthoehe ausgeben...

											// Aber nur, wenn davor auch Unterstrichen!
											sal_Bool bSpecialUnderline = sal_False;
											EditCharAttrib* pPrev = pPortion->GetNode()->GetCharAttribs().FindAttrib( EE_CHAR_ESCAPEMENT, nIndex );
											if ( pPrev )
											{
												SvxFont aDummy;
												// Unterstreichung davor?
												if ( pPrev->GetStart() )
												{
													SeekCursor( pPortion->GetNode(), pPrev->GetStart(), aDummy );
													if ( aDummy.GetUnderline() != UNDERLINE_NONE )
														bSpecialUnderline = sal_True;
												}
												if ( !bSpecialUnderline && ( pPrev->GetEnd() < pPortion->GetNode()->Len() ) )
												{
													SeekCursor( pPortion->GetNode(), pPrev->GetEnd()+1, aDummy );
													if ( aDummy.GetUnderline() != UNDERLINE_NONE )
														bSpecialUnderline = sal_True;
												}
											}
											if ( bSpecialUnderline )
											{
												Size aSz = aTmpFont.GetPhysTxtSize( pOutDev, aText, nTextStart, nTextLen );
												sal_uInt8 nProp = aTmpFont.GetPropr();
												aTmpFont.SetEscapement( 0 );
												aTmpFont.SetPropr( 100 );
												aTmpFont.SetPhysFont( pOutDev );
												String aBlanks;
												aBlanks.Fill( nTextLen, ' ' );
												Point aUnderlinePos( aOutPos );
												if ( nOrientation )
													aUnderlinePos = lcl_ImplCalcRotatedPos( aTmpPos, aOrigin, nSin, nCos );
												pOutDev->DrawStretchText( aUnderlinePos, aSz.Width(), aBlanks, 0, nTextLen );

												aTmpFont.SetUnderline( UNDERLINE_NONE );
												if ( !nOrientation )
													aTmpFont.SetEscapement( nEsc );
												aTmpFont.SetPropr( nProp );
												aTmpFont.SetPhysFont( pOutDev );
											}
										}
                                        Point aRealOutPos( aOutPos );
                                        if ( ( pTextPortion->GetKind() == PORTIONKIND_TEXT )
                                               && pTextPortion->GetExtraInfos() && pTextPortion->GetExtraInfos()->bCompressed
                                               && pTextPortion->GetExtraInfos()->bFirstCharIsRightPunktuation )
                                        {
                                            aRealOutPos.X() += pTextPortion->GetExtraInfos()->nPortionOffsetX;
                                        }

                                        // --> FME 2005-06-17 #i37132# RTL portions with
                                        // compressed blank should not paint this blank:
                                        if ( pTextPortion->IsRightToLeft() && nTextLen >= 2 &&
                                             pDXArray[ nTextLen - 1 ] ==
                                             pDXArray[ nTextLen - 2 ] &&
                                             ' ' == aText.GetChar( nTextStart + nTextLen - 1 ) )
                                            --nTextLen;
                                        // <--

                                        // output directly
                                        aTmpFont.QuickDrawText( pOutDev, aRealOutPos, aText, nTextStart, nTextLen, pDXArray );

                                        if ( bDrawFrame )
                                        {
                                            Point aTopLeft( aTmpPos );
                                            aTopLeft.Y() -= pLine->GetMaxAscent();
									        if ( nOrientation )
                                                aTopLeft = lcl_ImplCalcRotatedPos( aTopLeft, aOrigin, nSin, nCos );
                                            Rectangle aRect( aTopLeft, pTextPortion->GetSize() );
                                            pOutDev->DrawRect( aRect );
                                        }


										// PDF export:
										if ( pPDFExtOutDevData )
										{
											if ( pTextPortion->GetKind() == PORTIONKIND_FIELD )
											{
												EditCharAttrib* pAttr = pPortion->GetNode()->GetCharAttribs().FindFeature( nIndex );
												SvxFieldItem* pFieldItem = PTR_CAST( SvxFieldItem, pAttr->GetItem() );
												if( pFieldItem )
												{
													const SvxFieldData* pFieldData = pFieldItem->GetField();
													if ( pFieldData->ISA( SvxURLField ) )
													{
														Point aTopLeft( aTmpPos );
														aTopLeft.Y() -= pLine->GetMaxAscent();
//														if ( nOrientation )
//					                                        aTopLeft = lcl_ImplCalcRotatedPos( aTopLeft, aOrigin, nSin, nCos );

														Rectangle aRect( aTopLeft, pTextPortion->GetSize() );
														vcl::PDFExtOutDevBookmarkEntry aBookmark;
														aBookmark.nLinkId = pPDFExtOutDevData->CreateLink( aRect );
														aBookmark.aBookmark = ((SvxURLField*)pFieldData)->GetURL();
														std::vector< vcl::PDFExtOutDevBookmarkEntry >& rBookmarks = pPDFExtOutDevData->GetBookmarks();
														rBookmarks.push_back( aBookmark );
													}
												}
											}
										}

										// comment




									}

#ifndef SVX_LIGHT
                                    if ( GetStatus().DoOnlineSpelling() && pPortion->GetNode()->GetWrongList()->HasWrongs() && pTextPortion->GetLen() )
									{
                                        {//#105750# adjust LinePos for superscript or subscript text
                                            short _nEsc = aTmpFont.GetEscapement();
                                            if( _nEsc )
                                            {
                                                long nShift = ((_nEsc*long(aTmpFont.GetSize().Height()))/ 100L);
                                                if( !IsVertical() )
                                                    aRedLineTmpPos.Y() -= nShift;
                                                else
                                                    aRedLineTmpPos.X() += nShift;
                                            }
                                        }
			                            Color aOldColor( pOutDev->GetLineColor() );
                                        pOutDev->SetLineColor( Color( GetColorConfig().GetColorValue( svtools::SPELL ).nColor ) );
										lcl_DrawRedLines( pOutDev, aTmpFont.GetSize().Height(), aRedLineTmpPos, nIndex, nIndex + pTextPortion->GetLen(), pDXArray, pPortion->GetNode()->GetWrongList(), nOrientation, aOrigin, IsVertical(), pTextPortion->IsRightToLeft() );
			                            pOutDev->SetLineColor( aOldColor );
									}
#endif // !SVX_LIGHT
								}

                                pOutDev->Pop();

                                if ( pTmpDXArray )
									delete[] pTmpDXArray;

// R2L                                if ( !pTextPortion->GetRightToLeft() )
// R2L                                {
// R2L								    if ( !IsVertical() )
// R2L									    aTmpPos.X() += nTxtWidth;
// R2L								    else
// R2L									    aTmpPos.Y() += nTxtWidth;
// R2L                                }
// R2L                                else
// R2L                                {
// R2L                                    nR2LWidth += nTxtWidth;
// R2L                                }

								if ( pTextPortion->GetKind() == PORTIONKIND_FIELD )
								{
									EditCharAttrib* pAttr = pPortion->GetNode()->GetCharAttribs().FindFeature( nIndex );
									DBG_ASSERT( pAttr, "Feld nicht gefunden" );
									DBG_ASSERT( pAttr && pAttr->GetItem()->ISA( SvxFieldItem ), "Feld vom falschen Typ!" );

									// add a meta file comment if we record to a metafile
								    if( bMetafileValid )
									{
										SvxFieldItem* pFieldItem = PTR_CAST( SvxFieldItem, pAttr->GetItem() );

										if( pFieldItem )
										{
											const SvxFieldData* pFieldData = pFieldItem->GetField();
											if( pFieldData )
												pMtf->AddAction( pFieldData->createEndComment() );
										}
									}

								}

							}
							break;
//							case PORTIONKIND_EXTRASPACE:
							case PORTIONKIND_TAB:
							{
								if ( pTextPortion->GetExtraValue() && ( pTextPortion->GetExtraValue() != ' ' ) )
								{
                                    SeekCursor( pPortion->GetNode(), nIndex+1, aTmpFont, pOutDev );
									aTmpFont.SetTransparent( sal_False );
									aTmpFont.SetEscapement( 0 );
									aTmpFont.SetPhysFont( pOutDev );
									long nCharWidth = aTmpFont.QuickGetTextSize( pOutDev, pTextPortion->GetExtraValue(), 0, 1, NULL ).Width();
									long nChars = 2;
									if( nCharWidth )
										nChars = pTextPortion->GetSize().Width() / nCharWidth;
									if ( nChars < 2 )
										nChars = 2;	// wird durch DrawStretchText gestaucht.
									else if ( nChars == 2 )
										nChars = 3;	// sieht besser aus

									String aText;
									aText.Fill( (sal_uInt16)nChars, pTextPortion->GetExtraValue() );

                                    if(bStripOnly)
                                    {
                                        // #71056# when converting to primitives, visualized TAB spaces need to be
                                        // visualized. Add tab#ed text here. Alternatively a primitive especially
                                        // representing this space and the single fill character would be possible, too.
                                        // For now, use what we have (the DrawingText callback)
                                        const lang::Locale aLocale(GetLocale(EditPaM(pPortion->GetNode(),nIndex + 1)));

                                        // get Overline color (from ((const SvxOverlineItem*)GetItem())->GetColor() in
                                        // consequence, but also already set at pOutDev)
                                        const Color aOverlineColor(pOutDev->GetOverlineColor());

                                        // get TextLine color (from ((const SvxUnderlineItem*)GetItem())->GetColor() in
                                        // consequence, but also already set at pOutDev)
                                        const Color aTextLineColor(pOutDev->GetTextLineColor());

                                        // get AllWidth and together with nCharWidth create DXArray using per character
                                        // difference
                                        const sal_Int32 nAllWidth(pTextPortion->GetSize().Width());
                                        const double fSingleCharDiff((double(nAllWidth) / double(nChars)) - double(nCharWidth));
                                        sal_Int32* pTmpDXArray = 0;

                                        if(fSingleCharDiff > 1.0)
                                        {
                                            // if more than one unit per character, create DXArray to create
                                            // something adequate to StretchText
                                            const double fAdvance(nCharWidth + fSingleCharDiff);
                                            const sal_uInt32 nCount(static_cast< sal_uInt32 >(nChars));
                                            pTmpDXArray = new sal_Int32[nCount];
                                            double fPos(0.0);

                                            for(sal_uInt32 a(0); a < nCount; a++)
                                            {
                                                fPos += fAdvance;
                                                pTmpDXArray[a] = basegfx::fround(fPos);
                                            }
                                        }

                                        // StripPortions() data callback
                                        GetEditEnginePtr()->DrawingText(
                                            aTmpPos,
                                            aText,
                                            0,
                                            nChars,
                                            pTmpDXArray,
                                            aTmpFont,
                                            n,
                                            nIndex,
                                            pTextPortion->GetRightToLeft(),
                                            0,
                                            0,
                                            false,
                                            false,
                                            false, // support for EOL/EOP TEXT comments
                                            &aLocale,
                                            aOverlineColor,
                                            aTextLineColor);

                                        if(pTmpDXArray)
                                        {
                                            delete pTmpDXArray;
                                        }
                                    }
                                    else
                                    {
                                        pOutDev->DrawStretchText(aTmpPos,pTextPortion->GetSize().Width(),aText);
                                    }
								}
							}
							break;
						}
						nIndex = nIndex + pTextPortion->GetLen();
					}
				}

				if ( ( nLine != nLastLine ) && !aStatus.IsOutliner() )
				{
					if ( !IsVertical() )
						aStartPos.Y() += nSBL;
					else
						aStartPos.X() -= nSBL;
				}

				// keine sichtbaren Aktionen mehr?
				if ( !IsVertical() && ( aStartPos.Y() >= aClipRec.Bottom() ) )
					break;
				else if ( IsVertical() && ( aStartPos.X() <= aClipRec.Left() ) )
					break;
			}

			if ( !aStatus.IsOutliner() )
			{
				const SvxULSpaceItem& rULItem = (const SvxULSpaceItem&)pPortion->GetNode()->GetContentAttribs().GetItem( EE_PARA_ULSPACE );
				long nUL = GetYValue( rULItem.GetLower() );
				if ( !IsVertical() )
					aStartPos.Y() += nUL;
				else
					aStartPos.X() -= nUL;
			}

            // #108052# Safer way for #i108052# and #i118881#: If for the current ParaPortion
            // EOP is not written, do it now. This will be safer than before. It has shown
            // that the reason for #i108052# was fixed/removed again, so this is a try to fix
            // the number of paragraphs (and counting empty ones) now independent from the
            // changes in EditEngine behaviour.
            if(!bEndOfParagraphWritten && !bPaintBullet && bStripOnly)
            {
                const Color aOverlineColor(pOutDev->GetOverlineColor());
                const Color aTextLineColor(pOutDev->GetTextLineColor());
                        
				GetEditEnginePtr()->DrawingText( 
					aTmpPos, String(), 0, 0, 0,
                    aTmpFont, n, nIndex, 0,
                    0,
                    0,
                    false, true, false, // support for EOL/EOP TEXT comments
                    0,
                    aOverlineColor,
                    aTextLineColor);
            }
		}
		else
		{
			if ( !IsVertical() )
				aStartPos.Y() += nParaHeight;
			else
				aStartPos.X() -= nParaHeight;
		}

        if ( pPDFExtOutDevData )
            pPDFExtOutDevData->EndStructureElement();

		// keine sichtbaren Aktionen mehr?
		if ( !IsVertical() && ( aStartPos.Y() > aClipRec.Bottom() ) )
			break;
		if ( IsVertical() && ( aStartPos.X() < aClipRec.Left() ) )
			break;
	}
	if ( aStatus.DoRestoreFont() )
		pOutDev->SetFont( aOldFont );
}

void ImpEditEngine::Paint( ImpEditView* pView, const Rectangle& rRec, OutputDevice* pTargetDevice, sal_Bool bUseVirtDev )
{
	DBG_ASSERT( pView, "Keine View - Kein Paint!" );
	DBG_CHKOBJ( GetEditEnginePtr(), EditEngine, 0 );

	if ( !GetUpdateMode() || IsInUndo() )
		return;

	// Schnittmenge aus Paintbereich und OutputArea.
	Rectangle aClipRec( pView->GetOutputArea() );
	aClipRec.Intersection( rRec );

	OutputDevice* pTarget = pTargetDevice ? pTargetDevice : pView->GetWindow();

	if ( bUseVirtDev )
	{
		Rectangle aClipRecPixel( pTarget->LogicToPixel( aClipRec ) );
		if ( !IsVertical() )
		{
			// etwas mehr, falls abgerundet!
			aClipRecPixel.Right() += 1;
			aClipRecPixel.Bottom() += 1;
		}
		else
		{
			aClipRecPixel.Left() -= 1;
			aClipRecPixel.Bottom() += 1;
		}

		// Wenn aClipRecPixel > XXXX, dann invalidieren ?!

		VirtualDevice* pVDev = GetVirtualDevice( pTarget->GetMapMode(), pTarget->GetDrawMode() );
		pVDev->SetDigitLanguage( GetRefDevice()->GetDigitLanguage() );

		{
			Color aBackgroundColor( pView->GetBackgroundColor() );
			// #i47161# Check if text is visible on background
			SvxFont aTmpFont;
			ContentNode* pNode = GetEditDoc().SaveGetObject( 0 );
			SeekCursor( pNode, 1, aTmpFont );
			Color aFontColor( aTmpFont.GetColor() );
			if( (aFontColor == COL_AUTO) || IsForceAutoColor() )
				aFontColor = GetAutoColor();

            // #i69346# check for reverse color of input method attribute
            if( mpIMEInfos && (mpIMEInfos->aPos.GetNode() == pNode &&
                mpIMEInfos->pAttribs))
            {
                sal_uInt16 nAttr = mpIMEInfos->pAttribs[ 0 ];
                if ( nAttr & EXTTEXTINPUT_ATTR_HIGHLIGHT )
                {
                    const StyleSettings& rStyleSettings = Application::GetSettings().GetStyleSettings();
                    aFontColor = rStyleSettings.GetHighlightColor() ;
                }
            }

            sal_uInt8 nColorDiff = aFontColor.GetColorError( aBackgroundColor );
			if( nColorDiff < 8 )
				aBackgroundColor = aFontColor.IsDark() ? COL_WHITE : COL_BLACK;
			pVDev->SetBackground( aBackgroundColor );
		}

		sal_Bool bVDevValid = sal_True;
		Size aOutSz( pVDev->GetOutputSizePixel() );
		if ( (	aOutSz.Width() < aClipRecPixel.GetWidth() ) ||
			 (	aOutSz.Height() < aClipRecPixel.GetHeight() ) )
		{
			bVDevValid = pVDev->SetOutputSizePixel( aClipRecPixel.GetSize() );
		}
		else
		{
			// Das VirtDev kann bei einem Resize sehr gross werden =>
			// irgendwann mal kleiner machen!
			if ( ( aOutSz.Height() > ( aClipRecPixel.GetHeight() + RESDIFF ) ) ||
				 ( aOutSz.Width() > ( aClipRecPixel.GetWidth() + RESDIFF ) ) )
			{
				bVDevValid = pVDev->SetOutputSizePixel( aClipRecPixel.GetSize() );
			}
			else
			{
				pVDev->Erase();
			}
		}
		DBG_ASSERT( bVDevValid, "VirtualDevice failed to be enlarged!" );
		if ( !bVDevValid )
		{
			Paint( pView, rRec, NULL /* without VirtualDevice */ );
			return;
		}

		// PaintRect fuer VDev nicht mit alignter Groesse,
		// da sonst die Zeile darunter auch ausgegeben werden muss:
		Rectangle aTmpRec( Point( 0, 0 ), aClipRec.GetSize() );

		aClipRec = pTarget->PixelToLogic( aClipRecPixel );
		Point aStartPos;
		if ( !IsVertical() )
		{
			aStartPos = aClipRec.TopLeft();
			aStartPos = pView->GetDocPos( aStartPos );
			aStartPos.X() *= (-1);
			aStartPos.Y() *= (-1);
		}
		else
		{
			aStartPos = aClipRec.TopRight();
			Point aDocPos( pView->GetDocPos( aStartPos ) );
			aStartPos.X() = aClipRec.GetSize().Width() + aDocPos.Y();
			aStartPos.Y() = -aDocPos.X();
		}

		Paint( pVDev, aTmpRec, aStartPos );

		sal_Bool bClipRegion = sal_False;
		Region aOldRegion;
		MapMode aOldMapMode;
		if ( GetTextRanger() )
		{
			// Some problems here with push/pop, why?!
//			pTarget->Push( PUSH_CLIPREGION|PUSH_MAPMODE );
			bClipRegion = pTarget->IsClipRegion();
			aOldRegion = pTarget->GetClipRegion();
			// Wie bekomme ich das Polygon an die richtige Stelle????
			// Das Polygon bezieht sich auf die View, nicht auf das Window
			// => Origin umsetzen...
			aOldMapMode = pTarget->GetMapMode();
			Point aOrigin = aOldMapMode.GetOrigin();
			Point aViewPos = pView->GetOutputArea().TopLeft();
			aOrigin.Move( aViewPos.X(), aViewPos.Y() );
			aClipRec.Move( -aViewPos.X(), -aViewPos.Y() );
			MapMode aNewMapMode( aOldMapMode );
			aNewMapMode.SetOrigin( aOrigin );
			pTarget->SetMapMode( aNewMapMode );
			pTarget->SetClipRegion( Region( GetTextRanger()->GetPolyPolygon() ) );
		}

		pTarget->DrawOutDev( aClipRec.TopLeft(), aClipRec.GetSize(),
							Point(0,0), aClipRec.GetSize(), *pVDev );

		if ( GetTextRanger() )
		{
//			pTarget->Pop();
			if ( bClipRegion )
				pTarget->SetClipRegion( aOldRegion );
			else
				pTarget->SetClipRegion();
			pTarget->SetMapMode( aOldMapMode );
		}


		pView->DrawSelection(pView->GetEditSelection(), 0, pTarget);
	}
	else
	{
		Point aStartPos;
		if ( !IsVertical() )
		{
			aStartPos = pView->GetOutputArea().TopLeft();
			aStartPos.X() -= pView->GetVisDocLeft();
			aStartPos.Y() -= pView->GetVisDocTop();
		}
		else
		{
			aStartPos = pView->GetOutputArea().TopRight();
			aStartPos.X() += pView->GetVisDocTop();
			aStartPos.Y() -= pView->GetVisDocLeft();
		}

		// Wenn Doc-Breite < OutputArea,Width, nicht umgebrochene Felder,
		// stehen die Felder sonst �ber, wenn > Zeile.
		// ( Oben nicht, da dort bereits Doc-Breite von Formatierung mit drin )
		if ( !IsVertical() && ( pView->GetOutputArea().GetWidth() > GetPaperSize().Width() ) )
		{
			long nMaxX = pView->GetOutputArea().Left() + GetPaperSize().Width();
			if ( aClipRec.Left() > nMaxX )
				return;
			if ( aClipRec.Right() > nMaxX )
				aClipRec.Right() = nMaxX;
		}

		sal_Bool bClipRegion = pTarget->IsClipRegion();
		Region aOldRegion = pTarget->GetClipRegion();
		pTarget->IntersectClipRegion( aClipRec );

		Paint( pTarget, aClipRec, aStartPos );

		if ( bClipRegion )
			pTarget->SetClipRegion( aOldRegion );
		else
			pTarget->SetClipRegion();

		pView->DrawSelection(pView->GetEditSelection(), 0, pTarget);
	}

}

void ImpEditEngine::InsertContent( ContentNode* pNode, sal_uInt16 nPos )
{
	DBG_ASSERT( pNode, "NULL-Poointer in InsertContent! " );
	DBG_ASSERT( IsInUndo(), "InsertContent nur fuer Undo()!" );
	ParaPortion* pNew = new ParaPortion( pNode );
	GetParaPortions().Insert( pNew, nPos );
	aEditDoc.Insert( pNode, nPos );
	if ( IsCallParaInsertedOrDeleted() )
		GetEditEnginePtr()->ParagraphInserted( nPos );
}

EditPaM ImpEditEngine::SplitContent( sal_uInt16 nNode, sal_uInt16 nSepPos )
{
	ContentNode* pNode = aEditDoc.SaveGetObject( nNode );
	DBG_ASSERT( pNode, "Ungueltiger Node in SplitContent" );
	DBG_ASSERT( IsInUndo(), "SplitContent nur fuer Undo()!" );
	DBG_ASSERT( nSepPos <= pNode->Len(), "Index im Wald: SplitContent" );
	EditPaM aPaM( pNode, nSepPos );
	return ImpInsertParaBreak( aPaM );
}

EditPaM ImpEditEngine::ConnectContents( sal_uInt16 nLeftNode, sal_Bool bBackward )
{
	ContentNode* pLeftNode = aEditDoc.SaveGetObject( nLeftNode );
	ContentNode* pRightNode = aEditDoc.SaveGetObject( nLeftNode+1 );
	DBG_ASSERT( pLeftNode, "Ungueltiger linker Node in ConnectContents" );
	DBG_ASSERT( pRightNode, "Ungueltiger rechter Node in ConnectContents" );
	DBG_ASSERT( IsInUndo(), "ConnectContent nur fuer Undo()!" );
	return ImpConnectParagraphs( pLeftNode, pRightNode, bBackward );
}

void ImpEditEngine::SetUpdateModeForAcc( sal_Bool bUp)
{
	bUpdateForAcc = bUp;
}

sal_Bool ImpEditEngine::GetUpdateModeForAcc()
{
	return bUpdateForAcc;
}

void ImpEditEngine::SetUpdateMode( sal_Bool bUp, EditView* pCurView, sal_Bool bForceUpdate )
{
	sal_Bool bChanged = ( GetUpdateMode() != bUp );

	// Beim Umschalten von sal_True auf sal_False waren alle Selektionen sichtbar,
	// => Wegmalen
	// Umgekehrt waren alle unsichtbar => malen

//	DrawAllSelections();	sieht im Outliner schlecht aus !
//	EditView* pView = aEditViewList.First();
//	while ( pView )
//	{
//		DBG_CHKOBJ( pView, EditView, 0 );
//		pView->pImpEditView->DrawSelection();
//		pView = aEditViewList.Next();
//	}

	// Wenn !bFormatted, also z.B. nach SetText, braucht bei UpdateMode sal_True
	// nicht sofort formatiert werden, weil warscheinlich noch Text kommt.
	// Spaetestens bei einem Paint / CalcTextWidth wird formatiert.

	bUpdate = bUp;
	if ( bUpdate && ( bChanged || bForceUpdate ) )
		FormatAndUpdate( pCurView );
}

void ImpEditEngine::ShowParagraph( sal_uInt16 nParagraph, sal_Bool bShow )
{
	ParaPortion* pPPortion = GetParaPortions().SaveGetObject( nParagraph );
	DBG_ASSERT( pPPortion, "ShowParagraph: Absatz existiert nicht!" );
	if ( pPPortion && ( pPPortion->IsVisible() != bShow ) )
	{
		pPPortion->SetVisible( bShow );

		if ( !bShow )
		{
			// Als deleted kenzeichnen, damit keine Selektion auf diesem
			// Absatz beginnt oder endet...
			DeletedNodeInfo* pDelInfo = new DeletedNodeInfo( (sal_uIntPtr)pPPortion->GetNode(), nParagraph );
			aDeletedNodes.Insert( pDelInfo, aDeletedNodes.Count() );
			UpdateSelections();
			// Dann kriege ich den unteren Bereich nicht invalidiert,
			// wenn UpdateMode = sal_False!
			// Wenn doch, dann vor SetVisible auf sal_False merken!
//			nCurTextHeight -= pPPortion->GetHeight();
		}

		if ( bShow && ( pPPortion->IsInvalid() || !pPPortion->nHeight ) )
		{
			if ( !GetTextRanger() )
			{
				if ( pPPortion->IsInvalid() )
				{
					Font aOldFont( GetRefDevice()->GetFont() );
					CreateLines( nParagraph, 0 );	// 0: Kein TextRanger
					if ( aStatus.DoRestoreFont() )
						GetRefDevice()->SetFont( aOldFont );
				}
				else
				{
					CalcHeight( pPPortion );
				}
				nCurTextHeight += pPPortion->GetHeight();
			}
			else
			{
				nCurTextHeight = 0x7fffffff;
			}
		}

		pPPortion->SetMustRepaint( sal_True );
		if ( GetUpdateMode() && !IsInUndo() && !GetTextRanger() )
		{
			aInvalidRec = Rectangle(	Point( 0, GetParaPortions().GetYOffset( pPPortion ) ),
										Point( GetPaperSize().Width(), nCurTextHeight ) );
			UpdateViews( GetActiveView() );
		}
	}
}

sal_Bool ImpEditEngine::IsParagraphVisible( sal_uInt16 nParagraph )
{
	ParaPortion* pPPortion = GetParaPortions().SaveGetObject( nParagraph );
	DBG_ASSERT( pPPortion, "IsParagraphVisible: Absatz existiert nicht!" );
	if ( pPPortion )
		return pPPortion->IsVisible();
	return sal_False;
}

EditSelection ImpEditEngine::MoveParagraphs( Range aOldPositions, sal_uInt16 nNewPos, EditView* pCurView )
{
	DBG_ASSERT( GetParaPortions().Count() != 0, "Keine Absaetze gefunden: MoveParagraphs" );
	if ( GetParaPortions().Count() == 0 )
		return EditSelection();
	aOldPositions.Justify();

	EditSelection aSel( ImpMoveParagraphs( aOldPositions, nNewPos ) );

	if ( nNewPos >= GetParaPortions().Count() )
		nNewPos = GetParaPortions().Count() - 1;

	// Dort, wo der Absatz eingefuegt wurde, muss richtig gepainted werden:
	// Dort, wo der Absatz entfernt wurde, muss richtig gepainted werden:
	// ( Und dazwischen entsprechend auch...)
	if ( pCurView && ( GetUpdateMode() == sal_True ) )
	{
		// in diesem Fall kann ich direkt neu malen, ohne die
		// Portions zu Invalidieren.
		sal_uInt16 nFirstPortion = Min( (sal_uInt16)aOldPositions.Min(), nNewPos );
		sal_uInt16 nLastPortion = Max( (sal_uInt16)aOldPositions.Max(), nNewPos );

		ParaPortion* pUpperPortion = GetParaPortions().SaveGetObject( nFirstPortion );
		ParaPortion* pLowerPortion = GetParaPortions().SaveGetObject( nLastPortion );

		aInvalidRec = Rectangle();	// leermachen
		aInvalidRec.Left() = 0;
		aInvalidRec.Right() = aPaperSize.Width();
		aInvalidRec.Top() = GetParaPortions().GetYOffset( pUpperPortion );
		aInvalidRec.Bottom() = GetParaPortions().GetYOffset( pLowerPortion ) + pLowerPortion->GetHeight();

		UpdateViews( pCurView );
	}
	else
	{
		// aber der oberen ungueltigen Position neu painten...
		sal_uInt16 nFirstInvPara = Min( (sal_uInt16)aOldPositions.Min(), nNewPos );
		InvalidateFromParagraph( nFirstInvPara );
	}
	return aSel;
}

void ImpEditEngine::InvalidateFromParagraph( sal_uInt16 nFirstInvPara )
{
	// Es werden nicht die folgenden Absaetze invalidiert,
	// da ResetHeight() => Groessenanderung => alles folgende wird
	// sowieso neu ausgegeben.
	ParaPortion* pTmpPortion;
	if ( nFirstInvPara != 0 )
	{
		pTmpPortion = GetParaPortions().GetObject( nFirstInvPara-1 );
		pTmpPortion->MarkInvalid( pTmpPortion->GetNode()->Len(), 0 );
	}
	else
	{
		pTmpPortion = GetParaPortions().GetObject( 0 );
		pTmpPortion->MarkSelectionInvalid( 0, pTmpPortion->GetNode()->Len() );
	}
	pTmpPortion->ResetHeight();
}

IMPL_LINK_INLINE_START( ImpEditEngine, StatusTimerHdl, Timer *, EMPTYARG )
{
	CallStatusHdl();
	return 0;
}
IMPL_LINK_INLINE_END( ImpEditEngine, StatusTimerHdl, Timer *, EMPTYARG )

void ImpEditEngine::CallStatusHdl()
{
	if ( aStatusHdlLink.IsSet() && aStatus.GetStatusWord() )
	{
		// Der Status muss vor Call zurueckgesetzt werden,
		// da im Hdl evtl. weitere Fags gesetzt werden...
		EditStatus aTmpStatus( aStatus );
		aStatus.Clear();
		aStatusHdlLink.Call( &aTmpStatus );
		aStatusTimer.Stop();	// Falls von Hand gerufen...
	}
}

ContentNode* ImpEditEngine::GetPrevVisNode( ContentNode* pCurNode )
{
	ParaPortion* pPortion = FindParaPortion( pCurNode );
	DBG_ASSERT( pPortion, "GetPrevVisibleNode: Keine passende Portion!" );
	pPortion = GetPrevVisPortion( pPortion );
	if ( pPortion )
		return pPortion->GetNode();
	return 0;
}

ContentNode* ImpEditEngine::GetNextVisNode( ContentNode* pCurNode )
{
	ParaPortion* pPortion = FindParaPortion( pCurNode );
	DBG_ASSERT( pPortion, "GetNextVisibleNode: Keine passende Portion!" );
	pPortion = GetNextVisPortion( pPortion );
	if ( pPortion )
		return pPortion->GetNode();
	return 0;
}

ParaPortion* ImpEditEngine::GetPrevVisPortion( ParaPortion* pCurPortion )
{
	sal_uInt16 nPara = GetParaPortions().GetPos( pCurPortion );
	DBG_ASSERT( nPara < GetParaPortions().Count() , "Portion nicht gefunden: GetPrevVisPortion" );
	ParaPortion* pPortion = nPara ? GetParaPortions()[--nPara] : 0;
	while ( pPortion && !pPortion->IsVisible() )
		pPortion = nPara ? GetParaPortions()[--nPara] : 0;

	return pPortion;
}

ParaPortion* ImpEditEngine::GetNextVisPortion( ParaPortion* pCurPortion )
{
	sal_uInt16 nPara = GetParaPortions().GetPos( pCurPortion );
	DBG_ASSERT( nPara < GetParaPortions().Count() , "Portion nicht gefunden: GetPrevVisNode" );
	ParaPortion* pPortion = GetParaPortions().SaveGetObject( ++nPara );
	while ( pPortion && !pPortion->IsVisible() )
		pPortion = GetParaPortions().SaveGetObject( ++nPara );

	return pPortion;
}

EditPaM ImpEditEngine::InsertParagraph( sal_uInt16 nPara )
{
	EditPaM aPaM;
	if ( nPara != 0 )
	{
		ContentNode* pNode = GetEditDoc().SaveGetObject( nPara-1 );
		if ( !pNode )
			pNode = GetEditDoc().SaveGetObject( GetEditDoc().Count() - 1 );
		DBG_ASSERT( pNode, "Kein einziger Absatz in InsertParagraph ?" );
		aPaM = EditPaM( pNode, pNode->Len() );
	}
	else
	{
		ContentNode* pNode = GetEditDoc().SaveGetObject( 0 );
		aPaM = EditPaM( pNode, 0 );
	}

	return ImpInsertParaBreak( aPaM );
}

EditSelection* ImpEditEngine::SelectParagraph( sal_uInt16 nPara )
{
	EditSelection* pSel = 0;
	ContentNode* pNode = GetEditDoc().SaveGetObject( nPara );
	DBG_ASSERTWARNING( pNode, "Absatz existiert nicht: SelectParagraph" );
	if ( pNode )
		pSel = new EditSelection( EditPaM( pNode, 0 ), EditPaM( pNode, pNode->Len() ) );

	return pSel;
}

void ImpEditEngine::FormatAndUpdate( EditView* pCurView )
{
	if ( bDowning )
		return ;

	if ( IsInUndo() )
		IdleFormatAndUpdate( pCurView );
	else
	{
		FormatDoc();
		UpdateViews( pCurView );
	}
}

void ImpEditEngine::SetFlatMode( sal_Bool bFlat )
{
	if ( bFlat != aStatus.UseCharAttribs() )
		return;

	if ( !bFlat )
		aStatus.TurnOnFlags( EE_CNTRL_USECHARATTRIBS );
	else
		aStatus.TurnOffFlags( EE_CNTRL_USECHARATTRIBS );

	aEditDoc.CreateDefFont( !bFlat );

	FormatFullDoc();
	UpdateViews( (EditView*) 0);
	if ( pActiveView )
		pActiveView->ShowCursor();
}

void ImpEditEngine::SetCharStretching( sal_uInt16 nX, sal_uInt16 nY )
{
	if ( !IsVertical() )
	{
		nStretchX = nX;
		nStretchY = nY;
	}
	else
	{
		nStretchX = nY;
		nStretchY = nX;
	}

	if ( aStatus.DoStretch() )
	{
		FormatFullDoc();
		UpdateViews( GetActiveView() );
	}
}

void ImpEditEngine::DoStretchChars( sal_uInt16 nX, sal_uInt16 nY )
{
	UndoActionStart( EDITUNDO_STRETCH );
	sal_uInt16 nParas = GetEditDoc().Count();
	for ( sal_uInt16 nPara = 0; nPara < nParas; nPara++ )
	{
		ContentNode* pNode = GetEditDoc()[nPara];
		SfxItemSet aTmpSet( pNode->GetContentAttribs().GetItems() );

		if ( nX != 100 )
		{
			// Fontbreite
			SvxCharScaleWidthItem* pNewWidth = (SvxCharScaleWidthItem*) pNode->GetContentAttribs().GetItem( EE_CHAR_FONTWIDTH ).Clone();
			sal_uInt32 nProp = pNewWidth->GetValue();	// sal_uInt32, kann temporaer gross werden
			nProp *= nX;
			nProp /= 100;
			pNewWidth->SetValue( (sal_uInt16)nProp );
			aTmpSet.Put( *pNewWidth );
			delete pNewWidth;

			// Kerning:
			const SvxKerningItem& rKerningItem =
				(const SvxKerningItem&)pNode->GetContentAttribs().GetItem( EE_CHAR_KERNING );
			SvxKerningItem* pNewKerning = (SvxKerningItem*)rKerningItem.Clone();
			long nKerning = pNewKerning->GetValue();
			if ( nKerning > 0 )
			{
				nKerning *= nX;
				nKerning /= 100;
			}
			else if ( nKerning < 0 )
			{
				// Bei Negativen Werten:
				// Bei Stretching > 100 muessen die Werte kleiner werden und umgekehrt.
				nKerning *= 100;
				nKerning /= nX;
			}
			pNewKerning->SetValue( (short)nKerning );
			aTmpSet.Put( *pNewKerning);
			delete pNewKerning;
		}
		else
			aTmpSet.ClearItem( EE_CHAR_FONTWIDTH );

		if ( nY != 100 )
		{
			// Fonthoehe
			for ( int nItem = 0; nItem < 3; nItem++ )
			{
				sal_uInt16 nItemId = EE_CHAR_FONTHEIGHT;
				if ( nItem == 1 )
					nItemId = EE_CHAR_FONTHEIGHT_CJK;
				else if ( nItem == 2 )
					nItemId = EE_CHAR_FONTHEIGHT_CTL;

				const SvxFontHeightItem& rHeightItem =
					(const SvxFontHeightItem&)pNode->GetContentAttribs().GetItem( nItemId );
				SvxFontHeightItem* pNewHeight = (SvxFontHeightItem*)rHeightItem.Clone();
				sal_uInt32 nHeight = pNewHeight->GetHeight();
				nHeight *= nY;
				nHeight /= 100;
				pNewHeight->SetHeightValue( nHeight );
				aTmpSet.Put( *pNewHeight );
				delete pNewHeight;
			}

			// Absatzabstaende
			const SvxULSpaceItem& rULSpaceItem =
				(const SvxULSpaceItem&)pNode->GetContentAttribs().GetItem( EE_PARA_ULSPACE );
			SvxULSpaceItem* pNewUL = (SvxULSpaceItem*)rULSpaceItem.Clone();
			sal_uInt32 nUpper = pNewUL->GetUpper();
			nUpper *= nY;
			nUpper /= 100;
			pNewUL->SetUpper( (sal_uInt16)nUpper );
			sal_uInt32 nLower = pNewUL->GetLower();
			nLower *= nY;
			nLower /= 100;
			pNewUL->SetLower( (sal_uInt16)nLower );
			aTmpSet.Put( *pNewUL );
			delete pNewUL;
		}
		else
			aTmpSet.ClearItem( EE_CHAR_FONTHEIGHT );

		SetParaAttribs( nPara, aTmpSet );

		// harte Attribute:
		sal_uInt16 nLastEnd = 0;	// damit nach entfernen und neu nicht nochmal
		CharAttribArray& rAttribs = pNode->GetCharAttribs().GetAttribs();
		sal_uInt16 nAttribs = rAttribs.Count();
		for ( sal_uInt16 nAttr = 0; nAttr < nAttribs; nAttr++ )
		{
			EditCharAttrib* pAttr = rAttribs[nAttr];
			if ( pAttr->GetStart() >= nLastEnd )
			{
				sal_uInt16 nWhich = pAttr->Which();
				SfxPoolItem* pNew = 0;
				if ( nWhich == EE_CHAR_FONTHEIGHT )
				{
					SvxFontHeightItem* pNewHeight = (SvxFontHeightItem*)pAttr->GetItem()->Clone();
					sal_uInt32 nHeight = pNewHeight->GetHeight();
					nHeight *= nY;
					nHeight /= 100;
					pNewHeight->SetHeightValue( nHeight );
					pNew = pNewHeight;
				}
				else if ( nWhich == EE_CHAR_FONTWIDTH )
				{
					SvxCharScaleWidthItem* pNewWidth = (SvxCharScaleWidthItem*)pAttr->GetItem()->Clone();
					sal_uInt32 nProp = pNewWidth->GetValue();
					nProp *= nX;
					nProp /= 100;
					pNewWidth->SetValue( (sal_uInt16)nProp );
					pNew = pNewWidth;
				}
				else if ( nWhich == EE_CHAR_KERNING )
				{
					SvxKerningItem* pNewKerning = (SvxKerningItem*)pAttr->GetItem()->Clone();
					long nKerning = pNewKerning->GetValue();
					if ( nKerning > 0 )
					{
						nKerning *= nX;
						nKerning /= 100;
					}
					else if ( nKerning < 0 )
					{
						// Bei Negativen Werten:
						// Bei Stretching > 100 muessen die Werte kleiner werden und umgekehrt.
						nKerning *= 100;
						nKerning /= nX;
					}
					pNewKerning->SetValue( (short)nKerning );
					pNew = pNewKerning;
				}
				if ( pNew )
				{
					SfxItemSet _aTmpSet( GetEmptyItemSet() );
					_aTmpSet.Put( *pNew );
					SetAttribs( EditSelection( EditPaM( pNode, pAttr->GetStart() ),
						EditPaM( pNode, pAttr->GetEnd() ) ), _aTmpSet );

					nLastEnd = pAttr->GetEnd();
					delete pNew;
				}
			}
		}
	}
	UndoActionEnd( EDITUNDO_STRETCH );
}

const SvxNumberFormat* ImpEditEngine::GetNumberFormat( const ContentNode *pNode ) const
{
    const SvxNumberFormat *pRes = 0;

    if (pNode)
    {
        // get index of paragraph
        sal_uInt16 nPara = GetEditDoc().GetPos( const_cast< ContentNode * >(pNode) );
        DBG_ASSERT( nPara < USHRT_MAX, "node not found in array" );
        if (nPara < USHRT_MAX)
        {
            // the called function may be overloaded by an OutlinerEditEng object to provide
            // access to the SvxNumberFormat of the Outliner.
            // The EditEngine implementation will just return 0.
            pRes = pEditEngine->GetNumberFormat( nPara );
        }
    }

    return pRes;
}

sal_Int32 ImpEditEngine::GetSpaceBeforeAndMinLabelWidth(
    const ContentNode *pNode,
    sal_Int32 *pnSpaceBefore, sal_Int32 *pnMinLabelWidth ) const
{
    // nSpaceBefore     matches the ODF attribut text:space-before
    // nMinLabelWidth   matches the ODF attribut text:min-label-width

    const SvxNumberFormat *pNumFmt = GetNumberFormat( pNode );

    // if no number format was found we have no Outliner or the numbering level
    // within the Outliner is -1 which means no number format should be applied.
    // Thus the default values to be returned are 0.
    sal_Int32 nSpaceBefore   = 0;
    sal_Int32 nMinLabelWidth = 0;

    if (pNumFmt)
    {
        nMinLabelWidth = -pNumFmt->GetFirstLineOffset();
        nSpaceBefore   = pNumFmt->GetAbsLSpace() - nMinLabelWidth;
        DBG_ASSERT( nMinLabelWidth >= 0, "ImpEditEngine::GetSpaceBeforeAndMinLabelWidth: min-label-width < 0 encountered" );
    }
    if (pnSpaceBefore)
        *pnSpaceBefore      = nSpaceBefore;
    if (pnMinLabelWidth)
        *pnMinLabelWidth    = nMinLabelWidth;

    return nSpaceBefore + nMinLabelWidth;
}

const SvxLRSpaceItem& ImpEditEngine::GetLRSpaceItem( ContentNode* pNode )
{
    return (const SvxLRSpaceItem&)pNode->GetContentAttribs().GetItem( aStatus.IsOutliner() ? EE_PARA_OUTLLRSPACE : EE_PARA_LRSPACE );
}

// Either sets the digit mode at the output device or
// modifies the passed string according to the text numeral setting:
void ImpEditEngine::ImplInitDigitMode( OutputDevice* pOutDev, String* pString, xub_StrLen nStt, xub_StrLen nLen, LanguageType eCurLang )
{
    // #114278# Also setting up digit language from Svt options
    // (cannot reliably inherit the outdev's setting)
    if( !pCTLOptions )
        pCTLOptions = new SvtCTLOptions;

    LanguageType eLang = eCurLang;
    const SvtCTLOptions::TextNumerals nCTLTextNumerals = pCTLOptions->GetCTLTextNumerals();

    if ( SvtCTLOptions::NUMERALS_HINDI == nCTLTextNumerals )
        eLang = LANGUAGE_ARABIC_SAUDI_ARABIA;
    else if ( SvtCTLOptions::NUMERALS_ARABIC == nCTLTextNumerals )
        eLang = LANGUAGE_ENGLISH;
    else if ( SvtCTLOptions::NUMERALS_SYSTEM == nCTLTextNumerals )
        eLang = (LanguageType) Application::GetSettings().GetLanguage();

	if(pOutDev)
	{
		pOutDev->SetDigitLanguage( eLang );
	}
	else if (pString)
	{
		// see sallayout.cxx in vcl
        int nOffset;
        switch( eLang & LANGUAGE_MASK_PRIMARY )
        {
            default:
                nOffset = 0;
                break;
            case LANGUAGE_ARABIC_SAUDI_ARABIA  & LANGUAGE_MASK_PRIMARY:
                nOffset = 0x0660 - '0';  // arabic-indic digits
                break;
            case LANGUAGE_URDU          & LANGUAGE_MASK_PRIMARY:
            case LANGUAGE_PUNJABI       & LANGUAGE_MASK_PRIMARY: //???
            case LANGUAGE_SINDHI        & LANGUAGE_MASK_PRIMARY:    
                nOffset = 0x06F0 - '0';  // eastern arabic-indic digits
                break;
        }
        if (nOffset)
        {
			const xub_StrLen nEnd = nStt + nLen;
			for( xub_StrLen nIdx = nStt; nIdx < nEnd; ++nIdx )
			{
				sal_Unicode nChar = pString->GetChar( nIdx );
				if( (nChar < '0') || ('9' < nChar) )
					continue;
                nChar = (sal_Unicode)(nChar + nOffset);
                pString->SetChar( nIdx, nChar );
			}
        }
	}
}

void ImpEditEngine::ImplInitLayoutMode( OutputDevice* pOutDev, sal_uInt16 nPara, sal_uInt16 nIndex )
{
    sal_Bool bCTL = sal_False;
    sal_uInt8 bR2L = sal_False;
    if ( nIndex == 0xFFFF )
    {
        bCTL = HasScriptType( nPara, i18n::ScriptType::COMPLEX );
        bR2L = IsRightToLeft( nPara );
    }
    else
    {
        ContentNode* pNode = GetEditDoc().SaveGetObject( nPara );
        short nScriptType = GetScriptType( EditPaM( pNode, nIndex+1 ) );
        bCTL = nScriptType == i18n::ScriptType::COMPLEX;
        bR2L = GetRightToLeft( nPara, nIndex + 1);  // this change was discussed in issue 37190
                                                    // it also works for issue 55927
    }

    sal_uLong nLayoutMode = pOutDev->GetLayoutMode();

    // We always use the left position for DrawText()
    nLayoutMode &= ~(TEXT_LAYOUT_BIDI_RTL);

    if ( !bCTL && !bR2L)
    {
        // No CTL/Bidi checking necessary
        nLayoutMode |= ( TEXT_LAYOUT_COMPLEX_DISABLED | TEXT_LAYOUT_BIDI_STRONG );
    }
    else
    {
        // CTL/Bidi checking necessary
        // Don't use BIDI_STRONG, VCL must do some checks.
        nLayoutMode &= ~( TEXT_LAYOUT_COMPLEX_DISABLED | TEXT_LAYOUT_BIDI_STRONG );

        if ( bR2L )
            nLayoutMode |= TEXT_LAYOUT_BIDI_RTL|TEXT_LAYOUT_TEXTORIGIN_LEFT;
    }

    pOutDev->SetLayoutMode( nLayoutMode );

    // #114278# Also setting up digit language from Svt options
    // (cannot reliably inherit the outdev's setting)
    LanguageType eLang;

    if( !pCTLOptions )
        pCTLOptions = new SvtCTLOptions;

    if ( SvtCTLOptions::NUMERALS_HINDI == pCTLOptions->GetCTLTextNumerals() )
        eLang = LANGUAGE_ARABIC_SAUDI_ARABIA;
    else if ( SvtCTLOptions::NUMERALS_ARABIC == pCTLOptions->GetCTLTextNumerals() )
        eLang = LANGUAGE_ENGLISH;
    else
        eLang = (LanguageType) Application::GetSettings().GetLanguage();

    pOutDev->SetDigitLanguage( eLang );
}

Reference < i18n::XBreakIterator > ImpEditEngine::ImplGetBreakIterator() const
{
	if ( !xBI.is() )
	{
		Reference< lang::XMultiServiceFactory > xMSF( ::comphelper::getProcessServiceFactory() );
		xBI.set( xMSF->createInstance( OUString::createFromAscii( "com.sun.star.i18n.BreakIterator" ) ), UNO_QUERY );
	}
	return xBI;
}

Reference < i18n::XExtendedInputSequenceChecker > ImpEditEngine::ImplGetInputSequenceChecker() const
{
    if ( !xISC.is() )
    {
        Reference< lang::XMultiServiceFactory > xMSF = ::comphelper::getProcessServiceFactory();
        Reference < XInterface > xI = xMSF->createInstance( OUString::createFromAscii( "com.sun.star.i18n.InputSequenceChecker" ) );
        if ( xI.is() )
        {
            Any x = xI->queryInterface( ::getCppuType((const Reference< i18n::XExtendedInputSequenceChecker >*)0) );
            x >>= xISC;
        }
    }
    return xISC;
}

Color ImpEditEngine::GetAutoColor() const
{
	Color aColor = const_cast<ImpEditEngine*>(this)->GetColorConfig().GetColorValue( svtools::FONTCOLOR ).nColor;

	if ( GetBackgroundColor() != COL_AUTO )
	{
        if ( GetBackgroundColor().IsDark() && aColor.IsDark() )
		    aColor = COL_WHITE;
        else if ( GetBackgroundColor().IsBright() && aColor.IsBright() )
		    aColor = COL_BLACK;
	}

	return aColor;
}


sal_Bool ImpEditEngine::ImplCalcAsianCompression( ContentNode* pNode, TextPortion* pTextPortion, sal_uInt16 nStartPos, sal_Int32* pDXArray, sal_uInt16 n100thPercentFromMax, sal_Bool bManipulateDXArray )
{
    DBG_ASSERT( GetAsianCompressionMode(), "ImplCalcAsianCompression - Why?" );
    DBG_ASSERT( pTextPortion->GetLen(), "ImplCalcAsianCompression - Empty Portion?" );

    // Percent is 1/100 Percent...

    if ( n100thPercentFromMax == 10000 )
        pTextPortion->SetExtraInfos( NULL );

    sal_Bool bCompressed = sal_False;

	if ( GetScriptType( EditPaM( pNode, nStartPos+1 ) ) == i18n::ScriptType::ASIAN )
    {
        long nNewPortionWidth = pTextPortion->GetSize().Width();
        sal_uInt16 nPortionLen = pTextPortion->GetLen();
        for ( sal_uInt16 n = 0; n < nPortionLen; n++ )
        {
            sal_uInt8 nType = GetCharTypeForCompression( pNode->GetChar( n+nStartPos ) );

            sal_Bool bCompressPunctuation = ( nType == CHAR_PUNCTUATIONLEFT ) || ( nType == CHAR_PUNCTUATIONRIGHT );
            sal_Bool bCompressKana = ( nType == CHAR_KANA ) && ( GetAsianCompressionMode() == text::CharacterCompressionType::PUNCTUATION_AND_KANA );

            // create Extra infos only if needed...
            if ( bCompressPunctuation || bCompressKana )
            {
                if ( !pTextPortion->GetExtraInfos() )
                {
                    ExtraPortionInfo* pExtraInfos = new ExtraPortionInfo;
                    pTextPortion->SetExtraInfos( pExtraInfos );
                    pExtraInfos->nOrgWidth = pTextPortion->GetSize().Width();
                    pExtraInfos->nAsianCompressionTypes = CHAR_NORMAL;
                }
                pTextPortion->GetExtraInfos()->nMaxCompression100thPercent = n100thPercentFromMax;
                pTextPortion->GetExtraInfos()->nAsianCompressionTypes |= nType;
//                pTextPortion->GetExtraInfos()->nCompressedChars++;

                long nOldCharWidth;
                if ( (n+1) < nPortionLen )
                {
                    nOldCharWidth = pDXArray[n];
                }
                else
                {
                    if ( bManipulateDXArray )
                        nOldCharWidth = nNewPortionWidth - pTextPortion->GetExtraInfos()->nPortionOffsetX;
                    else
                        nOldCharWidth = pTextPortion->GetExtraInfos()->nOrgWidth;
                }
                nOldCharWidth -= ( n ? pDXArray[n-1] : 0 );

                long nCompress = 0;

                if ( bCompressPunctuation )
                {
                    // pTextPortion->GetExtraInfos()->nComressionWeight += 5;
                    nCompress = nOldCharWidth / 2;
                }
                else // Kana
                {
                    // pTextPortion->GetExtraInfos()->nComressionWeight += 1;
                    nCompress = nOldCharWidth / 10;
                }

                if ( n100thPercentFromMax != 10000 )
                {
                    nCompress *= n100thPercentFromMax;
                    nCompress /= 10000;
                }

                if ( nCompress )
                {
                    bCompressed = sal_True;
                    nNewPortionWidth -= nCompress;
                    pTextPortion->GetExtraInfos()->bCompressed = sal_True;


                    // Special handling for rightpunctuation: For the 'compression' we must
                    // start th eoutput before the normal char position....
                    if ( bManipulateDXArray && ( pTextPortion->GetLen() > 1 ) )
                    {
                        if ( !pTextPortion->GetExtraInfos()->pOrgDXArray )
                            pTextPortion->GetExtraInfos()->SaveOrgDXArray( pDXArray, pTextPortion->GetLen()-1 );

                        if ( nType == CHAR_PUNCTUATIONRIGHT )
                        {
                            // If it's the first char, I must handle it in Paint()...
                            if ( n )
                            {
                                // -1: No entry for the last character
                                for ( sal_uInt16 i = n-1; i < (nPortionLen-1); i++ )
                                    pDXArray[i] -= nCompress;
                            }
                            else
                            {
                                pTextPortion->GetExtraInfos()->bFirstCharIsRightPunktuation = sal_True;
                                pTextPortion->GetExtraInfos()->nPortionOffsetX = -nCompress;
                            }
                        }
                        else
                        {
                            // -1: No entry for the last character
                            for ( sal_uInt16 i = n; i < (nPortionLen-1); i++ )
                                pDXArray[i] -= nCompress;
                        }
                    }
                }
            }
        }

        if ( bCompressed && ( n100thPercentFromMax == 10000 ) )
            pTextPortion->GetExtraInfos()->nWidthFullCompression = nNewPortionWidth;

        pTextPortion->GetSize().Width() = nNewPortionWidth;

        if ( pTextPortion->GetExtraInfos() && ( n100thPercentFromMax != 10000 ) )
        {
            // Maybe rounding errors in nNewPortionWidth, assure that width not bigger than expected
            long nShrink = pTextPortion->GetExtraInfos()->nOrgWidth - pTextPortion->GetExtraInfos()->nWidthFullCompression;
            nShrink *= n100thPercentFromMax;
            nShrink /= 10000;
            long nNewWidth = pTextPortion->GetExtraInfos()->nOrgWidth - nShrink;
            if ( nNewWidth < pTextPortion->GetSize().Width() )
            pTextPortion->GetSize().Width() = nNewWidth;
        }
    }
    return bCompressed;
}


void ImpEditEngine::ImplExpandCompressedPortions( EditLine* pLine, ParaPortion* pParaPortion, long nRemainingWidth )
{
    sal_Bool bFoundCompressedPortion = sal_False;
    long nCompressed = 0;
//    long nCompressWeight = 0;
    TextPortionList aCompressedPortions;

    sal_uInt16 nPortion = pLine->GetEndPortion();
    TextPortion* pTP = pParaPortion->GetTextPortions()[ nPortion ];
    while ( pTP && ( pTP->GetKind() == PORTIONKIND_TEXT ) )
    {
        if ( pTP->GetExtraInfos() && pTP->GetExtraInfos()->bCompressed )
        {
            bFoundCompressedPortion = sal_True;
            nCompressed += pTP->GetExtraInfos()->nOrgWidth - pTP->GetSize().Width();
            aCompressedPortions.Insert( pTP, aCompressedPortions.Count() );
        }
        pTP = ( nPortion > pLine->GetStartPortion() ) ? pParaPortion->GetTextPortions()[ --nPortion ] : NULL;
    }

    if ( bFoundCompressedPortion )
    {
        long nCompressPercent = 0;
        if ( nCompressed > nRemainingWidth )
        {
            nCompressPercent = nCompressed - nRemainingWidth;
            DBG_ASSERT( nCompressPercent < 200000, "ImplExpandCompressedPortions - Overflow!" );
            nCompressPercent *= 10000;
            nCompressPercent /= nCompressed;
        }

        for ( sal_uInt16 n = 0; n < aCompressedPortions.Count(); n++ )
        {
            pTP = aCompressedPortions[n];
            pTP->GetExtraInfos()->bCompressed = sal_False;
            pTP->GetSize().Width() = pTP->GetExtraInfos()->nOrgWidth;
            if ( nCompressPercent )
            {
                sal_uInt16 nTxtPortion = pParaPortion->GetTextPortions().GetPos( pTP );
                sal_uInt16 nTxtPortionStart = pParaPortion->GetTextPortions().GetStartPos( nTxtPortion );
                DBG_ASSERT( nTxtPortionStart >= pLine->GetStart(), "Portion doesn't belong to the line!!!" );
                sal_Int32* pDXArray = const_cast< sal_Int32* >( pLine->GetCharPosArray().GetData()+( nTxtPortionStart-pLine->GetStart() ) );
                if ( pTP->GetExtraInfos()->pOrgDXArray )
                    memcpy( pDXArray, pTP->GetExtraInfos()->pOrgDXArray, (pTP->GetLen()-1)*sizeof(sal_Int32) );
                ImplCalcAsianCompression( pParaPortion->GetNode(), pTP, nTxtPortionStart, pDXArray, (sal_uInt16)nCompressPercent, sal_True );
            }
        }
    }

    aCompressedPortions.Remove( 0, aCompressedPortions.Count() );
}

// redesigned to work with TextMarkingVector
void ImpEditEngine::ImplFillTextMarkingVector(const lang::Locale& rLocale, EEngineData::TextMarkingVector& rTextMarkingVector, const String& rTxt, const sal_uInt16 nIdx, const sal_uInt16 nLen) const
{
    // determine relevant logical text elements for the just-rendered
    // string of characters.
    Reference< i18n::XBreakIterator > _xBI(ImplGetBreakIterator());

    if(_xBI.is())
    {
        sal_Int32 nDone;
        sal_Int32 nNextCellBreak(_xBI->nextCharacters(rTxt, nIdx, rLocale, i18n::CharacterIteratorMode::SKIPCELL, 0, nDone));
        i18n::Boundary nNextWordBoundary(_xBI->getWordBoundary(rTxt, nIdx, rLocale, i18n::WordType::ANY_WORD, sal_True));
        sal_Int32 nNextSentenceBreak(_xBI->endOfSentence(rTxt, nIdx, rLocale));

        const sal_Int32 nEndPos(nIdx + nLen);
        sal_Int32 i;

        for(i = nIdx; i < nEndPos; i++)
        {
            // create the entries for the respective break positions
            if(i == nNextCellBreak)
            {
                rTextMarkingVector.push_back(EEngineData::TextMarkingClass(EEngineData::EndOfCaracter, i - nIdx));
                nNextCellBreak = _xBI->nextCharacters(rTxt, i, rLocale, i18n::CharacterIteratorMode::SKIPCELL, 1, nDone);
            }
            if(i == nNextWordBoundary.endPos)
            {
                rTextMarkingVector.push_back(EEngineData::TextMarkingClass(EEngineData::EndOfWord, i - nIdx));
                nNextWordBoundary = _xBI->getWordBoundary(rTxt, i + 1, rLocale, i18n::WordType::ANY_WORD, sal_True);
            }
            if(i == nNextSentenceBreak)
            {
                rTextMarkingVector.push_back(EEngineData::TextMarkingClass(EEngineData::EndOfSentence, i - nIdx));
                nNextSentenceBreak = _xBI->endOfSentence(rTxt, i + 1, rLocale);
            }
        }
    }
}

// eof
