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
#include "precompiled_sd.hxx"

#include "OutlineViewShell.hxx"

#include "ViewShellImplementation.hxx"
#include <memory>
#include "helpids.h"
#include "app.hrc"
#include <svx/hyprlink.hxx>
#include <svx/hyperdlg.hxx>
#include <svx/zoomslideritem.hxx>

#include <sfx2/objface.hxx>
#include <sot/exchange.hxx>
#include <svx/ruler.hxx>
#include <svx/zoomitem.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/flditem.hxx>
#include <sfx2/shell.hxx>
#include <sfx2/templdlg.hxx>
#include <sfx2/viewfac.hxx>
#include <sfx2/request.hxx>
#include <svx/hlnkitem.hxx>
#include <svx/svdotext.hxx>
#include <sfx2/dispatch.hxx>
#include <vcl/scrbar.hxx>
#include <svl/whiter.hxx>
#include <editeng/editstat.hxx>
#include <svl/itempool.hxx>
#include <sfx2/tplpitem.hxx>
#include <sfx2/sidebar/SidebarChildWindow.hxx>
#include <sfx2/sidebar/EnumContext.hxx>
#include <svx/svdorect.hxx>
#include <sot/formats.hxx>
#include <com/sun/star/linguistic2/XThesaurus.hpp>
#include <com/sun/star/i18n/TransliterationModules.hpp>
#include <com/sun/star/i18n/TransliterationModulesExtra.hpp>
#include <editeng/unolingu.hxx>
#include <comphelper/processfactory.hxx>
#include <editeng/outlobj.hxx>
#include <svl/cjkoptions.hxx>
#include <svtools/cliplistener.hxx>
#include <svl/srchitem.hxx>
#include <editeng/editobj.hxx>
#include "fubullet.hxx"
#include "optsitem.hxx"

#include "strings.hrc"
#include "glob.hrc"
#include "res_bmp.hrc"
#include "Outliner.hxx"
#include "Window.hxx"
#include "TextObjectBar.hxx"
#include "drawdoc.hxx"
#include "sdresid.hxx"
#include "sdpage.hxx"
#include "fuoltext.hxx"
#include "FrameView.hxx"
#include "zoomlist.hxx"
#include "stlsheet.hxx"
#include "slideshow.hxx"
#include "SdUnoOutlineView.hxx"
#include "SpellDialogChildWindow.hxx"

#include "AccessibleOutlineView.hxx"
#include "ViewShellBase.hxx"
#include "ViewShellManager.hxx"
#include "DrawController.hxx"
#include "framework/FrameworkHelper.hxx"

using ::rtl::OUString;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::linguistic2;

using namespace sd;
#define OutlineViewShell
#include "sdslots.hxx"

namespace sd {

#define MIN_ZOOM	       10       // Minimaler Zoomfaktor
#define MAX_ZOOM	     1000	    // Maximaler Zoomfaktor

/************************************************************************/


/*************************************************************************
|*
|* SFX-Slotmap und Standardinterface deklarieren
|*
\************************************************************************/


SFX_IMPL_INTERFACE(OutlineViewShell, SfxShell, SdResId(STR_OUTLINEVIEWSHELL))
{
	SFX_POPUPMENU_REGISTRATION( SdResId(RID_OUTLINE_POPUP) );
	SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_TOOLS | SFX_VISIBILITY_STANDARD |
								SFX_VISIBILITY_FULLSCREEN | SFX_VISIBILITY_SERVER,
								SdResId(RID_OUTLINE_TOOLBOX) );
    SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_APPLICATION | SFX_VISIBILITY_DESKTOP | SFX_VISIBILITY_STANDARD | SFX_VISIBILITY_CLIENT | SFX_VISIBILITY_VIEWER | SFX_VISIBILITY_READONLYDOC,
            					SdResId(RID_DRAW_VIEWER_TOOLBOX) );
	SFX_CHILDWINDOW_REGISTRATION( SfxTemplateDialogWrapper::GetChildWindowId() );
	SFX_CHILDWINDOW_REGISTRATION( SvxHyperlinkDlgWrapper::GetChildWindowId() );
	SFX_CHILDWINDOW_REGISTRATION( SvxHlinkDlgWrapper::GetChildWindowId() );
	SFX_CHILDWINDOW_REGISTRATION( ::sd::SpellDialogChildWindow::GetChildWindowId() );
	SFX_CHILDWINDOW_REGISTRATION( SID_SEARCH_DLG );
	SFX_CHILDWINDOW_REGISTRATION(::sfx2::sidebar::SidebarChildWindow::GetChildWindowId());
}


TYPEINIT1( OutlineViewShell, ViewShell );


/*************************************************************************
|*
|* gemeinsamer Initialiserungsanteil der beiden Konstruktoren
|*
\************************************************************************/

void OutlineViewShell::Construct(DrawDocShell* )
{
	sal_Bool bModified = GetDoc()->IsChanged();

    meShellType = ST_OUTLINE;
	Size aSize(29700, 21000);
	Point aWinPos (0, 0);
	Point aViewOrigin(0, 0);
	GetActiveWindow()->SetMinZoomAutoCalc(sal_False);
	GetActiveWindow()->SetMinZoom( MIN_ZOOM );
	GetActiveWindow()->SetMaxZoom( MAX_ZOOM );
	InitWindows(aViewOrigin, aSize, aWinPos);
	pOlView = new OutlineView(GetDocSh(), GetActiveWindow(), this);
	mpView = pOlView;			 // Pointer der Basisklasse ViewShell

	SetPool( &GetDoc()->GetPool() );

	SetZoom(69);

	// Einstellungen der FrameView uebernehmen
	ReadFrameViewData(mpFrameView);

	::Outliner* pOutl = pOlView->GetOutliner();
	pOutl->SetUpdateMode(sal_True);

	if (!bModified)
	{
		pOutl->ClearModifyFlag();
	}

	pLastPage = GetActualPage();

	String aName( RTL_CONSTASCII_USTRINGPARAM( "OutlineViewShell" ));
	SetName (aName);

	SetHelpId( SD_IF_SDOUTLINEVIEWSHELL );
	GetActiveWindow()->SetHelpId( HID_SDOUTLINEVIEWSHELL );
	GetActiveWindow()->SetUniqueId( HID_SDOUTLINEVIEWSHELL );
}




Reference<drawing::XDrawSubController> OutlineViewShell::CreateSubController (void)
{
    Reference<drawing::XDrawSubController> xSubController;

    if (IsMainViewShell())
    {
        // Create uno sub controller for the main view shell.
        xSubController = Reference<drawing::XDrawSubController>(
            new SdUnoOutlineView (
                GetViewShellBase().GetDrawController(),
                *this,
                *GetView()));
    }

    return xSubController;
}




/*************************************************************************
|*
|* Standard-Konstruktor, Fenster duerfen nicht automatisch zentrieren
|*
\************************************************************************/

OutlineViewShell::OutlineViewShell (
    SfxViewFrame* pFrame,
    ViewShellBase& rViewShellBase,
    ::Window* pParentWindow,
    FrameView* pFrameViewArgument)
    : ViewShell(pFrame, pParentWindow, rViewShellBase),
      pOlView(NULL),
      pLastPage( NULL ),
      pClipEvtLstnr(NULL),
      bPastePossible(false),
      mbInitialized(false)

{
	if (pFrameViewArgument != NULL)
		mpFrameView = pFrameViewArgument;
	else
        mpFrameView = new FrameView(GetDoc());

	mpFrameView->Connect();

	Construct(GetDocSh());

    SetContextName(sfx2::sidebar::EnumContext::GetContextName(sfx2::sidebar::EnumContext::Context_OutlineText));
	m_StrOldPageName.EmptyString();
}

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

OutlineViewShell::~OutlineViewShell()
{
	DisposeFunctions();

	delete pOlView;

	mpFrameView->Disconnect();

	if ( pClipEvtLstnr )
	{
		pClipEvtLstnr->AddRemoveListener( GetActiveWindow(), sal_False );
		pClipEvtLstnr->ClearCallbackLink();		// #103849# prevent callback if another thread is waiting
		pClipEvtLstnr->release();
	}
}




void OutlineViewShell::Shutdown (void)
{
    ViewShell::Shutdown();

    PrepareClose();
}




/*************************************************************************
|*
|* Paint-Methode: das Ereignis wird vom Fenster pWindow an
|* die Viewshell und die aktuelle Funktion weitergeleitet
|*
\************************************************************************/

void OutlineViewShell::Paint(const Rectangle& rRect, ::sd::Window* pWin)
{
	if (pOlView)
	{
		pOlView->Paint(rRect, pWin);
	}

	if(HasCurrentFunction())
	{
		GetCurrentFunction()->Paint(rRect, pWin);
	}
}

