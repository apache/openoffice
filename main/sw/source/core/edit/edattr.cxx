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


#include <hintids.hxx>

#include <editeng/tstpitem.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/scripttypeitem.hxx>
#include <com/sun/star/i18n/ScriptType.hdl>
#include <txatbase.hxx>
#include <txtftn.hxx>
#include <fmtftn.hxx>
#include <editsh.hxx>
#include <edimp.hxx>	// fuer MACROS
#include <doc.hxx>
#include <swundo.hxx>	// fuer UNDO-Ids
#include <ndtxt.hxx>
#include <ftnidx.hxx>
#include <expfld.hxx>
#include <rootfrm.hxx>
#include <cntfrm.hxx>
#include <breakit.hxx>
#include <txtfld.hxx>
#include <fmtfld.hxx>
#include <crsskip.hxx>
#include <txtfrm.hxx>		// SwTxtFrm
#include <scriptinfo.hxx>
#include <svl/ctloptions.hxx>
#include <charfmt.hxx>  // #i27615#
#include <numrule.hxx>


/*************************************
 * harte Formatierung (Attribute)
 *************************************/

// wenn Selektion groesser Max Nodes oder mehr als Max Selektionen
// => keine Attribute
const sal_uInt16& getMaxLookup()
{
	static const sal_uInt16 nMaxLookup = 1000;
	return nMaxLookup;
}

// --> OD 2008-01-16 #newlistlevelattrs#
sal_Bool SwEditShell::GetCurAttr( SfxItemSet& rSet,
                              const bool bMergeIndentValuesOfNumRule ) const
// <--
{
	if( GetCrsrCnt() > getMaxLookup() )
	{
		rSet.InvalidateAllItems();
		return sal_False;
	}

	SfxItemSet aSet( *rSet.GetPool(), rSet.GetRanges() );
	SfxItemSet *pSet = &rSet;

	FOREACHPAM_START(this)

        // #i27615# if the cursor is in front of the numbering label
        // the attributes to get are those from the numbering format.
        if (PCURCRSR->IsInFrontOfLabel())
        {
            SwTxtNode * pTxtNd =
                PCURCRSR->GetPoint()->nNode.GetNode().GetTxtNode();

            if (pTxtNd)
            {
                SwNumRule * pNumRule = pTxtNd->GetNumRule();

                if (pNumRule)
                {
                    const String & aCharFmtName =
                        pNumRule->Get(static_cast<sal_uInt16>(pTxtNd->GetActualListLevel())).GetCharFmtName();
                    SwCharFmt * pCharFmt =
                        GetDoc()->FindCharFmtByName(aCharFmtName);

                    if (pCharFmt)
                        rSet.Put(pCharFmt->GetAttrSet());
                }
            }

            continue;
        }

		sal_uLong nSttNd = PCURCRSR->GetMark()->nNode.GetIndex(),
			  nEndNd = PCURCRSR->GetPoint()->nNode.GetIndex();
		xub_StrLen nSttCnt = PCURCRSR->GetMark()->nContent.GetIndex(),
				   nEndCnt = PCURCRSR->GetPoint()->nContent.GetIndex();

		if( nSttNd > nEndNd || ( nSttNd == nEndNd && nSttCnt > nEndCnt ))
		{
			sal_uLong nTmp = nSttNd; nSttNd = nEndNd; nEndNd = nTmp;
			nTmp = nSttCnt; nSttCnt = nEndCnt; nEndCnt = (xub_StrLen)nTmp;
		}

		if( nEndNd - nSttNd >= getMaxLookup() )
		{
			rSet.ClearItem();
			rSet.InvalidateAllItems();
			return sal_False;
		}

		// beim 1.Node traegt der Node die Werte in den GetSet ein (Initial)
		// alle weiteren Nodes werden zum GetSet zu gemergt
		for( sal_uLong n = nSttNd; n <= nEndNd; ++n )
		{
			SwNode* pNd = GetDoc()->GetNodes()[ n ];
			switch( pNd->GetNodeType() )
			{
			case ND_TEXTNODE:
				{
					xub_StrLen nStt = n == nSttNd ? nSttCnt : 0,
						   	   nEnd = n == nEndNd ? nEndCnt
										: ((SwTxtNode*)pNd)->GetTxt().Len();
                    // --> OD 2008-01-16 #newlistlevelattrs#
                    ((SwTxtNode*)pNd)->GetAttr( *pSet, nStt, nEnd,
                                                sal_False, sal_True,
                                                bMergeIndentValuesOfNumRule );
                    // <--
				}
				break;
			case ND_GRFNODE:
			case ND_OLENODE:
				((SwCntntNode*)pNd)->GetAttr( *pSet );
				break;

			default:
				pNd = 0;
			}

			if( pNd )
			{
				if( pSet != &rSet )
					rSet.MergeValues( aSet );

				if( aSet.Count() )
					aSet.ClearItem();
			}
			pSet = &aSet;
		}

	FOREACHPAM_END()

	return sal_True;
}

