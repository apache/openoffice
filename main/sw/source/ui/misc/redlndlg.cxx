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


#define _SVSTDARR_STRINGSSORTDTOR
#define _SVSTDARR_USHORTSSORT
#define _SVSTDARR_USHORTS


#include <redline.hxx>
#include <tools/datetime.hxx>
#include <vcl/msgbox.hxx>
#include <svl/svstdarr.hxx>
#include <svl/eitem.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/dispatch.hxx>
#include <svx/ctredlin.hxx>
#include <svx/postattr.hxx>
#include <swtypes.hxx>
#include <wrtsh.hxx>
#include <view.hxx>
#include <swmodule.hxx>
#ifndef _REDLNDLG_HXX
#define _REDLNACCEPTDLG
#include <redlndlg.hxx>
#endif
#include <swwait.hxx>
#include <uitool.hxx>

#include <helpid.h>
#include <cmdid.h>
#include <misc.hrc>
#include <redlndlg.hrc>
#include <shells.hrc>

// -> #111827#
#include <comcore.hrc>
#include <swundo.hxx>
#include <SwRewriter.hxx>
// <- #111827#

#include <vector>
#include <svx/svxdlg.hxx>
#include <svx/dialogs.hrc>

#include <unomid.h>

#include <docsh.hxx>

#include <IDocumentRedlineAccess.hxx>

/*------------------------------------------------------------------------
	Beschreibung:
------------------------------------------------------------------------*/

SFX_IMPL_MODELESSDIALOG( SwRedlineAcceptChild, FN_REDLINE_ACCEPT )

SV_IMPL_PTRARR(SwRedlineDataParentArr, SwRedlineDataParentPtr)
SV_IMPL_OP_PTRARR_SORT(SwRedlineDataParentSortArr, SwRedlineDataParentPtr)
SV_IMPL_PTRARR(SwRedlineDataChildArr, SwRedlineDataChildPtr)
SV_IMPL_PTRARR(SvLBoxEntryArr, SvLBoxEntryPtr)

static sal_uInt16 nSortMode = 0xffff;
static sal_Bool   bSortDir = sal_True;

/*------------------------------------------------------------------------
	Beschreibung:
------------------------------------------------------------------------*/

SwRedlineAcceptChild::SwRedlineAcceptChild( Window* _pParent,
											sal_uInt16 nId,
											SfxBindings* pBindings,
											SfxChildWinInfo* pInfo ) :
    SwChildWinWrapper( _pParent, nId )
{
    pWindow = new SwModelessRedlineAcceptDlg( pBindings, this, _pParent);

	((SwModelessRedlineAcceptDlg *)pWindow)->Initialize(pInfo);
}

/*--------------------------------------------------------------------
	Beschreibung: Nach Dok-Wechsel Dialog neu initialisieren
 --------------------------------------------------------------------*/

sal_Bool SwRedlineAcceptChild::ReInitDlg(SwDocShell *pDocSh)
{
	sal_Bool bRet;

	if ((bRet = SwChildWinWrapper::ReInitDlg(pDocSh)) == sal_True)	// Sofort aktualisieren, Dok-Wechsel
		((SwModelessRedlineAcceptDlg*)GetWindow())->Activate();

	return bRet;
}

/*------------------------------------------------------------------------
	Beschreibung:
------------------------------------------------------------------------*/

