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
#include <editeng/brkitem.hxx>
#include <tools/stream.hxx>
#include <doc.hxx>
#include <docstat.hxx>
#include <docary.hxx>
#include <fmtpdsc.hxx>
#include <laycache.hxx>
#include <layhelp.hxx>
#include <pagefrm.hxx>
#include <rootfrm.hxx>
#include <txtfrm.hxx>
#include <ndtxt.hxx>
#include <swtable.hxx>
#include <tabfrm.hxx>
#include <rowfrm.hxx>
#include <colfrm.hxx>
#include <bodyfrm.hxx>
#include <ndindex.hxx>
#include <sectfrm.hxx>
#include <frmfmt.hxx>
#include <fmtcntnt.hxx>
#include <pagedesc.hxx>
#include <frmtool.hxx>
#include <dflyobj.hxx>
#include <dcontact.hxx>
#include "viewopt.hxx"
#include "viewsh.hxx"
#include <flyfrm.hxx>
// OD 2004-05-24 #i28701#
#include <sortedobjs.hxx>
// --> OD 2006-03-22 #b6375613#
#include <pam.hxx>
#include <docsh.hxx>
#include <com/sun/star/document/XDocumentInfoSupplier.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

#include <set>


using namespace ::com::sun::star;
// <--

SV_IMPL_PTRARR( SwPageFlyCache, SwFlyCachePtr )

/*-----------------28.5.2001 10:06------------------
 *  Reading and writing of the layout cache.
 *  The layout cache is not necessary, but it improves
 *  the performance and reduces the text flow during
 *  the formatting.
 *  The layout cache contains the index of the paragraphs/tables
 *  at the top of every page, so it's possible to create
 *  the right count of pages and to distribute the document content
 *  to this pages before the formatting starts.
 *--------------------------------------------------*/

void SwLayoutCache::Read( SvStream &rStream )
{
    if( !pImpl )
	{
        pImpl = new SwLayCacheImpl;
		if( !pImpl->Read( rStream ) )
		{
			delete pImpl;
			pImpl = 0;
		}
	}
}

//-----------------------------------------------------------------------------

void SwLayCacheImpl::Insert( sal_uInt16 nType, sal_uLong nIndex, xub_StrLen nOffset )
{
	aType.Insert( nType, aType.Count() );
	SvULongs::Insert( nIndex, SvULongs::Count() );
	aOffset.push_back( nOffset );
}

sal_Bool SwLayCacheImpl::Read( SvStream& rStream )
{
	SwLayCacheIoImpl aIo( rStream, sal_False );
	if( aIo.GetMajorVersion() > SW_LAYCACHE_IO_VERSION_MAJOR )
		return sal_False;

    // Due to an evil bug in the layout cache (#102759#), we cannot trust the
    // sizes of fly frames which have been written using the "old" layout cache.
    // This flag should indicate that we do not want to trust the width and
    // height of fly frames
    bUseFlyCache = aIo.GetMinorVersion() >= 1;

	sal_uInt8 cFlags;
	sal_uInt32 nIndex, nOffset;

	aIo.OpenRec( SW_LAYCACHE_IO_REC_PAGES );
	aIo.OpenFlagRec();
	aIo.CloseFlagRec();
	while( aIo.BytesLeft() && !aIo.HasError() )
	{
		switch( aIo.Peek() )
		{
		case SW_LAYCACHE_IO_REC_PARA:
			aIo.OpenRec( SW_LAYCACHE_IO_REC_PARA );
			cFlags = aIo.OpenFlagRec();
			aIo.GetStream() >> nIndex;
			if( (cFlags & 0x01) != 0 )
				aIo.GetStream() >> nOffset;
			else
				nOffset = STRING_LEN;
			aIo.CloseFlagRec();
			Insert( SW_LAYCACHE_IO_REC_PARA, nIndex, (xub_StrLen)nOffset );
			aIo.CloseRec( SW_LAYCACHE_IO_REC_PARA );
			break;
		case SW_LAYCACHE_IO_REC_TABLE:
			aIo.OpenRec( SW_LAYCACHE_IO_REC_TABLE );
			aIo.OpenFlagRec();
            aIo.GetStream() >> nIndex
                            >> nOffset;
			Insert( SW_LAYCACHE_IO_REC_TABLE, nIndex, (xub_StrLen)nOffset );
			aIo.CloseFlagRec();
			aIo.CloseRec( SW_LAYCACHE_IO_REC_TABLE );
			break;
        case SW_LAYCACHE_IO_REC_FLY:
        {
            aIo.OpenRec( SW_LAYCACHE_IO_REC_FLY );
			aIo.OpenFlagRec();
			aIo.CloseFlagRec();
            long nX, nY, nW, nH;
            sal_uInt16 nPgNum;
            aIo.GetStream() >> nPgNum >> nIndex
                    >> nX >> nY >> nW >> nH;
            SwFlyCache* pFly = new SwFlyCache( nPgNum, nIndex, nX, nY, nW, nH );
            aFlyCache.Insert( pFly, aFlyCache.Count() );
            aIo.CloseRec( SW_LAYCACHE_IO_REC_FLY );
			break;
        }
		default:
			aIo.SkipRec();
			break;
		}
	}
	aIo.CloseRec( SW_LAYCACHE_IO_REC_PAGES );

	return !aIo.HasError();
}

/*-----------------28.5.2001 10:19------------------
 * SwLayoutCache::Write(..)
 * writes the index (more precise: the difference between
 * the index and the first index of the document content)
 * of the first paragraph/table at the top of every page.
 * If at the top of a page is the rest of a paragraph/table
 * from the bottom of the previous page, the character/row
 * number is stored, too.
 * The position, size and page number of the text frames
 * are stored, too
 * --------------------------------------------------*/

