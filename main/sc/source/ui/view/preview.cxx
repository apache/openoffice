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
#include "precompiled_sc.hxx"



// INCLUDE ---------------------------------------------------------------
#include <tools/pstm.hxx>
#include "scitems.hxx"
#include <editeng/eeitem.hxx>


#include <svtools/colorcfg.hxx>
#include <svx/fmview.hxx>
#include <editeng/sizeitem.hxx>
#include <svx/svdpagv.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/dispatch.hxx>
#include <svtools/accessibilityoptions.hxx>
#include <svl/itemset.hxx>
#include <tools/multisel.hxx>
#include <vcl/waitobj.hxx>
#include <vcl/sound.hxx>

#include "preview.hxx"
#include "prevwsh.hxx"
#include "prevloc.hxx"
#include "docsh.hxx"
#include "docfunc.hxx"
#include "printfun.hxx"
#include "printopt.hxx"
#include "stlpool.hxx"
#include "undostyl.hxx"
#include "drwlayer.hxx"
#include "scmod.hxx"
#include "globstr.hrc"
#include "sc.hrc"			// fuer ShellInvalidate
#include "AccessibleDocumentPagePreview.hxx"
#include <vcl/lineinfo.hxx>
#include <svx/algitem.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/ulspitem.hxx>
#include <editeng/sizeitem.hxx>
#include "attrib.hxx"
#include "pagepar.hxx"
#include <com/sun/star/accessibility/XAccessible.hpp>
#include "AccessibilityHints.hxx"
#include <vcl/svapp.hxx>
#include "viewutil.hxx"

// STATIC DATA -----------------------------------------------------------

//==================================================================

//#define SC_PREVIEW_SHADOWSIZE	2

long lcl_GetDisplayStart( SCTAB nTab, ScDocument* pDoc, long* pPages )
{
	long nDisplayStart = 0;
	for (SCTAB i=0; i<nTab; i++)
	{
		if ( pDoc->NeedPageResetAfterTab(i) )
			nDisplayStart = 0;
		else
			nDisplayStart += pPages[i];
	}
	return nDisplayStart;
}


ScPreview::ScPreview( Window* pParent, ScDocShell* pDocSh, ScPreviewShell* pViewSh ) :
	Window( pParent ),
	nPageNo( 0 ),
	nZoom( 100 ),
	bValid( sal_False ),
	nTabsTested( 0 ),
	nTab( 0 ),
	nTabStart( 0 ),
	nDisplayStart( 0 ),
	nTotalPages( 0 ),
	bStateValid( sal_False ),
	bLocationValid( sal_False ),
	pLocationData( NULL ),
	pDrawView( NULL ),
    bInPaint( false ),
    bInSetZoom( false ),
	bInGetState( sal_False ),
	pDocShell( pDocSh ),
    pViewShell( pViewSh ),
    bLeftRulerMove( sal_False ),
    bRightRulerMove( sal_False ),
    bTopRulerMove( sal_False ),
    bBottomRulerMove( sal_False ),
    bHeaderRulerMove( sal_False ),
    bFooterRulerMove( sal_False ),
    bLeftRulerChange( sal_False ),
    bRightRulerChange( sal_False ),
    bTopRulerChange( sal_False ),
    bBottomRulerChange( sal_False ),
    bHeaderRulerChange( sal_False ),
    bFooterRulerChange( sal_False ),
    bPageMargin ( sal_False ),
    bColRulerMove( sal_False ),
    mnScale( 0 ),
    nColNumberButttonDown( 0 ),
    nHeaderHeight ( 0 ),
    nFooterHeight ( 0 )
{
    SetOutDevViewType( OUTDEV_VIEWTYPE_PRINTPREVIEW ); //#106611#
	SetBackground();

	SetHelpId( HID_SC_WIN_PREVIEW );
	SetUniqueId( HID_SC_WIN_PREVIEW );

	SetDigitLanguage( SC_MOD()->GetOptDigitLanguage() );

    for (SCCOL i=0; i<=MAXCOL; i++)
        nRight[i] = 0;                  // initialized with actual positions when markers are drawn
}


__EXPORT ScPreview::~ScPreview()
{
	delete pDrawView;
	delete pLocationData;
}

void ScPreview::UpdateDrawView()		// nTab muss richtig sein
{
	ScDocument* pDoc = pDocShell->GetDocument();
	ScDrawLayer* pModel = pDoc->GetDrawLayer();		// ist nicht 0

	// #114135#
	if ( pModel )
	{
        SdrPage* pPage = pModel->GetPage(nTab);
        if ( pDrawView && ( !pDrawView->GetSdrPageView() || pDrawView->GetSdrPageView()->GetPage() != pPage ) )
		{
			//	die angezeigte Page der DrawView umzustellen (s.u.) funktioniert nicht ?!?
			delete pDrawView;
			pDrawView = NULL;
		}

		if ( !pDrawView )									// neu anlegen?
		{
			pDrawView = new FmFormView( pModel, this );
			// #55259# die DrawView uebernimmt den Design-Modus vom Model
			// (Einstellung "Im Entwurfsmodus oeffnen"), darum hier zuruecksetzen
			pDrawView->SetDesignMode( sal_True );
			pDrawView->SetPrintPreview( sal_True );
            pDrawView->ShowSdrPage(pPage);
		}
#if 0
		else if ( !pDrawView->GetSdrPageView())		// angezeigte Page umstellen
		{
			pDrawView->HideSdrPage();
			pDrawView->ShowSdrPage(pDrawView->GetModel()->GetPage(nTab));
		}
#endif
	}
	else if ( pDrawView )
	{
		delete pDrawView;			// fuer diese Tabelle nicht gebraucht
		pDrawView = NULL;
	}
}


void ScPreview::TestLastPage()
{
	if (nPageNo >= nTotalPages)
	{
		if (nTotalPages)
		{
			nPageNo = nTotalPages - 1;
			nTab = nTabCount - 1;
			while (nTab > 0 && !nPages[nTab])		// letzte nicht leere Tabelle
				--nTab;
			DBG_ASSERT(nPages[nTab],"alle Tabellen leer?");
			nTabPage = nPages[nTab] - 1;
			nTabStart = 0;
			for (sal_uInt16 i=0; i<nTab; i++)
				nTabStart += nPages[i];

			ScDocument* pDoc = pDocShell->GetDocument();
			nDisplayStart = lcl_GetDisplayStart( nTab, pDoc, nPages );
		}
		else		// leeres Dokument
		{
			nTab = 0;
			nPageNo = nTabPage = nTabStart = nDisplayStart = 0;
			aState.nPrintTab = 0;
                        aState.nStartCol = aState.nEndCol = 0;
                        aState.nStartRow = aState.nEndRow = 0;
                        aState.nZoom = 0;
			aState.nPagesX = aState.nPagesY = 0;
			aState.nTabPages = aState.nTotalPages =
			aState.nPageStart = aState.nDocPages = 0;
		}
	}
}


void ScPreview::CalcPages( SCTAB /*nToWhichTab*/ )
{
	WaitObject( this );

	ScDocument* pDoc = pDocShell->GetDocument();
	nTabCount = pDoc->GetTableCount();

	//SCTAB nAnz = Min( nTabCount, SCTAB(nToWhichTab+1) );
    SCTAB nAnz = nTabCount;
	SCTAB nStart = nTabsTested;
	if (!bValid)
	{
		nStart = 0;
		nTotalPages = 0;
		nTabsTested = 0;
	}

    // update all pending row heights with a single progress bar,
    // instead of a separate progress for each sheet from ScPrintFunc
    pDocShell->UpdatePendingRowHeights( nAnz-1, true );

	//	PrintOptions is passed to PrintFunc for SkipEmpty flag,
	//	but always all sheets are used (there is no selected sheet)
	ScPrintOptions aOptions = SC_MOD()->GetPrintOptions();

	for (SCTAB i=nStart; i<nAnz; i++)
	{
		long nAttrPage = i > 0 ? nFirstAttr[i-1] : 1;

		long nThisStart = nTotalPages;
		ScPrintFunc aPrintFunc( this, pDocShell, i, nAttrPage, 0, NULL, &aOptions );
		long nThisTab = aPrintFunc.GetTotalPages();
		nPages[i] = nThisTab;
		nTotalPages += nThisTab;
		nFirstAttr[i] = aPrintFunc.GetFirstPageNo();	// behalten oder aus Vorlage

		if (nPageNo>=nThisStart && nPageNo<nTotalPages)
		{
			nTab = i;
			nTabPage = nPageNo - nThisStart;
			nTabStart = nThisStart;

			aPrintFunc.GetPrintState( aState );
			aPageSize = aPrintFunc.GetPageSize();
		}
	}

	nDisplayStart = lcl_GetDisplayStart( nTab, pDoc, nPages );

	if (nAnz > nTabsTested)
		nTabsTested = nAnz;

	//	testen, ob hinter letzter Seite

	if ( nTabsTested >= nTabCount )
		TestLastPage();

	aState.nDocPages = nTotalPages;

	bValid = sal_True;
	bStateValid = sal_True;
	DoInvalidate();
}


