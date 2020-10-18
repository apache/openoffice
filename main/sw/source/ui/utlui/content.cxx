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

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <svl/svstdarr.hxx>
#endif
#include <svl/urlbmk.hxx>
#include <tools/urlobj.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/dispatch.hxx>
#include <vcl/help.hxx>
#include <sot/formats.hxx>
#include <vcl/sound.hxx>
#include <uiitems.hxx>
#include <fmtinfmt.hxx>
#include <txtinet.hxx>
#include <fmtfld.hxx>
#include <swmodule.hxx>
#include <wrtsh.hxx>
#include <view.hxx>
#include <errhdl.hxx>
#include <docsh.hxx>
#include <content.hxx>
#include <frmfmt.hxx>
#include <fldbas.hxx>
#include <txtatr.hxx>
#include <IMark.hxx>
#include <section.hxx>
#include <tox.hxx>
#define NAVIPI_CXX
#include <navipi.hxx>
#include <navicont.hxx>
#include <navicfg.hxx>
#include <edtwin.hxx>
#include <doc.hxx>
#include <unotools.hxx>
#include <crsskip.hxx>
#include <cmdid.h>
#include <helpid.h>
#include <navipi.hrc>
#include <utlui.hrc>
#include <misc.hrc>
#include <comcore.hrc>
#include <com/sun/star/text/XTextSectionsSupplier.hpp>
#include <com/sun/star/text/XTextGraphicObjectsSupplier.hpp>
#include <com/sun/star/text/XTextTablesSupplier.hpp>
#include <com/sun/star/text/XDocumentIndexesSupplier.hpp>
#include <com/sun/star/text/XDocumentIndex.hpp>
#include <com/sun/star/text/XBookmarksSupplier.hpp>
#include <com/sun/star/text/XTextEmbeddedObjectsSupplier.hpp>
#include <com/sun/star/text/XTextFramesSupplier.hpp>
#include <dcontact.hxx>
#include <svx/svdogrp.hxx>
#include <svx/svdmodel.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdview.hxx>
#include <vcl/scrbar.hxx>
#include <comcore.hrc>
#include <SwRewriter.hxx>
#include <hints.hxx>
#include <numrule.hxx>
#include <swundo.hxx>
#include <ndtxt.hxx>
#include <fmtcntnt.hxx>
#include <PostItMgr.hxx>
#include <postithelper.hxx>
#include <redline.hxx>
#include <docary.hxx>
#include "swabstdlg.hxx"
#include "globals.hrc"
#include <unomid.h>
#include <drawdoc.hxx>

#define CTYPE_CNT	0
#define CTYPE_CTT	1

using namespace ::std;
using namespace ::com::sun::star;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::container;


#define NAVI_BOOKMARK_DELIM		(sal_Unicode)1

/***************************************************************************

***************************************************************************/

typedef SwContent* SwContentPtr;
SV_DECL_PTRARR_SORT_DEL( SwContentArr, SwContentPtr, 0,4)
SV_IMPL_OP_PTRARR_SORT(SwContentArr, SwContentPtr)

sal_Bool SwContentTree::bIsInDrag = sal_False;


namespace
{
    static sal_Bool lcl_IsContent(SvLBoxEntry* pEntry)
    {
        return ((SwTypeNumber*)pEntry->GetUserData())->GetTypeId() == CTYPE_CNT;
    }


    static sal_Bool lcl_IsContentType(SvLBoxEntry* pEntry)
    {
        return ((SwTypeNumber*)pEntry->GetUserData())->GetTypeId() == CTYPE_CTT;
    }


    static sal_Bool lcl_FindShell(SwWrtShell* pShell)
    {
        sal_Bool bFound = sal_False;
        SwView *pView = SwModule::GetFirstView();
        while (pView)
        {
            if(pShell == &pView->GetWrtShell())
            {
                bFound = sal_True;
                break;
            }
            pView = SwModule::GetNextView(pView);
        }
        return bFound;
    }

    static bool lcl_IsUiVisibleBookmark(const IDocumentMarkAccess::pMark_t& rpMark)
    {
        return IDocumentMarkAccess::GetType(*rpMark) == IDocumentMarkAccess::BOOKMARK;
    }
}

/***************************************************************************
	Beschreibung: Inhalt, enthaelt Namen und Verweis auf den Inhaltstyp
***************************************************************************/


SwContent::SwContent(const SwContentType* pCnt, const String& rName, long nYPos) :
	SwTypeNumber(CTYPE_CNT),
	pParent(pCnt),
	sContentName(rName),
	nYPosition(nYPos),
	bInvisible(sal_False)
{
}


sal_uInt8	SwTypeNumber::GetTypeId()
{
	return nTypeId;
}

SwTypeNumber::~SwTypeNumber()
{
}

sal_Bool SwContent::IsProtect() const
{
	return sal_False;
}

sal_Bool SwPostItContent::IsProtect() const
{
	if (mbPostIt)
		return pFld->IsProtect();
	else
		return false;
}

sal_Bool SwURLFieldContent::IsProtect() const
{
	return pINetAttr->IsProtect();
}

SwGraphicContent::~SwGraphicContent()
{
}
SwTOXBaseContent::~SwTOXBaseContent()
{
}

/***************************************************************************
	Beschreibung: 	Inhaltstyp, kennt seine Inhalte und die WrtShell
***************************************************************************/


SwContentType::SwContentType(SwWrtShell* pShell, sal_uInt16 nType, sal_uInt8 nLevel) :
	SwTypeNumber(CTYPE_CTT),
	pWrtShell(pShell),
	pMember(0),
	sContentTypeName(SW_RES(STR_CONTENT_TYPE_FIRST + nType)),
	sSingleContentTypeName(SW_RES(STR_CONTENT_TYPE_SINGLE_FIRST + nType)),
	nMemberCount(0),
	nContentType(nType),
	nOutlineLevel(nLevel),
	bMemberFilled(sal_False),
	bDataValid(sal_False),
	bEdit(sal_False),
	bDelete(sal_True)
{
	Init();
}

/***************************************************************************
	Beschreibung: 	Initialisierung
***************************************************************************/


void SwContentType::Init(sal_Bool* pbInvalidateWindow)
{
	// wenn sich der MemberCount aendert ...
	sal_uInt16 nOldMemberCount = nMemberCount;
	nMemberCount = 0;
	switch(nContentType)
	{
		case CONTENT_TYPE_OUTLINE	:
		{
			sTypeToken = C2S(pMarkToOutline);
            sal_uInt16 nOutlineCount = nMemberCount =
                static_cast<sal_uInt16>(pWrtShell->getIDocumentOutlineNodesAccess()->getOutlineNodesCount());
			if(nOutlineLevel < MAXLEVEL)
			{
				for(sal_uInt16 j = 0; j < nOutlineCount; j++)
				{
                    if(pWrtShell->getIDocumentOutlineNodesAccess()->getOutlineLevel(j) > nOutlineLevel )
						nMemberCount --;
				}
			}
			bDelete = sal_False;
		}
		break;

		case CONTENT_TYPE_TABLE 	:
			sTypeToken = C2S(pMarkToTable);
			nMemberCount = pWrtShell->GetTblFrmFmtCount(sal_True);
			bEdit = sal_True;
		break;

		case CONTENT_TYPE_FRAME     :
		case CONTENT_TYPE_GRAPHIC   :
		case CONTENT_TYPE_OLE       :
		{
			FlyCntType eType = FLYCNTTYPE_FRM;
			sTypeToken = C2S(pMarkToFrame);
			if(nContentType == CONTENT_TYPE_OLE)
			{
				eType = FLYCNTTYPE_OLE;
				sTypeToken = C2S(pMarkToOLE);
			}
			else if(nContentType == CONTENT_TYPE_GRAPHIC)
			{
				eType = FLYCNTTYPE_GRF;
				sTypeToken = C2S(pMarkToGraphic);
			}
			nMemberCount = pWrtShell->GetFlyCount(eType);
			bEdit = sal_True;
		}
		break;
        case CONTENT_TYPE_BOOKMARK:
        {
            IDocumentMarkAccess* const pMarkAccess = pWrtShell->getIDocumentMarkAccess();
            nMemberCount = static_cast<sal_uInt16>(count_if(
                pMarkAccess->getBookmarksBegin(),
                pMarkAccess->getBookmarksEnd(),
                &lcl_IsUiVisibleBookmark));
            sTypeToken = aEmptyStr;
            bEdit = sal_True;
        }
        break;
		case CONTENT_TYPE_REGION :
		{
			SwContentArr* 	pOldMember = 0;
			sal_uInt16 nOldRegionCount = 0;
			sal_Bool bInvalidate = sal_False;
			if(!pMember)
				pMember = new SwContentArr;
			else if(pMember->Count())
			{
				pOldMember = pMember;
				nOldRegionCount = pOldMember->Count();
				pMember = new SwContentArr;
			}
			const Point aNullPt;
			nMemberCount = pWrtShell->GetSectionFmtCount();
			for(sal_uInt16 i = 0; i < nMemberCount; i++)
			{
				const SwSectionFmt* pFmt;
				SectionType eTmpType;
				if( (pFmt = &pWrtShell->GetSectionFmt(i))->IsInNodesArr() &&
				(eTmpType = pFmt->GetSection()->GetType()) != TOX_CONTENT_SECTION
				&& TOX_HEADER_SECTION != eTmpType )
				{
                    const String& rSectionName =
                        pFmt->GetSection()->GetSectionName();
                    sal_uInt8 nLevel = 0;
					SwSectionFmt* pParentFmt = pFmt->GetParent();
					while(pParentFmt)
					{
						nLevel++;
						pParentFmt = pParentFmt->GetParent();
					}

					SwContent* pCnt = new SwRegionContent(this, rSectionName,
							nLevel,
							pFmt->FindLayoutRect( sal_False, &aNullPt ).Top());

					SwPtrMsgPoolItem aAskItem( RES_CONTENT_VISIBLE, 0 );
					if( !pFmt->GetInfo( aAskItem ) &&
						!aAskItem.pObject )		// not visible
						pCnt->SetInvisible();
					pMember->Insert(pCnt);//, pMember->Count());

					sal_uInt16 nPos = pMember->Count() - 1;
					if(nOldRegionCount > nPos &&
						(pOldMember->GetObject(nPos))->IsInvisible()
								!= pCnt->IsInvisible())
							bInvalidate = sal_True;
				}
			}
			nMemberCount = pMember->Count();
			sTypeToken = C2S(pMarkToRegion);
			bEdit = sal_True;
			bDelete = sal_False;
			if(pOldMember)
			{
				pOldMember->DeleteAndDestroy(0, pOldMember->Count());
				delete pOldMember;
				if(pbInvalidateWindow && bInvalidate)
					*pbInvalidateWindow = sal_True;
			}
		}
		break;
		case CONTENT_TYPE_INDEX:
		{
			nMemberCount = pWrtShell->GetTOXCount();
			bEdit = sal_True;
			bDelete = sal_False;
		}
		break;
		case CONTENT_TYPE_REFERENCE:
		{
			nMemberCount = pWrtShell->GetRefMarks( 0 );
			bDelete = sal_False;
		}
		break;
		case CONTENT_TYPE_URLFIELD:
		{
			nMemberCount = 0;
			if(!pMember)
				pMember = new SwContentArr;
			else if(pMember->Count())
				pMember->DeleteAndDestroy(0, pMember->Count());

			SwGetINetAttrs aArr;
			nMemberCount = pWrtShell->GetINetAttrs( aArr );
			for( sal_uInt16 n = 0; n < nMemberCount; ++n )
			{
				SwGetINetAttr* p = aArr[ n ];
				SwURLFieldContent* pCnt = new SwURLFieldContent(
									this,
									p->sText,
									INetURLObject::decode(
										p->rINetAttr.GetINetFmt().GetValue(),
										INET_HEX_ESCAPE,
										INetURLObject::DECODE_UNAMBIGUOUS,
										RTL_TEXTENCODING_UTF8 ),
									&p->rINetAttr,
									n );
				pMember->Insert( pCnt );//, n );
			}
			bEdit = sal_True;
			nOldMemberCount = nMemberCount;
			bDelete = sal_False;
		}
		break;
		case CONTENT_TYPE_POSTIT:
		{
			nMemberCount = 0;
			if(!pMember)
				pMember = new SwContentArr;
			else if(pMember->Count())
				pMember->DeleteAndDestroy(0, pMember->Count());

			SwPostItMgr* aMgr = pWrtShell->GetView().GetPostItMgr();
			if (aMgr)
			{
				for(SwPostItMgr::const_iterator i = aMgr->begin(); i != aMgr->end(); ++i)
				{
					if ( (*i)->GetBroadCaster()->ISA(SwFmtFld)) // SwPostit
					{
						const SwFmtFld* aFmtFld = static_cast<const SwFmtFld*>((*i)->GetBroadCaster());
						if (aFmtFld->GetTxtFld() && aFmtFld->IsFldInDoc() &&
							(*i)->mLayoutStatus!=SwPostItHelper::INVISIBLE )
						{
							String sEntry = aFmtFld->GetField()->GetPar2();
							RemoveNewline(sEntry);
							SwPostItContent* pCnt = new SwPostItContent(
												this,
												sEntry,
												aFmtFld,
												nMemberCount);
							pMember->Insert(pCnt);
							nMemberCount++;
						}
					}
				}
			}
			//
			sTypeToken = aEmptyStr;
			bEdit = sal_True;
			nOldMemberCount = nMemberCount;
		}
		break;
		case CONTENT_TYPE_DRAWOBJECT:
        {
            sTypeToken = aEmptyStr;
            nMemberCount = 0;
            SwDrawModel* pModel = pWrtShell->getIDocumentDrawModelAccess()->GetDrawModel();
            if(pModel)
            {
                SdrPage* pPage = pModel->GetPage(0);
                sal_uInt32 nCount = pPage->GetObjCount();
                for( sal_uInt32 i=0; i< nCount; i++ )
                {
                    SdrObject* pTemp = pPage->GetObj(i);
                    // --> OD 2006-03-09 #i51726# - all drawing objects can be named now
//                    if(pTemp->ISA(SdrObjGroup) && pTemp->GetName().Len())
                    if ( pTemp->GetName().Len() )
                    // <--
                        nMemberCount++;
                }
            }
        }
		break;
	}
	// ... dann koennen die Daten auch nicht mehr gueltig sein
	// abgesehen von denen, die schon korrigiert wurden, dann ist
	// nOldMemberCount doch nicht so old
	if(	nOldMemberCount != nMemberCount )
		bDataValid = sal_False;
}

/***************************************************************************
	Beschreibung:
***************************************************************************/


SwContentType::~SwContentType()
{
	delete pMember;
}

/***************************************************************************
	Beschreibung:	 Inhalt liefern, dazu ggf. die Liste fuellen
***************************************************************************/


const SwContent* SwContentType::GetMember(sal_uInt16 nIndex)
{
	if(!bDataValid || !pMember)
	{
		FillMemberList();
	}
	if(nIndex < pMember->Count())
		return pMember->GetObject(nIndex);
	else
		return 0;

}


/***************************************************************************
	Beschreibung:
***************************************************************************/


void	SwContentType::Invalidate()
{
	bDataValid = sal_False;
}

/***************************************************************************
	Beschreibung: Liste der Inhalte fuellen
***************************************************************************/