void SwLayoutCache::Write( SvStream &rStream, const SwDoc& rDoc )
{
    if( rDoc.GetCurrentLayout() ) // the layout itself ..	//swmod 080218
    {
		SwLayCacheIoImpl aIo( rStream, sal_True );
        // We want to save the relative index, so we need the index
        // of the first content
        sal_uLong nStartOfContent = rDoc.GetNodes().GetEndOfContent().
                                StartOfSectionNode()->GetIndex();
        // The first page..
        SwPageFrm* pPage = (SwPageFrm*)rDoc.GetCurrentLayout()->Lower();	//swmod 080218

		aIo.OpenRec( SW_LAYCACHE_IO_REC_PAGES );
		aIo.OpenFlagRec( 0, 0 );
		aIo.CloseFlagRec();
        while( pPage )
        {
            if( pPage->GetPrev() )
            {
                SwLayoutFrm* pLay = pPage->FindBodyCont();
                SwFrm* pTmp = pLay ? pLay->ContainsAny() : NULL;
                // We are only interested in paragraph or table frames,
                // a section frames contains paragraphs/tables.
                if( pTmp && pTmp->IsSctFrm() )
                    pTmp = ((SwSectionFrm*)pTmp)->ContainsAny();

                if( pTmp ) // any content
                {
                    if( pTmp->IsTxtFrm() )
                    {
                        sal_uLong nNdIdx = ((SwTxtFrm*)pTmp)->GetNode()->GetIndex();
                        if( nNdIdx > nStartOfContent )
                        {
                            /*  Open Paragraph Record */
                            aIo.OpenRec( SW_LAYCACHE_IO_REC_PARA );
                            sal_Bool bFollow = ((SwTxtFrm*)pTmp)->IsFollow();
                            aIo.OpenFlagRec( bFollow ? 0x01 : 0x00,
                                            bFollow ? 8 : 4 );
                            nNdIdx -= nStartOfContent;
                            aIo.GetStream() << static_cast<sal_uInt32>(nNdIdx);
                            if( bFollow )
                                aIo.GetStream() << static_cast<sal_uInt32>(((SwTxtFrm*)pTmp)->GetOfst());
                            aIo.CloseFlagRec();
                            /*  Close Paragraph Record */
                            aIo.CloseRec( SW_LAYCACHE_IO_REC_PARA );
                        }
                    }
                    else if( pTmp->IsTabFrm() )
                    {
                        SwTabFrm* pTab = (SwTabFrm*)pTmp;
                        sal_uLong nOfst = STRING_LEN;
                        if( pTab->IsFollow() )
                        {
                            // If the table is a follow, we have to look for the
                            // master and to count all rows to get the row number
                            nOfst = 0;
                            if( pTab->IsFollow() )
                                pTab = pTab->FindMaster( true );
                            while( pTab != pTmp )
                            {
                                SwFrm* pSub = pTab->Lower();
                                while( pSub )
                                {
                                    ++nOfst;
                                    pSub = pSub->GetNext();
                                }
                                pTab = pTab->GetFollow();
                                ASSERT( pTab, "Table follow without master" );
                            }
                        }
                        do
                        {
                            sal_uLong nNdIdx =
                                    pTab->GetTable()->GetTableNode()->GetIndex();
                            if( nNdIdx > nStartOfContent )
                            {
                                /* Open Table Record */
                                aIo.OpenRec( SW_LAYCACHE_IO_REC_TABLE );
                                aIo.OpenFlagRec( 0, 8 );
                                nNdIdx -= nStartOfContent;
                                aIo.GetStream() << static_cast<sal_uInt32>(nNdIdx)
                                                << static_cast<sal_uInt32>(nOfst);
                                aIo.CloseFlagRec();
                                /* Close Table Record  */
                                aIo.CloseRec( SW_LAYCACHE_IO_REC_TABLE );
                            }
                            // If the table has a follow on the next page,
                            // we know already the row number and store this
                            // immediately.
                            if( pTab->GetFollow() )
                            {
                                if( nOfst == STRING_LEN )
                                    nOfst = 0;
                                do
                                {
                                    SwFrm* pSub = pTab->Lower();
                                    while( pSub )
                                    {
                                        ++nOfst;
                                        pSub = pSub->GetNext();
                                    }
                                    pTab = pTab->GetFollow();
                                    SwPageFrm *pTabPage = pTab->FindPageFrm();
                                    if( pTabPage != pPage )
                                    {
                                        ASSERT( pPage->GetPhyPageNum() <
                                                pTabPage->GetPhyPageNum(),
                                                "Looping Tableframes" );
                                        pPage = pTabPage;
                                        break;
                                    }
                                } while ( pTab->GetFollow() );
                            }
                            else
                                break;
                        } while( pTab );
                    }
                }
            }
            if( pPage->GetSortedObjs() )
            {
                SwSortedObjs &rObjs = *pPage->GetSortedObjs();
                for ( sal_uInt16 i = 0; i < rObjs.Count(); ++i )
                {
                    SwAnchoredObject* pAnchoredObj = rObjs[i];
                    if ( pAnchoredObj->ISA(SwFlyFrm) )
                    {
                        SwFlyFrm *pFly = static_cast<SwFlyFrm*>(pAnchoredObj);
                        if( pFly->Frm().Left() != WEIT_WECH &&
                            !pFly->GetAnchorFrm()->FindFooterOrHeader() )
                        {
                            const SwContact *pC =
                                    ::GetUserCall(pAnchoredObj->GetDrawObj());
                            if( pC )
                            {
                                sal_uInt32 nOrdNum = pAnchoredObj->GetDrawObj()->GetOrdNum();
                                sal_uInt16 nPageNum = pPage->GetPhyPageNum();
                                /* Open Fly Record */
                                aIo.OpenRec( SW_LAYCACHE_IO_REC_FLY );
                                aIo.OpenFlagRec( 0, 0 );
                                aIo.CloseFlagRec();
                                SwRect &rRct = pFly->Frm();
                                sal_Int32 nX = rRct.Left() - pPage->Frm().Left();
                                sal_Int32 nY = rRct.Top() - pPage->Frm().Top();
                                aIo.GetStream() << nPageNum << nOrdNum
                                                << nX << nY << rRct.Width()
                                                << rRct.Height();
                                /* Close Fly Record  */
                                aIo.CloseRec( SW_LAYCACHE_IO_REC_FLY );
                            }
                        }
                    }
                }
            }
            pPage = (SwPageFrm*)pPage->GetNext();
        }
		aIo.CloseRec( SW_LAYCACHE_IO_REC_PAGES );
    }
}