void ScPreview::RecalcPages()					// nur nPageNo geaendert
{
	if (!bValid)
		return;							// dann wird CalcPages aufgerufen

	SCTAB nOldTab = nTab;

	sal_Bool bDone = sal_False;
	while (nPageNo >= nTotalPages && nTabsTested < nTabCount)
	{
		CalcPages( nTabsTested );
		bDone = sal_True;
	}

	if (!bDone)
	{
		long nPartPages = 0;
		for (SCTAB i=0; i<nTabsTested; i++)
		{
			long nThisStart = nPartPages;
			nPartPages += nPages[i];

			if (nPageNo>=nThisStart && nPageNo<nPartPages)
			{
				nTab = i;
				nTabPage = nPageNo - nThisStart;
				nTabStart = nThisStart;

//				aPageSize = aPrintFunc.GetPageSize();
			}
		}

		ScDocument* pDoc = pDocShell->GetDocument();
		nDisplayStart = lcl_GetDisplayStart( nTab, pDoc, nPages );
	}

	TestLastPage();			// testen, ob hinter letzter Seite

	if ( nTab != nOldTab )
		bStateValid = sal_False;

	DoInvalidate();
}


void ScPreview::DoPrint( ScPreviewLocationData* pFillLocation )
{
	if (!bValid)
	{
		CalcPages(0);
		RecalcPages();
		UpdateDrawView();		// Tabelle evtl. geaendert
	}

	Fraction aPreviewZoom( nZoom, 100 );
	Fraction aHorPrevZoom( (long)( 100 * nZoom / pDocShell->GetOutputFactor() ), 10000 );
	MapMode aMMMode( MAP_100TH_MM, Point(), aHorPrevZoom, aPreviewZoom );

	sal_Bool bDoPrint = ( pFillLocation == NULL );
	sal_Bool bValidPage = ( nPageNo < nTotalPages );

	ScModule* pScMod = SC_MOD();
    const svtools::ColorConfig& rColorCfg = pScMod->GetColorConfig();
    Color aBackColor( rColorCfg.GetColorValue(svtools::APPBACKGROUND).nColor );

	if ( bDoPrint && ( aOffset.X() < 0 || aOffset.Y() < 0 ) && bValidPage )
	{
		SetMapMode( aMMMode );
		SetLineColor();
		SetFillColor(aBackColor);

		Size aWinSize = GetOutputSize();
		if ( aOffset.X() < 0 )
			DrawRect(Rectangle( 0, 0, -aOffset.X(), aWinSize.Height() ));
		if ( aOffset.Y() < 0 )
			DrawRect(Rectangle( 0, 0, aWinSize.Width(), -aOffset.Y() ));
	}

    long   nLeftMargin = 0;
    long   nRightMargin = 0;
    long   nTopMargin = 0;
    long   nBottomMargin = 0;
    sal_Bool bHeaderOn = sal_False;
    sal_Bool bFooterOn = sal_False;

    ScDocument* pDoc = pDocShell->GetDocument();
    sal_Bool bLayoutRTL = pDoc->IsLayoutRTL( nTab );

	Size aLocalPageSize;
	if ( bValidPage )
	{
		ScPrintOptions aOptions = pScMod->GetPrintOptions();

		ScPrintFunc* pPrintFunc;
		if (bStateValid)
			pPrintFunc = new ScPrintFunc( this, pDocShell, aState, &aOptions );
		else
			pPrintFunc = new ScPrintFunc( this, pDocShell, nTab, nFirstAttr[nTab], nTotalPages, NULL, &aOptions );

		pPrintFunc->SetOffset(aOffset);
		pPrintFunc->SetManualZoom(nZoom);
		pPrintFunc->SetDateTime(aDate,aTime);
		pPrintFunc->SetClearFlag(sal_True);
		pPrintFunc->SetUseStyleColor( pScMod->GetAccessOptions().GetIsForPagePreviews() );

		pPrintFunc->SetDrawView( pDrawView );

		// MultiSelection fuer die eine Seite muss etwas umstaendlich erzeugt werden...
		Range aPageRange( nPageNo+1, nPageNo+1 );
		MultiSelection aPage( aPageRange );
		aPage.SetTotalRange( Range(0,RANGE_MAX) );
		aPage.Select( aPageRange );

        long nPrinted = pPrintFunc->DoPrint( aPage, nTabStart, nDisplayStart, bDoPrint, pFillLocation );
		DBG_ASSERT(nPrinted<=1, "was'n nu los?");

		SetMapMode(aMMMode);

        //init nLeftMargin ... in the ScPrintFunc::InitParam!!!
        nLeftMargin = pPrintFunc->GetLeftMargin();
        nRightMargin = pPrintFunc->GetRightMargin();
        nTopMargin = pPrintFunc->GetTopMargin();
        nBottomMargin = pPrintFunc->GetBottomMargin();
        nHeaderHeight = pPrintFunc->GetHeader().nHeight;
        nFooterHeight = pPrintFunc->GetFooter().nHeight;
        bHeaderOn = pPrintFunc->GetHeader().bEnable;
        bFooterOn = pPrintFunc->GetFooter().bEnable;
        mnScale = pPrintFunc->GetZoom();

        if ( bDoPrint && bPageMargin && pLocationData )     // don't make use of pLocationData while filling it
        {
            Rectangle aPixRect;
            Rectangle aRectCellPosition;
            Rectangle aRectPosition;
            pLocationData->GetMainCellRange( aPageArea, aPixRect );
            if( !bLayoutRTL )
            {
                pLocationData->GetCellPosition( aPageArea.aStart, aRectPosition );
                nLeftPosition = aRectPosition.Left();
                for( SCCOL i = aPageArea.aStart.Col(); i <= aPageArea.aEnd.Col(); i++ )
                {
                    pLocationData->GetCellPosition( ScAddress( i,aPageArea.aStart.Row(),aPageArea.aStart.Tab()),aRectCellPosition );
                    nRight[i] = aRectCellPosition.Right();
                }
            }
            else
            {
                pLocationData->GetCellPosition( aPageArea.aEnd, aRectPosition );
                nLeftPosition = aRectPosition.Right()+1;

                pLocationData->GetCellPosition( aPageArea.aStart,aRectCellPosition );
                nRight[ aPageArea.aEnd.Col() ] = aRectCellPosition.Left();
                for( SCCOL i = aPageArea.aEnd.Col(); i > aPageArea.aStart.Col(); i-- )
                {
                    pLocationData->GetCellPosition( ScAddress( i,aPageArea.aEnd.Row(),aPageArea.aEnd.Tab()),aRectCellPosition );
                    nRight[ i-1 ] = nRight[ i ] + aRectCellPosition.Right() - aRectCellPosition.Left() + 1;
                }
            }
        }

		if (nPrinted)	// wenn nichts, alles grau zeichnen
		{
			aLocalPageSize = pPrintFunc->GetPageSize();
			aLocalPageSize.Width()  = (long) (aLocalPageSize.Width()  * HMM_PER_TWIPS );
			aLocalPageSize.Height() = (long) (aLocalPageSize.Height() * HMM_PER_TWIPS );

            nLeftMargin = (long) ( nLeftMargin * HMM_PER_TWIPS );
            nRightMargin = (long) ( nRightMargin * HMM_PER_TWIPS );
            nTopMargin = (long) ( nTopMargin * HMM_PER_TWIPS );
            nBottomMargin = (long) ( nBottomMargin * HMM_PER_TWIPS );
            nHeaderHeight = (long) ( nHeaderHeight * HMM_PER_TWIPS * mnScale / 100 + nTopMargin );
            nFooterHeight = (long) ( nFooterHeight * HMM_PER_TWIPS * mnScale / 100 + nBottomMargin );
		}

		if (!bStateValid)
		{
			pPrintFunc->GetPrintState( aState );
			aState.nDocPages = nTotalPages;
			bStateValid = sal_True;
		}
		delete pPrintFunc;
	}

	if ( bDoPrint )
	{
		long nPageEndX = aLocalPageSize.Width()  - aOffset.X();
		long nPageEndY = aLocalPageSize.Height() - aOffset.Y();
		if ( !bValidPage )
			nPageEndX = nPageEndY = 0;

		Size aWinSize = GetOutputSize();
		Point aWinEnd( aWinSize.Width(), aWinSize.Height() );
		sal_Bool bRight  = nPageEndX <= aWinEnd.X();
		sal_Bool bBottom = nPageEndY <= aWinEnd.Y();

        if( bPageMargin && bValidPage )
        {
            SetMapMode(aMMMode);
            SetLineColor( COL_BLACK );
            DrawInvert( (long)( nTopMargin - aOffset.Y() ), POINTER_VSIZEBAR );
            DrawInvert( (long)(nPageEndY - nBottomMargin ), POINTER_VSIZEBAR );
            DrawInvert( (long)( nLeftMargin - aOffset.X() ), POINTER_HSIZEBAR );
            DrawInvert( (long)( nPageEndX - nRightMargin ) , POINTER_HSIZEBAR );
            if( bHeaderOn )
            {
                DrawInvert( nHeaderHeight - aOffset.Y(), POINTER_VSIZEBAR );
            }
            if( bFooterOn )
            {
                DrawInvert( nPageEndY - nFooterHeight, POINTER_VSIZEBAR );
            }

            SetMapMode( MapMode( MAP_PIXEL ) );
            for( int i= aPageArea.aStart.Col(); i<= aPageArea.aEnd.Col(); i++ )
            {
                Point aColumnTop = LogicToPixel( Point( 0, -aOffset.Y() ) ,aMMMode );
                SetLineColor( COL_BLACK );
                SetFillColor( COL_BLACK );
                DrawRect( Rectangle( Point( nRight[i] - 2, aColumnTop.Y() ),Point( nRight[i] + 2 , 4 + aColumnTop.Y()) ));
                DrawLine( Point( nRight[i], aColumnTop.Y() ), Point( nRight[i],  10 + aColumnTop.Y()) );
            }
            SetMapMode( aMMMode );
        }

		if (bRight || bBottom)
		{
            SetMapMode(aMMMode);
			SetLineColor();
			SetFillColor(aBackColor);
			if (bRight)
				DrawRect(Rectangle(nPageEndX,0, aWinEnd.X(),aWinEnd.Y()));
			if (bBottom)
			{
				if (bRight)
					DrawRect(Rectangle(0,nPageEndY, nPageEndX,aWinEnd.Y()));	// Ecke nicht doppelt
				else
					DrawRect(Rectangle(0,nPageEndY, aWinEnd.X(),aWinEnd.Y()));
			}
		}

		if ( bValidPage )
		{
            Color aBorderColor( SC_MOD()->GetColorConfig().GetColorValue(svtools::FONTCOLOR).nColor );

			//	draw border

			if ( aOffset.X() <= 0 || aOffset.Y() <= 0 || bRight || bBottom )
			{
				SetLineColor( aBorderColor );
				SetFillColor();

				Rectangle aPixel( LogicToPixel( Rectangle( -aOffset.X(), -aOffset.Y(), nPageEndX, nPageEndY ) ) );
				--aPixel.Right();
				--aPixel.Bottom();
				DrawRect( PixelToLogic( aPixel ) );
			}

			//	draw shadow

//			SetLineColor();
//			SetFillColor( aBorderColor );

//			Rectangle aPixel;

//			aPixel = LogicToPixel( Rectangle( nPageEndX, -aOffset.Y(), nPageEndX, nPageEndY ) );
//			aPixel.Top() += SC_PREVIEW_SHADOWSIZE;
//			aPixel.Right() += SC_PREVIEW_SHADOWSIZE - 1;
//			aPixel.Bottom() += SC_PREVIEW_SHADOWSIZE - 1;
//			DrawRect( PixelToLogic( aPixel ) );

//			aPixel = LogicToPixel( Rectangle( -aOffset.X(), nPageEndY, nPageEndX, nPageEndY ) );
//			aPixel.Left() += SC_PREVIEW_SHADOWSIZE;
//			aPixel.Right() += SC_PREVIEW_SHADOWSIZE - 1;
//			aPixel.Bottom() += SC_PREVIEW_SHADOWSIZE - 1;
//			DrawRect( PixelToLogic( aPixel ) );
		}
	}
}