void	SwContentType::FillMemberList(sal_Bool* pbLevelOrVisibiblityChanged)
{
	SwContentArr* 	pOldMember = 0;
	int nOldMemberCount = -1;
	SwPtrMsgPoolItem aAskItem( RES_CONTENT_VISIBLE, 0 );
	if(pMember && pbLevelOrVisibiblityChanged)
	{
		pOldMember = pMember;
		nOldMemberCount = pOldMember->Count();
		pMember = new SwContentArr;
		*pbLevelOrVisibiblityChanged = sal_False;
	}
	else if(!pMember)
		pMember = new SwContentArr;
	else if(pMember->Count())
		pMember->DeleteAndDestroy(0, pMember->Count());
	switch(nContentType)
	{
		case CONTENT_TYPE_OUTLINE	:
		{
            sal_uInt16 nOutlineCount = nMemberCount =
                static_cast<sal_uInt16>(pWrtShell->getIDocumentOutlineNodesAccess()->getOutlineNodesCount());

			sal_uInt16 nPos = 0;
			for (sal_uInt16 i = 0; i < nOutlineCount; ++i)
			{
                const sal_Int8 nLevel = (sal_Int8)pWrtShell->getIDocumentOutlineNodesAccess()->getOutlineLevel(i);
				if(nLevel >= nOutlineLevel )
					nMemberCount--;
				else
				{
                    String aEntry(pWrtShell->getIDocumentOutlineNodesAccess()->getOutlineText(i));
					aEntry.EraseLeadingChars();
					SwNavigationPI::CleanEntry( aEntry );
					SwOutlineContent* pCnt = new SwOutlineContent(this, aEntry, i, nLevel,
														pWrtShell->IsOutlineMovable( i ), nPos );
					pMember->Insert(pCnt);//, nPos);
					// bei gleicher Anzahl und vorhandenem pOldMember wird die
					// alte mit der neuen OutlinePos verglichen
					// cast fuer Win16
					if(nOldMemberCount > (int)nPos &&
						((SwOutlineContent*)pOldMember->GetObject(nPos))->GetOutlineLevel() != nLevel)
						*pbLevelOrVisibiblityChanged = sal_True;

					nPos++;
				}
			}

		}
		break;

		case CONTENT_TYPE_TABLE 	:
		{
			DBG_ASSERT(nMemberCount ==
					pWrtShell->GetTblFrmFmtCount(sal_True),
					"MemberCount differiert");
			Point aNullPt;
			nMemberCount =	pWrtShell->GetTblFrmFmtCount(sal_True);
			for(sal_uInt16 i = 0; i < nMemberCount; i++)
			{
				const SwFrmFmt& rTblFmt = pWrtShell->GetTblFrmFmt(i, sal_True);
				String sTblName( rTblFmt.GetName() );

				SwContent* pCnt = new SwContent(this, sTblName,
						rTblFmt.FindLayoutRect(sal_False, &aNullPt).Top() );
				if( !rTblFmt.GetInfo( aAskItem ) &&
					!aAskItem.pObject )		// not visible
					pCnt->SetInvisible();

				pMember->Insert(pCnt);//, i);

				if(nOldMemberCount > (int)i &&
					(pOldMember->GetObject(i))->IsInvisible() != pCnt->IsInvisible())
						*pbLevelOrVisibiblityChanged = sal_True;
			}
		}
		break;
		case CONTENT_TYPE_OLE       :
		case CONTENT_TYPE_FRAME     :
		case CONTENT_TYPE_GRAPHIC   :
		{
			FlyCntType eType = FLYCNTTYPE_FRM;
			if(nContentType == CONTENT_TYPE_OLE)
				eType = FLYCNTTYPE_OLE;
			else if(nContentType == CONTENT_TYPE_GRAPHIC)
				eType = FLYCNTTYPE_GRF;
			DBG_ASSERT(nMemberCount == 	pWrtShell->GetFlyCount(eType),
					"MemberCount differiert");
			Point aNullPt;
			nMemberCount = pWrtShell->GetFlyCount(eType);
			for(sal_uInt16 i = 0; i < nMemberCount; i++)
			{
				const SwFrmFmt* pFrmFmt = pWrtShell->GetFlyNum(i,eType);
				String sFrmName = pFrmFmt->GetName();

				SwContent* pCnt;
				if(CONTENT_TYPE_GRAPHIC == nContentType)
				{
					String sLink;
					pWrtShell->GetGrfNms( &sLink, 0, (SwFlyFrmFmt*) pFrmFmt);
					pCnt = new SwGraphicContent(this, sFrmName,
								INetURLObject::decode( sLink, INET_HEX_ESCAPE,
										INetURLObject::DECODE_UNAMBIGUOUS,
										RTL_TEXTENCODING_UTF8 ),
								pFrmFmt->FindLayoutRect(sal_False, &aNullPt).Top());
				}
				else
				{
					pCnt = new SwContent(this, sFrmName,
							pFrmFmt->FindLayoutRect(sal_False, &aNullPt).Top() );
				}
				if( !pFrmFmt->GetInfo( aAskItem ) &&
					!aAskItem.pObject )		// not visible
					pCnt->SetInvisible();
				pMember->Insert(pCnt);//, i);
				if(nOldMemberCount > (int)i &&
					(pOldMember->GetObject(i))->IsInvisible() != pCnt->IsInvisible())
						*pbLevelOrVisibiblityChanged = sal_True;
			}
		}
		break;
        case CONTENT_TYPE_BOOKMARK:
        {
            IDocumentMarkAccess* const pMarkAccess = pWrtShell->getIDocumentMarkAccess();
            for(IDocumentMarkAccess::const_iterator_t ppBookmark = pMarkAccess->getBookmarksBegin();
                ppBookmark != pMarkAccess->getBookmarksEnd();
                ppBookmark++)
            {
                if(lcl_IsUiVisibleBookmark(*ppBookmark))
                {
                    const String& rBkmName = ppBookmark->get()->GetName();
                    //nYPos von 0 -> text::Bookmarks werden nach Alphabet sortiert
                    SwContent* pCnt = new SwContent(this, rBkmName, 0);
                    pMember->Insert(pCnt);//, pMember->Count());
                }
            }
        }
		break;
		case CONTENT_TYPE_REGION    :
		{
			const Point aNullPt;
			nMemberCount = pWrtShell->GetSectionFmtCount();
			for(sal_uInt16 i = 0; i < nMemberCount; i++)
			{
				const SwSectionFmt* pFmt;
				SectionType eTmpType;
				if( (pFmt = &pWrtShell->GetSectionFmt(i))->IsInNodesArr() &&
				(eTmpType = pFmt->GetSection()->GetType()) != TOX_CONTENT_SECTION
				&& TOX_HEADER_SECTION != eTmpType )
				{
                    String sSectionName = pFmt->GetSection()->GetSectionName();

                    sal_uInt8 nLevel = 0;
					SwSectionFmt* pParentFmt = pFmt->GetParent();
					while(pParentFmt)
					{
						nLevel++;
						pParentFmt = pParentFmt->GetParent();
					}

					SwContent* pCnt = new SwRegionContent(this, sSectionName,
							nLevel,
							pFmt->FindLayoutRect( sal_False, &aNullPt ).Top());
					if( !pFmt->GetInfo( aAskItem ) &&
						!aAskItem.pObject )		// not visible
						pCnt->SetInvisible();
					pMember->Insert(pCnt);//, pMember->Count());

					sal_uInt16 nPos = pMember->Count() - 1;
					if(nOldMemberCount > nPos &&
						(pOldMember->GetObject(nPos))->IsInvisible()
								!= pCnt->IsInvisible())
							*pbLevelOrVisibiblityChanged = sal_True;
				}
			}
			nMemberCount = pMember->Count();
		}
		break;
		case CONTENT_TYPE_REFERENCE:
		{
			SvStringsDtor aRefMarks;
			nMemberCount = pWrtShell->GetRefMarks( &aRefMarks );

			for(sal_uInt16 i=0; i<nMemberCount; i++)
			{
				//Referenzen nach Alphabet sortiert
				SwContent* pCnt = new SwContent(
							this, *aRefMarks.GetObject(i), 0);
				pMember->Insert(pCnt);//, i);
			}
		}
		break;
		case CONTENT_TYPE_URLFIELD:
		{
			SwGetINetAttrs aArr;
			nMemberCount = pWrtShell->GetINetAttrs( aArr );
			for( sal_uInt16 n = 0; n < nMemberCount; ++n )
			{
				SwGetINetAttr* p = aArr[ n ];
				SwURLFieldContent* pCnt = new SwURLFieldContent(
									this,
									p->sText,
									INetURLObject::decode(
										p->rINetAttr.GetINetFmt().GetValue(),
										INET_HEX_ESCAPE,
										INetURLObject::DECODE_UNAMBIGUOUS,
										RTL_TEXTENCODING_UTF8 ),
									&p->rINetAttr,
									n );
				pMember->Insert( pCnt );//, n );
			}
		}
		break;
		case CONTENT_TYPE_INDEX:
		{

			sal_uInt16 nCount = nMemberCount = pWrtShell->GetTOXCount();
			for ( sal_uInt16 nTox = 0; nTox < nCount; nTox++ )
			{
				const SwTOXBase* pBase = pWrtShell->GetTOX( nTox );
				String sTOXNm( pBase->GetTOXName() );

				SwContent* pCnt = new SwTOXBaseContent(
						this, sTOXNm, nTox, *pBase);

				if( !pBase->GetInfo( aAskItem ) &&
					!aAskItem.pObject )		// not visible
					pCnt->SetInvisible();

				pMember->Insert( pCnt );//, nTox );
				sal_uInt16 nPos = pMember->Count() - 1;
				if(nOldMemberCount > nPos &&
					(pOldMember->GetObject(nPos))->IsInvisible()
							!= pCnt->IsInvisible())
						*pbLevelOrVisibiblityChanged = sal_True;
			}
		}
		break;
		case CONTENT_TYPE_POSTIT:
		{
			nMemberCount = 0;
			if(!pMember)
				pMember = new SwContentArr;
			else if(pMember->Count())
				pMember->DeleteAndDestroy(0, pMember->Count());
			SwPostItMgr* aMgr = pWrtShell->GetView().GetPostItMgr();
			if (aMgr)
			{
				for(SwPostItMgr::const_iterator i = aMgr->begin(); i != aMgr->end(); ++i)
				{
					if ( (*i)->GetBroadCaster()->ISA(SwFmtFld)) // SwPostit
					{
						const SwFmtFld* aFmtFld = static_cast<const SwFmtFld*>((*i)->GetBroadCaster());
						if (aFmtFld->GetTxtFld() && aFmtFld->IsFldInDoc() &&
							(*i)->mLayoutStatus!=SwPostItHelper::INVISIBLE )
						{
							String sEntry = aFmtFld->GetField()->GetPar2();
							RemoveNewline(sEntry);
							SwPostItContent* pCnt = new SwPostItContent(
												this,
												sEntry,
												aFmtFld,
												nMemberCount);
							pMember->Insert(pCnt);
							nMemberCount++;
						}
					}
					/*	this code can be used once we want redline comments in the margin
					else	// redcomment
					{
						SwRedline* pRedline = static_cast<SwRedline*>((*i)->GetBroadCaster());
						if ( pRedline->GetComment() != String(::rtl::OUString::createFromAscii("")) )
						{
							String sEntry = pRedline->GetComment();
							RemoveNewline(sEntry);
							SwPostItContent* pCnt = new SwPostItContent(
												this,
												sEntry,
												pRedline,
												nMemberCount);
							pMember->Insert(pCnt);
							nMemberCount++;
						}
					}
					*/
				}
			}
			//
		}
		break;
		case CONTENT_TYPE_DRAWOBJECT:
        {
            nMemberCount = 0;
			if(!pMember)
				pMember = new SwContentArr;
			else if(pMember->Count())
				pMember->DeleteAndDestroy(0, pMember->Count());

            IDocumentDrawModelAccess* pIDDMA = pWrtShell->getIDocumentDrawModelAccess();
            SwDrawModel* pModel = pIDDMA->GetDrawModel();
            if(pModel)
            {
                SdrPage* pPage = pModel->GetPage(0);
                sal_uInt32 nCount = pPage->GetObjCount();
                for( sal_uInt32 i=0; i< nCount; i++ )
                {
                    SdrObject* pTemp = pPage->GetObj(i);
                    // --> OD 2006-03-09 #i51726# - all drawing objects can be named now
//                    if(pTemp->ISA(SdrObjGroup) && pTemp->GetName().Len())
                    if ( pTemp->GetName().Len() )
                    // <--
                    {
                        SwContact* pContact = (SwContact*)pTemp->GetUserCall();
                        long nYPos = 0;
                        const Point aNullPt;
                        if(pContact && pContact->GetFmt())
                            nYPos = pContact->GetFmt()->FindLayoutRect(sal_False, &aNullPt).Top();
                        SwContent* pCnt = new SwContent(
                                            this,
                                            pTemp->GetName(),
                                            nYPos);
                        if(!pIDDMA->IsVisibleLayerId(pTemp->GetLayer()))
                            pCnt->SetInvisible();
                        pMember->Insert(pCnt);
                        nMemberCount++;
                        if(nOldMemberCount > (int)i &&
                            (pOldMember->GetObject((sal_uInt16)i))->IsInvisible() != pCnt->IsInvisible())
                                *pbLevelOrVisibiblityChanged = sal_True;
                    }
                }
            }
        }
        break;
	}
	bDataValid = sal_True;
	if(pOldMember)
		pOldMember->DeleteAndDestroy(0, pOldMember->Count());

}

/***************************************************************************
	Beschreibung: TreeListBox fuer Inhaltsanzeige
***************************************************************************/


SwContentTree::SwContentTree(Window* pParent, const ResId& rResId) :
		SvTreeListBox( pParent, rResId ),

        sSpace(C2S("                    ")),

        sRemoveIdx(SW_RES(ST_REMOVE_INDEX)),
        sUpdateIdx(SW_RES(ST_UPDATE)),
        sUnprotTbl(SW_RES(ST_REMOVE_TBL_PROTECTION)),
        sRename(SW_RES(ST_RENAME)),
        sReadonlyIdx(SW_RES(ST_READONLY_IDX)),
        sInvisible(SW_RES(ST_INVISIBLE)),

	sPostItShow(SW_RES(ST_POSTIT_SHOW)),
	sPostItHide(SW_RES(ST_POSTIT_HIDE)),
	sPostItDelete(SW_RES(ST_POSTIT_DELETE)),

        pHiddenShell(0),
	pActiveShell(0),
	pConfig(SW_MOD()->GetNavigationConfig()),

        nActiveBlock(0),
	nHiddenBlock(0),

        nRootType(USHRT_MAX),
        nLastSelType(USHRT_MAX),
        nOutlineLevel(MAXLEVEL),

        bIsActive(sal_True),
		bIsConstant(sal_False),
		bIsHidden(sal_False),
        bDocChgdInDragging(sal_False),
		bIsInternalDrag(sal_False),
		bIsRoot(sal_False),
		bIsIdleClear(sal_False),
		bIsLastReadOnly(sal_False),
		bIsOutlineMoveable(sal_True),
		bViewHasChanged(sal_False),
		bIsImageListInitialized(sal_False),
		bIsKeySpace(sal_False)
{
	sal_uInt16 i;

	SetHelpId(HID_NAVIGATOR_TREELIST);

	SetNodeDefaultImages();
	SetDoubleClickHdl(LINK(this, SwContentTree, ContentDoubleClickHdl));
	SetDragDropMode(SV_DRAGDROP_APP_COPY);
	for( i = 0; i < CONTENT_TYPE_MAX; i++ )
	{
		aActiveContentArr[i] 	= 0;
		aHiddenContentArr[i] 	= 0;
	}
	for( i = 0; i < CONTEXT_COUNT; i++ )
	{
		aContextStrings[i] = SW_RESSTR(i+ST_CONTEXT_FIRST);
	}
	nActiveBlock = pConfig->GetActiveBlock();
	aUpdTimer.SetTimeoutHdl(LINK(this, SwContentTree, TimerUpdate));
	aUpdTimer.SetTimeout(1000);
	Clear();
	EnableContextMenuHandling();
	SetStyle( GetStyle() | WB_QUICK_SEARCH );
}

/***************************************************************************
	Beschreibung:
***************************************************************************/


SwContentTree::~SwContentTree()
{
	Clear(); // vorher ggf. Inhaltstypen loeschen
	bIsInDrag = sal_False;
}