#ifdef DBG_UTIL
sal_Bool SwLayoutCache::CompareLayout( const SwDoc& rDoc ) const
{
    if( !pImpl )
        return sal_True;
    const SwRootFrm *pRootFrm = rDoc.GetCurrentLayout();
    sal_Bool bRet = sal_True;
    if( pRootFrm )
    {
        sal_uInt16 nIndex = 0;
        sal_uLong nStartOfContent = rDoc.GetNodes().GetEndOfContent().
                                StartOfSectionNode()->GetIndex();
        SwPageFrm* pPage = (SwPageFrm*)pRootFrm->Lower();
        if( pPage )
            pPage = (SwPageFrm*)pPage->GetNext();
        while( pPage )
        {
            if( nIndex >= pImpl->Count() )
            {
                if( bRet )
                    bRet = sal_False;
                break;
            }
            SwLayoutFrm* pLay = pPage->FindBodyCont();
            SwFrm* pTmp = pLay ? pLay->ContainsAny() : NULL;
            if( pTmp && pTmp->IsSctFrm() )
                pTmp = ((SwSectionFrm*)pTmp)->ContainsAny();
            if( pTmp )
            {
                if( pTmp->IsTxtFrm() )
                {
                    sal_uLong nNdIdx = ((SwTxtFrm*)pTmp)->GetNode()->GetIndex();
                    if( nNdIdx > nStartOfContent )
                    {
						sal_Bool bFollow = ((SwTxtFrm*)pTmp)->IsFollow();
                        nNdIdx -= nStartOfContent;
                        if( pImpl->GetBreakIndex( nIndex ) != nNdIdx ||
                            SW_LAYCACHE_IO_REC_PARA !=
                            pImpl->GetBreakType( nIndex ) ||
                            ( bFollow ? ((SwTxtFrm*)pTmp)->GetOfst()
                              : STRING_LEN ) != pImpl->GetBreakOfst( nIndex ) )
                        {
                            if( bRet )
                                bRet = sal_False;
                        }
                        ++nIndex;
                    }
                }
                else if( pTmp->IsTabFrm() )
                {
                    SwTabFrm* pTab = (SwTabFrm*)pTmp;
                    sal_uLong nOfst = STRING_LEN;
                    if( pTab->IsFollow() )
                    {
                        nOfst = 0;
                        if( pTab->IsFollow() )
                            pTab = pTab->FindMaster( true );
                        while( pTab != pTmp )
                        {
                            SwFrm* pSub = pTab->Lower();
                            while( pSub )
                            {
                                ++nOfst;
                                pSub = pSub->GetNext();
                            }
                            pTab = pTab->GetFollow();
                        }
                    }
                    do
                    {
                        sal_uLong nNdIdx =
                                pTab->GetTable()->GetTableNode()->GetIndex();
                        if( nNdIdx > nStartOfContent )
                        {
                            nNdIdx -= nStartOfContent;
                            if( pImpl->GetBreakIndex( nIndex ) != nNdIdx ||
                                SW_LAYCACHE_IO_REC_TABLE !=
                                pImpl->GetBreakType( nIndex ) ||
                               nOfst != pImpl->GetBreakOfst( nIndex ) )
                            {
                                if( bRet )
                                    bRet = sal_False;
                            }
                            ++nIndex;
                        }
                        if( pTab->GetFollow() )
                        {
                            if( nOfst == STRING_LEN )
                                nOfst = 0;
                            do
                            {
                                SwFrm* pSub = pTab->Lower();
                                while( pSub )
                                {
                                    ++nOfst;
                                    pSub = pSub->GetNext();
                                }
                                pTab = pTab->GetFollow();
                                SwPageFrm *pTabPage = pTab->FindPageFrm();
                                if( pTabPage != pPage )
                                {
                                    pPage = pTabPage;
                                    break;
                                }
                            } while ( pTab->GetFollow() );
                        }
                        else
                            break;
                    } while( pTab );
                }
            }
            pPage = (SwPageFrm*)pPage->GetNext();
        }
    }
    return bRet;
}
#endif

void SwLayoutCache::ClearImpl()
{
    if( !IsLocked() )
    {
        delete pImpl;
        pImpl = 0;
    }
}


SwLayoutCache::~SwLayoutCache()
{
    ASSERT( !nLockCount, "Deleting a locked SwLayoutCache!?" );
    delete pImpl;
}

/*-----------------28.5.2001 10:47------------------
 * SwActualSection,
 *  a help class to create not nested section frames
 *  for nested sections.
 * --------------------------------------------------*/

SwActualSection::SwActualSection( SwActualSection *pUp,
								  SwSectionFrm	  *pSect,
								  SwSectionNode	  *pNd ) :
	pUpper( pUp ),
	pSectFrm( pSect ),
	pSectNode( pNd )
{
	if ( !pSectNode )
	{
		const SwNodeIndex *pIndex = pSect->GetFmt()->GetCntnt().GetCntntIdx();
        pSectNode = pIndex->GetNode().FindSectionNode();
    }
}

/*-----------------28.5.2001 11:09------------------
 * SwLayHelper
 *  is the helper class, which utilizes the layout cache information
 *  to distribute the document content to the rigth pages.
 * It's used by the _InsertCnt(..)-function.
 * If there's no layout cache, the distibution to the pages is more
 * a guess, but a guess with statistical background.
 * --------------------------------------------------*/

SwLayHelper::SwLayHelper( SwDoc *pD, SwFrm* &rpF, SwFrm* &rpP, SwPageFrm* &rpPg,
            SwLayoutFrm* &rpL, SwActualSection* &rpA, sal_Bool &rB,
            sal_uLong nNodeIndex, sal_Bool bCache )
    : rpFrm( rpF ), rpPrv( rpP ), rpPage( rpPg ), rpLay( rpL ),
      rpActualSection( rpA ), rbBreakAfter(rB), pDoc(pD), nMaxParaPerPage( 25 ),
      nParagraphCnt( bCache ? 0 : USHRT_MAX ), bFirst( bCache )
{
    pImpl = pDoc->GetLayoutCache() ? pDoc->GetLayoutCache()->LockImpl() : NULL;
    if( pImpl )
    {
        nMaxParaPerPage = 1000;
        nStartOfContent = pDoc->GetNodes().GetEndOfContent().StartOfSectionNode()
                          ->GetIndex();
        nNodeIndex -= nStartOfContent;
        nIndex = 0;
        nFlyIdx = 0;
        while( nIndex < pImpl->Count() && (*pImpl)[ nIndex ] < nNodeIndex )
            ++nIndex;
        if( nIndex >= pImpl->Count() )
        {
            pDoc->GetLayoutCache()->UnlockImpl();
            pImpl = NULL;
        }
    }
    else
    {
        nIndex = USHRT_MAX;
        nStartOfContent = ULONG_MAX;
    }
}

SwLayHelper::~SwLayHelper()
{
    if( pImpl )
    {
        ASSERT( pDoc && pDoc->GetLayoutCache(), "Missing layoutcache" );
        pDoc->GetLayoutCache()->UnlockImpl();
    }
}

/*-----------------23.5.2001 16:40------------------
 * SwLayHelper::CalcPageCount() does not really calculate the page count,
 * it returns the page count value from the layout cache, if available,
 * otherwise it estimates the page count.
 * --------------------------------------------------*/