//Issue51656 Add resizeable margin on page preview from maoyg
void __EXPORT ScPreview::Paint( const Rectangle& /* rRect */ )
{
    bool bWasInPaint = bInPaint;        // nested calls shouldn't be necessary, but allow for now
    bInPaint = true;

    if (bPageMargin)
        GetLocationData();              // fill location data for column positions
    DoPrint( NULL );
	pViewShell->UpdateScrollBars();

    bInPaint = bWasInPaint;
}
//Issue51656 Add resizeable margin on page preview from maoyg

void __EXPORT ScPreview::Command( const CommandEvent& rCEvt )
{
	sal_uInt16 nCmd = rCEvt.GetCommand();
	if ( nCmd == COMMAND_WHEEL || nCmd == COMMAND_STARTAUTOSCROLL || nCmd == COMMAND_AUTOSCROLL )
	{
		sal_Bool bDone = pViewShell->ScrollCommand( rCEvt );
		if (!bDone)
			Window::Command(rCEvt);
	}
	else if ( nCmd == COMMAND_CONTEXTMENU )
		SfxDispatcher::ExecutePopup();
	else
		Window::Command( rCEvt );
}


void __EXPORT ScPreview::KeyInput( const KeyEvent& rKEvt )
{
    //  The + and - keys can't be configured as accelerator entries, so they must be handled directly
    //  (in ScPreview, not ScPreviewShell -> only if the preview window has the focus)

    const KeyCode& rKeyCode = rKEvt.GetKeyCode();
    sal_uInt16 nKey = rKeyCode.GetCode();
    sal_Bool bHandled = sal_False;
    if(!rKeyCode.GetModifier())
    {
        sal_uInt16 nSlot = 0;
        switch(nKey)
        {
            case KEY_ADD:      nSlot = SID_PREVIEW_ZOOMIN;  break;
            case KEY_ESCAPE:   nSlot = ScViewUtil::IsFullScreen( *pViewShell ) ? SID_CANCEL : SID_PREVIEW_CLOSE; break;
            case KEY_SUBTRACT: nSlot = SID_PREVIEW_ZOOMOUT; break;
        }
        if(nSlot)
        {
            bHandled = sal_True;
            pViewShell->GetViewFrame()->GetDispatcher()->Execute( nSlot, SFX_CALLMODE_ASYNCHRON );
        }
    }

    if ( !bHandled && !pViewShell->KeyInput(rKEvt) )
        Window::KeyInput(rKEvt);
}


const ScPreviewLocationData& ScPreview::GetLocationData()
{
	if ( !pLocationData )
	{
		pLocationData = new ScPreviewLocationData( pDocShell->GetDocument(), this );
		bLocationValid = sal_False;
	}
	if ( !bLocationValid )
	{
		pLocationData->Clear();
		DoPrint( pLocationData );
		bLocationValid = sal_True;
	}
	return *pLocationData;
}


void ScPreview::DataChanged(sal_Bool bNewTime)
{
	if (bNewTime)
	{
		aDate = Date();
		aTime = Time();
	}

	bValid = sal_False;
	InvalidateLocationData( SC_HINT_DATACHANGED );
	Invalidate();
}


String ScPreview::GetPosString()
{
	if (!bValid)
	{
		CalcPages(nTab);
		UpdateDrawView();		// Tabelle evtl. geaendert
	}

	String aString( ScGlobal::GetRscString( STR_PAGE ) );
	aString += ' ';
	aString += String::CreateFromInt32(nPageNo+1);

	if (nTabsTested >= nTabCount)
	{
		aString.AppendAscii(RTL_CONSTASCII_STRINGPARAM( " / " ));
		aString += String::CreateFromInt32(nTotalPages);
	}

	return aString;
}