void OutlineViewShell::ArrangeGUIElements ()
{
    // Retrieve the current size (thickness) of the scroll bars.  That is
    // the width of the vertical and the height of the horizontal scroll
    // bar.
    int nScrollBarSize =
        GetParentWindow()->GetSettings().GetStyleSettings().GetScrollBarSize();
	maScrBarWH = Size (nScrollBarSize, nScrollBarSize);

	ViewShell::ArrangeGUIElements ();

    ::sd::Window* pWindow = mpContentWindow.get();
    if (pWindow != NULL)
    {
        pWindow->SetMinZoomAutoCalc(sal_False);

        // pWindow->SetPosSizePixel(rNewPos, Size(nSizeX, nSizeY));

        // OutputArea der OutlinerView aendern
        OutlinerView* pOutlinerView = pOlView->GetViewByWindow(pWindow);

        Rectangle aWin(Point(0,0), pWindow->GetOutputSizePixel());
//		aWin.nLeft = pOlView->GetPageNumberWidthPixel();

        aWin = pWindow->PixelToLogic(aWin);
        pOutlinerView->SetOutputArea(aWin);

        Rectangle aVis = pOutlinerView->GetVisArea();

        Rectangle aText = Rectangle(Point(0,0),
            Size(pOlView->GetPaperWidth(),
                pOlView->GetOutliner()->GetTextHeight()));
        aText.Bottom() += aWin.GetHeight();

        if (!aWin.IsEmpty())			// nicht beim Oeffnen
        {
            InitWindows(Point(0,0), aText.GetSize(), Point(aVis.TopLeft()));
            UpdateScrollBars();
        }
    }
}

/*************************************************************************
|*
|* SfxRequests fuer Controller bearbeiten
|*
\************************************************************************/

void OutlineViewShell::ExecCtrl(SfxRequest &rReq)
{
	sal_uInt16 nSlot = rReq.GetSlot();
	switch ( nSlot )
	{
		case SID_MAIL_SCROLLBODY_PAGEDOWN:
		{
            ExecReq( rReq );
			break;
		}

		case SID_OPT_LOCALE_CHANGED:
		{
			pOlView->GetOutliner()->UpdateFields();
			UpdatePreview( GetActualPage() );
			rReq.Done();
            break;
		}

		default:
		break;
	}
}




void OutlineViewShell::AddWindow (::sd::Window* pWin)
{
    pOlView->AddWindowToPaintView(pWin);
}




void OutlineViewShell::RemoveWindow (::sd::Window* pWin)
{
    pOlView->DeleteWindowFromPaintView(pWin);
}




/*************************************************************************
|*
|* Activate(), beim ersten Aufruf erfolgt ein Update der Felder
|*
\************************************************************************/
void OutlineViewShell::Activate( sal_Bool bIsMDIActivate )
{
    if ( ! mbInitialized)
    {
        mbInitialized = true;
    	SfxRequest aRequest (SID_EDIT_OUTLINER, 0, GetDoc()->GetItemPool());
        FuPermanent (aRequest);
    }

    ViewShell::Activate( bIsMDIActivate );
    SfxShell::BroadcastContextForActivation(true);
    
    pOlView->SetLinks();
    pOlView->ConnectToApplication();

    if( bIsMDIActivate )
    {
        OutlinerView* pOutlinerView = pOlView->GetViewByWindow( GetActiveWindow() );
        ::Outliner* pOutl = pOutlinerView->GetOutliner();
        pOutl->UpdateFields();
    }
}

/*************************************************************************
|*
|* Deactivate()
|*
\************************************************************************/
void OutlineViewShell::Deactivate( sal_Bool bIsMDIActivate )
{
    pOlView->DisconnectFromApplication();

    // #96416# Links must be kept also on deactivated viewshell, to allow drag'n'drop
    // to function properly
    // pOlView->ResetLinks();

    ViewShell::Deactivate( bIsMDIActivate );
}

/*************************************************************************
|*
|* Status von Controller-SfxSlots setzen
|*
\************************************************************************/
void OutlineViewShell::GetCtrlState(SfxItemSet &rSet)
{
	if (SFX_ITEM_AVAILABLE == rSet.GetItemState(SID_HYPERLINK_GETLINK))
	{
		SvxHyperlinkItem aHLinkItem;

		OutlinerView* pOLV = pOlView->GetViewByWindow(GetActiveWindow());
		if (pOLV)
		{
			const SvxFieldItem* pFieldItem = pOLV->GetFieldAtSelection();
			if (pFieldItem)
			{
                ESelection aSel = pOLV->GetSelection();
                if ( abs( aSel.nEndPos - aSel.nStartPos ) == 1 )
                {
				    const SvxFieldData* pField = pFieldItem->GetField();
				    if ( pField->ISA(SvxURLField) )
				    {
					    aHLinkItem.SetName(((const SvxURLField*) pField)->GetRepresentation());
					    aHLinkItem.SetURL(((const SvxURLField*) pField)->GetURL());
					    aHLinkItem.SetTargetFrame(((const SvxURLField*) pField)->GetTargetFrame());
				    }
                }
			}
		}
		rSet.Put(aHLinkItem);
	}
    rSet.Put( SfxBoolItem( SID_READONLY_MODE, GetDocSh()->IsReadOnly() ) );

	if ( SFX_ITEM_AVAILABLE == rSet.GetItemState(SID_MAIL_SCROLLBODY_PAGEDOWN) )
		rSet.Put( SfxBoolItem( SID_MAIL_SCROLLBODY_PAGEDOWN, sal_True ) );

	if ( SFX_ITEM_AVAILABLE == rSet.GetItemState(SID_TRANSLITERATE_HALFWIDTH) ||
		 SFX_ITEM_AVAILABLE == rSet.GetItemState(SID_TRANSLITERATE_FULLWIDTH) ||
		 SFX_ITEM_AVAILABLE == rSet.GetItemState(SID_TRANSLITERATE_HIRAGANA) ||
		 SFX_ITEM_AVAILABLE == rSet.GetItemState(SID_TRANSLITERATE_KATAGANA) )
	{
        SvtCJKOptions aCJKOptions;
		if( !aCJKOptions.IsChangeCaseMapEnabled() )
		{
            GetViewFrame()->GetBindings().SetVisibleState( SID_TRANSLITERATE_HALFWIDTH, sal_False );
            GetViewFrame()->GetBindings().SetVisibleState( SID_TRANSLITERATE_FULLWIDTH, sal_False );
            GetViewFrame()->GetBindings().SetVisibleState( SID_TRANSLITERATE_HIRAGANA, sal_False );
            GetViewFrame()->GetBindings().SetVisibleState( SID_TRANSLITERATE_KATAGANA, sal_False );
			rSet.DisableItem( SID_TRANSLITERATE_HALFWIDTH );
			rSet.DisableItem( SID_TRANSLITERATE_FULLWIDTH );
			rSet.DisableItem( SID_TRANSLITERATE_HIRAGANA );
			rSet.DisableItem( SID_TRANSLITERATE_KATAGANA );
		}
        else
        {
            GetViewFrame()->GetBindings().SetVisibleState( SID_TRANSLITERATE_HALFWIDTH, sal_True );
            GetViewFrame()->GetBindings().SetVisibleState( SID_TRANSLITERATE_FULLWIDTH, sal_True );
            GetViewFrame()->GetBindings().SetVisibleState( SID_TRANSLITERATE_HIRAGANA, sal_True );
            GetViewFrame()->GetBindings().SetVisibleState( SID_TRANSLITERATE_KATAGANA, sal_True );
        }
	}
}

/*************************************************************************
|*
|* SfxRequests fuer Support-Funktionen
|*
\************************************************************************/

