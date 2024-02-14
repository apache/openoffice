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

#include <sfx2/bindings.hxx>
#include <svx/htmlmode.hxx>
#include <svx/sdtacitm.hxx>
#include <svx/svdobj.hxx>
#include <svx/sdtagitm.hxx>
#include <svx/sdtakitm.hxx>
#include <svx/sdtaditm.hxx>
#include <svx/sdtaaitm.hxx>
#include <svx/svdview.hxx>
#include <svx/svdocapt.hxx>
#include <editeng/outlobj.hxx>
#include <cmdid.h>
#include <view.hxx>
#include <edtwin.hxx>
#include <wrtsh.hxx>
#include <viewopt.hxx>
#include <drawbase.hxx>
#include <conrect.hxx>

/*************************************************************************
|* C'Tor
\************************************************************************/

ConstRectangle::ConstRectangle( SwWrtShell* pWrtShell, SwEditWin* pEditWin,
								SwView* pSwView )
	: SwDrawBase( pWrtShell, pEditWin, pSwView ),
	bMarquee(sal_False),
	// #93382#
	mbVertical(sal_False)
{
}

/*************************************************************************
|* MouseButtonDown-event
\************************************************************************/

sal_Bool ConstRectangle::MouseButtonDown(const MouseEvent& rMEvt)
{
	sal_Bool bReturn;

	if ((bReturn = SwDrawBase::MouseButtonDown(rMEvt)) == sal_True
									&& m_pWin->GetSdrDrawMode() == OBJ_CAPTION)
	{
		m_pView->NoRotate();
		if (m_pView->IsDrawSelMode())
		{
			m_pView->FlipDrawSelMode();
			m_pSh->GetDrawView()->SetFrameDragSingles(m_pView->IsDrawSelMode());
		}
	}
	return (bReturn);
}

/*************************************************************************
|* MouseButtonUp-event
\************************************************************************/

sal_Bool ConstRectangle::MouseButtonUp(const MouseEvent& rMEvt)
{
	Point aPnt(m_pWin->PixelToLogic(rMEvt.GetPosPixel()));

	sal_Bool bRet = SwDrawBase::MouseButtonUp(rMEvt);
	if( bRet )
	{
		SdrView *pSdrView = m_pSh->GetDrawView();
		const SdrMarkList& rMarkList = pSdrView->GetMarkedObjectList();
		SdrObject* pObj = rMarkList.GetMark(0) ? rMarkList.GetMark(0)->GetMarkedSdrObj()
											   : 0;
		switch( m_pWin->GetSdrDrawMode() )
		{
		case OBJ_TEXT:
			if( bMarquee )
			{
				m_pSh->ChgAnchor(FLY_AS_CHAR);

				if( pObj )
				{
					// die fuer das Scrollen benoetigten Attribute setzen
					SfxItemSet aItemSet( pSdrView->GetModel()->GetItemPool(),
										SDRATTR_MISC_FIRST, SDRATTR_MISC_LAST);

					aItemSet.Put( SdrTextAutoGrowWidthItem( sal_False ) );
					aItemSet.Put( SdrTextAutoGrowHeightItem( sal_False ) );
					aItemSet.Put( SdrTextAniKindItem( SDRTEXTANI_SCROLL ) );
					aItemSet.Put( SdrTextAniDirectionItem( SDRTEXTANI_LEFT ) );
					aItemSet.Put( SdrTextAniCountItem( 0 ) );
					aItemSet.Put( SdrTextAniAmountItem(
							(sal_Int16)m_pWin->PixelToLogic(Size(2,1)).Width()) );

					pObj->SetMergedItemSetAndBroadcast(aItemSet);
				}
			}
			else if(mbVertical && pObj && pObj->ISA(SdrTextObj))
			{
				// #93382#
				SdrTextObj* pText = (SdrTextObj*)pObj;
				SfxItemSet aSet(pSdrView->GetModel()->GetItemPool());

				pText->SetVerticalWriting(sal_True);

				aSet.Put(SdrTextAutoGrowWidthItem(sal_True));
				aSet.Put(SdrTextAutoGrowHeightItem(sal_False));
				aSet.Put(SdrTextVertAdjustItem(SDRTEXTVERTADJUST_TOP));
				aSet.Put(SdrTextHorzAdjustItem(SDRTEXTHORZADJUST_RIGHT));

				pText->SetMergedItemSet(aSet);
			}
			if( pObj )
			{
				SdrPageView* pPV = pSdrView->GetSdrPageView();
				m_pView->BeginTextEdit( pObj, pPV, m_pWin, sal_True );
			}
			m_pView->LeaveDrawCreate(); // In Selektionsmode wechseln
			m_pSh->GetView().GetViewFrame()->GetBindings().Invalidate(SID_INSERT_DRAW);
			break;

		case OBJ_CAPTION:
		{
			SdrCaptionObj* pCaptObj = dynamic_cast<SdrCaptionObj*>(pObj);
			if( bCapVertical && pCaptObj )
			{
				pCaptObj->ForceOutlinerParaObject();
				OutlinerParaObject* pOPO = pCaptObj->GetOutlinerParaObject();
				if( pOPO && !pOPO->IsVertical() )
					pOPO->SetVertical( sal_True );
			}
		}
		break;
		default:; //prevent warning
		}
	}
	return bRet;
}

/*************************************************************************
|* activate function
\************************************************************************/

void ConstRectangle::Activate(const sal_uInt16 nSlotId)
{
	bMarquee = bCapVertical = sal_False;
	mbVertical = sal_False;

	switch (nSlotId)
	{
	case SID_DRAW_LINE:
		m_pWin->SetSdrDrawMode(OBJ_LINE);
		break;

	case SID_DRAW_RECT:
		m_pWin->SetSdrDrawMode(OBJ_RECT);
		break;

	case SID_DRAW_ELLIPSE:
		m_pWin->SetSdrDrawMode(OBJ_CIRC);
		break;

	case SID_DRAW_TEXT_MARQUEE:
		bMarquee = sal_True;
		m_pWin->SetSdrDrawMode(OBJ_TEXT);
		break;

	case SID_DRAW_TEXT_VERTICAL:
		// #93382#
		mbVertical = sal_True;
		m_pWin->SetSdrDrawMode(OBJ_TEXT);
		break;

	case SID_DRAW_TEXT:
		m_pWin->SetSdrDrawMode(OBJ_TEXT);
		break;

	case SID_DRAW_CAPTION_VERTICAL:
		bCapVertical = sal_True;
		// no break
	case SID_DRAW_CAPTION:
		m_pWin->SetSdrDrawMode(OBJ_CAPTION);
		break;

	default:
		m_pWin->SetSdrDrawMode(OBJ_NONE);
		break;
	}

	SwDrawBase::Activate(nSlotId);
}

/* vim: set noet sw=4 ts=4: */