void ScPreview::SetZoom(sal_uInt16 nNewZoom)
{
	if (nNewZoom < 20)
		nNewZoom = 20;
	if (nNewZoom > 400)
		nNewZoom = 400;
	if (nNewZoom != nZoom)
	{
		nZoom = nNewZoom;

		//	apply new MapMode and call UpdateScrollBars to update aOffset

		Fraction aPreviewZoom( nZoom, 100 );
		Fraction aHorPrevZoom( (long)( 100 * nZoom / pDocShell->GetOutputFactor() ), 10000 );
		MapMode aMMMode( MAP_100TH_MM, Point(), aHorPrevZoom, aPreviewZoom );
		SetMapMode( aMMMode );

        bInSetZoom = true;              // don't scroll during SetYOffset in UpdateScrollBars
		pViewShell->UpdateScrollBars();
        bInSetZoom = false;

		bStateValid = sal_False;
		InvalidateLocationData( SC_HINT_ACC_VISAREACHANGED );
		DoInvalidate();
		Invalidate();
	}
}


void ScPreview::SetPageNo( long nPage )
{
	nPageNo = nPage;
	RecalcPages();
	UpdateDrawView();		// Tabelle evtl. geaendert
	InvalidateLocationData( SC_HINT_DATACHANGED );
	Invalidate();
}


long ScPreview::GetFirstPage(SCTAB nTabP)
{
	SCTAB nDocTabCount = pDocShell->GetDocument()->GetTableCount();
	if (nTabP >= nDocTabCount)
		nTabP = nDocTabCount-1;

	long nPage = 0;
	if (nTabP>0)
	{
		CalcPages( nTabP );
		UpdateDrawView();		// Tabelle evtl. geaendert

		for (SCTAB i=0; i<nTabP; i++)
			nPage += nPages[i];

		// bei leerer Tabelle vorhergehende Seite

		if ( nPages[nTabP]==0 && nPage > 0 )
			--nPage;
	}

	return nPage;
}


Size lcl_GetDocPageSize( ScDocument* pDoc, SCTAB nTab )
{
	String aName = pDoc->GetPageStyle( nTab );
	ScStyleSheetPool* pStylePool = pDoc->GetStyleSheetPool();
	SfxStyleSheetBase* pStyleSheet = pStylePool->Find( aName, SFX_STYLE_FAMILY_PAGE );
	if ( pStyleSheet )
	{
		SfxItemSet& rStyleSet = pStyleSheet->GetItemSet();
		return ((const SvxSizeItem&) rStyleSet.Get(ATTR_PAGE_SIZE)).GetSize();
	}
	else
	{
		DBG_ERROR( "PageStyle not found" );
		return Size();
	}
}


sal_uInt16 ScPreview::GetOptimalZoom(sal_Bool bWidthOnly)
{
	double nWinScaleX = ScGlobal::nScreenPPTX / pDocShell->GetOutputFactor();
	double nWinScaleY = ScGlobal::nScreenPPTY;
	Size aWinSize = GetOutputSizePixel();

	//	desired margin is 0.25cm in default MapMode (like Writer),
	//	but some additional margin is introduced by integer scale values
	//	-> add only 0.10cm, so there is some margin in all cases.
	Size aMarginSize( LogicToPixel( Size( 100, 100 ), MAP_100TH_MM ) );
	aWinSize.Width()  -= 2 * aMarginSize.Width();
	aWinSize.Height() -= 2 * aMarginSize.Height();

	Size aLocalPageSize = lcl_GetDocPageSize( pDocShell->GetDocument(), nTab );
	if ( aLocalPageSize.Width() && aLocalPageSize.Height() )
	{
		long nZoomX = (long) ( aWinSize.Width() * 100  / ( aLocalPageSize.Width() * nWinScaleX ));
		long nZoomY = (long) ( aWinSize.Height() * 100 / ( aLocalPageSize.Height() * nWinScaleY ));

		long nOptimal = nZoomX;
		if (!bWidthOnly && nZoomY<nOptimal)
			nOptimal = nZoomY;

		if (nOptimal<20)
			nOptimal = 20;
		if (nOptimal>400)
			nOptimal = 400;

		return (sal_uInt16) nOptimal;
	}
	else
		return nZoom;
}


void ScPreview::SetXOffset( long nX )
{
	if ( aOffset.X() == nX )
		return;

	if (bValid)
	{
		long nDif = LogicToPixel(aOffset).X() - LogicToPixel(Point(nX,0)).X();
		aOffset.X() = nX;
        if (nDif && !bInSetZoom)
		{
			MapMode aOldMode = GetMapMode(); SetMapMode(MAP_PIXEL);
			Scroll( nDif, 0 );
			SetMapMode(aOldMode);
		}
	}
	else
	{
		aOffset.X() = nX;
        if (!bInSetZoom)
			Invalidate();
	}
	InvalidateLocationData( SC_HINT_ACC_VISAREACHANGED );
    Paint(Rectangle());
}


void ScPreview::SetYOffset( long nY )
{
	if ( aOffset.Y() == nY )
		return;

	if (bValid)
	{
		long nDif = LogicToPixel(aOffset).Y() - LogicToPixel(Point(0,nY)).Y();
		aOffset.Y() = nY;
        if (nDif && !bInSetZoom)
		{
			MapMode aOldMode = GetMapMode(); SetMapMode(MAP_PIXEL);
			Scroll( 0, nDif );
			SetMapMode(aOldMode);
		}
	}
	else
	{
		aOffset.Y() = nY;
        if (!bInSetZoom)
			Invalidate();
	}
	InvalidateLocationData( SC_HINT_ACC_VISAREACHANGED );
    Paint(Rectangle());
}


void ScPreview::DoInvalidate()
{
	//	Wenn das ganze aus dem GetState der Shell gerufen wird,
	//	muss das Invalidate hinterher asynchron kommen...

	if (bInGetState)
		Application::PostUserEvent( STATIC_LINK( this, ScPreview, InvalidateHdl ) );
	else
		StaticInvalidate();		// sofort
}

void ScPreview::StaticInvalidate()
{
	//	static method, because it's called asynchronously
	//	-> must use current viewframe

	SfxViewFrame* pViewFrm = SfxViewFrame::Current();
	if (!pViewFrm)
		return;

	SfxBindings& rBindings = pViewFrm->GetBindings();
	rBindings.Invalidate(SID_STATUS_DOCPOS);
	rBindings.Invalidate(SID_STATUS_PAGESTYLE);
	rBindings.Invalidate(SID_PREVIEW_PREVIOUS);
	rBindings.Invalidate(SID_PREVIEW_NEXT);
	rBindings.Invalidate(SID_PREVIEW_FIRST);
	rBindings.Invalidate(SID_PREVIEW_LAST);
	rBindings.Invalidate(SID_ATTR_ZOOM);
	rBindings.Invalidate(SID_PREVIEW_ZOOMIN);
	rBindings.Invalidate(SID_PREVIEW_ZOOMOUT);
    rBindings.Invalidate(SID_PREVIEW_SCALINGFACTOR);
    rBindings.Invalidate(SID_ATTR_ZOOMSLIDER);
}

IMPL_STATIC_LINK( ScPreview, InvalidateHdl, void*, EMPTYARG )
{
    (void)pThis;    // avoid warning

	StaticInvalidate();
	return 0;
}

