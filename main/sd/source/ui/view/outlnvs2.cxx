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

#include <com/sun/star/presentation/XPresentation2.hpp>

#include "app.hrc"
#include <svx/hlnkitem.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/request.hxx>
#include <svl/eitem.hxx>
#ifndef _ZOOMITEM_HXX //autogen
#include <svx/zoomitem.hxx>
#endif
#include <vcl/msgbox.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/flditem.hxx>
#include <editeng/editstat.hxx>
#include "optsitem.hxx"
#include <unotools/useroptions.hxx>

#include <sfx2/viewfrm.hxx>
#include "Outliner.hxx"
#include "Window.hxx"
#include "OutlineViewShell.hxx"
#include "fubullet.hxx"
#include "fuolbull.hxx"
#include "FrameView.hxx"
#include "fuzoom.hxx"
#include "fuscale.hxx"
#include "fuchar.hxx"
#include "fuinsfil.hxx"
#include "fuprobjs.hxx"
#include "futhes.hxx"
#include "futempl.hxx"
#include "fusldlg.hxx"
#include "zoomlist.hxx"
#include "fuexpand.hxx"
#include "fusumry.hxx"
#include "fucushow.hxx"
#include "drawdoc.hxx"
#include "sdattr.hxx"
#include "ViewShellBase.hxx"
#include "sdabstdlg.hxx"
#include "framework/FrameworkHelper.hxx"
#include "DrawViewShell.hxx"

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::presentation;