void OutlineViewShell::FuSupport(SfxRequest &rReq)
{
	if( rReq.GetSlot() == SID_STYLE_FAMILY && rReq.GetArgs())
		GetDocSh()->SetStyleFamily(((SfxUInt16Item&)rReq.GetArgs()->Get( SID_STYLE_FAMILY )).GetValue());

	sal_Bool bPreviewState = sal_False;
	sal_uLong nSlot = rReq.GetSlot();

	std::auto_ptr< OutlineViewModelChangeGuard > aGuard;
	if( pOlView && (
        (nSlot == SID_TRANSLITERATE_SENTENCE_CASE) ||
        (nSlot == SID_TRANSLITERATE_TITLE_CASE) ||
        (nSlot == SID_TRANSLITERATE_TOGGLE_CASE) ||
		(nSlot == SID_TRANSLITERATE_UPPER) ||
		(nSlot == SID_TRANSLITERATE_LOWER) ||
		(nSlot == SID_TRANSLITERATE_HALFWIDTH) ||
		(nSlot == SID_TRANSLITERATE_FULLWIDTH) ||
		(nSlot == SID_TRANSLITERATE_HIRAGANA) ||
		(nSlot == SID_TRANSLITERATE_KATAGANA) ||
		(nSlot == SID_CUT) ||
//		(nSlot == SID_COPY) ||
		(nSlot == SID_PASTE) ||
		(nSlot == SID_DELETE)))
	{
		aGuard.reset( new OutlineViewModelChangeGuard( *pOlView ) );
	}

	switch ( nSlot )
	{
		case SID_CUT:
		{
			if(HasCurrentFunction())
			{
				GetCurrentFunction()->DoCut();
			}
			else if (pOlView)
			{
				pOlView->DoCut();
			}
			rReq.Done();
			bPreviewState = sal_True;
		}
		break;

		case SID_COPY:
		{
			if(HasCurrentFunction())
			{
				GetCurrentFunction()->DoCopy();
			}
			else if (pOlView)
			{
				pOlView->DoCopy();
			}
			rReq.Done();
			bPreviewState = sal_True;
		}
		break;

		case SID_PASTE:
		{
            OutlineViewPageChangesGuard aGuard2(pOlView);

			if(HasCurrentFunction())
			{
				GetCurrentFunction()->DoPaste();
			}
			else if (pOlView)
			{
				pOlView->DoPaste();
			}
			rReq.Done();
			bPreviewState = sal_True;
		}
		break;

		case SID_DELETE:
		{
			if( pOlView )
			{
				OutlinerView* pOutlView = pOlView->GetViewByWindow(GetActiveWindow());
				if (pOutlView)
				{
					OutlineViewPageChangesGuard aGuard2(pOlView);

					KeyCode  aKCode(KEY_DELETE);
					KeyEvent aKEvt( 0, aKCode );
					pOutlView->PostKeyEvent(aKEvt);

					FunctionReference xFunc( GetCurrentFunction() );
					FuOutlineText* pFuOutlineText = dynamic_cast< FuOutlineText* >( xFunc.get() );
                    if( pFuOutlineText )
                        pFuOutlineText->UpdateForKeyPress (aKEvt);
				}
			}
			rReq.Done();
			bPreviewState = sal_True;
		}
		break;

		case SID_DRAWINGMODE:
		case SID_NOTESMODE:
		case SID_HANDOUTMODE:
		case SID_DIAMODE:
		case SID_OUTLINEMODE:
            framework::FrameworkHelper::Instance(GetViewShellBase())->HandleModeChangeSlot(
                nSlot,
                rReq);
			rReq.Done();
			break;

		case SID_RULER:
			SetRuler( !HasRuler() );
			Invalidate( SID_RULER );
			rReq.Done();
		break;

		case SID_ZOOM_PREV:
		{
			if (mpZoomList->IsPreviousPossible())
			{
				// Vorheriges ZoomRect einstellen
				SetZoomRect(mpZoomList->GetPreviousZoomRect());
			}
			rReq.Done ();
		}
		break;

		case SID_ZOOM_NEXT:
		{
			if (mpZoomList->IsNextPossible())
			{
				// Naechstes ZoomRect einstellen
				SetZoomRect(mpZoomList->GetNextZoomRect());
			}
			rReq.Done ();
		}
		break;

		case SID_AUTOSPELL_CHECK:
		{
			GetDoc()->SetOnlineSpell(!GetDoc()->GetOnlineSpell());
			rReq.Done ();
		}
		break;

        case SID_TRANSLITERATE_SENTENCE_CASE:
        case SID_TRANSLITERATE_TITLE_CASE:
        case SID_TRANSLITERATE_TOGGLE_CASE:
		case SID_TRANSLITERATE_UPPER:
		case SID_TRANSLITERATE_LOWER:
		case SID_TRANSLITERATE_HALFWIDTH:
		case SID_TRANSLITERATE_FULLWIDTH:
		case SID_TRANSLITERATE_HIRAGANA:
		case SID_TRANSLITERATE_KATAGANA:
		{
			OutlinerView* pOLV = pOlView->GetViewByWindow( GetActiveWindow() );
			if( pOLV )
			{
				using namespace ::com::sun::star::i18n;
				sal_Int32 nType = 0;

				switch( nSlot )
				{
                    case SID_TRANSLITERATE_SENTENCE_CASE:
                        nType = TransliterationModulesExtra::SENTENCE_CASE;
                        break;
                    case SID_TRANSLITERATE_TITLE_CASE:
                        nType = TransliterationModulesExtra::TITLE_CASE;
                        break;
                    case SID_TRANSLITERATE_TOGGLE_CASE:
                        nType = TransliterationModulesExtra::TOGGLE_CASE;
                        break;
                    case SID_TRANSLITERATE_UPPER:
						nType = TransliterationModules_LOWERCASE_UPPERCASE;
						break;
					case SID_TRANSLITERATE_LOWER:
						nType = TransliterationModules_UPPERCASE_LOWERCASE;
						break;
					case SID_TRANSLITERATE_HALFWIDTH:
						nType = TransliterationModules_FULLWIDTH_HALFWIDTH;
						break;
					case SID_TRANSLITERATE_FULLWIDTH:
						nType = TransliterationModules_HALFWIDTH_FULLWIDTH;
						break;
					case SID_TRANSLITERATE_HIRAGANA:
						nType = TransliterationModules_KATAKANA_HIRAGANA;
						break;
					case SID_TRANSLITERATE_KATAGANA:
						nType = TransliterationModules_HIRAGANA_KATAKANA;
						break;
				}

				pOLV->TransliterateText( nType );
			}

			rReq.Done();
			bPreviewState = sal_True;
		}
		break;

		// #96090# added Undo/Redo handling
		case SID_UNDO :
		{
			OutlineViewPageChangesGuard aGuard2(pOlView);
			ImpSidUndo(sal_False, rReq);
		}
		break;
		case SID_REDO :
		{
			OutlineViewPageChangesGuard aGuard2(pOlView);
			ImpSidRedo(sal_False, rReq);
		}
		break;

		default:
		break;
	}

	if( bPreviewState )
		Invalidate( SID_PREVIEW_STATE );

	Invalidate(SID_CUT);
	Invalidate(SID_COPY);
	Invalidate(SID_PASTE);
}

/*************************************************************************
|*
|* SfxRequests fuer permanente Funktionen
|*
\************************************************************************/

void OutlineViewShell::FuPermanent(SfxRequest &rReq)
{
	if(HasCurrentFunction())
	{
		DeactivateCurrentFunction(true);
	}

	switch ( rReq.GetSlot() )
	{
		case SID_EDIT_OUTLINER:
		{
			::Outliner* pOutl = pOlView->GetOutliner();
			if( pOutl )
			{
				pOutl->GetUndoManager().Clear();
				pOutl->UpdateFields();
			}

			SetCurrentFunction( FuOutlineText::Create(this,GetActiveWindow(),pOlView,GetDoc(),rReq) );

			rReq.Done();
		}
		break;

	  default:
	  break;
	}

	if(HasOldFunction())
	{
		GetOldFunction()->Deactivate();
		SetOldFunction(0);
	}

	if(HasCurrentFunction())
	{
		GetCurrentFunction()->Activate();
		SetOldFunction(GetCurrentFunction());
	}
}


IMPL_LINK( OutlineViewShell, ClipboardChanged, TransferableDataHelper*, pDataHelper )
{
	if ( pDataHelper )
	{
		bPastePossible = ( pDataHelper->GetFormatCount() != 0 &&
							( pDataHelper->HasFormat( FORMAT_STRING ) ||
							  pDataHelper->HasFormat( FORMAT_RTF ) ||
							  pDataHelper->HasFormat( SOT_FORMATSTR_ID_HTML ) ) );

		SfxBindings& rBindings = GetViewFrame()->GetBindings();
		rBindings.Invalidate( SID_PASTE );
        rBindings.Invalidate( SID_PASTE_SPECIAL );
		rBindings.Invalidate( SID_CLIPBOARD_FORMAT_ITEMS );
	}
	return 0;
}

/*************************************************************************
|*
|* Status (Enabled/Disabled) von Menue-SfxSlots setzen
|*
\************************************************************************/