sal_uLong SwLayHelper::CalcPageCount()
{
    sal_uLong nPgCount;
    SwLayCacheImpl *pCache = pDoc->GetLayoutCache() ?
                             pDoc->GetLayoutCache()->LockImpl() : NULL;
    if( pCache )
    {
        nPgCount = pCache->Count() + 1;
        pDoc->GetLayoutCache()->UnlockImpl();
    }
    else
    {
		nPgCount = pDoc->GetDocStat().nPage;
        if ( nPgCount <= 10 ) // no page insertion for less than 10 pages
			nPgCount = 0;
		sal_uLong nNdCount = pDoc->GetDocStat().nPara;
		if ( nNdCount <= 1 )
		{
            //Estimates the number of paragraphs.
			sal_uLong nTmp = pDoc->GetNodes().GetEndOfContent().GetIndex() -
						pDoc->GetNodes().GetEndOfExtras().GetIndex();
            //Tables have a little overhead..
			nTmp -= pDoc->GetTblFrmFmts()->Count() * 25;
            //Fly frames, too ..
			nTmp -= (pDoc->GetNodes().GetEndOfAutotext().GetIndex() -
					   pDoc->GetNodes().GetEndOfInserts().GetIndex()) / 3 * 5;
			if ( nTmp > 0 )
				nNdCount = nTmp;
		}
        if ( nNdCount > 100 ) // no estimation below this value
		{
			if ( nPgCount > 0 )
				nMaxParaPerPage = nNdCount / nPgCount;
			else
			{
				nMaxParaPerPage = Max( sal_uLong(20),
									   sal_uLong(20 + nNdCount / 1000 * 3) );
#ifdef PM2
				const sal_uLong nMax = 49;
#else
				const sal_uLong nMax = 53;
#endif
				nMaxParaPerPage = Min( nMaxParaPerPage, nMax );
				nPgCount = nNdCount / nMaxParaPerPage;
			}
            if ( nNdCount < 1000 )
                nPgCount = 0;// no progress bar for small documents
            ViewShell *pSh = 0;
            if( rpLay && rpLay->getRootFrm() )
                pSh = rpLay->getRootFrm()->GetCurrShell();
            if( pSh && pSh->GetViewOptions()->getBrowseMode() )
				nMaxParaPerPage *= 6;
		}
    }
    return nPgCount;
}

/*-----------------23.5.2001 16:44------------------
 * SwLayHelper::CheckInsertPage()
 * inserts a page and return sal_True, if
 * - the break after flag is set
 * - the actual content wants a break before
 * - the maximum count of paragraph/rows is reached
 *
 * The break after flag is set, if the actual content
 * wants a break after.
 * --------------------------------------------------*/

sal_Bool SwLayHelper::CheckInsertPage()
{
	sal_Bool bEnd = 0 == rpPage->GetNext();
    const SwAttrSet* pAttr = rpFrm->GetAttrSet();
	const SvxFmtBreakItem& rBrk = pAttr->GetBreak();
	const SwFmtPageDesc& rDesc = pAttr->GetPageDesc();
    // --> FME 2004-10-26 #118195# Do not evaluate page description if frame
    // is a follow frame!
    const SwPageDesc* pDesc = rpFrm->IsFlowFrm() &&
                              SwFlowFrm::CastFlowFrm( rpFrm )->IsFollow() ?
                              0 :
                              rDesc.GetPageDesc();
    // <--

	sal_Bool bBrk = nParagraphCnt > nMaxParaPerPage || rbBreakAfter;
	rbBreakAfter = rBrk.GetBreak() == SVX_BREAK_PAGE_AFTER ||
				   rBrk.GetBreak() == SVX_BREAK_PAGE_BOTH;
	if ( !bBrk )
		bBrk = rBrk.GetBreak() == SVX_BREAK_PAGE_BEFORE ||
			   rBrk.GetBreak() == SVX_BREAK_PAGE_BOTH;

	if ( bBrk || pDesc )
	{
		sal_uInt16 nPgNum = 0;
		if ( !pDesc )
			pDesc = rpPage->GetPageDesc()->GetFollow();
		else
		{
			if ( 0 != (nPgNum = rDesc.GetNumOffset()) )
				((SwRootFrm*)rpPage->GetUpper())->SetVirtPageNum(sal_True);
		}
		sal_Bool bNextPageOdd = !rpPage->OnRightPage();
		sal_Bool bInsertEmpty = sal_False;
		if( nPgNum && bNextPageOdd != ( ( nPgNum % 2 ) != 0 ) )
		{
			bNextPageOdd = !bNextPageOdd;
			bInsertEmpty = sal_True;
		}
		::InsertNewPage( (SwPageDesc&)*pDesc, rpPage->GetUpper(),
						 bNextPageOdd, bInsertEmpty, sal_False, rpPage->GetNext() );
		if ( bEnd )
		{
			ASSERT( rpPage->GetNext(), "Keine neue Seite?" );
			do
			{	rpPage = (SwPageFrm*)rpPage->GetNext();
			} while ( rpPage->GetNext() );
		}
		else
		{
			ASSERT( rpPage->GetNext(), "Keine neue Seite?" );
			rpPage = (SwPageFrm*)rpPage->GetNext();
			if ( rpPage->IsEmptyPage() )
			{
				ASSERT( rpPage->GetNext(), "Keine neue Seite?" );
				rpPage = (SwPageFrm*)rpPage->GetNext();
			}
		}
		rpLay = rpPage->FindBodyCont();
		while( rpLay->Lower() )
			rpLay = (SwLayoutFrm*)rpLay->Lower();
		return sal_True;
	}
	return sal_False;
}

// --> OD 2006-03-22 #b6375613#
bool lcl_HasTextFrmAnchoredObjs( SwTxtFrm* p_pTxtFrm )
{
    bool bHasTextFrmAnchoredObjs( false );

    const SwSpzFrmFmts* pSpzFrmFmts = p_pTxtFrm->GetTxtNode()->GetDoc()->GetSpzFrmFmts();
    for ( sal_uInt16 i = 0; i < pSpzFrmFmts->Count(); ++i )
    {
        SwFrmFmt *pFmt = (SwFrmFmt*)(*pSpzFrmFmts)[i];
        const SwFmtAnchor &rAnch = pFmt->GetAnchor();
        if ( rAnch.GetCntntAnchor() &&
             ((rAnch.GetAnchorId() == FLY_AT_PARA) ||
              (rAnch.GetAnchorId() == FLY_AT_CHAR)) &&
             rAnch.GetCntntAnchor()->nNode.GetIndex() ==
                                        p_pTxtFrm->GetTxtNode()->GetIndex() )
        {
            bHasTextFrmAnchoredObjs = true;
            break;
        }
    }

    return bHasTextFrmAnchoredObjs;
}