SwTxtFmtColl* SwEditShell::GetCurTxtFmtColl() const
{
	SwTxtFmtColl *pFmt = 0;

	if ( GetCrsrCnt() > getMaxLookup() )
		return 0;

	FOREACHPAM_START(this)

		sal_uLong nSttNd = PCURCRSR->GetMark()->nNode.GetIndex(),
			  nEndNd = PCURCRSR->GetPoint()->nNode.GetIndex();
		xub_StrLen nSttCnt = PCURCRSR->GetMark()->nContent.GetIndex(),
				   nEndCnt = PCURCRSR->GetPoint()->nContent.GetIndex();

		if( nSttNd > nEndNd || ( nSttNd == nEndNd && nSttCnt > nEndCnt ))
		{
			sal_uLong nTmp = nSttNd; nSttNd = nEndNd; nEndNd = nTmp;
			nTmp = nSttCnt; nSttCnt = nEndCnt; nEndCnt = (xub_StrLen)nTmp;
		}

		if( nEndNd - nSttNd >= getMaxLookup() )
		{
			pFmt = 0;
			break;
		}

		for( sal_uLong n = nSttNd; n <= nEndNd; ++n )
		{
			SwNode* pNd = GetDoc()->GetNodes()[ n ];
			if( pNd->IsTxtNode() )
			{
				if( !pFmt )
					pFmt = ((SwTxtNode*)pNd)->GetTxtColl();
				else if( pFmt == ((SwTxtNode*)pNd)->GetTxtColl() ) // ???
					break;
			}
		}

	FOREACHPAM_END()
	return pFmt;
}



sal_Bool SwEditShell::GetCurFtn( SwFmtFtn* pFillFtn )
{
	// der Cursor muss auf dem akt. Fussnoten-Anker stehen:
	SwPaM* pCrsr = GetCrsr();
	SwTxtNode* pTxtNd = pCrsr->GetNode()->GetTxtNode();
	if( !pTxtNd )
		return sal_False;

    SwTxtAttr *const pFtn = pTxtNd->GetTxtAttrForCharAt(
        pCrsr->GetPoint()->nContent.GetIndex(), RES_TXTATR_FTN);
	if( pFtn && pFillFtn )
	{
		// Daten vom Attribut uebertragen
		const SwFmtFtn &rFtn = ((SwTxtFtn*)pFtn)->GetFtn();
		pFillFtn->SetNumber( rFtn );
		pFillFtn->SetEndNote( rFtn.IsEndNote() );
	}
	return 0 != pFtn;
}


bool SwEditShell::SetCurFtn( const SwFmtFtn& rFillFtn )
{
    bool bChgd = false;
    StartAllAction();

    SwPaM* pCrsr = GetCrsr(), *pFirst = pCrsr;
    do {
        bChgd |=
            pDoc->SetCurFtn( *pCrsr, rFillFtn.GetNumStr(), rFillFtn.GetNumber(), rFillFtn.IsEndNote() );

    } while( pFirst != ( pCrsr = (SwPaM*)pCrsr->GetNext() ));

    EndAllAction();
    return bChgd;
}