String SwContentTree::GetEntryAltText( SvLBoxEntry* pEntry ) const
{
	if( pEntry == NULL)
		return String();

	SwContent* pCnt = (SwContent*)pEntry->GetUserData();
	if( pCnt == NULL || pCnt->GetParent() == NULL)
		return String();

	sal_uInt16 nJumpType = pCnt->GetParent()->GetType();
	SdrObject* pTemp;

	switch(nJumpType)
	{
		case CONTENT_TYPE_DRAWOBJECT:
			{
				SdrView* pDrawView = pActiveShell->GetDrawView();
				if (pDrawView)
				{
                    SwDrawModel* pDrawModel = pActiveShell->GetDoc()->GetDrawModel();
                    SdrPage* pPage = pDrawModel->GetPage(0);
                    const sal_uInt32 nCount = pPage->GetObjCount();
					for( sal_uInt32 i=0; i< nCount; i++ )
					{
						pTemp = pPage->GetObj(i);
						sal_uInt16 nCmpId;
						switch( pTemp->GetObjIdentifier() )
						{
						case OBJ_GRUP:
						case OBJ_TEXT:
						case OBJ_TEXTEXT:
						case OBJ_wegFITTEXT:
						case OBJ_LINE:
						case OBJ_RECT:
							//caoxueqin added custom shape
						case OBJ_CUSTOMSHAPE:
							//end 2005/08/05
						case OBJ_CIRC:
						case OBJ_SECT:
						case OBJ_CARC:
						case OBJ_CCUT:
						case OBJ_POLY:
						case OBJ_PLIN:
						case OBJ_PATHLINE:
						case OBJ_PATHFILL:
						case OBJ_FREELINE:
						case OBJ_FREEFILL:
						case OBJ_PATHPOLY:
						case OBJ_PATHPLIN:
						case OBJ_CAPTION:
							nCmpId = OBJ_GRUP;
							break;
						default:
							nCmpId = pTemp->GetObjIdentifier();
						}
						if(nCmpId == OBJ_GRUP /*pTemp->ISA(SdrObjGroup)*/ && pTemp->GetName() == pCnt->GetName())
						{
							return pTemp->GetTitle();
						}
						//Commented End
					}
				}
			}
			break;
		case CONTENT_TYPE_GRAPHIC   :
			{
				if( pActiveShell && pActiveShell->GetDoc() )
				{
					const SwFlyFrmFmt* pFrmFmt = pActiveShell->GetDoc()->FindFlyByName( pCnt->GetName(), 0);
					if( pFrmFmt )
					{
//                        SwNodeIndex aIdx( *(pFrmFmt->GetCntnt().GetCntntIdx()), 1 );
//                        const SwGrfNode* pGrfNd = aIdx.GetNode().GetGrfNode();
//                        if( pGrfNd )
//                            return pGrfNd->GetAlternateText();
                        return pFrmFmt->GetObjTitle();
					}
				}
			}
			break;
		case CONTENT_TYPE_OLE       :
		case CONTENT_TYPE_FRAME     :
			{
				//Can't find the GetAlternateText function. Need to verify again.
				const SwFlyFrmFmt* pFlyFmt = pActiveShell->GetDoc()->FindFlyByName( pCnt->GetName(), 0);
				if( pFlyFmt )
					return pFlyFmt->/*GetAlternateText*/GetName();
			}
			break;
	}
	return String();
}

String SwContentTree::GetEntryLongDescription( SvLBoxEntry* pEntry ) const
{
	if( pEntry == NULL)
		return String();

	SwContent* pCnt = (SwContent*)pEntry->GetUserData();
	if( pCnt == NULL || pCnt->GetParent() == NULL)
		return String();

	sal_uInt16 nJumpType = pCnt->GetParent()->GetType();
	SdrObject* pTemp;

	switch(nJumpType)
	{
		case CONTENT_TYPE_DRAWOBJECT:
			{
				SdrView* pDrawView = pActiveShell->GetDrawView();
				if (pDrawView)
				{
                    SwDrawModel* pDrawModel = pActiveShell->GetDoc()->GetDrawModel();
                    SdrPage* pPage = pDrawModel->GetPage(0);
					sal_uInt32 nCount = pPage->GetObjCount();
					for( sal_uInt32 i=0; i< nCount; i++ )
					{
						pTemp = pPage->GetObj(i);
						sal_uInt16 nCmpId;
						switch( pTemp->GetObjIdentifier() )
						{
						case OBJ_GRUP:
						case OBJ_TEXT:
						case OBJ_TEXTEXT:
						case OBJ_wegFITTEXT:
						case OBJ_LINE:
						case OBJ_RECT:
							//caoxueqin added custom shape
						case OBJ_CUSTOMSHAPE:
							//end 2005/08/05
						case OBJ_CIRC:
						case OBJ_SECT:
						case OBJ_CARC:
						case OBJ_CCUT:
						case OBJ_POLY:
						case OBJ_PLIN:
						case OBJ_PATHLINE:
						case OBJ_PATHFILL:
						case OBJ_FREELINE:
						case OBJ_FREEFILL:
						case OBJ_PATHPOLY:
						case OBJ_PATHPLIN:
						case OBJ_CAPTION:
							nCmpId = OBJ_GRUP;
							break;
						default:
							nCmpId = pTemp->GetObjIdentifier();
						}
						if(nCmpId == OBJ_GRUP /*pTemp->ISA(SdrObjGroup)*/ && pTemp->GetName() == pCnt->GetName())
						{
							return pTemp->GetDescription();
						}
						//Commented End
					}
				}
			}
			break;
		case CONTENT_TYPE_GRAPHIC   :
		case CONTENT_TYPE_OLE       :
		case CONTENT_TYPE_FRAME     :
			{
				//Can't find the function "GetLongDescription". Need to verify again.
				const SwFlyFrmFmt* pFlyFmt = pActiveShell->GetDoc()->FindFlyByName( pCnt->GetName(), 0);
				if( pFlyFmt )
					return pFlyFmt->GetDescription();
			}
			break;
	}
	return String();
}

/***************************************************************************
	Drag&Drop methods
***************************************************************************/

void SwContentTree::StartDrag( sal_Int8 nAction, const Point& rPosPixel )
{
	if( !bIsRoot || nRootType != CONTENT_TYPE_OUTLINE )
	{
		ReleaseMouse();

		TransferDataContainer* pContainer = new TransferDataContainer;
        uno::Reference<
            datatransfer::XTransferable > xRef( pContainer );

		sal_Int8 nDragMode = DND_ACTION_COPYMOVE | DND_ACTION_LINK;
		if( FillTransferData( *pContainer, nDragMode ))
		{
			SwContentTree::SetInDrag(sal_True);
			pContainer->StartDrag( this, nDragMode, GetDragFinishedHdl() );
		}
	}
	else
		SvTreeListBox::StartDrag( nAction, rPosPixel );
}

void SwContentTree::DragFinished( sal_Int8 nAction )
{
    //to prevent the removing of the selected entry in external drag and drop
    // the drag action mustn't be MOVE
    SvTreeListBox::DragFinished( bIsInternalDrag ? nAction : DND_ACTION_COPY );
	SwContentTree::SetInDrag(sal_False);
	bIsInternalDrag = sal_False;
}

/***************************************************************************
	Beschreibung:   QueryDrop wird im Navigator ausgefuehrt
***************************************************************************/
sal_Int8 SwContentTree::AcceptDrop( const AcceptDropEvent& rEvt )
{
	sal_Int8 nRet = DND_ACTION_NONE;
	if( bIsRoot )
	{
		if( bIsOutlineMoveable )
			nRet = SvTreeListBox::AcceptDrop( rEvt );
	}
	else if( !bIsInDrag )
		nRet = GetParentWindow()->AcceptDrop( rEvt );
	return nRet;
}

/***************************************************************************
	Beschreibung:	Drop wird im Navigator ausgefuehrt
***************************************************************************/
sal_Int8 SwContentTree::ExecuteDrop( const ExecuteDropEvent& rEvt )
{
	if( bIsRoot )
		return SvTreeListBox::ExecuteDrop( rEvt );
	return bIsInDrag ? DND_ACTION_NONE : GetParentWindow()->ExecuteDrop(rEvt);
}


/***************************************************************************
	Beschreibung:	Handler fuer Dragging und ContextMenu
***************************************************************************/
PopupMenu* SwContentTree::CreateContextMenu( void )
{
    PopupMenu* pPop = new PopupMenu;
    PopupMenu* pSubPop1 = new PopupMenu;
    PopupMenu* pSubPop2 = new PopupMenu;
    PopupMenu* pSubPop3 = new PopupMenu;
    PopupMenu* pSubPop4 = new PopupMenu; // Edit

    sal_uInt16 i;
    for(i = 1; i <= MAXLEVEL; i++ )
    {
        pSubPop1->InsertItem( i + 100, String::CreateFromInt32(i));
    }
    pSubPop1->CheckItem(100 + nOutlineLevel);
    for(i=0; i < 3; i++ )
    {
        pSubPop2->InsertItem( i + 201, aContextStrings[
                ST_HYPERLINK - ST_CONTEXT_FIRST + i]);
    }
    pSubPop2->CheckItem( 201 +
                    GetParentWindow()->GetRegionDropMode());
	// Insert list of open files (filter out help pages)
	sal_uInt16 nId = 301;
	const SwView* pActiveView = ::GetActiveView();
	SwView *pView = SwModule::GetFirstView();
	while (pView)
	{
		if (!pView->GetDocShell()->IsHelpDocument())
		{
			String sInsert = pView->GetDocShell()->GetTitle();
			sInsert += C2S(" (");
			if(pView == pActiveView)
			{
				sInsert += aContextStrings[ ST_ACTIVE - ST_CONTEXT_FIRST];
			}
			else
			{
				sInsert += aContextStrings[ ST_INACTIVE - ST_CONTEXT_FIRST];
			}
			sInsert += ')';
			pSubPop3->InsertItem(nId, sInsert);
			if(bIsConstant && pActiveShell == &pView->GetWrtShell())
				pSubPop3->CheckItem(nId);
		}
	pView = SwModule::GetNextView(pView);
	nId++;
	}
    pSubPop3->InsertItem(nId++, aContextStrings[ST_ACTIVE_VIEW - ST_CONTEXT_FIRST]);
    if(pHiddenShell)
    {
        String sHiddenEntry = pHiddenShell->GetView().GetDocShell()->GetTitle();
        sHiddenEntry += C2S(" (");
        sHiddenEntry += aContextStrings[ ST_HIDDEN - ST_CONTEXT_FIRST];
        sHiddenEntry += ')';
        pSubPop3->InsertItem(nId, sHiddenEntry);
    }

    if(bIsActive)
        pSubPop3->CheckItem( --nId );
    else if(bIsHidden)
        pSubPop3->CheckItem( nId );

    pPop->InsertItem( 1, aContextStrings[ST_OUTLINE_LEVEL - ST_CONTEXT_FIRST]);
    pPop->InsertItem(2, aContextStrings[ST_DRAGMODE - ST_CONTEXT_FIRST]);
    pPop->InsertItem(3, aContextStrings[ST_DISPLAY - ST_CONTEXT_FIRST]);
    //jetzt noch bearbeiten
    SvLBoxEntry* pEntry = 0;
    //Bearbeiten nur, wenn die angezeigten Inhalte aus der aktiven View kommen
    if((bIsActive || pActiveShell == pActiveView->GetWrtShellPtr())
            && 0 != (pEntry = FirstSelected()) && lcl_IsContent(pEntry))
    {
        const SwContentType* pContType = ((SwContent*)pEntry->GetUserData())->GetParent();
        const sal_uInt16 nContentType = pContType->GetType();
        sal_Bool bReadonly = pActiveShell->GetView().GetDocShell()->IsReadOnly();
        sal_Bool bVisible = !((SwContent*)pEntry->GetUserData())->IsInvisible();
        sal_Bool bProtected = ((SwContent*)pEntry->GetUserData())->IsProtect();
        sal_Bool bEditable = pContType->IsEditable() &&
            ((bVisible && !bProtected) ||CONTENT_TYPE_REGION == nContentType);
        sal_Bool bDeletable = pContType->IsDeletable() &&
            ((bVisible && !bProtected) ||CONTENT_TYPE_REGION == nContentType);
        sal_Bool bRenamable = bEditable && !bReadonly &&
            (CONTENT_TYPE_TABLE == nContentType ||
                CONTENT_TYPE_FRAME == nContentType ||
                CONTENT_TYPE_GRAPHIC == nContentType ||
                CONTENT_TYPE_OLE == nContentType ||
                CONTENT_TYPE_BOOKMARK == nContentType ||
                CONTENT_TYPE_REGION == nContentType||
                CONTENT_TYPE_INDEX == nContentType);

        if(!bReadonly && (bEditable || bDeletable))
        {
            sal_Bool bSubPop4 = sal_False;
            if(CONTENT_TYPE_INDEX == nContentType)
            {
                bSubPop4 = sal_True;
                pSubPop4->InsertItem(401, sRemoveIdx);
                pSubPop4->InsertItem(402, sUpdateIdx);

                const SwTOXBase* pBase = ((SwTOXBaseContent*)pEntry->GetUserData())->GetTOXBase();
                if(!pBase->IsTOXBaseInReadonly())
                    pSubPop4->InsertItem(403, aContextStrings[ST_EDIT_ENTRY - ST_CONTEXT_FIRST]);
                pSubPop4->InsertItem(405, sReadonlyIdx);

                pSubPop4->CheckItem( 405, pActiveShell->IsTOXBaseReadonly(*pBase));
                pSubPop4->InsertItem(501, aContextStrings[ST_DELETE_ENTRY - ST_CONTEXT_FIRST]);
            }
            else if(CONTENT_TYPE_TABLE == nContentType && !bReadonly)
            {
                bSubPop4 = sal_True;
                pSubPop4->InsertItem(403, aContextStrings[ST_EDIT_ENTRY - ST_CONTEXT_FIRST]);
                pSubPop4->InsertItem(404, sUnprotTbl);
                sal_Bool bFull = sal_False;
                String sTblName = ((SwContent*)pEntry->GetUserData())->GetName();
                sal_Bool bProt =pActiveShell->HasTblAnyProtection( &sTblName, &bFull );
                pSubPop4->EnableItem(403, !bFull );
                pSubPop4->EnableItem(404, bProt );
                pSubPop4->InsertItem(501, aContextStrings[ST_DELETE_ENTRY - ST_CONTEXT_FIRST]);
            }
            else if(bEditable || bDeletable)
            {

				if(bEditable && bDeletable)
                {
                    pSubPop4->InsertItem(403, aContextStrings[ST_EDIT_ENTRY - ST_CONTEXT_FIRST]);
                    pSubPop4->InsertItem(501, aContextStrings[ST_DELETE_ENTRY - ST_CONTEXT_FIRST]);
                    bSubPop4 = sal_True;
                }
                else if(bEditable)
                    pPop->InsertItem(403, aContextStrings[ST_EDIT_ENTRY - ST_CONTEXT_FIRST]);
                else if(bDeletable)
				{
                    pSubPop4->InsertItem(501, aContextStrings[ST_DELETE_ENTRY - ST_CONTEXT_FIRST]);
				}
            }
            //Rename object
            if(bRenamable)
            {
                if(bSubPop4)
                    pSubPop4->InsertItem(502, sRename);
                else
                    pPop->InsertItem(502, sRename);
            }

            if(bSubPop4)
            {
                pPop->InsertItem(4, pContType->GetSingleName());
                pPop->SetPopupMenu(4, pSubPop4);
            }
        }
    }
    else if( pEntry )
	{
		SwContentType* pType = (SwContentType*)pEntry->GetUserData();
		if ( (pType->GetType() == CONTENT_TYPE_POSTIT) && (!pActiveShell->GetView().GetDocShell()->IsReadOnly()) && ( pType->GetMemberCount() > 0) )
		{
				pSubPop4->InsertItem(600, sPostItShow );
				pSubPop4->InsertItem(601, sPostItHide );
				pSubPop4->InsertItem(602, sPostItDelete );
				/*
				pSubPop4->InsertItem(603,rtl::OUString::createFromAscii("Sort"));
				PopupMenu* pMenuSort = new PopupMenu;
				pMenuSort->InsertItem(604,rtl::OUString::createFromAscii("By Position"));
				pMenuSort->InsertItem(605,rtl::OUString::createFromAscii("By Author"));
				pMenuSort->InsertItem(606,rtl::OUString::createFromAscii("By Date"));
				pSubPop4->SetPopupMenu(603, pMenuSort);
				*/
			    pPop->InsertItem(4, pType->GetSingleName());
                pPop->SetPopupMenu(4, pSubPop4);
		}
	}

    pPop->SetPopupMenu( 1, pSubPop1 );
    pPop->SetPopupMenu( 2, pSubPop2 );
    pPop->SetPopupMenu( 3, pSubPop3 );
    return pPop;

}
/***************************************************************************
	Beschreibung:	Einrueckung fuer outlines (und sections)
***************************************************************************/