void lcl_ApplyWorkaroundForB6375613( SwFrm* p_pFirstFrmOnNewPage )
{
    SwTxtFrm* pFirstTextFrmOnNewPage = dynamic_cast<SwTxtFrm*>(p_pFirstFrmOnNewPage);
    //
    if ( pFirstTextFrmOnNewPage &&
         !pFirstTextFrmOnNewPage->IsFollow() &&
         pFirstTextFrmOnNewPage->GetTxt().Len() == 0 &&
         lcl_HasTextFrmAnchoredObjs( pFirstTextFrmOnNewPage ) )
    {
        // apply page break before at this text frame to assure, that it doesn't flow backward.
        const SvxBreak eBreak =
                    pFirstTextFrmOnNewPage->GetAttrSet()->GetBreak().GetBreak();
        if ( eBreak == SVX_BREAK_NONE )
        {
            pFirstTextFrmOnNewPage->GetTxtNode()->LockModify();
            SwDoc* pDoc( pFirstTextFrmOnNewPage->GetTxtNode()->GetDoc() );
            IDocumentContentOperations* pIDCO = pFirstTextFrmOnNewPage->GetTxtNode()->getIDocumentContentOperations();
            const SwPaM aTmpPaM( *(pFirstTextFrmOnNewPage->GetTxtNode()) );
            pIDCO->InsertPoolItem(
                aTmpPaM, SvxFmtBreakItem( SVX_BREAK_PAGE_BEFORE, RES_BREAK ), 0 );
            pFirstTextFrmOnNewPage->GetTxtNode()->UnlockModify();

            uno::Reference< document::XDocumentInfoSupplier > xDoc(
                                        pDoc->GetDocShell()->GetBaseModel(),
                                        uno::UNO_QUERY);
            uno::Reference< beans::XPropertySet > xDocInfo(
                                        xDoc->getDocumentInfo(),
                                        uno::UNO_QUERY );
            try
            {
                xDocInfo->setPropertyValue( rtl::OUString::createFromAscii("WorkaroundForB6375613Applied"), uno::makeAny( true ) );
            }
            catch( uno::Exception& )
            {
            }
        }
    }
}
// <--

/*-----------------28.5.2001 11:31------------------
 * SwLayHelper::CheckInsert
 *  is the entry point for the _InsertCnt-function.
 *  The document content index is checked either it is
 *  in the layout cache either it's time to insert a page
 *  cause the maximal estimation of content per page is reached.
 *  A really big table or long paragraph may contains more than
 *  one page, in this case the needed count of pages will inserted.
 * --------------------------------------------------*/