void OutlineViewShell::GetMenuState( SfxItemSet &rSet )
{
	ViewShell::GetMenuState(rSet);

	// Vorlagenkatalog darf nicht aufgerufen werden
	rSet.DisableItem( SID_STYLE_CATALOG );

	rSet.Put(SfxBoolItem(SID_DIAMODE, sal_False));
	rSet.Put(SfxBoolItem(SID_DRAWINGMODE, sal_False));
	rSet.Put(SfxBoolItem(SID_OUTLINEMODE, sal_True));
	rSet.Put(SfxBoolItem(SID_NOTESMODE, sal_False));
	rSet.Put(SfxBoolItem(SID_HANDOUTMODE, sal_False));

	if (!mpZoomList->IsNextPossible())
	{
	   rSet.DisableItem(SID_ZOOM_NEXT);
	}
	if (!mpZoomList->IsPreviousPossible())
	{
	   rSet.DisableItem(SID_ZOOM_PREV);
	}

	if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_ZOOM_IN ) ||
		SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_ZOOM_OUT ) )
	{
		if( GetActiveWindow()->GetZoom() <= GetActiveWindow()->GetMinZoom() || GetDocSh()->IsUIActive() )
			rSet.DisableItem( SID_ZOOM_IN );
		if( GetActiveWindow()->GetZoom() >= GetActiveWindow()->GetMaxZoom() || GetDocSh()->IsUIActive() )
			rSet.DisableItem( SID_ZOOM_OUT );
	}

	::Outliner* pOutl = pOlView->GetOutliner();
	DBG_ASSERT(pOutl, "OutlineViewShell::GetMenuState(), no outliner? Fatality!");
	if( !pOutl )
		return;

	// 'Alles auswaehlen' zulassen?
	if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_SELECTALL ) )
	{
		sal_uLong nParaCount = pOutl->GetParagraphCount();
		sal_Bool bDisable = nParaCount == 0;
		if (!bDisable && nParaCount == 1)
		{
			String aTest( pOutl->GetText( pOutl->GetParagraph( 0 ) ) );
			if (aTest.Len() == 0)
			{
				bDisable = sal_True;
			}
		}
		if (bDisable)
			rSet.DisableItem(SID_SELECTALL);
	}

	// Status des Lineals setzen
	rSet.Put( SfxBoolItem( SID_RULER, HasRuler() ) );

	// Formatierung ein oder aus?
	rSet.Put( SfxBoolItem( SID_OUTLINE_FORMAT, !pOutl->IsFlatMode() ) );

	if( pOutl->IsFlatMode() )
		rSet.DisableItem( SID_COLORVIEW );
	else
	{
		// Farbansicht ein/aus
		sal_uLong nCntrl = pOutl->GetControlWord();
		sal_Bool bNoColor = sal_False;
		if (nCntrl & EE_CNTRL_NOCOLORS)
			bNoColor = sal_True;

		rSet.Put( SfxBoolItem( SID_COLORVIEW, bNoColor ) );
	}

	// Buttons der Werkzeugleiste
	// zunaechst selektionsabhaengige: COLLAPSE, EXPAND
	sal_Bool bDisableCollapse = sal_True;
	sal_Bool bDisableExpand   = sal_True;
	sal_Bool bUnique		  = sal_True;
	OutlinerView* pOutlinerView = pOlView->GetViewByWindow(GetActiveWindow());
	List* pList = pOutlinerView->CreateSelectionList();
	Paragraph* pPara = (Paragraph*)pList->First();

	sal_Int16 nDepth;
	sal_Int16 nTmpDepth = pOutl->GetDepth( (sal_uInt16) pOutl->GetAbsPos( pPara ) );
	bool bPage = pOutl->HasParaFlag( pPara, PARAFLAG_ISPAGE );
	while (pPara)
	{
		nDepth = pOutl->GetDepth( (sal_uInt16) pOutl->GetAbsPos( pPara ) );

		if( nDepth != nTmpDepth )
			bUnique = sal_False;
		if( bPage != pOutl->HasParaFlag( pPara, PARAFLAG_ISPAGE ) )
			bUnique = sal_False;
		if (!pOutl->IsExpanded(pPara) && pOutl->HasChilds(pPara))
			bDisableExpand = sal_False;
		if (pOutl->IsExpanded(pPara) && pOutl->HasChilds(pPara))
			bDisableCollapse = sal_False;

		pPara = (Paragraph*)pList->Next();
	}

	delete pList;

	if (bDisableExpand)
		rSet.DisableItem(SID_OUTLINE_EXPAND);
	if (bDisableCollapse)
		rSet.DisableItem(SID_OUTLINE_COLLAPSE);

	// ergibt die Selektion ein eindeutiges Praesentationslayout?
	// wenn nicht, duerfen die Vorlagen nicht bearbeitet werden
	SfxItemSet aSet(*rSet.GetPool(), SID_STATUS_LAYOUT, SID_STATUS_LAYOUT);
	GetStatusBarState(aSet);
	String aTest(((SfxStringItem&)aSet.Get(SID_STATUS_LAYOUT)).GetValue());
	if (aTest.Len() == 0)
	{
		bUnique = sal_False;
		rSet.DisableItem(SID_PRESENTATION_TEMPLATES);
	}

	if (!bUnique)
		rSet.DisableItem( SID_PRESENTATIONOBJECT );

	// jetzt die selektionsunabhaengigen: COLLAPSE_ALL, EXPAND_ALL
	sal_Bool bDisableCollapseAll = sal_True;
	sal_Bool bDisableExpandAll	 = sal_True;

	// wenn schon die Selektion etwas kollabierbares/expandierbares enthaelt
	if (!bDisableCollapse)
		bDisableCollapseAll = sal_False;
	if (!bDisableExpand)
		bDisableExpandAll = sal_False;

	// schade, so billig kommen wir nicht davon; alle Absaetze durchsuchen
	if (bDisableCollapseAll || bDisableExpandAll)
	{
		sal_uLong nParaPos = 0;
		pPara = pOutl->GetParagraph( nParaPos );
		while (pPara && (bDisableCollapseAll || bDisableExpandAll))
		{
			if (!pOutl->IsExpanded(pPara) && pOutl->HasChilds(pPara))
				bDisableExpandAll = sal_False;

			if (pOutl->IsExpanded(pPara) && pOutl->HasChilds(pPara))
				bDisableCollapseAll = sal_False;

			pPara = pOutl->GetParagraph( ++nParaPos );
		}
	}

	if (bDisableExpandAll)
		rSet.DisableItem(SID_OUTLINE_EXPAND_ALL);
	if (bDisableCollapseAll)
		rSet.DisableItem(SID_OUTLINE_COLLAPSE_ALL);

	if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_PASTE ) )
	{
		if ( !pClipEvtLstnr )
		{
			// create listener
			pClipEvtLstnr = new TransferableClipboardListener( LINK( this, OutlineViewShell, ClipboardChanged ) );
			pClipEvtLstnr->acquire();
			pClipEvtLstnr->AddRemoveListener( GetActiveWindow(), sal_True );

			// get initial state
			TransferableDataHelper aDataHelper( TransferableDataHelper::CreateFromSystemClipboard( GetActiveWindow() ) );
			bPastePossible = ( aDataHelper.GetFormatCount() != 0 &&
								( aDataHelper.HasFormat( FORMAT_STRING ) ||
								  aDataHelper.HasFormat( FORMAT_RTF ) ||
								  aDataHelper.HasFormat( SOT_FORMATSTR_ID_HTML ) ) );
		}

		if( !bPastePossible )
		{
			rSet.DisableItem( SID_PASTE );
		}
	}

	if (!pOlView->GetViewByWindow(GetActiveWindow())->HasSelection())
	{
		rSet.DisableItem(SID_CUT);
		rSet.DisableItem(SID_COPY);
	}

	if (pOlView->GetOutliner()->IsModified())
	{
		GetDoc()->SetChanged(sal_True);
	}

	// Da �berladen, muss hier der Status gesetzt werden
	if( !GetDocSh()->IsModified() )
	{
		rSet.DisableItem( SID_SAVEDOC );
	}

	if ( GetDocSh()->IsReadOnly() )
	{
		rSet.DisableItem( SID_AUTOSPELL_CHECK );
	}
	else
	{
		if (GetDoc()->GetOnlineSpell())
		{
			rSet.Put(SfxBoolItem(SID_AUTOSPELL_CHECK, sal_True));
		}
		else
		{
			rSet.Put(SfxBoolItem(SID_AUTOSPELL_CHECK, sal_False));
		}
	}

	// Feldbefehle
	if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_MODIFY_FIELD ) )
	{
		const SvxFieldItem* pFldItem = pOutlinerView->GetFieldAtSelection();

		if( !( pFldItem && (pFldItem->GetField()->ISA( SvxDateField ) ||
							pFldItem->GetField()->ISA( SvxAuthorField ) ||
							pFldItem->GetField()->ISA( SvxExtFileField ) ||
							pFldItem->GetField()->ISA( SvxExtTimeField ) ) ) )
		{
			rSet.DisableItem( SID_MODIFY_FIELD );
		}
	}

	if (SFX_ITEM_AVAILABLE == rSet.GetItemState(SID_EXPAND_PAGE))
	{
		sal_Bool bDisable = sal_True;
		sal_uInt16 i = 0;
		sal_uInt16 nCount = GetDoc()->GetSdPageCount(PK_STANDARD);
		pOlView->SetSelectedPages();

		while (i < nCount && bDisable)
		{
			SdPage* pPage = GetDoc()->GetSdPage(i, PK_STANDARD);

			if (pPage->IsSelected())
			{
				SdrObject* pObj = pPage->GetPresObj(PRESOBJ_OUTLINE);

                if (pObj!=NULL )
				{
					if( !pObj->IsEmptyPresObj() )
					{
						bDisable = false;
					}
					else
					{
						// check if the object is in edit, than its temporarely not empty
						SdrTextObj* pTextObj = dynamic_cast< SdrTextObj* >( pObj );
						if( pTextObj )
						{
							OutlinerParaObject* pParaObj = pTextObj->GetEditOutlinerParaObject();
							if( pParaObj )
							{
								delete pParaObj;
								bDisable = false;
							}
						}
					}
				}
			}

			i++;
		}

		if (bDisable)
		{
			rSet.DisableItem(SID_EXPAND_PAGE);
		}
	}

	if (SFX_ITEM_AVAILABLE == rSet.GetItemState(SID_SUMMARY_PAGE))
	{
		sal_Bool bDisable = sal_True;
		sal_uInt16 i = 0;
		sal_uInt16 nCount = GetDoc()->GetSdPageCount(PK_STANDARD);
		pOlView->SetSelectedPages();

		while (i < nCount && bDisable)
		{
			SdPage* pPage = GetDoc()->GetSdPage(i, PK_STANDARD);

			if (pPage->IsSelected())
			{
				SdrObject* pObj = pPage->GetPresObj(PRESOBJ_TITLE);

				if (pObj && !pObj->IsEmptyPresObj())
				{
					bDisable = sal_False;
				}
			}

			i++;
		}

		if (bDisable)
		{
			rSet.DisableItem(SID_SUMMARY_PAGE);
		}
	}

    if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_THESAURUS ) )
	{
		if ( !pOlView->IsTextEdit() )
		{
			rSet.DisableItem( SID_THESAURUS );
		}
		else
		{
			LanguageType            eLang = GetDoc()->GetLanguage( EE_CHAR_LANGUAGE );
	        Reference< XThesaurus > xThesaurus( LinguMgr::GetThesaurus() );
			Locale                  aLocale;

			SvxLanguageToLocale( aLocale, eLang );

            if (!xThesaurus.is() || eLang == LANGUAGE_NONE || !xThesaurus->hasLocale(aLocale))
				rSet.DisableItem( SID_THESAURUS );
		}
	}

	// Starten der Praesentation moeglich?
	if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_PRESENTATION ) )
	{
		sal_Bool bDisable = sal_True;
		sal_uInt16 nCount = GetDoc()->GetSdPageCount( PK_STANDARD );

		for( sal_uInt16 i = 0; i < nCount && bDisable; i++ )
		{
			SdPage* pPage = GetDoc()->GetSdPage(i, PK_STANDARD);

			if( !pPage->IsExcluded() )
				bDisable = sal_False;
		}
		if( bDisable || GetDocSh()->IsPreview())
		{
			rSet.DisableItem( SID_PRESENTATION );
		}
	}

	FuBullet::GetSlotState( rSet, this, GetViewFrame() );

	//rSet.DisableItem( SID_PRINTDOC );
	//rSet.DisableItem( SID_PRINTDOCDIRECT );
	//rSet.DisableItem( SID_SETUPPRINTER );
}