long	SwContentTree::GetTabPos( SvLBoxEntry* pEntry, SvLBoxTab* pTab)
{
	sal_uInt16 nLevel = 0;
	if(lcl_IsContent(pEntry))
	{
		nLevel++;
		SwContent* pCnt = (SwContent *) pEntry->GetUserData();
		const SwContentType*	pParent;
		if(pCnt &&	0 != (pParent = pCnt->GetParent()))
		{
			if(pParent->GetType() == CONTENT_TYPE_OUTLINE)
                nLevel = nLevel + ((SwOutlineContent*)pCnt)->GetOutlineLevel();
			else if(pParent->GetType() == CONTENT_TYPE_REGION)
                nLevel = nLevel + ((SwRegionContent*)pCnt)->GetRegionLevel();
		}
	}
	sal_uInt16 nBasis = bIsRoot ? 0 : 5;
	return nLevel * 10 + nBasis + pTab->GetPos();  //empirisch ermittelt
}

/***************************************************************************
	Beschreibung:	Inhalte werden erst auf Anforderung in die Box eingefuegt
***************************************************************************/


void SwContentTree::RequestingChilds( SvLBoxEntry* pParent )
{
	// ist es ein Inhaltstyp?
	if(lcl_IsContentType(pParent))
	{
		if(!pParent->HasChilds())
		{
			DBG_ASSERT(pParent->GetUserData(), "keine UserData?");
			SwContentType* pCntType = (SwContentType*)pParent->GetUserData();

			sal_uInt16 nCount = pCntType->GetMemberCount();
			for(sal_uInt16 i = 0; i < nCount; i++)
			{
				const SwContent* pCnt = pCntType->GetMember(i);
				if(pCnt)
				{
					String sEntry = pCnt->GetName();
					if(!sEntry.Len())
						sEntry = sSpace;
					SvLBoxEntry* pChild = InsertEntry(sEntry, pParent,
							sal_False, LIST_APPEND, (void*)pCnt);
					//Solution: If object is marked , the corresponding entry is set true ,
					//else the corresponding entry is set false .
					//==================================================
					SdrObject * pObj = GetDrawingObjectsByContent(pCnt);
					if(pChild)
					      pChild->SetMarked(sal_False);
					if(pObj)
					{
						SdrView* pDrawView = pActiveShell->GetDrawView();
						SdrPageView* pPV = pDrawView->/*GetPageViewPvNum*/GetSdrPageView(/*0*/);
						if( pPV )
						{
							sal_Bool Marked = pDrawView->IsObjMarked(pObj);
							if(Marked)
							{
								//sEntry += String::CreateFromAscii(" *");
								pChild->SetMarked(sal_True);
							}

						}
					}
				}
			}
		}
	}
}
/***************************************************************************
	Beschreibung:	Expand - Zustand fuer Inhaltstypen merken
***************************************************************************/

//Solution: Get drawing Objects by content .
SdrObject* SwContentTree::GetDrawingObjectsByContent(const SwContent *pCnt)
{
	SdrObject *pRetObj = NULL;
	sal_uInt16 nJumpType = pCnt->GetParent()->GetType();
	switch(nJumpType)
	{
		case CONTENT_TYPE_DRAWOBJECT:
		{
			SdrView* pDrawView = pActiveShell->GetDrawView();
			if (pDrawView)
			{
                SwDrawModel* pDrawModel = pActiveShell->GetDoc()->GetDrawModel();
                SdrPage* pPage = pDrawModel->GetPage(0);
				sal_uInt32 nCount = pPage->GetObjCount();

				for( sal_uInt32 i=0; i< nCount; i++ )
				{
					SdrObject* pTemp = pPage->GetObj(i);
					if( pTemp->GetName() == pCnt->GetName())
					{
						pRetObj = pTemp;
						break;
					}
				}
			}
			break;
		}
		default:
			pRetObj = NULL;
	}
	return pRetObj;
}

sal_Bool SwContentTree::Expand( SvLBoxEntry* pParent )
{
	if(!bIsRoot)
	{
		if(lcl_IsContentType(pParent))
		{
			SwContentType* pCntType = (SwContentType*)pParent->GetUserData();
			sal_uInt16 nOr = 1 << pCntType->GetType(); //linear -> Bitposition
			if(bIsActive || bIsConstant)
			{
				nActiveBlock |= nOr;
				pConfig->SetActiveBlock(nActiveBlock);
			}
			else
				nHiddenBlock |= nOr;
		}
	}
	return SvTreeListBox::Expand(pParent);
}
/***************************************************************************
	Beschreibung:	Collapse - Zustand fuer Inhaltstypen merken
***************************************************************************/


sal_Bool SwContentTree::Collapse( SvLBoxEntry* pParent )
{
	sal_Bool bRet;
	if(!bIsRoot)
	{
		if(lcl_IsContentType(pParent))
		{
			SwContentType* pCntType = (SwContentType*)pParent->GetUserData();
			sal_uInt16 nAnd = 1 << pCntType->GetType();
			nAnd = ~nAnd;
			if(bIsActive || bIsConstant)
			{
				nActiveBlock &= nAnd;
				pConfig->SetActiveBlock(nActiveBlock);
			}
			else
				nHiddenBlock &= nAnd;
		}
			bRet = SvTreeListBox::Collapse(pParent);
	}
	else
		bRet = sal_False;
	return bRet;
}


/***************************************************************************
	Beschreibung:	Auch auf Doppelclick wird zunaechst nur aufgeklappt
***************************************************************************/


IMPL_LINK( SwContentTree, ContentDoubleClickHdl, SwContentTree *, EMPTYARG )
{
	SvLBoxEntry* pEntry = GetCurEntry();
	// ist es ein Inhaltstyp?
	DBG_ASSERT(pEntry, "kein aktueller Eintrag!");
	if(pEntry)
	{
		if(lcl_IsContentType(pEntry))
			RequestingChilds(pEntry);
		else if(bIsActive || bIsConstant)
		{
			if(bIsConstant)
			{
				pActiveShell->GetView().GetViewFrame()->GetWindow().ToTop();
			}
			//Inhaltstyp anspringen:
			SwContent* pCnt = (SwContent*)pEntry->GetUserData();
			DBG_ASSERT( pCnt, "keine UserData");
			GotoContent(pCnt);
			if(pCnt->GetParent()->GetType()	== CONTENT_TYPE_FRAME)
				pActiveShell->EnterStdMode();
		}
	}
	return 0;
}

/***************************************************************************
	Beschreibung:	Anzeigen der Datei
***************************************************************************/


void SwContentTree::Display( sal_Bool bActive )
{
	if(!bIsImageListInitialized)
	{
        sal_uInt16 nResId = GetSettings().GetStyleSettings().GetHighContrastMode() ? IMG_NAVI_ENTRYBMPH : IMG_NAVI_ENTRYBMP;
        aEntryImages = ImageList(SW_RES(nResId));
		bIsImageListInitialized = sal_True;
	}
	// erst den selektierten Eintrag auslesen, um ihn spaeter evtl. wieder
	// zu selektieren -> die UserDaten sind hier nicht mehr gueltig!
	SvLBoxEntry* pOldSelEntry = FirstSelected();
	String sEntryName;	// Name des Eintrags
	sal_uInt16 nEntryRelPos = 0; // rel. Pos zu seinem Parent
    sal_uInt32 nOldEntryCount = GetEntryCount();
    sal_Int32 nOldScrollPos = 0;
	if(pOldSelEntry)
	{
        ScrollBar* pVScroll = GetVScroll();
        if(pVScroll && pVScroll->IsVisible())
            nOldScrollPos = pVScroll->GetThumbPos();

        sEntryName = GetEntryText(pOldSelEntry);
		if(GetParent(pOldSelEntry))
		{
			nEntryRelPos = (sal_uInt16)(GetModel()->GetAbsPos(pOldSelEntry) - GetModel()->GetAbsPos(GetParent(pOldSelEntry)));
		}
	}
	Clear();
	SetUpdateMode( sal_False );
	if(bActive && !bIsConstant && !bIsActive)
		bIsActive = bActive;
	bIsHidden = !bActive;
	SwWrtShell* pShell = GetWrtShell();
	sal_Bool bReadOnly = pShell ? pShell->GetView().GetDocShell()->IsReadOnly() : sal_True;
	if(bReadOnly != bIsLastReadOnly)
	{
		bIsLastReadOnly = bReadOnly;
		sal_Bool bDisable = pShell == 0 || bReadOnly;
		SwNavigationPI* pNavi = GetParentWindow();
		pNavi->aContentToolBox.EnableItem(FN_ITEM_UP , !bDisable);
		pNavi->aContentToolBox.EnableItem(FN_ITEM_DOWN, !bDisable);
		pNavi->aContentToolBox.EnableItem(FN_ITEM_LEFT, !bDisable);
		pNavi->aContentToolBox.EnableItem(FN_ITEM_RIGHT, !bDisable);
		pNavi->aContentToolBox.EnableItem(FN_SELECT_SET_AUTO_BOOKMARK, !bDisable);
	}
	if(pShell)
	{
		SvLBoxEntry* pSelEntry = 0;
		if(nRootType == USHRT_MAX)
		{
			for(sal_uInt16 nCntType = CONTENT_TYPE_OUTLINE;
                        nCntType <= CONTENT_TYPE_DRAWOBJECT; nCntType++ )
			{
				SwContentType** ppContentT = bActive ?
								&aActiveContentArr[nCntType] :
									&aHiddenContentArr[nCntType];
				if(!*ppContentT)
					(*ppContentT) = new SwContentType(pShell, nCntType, nOutlineLevel );


				String sEntry = (*ppContentT)->GetName();
				SvLBoxEntry* pEntry;
				const Image& rImage = aEntryImages.GetImage(SID_SW_START + nCntType);
				sal_Bool bChOnDemand = 0 != (*ppContentT)->GetMemberCount();
				pEntry = InsertEntry(sEntry, rImage, rImage,
								0, bChOnDemand, LIST_APPEND, (*ppContentT));
				if(nCntType == nLastSelType)
					pSelEntry = pEntry;
                sal_Int32 nExpandOptions = bIsActive || bIsConstant ?
											nActiveBlock :
												nHiddenBlock;
				if(nExpandOptions & (1 << nCntType))
				{
					Expand(pEntry);
					if(nEntryRelPos && nCntType == nLastSelType)
					{
						// jetzt vielleicht noch ein Child selektieren
						SvLBoxEntry* pChild = pEntry;
						SvLBoxEntry* pTemp = 0;
						sal_uInt16 nPos = 1;
						while(0 != (pChild = Next(pChild)))
						{
							// der alte Text wird leicht bevorzugt
							if(sEntryName == GetEntryText(pChild) ||
								nPos == nEntryRelPos )
							{
								pSelEntry = pChild;
								break;
							}
							pTemp = pChild;
							nPos++;
						}
						if(!pSelEntry || lcl_IsContentType(pSelEntry))
							pSelEntry = pTemp;
					}

				}
			}
			if(pSelEntry)
			{
				MakeVisible(pSelEntry);
				Select(pSelEntry);
            }
            else
                nOldScrollPos = 0;
		}
		else
		{
			SwContentType** ppRootContentT = bActive ?
								&aActiveContentArr[nRootType] :
									&aHiddenContentArr[nRootType];
			if(!(*ppRootContentT))
				(*ppRootContentT) = new SwContentType(pShell, nRootType, nOutlineLevel );
			const Image& rImage = aEntryImages.GetImage(20000 + nRootType);
			SvLBoxEntry* pParent = InsertEntry(
					(*ppRootContentT)->GetName(), rImage, rImage,
						0, sal_False, LIST_APPEND, *ppRootContentT);

			for(sal_uInt16 i = 0; i < (*ppRootContentT)->GetMemberCount(); i++ )
			{
				const SwContent* pCnt = (*ppRootContentT)->GetMember(i);
				if(pCnt)
				{
					String sEntry = pCnt->GetName();
					if(!sEntry.Len())
						sEntry = sSpace;
                    InsertEntry( sEntry, pParent,
								sal_False, LIST_APPEND, (void*)pCnt);
				}
			}
			Expand(pParent);
			if( nRootType == CONTENT_TYPE_OUTLINE && bIsActive )
			{
				//feststellen, wo der Cursor steht
				const sal_uInt16 nActPos = pShell->GetOutlinePos(MAXLEVEL);
				SvLBoxEntry* pEntry = First();

				while( 0 != (pEntry = Next(pEntry)) )
				{
					if(((SwOutlineContent*)pEntry->GetUserData())->GetPos() == nActPos)
					{
						MakeVisible(pEntry);
						Select(pEntry);
					}
				}

			}
			else
			{
				// jetzt vielleicht noch ein Child selektieren
				SvLBoxEntry* pChild = pParent;
				SvLBoxEntry* pTemp = 0;
				sal_uInt16 nPos = 1;
				while(0 != (pChild = Next(pChild)))
				{
					// der alte Text wird leicht bevorzugt
					if(sEntryName == GetEntryText(pChild) ||
						nPos == nEntryRelPos )
					{
						pSelEntry = pChild;
						break;
					}
					pTemp = pChild;
					nPos++;
				}
				if(!pSelEntry)
					pSelEntry = pTemp;
				if(pSelEntry)
				{
					MakeVisible(pSelEntry);
					Select(pSelEntry);
				}
			}
		}
	}
	SetUpdateMode( sal_True );
    ScrollBar* pVScroll = GetVScroll();
    if(GetEntryCount() == nOldEntryCount &&
        nOldScrollPos && pVScroll && pVScroll->IsVisible()
        && pVScroll->GetThumbPos() != nOldScrollPos)
    {
        sal_Int32 nDelta = pVScroll->GetThumbPos() - nOldScrollPos;
        ScrollOutputArea( (short)nDelta );
    }

}

/***************************************************************************
	Beschreibung:	Im Clear muessen auch die ContentTypes geloescht werden
***************************************************************************/


void SwContentTree::Clear()
{
	SetUpdateMode(sal_False);
	SvTreeListBox::Clear();
	SetUpdateMode(sal_True);
}

/***************************************************************************
	Beschreibung:
***************************************************************************/

