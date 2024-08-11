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
#include "precompiled_sw.hxx"

#include <sfx2/objface.hxx>
#include <vcl/timer.hxx>
#include <vcl/field.hxx>
#include <vcl/fixed.hxx>
#include <vcl/help.hxx>
#include <vcl/cmdevt.hxx>
#include <vcl/button.hxx>
#include <svl/whiter.hxx>
#include <svl/stritem.hxx>
#include <svl/eitem.hxx>
#include <sfx2/printer.hxx>
#include <sfx2/progress.hxx>
#include <sfx2/app.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/request.hxx>
#include <sfx2/dispatch.hxx>
#include <vcl/msgbox.hxx>
#include <svx/stddlg.hxx>
#include <editeng/paperinf.hxx>
#include <svl/srchitem.hxx>
#include <svx/svdview.hxx>
#include <svx/dlgutil.hxx>
#include <svx/zoomslideritem.hxx>
#include <svx/svxids.hrc>

#include <swwait.hxx>
#include <globdoc.hxx>
#include <wdocsh.hxx>
#include <pvprtdat.hxx>
#include <swmodule.hxx>
#include <modcfg.hxx>
#include <wrtsh.hxx>
#include <docsh.hxx>
#include <viewopt.hxx>
#include <doc.hxx>
#include <pview.hxx>
#include <view.hxx>
#include <textsh.hxx>
#include <scroll.hxx>
#include <prtopt.hxx>
#include <docstat.hxx>
#include <usrpref.hxx>
#include <viewfunc.hxx>

#include <helpid.h>
#include <cmdid.h>
#include <globals.hrc>
#include <popup.hrc>
#include <pview.hrc>

#define SwPagePreView
#include <sfx2/msg.hxx>
#include <swslots.hxx>
// OD 12.12.2002 #103492#
#include <pagepreviewlayout.hxx>

#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>
#include <vos/mutex.hxx>


using namespace ::com::sun::star;

SFX_IMPL_NAMED_VIEWFACTORY(SwPagePreView, "PrintPreview")
{
	SFX_VIEW_REGISTRATION(SwDocShell);
	SFX_VIEW_REGISTRATION(SwWebDocShell);
	SFX_VIEW_REGISTRATION(SwGlobalDocShell);
}

SFX_IMPL_INTERFACE(SwPagePreView, SfxViewShell, SW_RES(RID_PVIEW_TOOLBOX))
{
	SFX_POPUPMENU_REGISTRATION(SW_RES(MN_PPREVIEW_POPUPMENU));
	SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_OBJECT|SFX_VISIBILITY_STANDARD|
								SFX_VISIBILITY_CLIENT|SFX_VISIBILITY_FULLSCREEN|
								SFX_VISIBILITY_READONLYDOC,
								SW_RES(RID_PVIEW_TOOLBOX));
}


TYPEINIT1(SwPagePreView,SfxViewShell)

#define SWVIEWFLAGS ( SFX_VIEW_CAN_PRINT|SFX_VIEW_HAS_PRINTOPTIONS )

#define MIN_PREVIEW_ZOOM 25
#define MAX_PREVIEW_ZOOM 600
/*  */
/* -----------------26.11.2002 10:41-----------------
 *
 * --------------------------------------------------*/
sal_uInt16 lcl_GetNextZoomStep(sal_uInt16 nCurrentZoom, sal_Bool bZoomIn)
{
    static sal_uInt16 aZoomArr[] =
    {
        25, 50, 75, 100, 150, 200, 400, 600
    };
    const sal_uInt16 nZoomArrSize = sizeof(aZoomArr)/sizeof(sal_uInt16);
    if(bZoomIn)
        for(int i = nZoomArrSize - 1; i >= 0; --i)
        {
            if(nCurrentZoom > aZoomArr[i] || !i)
                return aZoomArr[i];
        }
    else
        for(int i = 0; i < nZoomArrSize; ++i)
        {
            if(nCurrentZoom < aZoomArr[i])
                return aZoomArr[i];
        }
    return bZoomIn ? MAX_PREVIEW_ZOOM : MIN_PREVIEW_ZOOM;
};
/* -----------------02.12.2002 09:11-----------------
 *
 * --------------------------------------------------*/
void lcl_InvalidateZoomSlots(SfxBindings& rBindings)
{
    static sal_uInt16 __READONLY_DATA aInval[] =
    {
        SID_ATTR_ZOOM, SID_ZOOM_OUT, SID_ZOOM_IN, SID_ATTR_ZOOMSLIDER, FN_PREVIEW_ZOOM, FN_STAT_ZOOM,
        0
    };
    rBindings.Invalidate( aInval );
}
/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

// erstmal der Zoom-Dialog

class SwPreViewZoomDlg : public SvxStandardDialog
{
	FixedText		aRowLbl;
	NumericField 	aRowEdit;
	FixedText		aColLbl;
	NumericField 	aColEdit;

	OKButton	 	aOkBtn;
	CancelButton 	aCancelBtn;
	HelpButton 		aHelpBtn;

	virtual void 	Apply();

public:
	SwPreViewZoomDlg( SwPagePreViewWin& rParent );
	~SwPreViewZoomDlg();
};

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