/*************************************************************************
|*
|* wird gerufen, wenn ScrollBar benutzt wird
|*
\************************************************************************/

long OutlineViewShell::VirtHScrollHdl(ScrollBar* pHScroll)
{
	long   nThumb = pHScroll->GetThumbPos();
	long   nRange = pHScroll->GetRange().Len();
	double fX	  = (double) nThumb / nRange;

    Window* 	  pWin			= mpContentWindow.get();
    OutlinerView* pOutlinerView = pOlView->GetViewByWindow(pWin);
    long		  nViewWidth	= pWin->PixelToLogic(
        pWin->GetSizePixel()).Width();
    long		  nTextWidth	= pOlView->GetPaperWidth();
    nViewWidth					= Max(nViewWidth, nTextWidth);
    long		  nCurrentPos	= pOutlinerView->GetVisArea().Left();
    long		  nTargetPos	= (long)(fX * nViewWidth);
    long		  nDelta		= nTargetPos - nCurrentPos;

    pOutlinerView->HideCursor();
    pOutlinerView->Scroll(-nDelta, 0);
    pOutlinerView->ShowCursor(sal_False);

    pOlView->InvalidateSlideNumberArea();
	return 0;
}

/*************************************************************************
|*
|* wird gerufen, wenn ScrollBar benutzt wird
|*
\************************************************************************/

long OutlineViewShell::VirtVScrollHdl(ScrollBar* pVScroll)
{
	long nThumb = pVScroll->GetThumbPos();
	long nRange = pVScroll->GetRange().Len();
	double fY = (double) nThumb / nRange;

    Window* 	  pWin			= mpContentWindow.get();
    OutlinerView* pOutlinerView = pOlView->GetViewByWindow(pWin);
    long		  nViewHeight	= pWin->PixelToLogic(
        pWin->GetSizePixel()).Height();
    long		  nTextHeight	= pOlView->GetOutliner()->GetTextHeight();
    nViewHeight 			   += nTextHeight;
    long		  nCurrentPos	= pOutlinerView->GetVisArea().Top();
    long		  nTargetPos	= (long)(fY * nViewHeight);
    long		  nDelta		= nTargetPos - nCurrentPos;

    pOutlinerView->HideCursor();
    pOutlinerView->Scroll(0, -nDelta);
    pOutlinerView->ShowCursor(sal_False);

    pOlView->InvalidateSlideNumberArea();

	return 0;
}

/*************************************************************************
|*
|* PrepareClose, wird gerufen, wenn die Shell zestoert werden soll,
|* leitet den Aufruf an die View weiter
|*
\************************************************************************/

sal_uInt16 OutlineViewShell::PrepareClose( sal_Bool bUI, sal_Bool bForBrowsing )
{
	if( ViewShell::PrepareClose(bUI, bForBrowsing) != sal_True )
		return sal_False;

	return pOlView == NULL || pOlView->PrepareClose(bUI);
}


/*************************************************************************
|*
|* Zoomen mit Zoomfaktor, OutlinerView informieren
|*
\************************************************************************/

void OutlineViewShell::SetZoom(long nZoom)
{
	ViewShell::SetZoom(nZoom);

    ::sd::Window* pWindow = mpContentWindow.get();
    if (pWindow)
    {
        // OutputArea der OutlinerView aendern
        OutlinerView* pOutlinerView = pOlView->GetViewByWindow(pWindow);
        Rectangle aWin(Point(0,0), pWindow->GetOutputSizePixel());
        aWin = pWindow->PixelToLogic(aWin);
        pOutlinerView->SetOutputArea(aWin);
    }

	// #106268#
	GetViewFrame()->GetBindings().Invalidate( SID_ATTR_ZOOM );
	GetViewFrame()->GetBindings().Invalidate( SID_ATTR_ZOOMSLIDER );
}

/*************************************************************************
|*
|* Zoomen mit Zoomrechteck, OutlinerView informieren
|*
\************************************************************************/

void OutlineViewShell::SetZoomRect(const Rectangle& rZoomRect)
{
	ViewShell::SetZoomRect(rZoomRect);

    ::sd::Window* pWindow = mpContentWindow.get();
    if (pWindow)
    {
        // OutputArea der OutlinerView aendern
        OutlinerView* pOutlinerView = pOlView->GetViewByWindow(pWindow);
        Rectangle aWin(Point(0,0), pWindow->GetOutputSizePixel());
        aWin = pWindow->PixelToLogic(aWin);
        pOutlinerView->SetOutputArea(aWin);
    }

	// #106268#
	GetViewFrame()->GetBindings().Invalidate( SID_ATTR_ZOOM );
	GetViewFrame()->GetBindings().Invalidate( SID_ATTR_ZOOMSLIDER );
}

/*************************************************************************
|*
|* Vorm Speichern das Model der Drawing Engine aktualisieren, dann den
|* Call weiterleiten an die ObjectShell.
|*
\************************************************************************/

void OutlineViewShell::Execute(SfxRequest& rReq)
{
    bool bForwardCall = true;

	switch(rReq.GetSlot())
	{
		case SID_SAVEDOC:
		case SID_SAVEASDOC:
			PrepareClose();
            break;

		case SID_SEARCH_ITEM:
            // Forward this request to the common (old) code of the
            // document shell.
            GetDocSh()->Execute (rReq);
            bForwardCall = false;
            break;

        case SID_SPELL_DIALOG:
        {
            SfxViewFrame* pViewFrame = GetViewFrame();
            if (rReq.GetArgs() != NULL)
                pViewFrame->SetChildWindow (SID_SPELL_DIALOG,
                    ((const SfxBoolItem&) (rReq.GetArgs()->
                        Get(SID_SPELL_DIALOG))).GetValue());
            else
                pViewFrame->ToggleChildWindow(SID_SPELL_DIALOG);

            pViewFrame->GetBindings().Invalidate(SID_SPELL_DIALOG);
            rReq.Done ();

            bForwardCall = false;
        }
        break;

		default:
            OSL_TRACE ("OutlineViewShell::Execute(): can not handle slot %d", rReq.GetSlot());
            break;

	}

    if (bForwardCall)
        ((DrawDocShell*)GetViewFrame()->GetObjectShell())->ExecuteSlot( rReq );
}

/*************************************************************************
|*
|* Read FrameViews data and set actual views data
|*
\************************************************************************/

void OutlineViewShell::ReadFrameViewData(FrameView* pView)
{
	::Outliner* pOutl = pOlView->GetOutliner();

	if ( pView->IsNoAttribs() )
		pOutl->SetFlatMode( sal_True );   // Attribut-Darstellung ausschalten
	else
		pOutl->SetFlatMode( sal_False );  // Attribut-Darstellung einschalten

	sal_uLong nCntrl = pOutl->GetControlWord();

	if ( pView->IsNoColors() )
		pOutl->SetControlWord(nCntrl | EE_CNTRL_NOCOLORS);   // Farbansicht ausschalten
	else
		pOutl->SetControlWord(nCntrl & ~EE_CNTRL_NOCOLORS);  // Farbansicht einschalten

	sal_uInt16 nPage = mpFrameView->GetSelectedPage();
	pLastPage = GetDoc()->GetSdPage( nPage, PK_STANDARD );
    pOlView->SetActualPage(pLastPage);
}



/*************************************************************************
|*
|* Write actual views data to FrameView
|*
\************************************************************************/

void OutlineViewShell::WriteFrameViewData()
{
	::Outliner* pOutl = pOlView->GetOutliner();

	sal_uLong nCntrl = pOutl->GetControlWord();
	sal_Bool bNoColor = sal_False;
	if (nCntrl & EE_CNTRL_NOCOLORS)
		bNoColor = sal_True;
	mpFrameView->SetNoColors(bNoColor);
	mpFrameView->SetNoAttribs( pOutl->IsFlatMode() );
	SdPage* pActualPage = pOlView->GetActualPage();
    DBG_ASSERT(pActualPage, "No current page");
	if( pActualPage )
        mpFrameView->SetSelectedPage((pActualPage->GetPageNum() - 1) / 2);
}


/*************************************************************************
|*
|* SfxRequests fuer StatusBar bearbeiten
|*
\************************************************************************/

void OutlineViewShell::ExecStatusBar(SfxRequest&)
{
}

/*************************************************************************
|*
|* Statuswerte der Statusbar zurueckgeben
|*
\************************************************************************/