sal_Bool SwContentTree::FillTransferData( TransferDataContainer& rTransfer,
											sal_Int8& rDragMode )
{
	SwWrtShell* pWrtShell = GetWrtShell();
	DBG_ASSERT(pWrtShell, "keine Shell!");
	SvLBoxEntry* pEntry = GetCurEntry();
	if(!pEntry || lcl_IsContentType(pEntry) || !pWrtShell)
		return sal_False;
	String sEntry;
	SwContent* pCnt = ((SwContent*)pEntry->GetUserData());

	sal_uInt16 nActType = pCnt->GetParent()->GetType();
	String sUrl;
	sal_Bool bOutline = sal_False;
	String sOutlineText;
	switch( nActType )
	{
		case CONTENT_TYPE_OUTLINE:
		{
			sal_uInt16 nPos = ((SwOutlineContent*)pCnt)->GetPos();
            DBG_ASSERT(nPos < pWrtShell->getIDocumentOutlineNodesAccess()->getOutlineNodesCount(),
                       "outlinecnt veraendert");

            // #100738# make sure outline may actually be copied
            if( pWrtShell->IsOutlineCopyable( nPos ) )
            {
                const SwNumRule* pOutlRule = pWrtShell->GetOutlineNumRule();
                const SwTxtNode* pTxtNd =
                        pWrtShell->getIDocumentOutlineNodesAccess()->getOutlineNode(nPos);
                if( pTxtNd && pOutlRule && pTxtNd->IsNumbered())
                {
                    SwNumberTree::tNumberVector aNumVector =
                        pTxtNd->GetNumberVector();
                    for( sal_Int8 nLevel = 0;
                         nLevel <= pTxtNd->GetActualListLevel();
                         nLevel++ )
                    {
                        sal_uInt16 nVal = (sal_uInt16)aNumVector[nLevel];
                        nVal ++;
                        nVal = nVal - pOutlRule->Get(nLevel).GetStart();
                        sEntry += String::CreateFromInt32( nVal );
                        sEntry += '.';
                    }
                }
                sEntry += pWrtShell->getIDocumentOutlineNodesAccess()->getOutlineText(nPos, false);
                sOutlineText = pWrtShell->getIDocumentOutlineNodesAccess()->getOutlineText(nPos, true);
                bIsOutlineMoveable = ((SwOutlineContent*)pCnt)->IsMoveable();
                bOutline = sal_True;
            }
		}
		break;
		case CONTENT_TYPE_POSTIT:
		case CONTENT_TYPE_INDEX:
		case CONTENT_TYPE_REFERENCE :
			// koennen weder als URL noch als Bereich eingefuegt werden
		break;
		case CONTENT_TYPE_URLFIELD:
			sUrl = ((SwURLFieldContent*)pCnt)->GetURL();
		// kein break;
		case CONTENT_TYPE_OLE:
		case CONTENT_TYPE_GRAPHIC:
			if(GetParentWindow()->GetRegionDropMode() != REGION_MODE_NONE)
				break;
			else
				rDragMode &= ~( DND_ACTION_MOVE | DND_ACTION_LINK );
		default:
			sEntry = GetEntryText(pEntry);
	}

	sal_Bool bRet = sal_False;
	if(sEntry.Len())
	{
		const SwDocShell* pDocShell = pWrtShell->GetView().GetDocShell();
		if(!sUrl.Len())
		{
			if(pDocShell->HasName())
			{
				SfxMedium* pMedium = pDocShell->GetMedium();
				sUrl = pMedium->GetURLObject().GetURLNoMark();
				// nur, wenn primaer ein Link eingefuegt werden soll
				bRet = sal_True;
			}
			else if(	nActType == CONTENT_TYPE_REGION ||
						nActType == CONTENT_TYPE_BOOKMARK )
			{
				// fuer Bereich und Textmarken ist ein Link auch ohne
				// Dateiname ins eigene Dokument erlaubt
				bRet = sal_True;
			}
			else if(bIsConstant &&
					( !::GetActiveView() ||
						pActiveShell != ::GetActiveView()->GetWrtShellPtr()))
			{
				// Urls von inaktiven Views ohne Dateinamen koennen auch nicht
				// gedraggt werden
				bRet = sal_False;
			}
			else
			{
				bRet = GetParentWindow()->GetRegionDropMode() == REGION_MODE_NONE;
				rDragMode = DND_ACTION_MOVE;
			}

			const String& rToken = pCnt->GetParent()->GetTypeToken();
			sUrl += '#';
			sUrl += sEntry;
			if(rToken.Len())
			{
				sUrl += cMarkSeperator;
				sUrl += rToken;
			}
		}
		else
			bRet = sal_True;

		if( bRet )
		{
			//fuer Outlines muss in die Description der Ueberschrifttext mit der echten Nummer
			if(bOutline)
				sEntry = sOutlineText;

			{
				NaviContentBookmark aBmk( sUrl, sEntry,
									GetParentWindow()->GetRegionDropMode(),
									pDocShell);
				aBmk.Copy( rTransfer );
			}

			// fuer fremde DocShells muss eine INetBookmark
			// dazugeliefert werden
			if( pDocShell->HasName() )
			{
				INetBookmark aBkmk( sUrl, sEntry );
				rTransfer.CopyINetBookmark( aBkmk );
			}
		}
	}
	return bRet;
}
/***************************************************************************
	Beschreibung:	Umschalten der Anzeige auf Root
***************************************************************************/


sal_Bool SwContentTree::ToggleToRoot()
{
	if(!bIsRoot)
	{
		SvLBoxEntry* pEntry = GetCurEntry();
		const SwContentType* pCntType;
		if(pEntry)
		{
			if(lcl_IsContentType(pEntry))
				pCntType = (SwContentType*)pEntry->GetUserData();
			else
				pCntType = ((SwContent*)pEntry->GetUserData())->GetParent();
			nRootType = pCntType->GetType();
			bIsRoot = sal_True;
			Display(bIsActive || bIsConstant);
		}
	}
	else
	{
		nRootType = USHRT_MAX;
		bIsRoot = sal_False;
		FindActiveTypeAndRemoveUserData();
		Display(bIsActive || bIsConstant);
	}
	pConfig->SetRootType( nRootType );
	GetParentWindow()->aContentToolBox.CheckItem(FN_SHOW_ROOT, bIsRoot);
	return bIsRoot;
}

/***************************************************************************
	Beschreibung:	Angezeigten Inhalt auf Gueltigkeit pruefen
***************************************************************************/


sal_Bool SwContentTree::HasContentChanged()
{
/*
	-Parallel durch das lokale Array und die Treelistbox laufen.
	-Sind die Eintraege nicht expandiert, werden sie nur im Array verworfen
	und der Contenttype wird als UserData neu gesetzt.
	- ist der Root-Modus aktiv, wird nur dieser aktualisiert,
	fuer die nicht angezeigten Inhaltstypen gilt:
		die Memberliste wird geloescht und der Membercount aktualisiert
	Wenn Inhalte ueberprueft werden, werden gleichzeitig die vorhanden
	Memberlisten aufgefuellt. Sobald ein Unterschied auftritt wird nur noch
	gefuellt und nicht mehr ueberprueft. Abschliessend wird die Box neu gefuellt.

*/

	sal_Bool bRepaint = sal_False;
	sal_Bool bInvalidate = sal_False;

	if(!bIsActive && ! bIsConstant)
	{
		for(sal_uInt16 i=0; i < CONTENT_TYPE_MAX; i++)
		{
			if(aActiveContentArr[i])
				aActiveContentArr[i]->Invalidate();
		}
	}
	else if(bIsRoot)
	{
		sal_Bool bOutline = sal_False;
		SvLBoxEntry* pEntry = First();
		if(!pEntry)
			bRepaint = sal_True;
		else
		{
			sal_uInt16 nType = ((SwContentType*)pEntry->GetUserData())->GetType();
			bOutline = nRootType == CONTENT_TYPE_OUTLINE;
			SwContentType* pArrType = aActiveContentArr[nType];
			if(!pArrType)
				bRepaint = sal_True;
			else
			{
				sal_uInt16 nSelLevel = USHRT_MAX;

				SvLBoxEntry* pFirstSel;
				if(bOutline &&
						0 != ( pFirstSel = FirstSelected()) &&
							lcl_IsContent(pFirstSel))
				{
					nSelLevel = ((SwOutlineContent*)pFirstSel->GetUserData())->GetOutlineLevel();
					SwWrtShell* pSh = GetWrtShell();
					sal_uInt16 nOutlinePos = pSh->GetOutlinePos(MAXLEVEL);
                    bRepaint |= nOutlinePos != USHRT_MAX &&
                                pSh->getIDocumentOutlineNodesAccess()->getOutlineLevel(nOutlinePos) != nSelLevel;
				}

				pArrType->Init(&bInvalidate);
				pArrType->FillMemberList();
				pEntry->SetUserData((void*)pArrType);
				if(!bRepaint)
				{
					if(GetChildCount(pEntry) != pArrType->GetMemberCount())
							bRepaint = sal_True;
					else
					{
						sal_uInt16 nChildCount = (sal_uInt16)GetChildCount(pEntry);
						for(sal_uInt16 j = 0; j < nChildCount; j++)
						{
							pEntry = Next(pEntry);
							const SwContent* pCnt = pArrType->GetMember(j);
							pEntry->SetUserData((void*)pCnt);
							String sEntryText = GetEntryText(pEntry);
							if( sEntryText != pCnt->GetName() &&
								!(sEntryText == sSpace && !pCnt->GetName().Len()))
								bRepaint = sal_True;
						}
					}
				}
			}
		}
		if( !bRepaint && bOutline )
		{
			//feststellen, wo der Cursor steht
			const sal_uInt16 nActPos = GetWrtShell()->GetOutlinePos(MAXLEVEL);
            SvLBoxEntry* pFirstEntry = First();

            while( 0 != (pFirstEntry = Next(pFirstEntry)) )
			{
                if(((SwOutlineContent*)pFirstEntry->GetUserData())->GetPos() == nActPos)
				{
                    if(FirstSelected() != pFirstEntry)
					{
                        Select(pFirstEntry);
                        MakeVisible(pFirstEntry);
					}
				}
			}

		}

	}
	else
	{
		SvLBoxEntry* pEntry = First();
		while ( pEntry )
		{
			sal_Bool bNext = sal_True; // mindestens ein Next muss sein
			SwContentType* pTreeType = (SwContentType*)pEntry->GetUserData();
			sal_uInt16 nType = pTreeType->GetType();
			sal_uInt16 nTreeCount = pTreeType->GetMemberCount();
			SwContentType* pArrType = aActiveContentArr[nType];
			if(!pArrType)
				bRepaint = sal_True;
			else
			{
				pArrType->Init(&bInvalidate);
				pEntry->SetUserData((void*)pArrType);
				if(IsExpanded(pEntry))
				{
					sal_Bool bLevelOrVisibiblityChanged = sal_False;
					// bLevelOrVisibiblityChanged is set if outlines have changed their level
					// or if the visibility of objects (frames, sections, tables) has changed
					// i.e. in header/footer
					pArrType->FillMemberList(&bLevelOrVisibiblityChanged);
					if(bLevelOrVisibiblityChanged)
						bInvalidate = sal_True;
					sal_uInt16 nChildCount = (sal_uInt16)GetChildCount(pEntry);
					if(bLevelOrVisibiblityChanged)
						bInvalidate = sal_True;

					if(nChildCount != pArrType->GetMemberCount())
						bRepaint = sal_True;
					else
					{
						for(sal_uInt16 j = 0; j < nChildCount; j++)
						{
							pEntry = Next(pEntry);
							bNext = sal_False;
							const SwContent* pCnt = pArrType->GetMember(j);
							pEntry->SetUserData((void*)pCnt);
							String sEntryText = GetEntryText(pEntry);
							if( sEntryText != pCnt->GetName() &&
								!(sEntryText == sSpace && !pCnt->GetName().Len()))
								bRepaint = sal_True;
						}
					}

				}
				else if(pEntry->HasChilds())
				{
					//war der Eintrag einmal aufgeklappt, dann muessen auch
					// die unsichtbaren Eintraege geprueft werden.
					// zumindest muessen die Userdaten aktualisiert werden
					sal_Bool bLevelOrVisibiblityChanged = sal_False;
					// bLevelOrVisibiblityChanged is set if outlines have changed their level
					// or if the visibility of objects (frames, sections, tables) has changed
					// i.e. in header/footer
					pArrType->FillMemberList(&bLevelOrVisibiblityChanged);
					sal_Bool bRemoveChildren = sal_False;
					sal_uInt16 nChildCount = (sal_uInt16)GetChildCount(pEntry);
					if( nChildCount != pArrType->GetMemberCount() )
					{
						bRemoveChildren = sal_True;
					}
					else
					{
						SvLBoxEntry* pChild = FirstChild(pEntry);
						for(sal_uInt16 j = 0; j < nChildCount; j++)
						{
							const SwContent* pCnt = pArrType->GetMember(j);
							pChild->SetUserData((void*)pCnt);
							String sEntryText = GetEntryText(pChild);
							if( sEntryText != pCnt->GetName() &&
								!(sEntryText == sSpace && !pCnt->GetName().Len()))
								bRemoveChildren = sal_True;
							pChild = Next(pChild);
						}
					}
					if(bRemoveChildren)
					{
						SvLBoxEntry* pChild = FirstChild(pEntry);
						SvLBoxEntry* pRemove = pChild;
						for(sal_uInt16 j = 0; j < nChildCount; j++)
						{
							pChild = Next(pRemove);
							GetModel()->Remove(pRemove);
							pRemove = pChild;
						}
					}
					if(!nChildCount)
					{
						pEntry->EnableChildsOnDemand(sal_False);
						InvalidateEntry(pEntry);
					}

				}
				else if((nTreeCount != 0)
							!= (pArrType->GetMemberCount()!=0))
				{
					bRepaint = sal_True;
				}
			}
			//hier muss noch der naechste Root-Entry gefunden werden
			while( pEntry && (bNext || GetParent(pEntry ) ))
			{
				pEntry = Next(pEntry);
				bNext = sal_False;
			}
		}
	}
	if(!bRepaint && bInvalidate)
		Invalidate();
	return bRepaint;
}

/***************************************************************************
	Beschreibung: 	Bevor alle Daten geloescht werden, soll noch der letzte
 * 					aktive Eintrag festgestellt werden. Dann werden die
 * 					UserData geloescht
***************************************************************************/
void SwContentTree::FindActiveTypeAndRemoveUserData()
{
	SvLBoxEntry* pEntry = FirstSelected();
	if(pEntry)
	{
		// wird Clear ueber TimerUpdate gerufen, kann nur fuer die Root
		// die Gueltigkeit der UserData garantiert werden
		SvLBoxEntry* pParent;
		while(0 != (pParent = GetParent(pEntry)))
			pEntry = pParent;
		if(pEntry->GetUserData() && lcl_IsContentType(pEntry))
			nLastSelType = ((SwContentType*)pEntry->GetUserData())->GetType();
	}
//	else
//		nLastSelType = USHRT_MAX;
	pEntry = First();
	while(pEntry)
	{
		pEntry->SetUserData(0);
		pEntry = Next(pEntry);
	}
}

/***************************************************************************
	Beschreibung: 	Nachdem ein File auf den Navigator gedroppt wurde,
					wird die neue Shell gesetzt
***************************************************************************/


void SwContentTree::SetHiddenShell(SwWrtShell* pSh)
{
	pHiddenShell = pSh;
	bIsHidden = sal_True;
	bIsActive = bIsConstant = sal_False;
	FindActiveTypeAndRemoveUserData();
	for(sal_uInt16 i=0; i < CONTENT_TYPE_MAX; i++)
	{
		DELETEZ(aHiddenContentArr[i]);
	}
	Display(bIsActive);

	GetParentWindow()->UpdateListBox();
}
/***************************************************************************
	Beschreibung:	Dokumentwechsel - neue Shell setzen
***************************************************************************/


void SwContentTree::SetActiveShell(SwWrtShell* pSh)
{
	if(bIsInternalDrag)
		bDocChgdInDragging = sal_True;
	sal_Bool bClear = pActiveShell != pSh;
	if(bIsActive && bClear)
	{
		pActiveShell = pSh;
		FindActiveTypeAndRemoveUserData();
		Clear();
	}
	else if(bIsConstant)
	{
		if(!lcl_FindShell(pActiveShell))
		{
			pActiveShell = pSh;
			bIsActive = sal_True;
			bIsConstant = sal_False;
			bClear = sal_True;
		}
	}
	// nur wenn es die aktive View ist, wird das Array geloescht und
	// die Anzeige neu gefuellt
	if(bIsActive && bClear)
	{
		FindActiveTypeAndRemoveUserData();
		for(sal_uInt16 i=0; i < CONTENT_TYPE_MAX; i++)
		{
			DELETEZ(aActiveContentArr[i]);
		}
		Display(sal_True);
	}
}

/***************************************************************************
	Beschreibung: 	Eine offene View als aktiv festlegen
***************************************************************************/


void SwContentTree::SetConstantShell(SwWrtShell* pSh)
{
	pActiveShell = pSh;
	bIsActive 		= sal_False;
	bIsConstant 	= sal_True;
	FindActiveTypeAndRemoveUserData();
	for(sal_uInt16 i=0; i < CONTENT_TYPE_MAX; i++)
	{
		DELETEZ(aActiveContentArr[i]);
	}
	Display(sal_True);
}
/***************************************************************************
	Beschreibung:	Kommandos des Navigators ausfuehren
***************************************************************************/