sal_Bool SwLayHelper::CheckInsert( sal_uLong nNodeIndex )
{
    sal_Bool bRet = sal_False;
    sal_Bool bLongTab = sal_False;
    sal_uLong nMaxRowPerPage( 0 );
    nNodeIndex -= nStartOfContent;
    sal_uInt16 nRows( 0 );
    if( rpFrm->IsTabFrm() )
    {
        //Inside a table counts every row as a paragraph
        SwFrm *pLow = ((SwTabFrm*)rpFrm)->Lower();
        nRows = 0;
        do
        {
            ++nRows;
            pLow = pLow->GetNext();
        } while ( pLow );
        nParagraphCnt += nRows;
        if( !pImpl && nParagraphCnt > nMaxParaPerPage + 10 )
        {
            // OD 09.04.2003 #108698# - improve heuristics:
            // Assume that a table, which has more than three times the quantity
            // of maximal paragraphs per page rows, consists of rows, which have
            // the height of a normal paragraph. Thus, allow as much rows per page
            // as much paragraphs are allowed.
            if ( nRows > ( 3*nMaxParaPerPage ) )
            {
                nMaxRowPerPage = nMaxParaPerPage;
            }
            else
            {
                SwFrm *pTmp = ((SwTabFrm*)rpFrm)->Lower();
                if( pTmp->GetNext() )
                    pTmp = pTmp->GetNext();
                pTmp = ((SwRowFrm*)pTmp)->Lower();
                sal_uInt16 nCnt = 0;
                do
                {
                    ++nCnt;
                    pTmp = pTmp->GetNext();
                } while( pTmp );
                nMaxRowPerPage = Max( sal_uLong(2), nMaxParaPerPage / nCnt );
            }
            bLongTab = sal_True;
        }
    }
    else
        ++nParagraphCnt;
    if( bFirst && pImpl && nIndex < pImpl->Count() &&
        pImpl->GetBreakIndex( nIndex ) == nNodeIndex &&
        ( pImpl->GetBreakOfst( nIndex ) < STRING_LEN ||
          ( ++nIndex < pImpl->Count() &&
          pImpl->GetBreakIndex( nIndex ) == nNodeIndex ) ) )
        bFirst = sal_False;
#if OSL_DEBUG_LEVEL > 1
    sal_uLong nBreakIndex = ( pImpl && nIndex < pImpl->Count() ) ?
                        pImpl->GetBreakIndex(nIndex) : 0xffff;
    (void)nBreakIndex;
#endif
    // OD 09.04.2003 #108698# - always split a big tables.
    if ( !bFirst ||
         ( rpFrm->IsTabFrm() && bLongTab )
       )
    {
        sal_uLong nRowCount = 0;
        do
        {
            if( pImpl || bLongTab )
            {
#if OSL_DEBUG_LEVEL > 1
                sal_uLong nBrkIndex = ( pImpl && nIndex < pImpl->Count() ) ?
                        pImpl->GetBreakIndex(nIndex) : 0xffff;
                (void)nBrkIndex;
#endif
                xub_StrLen nOfst = STRING_LEN;
                sal_uInt16 nType = SW_LAYCACHE_IO_REC_PAGES;
                if( bLongTab )
                {
                    rbBreakAfter = sal_True;
                    nOfst = static_cast<xub_StrLen>(nRowCount + nMaxRowPerPage);
                }
                else
                {
                    while( nIndex < pImpl->Count() &&
                           pImpl->GetBreakIndex(nIndex) < nNodeIndex)
                        ++nIndex;
                    if( nIndex < pImpl->Count() &&
                        pImpl->GetBreakIndex(nIndex) == nNodeIndex )
                    {
                        nType = pImpl->GetBreakType( nIndex );
                        nOfst = pImpl->GetBreakOfst( nIndex++ );
                        rbBreakAfter = sal_True;
                    }
                }

                if( nOfst < STRING_LEN )
                {
                    sal_Bool bSplit = sal_False;
                    sal_uInt16 nRepeat( 0 );
                    if( !bLongTab && rpFrm->IsTxtFrm() &&
                        SW_LAYCACHE_IO_REC_PARA == nType &&
                        nOfst<((SwTxtFrm*)rpFrm)->GetTxtNode()->GetTxt().Len() )
                        bSplit = sal_True;
                    else if( rpFrm->IsTabFrm() && nRowCount < nOfst &&
                             ( bLongTab || SW_LAYCACHE_IO_REC_TABLE == nType ) )
                    {
                        nRepeat = ((SwTabFrm*)rpFrm)->
                                  GetTable()->GetRowsToRepeat();
                        bSplit = nOfst < nRows && nRowCount + nRepeat < nOfst;
                        bLongTab = bLongTab && bSplit;
                    }
                    if( bSplit )
                    {
                        rpFrm->InsertBehind( rpLay, rpPrv );
                        rpFrm->Frm().Pos() = rpLay->Frm().Pos();
                        rpFrm->Frm().Pos().Y() += 1;
                        rpPrv = rpFrm;
                        if( rpFrm->IsTabFrm() )
                        {
                            SwTabFrm* pTab = (SwTabFrm*)rpFrm;
                            // --> OD 2004-09-23 #i33629#, #i29955#
                            ::RegistFlys( pTab->FindPageFrm(), pTab );
                            // <--
                            SwFrm *pRow = pTab->Lower();
                            SwTabFrm *pFoll = new SwTabFrm( *pTab );

                            SwFrm *pPrv;
                            if( nRepeat > 0 )
                            {
                                bDontCreateObjects = sal_True; //frmtool

                                // Insert new headlines:
                                sal_uInt16 nRowIdx = 0;
                                SwRowFrm* pHeadline = 0;
                                while( nRowIdx < nRepeat )
                                {
                                    ASSERT( pTab->GetTable()->GetTabLines()[ nRowIdx ], "Table ohne Zeilen?" );
                                    pHeadline =
                                        new SwRowFrm( *pTab->GetTable()->GetTabLines()[ nRowIdx ], pTab );
                                    pHeadline->SetRepeatedHeadline( true );
                                    pHeadline->InsertBefore( pFoll, 0 );
                                    pHeadline->RegistFlys();

                                    ++nRowIdx;
                                }

                                bDontCreateObjects = sal_False;
                                pPrv = pHeadline;
                                nRows = nRows + nRepeat;
                            }
                            else
                                pPrv = 0;
                            while( pRow && nRowCount < nOfst )
                            {
                                pRow = pRow->GetNext();
                                ++nRowCount;
                            }
                            while ( pRow )
                            {
                                SwFrm* pNxt = pRow->GetNext();
                                pRow->Remove();
                                pRow->InsertBehind( pFoll, pPrv );
                                pPrv = pRow;
                                pRow = pNxt;
                            }
                            rpFrm = pFoll;
                        }
                        else
                        {
                            SwTxtFrm *pNew = new SwTxtFrm( ((SwTxtFrm*)rpFrm)->
                                                            GetTxtNode(), rpFrm );
                            pNew->_SetIsFollow( sal_True );
                            pNew->ManipOfst( nOfst );
                            pNew->SetFollow( ((SwTxtFrm*)rpFrm)->GetFollow() );
                            ((SwTxtFrm*)rpFrm)->SetFollow( pNew );
                            rpFrm = pNew;
                        }
                    }
                }
            }

            SwPageFrm* pLastPage = rpPage;
            if( CheckInsertPage() )
            {
                // --> OD 2006-03-21 #b6375613#
                if ( pDoc->ApplyWorkaroundForB6375613() )
                {
                    lcl_ApplyWorkaroundForB6375613( rpFrm );
                }
                // <--
                _CheckFlyCache( pLastPage );
                if( rpPrv && rpPrv->IsTxtFrm() && !rpPrv->GetValidSizeFlag() )
                    rpPrv->Frm().Height( rpPrv->GetUpper()->Prt().Height() );

                bRet = sal_True;
                rpPrv = 0;
                nParagraphCnt = 0;

                if ( rpActualSection )
                {
                    //Hatte der SectionFrm ueberhaupt Inhalt? Wenn
                    //nicht kann er gleich umgehaengt werden.
                    SwSectionFrm *pSct;
                    bool bInit = false;
                    if ( !rpActualSection->GetSectionFrm()->ContainsCntnt())
                    {
                        pSct = rpActualSection->GetSectionFrm();
                        pSct->Remove();
                    }
                    else
                    {
                        pSct = new SwSectionFrm(
                            *rpActualSection->GetSectionFrm(), sal_False );
                        rpActualSection->GetSectionFrm()->SimpleFormat();
                        bInit = true;
                    }
                    rpActualSection->SetSectionFrm( pSct );
                    pSct->InsertBehind( rpLay, 0 );
                    if( bInit )
                        pSct->Init();
                    pSct->Frm().Pos() = rpLay->Frm().Pos();
                    pSct->Frm().Pos().Y() += 1; //wg. Benachrichtigungen.

                    rpLay = pSct;
                    if ( rpLay->Lower() && rpLay->Lower()->IsLayoutFrm() )
                        rpLay = rpLay->GetNextLayoutLeaf();
                }
            }
        } while( bLongTab || ( pImpl && nIndex < pImpl->Count() &&
                 (*pImpl)[ nIndex ] == nNodeIndex ) );
    }
    bFirst = sal_False;
    return bRet;
}

struct SdrObjectCompare
{
  bool operator()( const SdrObject* pF1, const SdrObject* pF2 ) const
  {
    return pF1->GetOrdNum() < pF2->GetOrdNum();
  }
};

struct FlyCacheCompare
{
  bool operator()( const SwFlyCache* pC1, const SwFlyCache* pC2 ) const
  {
    return pC1->nOrdNum < pC2->nOrdNum;
  }
};

 /*-----------------28.6.2001 14:40------------------
  * SwLayHelper::_CheckFlyCache(..)
  * If a new page is inserted, the last page is analysed.
  * If there are text frames with default position, the fly cache
  * is checked, if these frames are stored in the cache.
  * --------------------------------------------------*/