void OutlineViewShell::GetStatusBarState(SfxItemSet& rSet)
{
	// Zoom-Item
	if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_ATTR_ZOOM ) )
	{
		SvxZoomItem* pZoomItem;
		sal_uInt16 nZoom = (sal_uInt16) GetActiveWindow()->GetZoom();

		pZoomItem = new SvxZoomItem( SVX_ZOOM_PERCENT, nZoom );

		// Bereich einschraenken
		sal_uInt16 nZoomValues = SVX_ZOOM_ENABLE_ALL;
		nZoomValues &= ~SVX_ZOOM_ENABLE_OPTIMAL;
		nZoomValues &= ~SVX_ZOOM_ENABLE_WHOLEPAGE;
		nZoomValues &= ~SVX_ZOOM_ENABLE_PAGEWIDTH;

		pZoomItem->SetValueSet( nZoomValues );
		rSet.Put( *pZoomItem );
		delete pZoomItem;
	}

	if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_ATTR_ZOOMSLIDER ) )
	{
		if (GetDocSh()->IsUIActive() || !GetActiveWindow() )
		{
			rSet.DisableItem( SID_ATTR_ZOOMSLIDER );
		}
		else
		{
			sd::Window * pActiveWindow = GetActiveWindow();
			SvxZoomSliderItem aZoomItem( (sal_uInt16) pActiveWindow->GetZoom(), (sal_uInt16)pActiveWindow->GetMinZoom(), (sal_uInt16)pActiveWindow->GetMaxZoom() ) ;
			aZoomItem.AddSnappingPoint(100);
			rSet.Put( aZoomItem );
		}
	}


	// Seitenanzeige und Layout
	/*
	if( SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_STATUS_PAGE ) ||
		SFX_ITEM_AVAILABLE == rSet.GetItemState( SID_STATUS_LAYOUT ) )
	*/
	sal_uInt16	nPageCount = GetDoc()->GetSdPageCount( PK_STANDARD );
	String	aPageStr, aLayoutStr;

	::sd::Window*		pWin		= GetActiveWindow();
	OutlinerView*	pActiveView = pOlView->GetViewByWindow( pWin );
	::Outliner*     pOutliner   = pOlView->GetOutliner();
	List*			pSelList	= (List*)pActiveView->CreateSelectionList();
	Paragraph*		pFirstPara	= (Paragraph*)pSelList->First();
	Paragraph*		pLastPara	= (Paragraph*)pSelList->Last();

	if( !pOutliner->HasParaFlag(pFirstPara,PARAFLAG_ISPAGE) )
		pFirstPara = pOlView->GetPrevTitle( pFirstPara );

	if( !pOutliner->HasParaFlag(pLastPara, PARAFLAG_ISPAGE) )
		pLastPara = pOlView->GetPrevTitle( pLastPara );

	delete pSelList;				// die wurde extra fuer uns erzeugt

	// nur eine Seite selektiert?
	if( pFirstPara == pLastPara )
	{
		// wieviele Seiten sind vor der selektierten Seite?
		sal_uLong nPos = 0L;
		while( pFirstPara )
		{
			pFirstPara = pOlView->GetPrevTitle( pFirstPara );
			if( pFirstPara )
				nPos++;
		}

		if( nPos >= GetDoc()->GetSdPageCount( PK_STANDARD ) )
			nPos = 0;

		SdrPage* pPage = GetDoc()->GetSdPage( (sal_uInt16) nPos, PK_STANDARD );

		aPageStr = String(SdResId( STR_SD_PAGE ));
		aPageStr += sal_Unicode(' ');
		aPageStr += String::CreateFromInt32( (sal_Int32)(nPos + 1) );	// sal_uLong -> sal_Int32
		aPageStr.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " / " ));
		aPageStr += String::CreateFromInt32( nPageCount );

		aLayoutStr = pPage->GetLayoutName();
		aLayoutStr.Erase( aLayoutStr.SearchAscii( SD_LT_SEPARATOR ) );
			//Now, CurrentPage property change is already sent for DrawView and OutlineView, so it is not necessary to send again here
			if(m_StrOldPageName!=aPageStr)
			{
				GetViewShellBase().GetDrawController().fireSwitchCurrentPage(nPos);
				m_StrOldPageName = aPageStr;
			}	
	}
	rSet.Put( SfxStringItem( SID_STATUS_PAGE, aPageStr ) );
	rSet.Put( SfxStringItem( SID_STATUS_LAYOUT, aLayoutStr ) );
}

/*************************************************************************
|*
|* Command event
|*
\************************************************************************/

void OutlineViewShell::Command( const CommandEvent& rCEvt, ::sd::Window* pWin )
{
	if ( rCEvt.GetCommand() == COMMAND_CONTEXTMENU )
	{
		GetActiveWindow()->ReleaseMouse();

		OutlinerView* pOLV = pOlView->GetViewByWindow(GetActiveWindow());
		Point aPos(rCEvt.GetMousePosPixel());

		if (pOLV && pOLV->IsWrongSpelledWordAtPos(aPos))
		{
			// #91457# Popup for Online-Spelling now handled by DrawDocShell
			// Link aLink = LINK(GetDoc(), SdDrawDocument, OnlineSpellCallback);
			Link aLink = LINK(GetDocSh(), DrawDocShell, OnlineSpellCallback);

			pOLV->ExecuteSpellPopup(aPos, &aLink);
		}
		else
		{
		   GetViewFrame()->GetDispatcher()->ExecutePopup(SdResId(RID_OUTLINE_POPUP));
		}
	}
	else
	{
		ViewShell::Command( rCEvt, pWin );

		// ggfs. Preview den neuen Kontext mitteilen
		Invalidate( SID_PREVIEW_STATE );

	}
}


/*************************************************************************
|*
|* Keyboard event
|*
\************************************************************************/

sal_Bool OutlineViewShell::KeyInput(const KeyEvent& rKEvt, ::sd::Window* pWin)
{
	sal_Bool bReturn = sal_False;
	OutlineViewPageChangesGuard aGuard(pOlView);

	if (pWin == NULL && HasCurrentFunction())
	{
		bReturn = GetCurrentFunction()->KeyInput(rKEvt);
	}

	// nein, weiterleiten an Basisklasse
	else
	{
		bReturn = ViewShell::KeyInput(rKEvt, pWin);
	}

	Invalidate(SID_STYLE_EDIT);
	Invalidate(SID_STYLE_NEW);
	Invalidate(SID_STYLE_DELETE);
	Invalidate(SID_STYLE_UPDATE_BY_EXAMPLE);
	Invalidate(SID_STYLE_NEW_BY_EXAMPLE);
	Invalidate(SID_STYLE_WATERCAN);
	Invalidate(SID_STYLE_FAMILY5);

	// Pruefen und Unterscheiden von CursorBewegungs- oder Eingabe-Keys
	KeyCode aKeyGroup( rKEvt.GetKeyCode().GetGroup() );
	if( (aKeyGroup != KEYGROUP_CURSOR && aKeyGroup != KEYGROUP_FKEYS) ||
		(GetActualPage() != pLastPage) )
	{
		Invalidate( SID_PREVIEW_STATE );
	}

	return(bReturn);
}


/*************************************************************************
|*
|* Optimale Groesse zurueckgeben
|*
\************************************************************************/

Size OutlineViewShell::GetOptimalSizePixel() const
{
	Size aResult(200, 200);
	if (pOlView)
	{
		::Outliner* pOutliner = pOlView->GetOutliner();
		if (pOutliner)
		{
			Size aTemp = pOutliner->CalcTextSize();
			aTemp = GetActiveWindow()->LogicToPixel(aTemp);
			aResult.Width() = Max(aResult.Width(), aTemp.Width());
			aResult.Height() = Max(aResult.Height(), aTemp.Height());
			if (4 * aResult.Height() > 3 * aResult.Width())
			{
				aResult.Height() = 3 * aResult.Width() / 4;
			}
		}
	}

	// und jetzt jetzt das Standardgelumpe draufaddieren
	aResult.Width()  += mpVerticalScrollBar->GetSizePixel().Width();
	aResult.Height() += mpHorizontalScrollBar->GetSizePixel().Height();
		//!!! +	System::GetMenuBarHeightPixel();  // statt Titlebar
	return aResult;
}


/*************************************************************************
|*
|* Text der Selektion zurueckgeben
|*
\************************************************************************/

String OutlineViewShell::GetSelectionText(sal_Bool bCompleteWords)
{
	String aStrSelection;
	::Outliner* pOl = pOlView->GetOutliner();
	OutlinerView* pOutlinerView = pOlView->GetViewByWindow( GetActiveWindow() );

	if (pOl && pOlView)
	{
		if (bCompleteWords)
		{
			ESelection aSel = pOutlinerView->GetSelection();
			String aStrCurrentDelimiters = pOl->GetWordDelimiters();

			pOl->SetWordDelimiters( String( RTL_CONSTASCII_USTRINGPARAM( " .,;\"'" )));
			aStrSelection = pOl->GetWord( aSel.nEndPara, aSel.nEndPos );
			pOl->SetWordDelimiters( aStrCurrentDelimiters );
		}
		else
		{
			aStrSelection = pOutlinerView->GetSelected();
		}
	}

	return (aStrSelection);
}


/*************************************************************************
|*
|* Ist etwas selektiert?
|*
\************************************************************************/

sal_Bool OutlineViewShell::HasSelection(sal_Bool bText) const
{
	sal_Bool bReturn = sal_False;

	if (bText)
	{
		OutlinerView* pOutlinerView = pOlView->GetViewByWindow( GetActiveWindow() );

		if (pOutlinerView && pOutlinerView->GetSelected().Len() != 0)
		{
			bReturn = sal_True;
		}
	}

	return bReturn;
}