void SwContentTree::ExecCommand(sal_uInt16 nCmd, sal_Bool bModifier)
{
	sal_Bool nMove = sal_False;
	switch( nCmd )
	{
		case FN_ITEM_DOWN:
		case FN_ITEM_UP:   nMove = sal_True;
		case FN_ITEM_LEFT:
		case FN_ITEM_RIGHT:
		if( !GetWrtShell()->GetView().GetDocShell()->IsReadOnly() &&
				(bIsActive ||
                    (bIsConstant && pActiveShell == GetParentWindow()->GetCreateView()->GetWrtShellPtr())))
		{
			SwWrtShell* pShell = GetWrtShell();
			sal_Int8 nActOutlineLevel = nOutlineLevel;
			sal_uInt16 nActPos = pShell->GetOutlinePos(nActOutlineLevel);
			SvLBoxEntry* pFirstEntry = FirstSelected();
			if (pFirstEntry && lcl_IsContent(pFirstEntry))
			{
				if((bIsRoot && nRootType == CONTENT_TYPE_OUTLINE) ||
					((SwContent*)pFirstEntry->GetUserData())->GetParent()->GetType()
												== 	CONTENT_TYPE_OUTLINE)
				{
					nActPos = ((SwOutlineContent*)pFirstEntry->GetUserData())->GetPos();
				}
			}
			if ( nActPos < USHRT_MAX &&
					( !nMove || pShell->IsOutlineMovable( nActPos )) )
			{
				pShell->StartAllAction();
				pShell->GotoOutline( nActPos); // Falls Textselektion != BoxSelektion
				pShell->Push();
				pShell->MakeOutlineSel( nActPos, nActPos,
									bModifier);
				if( nMove )
				{
					short nDir = nCmd == FN_ITEM_UP ? -1 : 1;
					if( !bModifier && ((nDir == -1 && nActPos > 0) ||
						(nDir == 1 && nActPos < GetEntryCount() - 2 )) )
					{
						pShell->MoveOutlinePara( nDir );
						//Cursor wieder an die aktuelle Position setzen
						pShell->GotoOutline( nActPos + nDir);
					}
					else if(bModifier)
					{
						sal_uInt16 nActEndPos = nActPos;
						SvLBoxEntry* pEntry = pFirstEntry;
						sal_uInt16 nActLevel = ((SwOutlineContent*)
								pFirstEntry->GetUserData())->GetOutlineLevel();
						pEntry = Next(pEntry);
						while( pEntry && CONTENT_TYPE_OUTLINE ==
							((SwTypeNumber*)pEntry->GetUserData())->GetTypeId() )
						{
							if(nActLevel >= ((SwOutlineContent*)
								pEntry->GetUserData())->GetOutlineLevel())
								break;
							pEntry = Next(pEntry);
							nActEndPos++;
						}
						sal_uInt16 nDest;
						if(nDir == 1)
						{
							//Wenn der letzte Eintrag bewegt werden soll
							//ist Schluss
							if(pEntry && CONTENT_TYPE_OUTLINE ==
								((SwTypeNumber*)pEntry->GetUserData())->GetTypeId())
							{
								// pEntry zeigt jetzt auf den
								// dem letzten sel. Eintrag folgenden E.
								nDest = nActEndPos;
								nDest++;
								//hier muss der uebernaechste Eintrag
								//gefunden werden. Die Selektion muss davor eingefuegt
								//werden
								while(pEntry )
								{
									pEntry = Next(pEntry);
									// nDest++ darf nur ausgefuehrt werden,
									// wenn pEntry != 0
									if(pEntry && nDest++ &&
									( nActLevel >= ((SwOutlineContent*)pEntry->GetUserData())->GetOutlineLevel()||
									 CONTENT_TYPE_OUTLINE != ((SwTypeNumber*)pEntry->GetUserData())->GetTypeId()))
									{
										nDest--;
										break;
									}
								}
								nDir = nDest - nActEndPos;
								//wenn kein Eintrag gefunden wurde, der der Bedingung
								//fuer das zuvor Einfuegen entspricht, muss etwas weniger
								//geschoben werden
							}
							else
								nDir = 0;
						}
						else
						{
							nDest = nActPos;
							pEntry = pFirstEntry;
							while(pEntry && nDest )
							{
								nDest--;
								pEntry = Prev(pEntry);
								if(pEntry &&
									(nActLevel >= ((SwOutlineContent*)pEntry->GetUserData())->GetOutlineLevel()||
									CONTENT_TYPE_OUTLINE !=
								((SwTypeNumber*)pEntry->GetUserData())->GetTypeId()))
								{
									break;
								}
							}
							nDir = nDest - nActPos;
						}
						if(nDir)
						{
							pShell->MoveOutlinePara( nDir );
							//Cursor wieder an die aktuelle Position setzen
							pShell->GotoOutline( nActPos + nDir);
						}
					}
				}
				else
				{
					if( pShell->IsProtectedOutlinePara() )
						Sound::Beep(); //konnte nicht umgestuft werden
					else
						pShell->OutlineUpDown( nCmd == FN_ITEM_LEFT ? -1 : 1 );
				}

				pShell->ClearMark();
				pShell->Pop(sal_False); //Cursor steht jetzt wieder an der akt. Ueberschrift
				pShell->EndAllAction();
				if(aActiveContentArr[CONTENT_TYPE_OUTLINE])
					aActiveContentArr[CONTENT_TYPE_OUTLINE]->Invalidate();
				Display(sal_True);
				if(!bIsRoot)
				{
                    const sal_uInt16 nCurrPos = pShell->GetOutlinePos(MAXLEVEL);
                    SvLBoxEntry* pFirst = First();

                    while( 0 != (pFirst = Next(pFirst)) && lcl_IsContent(pFirst))
					{
                        if(((SwOutlineContent*)pFirst->GetUserData())->GetPos() == nCurrPos)
						{
                            Select(pFirst);
                            MakeVisible(pFirst);
						}
					}
				}
			}
			else
				Sound::Beep(); //konnte nicht verschoben werden
		}
	}
}
/***************************************************************************
	Beschreibung:
***************************************************************************/


void	SwContentTree::ShowTree()
{
	aUpdTimer.Start();
	SvTreeListBox::Show();
}

/***************************************************************************
	Beschreibung:	zusammengefaltet wird nicht geidlet
***************************************************************************/


void	SwContentTree::HideTree()
{
	aUpdTimer.Stop();
	SvTreeListBox::Hide();
}

/***************************************************************************
	Beschreibung:	Kein Idle mit Focus oder waehrend des Dragging
***************************************************************************/


IMPL_LINK( SwContentTree, TimerUpdate, Timer*, EMPTYARG)
{
	// kein Update waehrend D&D
	// Viewabfrage, da der Navigator zu spaet abgeraeumt wird
    SwView* pView = GetParentWindow()->GetCreateView();
	if( (!HasFocus() || bViewHasChanged) &&
		 !bIsInDrag && !bIsInternalDrag && pView &&
		 pView->GetWrtShellPtr() && !pView->GetWrtShellPtr()->ActionPend() )
	{
		bViewHasChanged = sal_False;
		bIsIdleClear = sal_False;
		SwWrtShell* pActShell = pView->GetWrtShellPtr();
		if( bIsConstant && !lcl_FindShell( pActiveShell ) )
		{
			SetActiveShell(pActShell);
			GetParentWindow()->UpdateListBox();
		}

		if(bIsActive && pActShell != GetWrtShell())
			SetActiveShell(pActShell);
		else if( (bIsActive || (bIsConstant && pActShell == GetWrtShell())) &&
					HasContentChanged())
		{
			FindActiveTypeAndRemoveUserData();
			Display(sal_True);
			//Solution: Set focus
			if( bIsKeySpace )
			{
				HideFocus();
				ShowFocus( oldRectangle);
				bIsKeySpace = sal_False;
			}
		}
	}
	else if(!pView && bIsActive && !bIsIdleClear)
	{
		if(pActiveShell)
			SetActiveShell(0);
		Clear();
		bIsIdleClear = sal_True;
	}
	return 0;
}

/***************************************************************************
	Beschreibung:
***************************************************************************/


DragDropMode SwContentTree::NotifyStartDrag(
				TransferDataContainer& rContainer,
				SvLBoxEntry* pEntry )
{
	DragDropMode eMode = (DragDropMode)0;
	if( bIsActive && nRootType == CONTENT_TYPE_OUTLINE &&
		GetModel()->GetAbsPos( pEntry ) > 0
		&& !GetWrtShell()->GetView().GetDocShell()->IsReadOnly())
		eMode = GetDragDropMode();
	else if(!bIsActive && GetWrtShell()->GetView().GetDocShell()->HasName())
		eMode = SV_DRAGDROP_APP_COPY;

	sal_Int8 nDragMode;
	FillTransferData( rContainer, nDragMode );
	bDocChgdInDragging = sal_False;
	bIsInternalDrag = sal_True;
	return eMode;
}


/***************************************************************************
	Beschreibung :	Nach dem Drag wird der aktuelle Absatz m i t
					Childs verschoben
***************************************************************************/


sal_Bool  SwContentTree::NotifyMoving( SvLBoxEntry*  pTarget,
		SvLBoxEntry*  pEntry, SvLBoxEntry*& , sal_uLong& )
{
	if(!bDocChgdInDragging)
	{
		sal_uInt16 nTargetPos = 0;
		sal_uInt16 nSourcePos = (( SwOutlineContent* )pEntry->GetUserData())->GetPos();
		if(!lcl_IsContent(pTarget))
			nTargetPos = USHRT_MAX;
		else
			nTargetPos = (( SwOutlineContent* )pTarget->GetUserData())->GetPos();
		if(	MAXLEVEL > nOutlineLevel && // werden nicht alle Ebenen angezeigt
						nTargetPos != USHRT_MAX)
		{
			SvLBoxEntry* pNext = Next(pTarget);
			if(pNext)
				nTargetPos = (( SwOutlineContent* )pNext->GetUserData())->GetPos() -1;
			else
                nTargetPos = static_cast<sal_uInt16>(GetWrtShell()->getIDocumentOutlineNodesAccess()->getOutlineNodesCount())- 1;

		}

		DBG_ASSERT( pEntry &&
			lcl_IsContent(pEntry),"Source == 0 oder Source hat keinen Content" );
		GetParentWindow()->MoveOutline( nSourcePos,
									nTargetPos,
									sal_True);

		aActiveContentArr[CONTENT_TYPE_OUTLINE]->Invalidate();
		Display(sal_True);
	}
	//TreeListBox wird aus dem Dokument neu geladen
	return sal_False;
}
/***************************************************************************
	Beschreibung : 	Nach dem Drag wird der aktuelle Absatz o h n e
					Childs verschoben
***************************************************************************/


sal_Bool  SwContentTree::NotifyCopying( SvLBoxEntry*  pTarget,
		SvLBoxEntry*  pEntry, SvLBoxEntry*& , sal_uLong& )
{
	if(!bDocChgdInDragging)
	{
		sal_uInt16 nTargetPos = 0;
		sal_uInt16 nSourcePos = (( SwOutlineContent* )pEntry->GetUserData())->GetPos();
		if(!lcl_IsContent(pTarget))
			nTargetPos = USHRT_MAX;
		else
			nTargetPos = (( SwOutlineContent* )pTarget->GetUserData())->GetPos();

		if(	MAXLEVEL > nOutlineLevel && // werden nicht alle Ebenen angezeigt
						nTargetPos != USHRT_MAX)
		{
			SvLBoxEntry* pNext = Next(pTarget);
			if(pNext)
				nTargetPos = (( SwOutlineContent* )pNext->GetUserData())->GetPos() - 1;
			else
                nTargetPos = static_cast<sal_uInt16>(GetWrtShell()->getIDocumentOutlineNodesAccess()->getOutlineNodesCount()) - 1;

		}


		DBG_ASSERT( pEntry &&
			lcl_IsContent(pEntry),"Source == 0 oder Source hat keinen Content" );
		GetParentWindow()->MoveOutline( nSourcePos,	nTargetPos, sal_False);

		//TreeListBox wird aus dem Dokument neu geladen
		aActiveContentArr[CONTENT_TYPE_OUTLINE]->Invalidate();
		Display(sal_True);
	}
	return sal_False;
}

/***************************************************************************
	Beschreibung:	Kein Drop vor den ersten Eintrag - es ist ein SwContentType
***************************************************************************/

sal_Bool SwContentTree::NotifyAcceptDrop( SvLBoxEntry* pEntry)
{
	return pEntry != 0;
}


/***************************************************************************
	Beschreibung: 	Wird ein Ctrl+DoubleClick in einen freien Bereich ausgefuehrt,
 * 					dann soll die Basisfunktion des Controls gerufen werden
***************************************************************************/
void SwContentTree::MouseButtonDown( const MouseEvent& rMEvt )
{
	Point aPos( rMEvt.GetPosPixel());
	SvLBoxEntry* pEntry = GetEntry( aPos, sal_True );
	if( !pEntry && rMEvt.IsLeft() && rMEvt.IsMod1() && (rMEvt.GetClicks() % 2) == 0)
		Control::MouseButtonDown( rMEvt );
	else
		SvTreeListBox::MouseButtonDown( rMEvt );
}

/***************************************************************************
	Beschreibung:	sofort aktualisieren
***************************************************************************/


void SwContentTree::GetFocus()
{
	SwView* pActView = GetParentWindow()->GetCreateView();
	if(pActView)
	{
		SwWrtShell* pActShell = pActView->GetWrtShellPtr();
		if(bIsConstant && !lcl_FindShell(pActiveShell))
		{
			SetActiveShell(pActShell);
		}

		if(bIsActive && pActShell != GetWrtShell())
			SetActiveShell(pActShell);
		else if( (bIsActive || (bIsConstant && pActShell == GetWrtShell())) &&
					HasContentChanged())
		{
			Display(sal_True);
		}
	}
	else if(bIsActive)
		Clear();
	SvTreeListBox::GetFocus();
}

/***************************************************************************
	Beschreibung:
***************************************************************************/


void SwContentTree::KeyInput(const KeyEvent& rEvent)
{
	const KeyCode aCode = rEvent.GetKeyCode();
	if(aCode.GetCode() == KEY_RETURN)
	{
		SvLBoxEntry* pEntry = FirstSelected();
		if ( pEntry )
		{
			switch(aCode.GetModifier())
			{
				case KEY_MOD2:
					// Boxen umschalten
					GetParentWindow()->ToggleTree();
				break;
				case KEY_MOD1:
					// RootModus umschalten
					ToggleToRoot();
				break;
				case 0:
					if(lcl_IsContentType(pEntry))
					{
						IsExpanded(pEntry) ?
							Collapse(pEntry) :
								Expand(pEntry);
					}
					else
						ContentDoubleClickHdl(0);
				break;
			}
		}
	}
	else if(aCode.GetCode() == KEY_DELETE && 0 == aCode.GetModifier())
	{
		SvLBoxEntry* pEntry = FirstSelected();
		if(pEntry &&
			lcl_IsContent(pEntry) &&
				((SwContent*)pEntry->GetUserData())->GetParent()->IsDeletable() &&
					!pActiveShell->GetView().GetDocShell()->IsReadOnly())
		{
			EditEntry(pEntry, EDIT_MODE_DELETE);
            bViewHasChanged = sal_True;
            GetParentWindow()->UpdateListBox();
            TimerUpdate(&aUpdTimer);
            GrabFocus();
        }
	}
	//Solution: Make KEY_SPACE has same function as DoubleClick ,
	//and realize multi-selection .
	else if(aCode.GetCode() == KEY_SPACE && 0 == aCode.GetModifier())
	{

		SvLBoxEntry* pEntry = GetCurEntry();
		if( GetChildCount( pEntry ) == 0 )
			bIsKeySpace = sal_True;
		Point tempPoint = GetEntryPosition( pEntry );//Change from "GetEntryPos" to "GetEntryPosition" for acc migration
		oldRectangle = GetFocusRect( pEntry,tempPoint.Y() );

		if(pEntry)
		{
			if(bIsActive || bIsConstant)
			{
				if(bIsConstant)
				{
					pActiveShell->GetView().GetViewFrame()->GetWindow().ToTop();
				}

				SwContent* pCnt = (SwContent*)pEntry->GetUserData();

				sal_uInt16 nJumpType = pCnt->GetParent()->GetType();
				switch(nJumpType)
				{
					case CONTENT_TYPE_DRAWOBJECT:
					{
						SdrView* pDrawView = pActiveShell->GetDrawView();
						if (pDrawView)
						{
							pDrawView->SdrEndTextEdit();//Change from "EndTextEdit" to "SdrEndTextEdit" for acc migration

                            SwDrawModel* pDrawModel = pActiveShell->GetDoc()->GetDrawModel();
                            SdrPage* pPage = pDrawModel->GetPage(0);
							sal_uInt32 nCount = pPage->GetObjCount();
							sal_Bool hasObjectMarked = sal_False;

							SdrObject* pObject = NULL;
							pObject = GetDrawingObjectsByContent( pCnt );
							if( pObject )
							{
								SdrPageView* pPV = pDrawView->GetSdrPageView/*GetPageViewPvNum*/(/*0*/);
								if( pPV )
								{
									sal_Bool bUnMark = pDrawView->IsObjMarked(pObject);
									pDrawView->MarkObj( pObject, pPV, bUnMark);

								}
							}
							for( sal_uInt32 i=0; i< nCount; i++ )
							{
								SdrObject* pTemp = pPage->GetObj(i);
								sal_uInt16 nCmpId;
								sal_Bool bMark = pDrawView->IsObjMarked(pTemp);
								switch( pTemp->GetObjIdentifier() )
								{
									case OBJ_GRUP:
									case OBJ_TEXT:
									case OBJ_TEXTEXT:
									case OBJ_wegFITTEXT:
									case OBJ_LINE:
									case OBJ_RECT:
									case OBJ_CIRC:
									case OBJ_SECT:
									case OBJ_CARC:
									case OBJ_CCUT:
									case OBJ_POLY:
									case OBJ_PLIN:
									case OBJ_PATHLINE:
									case OBJ_PATHFILL:
									case OBJ_FREELINE:
									case OBJ_FREEFILL:
									case OBJ_PATHPOLY:
									case OBJ_PATHPLIN:
									case OBJ_CAPTION:
									case OBJ_CUSTOMSHAPE:
										nCmpId = OBJ_GRUP;
										if( bMark )
											hasObjectMarked = sal_True;
										break;
									default:
										nCmpId = pTemp->GetObjIdentifier();
										if ( bMark )
										{
											SdrPageView* pPV = pDrawView->GetSdrPageView/*GetPageViewPvNum*/(/*0*/);
											if (pPV)
											{
												pDrawView->MarkObj(pTemp, pPV, sal_True);
											}
										}
								}
								//mod end
							}
							if ( pActiveShell && !hasObjectMarked )
							{
								SwEditWin& pEditWindow =
									pActiveShell->GetView().GetEditWin();
								if( &pEditWindow )
								{
									KeyCode tempKeycode( KEY_ESCAPE );
									KeyEvent rKEvt( 0 , tempKeycode );
									((Window*)&pEditWindow)->KeyInput( rKEvt );

								}
								//rView.GetEditWin().GrabFocus();
							}
						}
					}
					break;
				}


				bViewHasChanged = sal_True;
			}
		}

	}
	else
		SvTreeListBox::KeyInput(rEvent);

}