void SwLayHelper::_CheckFlyCache( SwPageFrm* pPage )
{
    if( !pImpl || !pPage )
        return;
    sal_uInt16 nFlyCount = pImpl->GetFlyCount();
    // Any text frames at the page, fly cache available?
    if( pPage->GetSortedObjs() && nFlyIdx < nFlyCount )
    {
        SwSortedObjs &rObjs = *pPage->GetSortedObjs();
        sal_uInt16 nPgNum = pPage->GetPhyPageNum();

/*

        //
        // NOTE: This code assumes that all objects have already been
        // inserted into the drawing layout, so that the cached objects
        // can be identified by their ordnum. Unfortunately this function
        // is called with page n if page n+1 has been inserted. Thus
        // not all the objects have been inserted and the ordnums cannot
        // be used to identify the objects.
        //

        for ( sal_uInt16 i = 0; i < rObjs.Count(); ++i )  // check objects
        {
            SdrObject *pO = rObjs[i];
            if ( pO->ISA(SwVirtFlyDrawObj) )  // a text frame?
            {
                SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pO)->GetFlyFrm();
                if( pFly->Frm().Left() == WEIT_WECH && pFly->GetAnchor() &&
                    !pFly->GetAnchor()->FindFooterOrHeader() )
                {   // Only frame with default position and not in header/footer
                    const SwContact *pC = (SwContact*)GetUserCall(pO);
                    if( pC )
                    {
                        sal_uLong nOrdNum = pO->GetOrdNum(); // the Id
                        SwFlyCache* pFlyC;
                        while( nFlyIdx < nFlyCount && ( pFlyC = pImpl->
                               GetFlyCache(nFlyIdx) )->nPageNum < nPgNum)
                            ++nFlyIdx;
                        if( nFlyIdx < nFlyCount &&
                            pFlyC->nPageNum == nPgNum )
                        {
                            sal_uInt16 nIdx = nFlyIdx;
                            while( nIdx < nFlyCount && ( pFlyC = pImpl->
                                   GetFlyCache( nIdx ) )->nPageNum == nPgNum &&
                                   pFlyC->nOrdNum != nOrdNum )
                                ++nIdx;
                            if( nIdx < nFlyCount && pFlyC->nPageNum == nPgNum &&
                                pFlyC->nOrdNum == nOrdNum )
                            {   // we get the stored information
                                pFly->Frm().Pos().X() = pFlyC->Left() +
                                                        pPage->Frm().Left();
                                pFly->Frm().Pos().Y() = pFlyC->Top() +
                                                        pPage->Frm().Top();
                                pFly->Frm().Width( pFlyC->Width() );
                                pFly->Frm().Height( pFlyC->Height() );
                            }
                        }
                    }
                }
            }
        }
 */

        //
        // NOTE: Here we do not use the absolute ordnums but
        // relative ordnums for the objects on this page.

        // skip fly frames from pages before the current page
        SwFlyCache* pFlyC;
        while( nFlyIdx < nFlyCount && ( pFlyC = pImpl->
               GetFlyCache(nFlyIdx) )->nPageNum < nPgNum)
            ++nFlyIdx;

        // sort cached objects on this page by ordnum
        std::set< const SwFlyCache*, FlyCacheCompare > aFlyCacheSet;
        sal_uInt16 nIdx = nFlyIdx;

        while( nIdx < nFlyCount && ( pFlyC = pImpl->
               GetFlyCache( nIdx ) )->nPageNum == nPgNum )
        {
            aFlyCacheSet.insert( pFlyC );
            ++nIdx;
        }

        // sort objects on this page by ordnum
        std::set< const SdrObject*, SdrObjectCompare > aFlySet;
        for ( sal_uInt16 i = 0; i < rObjs.Count(); ++i )
        {
            SwAnchoredObject* pAnchoredObj = rObjs[i];
            if ( pAnchoredObj->ISA(SwFlyFrm) )  // a text frame?
            {
                SwFlyFrm *pFly = static_cast<SwFlyFrm*>(pAnchoredObj);
                if( pFly->GetAnchorFrm() &&
                    !pFly->GetAnchorFrm()->FindFooterOrHeader() )
                {
                    const SwContact *pC = ::GetUserCall( pAnchoredObj->GetDrawObj() );
                    if( pC )
                    {
                        aFlySet.insert( pAnchoredObj->GetDrawObj() );
                    }
                }
            }
        }

        if ( aFlyCacheSet.size() == aFlySet.size() )
        {
            std::set< const SwFlyCache*, FlyCacheCompare >::iterator aFlyCacheSetIt =
                    aFlyCacheSet.begin();
            std::set< const SdrObject*, SdrObjectCompare >::iterator aFlySetIt =
                    aFlySet.begin();

            while ( aFlyCacheSetIt != aFlyCacheSet.end() )
            {
                const SwFlyCache* pFlyCache = *aFlyCacheSetIt;
                SwFlyFrm* pFly = ((SwVirtFlyDrawObj*)*aFlySetIt)->GetFlyFrm();

                if ( pFly->Frm().Left() == WEIT_WECH )
                {
                    // we get the stored information
                    pFly->Frm().Pos().X() = pFlyCache->Left() +
                                            pPage->Frm().Left();
                    pFly->Frm().Pos().Y() = pFlyCache->Top() +
                                            pPage->Frm().Top();
                    if ( pImpl->IsUseFlyCache() )
                    {
                        pFly->Frm().Width( pFlyCache->Width() );
                        pFly->Frm().Height( pFlyCache->Height() );
                    }
                }

                ++aFlyCacheSetIt;
                ++aFlySetIt;
            }
        }
    }
}

/*-----------------28.6.2001 14:48------------------
 * SwLayHelper::CheckPageFlyCache(..)
 * looks for the given text frame in the fly cache and sets
 * the position and size, if possible.
 * The fly cache is sorted by pages and we start searching with the given page.
 * If we found the page number in the fly cache, we set
 * the rpPage parameter to the right page, if possible.
 * --------------------------------------------------*/

sal_Bool SwLayHelper::CheckPageFlyCache( SwPageFrm* &rpPage, SwFlyFrm* pFly )
{
    if( !pFly->GetAnchorFrm() || !pFly->GetVirtDrawObj() ||
        pFly->GetAnchorFrm()->FindFooterOrHeader() )
        return sal_False;
    sal_Bool bRet = sal_False;
    SwDoc* pDoc = rpPage->GetFmt()->GetDoc();
    SwLayCacheImpl *pCache = pDoc->GetLayoutCache() ?
                             pDoc->GetLayoutCache()->LockImpl() : NULL;
    if( pCache )
    {
        sal_uInt16 nPgNum = rpPage->GetPhyPageNum();
        sal_uInt16 nIdx = 0;
        sal_uInt16 nCnt = pCache->GetFlyCount();
        sal_uLong nOrdNum = pFly->GetVirtDrawObj()->GetOrdNum();
        SwFlyCache* pFlyC = 0;

        // skip fly frames from pages before the current page
        while( nIdx < nCnt &&
               nPgNum > (pFlyC = pCache->GetFlyCache( nIdx ))->nPageNum )
            ++nIdx;

        while( nIdx < nCnt &&
               nOrdNum != (pFlyC = pCache->GetFlyCache( nIdx ))->nOrdNum )
            ++nIdx;
        if( nIdx < nCnt )
        {
            SwPageFrm *pPage = rpPage;
            while( pPage && pPage->GetPhyPageNum() < pFlyC->nPageNum )
                pPage = (SwPageFrm*)pPage->GetNext();
            // --> OD 2005-02-22 #i43266# - if the found page is an empty page,
            // take the previous one (take next one, if previous one doesn't exists)
            if ( pPage && pPage->IsEmptyPage() )
            {
                pPage = static_cast<SwPageFrm*>( pPage->GetPrev()
                                                 ? pPage->GetPrev()
                                                 : pPage->GetNext() );
            }
            // <--
            if( pPage )
            {
                rpPage = pPage;
                pFly->Frm().Pos().X() = pFlyC->Left() + pPage->Frm().Left();
                pFly->Frm().Pos().Y() = pFlyC->Top() + pPage->Frm().Top();
                if ( pCache->IsUseFlyCache() )
                {
                    pFly->Frm().Width( pFlyC->Width() );
                    pFly->Frm().Height( pFlyC->Height() );
                }
                bRet = sal_True;
            }
        }
        pDoc->GetLayoutCache()->UnlockImpl();
    }
    return bRet;
}