/*************************************************************************
|*
|* Status der Attribut-Items
|*
\************************************************************************/

void OutlineViewShell::GetAttrState( SfxItemSet& rSet )
{
	SfxWhichIter  aIter( rSet );
	sal_uInt16        nWhich = aIter.FirstWhich();
	SfxAllItemSet aAllSet( *rSet.GetPool() );

	while ( nWhich )
	{
		sal_uInt16 nSlotId = SfxItemPool::IsWhich(nWhich)
			? GetPool().GetSlotId(nWhich)
			: nWhich;

		switch ( nSlotId )
		{
			case SID_STYLE_FAMILY2:
			case SID_STYLE_FAMILY3:
			{
				rSet.DisableItem( nWhich );
			}
			break;

			case SID_STYLE_FAMILY5:
			{
				SfxStyleSheet* pStyleSheet = pOlView->GetViewByWindow(GetActiveWindow())->GetStyleSheet();

				if( pStyleSheet )
				{
					pStyleSheet = ((SdStyleSheet*)pStyleSheet)->GetPseudoStyleSheet();

					if (pStyleSheet)
					{
						SfxTemplateItem aItem( nWhich, pStyleSheet->GetName() );
						aAllSet.Put( aItem, aItem.Which()  );
					}
				}

				if( !pStyleSheet )
				{
					SfxTemplateItem aItem( nWhich, String() );
					aAllSet.Put( aItem, aItem.Which() );
					// rSet.DisableItem( nWhich );
				}
			}
			break;

			case SID_STYLE_EDIT:
			{
				ISfxTemplateCommon* pTmplCommon = SFX_APP()->GetCurrentTemplateCommon(GetViewFrame()->GetBindings());

				if (pTmplCommon && pTmplCommon->GetActualFamily() == SD_STYLE_FAMILY_PSEUDO)
				{
					SfxItemSet aSet(*rSet.GetPool(), SID_STATUS_LAYOUT, SID_STATUS_LAYOUT);
					GetStatusBarState(aSet);
					String aRealStyle(((SfxStringItem&) aSet.Get(SID_STATUS_LAYOUT)).GetValue());

					if (!aRealStyle.Len())
					{
						// Kein eindeutiger Layoutname gefunden
						rSet.DisableItem(nWhich);
					}
				}
			}
			break;

			case SID_STYLE_UPDATE_BY_EXAMPLE:
			{
				::sd::Window*     pActWin = GetActiveWindow();
				OutlinerView* pOV = pOlView->GetViewByWindow(pActWin);
				ESelection aESel(pOV->GetSelection());

				if (aESel.nStartPara != aESel.nEndPara ||
					aESel.nStartPos  != aESel.nEndPos)
					// aufgespannte Selektion, also StyleSheet und/oder
					// Attributierung nicht zwingend eindeutig
					rSet.DisableItem(nWhich);
			}
			break;

			case SID_STYLE_NEW:
			case SID_STYLE_DELETE:
			case SID_STYLE_NEW_BY_EXAMPLE:
			case SID_STYLE_WATERCAN:
			{
				rSet.DisableItem(nWhich);
			}
			break;
		}

		nWhich = aIter.NextWhich();
	}

	rSet.Put( aAllSet, sal_False );
}



/*************************************************************************
|*
|* MouseButtonUp event
|*
\************************************************************************/

void OutlineViewShell::MouseButtonUp(const MouseEvent& rMEvt, ::sd::Window* pWin)
{
	// Zuerst die Basisklasse
	ViewShell::MouseButtonUp(rMEvt, pWin);

	Invalidate(SID_STYLE_EDIT);
	Invalidate(SID_STYLE_NEW);
	Invalidate(SID_STYLE_DELETE);
	Invalidate(SID_STYLE_UPDATE_BY_EXAMPLE);
	Invalidate(SID_STYLE_NEW_BY_EXAMPLE);
	Invalidate(SID_STYLE_WATERCAN);
	Invalidate(SID_STYLE_FAMILY5);

	// ggfs. Preview den neuen Kontext mitteilen
	if( GetActualPage() != pLastPage )
		Invalidate( SID_PREVIEW_STATE );
}



SdPage* OutlineViewShell::getCurrentPage() const
{
	// since there are no master pages in outline view, we can
	// for now use the GetActualPage method
	return const_cast<OutlineViewShell*>(this)->GetActualPage();
}

/*************************************************************************
|*
|* Liefert die erste selektierte Seite zurueck.
|* Wenn nichts selektiert ist, wird die erste Seite zurueckgeliefert.
|*
\************************************************************************/
SdPage* OutlineViewShell::GetActualPage()
{
	return pOlView->GetActualPage();
}


void OutlineViewShell::UpdatePreview( SdPage* pPage, sal_Bool )
{
	const bool bNewPage = pPage != pLastPage;
	pLastPage = pPage;
	if (bNewPage)
	{
		OutlineViewPageChangesGuard aGuard(pOlView);
        SetCurrentPage(pPage);
    }
}

/*************************************************************************
|*
|* Update Title
|*
\************************************************************************/

bool OutlineViewShell::UpdateTitleObject( SdPage* pPage, Paragraph* pPara )
{
	DBG_ASSERT( pPage, "sd::OutlineViewShell::UpdateTitleObject(), pPage == 0?" );
	DBG_ASSERT( pPara, "sd::OutlineViewShell::UpdateTitleObject(), pPara == 0?" );

	if( !pPage || !pPara )
		return false;

	::Outliner* 			pOutliner = pOlView->GetOutliner();
	SdrTextObj* 		pTO  = pOlView->GetTitleTextObject( pPage );
	OutlinerParaObject* pOPO = NULL;

	String	aTest( pOutliner->GetText( pPara ) );
	bool 	bText = aTest.Len() > 0;
	bool 	bNewObject = false;

	if( bText )
	{
		// create a title object if we don't have one but have text
		if( !pTO )
		{
			DBG_ASSERT( pOlView->isRecordingUndo(), "sd::OutlineViewShell::UpdateTitleObject(), no undo for model change!?" );
			pTO = pOlView->CreateTitleTextObject(pPage);
			bNewObject = sal_True;
		}

		// if we have a title object and a text, set the text
		if( pTO )
		{
			pOPO = pOutliner->CreateParaObject( (sal_uInt16) pOutliner->GetAbsPos( pPara ), 1 );
			pOPO->SetOutlinerMode( OUTLINERMODE_TITLEOBJECT );
			pOPO->SetVertical( pTO->IsVerticalWriting() );
			if( pTO->GetOutlinerParaObject() && (pOPO->GetTextObject() == pTO->GetOutlinerParaObject()->GetTextObject()) )
			{
				// do nothing, same text already set
				delete pOPO;
			}
			else
			{
				DBG_ASSERT( pOlView->isRecordingUndo(), "sd::OutlineViewShell::UpdateTitleObject(), no undo for model change!?" );
				if( !bNewObject && pOlView->isRecordingUndo() )
					pOlView->AddUndo(GetDoc()->GetSdrUndoFactory().CreateUndoObjectSetText(*pTO,0));

				pTO->SetOutlinerParaObject( pOPO );
				pTO->SetEmptyPresObj( sal_False );
				pTO->ActionChanged();
			}
		}
	}
	else if( pTO )
	{
		// no text but object available?
		// outline object available, but we have no text
		if(pPage->IsPresObj(pTO))
		{
			// if it is not already empty
			if( !pTO->IsEmptyPresObj() )
			{
				DBG_ASSERT( pOlView->isRecordingUndo(), "sd::OutlineViewShell::UpdateTitleObject(), no undo for model change!?" );

				// make it empty
				if( pOlView->isRecordingUndo() )
					pOlView->AddUndo(GetDoc()->GetSdrUndoFactory().CreateUndoObjectSetText(*pTO,0));
				pPage->RestoreDefaultText( pTO );
				pTO->SetEmptyPresObj(sal_True);
				pTO->ActionChanged();
			}
		}
		else
		{
			DBG_ASSERT( pOlView->isRecordingUndo(), "sd::OutlineViewShell::UpdateTitleObject(), no undo for model change!?" );
			// outline object is not part of the layout, delete it
			if( pOlView->isRecordingUndo() )
				pOlView->AddUndo(GetDoc()->GetSdrUndoFactory().CreateUndoRemoveObject(*pTO));
			pPage->RemoveObject(pTO->GetOrdNum());
		}
	}

	return bNewObject;
}

/*************************************************************************
|*
|* Update LayoutObject
|*
\************************************************************************/