/***************************************************************************
	Beschreibung:
***************************************************************************/


void SwContentTree::RequestHelp( const HelpEvent& rHEvt )
{
	sal_Bool bCallBase = sal_True;
	if( rHEvt.GetMode() & HELPMODE_QUICK )
	{
		Point aPos( ScreenToOutputPixel( rHEvt.GetMousePosPixel() ));
		SvLBoxEntry* pEntry = GetEntry( aPos );
		if( pEntry )
		{
			sal_uInt16 nType;
			sal_Bool bBalloon = sal_False;
			sal_Bool bContent = sal_False;
			void* pUserData = pEntry->GetUserData();
			if(lcl_IsContentType(pEntry))
				nType = ((SwContentType*)pUserData)->GetType();
			else
			{
				nType = ((SwContent*)pUserData)->GetParent()->GetType();
				bContent = sal_True;
			}
			String sEntry;
			sal_Bool bRet = sal_False;
			if(bContent)
			{
				switch( nType )
				{
					case CONTENT_TYPE_URLFIELD:
						sEntry = ((SwURLFieldContent*)pUserData)->GetURL();
						bRet = sal_True;
					break;

					case CONTENT_TYPE_POSTIT:
						sEntry = ((SwPostItContent*)pUserData)->GetName();
						bRet = sal_True;
						if(Help::IsBalloonHelpEnabled())
							bBalloon = sal_True;
					break;
					case CONTENT_TYPE_OUTLINE:
						sEntry = ((SwOutlineContent*)pUserData)->GetName();
						bRet = sal_True;
					break;
					case CONTENT_TYPE_GRAPHIC:
						sEntry = ((SwGraphicContent*)pUserData)->GetLink();
#if OSL_DEBUG_LEVEL > 1
						sEntry += ' ';
						sEntry += String::CreateFromInt32(
									((SwGraphicContent*)pUserData)->GetYPos());
#endif
						bRet = sal_True;
					break;
#if OSL_DEBUG_LEVEL > 1
					case CONTENT_TYPE_TABLE:
					case CONTENT_TYPE_FRAME:
						sEntry = String::CreateFromInt32(
										((SwContent*)pUserData)->GetYPos() );
						bRet = sal_True;
					break;
#endif
				}
				if(((SwContent*)pUserData)->IsInvisible())
				{
					if(sEntry.Len())
						sEntry += C2S(", ");
					sEntry += sInvisible;
					bRet = sal_True;
				}
			}
			else
			{
				sal_uInt16 nMemberCount = ((SwContentType*)pUserData)->GetMemberCount();
				sEntry = String::CreateFromInt32(nMemberCount);
				sEntry += ' ';
				sEntry += nMemberCount == 1
							? ((SwContentType*)pUserData)->GetSingleName()
							: ((SwContentType*)pUserData)->GetName();
				bRet = sal_True;
			}
			if(bRet)
			{
				SvLBoxTab* pTab;
				SvLBoxItem* pItem = GetItem( pEntry, aPos.X(), &pTab );
				if( pItem && SV_ITEM_ID_LBOXSTRING == pItem->IsA())
				{
					aPos = GetEntryPosition( pEntry );

					aPos.X() = GetTabPos( pEntry, pTab );
					Size aSize( pItem->GetSize( this, pEntry ) );

					if((aPos.X() + aSize.Width()) > GetSizePixel().Width())
						aSize.Width() = GetSizePixel().Width() - aPos.X();

					aPos = OutputToScreenPixel(aPos);
					Rectangle aItemRect( aPos, aSize );
					if(bBalloon)
					{
						aPos.X() += aSize.Width();
						Help::ShowBalloon( this, aPos, aItemRect, sEntry );
					}
					else
						Help::ShowQuickHelp( this, aItemRect, sEntry,
							QUICKHELP_LEFT|QUICKHELP_VCENTER );
					bCallBase = sal_False;
				}
			}
			else
			{
				Help::ShowQuickHelp( this, Rectangle(), aEmptyStr, 0 );
				bCallBase = sal_False;
			}
		}
	}
	if( bCallBase )
		Window::RequestHelp( rHEvt );
}

/***************************************************************************
	Beschreibung:
***************************************************************************/


void SwContentTree::ExcecuteContextMenuAction( sal_uInt16 nSelectedPopupEntry )
{
	SvLBoxEntry* pFirst = FirstSelected();
	switch( nSelectedPopupEntry )
	{
		//Outlinelevel
		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		case 106:
		case 107:
		case 108:
		case 109:
		case 110:
            nSelectedPopupEntry -= 100;
            if(nOutlineLevel != nSelectedPopupEntry )
                SetOutlineLevel((sal_Int8)nSelectedPopupEntry);
		break;
		case 201:
		case 202:
		case 203:
            GetParentWindow()->SetRegionDropMode(nSelectedPopupEntry - 201);
		break;
		case 401:
		case 402:
            EditEntry(pFirst, nSelectedPopupEntry == 401 ? EDIT_MODE_RMV_IDX : EDIT_MODE_UPD_IDX);
		break;
		// Eintrag bearbeiten
		case 403:
			EditEntry(pFirst, EDIT_MODE_EDIT);
		break;
		case 404:
			EditEntry(pFirst, EDIT_UNPROTECT_TABLE);
		break;
		case 405 :
		{
			const SwTOXBase* pBase = ((SwTOXBaseContent*)pFirst->GetUserData())
																->GetTOXBase();
            pActiveShell->SetTOXBaseReadonly(*pBase, !pActiveShell->IsTOXBaseReadonly(*pBase));
		}
		break;
		case 4:
		break;
		case 501:
			EditEntry(pFirst, EDIT_MODE_DELETE);
		break;
		case 502 :
			EditEntry(pFirst, EDIT_MODE_RENAME);
		break;
		case 600:
			pActiveShell->GetView().GetPostItMgr()->Show();
			break;
		case 601:
			pActiveShell->GetView().GetPostItMgr()->Hide();
			break;
		case 602:
			{
                pActiveShell->GetView().GetPostItMgr()->SetActiveSidebarWin(0);
				pActiveShell->GetView().GetPostItMgr()->Delete();
				break;
			}
		//Anzeige
        default: // nSelectedPopupEntry > 300
        if(nSelectedPopupEntry > 300 && nSelectedPopupEntry < 400)
		{
            nSelectedPopupEntry -= 300;
			SwView *pView = SwModule::GetFirstView();
			while (pView)
			{
                nSelectedPopupEntry --;
                if(nSelectedPopupEntry == 0)
				{
					SetConstantShell(&pView->GetWrtShell());
					break;
				}
				pView = SwModule::GetNextView(pView);
			}
            if(nSelectedPopupEntry)
			{
                bViewHasChanged = bIsActive = nSelectedPopupEntry == 1;
				bIsConstant = sal_False;
                Display(nSelectedPopupEntry == 1);
			}
		}
	}
	GetParentWindow()->UpdateListBox();
}

/***************************************************************************
	Beschreibung:
***************************************************************************/


void SwContentTree::SetOutlineLevel(sal_uInt8 nSet)
{
	nOutlineLevel = nSet;
	pConfig->SetOutlineLevel( nOutlineLevel );
	SwContentType** ppContentT = bIsActive ?
					&aActiveContentArr[CONTENT_TYPE_OUTLINE] :
						&aHiddenContentArr[CONTENT_TYPE_OUTLINE];
	if(*ppContentT)
	{
		(*ppContentT)->SetOutlineLevel(nOutlineLevel);
		(*ppContentT)->Init();
	}
	Display(bIsActive);
}

/***************************************************************************
	Beschreibung:	Moduswechsel: gedropptes Doc anzeigen
***************************************************************************/


void SwContentTree::ShowHiddenShell()
{
	if(pHiddenShell)
	{
		bIsConstant = sal_False;
		bIsActive = sal_False;
		Display(sal_False);
	}
}

/***************************************************************************
	Beschreibung:	Moduswechsel: aktive Sicht anzeigen
***************************************************************************/


void SwContentTree::ShowActualView()
{
	bIsActive = sal_True;
	bIsConstant = sal_False;
	Display(sal_True);
	GetParentWindow()->UpdateListBox();
}

/*-----------------20.11.96 13.34-------------------
	Beschreibung: Hier sollen die Buttons zum Verschieben von
				  Outlines en-/disabled werden
--------------------------------------------------*/

sal_Bool  SwContentTree::Select( SvLBoxEntry* pEntry, sal_Bool bSelect )
{
	if(!pEntry)
		return sal_False;
	sal_Bool bEnable = sal_False;
	SvLBoxEntry* pParentEntry = GetParent(pEntry);
	if(!bIsLastReadOnly && (!IsVisible() ||
		((bIsRoot && nRootType == CONTENT_TYPE_OUTLINE && pParentEntry) ||
			(lcl_IsContent(pEntry) && ((SwContentType*)pParentEntry->GetUserData())->GetType() == CONTENT_TYPE_OUTLINE))))
		bEnable = sal_True;
	SwNavigationPI* pNavi = GetParentWindow();
	pNavi->aContentToolBox.EnableItem(FN_ITEM_UP ,  bEnable);
	pNavi->aContentToolBox.EnableItem(FN_ITEM_DOWN, bEnable);
	pNavi->aContentToolBox.EnableItem(FN_ITEM_LEFT, bEnable);
	pNavi->aContentToolBox.EnableItem(FN_ITEM_RIGHT,bEnable);

	return SvTreeListBox::Select(pEntry, bSelect);
}

/*-----------------27.11.96 12.56-------------------

--------------------------------------------------*/

void SwContentTree::SetRootType(sal_uInt16 nType)
{
	nRootType = nType;
	bIsRoot = sal_True;
	pConfig->SetRootType( nRootType );
}

/*-----------------10.01.97 12.19-------------------

--------------------------------------------------*/

void SwContentType::RemoveNewline(String& rEntry)
{
	sal_Unicode* pStr = rEntry.GetBufferAccess();
	for(xub_StrLen i = rEntry.Len(); i; --i, ++pStr )
	{
		if( *pStr == 10 || *pStr == 13 )
			*pStr = 0x20;
	}
}

/*-----------------14.01.97 16.38-------------------

--------------------------------------------------*/