/*sal_uInt16 SwEditShell::GetFtnCnt( sal_Bool bEndNotes = sal_False ) const
{
	const SwFtnIdxs &rIdxs = pDoc->GetFtnIdxs();
	sal_uInt16 nCnt = 0;
	for ( sal_uInt16 i = 0; i < rIdxs.Count(); ++i )
	{
		const SwFmtFtn &rFtn = rIdxs[i]->GetFtn();
		if ( bEndNotes == rFtn.IsEndNote() )
			nCnt++;
	}
	return nCnt;
} */


bool SwEditShell::HasFtns( bool bEndNotes ) const
{
	const SwFtnIdxs &rIdxs = pDoc->GetFtnIdxs();
	for ( sal_uInt16 i = 0; i < rIdxs.Count(); ++i )
	{
		const SwFmtFtn &rFtn = rIdxs[i]->GetFtn();
		if ( bEndNotes == rFtn.IsEndNote() )
			return sal_True;
	}
	return sal_False;
}


	// gebe Liste aller Fussnoten und deren Anfangstexte
sal_uInt16 SwEditShell::GetSeqFtnList( SwSeqFldList& rList, bool bEndNotes )
{
	if( rList.Count() )
		rList.Remove( 0, rList.Count() );

	sal_uInt16 n, nFtnCnt = pDoc->GetFtnIdxs().Count();
	SwTxtFtn* pTxtFtn;
	for( n = 0; n < nFtnCnt; ++n )
	{
		pTxtFtn = pDoc->GetFtnIdxs()[ n ];
		const SwFmtFtn& rFtn = pTxtFtn->GetFtn();
		if ( rFtn.IsEndNote() != bEndNotes )
			continue;

		SwNodeIndex* pIdx = pTxtFtn->GetStartNode();
		if( pIdx )
		{
			SwNodeIndex aIdx( *pIdx, 1 );
			SwTxtNode* pTxtNd = aIdx.GetNode().GetTxtNode();
			if( !pTxtNd )
				pTxtNd = (SwTxtNode*)pDoc->GetNodes().GoNext( &aIdx );

			if( pTxtNd )
			{
				String sTxt( rFtn.GetViewNumStr( *pDoc ));
				if( sTxt.Len() )
					sTxt += ' ';
                sTxt += pTxtNd->GetExpandTxt( 0, USHRT_MAX );

				_SeqFldLstElem* pNew = new _SeqFldLstElem( sTxt,
											pTxtFtn->GetSeqRefNo() );
				while( rList.InsertSort( pNew ) )
					pNew->sDlgEntry += ' ';
			}
		}
	}

	return rList.Count();
}


// linken Rand ueber Objectleiste einstellen (aenhlich dem Stufen von
// Numerierungen)
sal_Bool SwEditShell::IsMoveLeftMargin( sal_Bool bRight, sal_Bool bModulus ) const
{
	sal_Bool bRet = sal_True;

	const SvxTabStopItem& rTabItem = (SvxTabStopItem&)GetDoc()->
								GetDefault( RES_PARATR_TABSTOP );
	sal_uInt16 nDefDist = static_cast<sal_uInt16>(rTabItem.Count() ? rTabItem[0].GetTabPos() : 1134);
	if( !nDefDist )
		return sal_False;

	FOREACHPAM_START(this)

		sal_uLong nSttNd = PCURCRSR->GetMark()->nNode.GetIndex(),
			  nEndNd = PCURCRSR->GetPoint()->nNode.GetIndex();

		if( nSttNd > nEndNd )
		{
			sal_uLong nTmp = nSttNd; nSttNd = nEndNd; nEndNd = nTmp;
		}

		SwCntntNode* pCNd;
		for( sal_uLong n = nSttNd; bRet && n <= nEndNd; ++n )
			if( 0 != ( pCNd = GetDoc()->GetNodes()[ n ]->GetTxtNode() ))
			{
				const SvxLRSpaceItem& rLS = (SvxLRSpaceItem&)
											pCNd->GetAttr( RES_LR_SPACE );
				if( bRight )
				{
					long nNext = rLS.GetTxtLeft() + nDefDist;
					if( bModulus )
						nNext = ( nNext / nDefDist ) * nDefDist;
					SwFrm* pFrm = pCNd->getLayoutFrm( GetLayout() );
                    if ( pFrm )
                    {
                        const sal_uInt16 nFrmWidth = static_cast<sal_uInt16>( pFrm->IsVertical() ?
                                                 pFrm->Frm().Height() :
                                                 pFrm->Frm().Width() );
                        bRet = nFrmWidth > ( nNext + MM50 );
                    }
                    else
                        bRet = sal_False;
				}
			}

		if( !bRet )
			break;

	FOREACHPAM_END()
	return bRet;
}