namespace sd {


/************************************************************************/

/*************************************************************************
|*
|* SfxRequests fuer temporaere Funktionen
|*
\************************************************************************/

void OutlineViewShell::FuTemporary(SfxRequest &rReq)
{
	DeactivateCurrentFunction();

	OutlinerView* pOutlinerView = pOlView->GetViewByWindow( GetActiveWindow() );
	sal_uInt16 nSId = rReq.GetSlot();

	switch( nSId )
	{
		case SID_ATTR_ZOOM:
		{
			const SfxItemSet* pArgs = rReq.GetArgs();

			if ( pArgs )
			{
				SvxZoomType eZT = ( ( const SvxZoomItem& ) pArgs->
											Get( SID_ATTR_ZOOM ) ).GetType();
				switch( eZT )
				{
					case SVX_ZOOM_PERCENT:
						SetZoom( (long) ( ( const SvxZoomItem& ) pArgs->
											Get( SID_ATTR_ZOOM ) ).GetValue() );
						Invalidate( SID_ATTR_ZOOM );
						Invalidate( SID_ATTR_ZOOMSLIDER );
						break;
					default:
						break;
				}
				rReq.Done();
			}
			else
			{
				// hier den Zoom-Dialog oeffnen
				SetCurrentFunction( FuScale::Create( this, GetActiveWindow(), pOlView, GetDoc(), rReq ) );
			}
			Cancel();
		}
		break;

		case SID_ATTR_ZOOMSLIDER:
		{
			const SfxItemSet* pArgs = rReq.GetArgs();

			if (pArgs && pArgs->Count () == 1 )
			{
				SFX_REQUEST_ARG (rReq, pScale, SfxUInt16Item, SID_ATTR_ZOOMSLIDER, sal_False);
				if (CHECK_RANGE (5, pScale->GetValue (), 3000))
				{
					SetZoom (pScale->GetValue ());

					SfxBindings& rBindings = GetViewFrame()->GetBindings();
					rBindings.Invalidate( SID_ATTR_ZOOM );
					rBindings.Invalidate( SID_ZOOM_IN );
					rBindings.Invalidate( SID_ZOOM_OUT );
					rBindings.Invalidate( SID_ATTR_ZOOMSLIDER );

				}
			}

			Cancel();
			rReq.Done ();
			break;
		}

		case SID_ZOOM_OUT:
		{
			SetCurrentFunction( FuZoom::Create(this, GetActiveWindow(), pOlView, GetDoc(), rReq) );
			// Beendet sich selbst, kein Cancel() notwendig!
			rReq.Done();
		}
		break;

		case SID_SIZE_REAL:
		{
			SetZoom( 100 );
			Rectangle aVisAreaWin = GetActiveWindow()->PixelToLogic( Rectangle( Point(0,0),
											 GetActiveWindow()->GetOutputSizePixel()) );
			mpZoomList->InsertZoomRect(aVisAreaWin);
			Invalidate( SID_ATTR_ZOOM );
			Invalidate( SID_ATTR_ZOOMSLIDER );
			Cancel();
			rReq.Done();
		}
		break;

		case SID_ZOOM_IN:
		{
			SetZoom( Max( (long) ( GetActiveWindow()->GetZoom() / 2 ), (long) GetActiveWindow()->GetMinZoom() ) );
			Rectangle aVisAreaWin = GetActiveWindow()->PixelToLogic( Rectangle( Point(0,0),
											 GetActiveWindow()->GetOutputSizePixel()) );
			mpZoomList->InsertZoomRect(aVisAreaWin);
			Invalidate( SID_ATTR_ZOOM );
			Invalidate( SID_ZOOM_OUT);
			Invalidate( SID_ZOOM_IN );
			Invalidate( SID_ATTR_ZOOMSLIDER );
			Cancel();
			rReq.Done();
		}
		break;

		case SID_OUTLINE_COLLAPSE_ALL:
		{
			pOutlinerView->CollapseAll();
			Cancel();
			rReq.Done();
		}
		break;

		case SID_OUTLINE_COLLAPSE:
		{
			pOutlinerView->Collapse();
			Cancel();
			rReq.Done();
		}
		break;

		case SID_OUTLINE_EXPAND_ALL:
		{
			pOutlinerView->ExpandAll();
			Cancel();
			rReq.Done();
		}
		break;

		case SID_OUTLINE_EXPAND:
		{
			pOutlinerView->Expand();
			Cancel();
			rReq.Done();
		}
		break;

		case SID_OUTLINE_FORMAT:
		{
			::Outliner* pOutl = pOutlinerView->GetOutliner();
			pOutl->SetFlatMode( !pOutl->IsFlatMode() );
			Invalidate( SID_COLORVIEW );
			Cancel();
			rReq.Done();
		}
		break;

		case SID_SELECTALL:
		{
			::Outliner* pOutl = pOlView->GetOutliner();
			sal_uLong nParaCount = pOutl->GetParagraphCount();
			if (nParaCount > 0)
			{
				pOutlinerView->SelectRange( 0, nParaCount );
			}
			Cancel();
		}
		break;

		case SID_PRESENTATION:
		case SID_REHEARSE_TIMINGS:
		{
			pOlView->PrepareClose();

			Reference< XPresentation2 > xPresentation( GetDoc()->getPresentation() );
			if( xPresentation.is() )
            {
				if( ( SID_REHEARSE_TIMINGS != rReq.GetSlot() ) )
					xPresentation->start();
				else
					xPresentation->rehearseTimings();
            }
			Cancel();
			rReq.Done();
        }
        break;

		case SID_COLORVIEW:
		{
			::Outliner* pOutl = pOutlinerView->GetOutliner();
			sal_uLong nCntrl = pOutl->GetControlWord();

			if ( !(nCntrl & EE_CNTRL_NOCOLORS) )
			{
				// Farbansicht ist eingeschaltet: ausschalten
				pOutl->SetControlWord(nCntrl | EE_CNTRL_NOCOLORS);
			}
			else
			{
				// Farbansicht ist ausgeschaltet: einschalten
				pOutl->SetControlWord(nCntrl & ~EE_CNTRL_NOCOLORS);
			}

			InvalidateWindows();
			Invalidate( SID_COLORVIEW );
			Cancel();
			rReq.Done();
		}
		break;

		case SID_STYLE_EDIT:
		case SID_STYLE_UPDATE_BY_EXAMPLE:
		{
			if( rReq.GetArgs() )
			{
				SetCurrentFunction( FuTemplate::Create( this, GetActiveWindow(), pOlView, GetDoc(), rReq ) );
				Cancel();
			}

			rReq.Ignore ();
		}
		break;

		case SID_PRESENTATION_DLG:
		{
			SetCurrentFunction( FuSlideShowDlg::Create( this, GetActiveWindow(), pOlView, GetDoc(), rReq ) );
			Cancel();
		}
		break;

		case SID_CUSTOMSHOW_DLG:
		{
			SetCurrentFunction( FuCustomShowDlg::Create( this, GetActiveWindow(), pOlView, GetDoc(), rReq ) );
			Cancel();
		}
		break;
	}

	if(HasCurrentFunction())
		GetCurrentFunction()->Activate();

	Invalidate( SID_OUTLINE_COLLAPSE_ALL );
	Invalidate( SID_OUTLINE_COLLAPSE );
	Invalidate( SID_OUTLINE_EXPAND_ALL );
	Invalidate( SID_OUTLINE_EXPAND );

	SfxBindings& rBindings = GetViewFrame()->GetBindings();
	rBindings.Invalidate( SID_OUTLINE_LEFT );
	rBindings.Invalidate( SID_OUTLINE_RIGHT );
	rBindings.Invalidate( SID_OUTLINE_UP );
	rBindings.Invalidate( SID_OUTLINE_DOWN );

	Invalidate( SID_OUTLINE_FORMAT );
	Invalidate( SID_COLORVIEW );
	Invalidate(SID_CUT);
	Invalidate(SID_COPY);
	Invalidate(SID_PASTE);
}

void OutlineViewShell::FuTemporaryModify(SfxRequest &rReq)
{
	sal_uInt16 nSId = rReq.GetSlot();
	std::auto_ptr< OutlineViewModelChangeGuard > aGuard;
	if (nSId != SID_OUTLINE_BULLET && nSId != FN_SVX_SET_BULLET && nSId != FN_SVX_SET_NUMBER)
	{
		aGuard.reset( new OutlineViewModelChangeGuard(*pOlView) );
	}
	DeactivateCurrentFunction();

	OutlinerView* pOutlinerView = pOlView->GetViewByWindow( GetActiveWindow() );
	//sal_uInt16 nSId = rReq.GetSlot();

	switch( nSId )
	{
		case SID_HYPERLINK_SETLINK:
		{
			const SfxItemSet* pReqArgs = rReq.GetArgs();

			if (pReqArgs)
			{
				SvxHyperlinkItem* pHLItem =
                (SvxHyperlinkItem*) &pReqArgs->Get(SID_HYPERLINK_SETLINK);

				SvxFieldItem aURLItem(SvxURLField(pHLItem->GetURL(),
												  pHLItem->GetName(),
                                                  SVXURLFORMAT_REPR), EE_FEATURE_FIELD);
				ESelection aSel( pOutlinerView->GetSelection() );
				pOutlinerView->InsertField(aURLItem);
                if ( aSel.nStartPos <= aSel.nEndPos )
		            aSel.nEndPos = aSel.nStartPos + 1;
                else
                    aSel.nStartPos = aSel.nEndPos + 1;
				pOutlinerView->SetSelection( aSel );
			}

			Cancel();
			rReq.Ignore ();
		}
		break;

		case FN_INSERT_SOFT_HYPHEN:
		case FN_INSERT_HARDHYPHEN:
		case FN_INSERT_HARD_SPACE:
		case SID_INSERT_RLM :
		case SID_INSERT_LRM :
		case SID_INSERT_ZWNBSP :
		case SID_INSERT_ZWSP:
		case SID_CHARMAP:
		{
			SetCurrentFunction( FuBullet::Create( this, GetActiveWindow(), pOlView, GetDoc(), rReq ) );
			Cancel();
		}
		break;

		case SID_OUTLINE_BULLET:
		case FN_SVX_SET_BULLET:
		case FN_SVX_SET_NUMBER:
		{
			SetCurrentFunction( FuOutlineBullet::Create( this, GetActiveWindow(), pOlView, GetDoc(), rReq ) );
			Cancel();
		}
		break;

        case SID_THESAURUS:
		{
			SetCurrentFunction( FuThesaurus::Create( this, GetActiveWindow(), pOlView, GetDoc(), rReq ) );
			Cancel();
			rReq.Ignore ();
		}
		break;

		case SID_CHAR_DLG_EFFECT:
		case SID_CHAR_DLG:
		{
			SetCurrentFunction( FuChar::Create( this, GetActiveWindow(), pOlView, GetDoc(), rReq ) );
			Cancel();
		}
		break;

		case SID_INSERTFILE:
		{
			SetCurrentFunction( FuInsertFile::Create(this, GetActiveWindow(), pOlView, GetDoc(), rReq) );
			Cancel();
		}
		break;

		case SID_PRESENTATIONOBJECT:
		{
			SetCurrentFunction( FuPresentationObjects::Create(this, GetActiveWindow(), pOlView, GetDoc(), rReq) );
			Cancel();
		}
		break;

		case SID_SET_DEFAULT:
		{
			// 1. Selektion merken (kriegt die eselige EditEngine nicht selbst
			//	  auf die Reihe!)
			// 2. Update auf False (sonst flackert's noch staerker
			// an allen selektierten Absaetzen:
			//	a. deren Vorlage nochmal setzen, um absatzweite harte Attribute
			//	   zu entfernen
			//	b. harte Zeichenattribute loeschen
			// 3. Update auf True und Selektion wieder setzen
			/*
			ESelection aEsel= pOutlinerView->GetSelection();
			Outliner* pOutl = pOutlinerView->GetOutliner();
			pOutl->SetUpdateMode(sal_False);
			List* pSelectedParas = pOutlinerView->CreateSelectionList();
			Paragraph* pPara = (Paragraph*)pSelectedParas->First();
			while (pPara)
			{
				sal_uLong nParaPos = pOutl->GetAbsPos(pPara);
				String aName;
				SfxStyleFamily aFamily;
				pOutl->GetStyleSheet(nParaPos, aName, aFamily);
				pOutl->SetStyleSheet(nParaPos, aName, aFamily);
				pOutl->QuickRemoveCharAttribs(nParaPos);
				pPara = (Paragraph*)pSelectedParas->Next();
			}
			delete pSelectedParas;
			pOutl->SetUpdateMode(sal_True);
			pOutlinerView->SetSelection(aEsel);
			*/
			pOutlinerView->RemoveAttribs(sal_True); // sal_True = auch Absatzattribute
			Cancel();
			rReq.Done();
		}
		break;

		case SID_SUMMARY_PAGE:
		{
			pOlView->SetSelectedPages();
			SetCurrentFunction( FuSummaryPage::Create( this, GetActiveWindow(), pOlView, GetDoc(), rReq ) );
			pOlView->GetOutliner()->Clear();
			pOlView->FillOutliner();
			pOlView->GetActualPage();
			Cancel();
		}
		break;

		case SID_EXPAND_PAGE:
		{
			pOlView->SetSelectedPages();
			SetCurrentFunction( FuExpandPage::Create( this, GetActiveWindow(), pOlView, GetDoc(), rReq ) );
			pOlView->GetOutliner()->Clear();
			pOlView->FillOutliner();
			pOlView->GetActualPage();
			Cancel();
		}
		break;

		case SID_INSERT_FLD_DATE_FIX:
		case SID_INSERT_FLD_DATE_VAR:
		case SID_INSERT_FLD_TIME_FIX:
		case SID_INSERT_FLD_TIME_VAR:
		case SID_INSERT_FLD_AUTHOR:
		case SID_INSERT_FLD_PAGE:
		case SID_INSERT_FLD_PAGES:
		case SID_INSERT_FLD_FILE:
		{
			SvxFieldItem* pFieldItem = 0;

			switch( nSId )
			{
				case SID_INSERT_FLD_DATE_FIX:
					pFieldItem = new SvxFieldItem(
                        SvxDateField( Date(), SVXDATETYPE_FIX ), EE_FEATURE_FIELD );
				break;

				case SID_INSERT_FLD_DATE_VAR:
                    pFieldItem = new SvxFieldItem( SvxDateField(), EE_FEATURE_FIELD );
				break;

				case SID_INSERT_FLD_TIME_FIX:
					pFieldItem = new SvxFieldItem(
                        SvxExtTimeField( Time(), SVXTIMETYPE_FIX ), EE_FEATURE_FIELD );
				break;

				case SID_INSERT_FLD_TIME_VAR:
                    pFieldItem = new SvxFieldItem( SvxExtTimeField(), EE_FEATURE_FIELD );
				break;

				case SID_INSERT_FLD_AUTHOR:
				{
                    SvtUserOptions aUserOptions;
                    pFieldItem = new SvxFieldItem(
                            SvxAuthorField(
                                aUserOptions.GetFirstName(), aUserOptions.GetLastName(), aUserOptions.GetID() )
                                , EE_FEATURE_FIELD );
				}
				break;

				case SID_INSERT_FLD_PAGE:
                    pFieldItem = new SvxFieldItem( SvxPageField(), EE_FEATURE_FIELD );
				break;

				case SID_INSERT_FLD_PAGES:
                    pFieldItem = new SvxFieldItem( SvxPagesField(), EE_FEATURE_FIELD );
				break;

				case SID_INSERT_FLD_FILE:
				{
					String aName;
					if( GetDocSh()->HasName() )
						aName = GetDocSh()->GetMedium()->GetName();
					//else
					//	aName = GetDocSh()->GetName();
                    pFieldItem = new SvxFieldItem( SvxExtFileField( aName ), EE_FEATURE_FIELD );
				}
				break;
			}

			const SvxFieldItem* pOldFldItem = pOutlinerView->GetFieldAtSelection();

			if( pOldFldItem && ( pOldFldItem->GetField()->ISA( SvxURLField ) ||
								pOldFldItem->GetField()->ISA( SvxDateField ) ||
								pOldFldItem->GetField()->ISA( SvxTimeField ) ||
								pOldFldItem->GetField()->ISA( SvxExtTimeField ) ||
								pOldFldItem->GetField()->ISA( SvxExtFileField ) ||
								pOldFldItem->GetField()->ISA( SvxAuthorField ) ||
								pOldFldItem->GetField()->ISA( SvxPageField ) ||
                                pOldFldItem->GetField()->ISA( SvxPagesField )) )
			{
				// Feld selektieren, so dass es beim Insert geloescht wird
				ESelection aSel = pOutlinerView->GetSelection();
				if( aSel.nStartPos == aSel.nEndPos )
					aSel.nEndPos++;
				pOutlinerView->SetSelection( aSel );
			}

			if( pFieldItem )
				pOutlinerView->InsertField( *pFieldItem );

			delete pFieldItem;

			Cancel();
			rReq.Ignore ();
		}
		break;

		case SID_MODIFY_FIELD:
		{
			const SvxFieldItem* pFldItem = pOutlinerView->GetFieldAtSelection();

			if( pFldItem && (pFldItem->GetField()->ISA( SvxDateField ) ||
								pFldItem->GetField()->ISA( SvxAuthorField ) ||
								pFldItem->GetField()->ISA( SvxExtFileField ) ||
								pFldItem->GetField()->ISA( SvxExtTimeField ) ) )
			{
				// Dialog...
				SdAbstractDialogFactory* pFact = SdAbstractDialogFactory::Create();
				AbstractSdModifyFieldDlg* pDlg = pFact ? pFact->CreateSdModifyFieldDlg(GetActiveWindow(), pFldItem->GetField(), pOutlinerView->GetAttribs() ) : 0;
				if( pDlg && (pDlg->Execute() == RET_OK) )
				{
					SvxFieldData* pField = pDlg->GetField();
					if( pField )
					{
                        SvxFieldItem aFieldItem( *pField, EE_FEATURE_FIELD );
						//pOLV->DeleteSelected(); <-- fehlt leider !
						// Feld selektieren, so dass es beim Insert geloescht wird
						ESelection aSel = pOutlinerView->GetSelection();
						sal_Bool bSel = sal_True;
						if( aSel.nStartPos == aSel.nEndPos )
						{
							bSel = sal_False;
							aSel.nEndPos++;
						}
						pOutlinerView->SetSelection( aSel );

						pOutlinerView->InsertField( aFieldItem );

						// Selektion wird wieder in den Ursprungszustand gebracht
						if( !bSel )
							aSel.nEndPos--;
						pOutlinerView->SetSelection( aSel );

						delete pField;
					}

					SfxItemSet aSet( pDlg->GetItemSet() );
					if( aSet.Count() )
					{
						pOutlinerView->SetAttribs( aSet );

						::Outliner* pOutliner = pOutlinerView->GetOutliner();
						if( pOutliner )
							pOutliner->UpdateFields();
					}
				}
				delete pDlg;
			}

			Cancel();
			rReq.Ignore ();
		}
		break;
	}

	if(HasCurrentFunction())
		GetCurrentFunction()->Activate();

	Invalidate( SID_OUTLINE_COLLAPSE_ALL );
	Invalidate( SID_OUTLINE_COLLAPSE );
	Invalidate( SID_OUTLINE_EXPAND_ALL );
	Invalidate( SID_OUTLINE_EXPAND );

	SfxBindings& rBindings = GetViewFrame()->GetBindings();
	rBindings.Invalidate( SID_OUTLINE_LEFT );
	rBindings.Invalidate( SID_OUTLINE_RIGHT );
	rBindings.Invalidate( SID_OUTLINE_UP );
	rBindings.Invalidate( SID_OUTLINE_DOWN );

	Invalidate( SID_OUTLINE_FORMAT );
	Invalidate( SID_COLORVIEW );
	Invalidate(SID_CUT);
	Invalidate(SID_COPY);
	Invalidate(SID_PASTE);
}


} // end of namespace sd