// -----------------------------------------------------------------------------

SwLayCacheIoImpl::SwLayCacheIoImpl( SvStream& rStrm, sal_Bool bWrtMd ) :
	pStream( &rStrm ),
	nMajorVersion(SW_LAYCACHE_IO_VERSION_MAJOR),
	nMinorVersion(SW_LAYCACHE_IO_VERSION_MINOR),
	bWriteMode( bWrtMd ),
	bError( sal_False  )
{
	if( bWriteMode )
		*pStream << nMajorVersion
				 <<	nMinorVersion;

	else
		*pStream >> nMajorVersion
				 >>	nMinorVersion;
}

sal_Bool SwLayCacheIoImpl::OpenRec( sal_uInt8 cType )
{
	sal_Bool bRes = sal_True;
	size_t nLvl = aRecTypes.size();
	ASSERT( nLvl == aRecSizes.Count(), "OpenRec: Level" );
	sal_uInt32 nPos = pStream->Tell();
	if( bWriteMode )
	{
		aRecTypes.push_back( cType );
		aRecSizes.Insert( nPos, nLvl );
		*pStream << (sal_uInt32) 0;
	}
	else
	{
		sal_uInt32 nVal;
		*pStream >> nVal;
		sal_uInt8 cRecTyp = (sal_uInt8)nVal;
		aRecTypes.push_back( cRecTyp );
		sal_uInt32 nSize = nVal >> 8;
		aRecSizes.Insert( nPos + nSize, nLvl );
		if( !nVal || cRecTyp != cType ||
			pStream->GetErrorCode() != SVSTREAM_OK || pStream->IsEof() )
		{
			ASSERT( nVal, "OpenRec: Record-Header is 0" );
			ASSERT( cRecTyp == cType,
					"OpenRec: Wrong Record Type" );
			aRecTypes.back() = 0;
			aRecSizes[nLvl] = pStream->Tell();
			bRes = sal_False;
			bError = sal_True;
		}
	}
	return bRes;
}

// Close record

sal_Bool SwLayCacheIoImpl::CloseRec( sal_uInt8 )
{
	sal_Bool bRes = sal_True;
	size_t nLvl = aRecTypes.size();
	ASSERT( nLvl == aRecSizes.Count(), "CloseRec: wrong Level" );
	ASSERT( nLvl, "CloseRec: no levels" );
	if( nLvl )
	{
		nLvl--;
		sal_uInt32 nPos = pStream->Tell();
		if( bWriteMode )
		{
			sal_uInt32 nBgn = aRecSizes[nLvl];
			pStream->Seek( nBgn );
			sal_uInt32 nSize = nPos - nBgn;
			sal_uInt32 nVal = ( nSize << 8 ) | aRecTypes.back();
			*pStream << nVal;
			pStream->Seek( nPos );
			if( pStream->GetError() != SVSTREAM_OK )
			 	bRes = sal_False;
		}
		else
		{
			sal_uInt32 n = aRecSizes[nLvl];
			ASSERT( n >= nPos, "CloseRec: to much data read" );
			if( n != nPos )
			{
				pStream->Seek( n );
				if( n < nPos )
				 	bRes = sal_False;
			}
			if( pStream->GetErrorCode() != SVSTREAM_OK )
				bRes = sal_False;
		}

		aRecTypes.pop_back();
		aRecSizes.Remove( nLvl, 1 );
	}

	if( !bRes )
		bError = sal_True;

	return bRes;
}

sal_uInt32 SwLayCacheIoImpl::BytesLeft()
{
	sal_uInt16 nLvl = aRecSizes.Count();
	sal_uInt32 n = 0;
	if( !bError && nLvl )
	{
		sal_uInt32 nEndPos = aRecSizes[ nLvl-1 ];
		sal_uInt32 nPos = pStream->Tell();
		if( nEndPos > nPos )
			n = nEndPos - nPos;
	}

	return n;
}

sal_uInt8 SwLayCacheIoImpl::Peek()
{
	sal_uInt8 c = 0;
	if( !bError )
	{
		sal_uInt32 nPos = pStream->Tell();
		*pStream >> c;
		pStream->Seek( nPos );
		if( pStream->GetErrorCode() != SVSTREAM_OK )
		{
			c = 0;
			bError = sal_True;
		}
	}
	return c;
}

void SwLayCacheIoImpl::SkipRec()
{
	sal_uInt8 c = Peek();
	OpenRec( c );
	pStream->Seek( aRecSizes[aRecSizes.Count()-1] );
	CloseRec( c );
}

sal_uInt8 SwLayCacheIoImpl::OpenFlagRec()
{
	ASSERT( !bWriteMode, "OpenFlagRec illegal in write  mode" );
	sal_uInt8 cFlags;
	*pStream >> cFlags;
	nFlagRecEnd = pStream->Tell() + ( cFlags & 0x0F );
	return (cFlags >> 4);
}

void SwLayCacheIoImpl::OpenFlagRec( sal_uInt8 nFlags, sal_uInt8 nLen )
{
	ASSERT( bWriteMode, "OpenFlagRec illegal in read  mode" );
	ASSERT( (nFlags & 0xF0) == 0, "illegal flags set" );
	ASSERT( nLen < 16, "wrong flag record length" );
	sal_uInt8 cFlags = (nFlags << 4) + nLen;
	*pStream << cFlags;
	nFlagRecEnd = pStream->Tell() + nLen;
}

void SwLayCacheIoImpl::CloseFlagRec()
{
	if( bWriteMode )
	{
		ASSERT( pStream->Tell() == nFlagRecEnd, "Wrong amount of data written" );
	}
	else
	{
		ASSERT( pStream->Tell() <= nFlagRecEnd, "To many data read" );
		if( pStream->Tell() != nFlagRecEnd )
			pStream->Seek( nFlagRecEnd );
	}
}