SwPreViewZoomDlg::SwPreViewZoomDlg( SwPagePreViewWin& rParent ) :
	SvxStandardDialog( &rParent, SW_RES(DLG_PAGEPREVIEW_ZOOM) ),
	aRowLbl(this,SW_RES(FT_ROW)),
	aRowEdit(this,SW_RES(ED_ROW)),
	aColLbl(this,SW_RES(FT_COL)),
	aColEdit(this,SW_RES(ED_COL)),
	aOkBtn(this,SW_RES(BT_OK)),
	aCancelBtn(this,SW_RES(BT_CANCEL)),
	aHelpBtn(this,SW_RES(BT_HELP))
{
	FreeResource();

	aRowEdit.SetValue( rParent.GetRow() );
	aColEdit.SetValue( rParent.GetCol() );
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

SwPreViewZoomDlg::~SwPreViewZoomDlg() {}


void  SwPreViewZoomDlg::Apply()
{
	((SwPagePreViewWin*)GetParent())->CalcWish(
				sal_uInt8(aRowEdit.GetValue()),
				sal_uInt8(aColEdit.GetValue()) );
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

// alles fuers SwPagePreViewWin


SwPagePreViewWin::SwPagePreViewWin( Window *pParent, SwPagePreView& rPView )
	: Window( pParent, WinBits( WB_CLIPCHILDREN) ),
    mpViewShell( 0 ),
    mrView( rPView ),
    mbCalcScaleForPreviewLayout( true ),
    maPaintedPreviewDocRect( Rectangle(0,0,0,0) )
{
    SetOutDevViewType( OUTDEV_VIEWTYPE_PRINTPREVIEW ); //#106611#
	SetHelpId(HID_PAGEPREVIEW);
	SetFillColor( GetBackground().GetColor() );
	SetLineColor( GetBackground().GetColor());
	SetMapMode( MapMode(MAP_TWIP) );

	const SwMasterUsrPref *pUsrPref = SW_MOD()->GetUsrPref(sal_False);
    mnRow = pUsrPref->GetPagePrevRow();     // 1 Zeile
    mnCol = pUsrPref->GetPagePrevCol();     // 1 Spalte
    // OD 24.03.2003 #108282# - member <mnVirtPage> no longer exists.
    mnSttPage = USHRT_MAX;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


SwPagePreViewWin::~SwPagePreViewWin()
{
	// Remove to the deconstruction of SwPagePreView.
	//if( mpViewShell)
	//	delete mpViewShell;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreViewWin::Paint( const Rectangle& rRect )
{
    if( !mpViewShell || !mpViewShell->GetLayout() )
		return;

    if( USHRT_MAX == mnSttPage )        // wurde noch nie berechnet ? (Init-Phase!)
	{
		// das ist die Size, auf die ich mich immer beziehe
        if( !maPxWinSize.Height() || !maPxWinSize.Width() )
            maPxWinSize = GetOutputSizePixel();

		Rectangle aRect( LogicToPixel( rRect ));
        mpPgPrevwLayout->Prepare( 1, Point(0,0), maPxWinSize,
                                  mnSttPage, maPaintedPreviewDocRect );
        SetSelectedPage( 1 );
        mpPgPrevwLayout->Paint( PixelToLogic( aRect ) );
        SetPagePreview(mnRow, mnCol);
	}
	else
	{
        MapMode aMM( GetMapMode() );
        aMM.SetScaleX( maScale );
        aMM.SetScaleY( maScale );
		SetMapMode( aMM );
        mpPgPrevwLayout->Paint( rRect );
	}
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/
void SwPagePreViewWin::CalcWish( sal_uInt8 nNewRow, sal_uInt8 nNewCol )
{
    if( !mpViewShell || !mpViewShell->GetLayout() )
		return;

    sal_uInt16 nOldCol = mnCol;
    // OD 02.12.2002 #103492# - update <mnRow> and <mnCol>.
    mnRow = nNewRow;
    mnCol = nNewCol;
    sal_uInt16 nPages = mnRow * mnCol,
           nLastSttPg = mrView.GetPageCount()+1 > nPages
                            ? mrView.GetPageCount()+1 - nPages : 0;
    if( mnSttPage > nLastSttPg )
        mnSttPage = nLastSttPg;

    mpPgPrevwLayout->Init( mnCol, mnRow, maPxWinSize, true );
    mpPgPrevwLayout->Prepare( mnSttPage, Point(0,0), maPxWinSize,
                              mnSttPage, maPaintedPreviewDocRect );
    SetSelectedPage( mnSttPage );
    SetPagePreview(mnRow, mnCol);
    maScale = GetMapMode().GetScaleX();

    // falls an der Spaltigkeit gedreht wurde, so muss der Sonderfall
    // Einspaltig beachtet und ggfs. der Scrollbar korrigiert werden
    if( (1 == nOldCol) ^ (1 == mnCol) )
        mrView.ScrollDocSzChg();

    // Sortierung muss eingehalten werden!!
    // OD 24.03.2003 #108282# - additional invalidate page status.
    static sal_uInt16 __READONLY_DATA aInval[] =
    {
        SID_ATTR_ZOOM, SID_ZOOM_OUT, SID_ZOOM_IN,
        FN_PREVIEW_ZOOM,
        FN_START_OF_DOCUMENT, FN_END_OF_DOCUMENT, FN_PAGEUP, FN_PAGEDOWN,
        FN_STAT_PAGE, FN_STAT_ZOOM,
        FN_SHOW_TWO_PAGES, FN_SHOW_MULTIPLE_PAGES,
        0
    };
    SfxBindings& rBindings = mrView.GetViewFrame()->GetBindings();
    rBindings.Invalidate( aInval );
    rBindings.Update( FN_SHOW_TWO_PAGES );
    rBindings.Update( FN_SHOW_MULTIPLE_PAGES );
    // OD 18.12.2002 #103492# - adjust scrollbars
    mrView.ScrollViewSzChg();
}
/*--------------------------------------------------------------------
	Beschreibung:, mnSttPage is Absolute
 --------------------------------------------------------------------*/


int SwPagePreViewWin::MovePage( int eMoveMode )
{
	// soviele Seiten hoch
    sal_uInt16 nPages = mnRow * mnCol;
    sal_uInt16 nNewSttPage = mnSttPage;
    // OD 04.12.2002 #103492#
    sal_uInt16 nPageCount = mrView.GetPageCount();
	sal_uInt16 nDefSttPg = GetDefSttPage();
    // OD 06.12.2002 #103492#
    bool bPaintPageAtFirstCol = true;

	switch( eMoveMode )
	{
    case MV_PAGE_UP:
    {
        const sal_uInt16 nRelSttPage = mpPgPrevwLayout->ConvertAbsoluteToRelativePageNum( mnSttPage );
        const sal_uInt16 nNewAbsSttPage = nRelSttPage - nPages > 0 ?
                                          mpPgPrevwLayout->ConvertRelativeToAbsolutePageNum( nRelSttPage - nPages ) :
                                          nDefSttPg;
        nNewSttPage = nNewAbsSttPage;

        const sal_uInt16 nRelSelPage = mpPgPrevwLayout->ConvertAbsoluteToRelativePageNum( SelectedPage() );
        const sal_uInt16 nNewRelSelPage = nRelSelPage - nPages > 0 ?
                                          nRelSelPage - nPages :
                                          1;
        SetSelectedPage( mpPgPrevwLayout->ConvertRelativeToAbsolutePageNum( nNewRelSelPage ) );

        break;
    }
    case MV_PAGE_DOWN:
    {
        const sal_uInt16 nRelSttPage = mpPgPrevwLayout->ConvertAbsoluteToRelativePageNum( mnSttPage );
        const sal_uInt16 nNewAbsSttPage = mpPgPrevwLayout->ConvertRelativeToAbsolutePageNum( nRelSttPage + nPages );
        nNewSttPage = nNewAbsSttPage < nPageCount ? nNewAbsSttPage : nPageCount;

        const sal_uInt16 nRelSelPage = mpPgPrevwLayout->ConvertAbsoluteToRelativePageNum( SelectedPage() );
        const sal_uInt16 nNewAbsSelPage = mpPgPrevwLayout->ConvertRelativeToAbsolutePageNum( nRelSelPage + nPages );
        SetSelectedPage( nNewAbsSelPage < nPageCount ? nNewAbsSelPage : nPageCount );

        break;
    }
    case MV_DOC_STT:
        nNewSttPage = nDefSttPg;
        SetSelectedPage( mpPgPrevwLayout->ConvertRelativeToAbsolutePageNum( nNewSttPage ? nNewSttPage : 1 ) );
        break;
    case MV_DOC_END:
        // OD 03.12.2002 #103492# - correct calculation of new start page.
        nNewSttPage = nPageCount;
        SetSelectedPage( nPageCount );
        break;
    // OD 12.12.2002 #103492# - add new move mode
    case MV_SELPAGE:
        // <nNewSttPage> and <SelectedPage()> are already set.
        // OD 20.02.2003 #107369# - not start at first column, only if the
        // complete preview layout columns doesn't fit into window.
        if ( !mpPgPrevwLayout->DoesPreviewLayoutColsFitIntoWindow() )
            bPaintPageAtFirstCol = false;
        break;
    case MV_SCROLL:
        // OD 17.01.2003 #103492# - check, if paint page at first column
        // has to be avoided
        if ( !mpPgPrevwLayout->DoesPreviewLayoutRowsFitIntoWindow() ||
             !mpPgPrevwLayout->DoesPreviewLayoutColsFitIntoWindow() )
            bPaintPageAtFirstCol = false;
        break;
    case MV_NEWWINSIZE:
        // OD 18.12.2002 #103492# - nothing special to do.
        break;
    case MV_CALC:
        // OD 18.12.2002 #103492# - re-init page preview layout.
        mpPgPrevwLayout->ReInit();

        // OD 03.12.2002 #103492# - correct calculation of new start page.
        if( nNewSttPage > nPageCount )
            nNewSttPage = nPageCount;

        // OD 18.12.2002 #103492# - correct selected page number
        if( SelectedPage() > nPageCount )
            SetSelectedPage( nNewSttPage ? nNewSttPage : 1 );
	}

    mpPgPrevwLayout->Prepare( nNewSttPage, Point(0,0), maPxWinSize,
                              nNewSttPage,
                              maPaintedPreviewDocRect, bPaintPageAtFirstCol );
    if( nNewSttPage == mnSttPage &&
        eMoveMode != MV_SELPAGE )
        return sal_False;

    SetPagePreview(mnRow, mnCol);
    mnSttPage = nNewSttPage;

    // OD 24.03.2003 #108282# - additional invalidate page status.
	static sal_uInt16 __READONLY_DATA aInval[] =
	{
        FN_START_OF_DOCUMENT, FN_END_OF_DOCUMENT, FN_PAGEUP, FN_PAGEDOWN,
        FN_STAT_PAGE, 0
	};

    SfxBindings& rBindings = mrView.GetViewFrame()->GetBindings();
	rBindings.Invalidate( aInval );

	return sal_True;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreViewWin::SetWinSize( const Size& rNewSize )
{
	// die Size wollen wir aber immer in Pixel-Einheiten haben
    maPxWinSize = LogicToPixel( rNewSize );

    if( USHRT_MAX == mnSttPage )
    {
        mnSttPage = GetDefSttPage();
        SetSelectedPage( GetDefSttPage() );
    }

    if ( mbCalcScaleForPreviewLayout )
    {
        mpPgPrevwLayout->Init( mnCol, mnRow, maPxWinSize, true );
        maScale = GetMapMode().GetScaleX();
    }
    mpPgPrevwLayout->Prepare( mnSttPage, Point(0,0), maPxWinSize,
                              mnSttPage, maPaintedPreviewDocRect );
    if ( mbCalcScaleForPreviewLayout )
    {
        SetSelectedPage( mnSttPage );
        mbCalcScaleForPreviewLayout = false;
    }
    SetPagePreview(mnRow, mnCol);
    maScale = GetMapMode().GetScaleX();
}


/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreViewWin::GetStatusStr( String& rStr, sal_uInt16 nPageCnt ) const
{
    // OD 24.03.2003 #108282# - show physical and virtual page number of
    // selected page, if it's visible.
    sal_uInt16 nPageNum;
    if ( mpPgPrevwLayout->IsPageVisible( mpPgPrevwLayout->SelectedPage() ) )
    {
        nPageNum = mpPgPrevwLayout->SelectedPage();
    }
    else
    {
        nPageNum = mnSttPage > 1 ? mnSttPage : 1;
    }
    sal_uInt16 nVirtPageNum = mpPgPrevwLayout->GetVirtPageNumByPageNum( nPageNum );
    if( nVirtPageNum && nVirtPageNum != nPageNum )
	{
        rStr += String::CreateFromInt32( nVirtPageNum );
		rStr += ' ';
	}
    rStr += String::CreateFromInt32( nPageNum );
	rStr.AppendAscii( RTL_CONSTASCII_STRINGPARAM(" / "));
	rStr += String::CreateFromInt32( nPageCnt );
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreViewWin::KeyInput( const KeyEvent &rKEvt )
{
    const KeyCode& rKeyCode = rKEvt.GetKeyCode();
    sal_uInt16 nKey = rKeyCode.GetCode();
    sal_Bool bHandled = sal_False;
    if(!rKeyCode.GetModifier())
    {
        sal_uInt16 nSlot = 0;
        switch(nKey)
        {
            case KEY_ADD : nSlot = SID_ZOOM_OUT;         break;
            case KEY_ESCAPE: nSlot = FN_CLOSE_PAGEPREVIEW; break;
            case KEY_SUBTRACT : nSlot = SID_ZOOM_IN;    break;
        }
        if(nSlot)
        {
            bHandled = sal_True;
            mrView.GetViewFrame()->GetDispatcher()->Execute(
                                nSlot, SFX_CALLMODE_ASYNCHRON );
        }
    }
    if( !bHandled && !mrView.KeyInput( rKEvt ) )
		Window::KeyInput( rKEvt );
}

/******************************************************************************
 *	Beschreibung:
 ******************************************************************************/

void SwPagePreViewWin::Command( const CommandEvent& rCEvt )
{
	sal_Bool bCallBase = sal_True;
	switch( rCEvt.GetCommand() )
	{
        case COMMAND_CONTEXTMENU:
            mrView.GetViewFrame()->GetDispatcher()->ExecutePopup();
            bCallBase = sal_False;
        break;

        case COMMAND_WHEEL:
        case COMMAND_STARTAUTOSCROLL:
        case COMMAND_AUTOSCROLL:
        {
            const CommandWheelData* pData = rCEvt.GetWheelData();
            if( pData )
            {
                const CommandWheelData aDataNew(pData->GetDelta(),pData->GetNotchDelta(),COMMAND_WHEEL_PAGESCROLL,
                    pData->GetMode(),pData->GetModifier(),pData->IsHorz(), pData->IsDeltaPixel());
                const CommandEvent aEvent( rCEvt.GetMousePosPixel(),rCEvt.GetCommand(),rCEvt.IsMouseEvent(),&aDataNew);
                    bCallBase = !mrView.HandleWheelCommands( aEvent );
            }
            else
                bCallBase = !mrView.HandleWheelCommands( rCEvt );
       }
       break;
       default:
           // OD 17.12.2002 #103492# - delete assertion
           ;
	}

	if( bCallBase )
		Window::Command( rCEvt );
}

void SwPagePreViewWin::MouseButtonDown( const MouseEvent& rMEvt )
{
    // OD 17.12.2002 #103492# - consider single-click to set selected page
    if( MOUSE_LEFT == ( rMEvt.GetModifier() + rMEvt.GetButtons() ) )
	{
        Point aPrevwPos( PixelToLogic( rMEvt.GetPosPixel() ) );
        Point aDocPos;
        bool bPosInEmptyPage;
        sal_uInt16 nNewSelectedPage;
        bool bIsDocPos =
            mpPgPrevwLayout->IsPrevwPosInDocPrevwPage( aPrevwPos,
                                    aDocPos, bPosInEmptyPage, nNewSelectedPage );
        if ( bIsDocPos && rMEvt.GetClicks() == 2 )
        {
            // close page preview, set new cursor position and switch to
            // normal view.
            String sNewCrsrPos( String::CreateFromInt32( aDocPos.X() ));
            ((( sNewCrsrPos += ';' )
                            += String::CreateFromInt32( aDocPos.Y() )) )
                            += ';';
            mrView.SetNewCrsrPos( sNewCrsrPos );

            SfxViewFrame *pTmpFrm = mrView.GetViewFrame();
            pTmpFrm->GetBindings().Execute( SID_VIEWSHELL0, NULL, 0,
                                                    SFX_CALLMODE_ASYNCHRON );
        }
        else if ( bIsDocPos || bPosInEmptyPage )
        // OD 2004-03-04 #i20684# - add missing parenthesis
        {
            // show clicked page as the selected one
            mpPgPrevwLayout->MarkNewSelectedPage( nNewSelectedPage );
            GetViewShell()->ShowPreViewSelection( nNewSelectedPage );
            // OD 19.02.2003 #107369# - adjust position at vertical scrollbar.
            if ( mpPgPrevwLayout->DoesPreviewLayoutRowsFitIntoWindow() )
            {
                mrView.SetVScrollbarThumbPos( nNewSelectedPage );
            }
            // OD 24.03.2003 #108282# - invalidate page status.
            static sal_uInt16 __READONLY_DATA aInval[] =
            {
                FN_STAT_PAGE, 0
            };
            SfxBindings& rBindings = mrView.GetViewFrame()->GetBindings();
            rBindings.Invalidate( aInval );
        }
	}
}

/******************************************************************************
 *	Beschreibung: Userprefs bzw Viewoptions setzen
 ******************************************************************************/


void SwPagePreViewWin::SetPagePreview( sal_uInt8 nRow, sal_uInt8 nCol )
{
	SwMasterUsrPref *pOpt = (SwMasterUsrPref *)SW_MOD()->GetUsrPref(sal_False);

	if (nRow != pOpt->GetPagePrevRow() || nCol != pOpt->GetPagePrevCol())
	{
		pOpt->SetPagePrevRow( nRow );
		pOpt->SetPagePrevCol( nCol );
		pOpt->SetModified();

        //Scrollbar updaten!
        mrView.ScrollViewSzChg();
	}
}

/** get selected page in document preview

    OD 13.12.2002 #103492#

    @author OD
*/
sal_uInt16 SwPagePreViewWin::SelectedPage() const
{
    return mpPgPrevwLayout->SelectedPage();
}

/** set selected page number in document preview

    OD 13.12.2002 #103492#

    @author OD
*/
void SwPagePreViewWin::SetSelectedPage( sal_uInt16 _nSelectedPageNum )
{
    mpPgPrevwLayout->SetSelectedPage( _nSelectedPageNum );
}

/** method to enable/disable book preview

    OD 2004-03-05 #i18143#

    @author OD
*/
bool SwPagePreViewWin::SetBookPreviewMode( const bool _bBookPreview )
{
    return mpPgPrevwLayout->SetBookPreviewMode( _bBookPreview,
                                                mnSttPage,
                                                maPaintedPreviewDocRect );
}

void SwPagePreViewWin::DataChanged( const DataChangedEvent& rDCEvt )
{
	Window::DataChanged( rDCEvt );

	switch( rDCEvt.GetType() )
	{
	case DATACHANGED_SETTINGS:
		// ScrollBars neu anordnen bzw. Resize ausloesen, da sich
		// ScrollBar-Groesse geaendert haben kann. Dazu muss dann im
		// Resize-Handler aber auch die Groesse der ScrollBars aus
		// den Settings abgefragt werden.
		if( rDCEvt.GetFlags() & SETTINGS_STYLE )
            mrView.InvalidateBorder();              //Scrollbarbreiten
        //#106746# zoom has to be disabled if Accessibility support is switched on
        lcl_InvalidateZoomSlots(mrView.GetViewFrame()->GetBindings());
        break;

	case DATACHANGED_PRINTER:
	case DATACHANGED_DISPLAY:
	case DATACHANGED_FONTS:
	case DATACHANGED_FONTSUBSTITUTION:
        mrView.GetDocShell()->UpdateFontList(); //Fontwechsel
        if ( mpViewShell->GetWin() )
            mpViewShell->GetWin()->Invalidate();
		break;
	}
}

/** help method to execute SfxRequest FN_PAGEUP and FN_PAGEDOWN

    OD 04.03.2003 #107369#

    @author OD
*/
void SwPagePreView::_ExecPgUpAndPgDown( const bool  _bPgUp,
                                        SfxRequest* _pReq )
{
    SwPagePreviewLayout* pPagePrevwLay = GetViewShell()->PagePreviewLayout();
    // check, if top/bottom of preview is *not* already visible.
    if( pPagePrevwLay->GetWinPagesScrollAmount( _bPgUp ? -1 : 1 ) != 0 )
    {
        if ( pPagePrevwLay->DoesPreviewLayoutRowsFitIntoWindow() &&
             pPagePrevwLay->DoesPreviewLayoutColsFitIntoWindow() )
        {
            const int eMvMode = _bPgUp ?
                                SwPagePreViewWin::MV_PAGE_UP :
                                SwPagePreViewWin::MV_PAGE_DOWN;
            if ( ChgPage( eMvMode, sal_True ) )
                pViewWin->Invalidate();
        }
        else
        {
            SwTwips nScrollAmount;
            sal_uInt16 nNewSelectedPageNum = 0;
            const sal_uInt16 nVisPages = pViewWin->GetRow() * pViewWin->GetCol();
            if( _bPgUp )
            {
                if ( pPagePrevwLay->DoesPreviewLayoutRowsFitIntoWindow() )
                {
                    nScrollAmount = pPagePrevwLay->GetWinPagesScrollAmount( -1 );
                    if ( (pViewWin->SelectedPage() - nVisPages) > 0 )
                        nNewSelectedPageNum = pViewWin->SelectedPage() - nVisPages;
                    else
                        nNewSelectedPageNum = 1;
                }
                else
                    nScrollAmount = - Min( pViewWin->GetOutputSize().Height(),
                                           pViewWin->GetPaintedPreviewDocRect().Top() );
            }
            else
            {
                if ( pPagePrevwLay->DoesPreviewLayoutRowsFitIntoWindow() )
                {
                    nScrollAmount = pPagePrevwLay->GetWinPagesScrollAmount( 1 );
                    if ( (pViewWin->SelectedPage() + nVisPages) <= mnPageCount )
                        nNewSelectedPageNum = pViewWin->SelectedPage() + nVisPages;
                    else
                        nNewSelectedPageNum = mnPageCount;
                }
                else
                    nScrollAmount = Min( pViewWin->GetOutputSize().Height(),
                                         ( pPagePrevwLay->GetPrevwDocSize().Height() -
                                           pViewWin->GetPaintedPreviewDocRect().Bottom() ) );
            }
            pViewWin->Scroll( 0, nScrollAmount );
            if ( nNewSelectedPageNum != 0 )
            {
                pViewWin->SetSelectedPage( nNewSelectedPageNum );
            }
            ScrollViewSzChg();
			// OD 24.03.2003 #108282# - additional invalidate page status.
            static sal_uInt16 __READONLY_DATA aInval[] =
            {
                FN_START_OF_DOCUMENT, FN_END_OF_DOCUMENT, FN_PAGEUP, FN_PAGEDOWN,
				FN_STAT_PAGE, 0
            };
            SfxBindings& rBindings = GetViewFrame()->GetBindings();
            rBindings.Invalidate( aInval );
            pViewWin->Invalidate();
        }
    }

    if ( _pReq )
        _pReq->Done();
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/
// dann mal alles fuer die SwPagePreView
void  SwPagePreView::Execute( SfxRequest &rReq )
{
    int eMvMode;
	sal_uInt8 nRow = 1;
    sal_Bool bRetVal = sal_False;
    bool bRefresh = true;

	switch(rReq.GetSlot())
	{
		case FN_REFRESH_VIEW:
		case FN_STAT_PAGE:
		case FN_STAT_ZOOM:
			break;

        case FN_SHOW_MULTIPLE_PAGES:
        {
            const SfxItemSet *pArgs = rReq.GetArgs();
            if( pArgs && pArgs->Count() >= 2 )
            {
                sal_uInt8 nCols = (sal_uInt8)((SfxUInt16Item &)pArgs->Get(
                                        SID_ATTR_TABLE_COLUMN)).GetValue();
                sal_uInt8 nRows = (sal_uInt8)((SfxUInt16Item &)pArgs->Get(
                                        SID_ATTR_TABLE_ROW)).GetValue();
                pViewWin->CalcWish( nRows, nCols );

            }
            else
                SwPreViewZoomDlg( *pViewWin ).Execute();

        }
		break;
		case FN_SHOW_BOOKVIEW:
		{
            const SfxItemSet* pArgs = rReq.GetArgs();
            const SfxPoolItem* pItem;
            bool bBookPreview = GetViewShell()->GetViewOptions()->IsPagePrevBookview();
            if( pArgs && SFX_ITEM_SET == pArgs->GetItemState( FN_SHOW_BOOKVIEW, sal_False, &pItem ) )
            {
                bBookPreview = static_cast< const SfxBoolItem* >( pItem )->GetValue();
                ( ( SwViewOption* ) GetViewShell()->GetViewOptions() )->SetPagePrevBookview( bBookPreview );
					// cast is not gentleman like, but it's common use in writer and in this case
            }
            if ( pViewWin->SetBookPreviewMode( bBookPreview ) )
            {
                // book preview mode changed. Thus, adjust scrollbars and
                // invalidate corresponding states.
                ScrollViewSzChg();
                static sal_uInt16 __READONLY_DATA aInval[] =
                {
                    FN_START_OF_DOCUMENT, FN_END_OF_DOCUMENT, FN_PAGEUP, FN_PAGEDOWN,
                    FN_STAT_PAGE, FN_SHOW_BOOKVIEW, 0
                };
                SfxBindings& rBindings = GetViewFrame()->GetBindings();
                rBindings.Invalidate( aInval );
                pViewWin->Invalidate();
            }

        }
		break;
        case FN_SHOW_TWO_PAGES:
			pViewWin->CalcWish( nRow, 2 );
			break;

		case FN_PREVIEW_ZOOM:
		case SID_ATTR_ZOOM:
		{
			const SfxItemSet *pArgs = rReq.GetArgs();
            const SfxPoolItem* pItem;
			AbstractSvxZoomDialog *pDlg = 0;
            if(!pArgs)
            {
                SfxItemSet aCoreSet(GetPool(), SID_ATTR_ZOOM, SID_ATTR_ZOOM);
                const SwViewOption* pVOpt = GetViewShell()->GetViewOptions();
                SvxZoomItem aZoom( (SvxZoomType)pVOpt->GetZoomType(),
                                            pVOpt->GetZoom() );
                aZoom.SetValueSet(
                        SVX_ZOOM_ENABLE_50|
                        SVX_ZOOM_ENABLE_75|
                        SVX_ZOOM_ENABLE_100|
                        SVX_ZOOM_ENABLE_150|
                        SVX_ZOOM_ENABLE_200|
                        SVX_ZOOM_ENABLE_WHOLEPAGE);
                aCoreSet.Put( aZoom );

				SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
				if(pFact)
				{
                    pDlg = pFact->CreateSvxZoomDialog(&GetViewFrame()->GetWindow(), aCoreSet);
                    DBG_ASSERT(pDlg, "Dialogdiet fail!");
				}

                pDlg->SetLimits( MINZOOM, MAXZOOM );

                if( pDlg->Execute() != RET_CANCEL )
                    pArgs = pDlg->GetOutputItemSet();
            }
            if( pArgs )
            {
                enum SvxZoomType eType = SVX_ZOOM_PERCENT;
                sal_uInt16 nZoomFactor = USHRT_MAX;
                if(SFX_ITEM_SET == pArgs->GetItemState(SID_ATTR_ZOOM, sal_True, &pItem))
                {
                    eType = ((const SvxZoomItem *)pItem)->GetType();
                    nZoomFactor = ((const SvxZoomItem *)pItem)->GetValue();
                }
                else if(SFX_ITEM_SET == pArgs->GetItemState(FN_PREVIEW_ZOOM, sal_True, &pItem))
                    nZoomFactor = ((const SfxUInt16Item *)pItem)->GetValue();
                if(USHRT_MAX != nZoomFactor)
                    SetZoom(eType, nZoomFactor);
            }
			delete pDlg;
        }
		break;
        case SID_ATTR_ZOOMSLIDER :
        {
            const SfxItemSet *pArgs = rReq.GetArgs();
            const SfxPoolItem* pItem;

            if ( pArgs && SFX_ITEM_SET == pArgs->GetItemState(SID_ATTR_ZOOMSLIDER, sal_True, &pItem ) )
            {
                const sal_uInt16 nCurrentZoom = ((const SvxZoomSliderItem *)pItem)->GetValue();
                SetZoom( SVX_ZOOM_PERCENT, nCurrentZoom );
            }
        }
        break;
        case SID_ZOOM_IN:
        case SID_ZOOM_OUT:
        {
            enum SvxZoomType eType = SVX_ZOOM_PERCENT;
            const SwViewOption* pVOpt = GetViewShell()->GetViewOptions();
            SetZoom(eType,
                    lcl_GetNextZoomStep(pVOpt->GetZoom(), SID_ZOOM_IN == rReq.GetSlot()));
        }
        break;
        case FN_CHAR_LEFT:
        case FN_CHAR_RIGHT:
        case FN_LINE_UP:
        case FN_LINE_DOWN:
        {
            SwPagePreviewLayout* pPagePrevwLay = GetViewShell()->PagePreviewLayout();
            sal_uInt16 nNewSelectedPage;
            sal_uInt16 nNewStartPage;
            Point aNewStartPos;
            sal_Int16 nHoriMove = 0;
            sal_Int16 nVertMove = 0;
            switch(rReq.GetSlot())
            {
                case FN_CHAR_LEFT:  nHoriMove = -1; break;
                case FN_CHAR_RIGHT: nHoriMove = 1;  break;
                case FN_LINE_UP:    nVertMove = -1; break;
                case FN_LINE_DOWN:  nVertMove = 1;  break;
            }
            pPagePrevwLay->CalcStartValuesForSelectedPageMove( nHoriMove, nVertMove,
                                nNewSelectedPage, nNewStartPage, aNewStartPos );
            if ( pViewWin->SelectedPage() != nNewSelectedPage )
            {
                if ( pPagePrevwLay->IsPageVisible( nNewSelectedPage ) )
                {
                    pPagePrevwLay->MarkNewSelectedPage( nNewSelectedPage );
                    // OD 19.02.2003 #107369# - adjust position at vertical scrollbar.
                    SetVScrollbarThumbPos( nNewSelectedPage );
                    bRefresh = false;
                }
                else
                {
                    pViewWin->SetSelectedPage( nNewSelectedPage );
                    pViewWin->SetSttPage( nNewStartPage );
                    int nRet = ChgPage( SwPagePreViewWin::MV_SELPAGE, sal_True );
                    bRefresh = 0 != nRet;
                }
                GetViewShell()->ShowPreViewSelection( nNewSelectedPage );
                // OD 24.03.2003 #108282# - invalidate page status.
                static sal_uInt16 __READONLY_DATA aInval[] =
                {
                    FN_STAT_PAGE, 0
                };
                SfxBindings& rBindings = GetViewFrame()->GetBindings();
                rBindings.Invalidate( aInval );
                rReq.Done();
            }
			else
			{
				bRefresh = false;
			}
            break;
        }
		case FN_PAGEUP:
        case FN_PAGEDOWN:
        {
            _ExecPgUpAndPgDown( rReq.GetSlot() == FN_PAGEUP, &rReq );
            break;
        }
        case FN_START_OF_LINE:
        case FN_START_OF_DOCUMENT:
            pViewWin->SetSelectedPage( 1 );
			eMvMode = SwPagePreViewWin::MV_DOC_STT;	bRetVal = sal_True;	goto MOVEPAGE;
        case FN_END_OF_LINE:
        case FN_END_OF_DOCUMENT:
            pViewWin->SetSelectedPage( mnPageCount );
            eMvMode = SwPagePreViewWin::MV_DOC_END; bRetVal = sal_True; goto MOVEPAGE;
MOVEPAGE:
			{
				int nRet = ChgPage( eMvMode, sal_True );
				// return value fuer Basic
				if(bRetVal)
					rReq.SetReturnValue(SfxBoolItem(rReq.GetSlot(), nRet == 0));

				bRefresh = 0 != nRet;
				rReq.Done();
			}
			break;

		case FN_PRINT_PAGEPREVIEW:
		{
			const SwPagePreViewPrtData* pPPVPD = pViewWin->GetViewShell()->GetDoc()->GetPreViewPrtData();
			// die Sache mit der Orientation
			if(pPPVPD)
			{
				SfxPrinter* pPrinter = GetPrinter( sal_True );
				if((pPrinter->GetOrientation() == ORIENTATION_LANDSCAPE)
						!= pPPVPD->GetLandscape())
					pPrinter->SetOrientation(pPPVPD->GetLandscape() ? ORIENTATION_LANDSCAPE : ORIENTATION_PORTRAIT);
			}
			::SetAppPrintOptions( pViewWin->GetViewShell(), sal_False );
			bNormalPrint = sal_False;
			sal_uInt16 nPrtSlot = SID_PRINTDOC;
			rReq.SetSlot( nPrtSlot );
			SfxViewShell::ExecuteSlot( rReq, SfxViewShell::GetInterface() );
			rReq.SetSlot( FN_PRINT_PAGEPREVIEW );
			return;
		}
		case SID_PRINTDOCDIRECT:
		case SID_PRINTDOC:
			::SetAppPrintOptions( pViewWin->GetViewShell(), sal_False );
			bNormalPrint = sal_True;
			SfxViewShell::ExecuteSlot( rReq, SfxViewShell::GetInterface() );
			return;
		case FN_CLOSE_PAGEPREVIEW:
		case SID_PRINTPREVIEW:
			//	print preview is now always in the same frame as the tab view
			//	-> always switch this frame back to normal view
			//	(ScTabViewShell ctor reads stored view data)
            GetViewFrame()->GetDispatcher()->Execute( SID_VIEWSHELL0, 0, 0, SFX_CALLMODE_ASYNCHRON );
			break;
        case FN_INSERT_BREAK:
        {
            sal_uInt16 nSelPage = pViewWin->SelectedPage();
            //if a dummy page is selected (e.g. a non-existing right/left page)
            //the direct neighbor is used
            if(GetViewShell()->IsDummyPage( nSelPage ) && GetViewShell()->IsDummyPage( --nSelPage ))
                nSelPage +=2;
            SetNewPage( nSelPage );
            SfxViewFrame *pTmpFrm = GetViewFrame();
			pTmpFrm->GetBindings().Execute( SID_VIEWSHELL0, NULL, 0,
													SFX_CALLMODE_ASYNCHRON );
		}
        break;
        default:
			ASSERT(sal_False, "wrong dispatcher");
			return;
	}

	if( bRefresh )
		pViewWin->Invalidate();
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreView::GetState( SfxItemSet& rSet )
{
	SfxWhichIter aIter(rSet);
	sal_uInt8 nRow = 1;
	sal_uInt16 nWhich = aIter.FirstWhich();
    ASSERT(nWhich, leeres Set);
    SwPagePreviewLayout* pPagePrevwLay = GetViewShell()->PagePreviewLayout();
    //#106746# zoom has to be disabled if Accessibility support is switched on
    // MT 2010/01, see #110498#
    sal_Bool bZoomEnabled = sal_True; // !Application::GetSettings().GetMiscSettings().GetEnableATToolSupport();

	while(nWhich)
	{
		switch(nWhich)
		{
		case SID_BROWSER_MODE:
		case FN_PRINT_LAYOUT:
			rSet.DisableItem(nWhich);
			break;
		case FN_START_OF_DOCUMENT:
        {
            if ( pPagePrevwLay->IsPageVisible( 1 ) )
                rSet.DisableItem(nWhich);
            break;
        }
        case FN_END_OF_DOCUMENT:
        {
            if ( pPagePrevwLay->IsPageVisible( mnPageCount ) )
                rSet.DisableItem(nWhich);
            break;
        }
        case FN_PAGEUP:
        {
            if( pPagePrevwLay->GetWinPagesScrollAmount( -1 ) == 0 )
				rSet.DisableItem(nWhich);
			break;
        }
		case FN_PAGEDOWN:
        {
            if( pPagePrevwLay->GetWinPagesScrollAmount( 1 ) == 0 )
                rSet.DisableItem(nWhich);
            break;
        }

		case FN_STAT_PAGE:
			{
				String aStr( sPageStr );
                pViewWin->GetStatusStr( aStr, mnPageCount );
				rSet.Put( SfxStringItem( nWhich, aStr) );
			}
			break;

		case SID_ATTR_ZOOM:
		case FN_STAT_ZOOM:
			{
                if(bZoomEnabled)
                {
                    const SwViewOption* pVOpt = GetViewShell()->GetViewOptions();
                    SvxZoomItem aZoom((SvxZoomType)pVOpt->GetZoomType(),
                                        pVOpt->GetZoom());
                    aZoom.SetValueSet(
                            SVX_ZOOM_ENABLE_50|
                            SVX_ZOOM_ENABLE_75|
                            SVX_ZOOM_ENABLE_100|
                            SVX_ZOOM_ENABLE_150|
                            SVX_ZOOM_ENABLE_200);
                    rSet.Put( aZoom );
                }
                else
                    rSet.DisableItem(nWhich);
            }
        break;
        case SID_ATTR_ZOOMSLIDER :
            {
                if(bZoomEnabled)
                {
                    const SwViewOption* pVOpt = GetViewShell()->GetViewOptions();
                    const sal_uInt16 nCurrentZoom = pVOpt->GetZoom();
                    SvxZoomSliderItem aZoomSliderItem( nCurrentZoom, MINZOOM, MAXZOOM );
                    aZoomSliderItem.AddSnappingPoint( 100 );
                    rSet.Put( aZoomSliderItem );
                }
                else
                    rSet.DisableItem(nWhich);
            }
        break;
		case FN_PREVIEW_ZOOM:
		{
            if(bZoomEnabled)
            {
                const SwViewOption* pVOpt = GetViewShell()->GetViewOptions();
                rSet.Put(SfxUInt16Item(nWhich, pVOpt->GetZoom()));
            }
            else
                rSet.DisableItem(nWhich);
        }
		break;
        case SID_ZOOM_IN:
        case SID_ZOOM_OUT:
        {
            const SwViewOption* pVOpt = GetViewShell()->GetViewOptions();
            if(!bZoomEnabled || (SID_ZOOM_OUT == nWhich && pVOpt->GetZoom() >= MAX_PREVIEW_ZOOM)||
              (SID_ZOOM_IN == nWhich && pVOpt->GetZoom() <= MIN_PREVIEW_ZOOM))
            {
                rSet.DisableItem(nWhich);
            }
        }
        break;
        case FN_SHOW_MULTIPLE_PAGES:
        //should never be disabled
        break;
		case FN_SHOW_BOOKVIEW:
		{
			sal_Bool b = GetViewShell()->GetViewOptions()->IsPagePrevBookview();
			rSet.Put(SfxBoolItem(nWhich, b));
		}
		break;

		case FN_SHOW_TWO_PAGES:
			if( 2 == pViewWin->GetCol() && nRow == pViewWin->GetRow() )
				rSet.DisableItem( nWhich );
			break;

		case FN_PRINT_PAGEPREVIEW:
			// hat den gleichen Status wie das normale Drucken
			{
				const SfxPoolItem* pItem;
				SfxItemSet aSet( *rSet.GetPool(), SID_PRINTDOC, SID_PRINTDOC );
				GetSlotState( SID_PRINTDOC, SfxViewShell::GetInterface(), &aSet );
				if( SFX_ITEM_DISABLED == aSet.GetItemState( SID_PRINTDOC,
						sal_False, &pItem ))
					rSet.DisableItem( nWhich );
				else if( SFX_ITEM_SET == aSet.GetItemState( SID_PRINTDOC,
						sal_False, &pItem ))
				{
					((SfxPoolItem*)pItem)->SetWhich( FN_PRINT_PAGEPREVIEW );
					rSet.Put( *pItem );
				}
			}
			break;

		case SID_PRINTPREVIEW:
			rSet.Put( SfxBoolItem( nWhich, sal_True ) );
			break;

		case SID_PRINTDOC:
		case SID_PRINTDOCDIRECT:
			GetSlotState( nWhich, SfxViewShell::GetInterface(),	&rSet );
			break;
		}
		nWhich = aIter.NextWhich();
	}
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreView::StateUndo(SfxItemSet& rSet)
{
	SfxWhichIter aIter(rSet);
	sal_uInt16 nWhich = aIter.FirstWhich();

	while (nWhich)
	{
		rSet.DisableItem(nWhich);
		nWhich = aIter.NextWhich();
	}
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreView::Init(const SwViewOption * pPrefs)
{
    if ( GetViewShell()->HasDrawView() )
        GetViewShell()->GetDrawView()->SetAnimationEnabled( sal_False );

	bNormalPrint = sal_True;

	// Die DocSize erfragen und verarbeiten. Ueber die Handler konnte
	// die Shell nicht gefunden werden, weil die Shell innerhalb CTOR-Phase
	// nicht in der SFX-Verwaltung bekannt ist.

	if( !pPrefs )
		pPrefs = SW_MOD()->GetUsrPref(sal_False);

	// die Felder aktualisieren
	// ACHTUNG: hochcasten auf die EditShell, um die SS zu nutzen.
	//			In den Methoden wird auf die akt. Shell abgefragt!
    SwEditShell* pESh = (SwEditShell*)GetViewShell();
	sal_Bool bIsModified = pESh->IsModified();


	SwViewOption aOpt( *pPrefs );
    aOpt.SetPagePreview(sal_True);
	aOpt.SetTab( sal_False );
	aOpt.SetBlank( sal_False );
	aOpt.SetHardBlank( sal_False );
	aOpt.SetParagraph( sal_False );
	aOpt.SetLineBreak( sal_False );
	aOpt.SetPageBreak( sal_False );
	aOpt.SetColumnBreak( sal_False );
	aOpt.SetSoftHyph( sal_False );
	aOpt.SetFldName( sal_False );
	aOpt.SetPostIts( sal_False );
    aOpt.SetShowHiddenChar( sal_False );
    aOpt.SetShowHiddenField( sal_False );
    aOpt.SetShowHiddenPara( sal_False );
    aOpt.SetViewHRuler( sal_False );
    aOpt.SetViewVRuler( sal_False );
	aOpt.SetGraphic( sal_True );
    aOpt.SetTable( sal_True );
    aOpt.SetSnap( sal_False );
	aOpt.SetGridVisible( sal_False );

    GetViewShell()->ApplyViewOptions( aOpt );
    GetViewShell()->ApplyAccessiblityOptions(SW_MOD()->GetAccessibilityOptions());

    // OD 09.01.2003 #i6467# - adjust view shell option to the same as for print
    SwPrintData const aPrintOptions = *SW_MOD()->GetPrtOptions(false);
    GetViewShell()->AdjustOptionsForPagePreview( aPrintOptions );

    GetViewShell()->CalcLayout();
    DocSzChgd( GetViewShell()->GetDocSize() );

	if( !bIsModified )
		pESh->ResetModified();

    pVScrollbar->ExtendedShow(pPrefs->IsViewVScrollBar());
    pHScrollbar->ExtendedShow(pPrefs->IsViewHScrollBar());
    pScrollFill->Show(pPrefs->IsViewVScrollBar() && pPrefs->IsViewHScrollBar());
}


/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


SwPagePreView::SwPagePreView(SfxViewFrame *pViewFrame, SfxViewShell* pOldSh):
	SfxViewShell( pViewFrame, SWVIEWFLAGS ),
	pViewWin( new SwPagePreViewWin(&(GetViewFrame())->GetWindow(), *this ) ),
	nNewPage(USHRT_MAX),
	pHScrollbar(0),
	pVScrollbar(0),
	pPageUpBtn(0),
	pPageDownBtn(0),
    pScrollFill(new ScrollBarBox( &pViewFrame->GetWindow(),
        pViewFrame->GetFrame().GetParentFrame() ? 0 : WB_SIZEABLE )),
    mnPageCount( 0 ),
    // OD 09.01.2003 #106334#
    mbResetFormDesignMode( false ),
    mbFormDesignModeToReset( false )
{
	SetName(String::CreateFromAscii("PageView" ));
	SetWindow( pViewWin );
	SetHelpId(SW_PAGEPREVIEW);
    _CreateScrollbar( sal_True );
    _CreateScrollbar( sal_False );

    SfxObjectShell* pObjShell = pViewFrame->GetObjectShell();
	if ( !pOldSh )
	{
		//Gibt es schon eine Sicht auf das Dokument?
		SfxViewFrame *pF = SfxViewFrame::GetFirst( pObjShell );
		if ( pF == pViewFrame )
			pF = SfxViewFrame::GetNext( *pF, pObjShell );
		if ( pF )
			pOldSh = pF->GetViewShell();
	}

	ViewShell *pVS, *pNew;

	if( pOldSh && pOldSh->IsA( TYPE( SwPagePreView ) ) )
        pVS = ((SwPagePreView*)pOldSh)->GetViewShell();
	else
	{
		if( pOldSh && pOldSh->IsA( TYPE( SwView ) ) )
		{
			pVS = ((SwView*)pOldSh)->GetWrtShellPtr();
			// save the current ViewData of the previous SwView
			pOldSh->WriteUserData( sSwViewData, sal_False );
		}
		else
			pVS = GetDocShell()->GetWrtShell();
		if( pVS )
		{
			// setze die akt. Seite als die erste
			sal_uInt16 nPhysPg, nVirtPg;
			((SwCrsrShell*)pVS)->GetPageNum( nPhysPg, nVirtPg, /*sal_False*/sal_True, sal_False );
			if( 1 != pViewWin->GetCol() && 1 == nPhysPg )
				--nPhysPg;
			pViewWin->SetSttPage( nPhysPg );
		}
	}

    // OD 09.01.2003 #106334# - for form shell remember design mode of draw view
    // of previous view shell
    if ( pVS && pVS->HasDrawView() )
    {
        mbResetFormDesignMode = true;
        mbFormDesignModeToReset = pVS->GetDrawView()->IsDesignMode();
    }

	if( pVS )
		pNew = new ViewShell( *pVS, pViewWin, 0, VSHELLFLAG_ISPREVIEW );
	else
		pNew = new ViewShell(
				*((SwDocShell*)pViewFrame->GetObjectShell())->GetDoc(),
				pViewWin, 0, 0, VSHELLFLAG_ISPREVIEW );

	pViewWin->SetViewShell( pNew );
	pNew->SetSfxViewShell( this );
	Init();
}


/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


 SwPagePreView::~SwPagePreView()
{
	SetWindow( 0 );
	ViewShell* pVShell =  pViewWin->GetViewShell();
	pVShell->SetWin(0);
	//pViewWin->Hide();
	if( pVShell)
		delete pVShell;
	delete pViewWin;
	//pViewWin->Hide();

	delete pScrollFill;
	delete pHScrollbar;
	delete pVScrollbar;
	delete pPageUpBtn;
	delete pPageDownBtn;

/*    SfxObjectShell* pDocSh = GetDocShell();
	for( SfxViewFrame *pFrame = SfxViewFrame::GetFirst( pDocSh );
		pFrame; pFrame = SfxViewFrame::GetNext( *pFrame, pDocSh ) )
		if( pFrame != GetViewFrame() )
		{
			// es gibt noch eine weitere Sicht auf unser Dokument, also
			// aktiviere dieses
			pFrame->GetFrame().Appear();
			break;
		}
*/}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


SwDocShell* SwPagePreView::GetDocShell()
{
	return PTR_CAST(SwDocShell, GetViewFrame()->GetObjectShell());
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


int SwPagePreView::_CreateScrollbar( sal_Bool bHori )
{
	Window *pMDI = &GetViewFrame()->GetWindow();
	SwScrollbar** ppScrollbar = bHori ? &pHScrollbar : &pVScrollbar;

	ASSERT( !*ppScrollbar, "vorher abpruefen!" )

	if( !bHori )
	{

		pPageUpBtn		= new ImageButton(pMDI, SW_RES( BTN_PAGEUP ) );
		pPageUpBtn->SetHelpId(GetStaticInterface()->GetSlot(FN_PAGEUP)->GetCommand());
		pPageDownBtn	= new ImageButton(pMDI, SW_RES( BTN_PAGEDOWN ) );
		pPageDownBtn->SetHelpId(GetStaticInterface()->GetSlot(FN_PAGEDOWN)->GetCommand());
		Link aLk( LINK( this, SwPagePreView, BtnPage ) );
		pPageUpBtn->SetClickHdl( aLk );
		pPageDownBtn->SetClickHdl( aLk );
		pPageUpBtn->Show();
		pPageDownBtn->Show();
	}

	*ppScrollbar = new SwScrollbar( pMDI, bHori );

    ScrollDocSzChg();
    (*ppScrollbar)->EnableDrag( sal_True );
    (*ppScrollbar)->SetEndScrollHdl( LINK( this, SwPagePreView, EndScrollHdl ));


	(*ppScrollbar)->SetScrollHdl( LINK( this, SwPagePreView, ScrollHdl ));

	InvalidateBorder();
    (*ppScrollbar)->ExtendedShow();
	return 1;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/



/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

/*
 * Button-Handler
 */
IMPL_LINK_INLINE_START( SwPagePreView, BtnPage, Button *, pButton )
{
    // OD 04.03.2003 #107369# - use new helper method to perform page up
    // respectively page down.
    _ExecPgUpAndPgDown( pButton == pPageUpBtn );
	return 0;
}
IMPL_LINK_INLINE_END( SwPagePreView, BtnPage, Button *, pButton )

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


int SwPagePreView::ChgPage( int eMvMode, int bUpdateScrollbar )
{
	Rectangle aPixVisArea( pViewWin->LogicToPixel( aVisArea ) );
	int bChg = pViewWin->MovePage( eMvMode ) ||
               eMvMode == SwPagePreViewWin::MV_CALC ||
               eMvMode == SwPagePreViewWin::MV_NEWWINSIZE;
	aVisArea = pViewWin->PixelToLogic( aPixVisArea );

	if( bChg )
	{
		// Statusleiste updaten
		String aStr( sPageStr );
        pViewWin->GetStatusStr( aStr, mnPageCount );
		SfxBindings& rBindings = GetViewFrame()->GetBindings();

		if( bUpdateScrollbar )
		{
            ScrollViewSzChg();

			static sal_uInt16 __READONLY_DATA aInval[] =
			{
				FN_START_OF_DOCUMENT, FN_END_OF_DOCUMENT,
				FN_PAGEUP, FN_PAGEDOWN, 0
			};
			rBindings.Invalidate( aInval );
		}
		rBindings.SetState( SfxStringItem( FN_STAT_PAGE, aStr ) );
	}
	return bChg;
}


/*  */
/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


// ab hier alles aus der SwView uebernommen


void SwPagePreView::CalcAndSetBorderPixel( SvBorder &rToFill, sal_Bool /*bInner*/ )
{
//	const long nAdd = bInner ? 0 : ScrollBar::GetWindowOverlapPixel();
	const StyleSettings &rSet = pViewWin->GetSettings().GetStyleSettings();
	const long nTmp = rSet.GetScrollBarSize();// - nAdd;
	if ( pVScrollbar->IsVisible( sal_False ))
		rToFill.Right()  = nTmp;
	if ( pHScrollbar->IsVisible( sal_False ) )
		rToFill.Bottom() = nTmp;
	SetBorderPixel( rToFill );
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreView::InnerResizePixel( const Point &rOfst, const Size &rSize )
{
	SvBorder aBorder;
	CalcAndSetBorderPixel( aBorder, sal_True );
	Rectangle aRect( rOfst, rSize );
	aRect += aBorder;
	ViewResizePixel( *pViewWin, aRect.TopLeft(), aRect.GetSize(),
					pViewWin->GetOutputSizePixel(),
					sal_True,
                    *pVScrollbar, *pHScrollbar, pPageUpBtn, pPageDownBtn, 0,
                    *pScrollFill );

	//EditWin niemals einstellen!
	//VisArea niemals einstellen!
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void  SwPagePreView::OuterResizePixel( const Point &rOfst, const Size &rSize )
{
	SvBorder aBorder;
	CalcAndSetBorderPixel( aBorder, sal_False );
	ViewResizePixel( *pViewWin, rOfst, rSize, pViewWin->GetOutputSizePixel(),
                        sal_False, *pVScrollbar,
                        *pHScrollbar, pPageUpBtn, pPageDownBtn, 0, *pScrollFill );

	//EditWin niemals einstellen!

	Size aTmpSize( pViewWin->GetOutputSizePixel() );
	Point aBottomRight( pViewWin->PixelToLogic( Point( aTmpSize.Width(), aTmpSize.Height() ) ) );
	SetVisArea( Rectangle( Point(), aBottomRight ) );

	//Aufruf der DocSzChgd-Methode der Scrollbars ist noetig, da vom maximalen
	//Scrollrange immer die halbe Hoehe der VisArea abgezogen wird.
	if ( pVScrollbar &&
             aTmpSize.Width() > 0 && aTmpSize.Height() > 0 )
        {
            ScrollDocSzChg();
        }
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreView::SetVisArea( const Rectangle &rRect, sal_Bool bUpdateScrollbar )
{
	const Point aTopLeft(AlignToPixel(rRect.TopLeft()));
	const Point aBottomRight(AlignToPixel(rRect.BottomRight()));
	Rectangle aLR(aTopLeft,aBottomRight);

	if(aLR == aVisArea)
		return;
		// keine negative Position, keine neg. Groesse

	if(aLR.Top() < 0)
	{
		aLR.Bottom() += Abs(aLR.Top());
		aLR.Top() = 0;
	}

	if(aLR.Left() < 0)
	{
		aLR.Right() += Abs(aLR.Left());
		aLR.Left() = 0;
	}
	if(aLR.Right() < 0) aLR.Right() = 0;
	if(aLR.Bottom() < 0) aLR.Bottom() = 0;
	if(aLR == aVisArea ||
		// JP 29.10.97: Bug 45173 - Leeres Rechteck nicht beachten
		( 0 == aLR.Bottom() - aLR.Top() && 0 == aLR.Right() - aLR.Left() ) )
		return;

	if( aLR.Left() > aLR.Right() || aLR.Top() > aLR.Bottom() )
		return;

	//Bevor die Daten veraendert werden ggf. ein Update rufen. Dadurch wird
	//sichergestellt, da? anliegende Paints korrekt in Dokumentkoordinaten
	//umgerechnet werden.
	//Vorsichtshalber tun wir das nur wenn an der Shell eine Action laeuft,
	//denn dann wir nicht wirklich gepaintet sondern die Rechtecke werden
	//lediglich (in Dokumentkoordinaten) vorgemerkt.
	if( GetViewShell()->ActionPend() )
		pViewWin->Update();

	// setze am View-Win die aktuelle Size
	aVisArea = aLR;
	pViewWin->SetWinSize( aLR.GetSize() );
	// OD 18.12.2002 #103492# - use new mode
	ChgPage( SwPagePreViewWin::MV_NEWWINSIZE, bUpdateScrollbar );

	pViewWin->Invalidate();
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


IMPL_LINK( SwPagePreView, ScrollHdl, SwScrollbar *, pScrollbar )
{
    if(!GetViewShell())
        return 0;
    if( !pScrollbar->IsHoriScroll() &&
        pScrollbar->GetType() == SCROLL_DRAG &&
        Help::IsQuickHelpEnabled() &&
        GetViewShell()->PagePreviewLayout()->DoesPreviewLayoutRowsFitIntoWindow())
	{
		// wieviele Seiten scrollen ??
		String sStateStr(sPageStr);
		sal_uInt16 nThmbPos = (sal_uInt16)pScrollbar->GetThumbPos();
		if( 1 == pViewWin->GetCol() || !nThmbPos )
			++nThmbPos;
		sStateStr += String::CreateFromInt32( nThmbPos );
		Point aPos = pScrollbar->GetParent()->OutputToScreenPixel(
										pScrollbar->GetPosPixel());
		aPos.Y() = pScrollbar->OutputToScreenPixel(pScrollbar->GetPointerPosPixel()).Y();
		Size aSize = pScrollbar->GetSizePixel();
		Rectangle aRect;
		aRect.Left() 	= aPos.X() -8;
		aRect.Right() 	= aRect.Left();
		aRect.Top()		= aPos.Y();
		aRect.Bottom()	= aRect.Top();

		Help::ShowQuickHelp(pScrollbar, aRect, sStateStr,
				QUICKHELP_RIGHT|QUICKHELP_VCENTER);

	}
	else
		EndScrollHdl( pScrollbar );
	return 0;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


IMPL_LINK( SwPagePreView, EndScrollHdl, SwScrollbar *, pScrollbar )
{
    if(!GetViewShell())
        return 0;

    // OD 04.03.2003 #107369# - boolean to avoid unnecessary invalidation of the window.
    bool bInvalidateWin = true;

    if( !pScrollbar->IsHoriScroll() )       // scroll vertically
	{
        if ( Help::IsQuickHelpEnabled() )
			Help::ShowQuickHelp(pScrollbar, Rectangle(), aEmptyStr, 0);
        if ( GetViewShell()->PagePreviewLayout()->DoesPreviewLayoutRowsFitIntoWindow() )
        {
            // wieviele Seiten scrollen ??
            sal_uInt16 nThmbPos = (sal_uInt16)pScrollbar->GetThumbPos();
            // OD 05.12.2002 #103492# - adjust to new preview functionality
            if( nThmbPos != pViewWin->SelectedPage() )
            {
                // OD 17.01.2003 #103492# - consider case that page <nThmbPos>
                // is already visible
                SwPagePreviewLayout* pPagePrevwLay = GetViewShell()->PagePreviewLayout();
                if ( pPagePrevwLay->IsPageVisible( nThmbPos ) )
                {
                    pPagePrevwLay->MarkNewSelectedPage( nThmbPos );
                    // OD 04.03.2003 #107369# - invalidation of window is unnecessary
                    bInvalidateWin = false;
                }
                else
                {
                    // OD 17.01.2003 #103492# - consider whether layout columns
                    // fit or not.
                    if ( !pPagePrevwLay->DoesPreviewLayoutColsFitIntoWindow() )
                    {
                        pViewWin->SetSttPage( nThmbPos );
                        pViewWin->SetSelectedPage( nThmbPos );
                        ChgPage( SwPagePreViewWin::MV_SCROLL, sal_False );
                        // OD 20.01.2003 #103492# - update scrollbars
                        ScrollViewSzChg();
                    }
                    else
                    {
                        // OD 04.03.2003 #107369# - correct scroll amount
                        const sal_Int16 nPageDiff = nThmbPos - pViewWin->SelectedPage();
                        const sal_uInt16 nVisPages = pViewWin->GetRow() * pViewWin->GetCol();
                        sal_Int16 nWinPagesToScroll = nPageDiff / nVisPages;
                        if ( nPageDiff % nVisPages )
                        {
                            // decrease/increase number of preview pages to scroll
                            nPageDiff < 0 ? --nWinPagesToScroll : ++nWinPagesToScroll;
                        }
                        pViewWin->SetSelectedPage( nThmbPos );
                        pViewWin->Scroll( 0, pPagePrevwLay->GetWinPagesScrollAmount( nWinPagesToScroll ) );
                    }
                }
                // OD 17.01.2003 #103492# - update accessibility
                GetViewShell()->ShowPreViewSelection( nThmbPos );
            }
            else
            {
                // OD 04.03.2003 #107369# - invalidation of window is unnecessary
                bInvalidateWin = false;
            }
        }
        else
        {
            long nThmbPos = pScrollbar->GetThumbPos();
            pViewWin->Scroll(0, nThmbPos - pViewWin->GetPaintedPreviewDocRect().Top());
        }
	}
    else
    {
        long nThmbPos = pScrollbar->GetThumbPos();
        pViewWin->Scroll(nThmbPos - pViewWin->GetPaintedPreviewDocRect().Left(), 0);
    }
    // OD 24.03.2003 #108282# - additional invalidate page status.
    static sal_uInt16 __READONLY_DATA aInval[] =
    {
        FN_START_OF_DOCUMENT, FN_END_OF_DOCUMENT, FN_PAGEUP, FN_PAGEDOWN,
        FN_STAT_PAGE, 0
    };
    SfxBindings& rBindings = GetViewFrame()->GetBindings();
    rBindings.Invalidate( aInval );
    // OD 04.03.2003 #107369# - control invalidation of window
    if ( bInvalidateWin )
    {
        pViewWin->Invalidate();
    }
    return 0;
}
/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


Point SwPagePreView::AlignToPixel(const Point &rPt) const
{
	return pViewWin->PixelToLogic( pViewWin->LogicToPixel( rPt ) );
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreView::DocSzChgd( const Size &rSz )
{
	if( aDocSz == rSz )
		return;

	aDocSz = rSz;

    // --> OD 2009-08-20 #i96726#
    // Due to the multiple page layout it is needed to trigger recalculation
    // of the page preview layout, even if the count of pages is not changing.
    mnPageCount = GetViewShell()->GetNumPages();

    if( aVisArea.GetWidth() )
	{
		ChgPage( SwPagePreViewWin::MV_CALC, sal_True );
        ScrollDocSzChg();

		pViewWin->Invalidate();
	}
    // <--
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreView::ScrollViewSzChg()
{
    if(!GetViewShell())
        return ;
    if(pVScrollbar)
    {
        if(GetViewShell()->PagePreviewLayout()->DoesPreviewLayoutRowsFitIntoWindow())
        {
            //vertical scrolling by row
            // OD 04.12.2002 #103492# - adjust to new preview functionality
            sal_uInt16 nVisPages = pViewWin->GetRow() * pViewWin->GetCol();
            pVScrollbar->SetVisibleSize( nVisPages );
            // OD 19.02.2003 #107369# - set selected page as scroll bar position,
            // if it is visible.
            SwPagePreviewLayout* pPagePrevwLay = GetViewShell()->PagePreviewLayout();
            if ( pPagePrevwLay->IsPageVisible( pViewWin->SelectedPage() ) )
            {
                pVScrollbar->SetThumbPos( pViewWin->SelectedPage() );
            }
            else
            {
                pVScrollbar->SetThumbPos( pViewWin->GetSttPage() );
            }
            pVScrollbar->SetLineSize( pViewWin->GetCol() );
            pVScrollbar->SetPageSize( nVisPages );
            // calculate and set scrollbar range
            Range aScrollbarRange( 1, mnPageCount );
            // increase range by one, because left-top-corner is left blank.
            ++aScrollbarRange.Max();
            // OD 17.01.2003 #103492# - increase range in order to access all pages
            aScrollbarRange.Max() += ( nVisPages - 1 );
            pVScrollbar->SetRange( aScrollbarRange );
        }
        else //vertical scrolling by pixel
        {
            const Rectangle& rDocRect = pViewWin->GetPaintedPreviewDocRect();
            const Size& rPreviewSize =
                    GetViewShell()->PagePreviewLayout()->GetPrevwDocSize();
            pVScrollbar->SetRangeMax(rPreviewSize.Height()) ;
            long nVisHeight = rDocRect.GetHeight();
            pVScrollbar->SetVisibleSize( nVisHeight );
            pVScrollbar->SetThumbPos( rDocRect.Top() );
            pVScrollbar->SetLineSize( nVisHeight / 10 );
            pVScrollbar->SetPageSize( nVisHeight / 2 );
        }
    }
    if(pHScrollbar)
    {
        const Rectangle& rDocRect = pViewWin->GetPaintedPreviewDocRect();
        const Size& rPreviewSize =
                GetViewShell()->PagePreviewLayout()->GetPrevwDocSize();
        long nVisWidth = 0;
        long nThumb   = 0;
        Range aRange(0,0);

        if(rDocRect.GetWidth() < rPreviewSize.Width())
        {
            nVisWidth = rDocRect.GetWidth();
            nThumb = rDocRect.Left();
            aRange = Range(0, rPreviewSize.Width());
        }
        pHScrollbar->SetRange( aRange );
        pHScrollbar->SetVisibleSize( nVisWidth );
        pHScrollbar->SetThumbPos( nThumb );
        pHScrollbar->SetLineSize( nVisWidth / 10 );
        pHScrollbar->SetPageSize( nVisWidth / 2 );
    }
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


void SwPagePreView::ScrollDocSzChg()
{
    ScrollViewSzChg();
}


/*  */
/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


// alles zum Thema Drucken

SfxPrinter*  SwPagePreView::GetPrinter( sal_Bool bCreate )
{
    return pViewWin->GetViewShell()->getIDocumentDeviceAccess()->getPrinter( bCreate );
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


sal_uInt16  SwPagePreView::SetPrinter( SfxPrinter *pNew, sal_uInt16 nDiffFlags, bool )
{
    ViewShell &rSh = *GetViewShell();
    SfxPrinter* pOld = rSh.getIDocumentDeviceAccess()->getPrinter( false );
    if ( pOld && pOld->IsPrinting() )
        return SFX_PRINTERROR_BUSY;

	SwEditShell &rESh = (SwEditShell&)rSh;	//Buh...
	if( ( SFX_PRINTER_PRINTER | SFX_PRINTER_JOBSETUP ) & nDiffFlags )
	{
        rSh.getIDocumentDeviceAccess()->setPrinter( pNew, true, true );
		if( nDiffFlags & SFX_PRINTER_PRINTER )
			rESh.SetModified();
	}
	if ( ( nDiffFlags & SFX_PRINTER_OPTIONS ) == SFX_PRINTER_OPTIONS )
		::SetPrinter( rSh.getIDocumentDeviceAccess(), pNew, sal_False );

	const sal_Bool bChgOri = nDiffFlags & SFX_PRINTER_CHG_ORIENTATION ? sal_True : sal_False;
	const sal_Bool bChgSize= nDiffFlags & SFX_PRINTER_CHG_SIZE ? sal_True : sal_False;
	if ( bChgOri || bChgSize )
	{
		rESh.StartAllAction();
		if ( bChgOri )
			rSh.ChgAllPageOrientation( sal_uInt16(pNew->GetOrientation()) );
		if ( bChgSize )
		{
			Size aSz( SvxPaperInfo::GetPaperSize( pNew ) );
			rSh.ChgAllPageSize( aSz );
		}
		if( !bNormalPrint )
			pViewWin->CalcWish( pViewWin->GetRow(), pViewWin->GetCol() );
		rESh.SetModified();
		rESh.EndAllAction();

		static sal_uInt16 __READONLY_DATA aInval[] =
		{
			SID_ATTR_LONG_ULSPACE, SID_ATTR_LONG_LRSPACE,
			SID_RULER_BORDERS, SID_RULER_PAGE_POS, 0
		};
#ifdef DBG_UTIL
	{
		const sal_uInt16* pPtr = aInval + 1;
		do {
			ASSERT( *(pPtr - 1) < *pPtr, "falsche Sortierung!" );
		} while( *++pPtr );
	}
#endif

		GetViewFrame()->GetBindings().Invalidate(aInval);
	}

	return 0;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


SfxTabPage*  SwPagePreView::CreatePrintOptionsPage( Window *pParent,
												const SfxItemSet &rOptions )
{
	return ::CreatePrintOptionsPage( pParent, rOptions, !bNormalPrint );
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


// OD 18.12.2002 #103492# - no longer needed ??
Size  SwPagePreView::GetOptimalSizePixel() const
{
    ASSERT( false, "overloaded virtual method <SwPagePreView::GetOptimalSizePixel()> needed ??" )
    return Size( -1, -1 );
/*
    //JP 09.06.99: was wird hier errechnet ?????
// ALT:
//   SfxApplicationWindow* pWin = SFX_APPWINDOW ;
//   Rectangle aRect = pWin->GetClientAreaPixel();

	Window& rWin = GetViewFrame()->GetWindow();
	Rectangle aRect( Point(0, 0), rWin.GetOutputSizePixel() );
	Size aMaxSize( aRect.GetWidth(), aRect.GetHeight() );
	Size aInSize = rWin.GetOutputSizePixel();
	Size aOutSize = rWin.GetSizePixel();
	sal_uInt16 nXBorder = sal_uInt16(aOutSize.Width() - aInSize.Width());
	sal_uInt16 nYBorder = sal_uInt16(aOutSize.Height() - aInSize.Height());
	aMaxSize.Width() -= nXBorder;
	//'auf Verdacht' etwas vom Border abziehen (Menue)
	nYBorder -= (nYBorder - nXBorder) / 2;
	aMaxSize.Height() -= nYBorder;
	//mit der max. moeglichen Outputsize guenstigstes Verhaeltnis ausrechnen
	pViewWin->GetOptimalSize(aMaxSize);
	// Border wieder dazuzaehlen
	aMaxSize.Height() += nYBorder;
	aMaxSize.Width() += nXBorder;
	return aMaxSize;
*/
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

// OD 12.12.2002 #103492#
void SwPagePreViewWin::SetViewShell( ViewShell* pShell )
{
    mpViewShell = pShell;
    if ( mpViewShell && mpViewShell->IsPreView() )
    {
        mpPgPrevwLayout = mpViewShell->PagePreviewLayout();
    }
}

void SwPagePreViewWin::RepaintCoreRect( const SwRect& rRect )
{
    // OD 2004-03-04 #i24183#
    if ( mpPgPrevwLayout->PreviewLayoutValid() )
    {
        mpPgPrevwLayout->Repaint( Rectangle( rRect.Pos(), rRect.SSize() ) );
    }
}

/** method to adjust preview to a new zoom factor

    OD 02.12.2002 #103492#
    OD 24.09.2003 #i19975# - also consider zoom type - adding parameter <_eZoomType>
*/
void SwPagePreViewWin::AdjustPreviewToNewZoom( const sal_uInt16 _nZoomFactor,
                                               const SvxZoomType _eZoomType )
{
    // OD 24.09.2003 #i19975# - consider zoom type
    if ( _eZoomType == SVX_ZOOM_WHOLEPAGE )
    {
        mnRow = 1;
        mnCol = 1;
        mpPgPrevwLayout->Init( mnCol, mnRow, maPxWinSize, true );
        mpPgPrevwLayout->Prepare( mnSttPage, Point(0,0), maPxWinSize,
                                  mnSttPage, maPaintedPreviewDocRect );
        SetSelectedPage( mnSttPage );
        SetPagePreview(mnRow, mnCol);
        maScale = GetMapMode().GetScaleX();
    }
    else if ( _nZoomFactor != 0 )
    {
        // calculate new scaling and set mapping mode appropriately.
        Fraction aNewScale( _nZoomFactor, 100 );
        MapMode aNewMapMode = GetMapMode();
        aNewMapMode.SetScaleX( aNewScale );
        aNewMapMode.SetScaleY( aNewScale );
        SetMapMode( aNewMapMode );

        // calculate new start position for preview paint
        Size aNewWinSize = PixelToLogic( maPxWinSize );
        Point aNewPaintStartPos =
                mpPgPrevwLayout->GetPreviewStartPosForNewScale( aNewScale, maScale, aNewWinSize );

        // remember new scaling and prepare preview paint
        // Note: paint of preview will be performed by a corresponding invalidate
        //          due to property changes.
        maScale = aNewScale;
        mpPgPrevwLayout->Prepare( 0, aNewPaintStartPos, maPxWinSize,
                                  mnSttPage, maPaintedPreviewDocRect );
    }

}
/* -----------------04.12.2002 10:46-----------------
 * pixel scrolling - horizontally always or vertically
 * when less than the desired number of rows fits into
 * the view
 * --------------------------------------------------*/
void SwPagePreViewWin::Scroll(long nXMove, long nYMove, sal_uInt16 /*nFlags*/)
{
    maPaintedPreviewDocRect.Move(nXMove, nYMove);
    mpPgPrevwLayout->Prepare( 0, maPaintedPreviewDocRect.TopLeft(),
                              maPxWinSize, mnSttPage,
                              maPaintedPreviewDocRect );

}

sal_Bool SwPagePreView::HandleWheelCommands( const CommandEvent& rCEvt )
{
	sal_Bool bOk = sal_False;
	const CommandWheelData* pWData = rCEvt.GetWheelData();
	if( pWData && COMMAND_WHEEL_ZOOM == pWData->GetMode() )
	{
	//IAccessibility2 Implementation 2009-----, only the Preference shouldn't control the Zoom, it is better to detect AT tools running. So the bridge can be used here
        //if(!Application::GetSettings().GetMiscSettings().GetEnableATToolSupport())
	if ( !Application::IsAccessibilityEnabled() )
        {
            sal_uInt16 nFactor = GetViewShell()->GetViewOptions()->GetZoom();
            const sal_uInt16 nOffset = 10;
            if( 0L > pWData->GetDelta() )
            {
                nFactor -= nOffset;
                if(nFactor < MIN_PREVIEW_ZOOM)
                        nFactor = MIN_PREVIEW_ZOOM;
            }
            else
            {
                nFactor += nOffset;
                if(nFactor > MAX_PREVIEW_ZOOM)
                        nFactor = MAX_PREVIEW_ZOOM;
            }
            SetZoom(SVX_ZOOM_PERCENT, nFactor);
        }
		bOk = sal_True;
	}
	else
        bOk = pViewWin->HandleScrollCommand( rCEvt, pHScrollbar, pVScrollbar );
	return bOk;
}


uno::Reference< ::com::sun::star::accessibility::XAccessible >
    SwPagePreViewWin::CreateAccessible()
{
	vos::OGuard aGuard(Application::GetSolarMutex());	// this should have
														// happened already!!!

    DBG_ASSERT( GetViewShell() != NULL, "We need a view shell" );
	::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > xAcc = GetAccessible( sal_False );
	if (xAcc.is())
	{
		return xAcc;
	}
	if (mpViewShell)
	{
		::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > xAccPreview = mpViewShell->CreateAccessiblePreview();
		SetAccessible(xAccPreview);
	}
	return GetAccessible( sal_False );
}
// MT: Removed Windows::SwitchView() introduced with IA2 CWS.
// There are other notifications for this when the active view has changed, so please update the code to use that event mechanism
void SwPagePreViewWin::SwitchView()
{
#ifdef ACCESSIBLE_LAYOUT
	if (!Application::IsAccessibilityEnabled())
	{
		return ;
	}
	if (mpViewShell)
	{
		mpViewShell->InvalidateAccessibleFocus();
	}
#endif
}

/* -----------------------------06.05.2002 13:18------------------------------

 ---------------------------------------------------------------------------*/
void SwPagePreView::ApplyAccessiblityOptions(SvtAccessibilityOptions& rAccessibilityOptions)
{
    GetViewShell()->ApplyAccessiblityOptions(rAccessibilityOptions);
}
/* -----------------------------2002/06/26 14:30------------------------------

 ---------------------------------------------------------------------------*/
void SwPagePreView::ShowHScrollbar(sal_Bool bShow)
{
    pHScrollbar->Show(bShow);
	InvalidateBorder();
}

/* -----------------------------2002/06/26 14:30------------------------------

 ---------------------------------------------------------------------------*/
void SwPagePreView::ShowVScrollbar(sal_Bool bShow)
{
    pVScrollbar->Show(bShow);
	InvalidateBorder();
}

/* -----------------25.11.2002 16:36-----------------
 *
 * --------------------------------------------------*/
void SwPagePreView::SetZoom(SvxZoomType eType, sal_uInt16 nFactor)
{
    ViewShell& rSh = *GetViewShell();
    SwViewOption aOpt(*rSh.GetViewOptions());
    // OD 16.12.2002 #103492# - perform action only on changes of zoom or zoom type.
    if ( aOpt.GetZoom() != nFactor ||
         aOpt.GetZoomType() != eType )
    {
        aOpt.SetZoom(nFactor);
        aOpt.SetZoomType(eType);
        rSh.ApplyViewOptions( aOpt );
        lcl_InvalidateZoomSlots(GetViewFrame()->GetBindings());
        // OD 02.12.2002 #103492#
        // OD 24.09.2003 #i19975# - also consider zoom type
        pViewWin->AdjustPreviewToNewZoom( nFactor, eType );
        ScrollViewSzChg();
    }
}

/** adjust position of vertical scrollbar

    OD 19.02.2003 #107369

    @author OD
*/
void SwPagePreView::SetVScrollbarThumbPos( const sal_uInt16 _nNewThumbPos )
{
    if ( pVScrollbar )
    {
        pVScrollbar->SetThumbPos( _nNewThumbPos );
    }
}

/* vim: set noet sw=4 ts=4: */