void SwEditShell::MoveLeftMargin( sal_Bool bRight, sal_Bool bModulus )
{
	StartAllAction();
	StartUndo( UNDO_START );

	SwPaM* pCrsr = GetCrsr();
	if( pCrsr->GetNext() != pCrsr )			// Mehrfachselektion ?
	{
		SwPamRanges aRangeArr( *pCrsr );
		SwPaM aPam( *pCrsr->GetPoint() );
		for( sal_uInt16 n = 0; n < aRangeArr.Count(); ++n )
			GetDoc()->MoveLeftMargin( aRangeArr.SetPam( n, aPam ),
										bRight, bModulus );
	}
	else
		GetDoc()->MoveLeftMargin( *pCrsr, bRight, bModulus );

	EndUndo( UNDO_END );
	EndAllAction();
}


inline sal_uInt16 lcl_SetScriptFlags( sal_uInt16 nType )
{
	sal_uInt16 nRet;
   	switch( nType )
	{
	case ::com::sun::star::i18n::ScriptType::LATIN:		nRet = SCRIPTTYPE_LATIN;	break;
	case ::com::sun::star::i18n::ScriptType::ASIAN:		nRet = SCRIPTTYPE_ASIAN;	break;
	case ::com::sun::star::i18n::ScriptType::COMPLEX:	nRet = SCRIPTTYPE_COMPLEX;	break;
	default: nRet = 0;
	}
	return nRet;
}