void SwContentTree::EditEntry(SvLBoxEntry* pEntry, sal_uInt8 nMode)
{
	SwContent* pCnt = (SwContent*)pEntry->GetUserData();
	GotoContent(pCnt);
	sal_uInt16 nType = pCnt->GetParent()->GetType();
	sal_uInt16 nSlot = 0;

	uno::Reference< container::XNameAccess >  xNameAccess, xSecond, xThird;
	switch(nType)
	{
		case CONTENT_TYPE_TABLE 	:
			if(nMode == EDIT_UNPROTECT_TABLE)
			{
				pActiveShell->GetView().GetDocShell()->
						GetDoc()->UnProtectCells( pCnt->GetName());
			}
			else if(nMode == EDIT_MODE_DELETE)
			{
				pActiveShell->StartAction();
                String sTable = SW_RES(STR_TABLE_NAME);
                SwRewriter aRewriterTableName;
                aRewriterTableName.AddRule(UNDO_ARG1, SW_RES(STR_START_QUOTE));
                aRewriterTableName.AddRule(UNDO_ARG2, pCnt->GetName());
                aRewriterTableName.AddRule(UNDO_ARG3, SW_RES(STR_END_QUOTE));
                sTable = aRewriterTableName.Apply(sTable);

                SwRewriter aRewriter;
                aRewriter.AddRule(UNDO_ARG1, sTable);
				pActiveShell->StartUndo(UNDO_DELETE, &aRewriter);
				pActiveShell->GetView().GetViewFrame()->GetDispatcher()->Execute(FN_TABLE_SELECT_ALL);
				pActiveShell->DeleteRow();
                pActiveShell->EndUndo();
				pActiveShell->EndAction();
			}
			else if(nMode == EDIT_MODE_RENAME)
			{
				uno::Reference< frame::XModel >  xModel = pActiveShell->GetView().GetDocShell()->GetBaseModel();
				uno::Reference< text::XTextTablesSupplier >  xTables(xModel, uno::UNO_QUERY);
				xNameAccess = xTables->getTextTables();
			}
			else
				nSlot = FN_FORMAT_TABLE_DLG;
		break;

		case CONTENT_TYPE_GRAPHIC   :
			if(nMode == EDIT_MODE_DELETE)
			{
				pActiveShell->DelRight();
			}
			else if(nMode == EDIT_MODE_RENAME)
			{
				uno::Reference< frame::XModel >  xModel = pActiveShell->GetView().GetDocShell()->GetBaseModel();
				uno::Reference< text::XTextGraphicObjectsSupplier >  xGraphics(xModel, uno::UNO_QUERY);
				xNameAccess = xGraphics->getGraphicObjects();
				uno::Reference< text::XTextFramesSupplier >  xFrms(xModel, uno::UNO_QUERY);
				xSecond = xFrms->getTextFrames();
				uno::Reference< text::XTextEmbeddedObjectsSupplier >  xObjs(xModel, uno::UNO_QUERY);
				xThird = xObjs->getEmbeddedObjects();
			}
			else
				nSlot = FN_FORMAT_GRAFIC_DLG;
		break;

		case CONTENT_TYPE_FRAME     :
		case CONTENT_TYPE_OLE       :
			if(nMode == EDIT_MODE_DELETE)
			{
				pActiveShell->DelRight();
			}
			else if(nMode == EDIT_MODE_RENAME)
			{
				uno::Reference< frame::XModel >  xModel = pActiveShell->GetView().GetDocShell()->GetBaseModel();
				uno::Reference< text::XTextFramesSupplier >  xFrms(xModel, uno::UNO_QUERY);
				uno::Reference< text::XTextEmbeddedObjectsSupplier >  xObjs(xModel, uno::UNO_QUERY);
				if(CONTENT_TYPE_FRAME == nType)
				{
					xNameAccess = xFrms->getTextFrames();
					xSecond = xObjs->getEmbeddedObjects();
				}
				else
				{
					xNameAccess = xObjs->getEmbeddedObjects();
					xSecond = xFrms->getTextFrames();
				}
				uno::Reference< text::XTextGraphicObjectsSupplier >  xGraphics(xModel, uno::UNO_QUERY);
				xThird = xGraphics->getGraphicObjects();
			}
			else
				nSlot = FN_FORMAT_FRAME_DLG;
		break;
        case CONTENT_TYPE_BOOKMARK  :
            if(nMode == EDIT_MODE_DELETE)
            {
                IDocumentMarkAccess* const pMarkAccess = pActiveShell->getIDocumentMarkAccess();
                pMarkAccess->deleteMark( pMarkAccess->findMark(pCnt->GetName()) );
            }
            else if(nMode == EDIT_MODE_RENAME)
            {
                uno::Reference< frame::XModel >  xModel = pActiveShell->GetView().GetDocShell()->GetBaseModel();
                uno::Reference< text::XBookmarksSupplier >  xBkms(xModel, uno::UNO_QUERY);
                xNameAccess = xBkms->getBookmarks();
            }
            else
                nSlot = FN_INSERT_BOOKMARK;
        break;

		case CONTENT_TYPE_REGION    :
			if(nMode == EDIT_MODE_RENAME)
			{
				uno::Reference< frame::XModel >  xModel = pActiveShell->GetView().GetDocShell()->GetBaseModel();
				uno::Reference< text::XTextSectionsSupplier >  xSects(xModel, uno::UNO_QUERY);
				xNameAccess = xSects->getTextSections();
			}
			else
				nSlot = FN_EDIT_REGION;
		break;

		case CONTENT_TYPE_URLFIELD:
			nSlot = FN_EDIT_HYPERLINK;
		break;
		case CONTENT_TYPE_REFERENCE:
			nSlot = FN_EDIT_FIELD;
		break;

		case CONTENT_TYPE_POSTIT:
			pActiveShell->GetView().GetPostItMgr()->AssureStdModeAtShell();
			if(nMode == EDIT_MODE_DELETE)
			{
				if (((SwPostItContent*)pCnt)->IsPostIt())
				{
                    pActiveShell->GetView().GetPostItMgr()->SetActiveSidebarWin(0);
					pActiveShell->DelRight();
				}
				/*
				//	this code can be used once we want redline comments in the margin
				else
				{
					SwMarginWin* pComment = pActiveShell->GetView().GetPostItMgr()->GetPostIt(((SwPostItContent*)pCnt)->GetRedline());
					if (pComment)
						pComment->Delete();
				}
				*/
			}
			else
			{
				if (((SwPostItContent*)pCnt)->IsPostIt())
					nSlot = FN_POSTIT;
				else
					nSlot = FN_REDLINE_COMMENT;
			}
		break;
		case CONTENT_TYPE_INDEX:
		{
			const SwTOXBase* pBase = ((SwTOXBaseContent*)pCnt)->GetTOXBase();
			switch(nMode)
			{
				case EDIT_MODE_EDIT:
					if(pBase)
					{
						SwPtrItem aPtrItem( FN_INSERT_MULTI_TOX, (void*)pBase);
						pActiveShell->GetView().GetViewFrame()->
							GetDispatcher()->Execute(FN_INSERT_MULTI_TOX,
											SFX_CALLMODE_ASYNCHRON, &aPtrItem, 0L);

					}
				break;
				case EDIT_MODE_RMV_IDX:
				case EDIT_MODE_DELETE:
				{
					if( pBase )
						pActiveShell->DeleteTOX(*pBase, EDIT_MODE_DELETE == nMode);
				}
				break;
				case EDIT_MODE_UPD_IDX:
				case EDIT_MODE_RENAME:
				{
					Reference< frame::XModel >  xModel = pActiveShell->GetView().GetDocShell()->GetBaseModel();
					Reference< XDocumentIndexesSupplier >  xIndexes(xModel, UNO_QUERY);
					Reference< XIndexAccess> xIdxAcc(xIndexes->getDocumentIndexes());
					Reference< XNameAccess >xLocalNameAccess(xIdxAcc, UNO_QUERY);
					if(EDIT_MODE_RENAME == nMode)
						xNameAccess = xLocalNameAccess;
					else if(xLocalNameAccess.is() && xLocalNameAccess->hasByName(pBase->GetTOXName()))
					{
						Any aIdx = xLocalNameAccess->getByName(pBase->GetTOXName());
						Reference< XDocumentIndex> xIdx;
						if(aIdx >>= xIdx)
							xIdx->update();
					}
				}
				break;
			}
		}
		break;
        case CONTENT_TYPE_DRAWOBJECT :
            if(EDIT_MODE_DELETE == nMode)
                nSlot = SID_DELETE;
        break;
	}
	if(nSlot)
		pActiveShell->GetView().GetViewFrame()->
					GetDispatcher()->Execute(nSlot, SFX_CALLMODE_ASYNCHRON);
	else if(xNameAccess.is())
	{
		uno::Any aObj = xNameAccess->getByName(pCnt->GetName());
        uno::Reference< uno::XInterface >  xTmp;
        aObj >>= xTmp;
		uno::Reference< container::XNamed >  xNamed(xTmp, uno::UNO_QUERY);
        SwAbstractDialogFactory* pFact = SwAbstractDialogFactory::Create();
        DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");

        AbstractSwRenameXNamedDlg* pDlg = pFact->CreateSwRenameXNamedDlg( this, xNamed, xNameAccess, DLG_RENAME_XNAMED );
        DBG_ASSERT(pDlg, "Dialogdiet fail!");
		if(xSecond.is())
            pDlg->SetAlternativeAccess( xSecond, xThird);

		String sForbiddenChars;
		if(CONTENT_TYPE_BOOKMARK == nType)
		{
            sForbiddenChars = C2S("/\\@:*?\";,.#");
		}
		else if(CONTENT_TYPE_TABLE == nType)
		{
			sForbiddenChars = C2S(" .<>");
		}
        pDlg->SetForbiddenChars(sForbiddenChars);
        pDlg->Execute();
        delete pDlg;
	}
}

/*-----------------14.01.97 16.53-------------------

--------------------------------------------------*/

void SwContentTree::GotoContent(SwContent* pCnt)
{
	pActiveShell->EnterStdMode();

	sal_Bool bSel = sal_False;
	sal_uInt16 nJumpType = pCnt->GetParent()->GetType();
	switch(nJumpType)
	{
		case CONTENT_TYPE_OUTLINE	:
		{
			pActiveShell->GotoOutline(((SwOutlineContent*)pCnt)->GetPos());
		}
		break;
		case CONTENT_TYPE_TABLE 	:
		{
			pActiveShell->GotoTable(pCnt->GetName());
		}
		break;
		case CONTENT_TYPE_FRAME     :
		case CONTENT_TYPE_GRAPHIC   :
		case CONTENT_TYPE_OLE       :
		{
			if(pActiveShell->GotoFly(pCnt->GetName()))
				bSel = sal_True;
		}
		break;
        case CONTENT_TYPE_BOOKMARK:
        {
            pActiveShell->GotoMark(pCnt->GetName());
        }
		break;
		case CONTENT_TYPE_REGION    :
		{
			pActiveShell->GotoRegion(pCnt->GetName());
		}
		break;
		case CONTENT_TYPE_URLFIELD:
		{
			if(pActiveShell->GotoINetAttr(
							*((SwURLFieldContent*)pCnt)->GetINetAttr() ))
			{
				pActiveShell->Right( CRSR_SKIP_CHARS, sal_True, 1, sal_False);
				pActiveShell->SwCrsrShell::SelectTxtAttr( RES_TXTATR_INETFMT, sal_True );
			}

		}
		break;
		case CONTENT_TYPE_REFERENCE:
		{
			pActiveShell->GotoRefMark(pCnt->GetName());
		}
		break;
		case CONTENT_TYPE_INDEX:
		{
			if (!pActiveShell->GotoNextTOXBase(&pCnt->GetName()))
				pActiveShell->GotoPrevTOXBase(&pCnt->GetName());
		}
		break;
		case CONTENT_TYPE_POSTIT:
			pActiveShell->GetView().GetPostItMgr()->AssureStdModeAtShell();
			if (((SwPostItContent*)pCnt)->IsPostIt())
				pActiveShell->GotoFld(*((SwPostItContent*)pCnt)->GetPostIt());
			else
				pActiveShell->GetView().GetDocShell()->GetWrtShell()->GotoRedline(
						pActiveShell->GetView().GetDocShell()->GetWrtShell()->FindRedlineOfData(((SwPostItContent*)pCnt)->GetRedline()->GetRedlineData()));

		break;
		case CONTENT_TYPE_DRAWOBJECT:
        {
            SdrView* pDrawView = pActiveShell->GetDrawView();
			if (pDrawView)
			{
				pDrawView->SdrEndTextEdit();
				pDrawView->UnmarkAll();
                SwDrawModel* _pModel = pActiveShell->getIDocumentDrawModelAccess()->GetDrawModel();
                SdrPage* pPage = _pModel->GetPage(0);
                sal_uInt32 nCount = pPage->GetObjCount();
                for( sal_uInt32 i=0; i< nCount; i++ )
                {
                    SdrObject* pTemp = pPage->GetObj(i);
                    // --> OD 2006-03-09 #i51726# - all drawing objects can be named now
//                    if(pTemp->ISA(SdrObjGroup) && pTemp->GetName() == pCnt->GetName())
                    if ( pTemp->GetName() == pCnt->GetName() )
                    // <--
                    {
                        SdrPageView* pPV = pDrawView->GetSdrPageView();
                        if( pPV )
                        {
                            pDrawView->MarkObj( pTemp, pPV );
                        }
                    }
                }
            }
        }
		break;
	}
	if(bSel)
	{
		pActiveShell->HideCrsr();
		pActiveShell->EnterSelFrmMode();
	}
	SwView& rView = pActiveShell->GetView();
	rView.StopShellTimer();
    rView.GetPostItMgr()->SetActiveSidebarWin(0);
	rView.GetEditWin().GrabFocus();
}
/*-----------------06.02.97 19.14-------------------
    Jetzt noch die passende text::Bookmark
--------------------------------------------------*/

NaviContentBookmark::NaviContentBookmark()
    :
    nDocSh(0),
    nDefDrag( REGION_MODE_NONE )
{
}

/*-----------------06.02.97 20.12-------------------

--------------------------------------------------*/

NaviContentBookmark::NaviContentBookmark( const String &rUrl,
					const String& rDesc,
					sal_uInt16 nDragType,
					const SwDocShell* pDocSh ) :
	aUrl( rUrl ),
	aDescr(rDesc),
    nDocSh((long)pDocSh),
    nDefDrag( nDragType )
{
}

void NaviContentBookmark::Copy( TransferDataContainer& rData ) const
{
	rtl_TextEncoding eSysCSet = gsl_getSystemTextEncoding();

	ByteString sStr( aUrl, eSysCSet );
	sStr += static_cast< char >(NAVI_BOOKMARK_DELIM);
	sStr += ByteString( aDescr, eSysCSet );
	sStr += static_cast< char >(NAVI_BOOKMARK_DELIM);
	sStr += ByteString::CreateFromInt32( nDefDrag );
	sStr += static_cast< char >(NAVI_BOOKMARK_DELIM);
	sStr += ByteString::CreateFromInt32( nDocSh );
	rData.CopyByteString( SOT_FORMATSTR_ID_SONLK, sStr );
}

sal_Bool NaviContentBookmark::Paste( TransferableDataHelper& rData )
{
	String sStr;
	sal_Bool bRet = rData.GetString( SOT_FORMATSTR_ID_SONLK, sStr );
	if( bRet )
	{
		xub_StrLen nPos = 0;
		aUrl	= sStr.GetToken(0, NAVI_BOOKMARK_DELIM, nPos );
		aDescr	= sStr.GetToken(0, NAVI_BOOKMARK_DELIM, nPos );
        nDefDrag= (sal_uInt16)sStr.GetToken(0, NAVI_BOOKMARK_DELIM, nPos ).ToInt32();
		nDocSh	= sStr.GetToken(0, NAVI_BOOKMARK_DELIM, nPos ).ToInt32();
	}
	return bRet;
}


/* -----------------------------09.12.99 13:50--------------------------------

 ---------------------------------------------------------------------------*/
class SwContentLBoxString : public SvLBoxString
{
public:
	SwContentLBoxString( SvLBoxEntry* pEntry, sal_uInt16 nFlags,
		const String& rStr ) : SvLBoxString(pEntry,nFlags,rStr)	{}

	virtual void Paint( const Point& rPos, SvLBox& rDev, sal_uInt16 nFlags,
		SvLBoxEntry* pEntry);
};

/* -----------------------------09.12.99 13:49--------------------------------

 ---------------------------------------------------------------------------*/
void SwContentTree::InitEntry(SvLBoxEntry* pEntry,
		const XubString& rStr ,const Image& rImg1,const Image& rImg2,
        SvLBoxButtonKind eButtonKind)
{
	sal_uInt16 nColToHilite = 1; //0==Bitmap;1=="Spalte1";2=="Spalte2"
	SvTreeListBox::InitEntry( pEntry, rStr, rImg1, rImg2, eButtonKind );
	SvLBoxString* pCol = (SvLBoxString*)pEntry->GetItem( nColToHilite );
	SwContentLBoxString* pStr = new SwContentLBoxString( pEntry, 0, pCol->GetText() );
	pEntry->ReplaceItem( pStr, nColToHilite );
}
/* -----------------------------09.12.99 13:49--------------------------------

 ---------------------------------------------------------------------------*/
void SwContentLBoxString::Paint( const Point& rPos, SvLBox& rDev, sal_uInt16 nFlags,
	SvLBoxEntry* pEntry )
{
	if(lcl_IsContent(pEntry) &&
			((SwContent *)pEntry->GetUserData())->IsInvisible())
	{
		//* pCont = (SwContent*)pEntry->GetUserData();
		Font aOldFont( rDev.GetFont());
		Font aFont(aOldFont);
		Color aCol( COL_LIGHTGRAY );
		aFont.SetColor( aCol );
		rDev.SetFont( aFont );
		rDev.DrawText( rPos, GetText() );
		rDev.SetFont( aOldFont );
	}
	// IA2 CWS. MT: Removed for now (also in SvLBoxEntry) - only used in Sw/Sd/ScContentLBoxString, they should decide if they need this
	/*
	else if (pEntry->IsMarked())
	{
			rDev.DrawText( rPos, GetText() );
			XubString str;
			str = XubString::CreateFromAscii("*");
			Point rPosStar(rPos.X()-6,rPos.Y());
			Font aOldFont( rDev.GetFont());
			Font aFont(aOldFont);
			Color aCol( aOldFont.GetColor() );
			aCol.DecreaseLuminance( 200 );
			aFont.SetColor( aCol );
			rDev.SetFont( aFont );
			rDev.DrawText( rPosStar, str);
			rDev.SetFont( aOldFont );
	}
	*/
	else
		SvLBoxString::Paint( rPos, rDev, nFlags, pEntry);
}
/* -----------------------------06.05.2002 10:20------------------------------

 ---------------------------------------------------------------------------*/
void SwContentTree::DataChanged( const DataChangedEvent& rDCEvt )
{
  if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        sal_uInt16 nResId = GetSettings().GetStyleSettings().GetHighContrastMode() ? IMG_NAVI_ENTRYBMPH : IMG_NAVI_ENTRYBMP;
        aEntryImages = ImageList(SW_RES(nResId));
        FindActiveTypeAndRemoveUserData();
        Display(sal_True);
    }
    SvTreeListBox::DataChanged( rDCEvt );
}


sal_Int32  SwContentTree::GetEntryRealChildsNum( SvLBoxEntry* pParent ) const
{
	// ist es ein Inhaltstyp?
	if(lcl_IsContentType(pParent))
	{
		if(!pParent->HasChilds())
		{
			SwContentType* pCntType = (SwContentType*)pParent->GetUserData();
			return pCntType->GetMemberCount();
		}
	}
	return 0;
}