SwModelessRedlineAcceptDlg::SwModelessRedlineAcceptDlg( SfxBindings* _pBindings,
														SwChildWinWrapper* pChild,
                                                        Window *_pParent) :
    SfxModelessDialog(_pBindings, pChild, _pParent, SW_RES(DLG_REDLINE_ACCEPT)),
	pChildWin		(pChild)
{
	pImplDlg = new SwRedlineAcceptDlg(this);

	FreeResource();
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

void SwModelessRedlineAcceptDlg::Activate()
{
	SwView *pView = ::GetActiveView();

	if (!pView)	// Kann passieren, wenn man auf eine andere App umschaltet, wenn
		return;	// vorher eine Listbox im Dialog den Focus hatte (eigentlich THs Bug)

	SwDocShell *pDocSh = pView->GetDocShell();

	if (pChildWin->GetOldDocShell() != pDocSh)
	{	// Dok-Wechsel
		SwWait aWait( *pDocSh, false );
		SwWrtShell* pSh = pView->GetWrtShellPtr();

		pChildWin->SetOldDocShell(pDocSh);	// Rekursion vermeiden (durch Modified-Hdl)

		sal_Bool bMod = pSh->IsModified();
		SfxBoolItem aShow(FN_REDLINE_SHOW, sal_True);
		pSh->GetView().GetViewFrame()->GetDispatcher()->Execute(
			FN_REDLINE_SHOW, SFX_CALLMODE_SYNCHRON|SFX_CALLMODE_RECORD, &aShow, 0L);
		if (!bMod)
			pSh->ResetModified();
		pImplDlg->Init();

		return;
	}

	pImplDlg->Activate();
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

void SwModelessRedlineAcceptDlg::Initialize(SfxChildWinInfo *pInfo)
{
	String aStr;
	if (pInfo != NULL)
		pImplDlg->Initialize(pInfo->aExtraString);

	SfxModelessDialog::Initialize(pInfo);
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

void SwModelessRedlineAcceptDlg::FillInfo(SfxChildWinInfo& rInfo) const
{
	SfxModelessDialog::FillInfo(rInfo);
	pImplDlg->FillInfo(rInfo.aExtraString);
}

/*------------------------------------------------------------------------
	Beschreibung:
------------------------------------------------------------------------*/

void SwModelessRedlineAcceptDlg::Resize()
{
	pImplDlg->Resize();
	SfxModelessDialog::Resize();
}

/*------------------------------------------------------------------------
	Beschreibung:
------------------------------------------------------------------------*/

SwModelessRedlineAcceptDlg::~SwModelessRedlineAcceptDlg()
{
	delete pImplDlg;
}

/*------------------------------------------------------------------------
	Beschreibung:
------------------------------------------------------------------------*/

SwRedlineAcceptDlg::SwRedlineAcceptDlg(Dialog *pParent, sal_Bool bAutoFmt) :
	pParentDlg		(pParent),
    aTabPagesCTRL   (pParent, SW_RES(CTRL_TABPAGES)),
    aPopup          (SW_RES(MN_REDLINE_POPUP)),
    sInserted       (SW_RES(STR_REDLINE_INSERTED)),
    sDeleted        (SW_RES(STR_REDLINE_DELETED)),
    sFormated       (SW_RES(STR_REDLINE_FORMATED)),
    sTableChgd      (SW_RES(STR_REDLINE_TABLECHG)),
    sFmtCollSet     (SW_RES(STR_REDLINE_FMTCOLLSET)),
    sAutoFormat     (SW_RES(STR_REDLINE_AUTOFMT)),
	bOnlyFormatedRedlines( sal_False ),
	bHasReadonlySel	( sal_False ),
	bRedlnAutoFmt	(bAutoFmt),
    bInhibitActivate( false )
{
	aTabPagesCTRL.SetHelpId(HID_REDLINE_CTRL);
	pTPView = aTabPagesCTRL.GetViewPage();
	pTable = pTPView->GetTableControl();

	pTPView->InsertWriterHeader();
	pTPView->SetAcceptClickHdl(LINK(this, SwRedlineAcceptDlg, AcceptHdl));
	pTPView->SetAcceptAllClickHdl(LINK(this, SwRedlineAcceptDlg, AcceptAllHdl));
	pTPView->SetRejectClickHdl(LINK(this, SwRedlineAcceptDlg, RejectHdl));
	pTPView->SetRejectAllClickHdl(LINK(this, SwRedlineAcceptDlg, RejectAllHdl));
	pTPView->SetUndoClickHdl(LINK(this, SwRedlineAcceptDlg, UndoHdl));

	aTabPagesCTRL.GetFilterPage()->SetReadyHdl(LINK(this, SwRedlineAcceptDlg, FilterChangedHdl));

	ListBox *pActLB = aTabPagesCTRL.GetFilterPage()->GetLbAction();
	pActLB->InsertEntry(sInserted);
	pActLB->InsertEntry(sDeleted);
	pActLB->InsertEntry(sFormated);
	pActLB->InsertEntry(sTableChgd);

	if (HasRedlineAutoFmt())
	{
		pActLB->InsertEntry(sFmtCollSet);
		pActLB->InsertEntry(sAutoFormat);
		pTPView->ShowUndo(sal_True);
		pTPView->DisableUndo();		// Noch gibts keine UNDO-Events
	}

	pActLB->SelectEntryPos(0);

	pTable->SetStyle(pTable->GetStyle()|WB_HASLINES|WB_CLIPCHILDREN|WB_HASBUTTONS|WB_HASBUTTONSATROOT|WB_HSCROLL);
	pTable->SetNodeDefaultImages();
	pTable->SetSelectionMode(MULTIPLE_SELECTION);
	pTable->SetHighlightRange(1);

	static long aStaticTabs[]=
	{
		4,10,70,120,170
	};

	pTable->SetTabs(aStaticTabs);

	// Minimalgroesse setzen
	Size aMinSz(aTabPagesCTRL.GetMinSizePixel());
	Point aPos(aTabPagesCTRL.GetPosPixel());

	aMinSz.Width() += (aPos.X() * 2 - 1);
	aMinSz.Height() += (aPos.Y() * 2 - 1);
	pParentDlg->SetMinOutputSizePixel(aMinSz);

	if (pParentDlg->GetOutputSizePixel().Width() < aMinSz.Width())
		pParentDlg->SetOutputSizePixel(Size(aMinSz.Width(), pParentDlg->GetOutputSizePixel().Height()));
	if (pParentDlg->GetOutputSizePixel().Height() < aMinSz.Height())
		pParentDlg->SetOutputSizePixel(Size(pParentDlg->GetOutputSizePixel().Width(), aMinSz.Height()));

	pTable->SortByCol(nSortMode, bSortDir);

	aOldSelectHdl = pTable->GetSelectHdl();
	aOldDeselectHdl = pTable->GetDeselectHdl();
	pTable->SetSelectHdl(LINK(this, SwRedlineAcceptDlg, SelectHdl));
	pTable->SetDeselectHdl(LINK(this, SwRedlineAcceptDlg, DeselectHdl));
	pTable->SetCommandHdl(LINK(this, SwRedlineAcceptDlg, CommandHdl));

	// Flackern der Buttons vermeiden:
	aDeselectTimer.SetTimeout(100);
	aDeselectTimer.SetTimeoutHdl(LINK(this, SwRedlineAcceptDlg, SelectHdl));

	// Mehrfachselektion der selben Texte vermeiden:
	aSelectTimer.SetTimeout(100);
	aSelectTimer.SetTimeoutHdl(LINK(this, SwRedlineAcceptDlg, GotoHdl));
}

/*------------------------------------------------------------------------
	Beschreibung:
------------------------------------------------------------------------*/

SwRedlineAcceptDlg::~SwRedlineAcceptDlg()
{
}

/*------------------------------------------------------------------------
	Beschreibung:
------------------------------------------------------------------------*/

void SwRedlineAcceptDlg::Init(sal_uInt16 nStart)
{
	SwWait aWait( *::GetActiveView()->GetDocShell(), false );
	pTable->SetUpdateMode(sal_False);
	aUsedSeqNo.Remove((sal_uInt16)0, aUsedSeqNo.Count());

	if (nStart)
		RemoveParents(nStart, aRedlineParents.Count() - 1);
	else
	{
		pTable->Clear();
		aRedlineChilds.DeleteAndDestroy(0, aRedlineChilds.Count());
		aRedlineParents.DeleteAndDestroy(nStart, aRedlineParents.Count() - nStart);
	}

	// Parents einfuegen
	InsertParents(nStart);
	InitAuthors();

	pTable->SetUpdateMode(sal_True);
    // #i69618# this moves the list box to the right position, visually
    SvLBoxEntry* pSelEntry = pTable->FirstSelected();
    if( pSelEntry )
        pTable->MakeVisible( pSelEntry, sal_True ); //#i70937#, force the scroll
}

/*------------------------------------------------------------------------
	Beschreibung:
------------------------------------------------------------------------*/

void SwRedlineAcceptDlg::InitAuthors()
{
	SwWrtShell* pSh = ::GetActiveView()->GetWrtShellPtr();

	SvxTPFilter *pFilterPage = aTabPagesCTRL.GetFilterPage();

	String sAuthor;
	SvStringsSortDtor aStrings;
	String sOldAuthor(pFilterPage->GetSelectedAuthor());
	pFilterPage->ClearAuthors();

	String sParent;
	const sal_uInt16 nRedlineCount = pSh->GetRedlineCount();

	bOnlyFormatedRedlines = sal_True;
	bHasReadonlySel = sal_False;
	sal_Bool bIsNotFormated = sal_False;
	sal_uInt16 i;

	// Autoren ermitteln
	for ( i = 0; i < nRedlineCount; i++)
	{
		const SwRedline& rRedln = pSh->GetRedline(i);

		if( bOnlyFormatedRedlines && nsRedlineType_t::REDLINE_FORMAT != rRedln.GetType() )
			bOnlyFormatedRedlines = sal_False;

//JP 27.9.2001: make no sense if we handle readonly sections
//		if( !bHasReadonlySel && rRedln.HasReadonlySel() )
//			bHasReadonlySel = sal_True;

		String *pAuthor = new String(rRedln.GetAuthorString());
		if (!aStrings.Insert(pAuthor))
			delete pAuthor;

		for (sal_uInt16 nStack = 1; nStack < rRedln.GetStackCount(); nStack++)
		{
			pAuthor = new String(rRedln.GetAuthorString(nStack));
			if (!aStrings.Insert(pAuthor))
				delete pAuthor;
		}
	}

	for (i = 0; i < aStrings.Count(); i++)
		pFilterPage->InsertAuthor(*aStrings[i]);

	if (pFilterPage->SelectAuthor(sOldAuthor) == LISTBOX_ENTRY_NOTFOUND && aStrings.Count())
		pFilterPage->SelectAuthor(*aStrings[0]);

    sal_Bool bEnable = pTable->GetEntryCount() != 0 && !pSh->getIDocumentRedlineAccess()->GetRedlinePassword().getLength();
	sal_Bool bSel = pTable->FirstSelected() != 0;

    {
        SvLBoxEntry* pSelEntry = pTable->FirstSelected();
        while (pSelEntry)
        {
            const sal_uInt16 nPos = GetRedlinePos(*pSelEntry);
            if ( nPos < nRedlineCount )
            {
                const SwRedline& rRedln = pSh->GetRedline( nPos );
                bIsNotFormated |= nsRedlineType_t::REDLINE_FORMAT != rRedln.GetType();
            }
            pSelEntry = pTable->NextSelected(pSelEntry);
        }
    }

	pTPView->EnableAccept( bEnable && bSel );
	pTPView->EnableReject( bEnable && bIsNotFormated && bSel );
	pTPView->EnableAcceptAll( bEnable && !bHasReadonlySel );
	pTPView->EnableRejectAll( bEnable && !bHasReadonlySel &&
								!bOnlyFormatedRedlines );
}

/*------------------------------------------------------------------------
	Beschreibung:
------------------------------------------------------------------------*/

String SwRedlineAcceptDlg::GetRedlineText( const SwRedline& rRedln,
										DateTime &rDateTime, sal_uInt16 nStack)
{
	String sEntry(GetActionText(rRedln, nStack));
	sEntry += '\t';
	sEntry += rRedln.GetAuthorString(nStack);
	sEntry += '\t';

	const DateTime &rDT = rRedln.GetTimeStamp(nStack);
	rDateTime = rDT;

	sEntry += GetAppLangDateTimeString( rDT );
	sEntry += '\t';

	sEntry += rRedln.GetComment(nStack);

	return sEntry;
}

/*------------------------------------------------------------------------
	Beschreibung:
------------------------------------------------------------------------*/

const String &SwRedlineAcceptDlg::GetActionText(const SwRedline& rRedln, sal_uInt16 nStack)
{
	switch( rRedln.GetType(nStack) )
	{
		case nsRedlineType_t::REDLINE_INSERT:	return sInserted;
		case nsRedlineType_t::REDLINE_DELETE:	return sDeleted;
		case nsRedlineType_t::REDLINE_FORMAT:	return sFormated;
		case nsRedlineType_t::REDLINE_TABLE: 	return sTableChgd;
		case nsRedlineType_t::REDLINE_FMTCOLL: 	return sFmtCollSet;
        default:;//prevent warning
	}

	return aEmptyStr;
}

/*------------------------------------------------------------------------
	Beschreibung:
------------------------------------------------------------------------*/

void SwRedlineAcceptDlg::Resize()
{
	Size aSz(pParentDlg->GetOutputSizePixel());

	Point aPos(aTabPagesCTRL.GetPosPixel());

	aSz.Width() -= (aPos.X() * 2 - 1);
	aSz.Height() -= (aPos.Y() * 2 - 1);

	aTabPagesCTRL.SetOutputSizePixel(aSz);
}

/*--------------------------------------------------------------------
	Beschreibung: Nach Aktivierung neu initialisieren
 --------------------------------------------------------------------*/

void SwRedlineAcceptDlg::Activate()
{
    // prevent update if flag is set (#102547#)
    if( bInhibitActivate )
        return;

	SwView *pView = ::GetActiveView();
	SwWait aWait( *pView->GetDocShell(), false );

	aUsedSeqNo.Remove((sal_uInt16)0, aUsedSeqNo.Count());

	if (!pView)	// Kann passieren, wenn man auf eine andere App umschaltet, wenn
		return;	// vorher eine Listbox im Dialog den Focus hatte (eigentlich THs Bug)

/*	if (HasRedlineAutoFmt())
	{
		Init();
		return;
	}*/

	// Hat sich was geaendert?
	SwWrtShell* pSh = pView->GetWrtShellPtr();
	sal_uInt16 nCount = pSh->GetRedlineCount();

	// Anzahl und Pointer ueberpruefen
	SwRedlineDataParent *pParent = 0;
	sal_uInt16 i;

	for ( i = 0; i < nCount; i++)
	{
		const SwRedline& rRedln = pSh->GetRedline(i);

		if (i >= aRedlineParents.Count())
		{
			// Neue Eintraege wurden angehaengt
			Init(i);
			return;
		}

		pParent = aRedlineParents[i];
		if (&rRedln.GetRedlineData() != pParent->pData)
		{
			// Redline-Parents wurden eingefuegt, geaendert oder geloescht
			if ((i = CalcDiff(i, sal_False)) == USHRT_MAX)
				return;
			continue;
		}

		const SwRedlineData *pRedlineData = rRedln.GetRedlineData().Next();
		const SwRedlineDataChild *pBackupData = pParent->pNext;

		if (!pRedlineData && pBackupData)
		{
			// Redline-Childs wurden geloescht
			if ((i = CalcDiff(i, sal_True)) == USHRT_MAX)
				return;
			continue;
		}
		else
		{
			while (pRedlineData)
			{
				if (pRedlineData != pBackupData->pChild)
				{
					// Redline-Childs wurden eingefuegt, geaendert oder geloescht
					if ((i = CalcDiff(i, sal_True)) == USHRT_MAX)
						return;
					continue;
				}
				if (pBackupData)
					pBackupData = pBackupData->pNext;
				pRedlineData = pRedlineData->Next();
			}
		}
	}

	if (nCount != aRedlineParents.Count())
	{
		// Redlines wurden am Ende geloescht
		Init(nCount);
		return;
	}

	// Kommentar ueberpruefen
	for (i = 0; i < nCount; i++)
	{
		const SwRedline& rRedln = pSh->GetRedline(i);
		pParent = aRedlineParents[i];

		if(!rRedln.GetComment().Equals(pParent->sComment))
		{
			if (pParent->pTLBParent)
			{
				// Nur Kommentar aktualisieren
				String sComment(rRedln.GetComment());
				sComment.SearchAndReplaceAll((sal_Unicode)_LF,(sal_Unicode)' ');
				pTable->SetEntryText(sComment, pParent->pTLBParent, 3);
			}
			pParent->sComment = rRedln.GetComment();
		}
	}

	InitAuthors();
}

/* -----------------05.06.98 13:06-------------------
 *
 * --------------------------------------------------*/

sal_uInt16 SwRedlineAcceptDlg::CalcDiff(sal_uInt16 nStart, sal_Bool bChild)
{
	if (!nStart)
	{
		Init();
		return USHRT_MAX;
	}

	pTable->SetUpdateMode(sal_False);
	SwView *pView	= ::GetActiveView();
	SwWrtShell* pSh	= pView->GetWrtShellPtr();
    sal_uInt16 nAutoFmt = HasRedlineAutoFmt() ? nsRedlineType_t::REDLINE_FORM_AUTOFMT : 0;
    SwRedlineDataParent *pParent = aRedlineParents[nStart];
	const SwRedline& rRedln = pSh->GetRedline(nStart);

	if (bChild)		// Sollte eigentlich nie vorkommen, aber sicher ist sicher...
	{
		// Alle Childs des Eintrags wegwerfen und neu initialisieren
		SwRedlineDataChildPtr pBackupData = (SwRedlineDataChildPtr)pParent->pNext;
		SwRedlineDataChildPtr pNext;

		while (pBackupData)
		{
			pNext = (SwRedlineDataChildPtr)pBackupData->pNext;
			if (pBackupData->pTLBChild)
                pTable->RemoveEntry(pBackupData->pTLBChild);

			aRedlineChilds.DeleteAndDestroy(aRedlineChilds.GetPos(pBackupData), 1);
			pBackupData = pNext;
		}
		pParent->pNext = 0;

		// Neue Childs einfuegen
		InsertChilds(pParent, rRedln, nAutoFmt);

		pTable->SetUpdateMode(sal_True);
		return nStart;
	}

	// Wurden Eintraege geloescht?
	const SwRedlineData *pRedlineData = &rRedln.GetRedlineData();
	sal_uInt16 i;
	for ( i = nStart + 1; i < aRedlineParents.Count(); i++)
	{
		if (aRedlineParents[i]->pData == pRedlineData)
		{
			// Eintraege von nStart bis i-1 entfernen
			RemoveParents(nStart, i - 1);
			pTable->SetUpdateMode(sal_True);
			return nStart - 1;
		}
	}

	// Wurden Eintraege eingefuegt?
	sal_uInt16 nCount = pSh->GetRedlineCount();
	pRedlineData = aRedlineParents[nStart]->pData;

	for (i = nStart + 1; i < nCount; i++)
	{
		if (&pSh->GetRedline(i).GetRedlineData() == pRedlineData)
		{
			// Eintraege von nStart bis i-1 einfuegen
			InsertParents(nStart, i - 1);
			pTable->SetUpdateMode(sal_True);
			return nStart - 1;
		}
	}

	pTable->SetUpdateMode(sal_True);
	Init(nStart);	// Alle Eintraege bis zum Ende abgleichen
	return USHRT_MAX;
}

/* -----------------05.06.98 13:57-------------------
 *
 * --------------------------------------------------*/

void SwRedlineAcceptDlg::InsertChilds(SwRedlineDataParent *pParent, const SwRedline& rRedln, const sal_uInt16 nAutoFmt)
{
	String sChild;
	SwRedlineDataChild *pLastRedlineChild = 0;
	const SwRedlineData *pRedlineData = &rRedln.GetRedlineData();
	sal_Bool bAutoFmt = (rRedln.GetRealType() & nAutoFmt) != 0;

	const String *pAction = &GetActionText(rRedln);
	sal_Bool bValidParent = !sFilterAction.Len() || sFilterAction == *pAction;
	bValidParent = bValidParent && pTable->IsValidEntry(&rRedln.GetAuthorString(), &rRedln.GetTimeStamp(), &rRedln.GetComment());
	if (nAutoFmt)
	{
		sal_uInt16 nPos;

		if (pParent->pData->GetSeqNo() && !aUsedSeqNo.Insert(pParent, nPos))	// Gibts schon
		{
			if (pParent->pTLBParent)
			{
				pTable->SetEntryText(sAutoFormat, aUsedSeqNo[nPos]->pTLBParent, 0);
                pTable->RemoveEntry(pParent->pTLBParent);
				pParent->pTLBParent = 0;
			}
			return;
		}
		bValidParent = bValidParent && bAutoFmt;
	}
	sal_Bool bValidTree = bValidParent;

	for (sal_uInt16 nStack = 1; nStack < rRedln.GetStackCount(); nStack++)
	{
		pRedlineData = pRedlineData->Next();

		SwRedlineDataChildPtr pRedlineChild = new SwRedlineDataChild;
		pRedlineChild->pChild = pRedlineData;
		aRedlineChilds.Insert(pRedlineChild, aRedlineChilds.Count());

		if ( pLastRedlineChild )
			pLastRedlineChild->pNext = pRedlineChild;
		else
			pParent->pNext = pRedlineChild;

		pAction = &GetActionText(rRedln, nStack);
		sal_Bool bValidChild = !sFilterAction.Len() || sFilterAction == *pAction;
		bValidChild = bValidChild && pTable->IsValidEntry(&rRedln.GetAuthorString(nStack), &rRedln.GetTimeStamp(nStack), &rRedln.GetComment());
		if (nAutoFmt)
			bValidChild = bValidChild && bAutoFmt;
		bValidTree |= bValidChild;

		if (bValidChild)
		{
			RedlinData *pData = new RedlinData;
			pData->pData = pRedlineChild;
			pData->bDisabled = sal_True;
			sChild = GetRedlineText(rRedln, pData->aDateTime, nStack);

			SvLBoxEntry* pChild = pTable->InsertEntry(sChild, pData, pParent->pTLBParent);

			pRedlineChild->pTLBChild = pChild;
			if (!bValidParent)
				pTable->Expand(pParent->pTLBParent);
		}
		else
			pRedlineChild->pTLBChild = 0;

		pLastRedlineChild = pRedlineChild;
	}

	if (pLastRedlineChild)
		pLastRedlineChild->pNext = 0;

	if (!bValidTree && pParent->pTLBParent)
	{
        pTable->RemoveEntry(pParent->pTLBParent);
		pParent->pTLBParent = 0;
		if (nAutoFmt)
			aUsedSeqNo.Remove(pParent);
	}
}

/* -----------------05.06.98 15:20-------------------
 *
 * --------------------------------------------------*/

void SwRedlineAcceptDlg::RemoveParents(sal_uInt16 nStart, sal_uInt16 nEnd)
{
	SwWrtShell* pSh = ::GetActiveView()->GetWrtShellPtr();
	sal_uInt16 nCount = pSh->GetRedlineCount();

	SvLBoxEntryArr aLBoxArr;

	// Wegen Bug der TLB, die bei Remove den SelectHandler IMMER ruft:
	pTable->SetSelectHdl(aOldSelectHdl);
	pTable->SetDeselectHdl(aOldDeselectHdl);
	sal_Bool bChildsRemoved = sal_False;
	pTable->SelectAll(sal_False);

	// Hinter dem letzten Eintrag Cursor setzen, da sonst Performance-Problem in TLB.
	// TLB wuerde sonst bei jedem Remove den Cursor erneut umsetzen (teuer)
	sal_uInt16 nPos = Min((sal_uInt16)nCount, (sal_uInt16)aRedlineParents.Count());
	SvLBoxEntry *pCurEntry = NULL;
	while( ( pCurEntry == NULL ) && ( nPos > 0 ) )
	{
		--nPos;
		pCurEntry = aRedlineParents[nPos]->pTLBParent;
	}

	if (pCurEntry)
		pTable->SetCurEntry(pCurEntry);

	SvLBoxTreeList*	pModel = pTable->GetModel();

	for (sal_uInt16 i = nStart; i <= nEnd; i++)
	{
		if (!bChildsRemoved && aRedlineParents[i]->pNext)
		{
			SwRedlineDataChildPtr pChildPtr = (SwRedlineDataChildPtr)aRedlineParents[i]->pNext;
            sal_uInt16 nChildPos = aRedlineChilds.GetPos(pChildPtr);

            if (nChildPos != USHRT_MAX)
			{
				sal_uInt16 nChilds = 0;

				while (pChildPtr)
				{
					pChildPtr = (SwRedlineDataChildPtr)pChildPtr->pNext;
					nChilds++;
				}

                aRedlineChilds.DeleteAndDestroy(nChildPos, nChilds);
				bChildsRemoved = sal_True;
			}
		}
		SvLBoxEntry *pEntry = aRedlineParents[i]->pTLBParent;
		if (pEntry)
		{
            long nIdx = aLBoxArr.Count() - 1L;
			sal_uLong nAbsPos = pModel->GetAbsPos(pEntry);
			while (nIdx >= 0 &&
                    pModel->GetAbsPos(aLBoxArr[ static_cast< sal_uInt16 >(nIdx) ]) > nAbsPos)
				nIdx--;
            aLBoxArr.Insert( pEntry, static_cast< sal_uInt16 >(++nIdx) );
		}
	}

	// TLB von hinten abraeumen
	long nIdx = (long)aLBoxArr.Count() - 1L;
	while (nIdx >= 0)
        pTable->RemoveEntry(aLBoxArr[ static_cast< sal_uInt16 >(nIdx--) ]);

	pTable->SetSelectHdl(LINK(this, SwRedlineAcceptDlg, SelectHdl));
	pTable->SetDeselectHdl(LINK(this, SwRedlineAcceptDlg, DeselectHdl));
	// Durch Remove wurde leider wieder dauernd von der TLB selektiert...
	pTable->SelectAll(sal_False);

	aRedlineParents.DeleteAndDestroy( nStart, nEnd - nStart + 1);
}

/* -----------------05.06.98 15:20-------------------
 *
 * --------------------------------------------------*/

void SwRedlineAcceptDlg::InsertParents(sal_uInt16 nStart, sal_uInt16 nEnd)
{
	SwView *pView	= ::GetActiveView();
	SwWrtShell* pSh	= pView->GetWrtShellPtr();
	sal_uInt16 nAutoFmt	= HasRedlineAutoFmt() ? nsRedlineType_t::REDLINE_FORM_AUTOFMT : 0;

	String sParent;
	const sal_uInt16 nCount = pSh->GetRedlineCount();
	nEnd = Min((sal_uInt16)nEnd, (sal_uInt16)(nCount - 1));	// Handelt auch nEnd=USHRT_MAX (bis zum Ende) ab

	if (nEnd == USHRT_MAX)
		return;		// Keine Redlines im Dokument

	RedlinData *pData;
    SvLBoxEntry *pParent;
	SwRedlineDataParentPtr pRedlineParent;
	const SwRedline* pCurrRedline;
	if( !nStart && !pTable->FirstSelected() )
	{
		pCurrRedline = pSh->GetCurrRedline();
		if( !pCurrRedline )
		{
			pSh->SwCrsrShell::Push();
			if( 0 == (pCurrRedline = pSh->SelNextRedline()))
				pCurrRedline = pSh->SelPrevRedline();
			pSh->SwCrsrShell::Pop( sal_False );
		}
	}
	else
		pCurrRedline = 0;

	for (sal_uInt16 i = nStart; i <= nEnd; i++)
	{
		const SwRedline& rRedln = pSh->GetRedline(i);
		const SwRedlineData *pRedlineData = &rRedln.GetRedlineData();

		pRedlineParent = new SwRedlineDataParent;
		pRedlineParent->pData	 = pRedlineData;
		pRedlineParent->pNext	 = 0;
		String sComment(rRedln.GetComment());
		sComment.SearchAndReplaceAll((sal_Unicode)_LF,(sal_Unicode)' ');
		pRedlineParent->sComment = sComment;
		aRedlineParents.Insert(pRedlineParent, i);

		pData = new RedlinData;
		pData->pData = pRedlineParent;
		pData->bDisabled = sal_False;

		sParent = GetRedlineText(rRedln, pData->aDateTime);
		pParent = pTable->InsertEntry(sParent, pData, 0, i);
		if( pCurrRedline == &rRedln )
		{
			pTable->SetCurEntry( pParent );
			pTable->Select( pParent );
			pTable->MakeVisible( pParent );
		}

		pRedlineParent->pTLBParent = pParent;

		InsertChilds(pRedlineParent, rRedln, nAutoFmt);
	}
}

/* -----------------05.06.98 13:06-------------------
 *
 * --------------------------------------------------*/

void SwRedlineAcceptDlg::CallAcceptReject( sal_Bool bSelect, sal_Bool bAccept )
{
	SwWrtShell* pSh = ::GetActiveView()->GetWrtShellPtr();
	SvLBoxEntry* pEntry = bSelect ? pTable->FirstSelected() : pTable->First();
	sal_uLong nPos = LONG_MAX;

    typedef std::vector<SvLBoxEntry*> ListBoxEntries_t;
    ListBoxEntries_t aRedlines;

    // don't activate
    DBG_ASSERT( bInhibitActivate == false,
                "recursive call of CallAcceptReject?");
    bInhibitActivate = true;

    // collect redlines-to-be-accepted/rejected in aRedlines vector
	while( pEntry )
	{
		if( !pTable->GetParent( pEntry ) )
		{
			if( bSelect && LONG_MAX == nPos )
				nPos = pTable->GetModel()->GetAbsPos( pEntry );

			RedlinData *pData = (RedlinData *)pEntry->GetUserData();

			if( !pData->bDisabled )
				aRedlines.push_back( pEntry );
		}

		pEntry = bSelect ? pTable->NextSelected(pEntry) : pTable->Next(pEntry);
	}

	sal_Bool (SwEditShell:: *FnAccRej)( sal_uInt16 ) = &SwEditShell::AcceptRedline;
	if( !bAccept )
		FnAccRej = &SwEditShell::RejectRedline;

	SwWait aWait( *pSh->GetView().GetDocShell(), true );
	pSh->StartAction();

    // #111827#
    if (aRedlines.size() > 1)
    {
        String aTmpStr;
        {
            SwRewriter aRewriter;
            aRewriter.AddRule(UNDO_ARG1,
                              String::CreateFromInt32(aRedlines.size()));
            aTmpStr = aRewriter.Apply(String(SW_RES(STR_N_REDLINES)));
        }

        SwRewriter aRewriter;
        aRewriter.AddRule(UNDO_ARG1, aTmpStr);

        pSh->StartUndo(bAccept? UNDO_ACCEPT_REDLINE : UNDO_REJECT_REDLINE,
                       &aRewriter);
    }

    // accept/reject the redlines in aRedlines. The absolute
    // position may change during the process (e.g. when two redlines
    // are merged in result of another one being deleted), so the
    // position must be resolved late and checked before using it.
    // (cf #102547#)
    ListBoxEntries_t::iterator aEnd = aRedlines.end();
	for( ListBoxEntries_t::iterator aIter = aRedlines.begin();
         aIter != aEnd;
         aIter++ )
    {
        const sal_uInt16 nPosition = GetRedlinePos( **aIter );
        if( nPosition != USHRT_MAX )
            (pSh->*FnAccRej)( nPosition );
    }

    // #111827#
    if (aRedlines.size() > 1)
    {
        pSh->EndUndo();
    }

	pSh->EndAction();

    bInhibitActivate = false;
	Activate();

	if( ULONG_MAX != nPos && pTable->GetEntryCount() )
	{
		if( nPos >= pTable->GetEntryCount() )
			nPos = pTable->GetEntryCount() - 1;
		pEntry = pTable->GetEntry( nPos );
		if( !pEntry && nPos-- )
			pEntry = pTable->GetEntry( nPos );
		if( pEntry )
		{
			pTable->Select( pEntry );
			pTable->MakeVisible( pEntry );
			pTable->SetCurEntry(pEntry);
		}
	}
	pTPView->EnableUndo();
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

sal_uInt16 SwRedlineAcceptDlg::GetRedlinePos( const SvLBoxEntry& rEntry ) const
{
	SwWrtShell* pSh = ::GetActiveView()->GetWrtShellPtr();
	return pSh->FindRedlineOfData( *((SwRedlineDataParent*)((RedlinData *)
									rEntry.GetUserData())->pData)->pData );
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwRedlineAcceptDlg, AcceptHdl, void*, EMPTYARG)
{
	CallAcceptReject( sal_True, sal_True );
	return 0;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwRedlineAcceptDlg, AcceptAllHdl, void*, EMPTYARG )
{
	CallAcceptReject( sal_False, sal_True );
	return 0;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwRedlineAcceptDlg, RejectHdl, void*, EMPTYARG )
{
	CallAcceptReject( sal_True, sal_False );
	return 0;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwRedlineAcceptDlg, RejectAllHdl, void*, EMPTYARG )
{
	CallAcceptReject( sal_False, sal_False );
	return 0;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwRedlineAcceptDlg, UndoHdl, void*, EMPTYARG )
{
	SwView * pView = ::GetActiveView();
	pView->GetViewFrame()->GetDispatcher()->
				Execute(SID_UNDO, SFX_CALLMODE_SYNCHRON);
	pTPView->EnableUndo(pView->GetSlotState(SID_UNDO) != 0);

	Activate();

	return 0;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwRedlineAcceptDlg, FilterChangedHdl, void*, EMPTYARG )
{
	SvxTPFilter *pFilterTP = aTabPagesCTRL.GetFilterPage();

	if (pFilterTP->IsAction())
		sFilterAction = pFilterTP->GetLbAction()->GetSelectEntry();
	else
		sFilterAction = aEmptyStr;

	Init();

	return 0;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwRedlineAcceptDlg, DeselectHdl, void*, EMPTYARG )
{
	// Flackern der Buttons vermeiden:
	aDeselectTimer.Start();

	return 0;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwRedlineAcceptDlg, SelectHdl, void*, EMPTYARG )
{
	aDeselectTimer.Stop();
	aSelectTimer.Start();

	return 0;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwRedlineAcceptDlg, GotoHdl, void*, EMPTYARG )
{
	SwWrtShell* pSh = ::GetActiveView()->GetWrtShellPtr();
	aSelectTimer.Stop();

	sal_Bool bIsNotFormated = sal_False;
	sal_Bool bSel = sal_False;

    //#98883# don't select redlines while the dialog is not focussed
    //#107938# But not only ask pTable if it has the focus. To move
    //         the selection to the selected redline any child of pParentDlg
    //         may the focus.
    SvLBoxEntry* pSelEntry = 0;

    if (pParentDlg->HasChildPathFocus())
        pSelEntry = pTable->FirstSelected();

    if( pSelEntry )
	{
		SvLBoxEntry* pActEntry = pSelEntry;
		pSh->StartAction();
		pSh->EnterStdMode();
		pSh->SetCareWin(pParentDlg);

		while (pSelEntry)
		{
			if (pTable->GetParent(pSelEntry))
			{
				pActEntry = pTable->GetParent(pSelEntry);

				if (pTable->IsSelected(pActEntry))
				{
					pSelEntry = pActEntry = pTable->NextSelected(pSelEntry);
					continue;	// Nicht zweimal selektieren
				}
			}
			else
				bSel = sal_True;

            // #98864# find the selected redline (ignore, if the redline is already gone)
            const sal_uInt16 nPos = GetRedlinePos(*pActEntry);
            if ( nPos < pSh->GetRedlineCount() )
            {

                const SwRedline& rRedln = pSh->GetRedline( nPos );
                bIsNotFormated |= nsRedlineType_t::REDLINE_FORMAT != rRedln.GetType();

                if (pSh->GotoRedline(nPos, sal_True))
                {
                    pSh->SetInSelect();
                    pSh->EnterAddMode();
                }
            }

            pSelEntry = pActEntry = pTable->NextSelected(pSelEntry);
        }

        pSh->LeaveAddMode();
        pSh->EndAction();
        pSh->SetCareWin(NULL);
    }
    sal_Bool bEnable = !pSh->getIDocumentRedlineAccess()->GetRedlinePassword().getLength();
    pTPView->EnableAccept( bEnable && bSel /*&& !bReadonlySel*/ );
    pTPView->EnableReject( bEnable && bSel && bIsNotFormated /*&& !bReadonlySel*/ );
    pTPView->EnableRejectAll( bEnable && !bOnlyFormatedRedlines && !bHasReadonlySel );

	return 0;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

IMPL_LINK( SwRedlineAcceptDlg, CommandHdl, void*, EMPTYARG )
{
	const CommandEvent aCEvt(pTable->GetCommandEvent());

	switch ( aCEvt.GetCommand() )
	{
		case COMMAND_CONTEXTMENU:
		{
			SwWrtShell* pSh = ::GetActiveView()->GetWrtShellPtr();
			SvLBoxEntry* pEntry = pTable->FirstSelected();
			const SwRedline *pRed = 0;

			if (pEntry)
			{
				SvLBoxEntry* pTopEntry = pEntry;

				if (pTable->GetParent(pEntry))
					pTopEntry = pTable->GetParent(pEntry);

				const sal_uInt16 nPos = GetRedlinePos(*pTopEntry);

				// Bei geschuetzten Bereichen kommentieren disablen
				if ((pRed = pSh->GotoRedline(nPos, sal_True)) != 0)
				{
					if( pSh->IsCrsrPtAtEnd() )
						pSh->SwapPam();
					pSh->SetInSelect();
				}
			}

			aPopup.EnableItem( MN_EDIT_COMMENT, pEntry && pRed &&
											!pTable->GetParent(pEntry) &&
											!pTable->NextSelected(pEntry)
//JP 27.9.2001: make no sense if we handle readonly sections
//											&& pRed->HasReadonlySel()
											);

			aPopup.EnableItem( MN_SUB_SORT, pTable->First() != 0 );
			sal_uInt16 nColumn = pTable->GetSortedCol();
			if (nColumn == 0xffff)
				nColumn = 4;

			PopupMenu *pSubMenu = aPopup.GetPopupMenu(MN_SUB_SORT);
			if (pSubMenu)
			{
				for (sal_uInt16 i = MN_SORT_ACTION; i < MN_SORT_ACTION + 5; i++)
					pSubMenu->CheckItem(i, sal_False);

				pSubMenu->CheckItem(nColumn + MN_SORT_ACTION);
			}

			sal_uInt16 nRet = aPopup.Execute(pTable, aCEvt.GetMousePosPixel());

            switch( nRet )
            {
            case MN_EDIT_COMMENT:
                {
                    String sComment;
                    if (pEntry)
                    {
                        if (pTable->GetParent(pEntry))
                            pEntry = pTable->GetParent(pEntry);

                        const sal_uInt16 nPos = GetRedlinePos(*pEntry);
                        if ( nPos >= pSh->GetRedlineCount() )
                        {
                            break;
                        }
                        const SwRedline &rRedline = pSh->GetRedline(nPos);


                        /* enable again once we have redline comments in the margin
                        sComment = rRedline.GetComment();
                        if ( sComment == String(::rtl::OUString::createFromAscii("")) )
                        GetActiveView()->GetDocShell()->Broadcast(SwRedlineHint(&rRedline,SWREDLINE_INSERTED));
                        const_cast<SwRedline&>(rRedline).Broadcast(SwRedlineHint(&rRedline,SWREDLINE_FOCUS));
                        */

                        sComment = rRedline.GetComment();
                        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                        DBG_ASSERT(pFact, "Dialogdiet fail!");
                        ::DialogGetRanges fnGetRange = pFact->GetDialogGetRangesFunc( RID_SVXDLG_POSTIT );
                        DBG_ASSERT(fnGetRange, "Dialogdiet fail! GetRanges()");
                        SfxItemSet aSet( pSh->GetAttrPool(), fnGetRange() );

                        aSet.Put(SvxPostItTextItem(sComment.ConvertLineEnd(), SID_ATTR_POSTIT_TEXT));
                        aSet.Put(SvxPostItAuthorItem(rRedline.GetAuthorString(), SID_ATTR_POSTIT_AUTHOR));

                        aSet.Put(SvxPostItDateItem( GetAppLangDateTimeString(
                            rRedline.GetRedlineData().GetTimeStamp() ),
                            SID_ATTR_POSTIT_DATE ));

                        AbstractSvxPostItDialog* pDlg = pFact->CreateSvxPostItDialog( pParentDlg, aSet, sal_False );
                        DBG_ASSERT(pDlg, "Dialogdiet fail!");

                        pDlg->HideAuthor();

                        sal_uInt16 nResId = 0;
                        switch( rRedline.GetType() )
                        {
                        case nsRedlineType_t::REDLINE_INSERT:
                            nResId = STR_REDLINE_INSERTED;
                            break;
                        case nsRedlineType_t::REDLINE_DELETE:
                            nResId = STR_REDLINE_DELETED;
                            break;
                        case nsRedlineType_t::REDLINE_FORMAT:
                            nResId = STR_REDLINE_FORMATED;
                            break;
                        case nsRedlineType_t::REDLINE_TABLE:
                            nResId = STR_REDLINE_TABLECHG;
                            break;
                        default:;//prevent warning
                        }
                        String sTitle(SW_RES(STR_REDLINE_COMMENT));
                        if( nResId )
                            sTitle += SW_RESSTR( nResId );
                        pDlg->SetText(sTitle);

                        pSh->SetCareWin(pDlg->GetWindow());

                        if ( pDlg->Execute() == RET_OK )
                        {
                            const SfxItemSet* pOutSet = pDlg->GetOutputItemSet();
                            String sMsg(((const SvxPostItTextItem&)pOutSet->Get(SID_ATTR_POSTIT_TEXT)).GetValue());

                            // Kommentar einfuegen bzw aendern
                            pSh->SetRedlineComment(sMsg);
                            sMsg.SearchAndReplaceAll((sal_Unicode)_LF,(sal_Unicode)' ');
                            pTable->SetEntryText(sMsg, pEntry, 3);
                        }

                        delete pDlg;
                        pSh->SetCareWin(NULL);
                    }

                }
                break;

			case MN_SORT_ACTION:
			case MN_SORT_AUTHOR:
			case MN_SORT_DATE:
			case MN_SORT_COMMENT:
			case MN_SORT_POSITION:
				{
					bSortDir = sal_True;
					if (nRet - MN_SORT_ACTION == 4 && pTable->GetSortedCol() == 0xffff)
						break;	// Haben wir schon

					nSortMode = nRet - MN_SORT_ACTION;
					if (nSortMode == 4)
						nSortMode = 0xffff;	// unsortiert bzw sortiert nach Position

					if (pTable->GetSortedCol() == nSortMode)
						bSortDir = !pTable->GetSortDirection();

					SwWait aWait( *::GetActiveView()->GetDocShell(), false );
					pTable->SortByCol(nSortMode, bSortDir);
					if (nSortMode == 0xffff)
						Init();				// Alles neu fuellen
				}
				break;
			}
		}
		break;
	}

	return 0;
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

void SwRedlineAcceptDlg::Initialize(const String& rExtraData)
{
	if (rExtraData.Len())
	{
		sal_uInt16 nPos = rExtraData.Search(C2S("AcceptChgDat:"));

		// Versuche, den Alignment-String "ALIGN:(...)" einzulesen; wenn
		// er nicht vorhanden ist, liegt eine "altere Version vor
		if (nPos != STRING_NOTFOUND)
		{
			sal_uInt16 n1 = rExtraData.Search('(', nPos);
			if (n1 != STRING_NOTFOUND)
			{
				sal_uInt16 n2 = rExtraData.Search(')', n1);
				if (n2 != STRING_NOTFOUND)
				{
					// Alignment-String herausschneiden
					String aStr = rExtraData.Copy(nPos, n2 - nPos + 1);
					aStr.Erase(0, n1 - nPos + 1);

					if (aStr.Len())
					{
                        sal_uInt16 nCount = static_cast< sal_uInt16 >(aStr.ToInt32());

						for (sal_uInt16 i = 0; i < nCount; i++)
						{
                            sal_uInt16 n3 = aStr.Search(';');
                            aStr.Erase(0, n3 + 1);
							pTable->SetTab(i, aStr.ToInt32(), MAP_PIXEL);
						}
					}
				}
			}
		}
	}
}

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

void SwRedlineAcceptDlg::FillInfo(String &rExtraData) const
{
	rExtraData.AppendAscii("AcceptChgDat:(");

	sal_uInt16	nCount = pTable->TabCount();

	rExtraData += String::CreateFromInt32(nCount);
	rExtraData += ';';
	for(sal_uInt16 i = 0; i < nCount; i++)
	{
		rExtraData += String::CreateFromInt32( pTable->GetTab(i) );
		rExtraData += ';';
	}
	rExtraData += ')';
}