sal_Bool lcl_IsNoEndTxtAttrAtPos( const SwTxtNode& rTNd, xub_StrLen nPos,
                            sal_uInt16 &rScrpt, sal_Bool bInSelection, sal_Bool bNum )
{
	sal_Bool bRet = sal_False;
	const String& rTxt = rTNd.GetTxt();
    String sExp;

    // consider numbering
    if ( bNum )
    {
        bRet = sal_False;

        if ( rTNd.IsInList() )
        {
            ASSERT( rTNd.GetNumRule(),
                    "<lcl_IsNoEndTxtAttrAtPos(..)> - no list style found at text node. Serious defect -> please inform OD." );
            const SwNumRule* pNumRule = rTNd.GetNumRule();
            //Modified for i119959,2012.6.12
            //Under this scenario,this pointer is null,but on win,it doesn't crash immediately
            //it exits with exception,and associated memory will have problem which leads to crash problem in 
            //other place in dev env...
            if ( pNumRule )
            {
                //End
                const SwNumFmt &rNumFmt = pNumRule->Get( static_cast<sal_uInt16>(rTNd.GetActualListLevel()) );
                if( SVX_NUM_BITMAP != rNumFmt.GetNumberingType() )
                {
                    if ( SVX_NUM_CHAR_SPECIAL == rNumFmt.GetNumberingType() )
                        sExp = rNumFmt.GetBulletChar();
                    else
                        sExp = rTNd.GetNumString();
                }
                //Modified for i119959,2012.6.12
                //Under this scenario,this pointer is null,but on win,it doesn't crash immediately
                //it exits with exception,and associated memory will have problem which leads to crash problem in 
                //other place in dev env...
            }
            //End
        }
    }

    // and fields
    if ( CH_TXTATR_BREAKWORD == rTxt.GetChar( nPos ) )
    {
        const SwTxtAttr* const pAttr = rTNd.GetTxtAttrForCharAt( nPos );
        if (pAttr)
        {
            bRet = sal_True; // all other than fields can be
                         // defined as weak-script ?
            if ( RES_TXTATR_FIELD == pAttr->Which() )
            {
                const SwField* const pFld = pAttr->GetFmtFld().GetField();
                if (pFld)
                {
                    sExp += pFld->ExpandField(true);
                }
            }
        }
    }

    xub_StrLen nEnd = sExp.Len();
    if ( nEnd )
    {
        xub_StrLen n;
        if( bInSelection )
        {
            sal_uInt16 nScript;
            for( n = 0; n < nEnd; n = (xub_StrLen)
                    pBreakIt->GetBreakIter()->endOfScript( sExp, n, nScript ))
            {
                nScript = pBreakIt->GetBreakIter()->getScriptType( sExp, n );
                rScrpt |= lcl_SetScriptFlags( nScript );
            }
        }
        else
            rScrpt |= lcl_SetScriptFlags( pBreakIt->GetBreakIter()->
                                        getScriptType( sExp, nEnd-1 ));
    }

    return bRet;
}


// returns the scripttype of the selection
sal_uInt16 SwEditShell::GetScriptType() const
{
	sal_uInt16 nRet = 0;
	//if( pBreakIt->GetBreakIter().is() )
	{
		FOREACHPAM_START(this)

			const SwPosition *pStt = PCURCRSR->Start(),
							 *pEnd = pStt == PCURCRSR->GetMark()
									? PCURCRSR->GetPoint()
									: PCURCRSR->GetMark();
			if( pStt == pEnd || *pStt == *pEnd )
			{
				const SwTxtNode* pTNd = pStt->nNode.GetNode().GetTxtNode();
				if( pTNd )
				{
                    // try to get SwScriptInfo
                    const SwScriptInfo* pScriptInfo = SwScriptInfo::GetScriptInfo( *pTNd );

					xub_StrLen nPos = pStt->nContent.GetIndex();
					//Task 90448: we need the scripttype of the previous
					//				position, if no selection exist!
					if( nPos )
					{
						SwIndex aIdx( pStt->nContent );
						if( pTNd->GoPrevious( &aIdx, CRSR_SKIP_CHARS ) )
							nPos = aIdx.GetIndex();
					}

                    sal_uInt16 nScript;

                    if ( pTNd->GetTxt().Len() )
                    {
                        nScript = pScriptInfo ?
                                  pScriptInfo->ScriptType( nPos ) :
                                  pBreakIt->GetBreakIter()->getScriptType( pTNd->GetTxt(), nPos );
                    }
                    else
                        nScript = GetI18NScriptTypeOfLanguage( (sal_uInt16)GetAppLanguage() );

                    if( !lcl_IsNoEndTxtAttrAtPos( *pTNd, nPos, nRet, sal_False, sal_False ))
                        nRet |= lcl_SetScriptFlags( nScript );
				}
			}
			else if ( pBreakIt->GetBreakIter().is() )
			{
				sal_uLong nEndIdx = pEnd->nNode.GetIndex();
				SwNodeIndex aIdx( pStt->nNode );
				for( ; aIdx.GetIndex() <= nEndIdx; aIdx++ )
					if( aIdx.GetNode().IsTxtNode() )
					{
						const SwTxtNode* pTNd = aIdx.GetNode().GetTxtNode();
						const String& rTxt = pTNd->GetTxt();

                        // try to get SwScriptInfo
                        const SwScriptInfo* pScriptInfo = SwScriptInfo::GetScriptInfo( *pTNd );

						xub_StrLen nChg = aIdx == pStt->nNode
												? pStt->nContent.GetIndex()
												: 0,
									nEndPos = aIdx == nEndIdx
												? pEnd->nContent.GetIndex()
                                                : rTxt.Len();

						ASSERT( nEndPos <= rTxt.Len(), "Index outside the range - endless loop!" );
						if( nEndPos > rTxt.Len() )
							nEndPos = rTxt.Len();

						sal_uInt16 nScript;
						while( nChg < nEndPos )
						{
                            nScript = pScriptInfo ?
                                      pScriptInfo->ScriptType( nChg ) :
                                      pBreakIt->GetBreakIter()->getScriptType(
																rTxt, nChg );

                            if( !lcl_IsNoEndTxtAttrAtPos( *pTNd, nChg, nRet, sal_True,
                                                          0 == nChg && rTxt.Len() == nEndPos ) )
								nRet |= lcl_SetScriptFlags( nScript );

							if( (SCRIPTTYPE_LATIN | SCRIPTTYPE_ASIAN |
								SCRIPTTYPE_COMPLEX) == nRet )
								break;

							xub_StrLen nFldPos = nChg+1;

                            nChg = pScriptInfo ?
                                   pScriptInfo->NextScriptChg( nChg ) :
                                   (xub_StrLen)pBreakIt->GetBreakIter()->endOfScript(
													rTxt, nChg, nScript );

                            nFldPos = rTxt.Search(
											CH_TXTATR_BREAKWORD, nFldPos );
							if( nFldPos < nChg )
								nChg = nFldPos;
						}
						if( (SCRIPTTYPE_LATIN | SCRIPTTYPE_ASIAN |
								SCRIPTTYPE_COMPLEX) == nRet )
							break;
					}
			}
			if( (SCRIPTTYPE_LATIN | SCRIPTTYPE_ASIAN |
								SCRIPTTYPE_COMPLEX) == nRet )
				break;

		FOREACHPAM_END()
	}
	if( !nRet )
        nRet = SvtLanguageOptions::GetScriptTypeOfLanguage( LANGUAGE_SYSTEM );
	return nRet;
}