bool OutlineViewShell::UpdateOutlineObject( SdPage* pPage, Paragraph* pPara )
{
	DBG_ASSERT( pPage, "sd::OutlineViewShell::UpdateOutlineObject(), pPage == 0?" );
	DBG_ASSERT( pPara, "sd::OutlineViewShell::UpdateOutlineObject(), pPara == 0?" );

	if( !pPage || !pPara )
		return false;

	::Outliner* 		pOutliner = pOlView->GetOutliner();
	OutlinerParaObject* pOPO = NULL;
	SdrTextObj* 		pTO  = NULL;

	sal_Bool bNewObject = sal_False;

	sal_uInt16 eOutlinerMode = OUTLINERMODE_TITLEOBJECT;
	pTO = (SdrTextObj*)pPage->GetPresObj( PRESOBJ_TEXT );
	if( !pTO )
	{
		eOutlinerMode = OUTLINERMODE_OUTLINEOBJECT;
		pTO = pOlView->GetOutlineTextObject( pPage );
	}

	// wieviele Absaetze in der Gliederung?
	sal_uLong nTitlePara	 = pOutliner->GetAbsPos( pPara );
	sal_uLong nPara          = nTitlePara + 1;
	sal_uLong nParasInLayout = 0L;
	pPara = pOutliner->GetParagraph( nPara );
	while( pPara && !pOutliner->HasParaFlag(pPara, PARAFLAG_ISPAGE) )
	{
		nParasInLayout++;
		pPara = pOutliner->GetParagraph( ++nPara );
	}
	if( nParasInLayout )
	{
		// ein OutlinerParaObject erzeugen
		pPara = pOutliner->GetParagraph( nTitlePara + 1 );
		pOPO  = pOutliner->CreateParaObject( (sal_uInt16) nTitlePara + 1, (sal_uInt16) nParasInLayout );
	}

	if( pOPO )
	{
		DBG_ASSERT( pOlView->isRecordingUndo(), "sd::OutlineViewShell::UpdateOutlineObject(), no undo for model change!?" );

		// do we need an outline text object?
		if( !pTO )
		{
			pTO = pOlView->CreateOutlineTextObject( pPage );
			bNewObject = sal_True;
		}

		// Seitenobjekt, Gliederungstext im Outliner:
		// Text uebernehmen
		if( pTO )
		{
			pOPO->SetVertical( pTO->IsVerticalWriting() );
			pOPO->SetOutlinerMode( eOutlinerMode );
			if( pTO->GetOutlinerParaObject() && (pOPO->GetTextObject() == pTO->GetOutlinerParaObject()->GetTextObject()) )
			{
				// do nothing, same text already set
				delete pOPO;
			}
			else
			{
				if( !bNewObject && pOlView->isRecordingUndo() )
					pOlView->AddUndo(GetDoc()->GetSdrUndoFactory().CreateUndoObjectSetText(*pTO,0));

				pTO->SetOutlinerParaObject( pOPO );
				pTO->SetEmptyPresObj( sal_False );
				pTO->ActionChanged();
			}
		}
	}
	else if( pTO )
	{
		// Seitenobjekt, aber kein Gliederungstext:
		// wenn Objekt in Praesentationsliste der Seite ist -> Defaulttext,
		// sonst Objekt loeschen
		if( pPage->IsPresObj(pTO) )
		{
			if( !pTO->IsEmptyPresObj() )
			{
				DBG_ASSERT( pOlView->isRecordingUndo(), "sd::OutlineViewShell::UpdateOutlineObject(), no undo for model change!?" );

				// loescht auch altes OutlinerParaObject
				if( pOlView->isRecordingUndo() )
					pOlView->AddUndo(GetDoc()->GetSdrUndoFactory().CreateUndoObjectSetText(*pTO,0));
				pPage->RestoreDefaultText( pTO );
				pTO->SetEmptyPresObj(sal_True);
				pTO->ActionChanged();
			}
		}
		else
		{
			DBG_ASSERT( pOlView->isRecordingUndo(), "sd::OutlineViewShell::UpdateOutlineObject(), no undo for model change!?" );
			if( pOlView->isRecordingUndo() )
				pOlView->AddUndo(GetDoc()->GetSdrUndoFactory().CreateUndoRemoveObject(*pTO));
			pPage->RemoveObject(pTO->GetOrdNum());
		}
	}

	return bNewObject;
}


/*************************************************************************
|*
|* Outliner aus Stream fuellen
|*
\************************************************************************/

sal_uLong OutlineViewShell::Read(SvStream& rInput, const String& rBaseURL, sal_uInt16 eFormat)
{
	sal_uLong bRet = 0;

	::Outliner* pOutl = pOlView->GetOutliner();

	{
	OutlineViewPageChangesGuard aGuard( pOlView );
	OutlineViewModelChangeGuard aGuard2( *pOlView );

    bRet = pOutl->Read( rInput, rBaseURL, eFormat, GetDocSh()->GetHeaderAttributes() );

	SdPage* pPage = GetDoc()->GetSdPage( GetDoc()->GetSdPageCount(PK_STANDARD) - 1, PK_STANDARD );;
	SfxStyleSheet* pTitleSheet = pPage->GetStyleSheetForPresObj( PRESOBJ_TITLE );
	SfxStyleSheet* pOutlSheet = pPage->GetStyleSheetForPresObj( PRESOBJ_OUTLINE );

	sal_uInt16 nParaCount = (sal_uInt16)pOutl->GetParagraphCount();
	if ( nParaCount > 0 )
	{
		for ( sal_uInt16 nPara = 0; nPara < nParaCount; nPara++ )
		{
			pOlView->UpdateParagraph( nPara );

			sal_Int16 nDepth = pOutl->GetDepth( nPara );

			if( (nDepth == 0) || !nPara )
			{
				Paragraph* pPara = pOutl->GetParagraph( nPara );
				pOutl->SetDepth(pPara, -1);
				pOutl->SetParaFlag(pPara, PARAFLAG_ISPAGE);

				pOutl->SetStyleSheet( nPara, pTitleSheet );

				if( nPara ) // first slide already exists
					pOlView->InsertSlideForParagraph( pPara );
			}
			else
			{
				pOutl->SetDepth( pOutl->GetParagraph( nPara ), nDepth - 1 );
				String aStyleSheetName( pOutlSheet->GetName() );
				aStyleSheetName.Erase( aStyleSheetName.Len() - 1, 1 );
				aStyleSheetName += String::CreateFromInt32( nDepth );
				SfxStyleSheetBasePool* pStylePool = GetDoc()->GetStyleSheetPool();
				SfxStyleSheet* pStyle = (SfxStyleSheet*) pStylePool->Find( aStyleSheetName, pOutlSheet->GetFamily() );
				DBG_ASSERT( pStyle, "AutoStyleSheetName - Style not found!" );
				if ( pStyle )
					pOutl->SetStyleSheet( nPara, pStyle );
			}
		}
	}
	}

	pOutl->GetUndoManager().Clear();

	return( bRet );
}

void OutlineViewShell::WriteUserDataSequence ( ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >& rSequence, sal_Bool bBrowse )
{
	WriteFrameViewData();

	ViewShell::WriteUserDataSequence( rSequence, bBrowse );
}

void OutlineViewShell::ReadUserDataSequence ( const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >& rSequence, sal_Bool bBrowse )
{
	WriteFrameViewData();

	ViewShell::ReadUserDataSequence( rSequence, bBrowse );

	ReadFrameViewData( mpFrameView );
}

void OutlineViewShell::VisAreaChanged(const Rectangle& rRect)
{
	ViewShell::VisAreaChanged( rRect );

	GetViewShellBase().GetDrawController().FireVisAreaChanged(rRect);
}

/** If there is a valid controller then create a new instance of
    <type>AccessibleDrawDocumentView</type>.  Otherwise delegate this call
    to the base class to return a default object (probably an empty
    reference).
*/
::com::sun::star::uno::Reference<
    ::com::sun::star::accessibility::XAccessible>
    OutlineViewShell::CreateAccessibleDocumentView (::sd::Window* pWindow)
{
    OSL_ASSERT (GetViewShell()!=NULL);
    if (GetViewShell()->GetController() != NULL)
    {
        ::accessibility::AccessibleOutlineView* pDocumentView =
            new ::accessibility::AccessibleOutlineView (
                pWindow,
                this,
                GetViewShell()->GetController(),
                pWindow->GetAccessibleParentWindow()->GetAccessible());
        pDocumentView->Init();
        return ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessible>
            (static_cast< ::com::sun::star::uno::XWeak*>(pDocumentView),
                ::com::sun::star::uno::UNO_QUERY);
    }
    else
    {
        OSL_TRACE ("OutlineViewShell::CreateAccessibleDocumentView: no controller");
        return ViewShell::CreateAccessibleDocumentView (pWindow);
    }
}




void OutlineViewShell::GetState (SfxItemSet& rSet)
{
    // Iterate over all requested items in the set.
	SfxWhichIter aIter( rSet );
	sal_uInt16 nWhich = aIter.FirstWhich();
	while (nWhich)
	{
		switch (nWhich)
		{
			case SID_SEARCH_ITEM:
			case SID_SEARCH_OPTIONS:
                // Call common (old) implementation in the document shell.
                GetDocSh()->GetState (rSet);
                break;
			default:
                OSL_TRACE ("OutlineViewShell::GetState(): can not handle which id %d", nWhich);
                break;
        }
		nWhich = aIter.NextWhich();
	}
}




void OutlineViewShell::SetCurrentPage (SdPage* pPage)
{
    // Adapt the selection of the model.
    for (sal_uInt16 i=0; i<GetDoc()->GetSdPageCount(PK_STANDARD); i++)
        GetDoc()->SetSelected(
            GetDoc()->GetSdPage(i, PK_STANDARD),
            sal_False);
    GetDoc()->SetSelected (pPage, sal_True);

    DrawController& rController(GetViewShellBase().GetDrawController());
    rController.FireSelectionChangeListener();
    rController.FireSwitchCurrentPage (pPage);

    pOlView->SetActualPage(pPage);
}


} // end of namespace sd