void ScPreview::DataChanged( const DataChangedEvent& rDCEvt )
{
	Window::DataChanged(rDCEvt);

	if ( (rDCEvt.GetType() == DATACHANGED_PRINTER) ||
		 (rDCEvt.GetType() == DATACHANGED_DISPLAY) ||
		 (rDCEvt.GetType() == DATACHANGED_FONTS) ||
		 (rDCEvt.GetType() == DATACHANGED_FONTSUBSTITUTION) ||
		 ((rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
		  (rDCEvt.GetFlags() & SETTINGS_STYLE)) )
	{
		if ( rDCEvt.GetType() == DATACHANGED_FONTS )
			pDocShell->UpdateFontList();

        // #i114518# Paint of form controls may modify the window's settings.
        // Ignore the event if it is called from within Paint.
        if ( !bInPaint )
        {
            if ( rDCEvt.GetType() == DATACHANGED_SETTINGS &&
                  (rDCEvt.GetFlags() & SETTINGS_STYLE) )
            {
                //  scroll bar size may have changed
                pViewShell->InvalidateBorder();     // calls OuterResizePixel
            }

            Invalidate();
            InvalidateLocationData( SC_HINT_DATACHANGED );
        }
	}
}

//Issue51656 Add resizeable margin on page preview from maoyg
void __EXPORT ScPreview::MouseButtonDown( const MouseEvent& rMEvt )
{
    Fraction  aPreviewZoom( nZoom, 100 );
    Fraction  aHorPrevZoom( (long)( 100 * nZoom / pDocShell->GetOutputFactor() ), 10000 );
    MapMode   aMMMode( MAP_100TH_MM, Point(), aHorPrevZoom, aPreviewZoom );

    aButtonDownChangePoint = PixelToLogic( rMEvt.GetPosPixel(),aMMMode );
    aButtonDownPt = PixelToLogic( rMEvt.GetPosPixel(),aMMMode );

    CaptureMouse();

    if( rMEvt.IsLeft() && GetPointer() == POINTER_HSIZEBAR )
    {
        SetMapMode( aMMMode );
        if( bLeftRulerChange )
        {
           DrawInvert( aButtonDownChangePoint.X(), POINTER_HSIZEBAR );
           bLeftRulerMove = sal_True;
           bRightRulerMove = sal_False;
        }
        else if( bRightRulerChange )
        {
           DrawInvert( aButtonDownChangePoint.X(), POINTER_HSIZEBAR );
           bLeftRulerMove = sal_False;
           bRightRulerMove = sal_True;
        }
    }

    if( rMEvt.IsLeft() && GetPointer() == POINTER_VSIZEBAR )
    {
        SetMapMode( aMMMode );
        if( bTopRulerChange )
        {
            DrawInvert( aButtonDownChangePoint.Y(), POINTER_VSIZEBAR );
            bTopRulerMove = sal_True;
            bBottomRulerMove = sal_False;
        }
        else if( bBottomRulerChange )
        {
            DrawInvert( aButtonDownChangePoint.Y(), POINTER_VSIZEBAR );
            bTopRulerMove = sal_False;
            bBottomRulerMove = sal_True;
        }
        else if( bHeaderRulerChange )
        {
            DrawInvert( aButtonDownChangePoint.Y(), POINTER_VSIZEBAR );
            bHeaderRulerMove = sal_True;
            bFooterRulerMove = sal_False;
        }
        else if( bFooterRulerChange )
        {
            DrawInvert( aButtonDownChangePoint.Y(), POINTER_VSIZEBAR );
            bHeaderRulerMove = sal_False;
            bFooterRulerMove = sal_True;
        }
    }

    if( rMEvt.IsLeft() && GetPointer() == POINTER_HSPLIT )
    {
        Point  aNowPt = rMEvt.GetPosPixel();
        SCCOL i = 0;
        for( i = aPageArea.aStart.Col(); i<= aPageArea.aEnd.Col(); i++ )
        {
            if( aNowPt.X() < nRight[i] + 2 && aNowPt.X() > nRight[i] - 2 )
            {
                nColNumberButttonDown = i;
                break;
            }
        }
        if( i == aPageArea.aEnd.Col()+1 )
            return;

        SetMapMode( aMMMode );
        if( nColNumberButttonDown == aPageArea.aStart.Col() )
            DrawInvert( PixelToLogic( Point( nLeftPosition, 0 ),aMMMode ).X() ,POINTER_HSPLIT );
        else
            DrawInvert( PixelToLogic( Point( nRight[ nColNumberButttonDown-1 ], 0 ),aMMMode ).X() ,POINTER_HSPLIT );

        DrawInvert( aButtonDownChangePoint.X(), POINTER_HSPLIT );
        bColRulerMove = sal_True;
    }
}

void __EXPORT ScPreview::MouseButtonUp( const MouseEvent& rMEvt )
{
        Fraction  aPreviewZoom( nZoom, 100 );
        Fraction  aHorPrevZoom( (long)( 100 * nZoom / pDocShell->GetOutputFactor() ), 10000 );
        MapMode   aMMMode( MAP_100TH_MM, Point(), aHorPrevZoom, aPreviewZoom );

        aButtonUpPt = PixelToLogic( rMEvt.GetPosPixel(),aMMMode );

        long  nWidth = (long) lcl_GetDocPageSize(pDocShell->GetDocument(), nTab).Width();
        long  nHeight = (long) lcl_GetDocPageSize(pDocShell->GetDocument(), nTab).Height();

        if( rMEvt.IsLeft() && GetPointer() == POINTER_HSIZEBAR )
        {
            SetPointer( Pointer( POINTER_ARROW ) );

            sal_Bool bMoveRulerAction= sal_True;

            ScDocument * pDoc = pDocShell->GetDocument();
            String   aOldName = pDoc->GetPageStyle( nTab );
            sal_Bool  bUndo( pDoc->IsUndoEnabled() );
            ScStyleSheetPool* pStylePool = pDoc->GetStyleSheetPool();
            SfxStyleSheetBase* pStyleSheet = pStylePool->Find( aOldName, SFX_STYLE_FAMILY_PAGE );

            if ( pStyleSheet )
            {
                ScStyleSaveData aOldData;
                if( bUndo )
                    aOldData.InitFromStyle( pStyleSheet );

                SfxItemSet&  rStyleSet = pStyleSheet->GetItemSet();

                SvxLRSpaceItem aLRItem = ( const SvxLRSpaceItem& ) rStyleSet.Get( ATTR_LRSPACE );

                if(( bLeftRulerChange || bRightRulerChange ) && ( aButtonUpPt.X() <= ( 0 - aOffset.X() ) || aButtonUpPt.X() > nWidth * HMM_PER_TWIPS - aOffset.X() ) )
                {
                    bMoveRulerAction = sal_False;
                    Paint(Rectangle(0,0,10000,10000));
                }
                else if( bLeftRulerChange && ( aButtonUpPt.X() / HMM_PER_TWIPS > nWidth - aLRItem.GetRight() - aOffset.X() / HMM_PER_TWIPS ) )
                {
                    bMoveRulerAction = sal_False;
                    Paint(Rectangle(0,0,10000,10000));
                }
                else if( bRightRulerChange && ( aButtonUpPt.X() / HMM_PER_TWIPS < aLRItem.GetLeft() - aOffset.X() / HMM_PER_TWIPS ) )
                {
                    bMoveRulerAction = sal_False;
                    Paint(Rectangle(0,0,10000,10000));
                }
                else if( aButtonDownPt.X() == aButtonUpPt.X() )
                {
                    bMoveRulerAction = sal_False;
                    DrawInvert( aButtonUpPt.X(), POINTER_HSIZEBAR );
                }
                if( bMoveRulerAction )
                {
                    ScDocShellModificator aModificator( *pDocShell );
                    if( bLeftRulerChange && bLeftRulerMove )
                    {
                       aLRItem.SetLeft( (long)( aButtonUpPt.X() / HMM_PER_TWIPS + aOffset.X() / HMM_PER_TWIPS ));
                       rStyleSet.Put( aLRItem );
                    }
                    else if( bRightRulerChange && bRightRulerMove )
                    {
                        aLRItem.SetRight( (long)( nWidth - aButtonUpPt.X() / HMM_PER_TWIPS - aOffset.X() / HMM_PER_TWIPS ));
                        rStyleSet.Put( aLRItem );
                    }

                    ScStyleSaveData aNewData;
                    aNewData.InitFromStyle( pStyleSheet );
                    if( bUndo )
                    {
                        pDocShell->GetUndoManager()->AddUndoAction(
                            new ScUndoModifyStyle( pDocShell, SFX_STYLE_FAMILY_PAGE,
                            aOldData, aNewData ) );
                    }

                    if ( ValidTab( nTab ) )
                    {
                        ScPrintFunc aPrintFunc( this, pDocShell, nTab );
                        aPrintFunc.UpdatePages();
                    }

                    Rectangle aRect(0,0,10000,10000);
                    Paint( aRect );
                    aModificator.SetDocumentModified();
                    bLeftRulerChange = sal_False;
                    bRightRulerChange = sal_False;
                }
            }
            bLeftRulerMove = sal_False;
            bRightRulerMove = sal_False;
        }

        if( rMEvt.IsLeft() && GetPointer() == POINTER_VSIZEBAR )
        {
            SetPointer( POINTER_ARROW );

            sal_Bool  bMoveRulerAction = sal_True;
            if( ( bTopRulerChange || bBottomRulerChange || bHeaderRulerChange || bFooterRulerChange ) && ( aButtonUpPt.Y() <= ( 0 - aOffset.Y() ) || aButtonUpPt.Y() > nHeight * HMM_PER_TWIPS -aOffset.Y() ) )
            {
                bMoveRulerAction = sal_False;
                Paint( Rectangle(0,0,10000,10000) );
            }
            else if( aButtonDownPt.Y() == aButtonUpPt.Y() )
            {
                bMoveRulerAction = sal_False;
                DrawInvert( aButtonUpPt.Y(), POINTER_VSIZEBAR );
            }
            if( bMoveRulerAction )
            {
                ScDocument * pDoc = pDocShell->GetDocument();
                sal_Bool  bUndo( pDoc->IsUndoEnabled() );
                ScStyleSheetPool* pStylePool = pDoc->GetStyleSheetPool();
                SfxStyleSheetBase* pStyleSheet = pStylePool->Find( pDoc->GetPageStyle( nTab ), SFX_STYLE_FAMILY_PAGE );
                DBG_ASSERT( pStyleSheet, "PageStyle not found" );
                if ( pStyleSheet )
                {
                    ScDocShellModificator aModificator( *pDocShell );
                    ScStyleSaveData aOldData;
                    if( bUndo )
                        aOldData.InitFromStyle( pStyleSheet );

                    SfxItemSet& rStyleSet = pStyleSheet->GetItemSet();

                    SvxULSpaceItem aULItem = ( const SvxULSpaceItem&)rStyleSet.Get( ATTR_ULSPACE );

                    if( bTopRulerMove && bTopRulerChange )
                    {
                        aULItem.SetUpperValue( (sal_uInt16)( aButtonUpPt.Y() / HMM_PER_TWIPS + aOffset.Y() / HMM_PER_TWIPS ) );
                        rStyleSet.Put( aULItem );
                    }
                    else if( bBottomRulerMove && bBottomRulerChange )
                    {
                        aULItem.SetLowerValue( (sal_uInt16)( nHeight - aButtonUpPt.Y() / HMM_PER_TWIPS - aOffset.Y() / HMM_PER_TWIPS ) );
                        rStyleSet.Put( aULItem );
                    }
                    else if( bHeaderRulerMove && bHeaderRulerChange )
                    {
                        const SfxPoolItem* pItem = NULL;
                        if ( rStyleSet.GetItemState( ATTR_PAGE_HEADERSET, sal_False, &pItem ) == SFX_ITEM_SET )
                        {
                            SfxItemSet& pHeaderSet = ((SvxSetItem*)pItem)->GetItemSet();
                            Size  aHeaderSize = ((const SvxSizeItem&)pHeaderSet.Get(ATTR_PAGE_SIZE)).GetSize();
                            aHeaderSize.Height() = (long)( aButtonUpPt.Y() / HMM_PER_TWIPS + aOffset.Y() / HMM_PER_TWIPS - aULItem.GetUpper());
                            aHeaderSize.Height() = aHeaderSize.Height() * 100 / mnScale;
                            SvxSetItem  aNewHeader( (const SvxSetItem&)rStyleSet.Get(ATTR_PAGE_HEADERSET) );
                            aNewHeader.GetItemSet().Put( SvxSizeItem( ATTR_PAGE_SIZE, aHeaderSize ) );
                            rStyleSet.Put( aNewHeader );
                        }
                    }
                    else if( bFooterRulerMove && bFooterRulerChange )
                    {
                        const SfxPoolItem* pItem = NULL;
                        if( rStyleSet.GetItemState( ATTR_PAGE_FOOTERSET, sal_False, &pItem ) == SFX_ITEM_SET )
                        {
                            SfxItemSet& pFooterSet = ((SvxSetItem*)pItem)->GetItemSet();
                            Size aFooterSize = ((const SvxSizeItem&)pFooterSet.Get(ATTR_PAGE_SIZE)).GetSize();
                            aFooterSize.Height() = (long)( nHeight - aButtonUpPt.Y() / HMM_PER_TWIPS - aOffset.Y() / HMM_PER_TWIPS - aULItem.GetLower() );
                            aFooterSize.Height() = aFooterSize.Height() * 100 / mnScale;
                            SvxSetItem  aNewFooter( (const SvxSetItem&)rStyleSet.Get(ATTR_PAGE_FOOTERSET) );
                            aNewFooter.GetItemSet().Put( SvxSizeItem( ATTR_PAGE_SIZE, aFooterSize ) );
                            rStyleSet.Put( aNewFooter );
                        }
                    }

                    ScStyleSaveData aNewData;
                    aNewData.InitFromStyle( pStyleSheet );
                    if( bUndo )
                    {
                        pDocShell->GetUndoManager()->AddUndoAction(
                            new ScUndoModifyStyle( pDocShell, SFX_STYLE_FAMILY_PAGE,
                            aOldData, aNewData ) );
                    }

                    if ( ValidTab( nTab ) )
                    {
                        ScPrintFunc aPrintFunc( this, pDocShell, nTab );
                        aPrintFunc.UpdatePages();
                    }

                    Rectangle  aRect(0,0,10000,10000);
                    Paint( aRect );
                    aModificator.SetDocumentModified();
                    bTopRulerChange = sal_False;
                    bBottomRulerChange = sal_False;
                    bHeaderRulerChange = sal_False;
                    bFooterRulerChange = sal_False;
                 }
              }
              bTopRulerMove = sal_False;
              bBottomRulerMove = sal_False;
              bHeaderRulerMove = sal_False;
              bFooterRulerMove = sal_False;
        }
        if( rMEvt.IsLeft() && GetPointer() == POINTER_HSPLIT )
        {
            SetPointer(POINTER_ARROW);
            ScDocument* pDoc = pDocShell->GetDocument();
            sal_Bool  bLayoutRTL = pDoc->IsLayoutRTL( nTab );
            sal_Bool  bMoveRulerAction = sal_True;
            if( aButtonDownPt.X() == aButtonUpPt.X() )
            {
                bMoveRulerAction = sal_False;
                if( nColNumberButttonDown == aPageArea.aStart.Col() )
                    DrawInvert( PixelToLogic( Point( nLeftPosition, 0 ),aMMMode ).X() ,POINTER_HSPLIT );
                else
                    DrawInvert( PixelToLogic( Point( nRight[ nColNumberButttonDown-1 ], 0 ),aMMMode ).X() ,POINTER_HSPLIT );
                DrawInvert( aButtonUpPt.X(), POINTER_HSPLIT );
            }
            if( bMoveRulerAction )
            {
                long  nNewColWidth = 0;
                ScDocFunc aFunc(*pDocShell);
                SCCOLROW nCols[2] = { nColNumberButttonDown, nColNumberButttonDown };

                if( !bLayoutRTL )
                {
                    nNewColWidth = (long) ( PixelToLogic( Point( rMEvt.GetPosPixel().X() - nRight[ nColNumberButttonDown ], 0), aMMMode ).X() / HMM_PER_TWIPS ) * 100 / mnScale;
                    nNewColWidth += pDocShell->GetDocument()->GetColWidth( nColNumberButttonDown, nTab );
                }
                else
                {

                    nNewColWidth = (long) ( PixelToLogic( Point( nRight[ nColNumberButttonDown ] - rMEvt.GetPosPixel().X(), 0), aMMMode ).X() / HMM_PER_TWIPS ) * 100 / mnScale;
                    nNewColWidth += pDocShell->GetDocument()->GetColWidth( nColNumberButttonDown, nTab );
                }

                if( nNewColWidth >= 0 )
                {
                    aFunc.SetWidthOrHeight( sal_True, 1,nCols, nTab, SC_SIZE_DIRECT, (sal_uInt16)nNewColWidth, sal_True, sal_True);
                }
                if ( ValidTab( nTab ) )
                {
                    ScPrintFunc aPrintFunc( this, pDocShell, nTab );
                    aPrintFunc.UpdatePages();
                }
                Rectangle  nRect(0,0,10000,10000);
                Paint( nRect );
            }
            bColRulerMove = sal_False;
        }
        ReleaseMouse();
}

void __EXPORT ScPreview::MouseMove( const MouseEvent& rMEvt )
{
    Fraction aPreviewZoom( nZoom, 100 );
    Fraction aHorPrevZoom( (long)( 100 * nZoom / pDocShell->GetOutputFactor() ), 10000 );
    MapMode  aMMMode( MAP_100TH_MM, Point(), aHorPrevZoom, aPreviewZoom );
    Point    aMouseMovePoint = PixelToLogic( rMEvt.GetPosPixel(), aMMMode );

    long    nLeftMargin = 0;
    long    nRightMargin = 0;
    long    nTopMargin = 0;
    long    nBottomMargin = 0;
    Size    PageSize;

    long    nWidth = (long) lcl_GetDocPageSize(pDocShell->GetDocument(), nTab).Width();
    long    nHeight = (long) lcl_GetDocPageSize(pDocShell->GetDocument(), nTab).Height();

    if ( nPageNo < nTotalPages )
    {
        ScPrintOptions aOptions = SC_MOD()->GetPrintOptions();

        ScPrintFunc* pPrintFunc;

        if (bStateValid)
            pPrintFunc = new ScPrintFunc( this, pDocShell, aState, &aOptions );
        else
            pPrintFunc = new ScPrintFunc( this, pDocShell, nTab, nFirstAttr[nTab], nTotalPages, NULL, &aOptions );

        nLeftMargin = (long)( pPrintFunc->GetLeftMargin() * HMM_PER_TWIPS - aOffset.X() );
        nRightMargin = (long)( pPrintFunc->GetRightMargin() * HMM_PER_TWIPS );
        nRightMargin = (long)( nWidth * HMM_PER_TWIPS - nRightMargin - aOffset.X() );
        nTopMargin = (long)( pPrintFunc->GetTopMargin() * HMM_PER_TWIPS - aOffset.Y() );
        nBottomMargin = (long)( pPrintFunc->GetBottomMargin() * HMM_PER_TWIPS );
        nBottomMargin = (long)( nHeight * HMM_PER_TWIPS - nBottomMargin - aOffset.Y() );
        if( mnScale > 0 )
        {
            nHeaderHeight = (long)( nTopMargin + pPrintFunc->GetHeader().nHeight * HMM_PER_TWIPS * mnScale / 100 );
            nFooterHeight = (long)( nBottomMargin - pPrintFunc->GetFooter().nHeight * HMM_PER_TWIPS * mnScale / 100 );
        }
        else
        {
            nHeaderHeight = (long)( nTopMargin + pPrintFunc->GetHeader().nHeight * HMM_PER_TWIPS );
            nFooterHeight = (long)( nBottomMargin - pPrintFunc->GetFooter().nHeight * HMM_PER_TWIPS );
        }
        delete pPrintFunc;
    }

    Point   aPixPt( rMEvt.GetPosPixel() );
    Point   aLeftTop = LogicToPixel( Point( nLeftMargin, -aOffset.Y() ) , aMMMode );
    Point   aLeftBottom = LogicToPixel( Point( nLeftMargin ,(long)(nHeight * HMM_PER_TWIPS - aOffset.Y()) ), aMMMode );
    Point   aRightTop = LogicToPixel( Point( nRightMargin, -aOffset.Y() ), aMMMode );
    Point   aTopLeft = LogicToPixel( Point( -aOffset.X(), nTopMargin ), aMMMode );
    Point   aTopRight = LogicToPixel( Point( (long)(nWidth * HMM_PER_TWIPS - aOffset.X()), nTopMargin ), aMMMode );
    Point   aBottomLeft = LogicToPixel( Point( -aOffset.X(), nBottomMargin ), aMMMode );
    Point   aHeaderLeft = LogicToPixel( Point(  -aOffset.X(), nHeaderHeight ), aMMMode );
    Point   aFooderLeft = LogicToPixel( Point( -aOffset.X(), nFooterHeight ), aMMMode );

    sal_Bool   bOnColRulerChange = sal_False;

    for( SCCOL i=aPageArea.aStart.Col(); i<= aPageArea.aEnd.Col(); i++ )
    {
        Point   aColumnTop = LogicToPixel( Point( 0, -aOffset.Y() ) ,aMMMode );
        Point   aColumnBottom = LogicToPixel( Point( 0, (long)( nHeight * HMM_PER_TWIPS - aOffset.Y()) ), aMMMode );
        if( aPixPt.X() < ( nRight[i] + 2 ) && ( aPixPt.X() > ( nRight[i] - 2 ) ) && ( aPixPt.X() < aRightTop.X() ) && ( aPixPt.X() > aLeftTop.X() )
            && ( aPixPt.Y() > aColumnTop.Y() ) && ( aPixPt.Y() < aColumnBottom.Y() ) && !bLeftRulerMove && !bRightRulerMove
            && !bTopRulerMove && !bBottomRulerMove && !bHeaderRulerMove && !bFooterRulerMove )
        {
            bOnColRulerChange = sal_True;
            if( !rMEvt.GetButtons() && GetPointer() == POINTER_HSPLIT )
                nColNumberButttonDown = i;
            break;
        }
    }

    if( aPixPt.X() < ( aLeftTop.X() + 2 ) && aPixPt.X() > ( aLeftTop.X() - 2 ) && !bRightRulerMove )
    {
        bLeftRulerChange = sal_True;
        bRightRulerChange = sal_False;
    }
    else if( aPixPt.X() < ( aRightTop.X() + 2 ) && aPixPt.X() > ( aRightTop.X() - 2 ) && !bLeftRulerMove )
    {
        bLeftRulerChange = sal_False;
        bRightRulerChange = sal_True;
    }
    else if( aPixPt.Y() < ( aTopLeft.Y() + 2 ) && aPixPt.Y() > ( aTopLeft.Y() - 2 ) && !bBottomRulerMove && !bHeaderRulerMove && !bFooterRulerMove )
    {
        bTopRulerChange = sal_True;
        bBottomRulerChange = sal_False;
        bHeaderRulerChange = sal_False;
        bFooterRulerChange = sal_False;
    }
    else if( aPixPt.Y() < ( aBottomLeft.Y() + 2 ) && aPixPt.Y() > ( aBottomLeft.Y() - 2 ) && !bTopRulerMove && !bHeaderRulerMove && !bFooterRulerMove )
    {
        bTopRulerChange = sal_False;
        bBottomRulerChange = sal_True;
        bHeaderRulerChange = sal_False;
        bFooterRulerChange = sal_False;
    }
    else if( aPixPt.Y() < ( aHeaderLeft.Y() + 2 ) && aPixPt.Y() > ( aHeaderLeft.Y() - 2 ) && !bTopRulerMove && !bBottomRulerMove && !bFooterRulerMove )
    {
        bTopRulerChange = sal_False;
        bBottomRulerChange = sal_False;
        bHeaderRulerChange = sal_True;
        bFooterRulerChange = sal_False;
    }
    else if( aPixPt.Y() < ( aFooderLeft.Y() + 2 ) && aPixPt.Y() > ( aFooderLeft.Y() - 2 ) && !bTopRulerMove && !bBottomRulerMove && !bHeaderRulerMove )
    {
        bTopRulerChange = sal_False;
        bBottomRulerChange = sal_False;
        bHeaderRulerChange = sal_False;
        bFooterRulerChange = sal_True;
    }

    if( bPageMargin )
    {
        if(( (aPixPt.X() < ( aLeftTop.X() + 2 ) && aPixPt.X() > ( aLeftTop.X() - 2 )) || bLeftRulerMove ||
            ( aPixPt.X() < ( aRightTop.X() + 2 ) && aPixPt.X() > ( aRightTop.X() - 2 ) ) || bRightRulerMove || bOnColRulerChange || bColRulerMove )
            && aPixPt.Y() > aLeftTop.Y() && aPixPt.Y() < aLeftBottom.Y() )
        {
            if( bOnColRulerChange || bColRulerMove )
            {
                SetPointer( Pointer( POINTER_HSPLIT ) );
                if( bColRulerMove )
                {
                    if( aMouseMovePoint.X() > -aOffset.X() && aMouseMovePoint.X() < nWidth * HMM_PER_TWIPS - aOffset.X() )
                       DragMove( aMouseMovePoint.X(), POINTER_HSPLIT );
                }
            }
            else
            {
                if( bLeftRulerChange && !bTopRulerMove && !bBottomRulerMove && !bHeaderRulerMove && !bFooterRulerMove )
                {
                    SetPointer( Pointer( POINTER_HSIZEBAR ) );
                    if( bLeftRulerMove )
                    {
                       if( aMouseMovePoint.X() > -aOffset.X() && aMouseMovePoint.X() < nWidth * HMM_PER_TWIPS - aOffset.X() )
                           DragMove( aMouseMovePoint.X(), POINTER_HSIZEBAR );
                    }
                }
                else if( bRightRulerChange && !bTopRulerMove && !bBottomRulerMove && !bHeaderRulerMove && !bFooterRulerMove )
                {
                    SetPointer( Pointer( POINTER_HSIZEBAR ) );
                    if( bRightRulerMove )
                    {
                       if( aMouseMovePoint.X() > -aOffset.X() && aMouseMovePoint.X() < nWidth * HMM_PER_TWIPS - aOffset.X() )
                           DragMove( aMouseMovePoint.X(), POINTER_HSIZEBAR );
                    }
                }
            }
        }
        else
        {
            if( ( ( aPixPt.Y() < ( aTopLeft.Y() + 2 ) && aPixPt.Y() > ( aTopLeft.Y() - 2 ) ) || bTopRulerMove ||
                ( aPixPt.Y() < ( aBottomLeft.Y() + 2 ) && aPixPt.Y() > ( aBottomLeft.Y() - 2 ) ) || bBottomRulerMove ||
                ( aPixPt.Y() < ( aHeaderLeft.Y() + 2 ) && aPixPt.Y() > ( aHeaderLeft.Y() - 2 ) ) || bHeaderRulerMove ||
                ( aPixPt.Y() < ( aFooderLeft.Y() + 2 ) && aPixPt.Y() > ( aFooderLeft.Y() - 2 ) ) || bFooterRulerMove )
                && aPixPt.X() > aTopLeft.X() && aPixPt.X() < aTopRight.X() )
            {
                if( bTopRulerChange )
                {
                    SetPointer( Pointer( POINTER_VSIZEBAR ) );
                    if( bTopRulerMove )
                    {
                        if( aMouseMovePoint.Y() > -aOffset.Y() && aMouseMovePoint.Y() < nHeight * HMM_PER_TWIPS - aOffset.Y() )
                            DragMove( aMouseMovePoint.Y(), POINTER_VSIZEBAR );
                    }
                }
                else if( bBottomRulerChange )
                {
                    SetPointer( Pointer( POINTER_VSIZEBAR ) );
                    if( bBottomRulerMove )
                    {
                        if( aMouseMovePoint.Y() > -aOffset.Y() && aMouseMovePoint.Y() < nHeight * HMM_PER_TWIPS - aOffset.Y() )
                            DragMove( aMouseMovePoint.Y(), POINTER_VSIZEBAR );
                    }
                }
                else if( bHeaderRulerChange )
                {
                    SetPointer( Pointer( POINTER_VSIZEBAR ) );
                    if( bHeaderRulerMove )
                    {
                        if( aMouseMovePoint.Y() > -aOffset.Y() && aMouseMovePoint.Y() < nHeight * HMM_PER_TWIPS - aOffset.Y() )
                            DragMove( aMouseMovePoint.Y(), POINTER_VSIZEBAR );
                    }
                }
                else if( bFooterRulerChange )
                {
                    SetPointer( Pointer( POINTER_VSIZEBAR ) );
                    if( bFooterRulerMove )
                    {
                        if( aMouseMovePoint.Y() > -aOffset.Y() && aMouseMovePoint.Y() < nHeight * HMM_PER_TWIPS - aOffset.Y() )
                            DragMove( aMouseMovePoint.Y(), POINTER_VSIZEBAR );
                    }
                }
            }
            else
                SetPointer( Pointer( POINTER_ARROW ) );
        }
    }
}
//Issue51656 Add resizeable margin on page preview from maoyg
void ScPreview::InvalidateLocationData(sal_uLong nId)
{
	bLocationValid = sal_False;
    if (pViewShell->HasAccessibilityObjects())
    	pViewShell->BroadcastAccessibility( SfxSimpleHint( nId ) );
}

void ScPreview::GetFocus()
{
    if (pViewShell->HasAccessibilityObjects())
        pViewShell->BroadcastAccessibility( ScAccWinFocusGotHint(GetAccessible()) );
}

void ScPreview::LoseFocus()
{
    if (pViewShell->HasAccessibilityObjects())
        pViewShell->BroadcastAccessibility( ScAccWinFocusLostHint(GetAccessible()) );
}

com::sun::star::uno::Reference<com::sun::star::accessibility::XAccessible> ScPreview::CreateAccessible()
{
	com::sun::star::uno::Reference<com::sun::star::accessibility::XAccessible> xAcc= GetAccessible(sal_False);
	if (xAcc.is())
	{
		return xAcc;
	}
	ScAccessibleDocumentPagePreview* pAccessible =
		new ScAccessibleDocumentPagePreview( GetAccessibleParentWindow()->GetAccessible(), pViewShell );
	xAcc = pAccessible;
	SetAccessible(xAcc);
	pAccessible->Init();
	return xAcc;
}
// MT: Removed Windows::SwitchView() introduced with IA2 CWS.
// There are other notifications for this when the active view has chnaged, so please update the code to use that event mechanism
void ScPreview::SwitchView()
{
	if (!Application::IsAccessibilityEnabled())
	{
		return ;
	}
	ScAccessibleDocumentBase* pAccDoc = static_cast<ScAccessibleDocumentBase*>(GetAccessible(sal_False).get());
	if (pAccDoc)
	{
		pAccDoc->SwitchViewFireFocus();
	}		
}
//Issue51656 Add resizeable margin on page preview from maoyg
void ScPreview::DragMove( long nDragMovePos, sal_uInt16 nFlags )
{
    Fraction aPreviewZoom( nZoom, 100 );
    Fraction aHorPrevZoom( (long)( 100 * nZoom / pDocShell->GetOutputFactor() ), 10000 );
    MapMode  aMMMode( MAP_100TH_MM, Point(), aHorPrevZoom, aPreviewZoom );
    SetMapMode( aMMMode );
    long  nPos = nDragMovePos;
    if( nFlags == POINTER_HSIZEBAR || nFlags == POINTER_HSPLIT )
    {
        if( nDragMovePos != aButtonDownChangePoint.X() )
        {
            DrawInvert( aButtonDownChangePoint.X(), nFlags );
            aButtonDownChangePoint.X() = nPos;
            DrawInvert( aButtonDownChangePoint.X(), nFlags );
        }
    }
    else if( nFlags == POINTER_VSIZEBAR )
    {
        if( nDragMovePos != aButtonDownChangePoint.Y() )
        {
            DrawInvert( aButtonDownChangePoint.Y(), nFlags );
            aButtonDownChangePoint.Y() = nPos;
            DrawInvert( aButtonDownChangePoint.Y(), nFlags );
        }
    }
}

void ScPreview::DrawInvert( long nDragPos, sal_uInt16 nFlags )
{
    long  nHeight = (long) lcl_GetDocPageSize( pDocShell->GetDocument(), nTab ).Height();
    long  nWidth = (long) lcl_GetDocPageSize( pDocShell->GetDocument(), nTab ).Width();
    if( nFlags == POINTER_HSIZEBAR || nFlags == POINTER_HSPLIT )
    {
        Rectangle aRect( nDragPos, -aOffset.Y(), nDragPos + 1,(long)( ( nHeight * HMM_PER_TWIPS ) - aOffset.Y()));
        Invert( aRect,INVERT_50 );
    }
    else if( nFlags == POINTER_VSIZEBAR )
    {
        Rectangle aRect( -aOffset.X(), nDragPos,(long)( ( nWidth * HMM_PER_TWIPS ) - aOffset.X() ), nDragPos + 1 );
        Invert( aRect,INVERT_50 );
    }
}
//Issue51656 Add resizeable margin on page preview from maoyg