sal_uInt16 SwEditShell::GetCurLang() const
{
	const SwPaM* pCrsr = GetCrsr();
	const SwPosition& rPos = *pCrsr->GetPoint();
	const SwTxtNode* pTNd = rPos.nNode.GetNode().GetTxtNode();
	sal_uInt16 nLang;
	if( pTNd )
	{
		//JP 24.9.2001: if exist no selection, then get the language before
		//				the current character!
		xub_StrLen nPos = rPos.nContent.GetIndex();
		if( nPos && !pCrsr->HasMark() )
			--nPos;
		nLang = pTNd->GetLang( nPos );
	}
	else
		nLang = LANGUAGE_DONTKNOW;
	return nLang;
}

sal_uInt16 SwEditShell::GetScalingOfSelectedText() const
{
	const SwPaM* pCrsr = GetCrsr();
	const SwPosition* pStt = pCrsr->Start();
	const SwTxtNode* pTNd = pStt->nNode.GetNode().GetTxtNode();
	ASSERT( pTNd, "no textnode available" );

	sal_uInt16 nScaleWidth;
	if( pTNd )
	{
		xub_StrLen nStt = pStt->nContent.GetIndex(), nEnd;
		const SwPosition* pEnd = pStt == pCrsr->GetPoint()
										? pCrsr->GetMark()
										: pCrsr->GetPoint();
		if( pStt->nNode == pEnd->nNode )
			nEnd = pEnd->nContent.GetIndex();
		else
			nEnd = pTNd->GetTxt().Len();
		nScaleWidth = pTNd->GetScalingOfSelectedText( nStt, nEnd );
	}
	else
		nScaleWidth = 100;		        // default are no scaling -> 100%
	return nScaleWidth;
}
