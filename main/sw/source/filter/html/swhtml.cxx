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


#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>
#include <com/sun/star/document/XDocumentProperties.hpp>
#include <com/sun/star/i18n/ScriptType.hpp>
#include <sfx2/sfx.hrc>
#include <svx/svxids.hrc>
#ifdef DBG_UTIL
#include <stdlib.h>
#endif
#include <hintids.hxx>

#define _SVSTDARR_STRINGS
#include <svl/svstdarr.hxx>
#include <svl/stritem.hxx>
#include <svtools/imap.hxx>
#include <svtools/htmltokn.h>
#include <svtools/htmlkywd.hxx>
#include <svtools/ctrltool.hxx>
#include <unotools/pathoptions.hxx>
#include <vcl/svapp.hxx>
#include <vcl/wrkwin.hxx>
#include <sfx2/fcontnr.hxx>
#include <sfx2/docfile.hxx>

#include <svtools/htmlcfg.hxx>
#include <sfx2/linkmgr.hxx>
#include <editeng/kernitem.hxx>
#include <editeng/boxitem.hxx>
#include <editeng/fhgtitem.hxx>
#include <editeng/brkitem.hxx>
#include <editeng/postitem.hxx>
#include <editeng/wghtitem.hxx>
#include <editeng/crsditem.hxx>
#include <editeng/udlnitem.hxx>
#include <editeng/escpitem.hxx>
#include <editeng/blnkitem.hxx>
#include <editeng/ulspitem.hxx>
#include <editeng/colritem.hxx>
#include <editeng/fontitem.hxx>
#include <editeng/adjitem.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/protitem.hxx>
#include <editeng/flstitem.hxx>


#include <frmatr.hxx>
#include <charatr.hxx>
#include <fmtfld.hxx>
#include <fmtpdsc.hxx>
#include <txtfld.hxx>
#include <fmtanchr.hxx>
#include <fmtsrnd.hxx>
#include <fmtfsize.hxx>
#include <fmtclds.hxx>
#include <fchrfmt.hxx>
#include <fmtinfmt.hxx>
#include <docary.hxx>
#include <docstat.hxx>
#include <doc.hxx>
#include <IDocumentUndoRedo.hxx>
#include <pam.hxx>
#include <ndtxt.hxx>
#include <mdiexp.hxx>           // ...Percent()
#include <expfld.hxx>
#include <poolfmt.hxx>
#include <pagedesc.hxx>
#include <IMark.hxx>		// for SwBookmark ...
#include <docsh.hxx>
#include <editsh.hxx>		// for Start/EndAction
#include <docufld.hxx>
#include <swcss1.hxx>
#include <htmlvsh.hxx>
#include <fltini.hxx>
#include <htmltbl.hxx>
#include <htmlnum.hxx>
#include <swhtml.hxx>
#include <linkenum.hxx>
#include <breakit.hxx>
#include <SwAppletImpl.hxx>

#include <sfx2/viewfrm.hxx>

#include <statstr.hrc>          // ResId for Statusbar
#include <swerror.h>

#define FONTSIZE_MASK 			7
#define FONTCOLOR_MASK			(1<<15)
#define FONT_MASK				(1<<14)

#define HTML_ESC_PROP 80
#define HTML_ESC_SUPER DFLT_ESC_SUPER
#define HTML_ESC_SUB DFLT_ESC_SUB

#define HTML_SPTYPE_NONE 0
#define HTML_SPTYPE_BLOCK 1
#define HTML_SPTYPE_HORI 2
#define HTML_SPTYPE_VERT 3

#ifndef TOOLS_CONSTASCII_STRINGPARAM
#define TOOLS_CONSTASCII_STRINGPARAM( constAsciiStr ) constAsciiStr, sizeof( constAsciiStr )-1
#endif

using namespace ::com::sun::star;

// <P ALIGN=xxx>, <Hn ALIGN=xxx>, <TD ALIGN=xxx> usw.
HTMLOptionEnum __FAR_DATA aHTMLPAlignTable[] =
{
	{ OOO_STRING_SVTOOLS_HTML_AL_left,	SVX_ADJUST_LEFT		},
	{ OOO_STRING_SVTOOLS_HTML_AL_center,	SVX_ADJUST_CENTER	},
	{ OOO_STRING_SVTOOLS_HTML_AL_middle,	SVX_ADJUST_CENTER	}, // Netscape
	{ OOO_STRING_SVTOOLS_HTML_AL_right,	SVX_ADJUST_RIGHT	},
	{ OOO_STRING_SVTOOLS_HTML_AL_justify,	SVX_ADJUST_BLOCK	},
	{ OOO_STRING_SVTOOLS_HTML_AL_char,	SVX_ADJUST_LEFT		},
	{ 0,				0					}
};

// <SPACER TYPE=...>
static HTMLOptionEnum __FAR_DATA aHTMLSpacerTypeTable[] =
{
	{ OOO_STRING_SVTOOLS_HTML_SPTYPE_block,		HTML_SPTYPE_BLOCK		},
	{ OOO_STRING_SVTOOLS_HTML_SPTYPE_horizontal,	HTML_SPTYPE_HORI		},
	{ OOO_STRING_SVTOOLS_HTML_SPTYPE_vertical,	HTML_SPTYPE_VERT		},
	{ 0,					0						}
};

SV_IMPL_PTRARR( _HTMLAttrs, _HTMLAttrPtr )

HTMLReader::HTMLReader()
{
	bTmplBrowseMode = sal_True;
}

String HTMLReader::GetTemplateName() const
{
	String sTemplate(
			String::CreateFromAscii(TOOLS_CONSTASCII_STRINGPARAM("internal")) );
	sTemplate += INET_PATH_TOKEN;
	sTemplate.AppendAscii( TOOLS_CONSTASCII_STRINGPARAM("html") );
	String sTemplateWithoutExt( sTemplate );
#ifndef MAC_WITHOUT_EXT
    // --> OD 2005-01-26 - first search for OpenDocument Writer/Web template
    sTemplate.AppendAscii( TOOLS_CONSTASCII_STRINGPARAM(".oth") );
    // <--
#endif

	SvtPathOptions aPathOpt;
    // OpenDocument Writer/Web template (extension .oth)
	sal_Bool bSet = aPathOpt.SearchFile( sTemplate, SvtPathOptions::PATH_TEMPLATE );

#ifndef MAC_WITHOUT_EXT
	if( !bSet )
	{
        // 6.0 (extension .stw)
		sTemplate = sTemplateWithoutExt;
        // --> OD 2005-01-26 - no OpenDocument Writer/Web template found.
        // search for OpenOffice.org Writer/Web template
        sTemplate.AppendAscii( TOOLS_CONSTASCII_STRINGPARAM(".stw") );
        // <--
		bSet = aPathOpt.SearchFile( sTemplate, SvtPathOptions::PATH_TEMPLATE );
	}
#endif

	if( !bSet )
	{
		sTemplate.Erase();
		ASSERT( sal_False,
			"The html.vor is no longer in the defined directory!");
	}

	return sTemplate;
}

int HTMLReader::SetStrmStgPtr()
{
	ASSERT( pMedium, "Where is the medium??" );

	if( pMedium->IsRemote() || !pMedium->IsStorage() )
	{
		pStrm = pMedium->GetInStream();
		return sal_True;
	}
	return sal_False;

}

	// Call of generic Read Interface
sal_uLong HTMLReader::Read( SwDoc &rDoc, const String& rBaseURL, SwPaM &rPam, const String & rName )
{
	if( !pStrm )
	{
		ASSERT( pStrm, "HTML-Read without Stream" );
		return ERR_SWG_READ_ERROR;
	}

	if( !bInsertMode )
	{
		Reader::SetNoOutlineNum( rDoc );
		Reader::ResetFrmFmts( rDoc );

		// set HTML-template, if the document is not html,
		// else it is set.
		if( !rDoc.get(IDocumentSettingAccess::HTML_MODE) )
		{
            rDoc.InsertPoolItem( rPam, SwFmtPageDesc(
                rDoc.GetPageDescFromPool( RES_POOLPAGE_HTML, false )), 0 );
		}
	}

	// lock to prevent stealing of the Doc!
	rDoc.acquire();
	sal_uLong nRet = 0;
	SvParserRef xParser = new SwHTMLParser( &rDoc, rPam, *pStrm,
                                            rName, rBaseURL, !bInsertMode, pMedium,
											IsReadUTF8(),
                                            bIgnoreHTMLComments );

	SvParserState eState = xParser->CallParser();

	if( SVPAR_PENDING == eState )
		pStrm->ResetError();
	else if( SVPAR_ACCEPTED != eState )
	{
		String sErr( String::CreateFromInt32((sal_Int32)xParser->GetLineNr()));
		sErr += ',';
		sErr += String::CreateFromInt32((sal_Int32)xParser->GetLinePos());

		// use Stream as transporter error nummber
		nRet = *new StringErrorInfo( ERR_FORMAT_ROWCOL, sErr,
									ERRCODE_BUTTON_OK | ERRCODE_MSG_ERROR );
	}


	return nRet;
}


/*  */

SwHTMLParser::SwHTMLParser( SwDoc* pD, const SwPaM& rCrsr, SvStream& rIn,
                            const String& rPath,
                            const String& rBaseURL,
                            int bReadNewDoc,
							SfxMedium* pMed, sal_Bool bReadUTF8,
                            sal_Bool bNoHTMLComments )
    : SfxHTMLParser( rIn, static_cast< sal_Bool >(bReadNewDoc), pMed ),
	SwClient( 0 ),
	aPathToFile( rPath ),
	sBaseURL( rBaseURL ),
	pAppletImpl( 0 ),
	pCSS1Parser( 0 ),
	pNumRuleInfo( new SwHTMLNumRuleInfo ),
	pPendStack( 0 ),
	pDoc( pD ),
	pActionViewShell( 0 ),
	pSttNdIdx( 0 ),
	pTable(0),
	pFormImpl( 0 ),
	pMarquee( 0 ),
	pField( 0 ),
	pImageMap( 0 ),
	pImageMaps( 0 ),
	pFootEndNoteImpl( 0 ),
	nScriptStartLineNr( 0 ),
	nBaseFontStMin( 0 ),
	nFontStMin( 0 ),
	nDefListDeep( 0 ),
	nFontStHeadStart( 0 ),
	nSBModuleCnt( 0 ),
	nMissingImgMaps( 0 ),
	nParaCnt( 5 ),
    // --> OD 2007-10-26 #i83625#
    nContextStMin( 0 ),
    nContextStAttrMin( 0 ),
    // <--
	nOpenParaToken( 0 ),
	eJumpTo( JUMPTO_NONE ),
#ifdef DBG_UTIL
	nContinue( 0 ),
#endif
	eParaAdjust( SVX_ADJUST_END ),
	bDocInitialized( sal_False ),
	bSetModEnabled( sal_False ),
	bInFloatingFrame( sal_False ),
	bInField( sal_False ),
	bCallNextToken( sal_False ),
	bIgnoreRawData( sal_False ),
	bNoParSpace( sal_False ),
	bInNoEmbed( sal_False ),
	bInTitle( sal_False ),
	bUpdateDocStat( sal_False ),
	bFixSelectWidth( sal_False ),
	bFixSelectHeight( sal_False ),
	bTextArea( sal_False ),
	bSelect( sal_False ),
	bInFootEndNoteAnchor( sal_False ),
	bInFootEndNoteSymbol( sal_False ),
//    bIgnoreHTMLComments( bNoHTMLComments )
	bIgnoreHTMLComments( bNoHTMLComments ),
    bRemoveHidden( sal_False ),
    pTempViewFrame(0)
{
	nEventId = 0;
	bUpperSpace = bViewCreated = bChkJumpMark =
	bSetCrsr = sal_False;

	eScriptLang = HTML_SL_UNKNOWN;
	bAnyStarBasic = sal_True;

	pPam = new SwPaM( *rCrsr.GetPoint() );
	memset( &aAttrTab, 0, sizeof( _HTMLAttrTable ));

	// read Font-size 1-7 from INI-file 
	SvxHtmlOptions* pHtmlOptions = SvxHtmlOptions::Get();
	aFontHeights[0] = pHtmlOptions->GetFontSize( 0 ) * 20;
	aFontHeights[1] = pHtmlOptions->GetFontSize( 1 ) * 20;
	aFontHeights[2] = pHtmlOptions->GetFontSize( 2 ) * 20;
	aFontHeights[3] = pHtmlOptions->GetFontSize( 3 ) * 20;
	aFontHeights[4] = pHtmlOptions->GetFontSize( 4 ) * 20;
	aFontHeights[5] = pHtmlOptions->GetFontSize( 5 ) * 20;
	aFontHeights[6] = pHtmlOptions->GetFontSize( 6 ) * 20;

	bKeepUnknown = pHtmlOptions->IsImportUnknown();

    if(bReadNewDoc)
    {
        SvxFontHeightItem aFontHeight(aFontHeights[2], 100, RES_CHRATR_FONTSIZE);
        pDoc->SetDefault( aFontHeight );
        aFontHeight.SetWhich( RES_CHRATR_CJK_FONTSIZE );
        pDoc->SetDefault( aFontHeight );
        aFontHeight.SetWhich( RES_CHRATR_CTL_FONTSIZE );
        pDoc->SetDefault( aFontHeight );
    }

	// during import switch to HTML-Mode, in order
	// to set correct templates
	bOldIsHTMLMode = pDoc->get(IDocumentSettingAccess::HTML_MODE);
	pDoc->set(IDocumentSettingAccess::HTML_MODE, true);

    pCSS1Parser = new SwCSS1Parser( pDoc, aFontHeights, sBaseURL, IsNewDoc() );
	pCSS1Parser->SetIgnoreFontFamily( pHtmlOptions->IsIgnoreFontFamily() );

	if( bReadUTF8 )
	{
		SetSrcEncoding( RTL_TEXTENCODING_UTF8 );
	}
	else
	{
		SwDocShell *pDocSh = pDoc->GetDocShell();
		SvKeyValueIterator *pHeaderAttrs =
			pDocSh->GetHeaderAttributes();
		if( pHeaderAttrs )
			SetEncodingByHTTPHeader( pHeaderAttrs );
	}
	pCSS1Parser->SetDfltEncoding( gsl_getSystemTextEncoding() );

	// use Timer only with normal Documents!
	SwDocShell* pDocSh = pDoc->GetDocShell();
	if( pDocSh )
	{
		bViewCreated = sal_True;			// do not load synchron

		// a jumppoint is set.

		if( pMed )
		{
			sJmpMark = pMed->GetURLObject().GetMark();
			if( sJmpMark.Len() )
			{
				eJumpTo = JUMPTO_MARK;
				String sCmp;
				xub_StrLen nLastPos, nPos = 0;
				while( STRING_NOTFOUND != ( nLastPos =
						sJmpMark.Search( cMarkSeperator, nPos + 1 )) )
					nPos = nLastPos;

				if( nPos && ( sCmp = sJmpMark.Copy( nPos + 1 ) ).
												EraseAllChars().Len() )
				{
					sCmp.ToLowerAscii();
					if( sCmp.EqualsAscii( pMarkToRegion ) )
						eJumpTo = JUMPTO_REGION;
					else if( sCmp.EqualsAscii( pMarkToTable ) )
						eJumpTo = JUMPTO_TABLE;
					else if( sCmp.EqualsAscii( pMarkToGraphic ) )
						eJumpTo = JUMPTO_GRAPHIC;
					else if( sCmp.EqualsAscii( pMarkToOutline ) ||
							sCmp.EqualsAscii( pMarkToText ) ||
							sCmp.EqualsAscii( pMarkToFrame ) )
						eJumpTo = JUMPTO_NONE;	// this is not valid!
					else
						// else it is h normal (Book)Mark
						nPos = STRING_LEN;
				}
				else
					nPos = STRING_LEN;

				sJmpMark.Erase( nPos );
				if( !sJmpMark.Len() )
					eJumpTo = JUMPTO_NONE;
			}
		}
	}
}

__EXPORT SwHTMLParser::~SwHTMLParser()
{
#ifdef DBG_UTIL
	ASSERT( !nContinue, "DTOR in Continue - This will not work!!!" );
#endif
	sal_Bool bAsync = pDoc->IsInLoadAsynchron();
	pDoc->SetInLoadAsynchron( sal_False );
	pDoc->set(IDocumentSettingAccess::HTML_MODE, bOldIsHTMLMode);

	if( pDoc->GetDocShell() && nEventId )
		Application::RemoveUserEvent( nEventId );

	// DocumentDetected eventual deletes DocShells,
	// so requst it again
	if( pDoc->GetDocShell() )
	{
		// update linked areas
        sal_uInt16 nLinkMode = pDoc->getLinkUpdateMode( true );
		if( nLinkMode != NEVER && bAsync &&
			SFX_CREATE_MODE_INTERNAL!=pDoc->GetDocShell()->GetCreateMode() )
			pDoc->GetLinkManager().UpdateAllLinks( nLinkMode == MANUAL,
												   sal_True, sal_False );

        if ( pDoc->GetDocShell()->IsLoading() )
        {
            // --> OD 2006-11-07 #i59688#
            pDoc->GetDocShell()->LoadingFinished();
        }
	}

	delete pSttNdIdx;

	if( aSetAttrTab.Count() )
	{
		ASSERT( !aSetAttrTab.Count(),"There are still attributes on the stack" );
		aSetAttrTab.DeleteAndDestroy( 0, aSetAttrTab.Count() );
	}

	delete pPam;
	delete pCSS1Parser;
	delete pNumRuleInfo;
	DeleteFormImpl();
	DeleteFootEndNoteImpl();

	ASSERT( !pTable, "There is still one open table" );
	delete pImageMaps;
	//delete pTable;

	ASSERT( !pPendStack,
			"SwHTMLParser::~SwHTMLParser: There should be no more pending stack here" );
	while( pPendStack )
	{
		SwPendingStack* pTmp = pPendStack;
		pPendStack = pPendStack->pNext;
		delete pTmp->pData;
		delete pTmp;
	}

	if( !pDoc->release() )
	{
		// no more use for Doc, delete it
		delete pDoc;
        pDoc = NULL;
	}

	if ( pTempViewFrame )
    {
		pTempViewFrame->DoClose();

        // the temporary view frame is hidden, so the hidden flag might need to be removed
        if ( bRemoveHidden && pDoc && pDoc->GetDocShell() && pDoc->GetDocShell()->GetMedium() )
            pDoc->GetDocShell()->GetMedium()->GetItemSet()->ClearItem( SID_HIDDEN );
    }
}

IMPL_LINK( SwHTMLParser, AsyncCallback, void*, /*pVoid*/ )
{
	nEventId=0;

    // --> FME 2005-08-18 #i47907# If the document has already been destructed,
    // the parser should be aware of this:
    if( ( pDoc->GetDocShell() && pDoc->GetDocShell()->IsAbortingImport() )
        || 1 == pDoc->getReferenceCount() )
    {
        // Import of SFX aborted?
        eState = SVPAR_ERROR;
    }
    // <--

	GetAsynchCallLink().Call(0);
	return 0;
}

SvParserState __EXPORT SwHTMLParser::CallParser()
{
	// set tempory Index, on Pos 0 in order avoid moving!
	pSttNdIdx = new SwNodeIndex( pDoc->GetNodes() );
	if( !IsNewDoc() )		// insert to a new document ?
	{
		const SwPosition* pPos = pPam->GetPoint();

		pDoc->SplitNode( *pPos, false );

		*pSttNdIdx = pPos->nNode.GetIndex()-1;
		pDoc->SplitNode( *pPos, false );

		SwPaM aInsertionRangePam( *pPos );

		pPam->Move( fnMoveBackward );

		// #106634# split any redline over the insertion point
		aInsertionRangePam.SetMark();
		*aInsertionRangePam.GetPoint() = *pPam->GetPoint();
		aInsertionRangePam.Move( fnMoveBackward );
		pDoc->SplitRedline( aInsertionRangePam );

		pDoc->SetTxtFmtColl( *pPam,
				pCSS1Parser->GetTxtCollFromPool( RES_POOLCOLL_STANDARD ));
	}

	if( GetMedium() )
	{
		if( !bViewCreated )
		{
            nEventId = Application::PostUserEvent( LINK( this, SwHTMLParser, AsyncCallback ), 0 );
		}
		else
		{
			bViewCreated = sal_True;
			nEventId = 0;
		}
	}

	// show progress bar
	else if( !GetMedium() || !GetMedium()->IsRemote() )
	{
		rInput.Seek(STREAM_SEEK_TO_END);
		rInput.ResetError();
		::StartProgress( STR_STATSTR_W4WREAD, 0, rInput.Tell(),
						 pDoc->GetDocShell() );
		rInput.Seek(STREAM_SEEK_TO_BEGIN);
		rInput.ResetError();
	}

	SwPageDesc& rDesc = pDoc->_GetPageDesc( 0 );
	rDesc.Add( this );

	SvParserState eRet = HTMLParser::CallParser();
	return eRet;
}

void __EXPORT SwHTMLParser::Continue( int nToken )
{
#ifdef DBG_UTIL
	ASSERT( !nContinue, "Continue im Continue - That should not be, shouldn't it?" );
	nContinue++;
#endif

	// if Import (vom SFX) has been aborted, an error is set
	// in order to clean up, the process continues
	ASSERT( SVPAR_ERROR!=eState,
			"SwHTMLParser::Continue: already set an error" );
	if( pDoc->GetDocShell() && pDoc->GetDocShell()->IsAbortingImport() )
		eState = SVPAR_ERROR;

	// get ViewShell from Document, memorize and set as current.
	ViewShell *pInitVSh = CallStartAction();

	if( SVPAR_ERROR != eState && GetMedium() && !bViewCreated )
	{
		// returnen at first call, show Doc
		// and wait for Timer Callback.
		// At this point CallParser read only one character 
		//and a SaveState(0) called.
		eState = SVPAR_PENDING;
		bViewCreated = sal_True;
		pDoc->SetInLoadAsynchron( sal_True );

#ifdef DBG_UTIL
		nContinue--;
#endif

		return;
	}

	bSetModEnabled = sal_False;
	if( pDoc->GetDocShell() &&
		0 != (bSetModEnabled = pDoc->GetDocShell()->IsEnableSetModified()) )
	{
		pDoc->GetDocShell()->EnableSetModified( sal_False );
	}

	// during read. do not call OLE-Modify 
	Link aOLELink( pDoc->GetOle2Link() );
	pDoc->SetOle2Link( Link() );

	sal_Bool bModified = pDoc->IsModified();
    bool const bWasUndo = pDoc->GetIDocumentUndoRedo().DoesUndo();
    pDoc->GetIDocumentUndoRedo().DoUndo(false);

	// If Import aborts, do not call Continue.
	// if a Pending-Stack exists, call NextToken.
	// NextToken will take care of clean up.
	if( SVPAR_ERROR == eState )
	{
		ASSERT( !pPendStack || pPendStack->nToken,
				"SwHTMLParser::Continue: Pending-Stack without Token" );
		if( pPendStack && pPendStack->nToken )
			NextToken( pPendStack->nToken );
		ASSERT( !pPendStack,
				"SwHTMLParser::Continue: a Pend-Stack exists again" );
	}
	else
	{
		HTMLParser::Continue( pPendStack ? pPendStack->nToken : nToken );
	}

	// switch progress bar off
	EndProgress( pDoc->GetDocShell() );

	sal_Bool bLFStripped = sal_False;
	if( SVPAR_PENDING != GetStatus() )
	{
		// set last Attributes
		{
			if( aScriptSource.Len() )
			{
				SwScriptFieldType *pType =
					(SwScriptFieldType*)pDoc->GetSysFldType( RES_SCRIPTFLD );

				SwScriptField aFld( pType, aScriptType, aScriptSource,
									sal_False );
				InsertAttr( SwFmtFld( aFld ) );
			}

			if( pAppletImpl )
			{
                if( pAppletImpl->GetApplet().is() )
					EndApplet();
				else
					EndObject();
			}

			// remove LF behind last paragaf if exists
			if( IsNewDoc() )
				bLFStripped = StripTrailingLF() > 0;

			// end open nummerations.
			while( GetNumInfo().GetNumRule() )
				EndNumBulList();

			ASSERT( !nContextStMin, "There are protected contexts" );
			nContextStMin = 0;
			while( aContexts.Count() )
			{
				_HTMLAttrContext *pCntxt = PopContext();
				if( pCntxt )
				{
					EndContext( pCntxt );
					delete pCntxt;
				}
			}

			if( aParaAttrs.Count() )
				aParaAttrs.Remove( 0, aParaAttrs.Count() );

			SetAttr( sal_False );

			// set deferred Styles
			pCSS1Parser->SetDelayedStyles();
		}

		// fix Start
		if( !IsNewDoc() && pSttNdIdx->GetIndex() )
		{
			SwTxtNode* pTxtNode = pSttNdIdx->GetNode().GetTxtNode();
			SwNodeIndex aNxtIdx( *pSttNdIdx );
			if( pTxtNode && pTxtNode->CanJoinNext( &aNxtIdx ))
			{
				xub_StrLen nStt = pTxtNode->GetTxt().Len();
				// if Cursor is in Node, set it to the end
				if( pPam->GetPoint()->nNode == aNxtIdx )
				{
					pPam->GetPoint()->nNode = *pSttNdIdx;
					pPam->GetPoint()->nContent.Assign( pTxtNode, nStt );
				}

#ifdef DBG_UTIL
// !!! should not be possible, should it ??
ASSERT( pSttNdIdx->GetIndex()+1 != pPam->GetBound( sal_True ).nNode.GetIndex(),
			"Pam.Bound1 is in Node" );
ASSERT( pSttNdIdx->GetIndex()+1 != pPam->GetBound( sal_False ).nNode.GetIndex(),
			"Pam.Bound2 is in Node" );

if( pSttNdIdx->GetIndex()+1 == pPam->GetBound( sal_True ).nNode.GetIndex() )
{
	xub_StrLen nCntPos = pPam->GetBound( sal_True ).nContent.GetIndex();
	pPam->GetBound( sal_True ).nContent.Assign( pTxtNode,
					pTxtNode->GetTxt().Len() + nCntPos );
}
if( pSttNdIdx->GetIndex()+1 == pPam->GetBound( sal_False ).nNode.GetIndex() )
{
	xub_StrLen nCntPos = pPam->GetBound( sal_False ).nContent.GetIndex();
	pPam->GetBound( sal_False ).nContent.Assign( pTxtNode,
					pTxtNode->GetTxt().Len() + nCntPos );
}
#endif
				// keep character Attribute!
				SwTxtNode* pDelNd = aNxtIdx.GetNode().GetTxtNode();
				if( pTxtNode->GetTxt().Len() )
					pDelNd->FmtToTxtAttr( pTxtNode );
				else
					pTxtNode->ChgFmtColl( pDelNd->GetTxtColl() );
				pTxtNode->JoinNext();
			}
		}
	}

	if( SVPAR_ACCEPTED == eState )
	{
		if( nMissingImgMaps )
		{
			// some image map assigns are missing.
			// maybe Image-Maps do exist now?
			ConnectImageMaps();
		}

		// now delete the last needless paragraph
		SwPosition* pPos = pPam->GetPoint();
		if( !pPos->nContent.GetIndex() && !bLFStripped )
		{
			SwTxtNode* pAktNd;
			sal_uLong nNodeIdx = pPos->nNode.GetIndex();

			sal_Bool bHasFlysOrMarks =
				HasCurrentParaFlys() || HasCurrentParaBookmarks( sal_True );

			if( IsNewDoc() )
			{
				const SwNode *pPrev = pDoc->GetNodes()[nNodeIdx -1];
				if( !pPam->GetPoint()->nContent.GetIndex() &&
					( pPrev->IsCntntNode() ||
					  (pPrev->IsEndNode() &&
					  pPrev->StartOfSectionNode()->IsSectionNode()) ) )
				{
					SwCntntNode* pCNd = pPam->GetCntntNode();
					if( pCNd && pCNd->StartOfSectionIndex()+2 <
						pCNd->EndOfSectionIndex() && !bHasFlysOrMarks )
					{
						ViewShell *pVSh = CheckActionViewShell();
						SwCrsrShell *pCrsrSh = pVSh && pVSh->ISA(SwCrsrShell)
										? static_cast < SwCrsrShell * >( pVSh )
										: 0;
						if( pCrsrSh &&
							pCrsrSh->GetCrsr()->GetPoint()
								   ->nNode.GetIndex() == nNodeIdx )
						{
							pCrsrSh->MovePara(fnParaPrev, fnParaEnd );
							pCrsrSh->SetMark();
							pCrsrSh->ClearMark();
						}
						pPam->GetBound(sal_True).nContent.Assign( 0, 0 );
						pPam->GetBound(sal_False).nContent.Assign( 0, 0 );
						pDoc->GetNodes().Delete( pPam->GetPoint()->nNode );
					}
				}
			}
			else if( 0 != ( pAktNd = pDoc->GetNodes()[ nNodeIdx ]->GetTxtNode()) && !bHasFlysOrMarks )
			{
				if( pAktNd->CanJoinNext( &pPos->nNode ))
				{
					SwTxtNode* pNextNd = pPos->nNode.GetNode().GetTxtNode();
					pPos->nContent.Assign( pNextNd, 0 );
					pPam->SetMark(); pPam->DeleteMark();
					pNextNd->JoinPrev();
				}
				else if( !pAktNd->GetTxt().Len() )
				{
					pPos->nContent.Assign( 0, 0 );
					pPam->SetMark(); pPam->DeleteMark();
					pDoc->GetNodes().Delete( pPos->nNode, 1 );
					pPam->Move( fnMoveBackward );
				}
			}
		}

		// now cancel the SplitNode from the start
		else if( !IsNewDoc() )
		{
			if( pPos->nContent.GetIndex() ) 	// then there was no <P> at the end
				pPam->Move( fnMoveForward, fnGoNode ); 	// to the next node
			SwTxtNode* pTxtNode = pPos->nNode.GetNode().GetTxtNode();
			SwNodeIndex aPrvIdx( pPos->nNode );
			if( pTxtNode && pTxtNode->CanJoinPrev( &aPrvIdx ) &&
				*pSttNdIdx <= aPrvIdx )
			{
				// actually a JoinNext must be done here, but all cursors
				// etc. are registered in the pTxtNode, 
				// which MUST remain


				// Convert paragraph to character attributes, 
				// take over the paragraph attributes and the template from the Prev!
				SwTxtNode* pPrev = aPrvIdx.GetNode().GetTxtNode();
				pTxtNode->ChgFmtColl( pPrev->GetTxtColl() );
				pTxtNode->FmtToTxtAttr( pPrev );
                pTxtNode->ResetAllAttr();

                if( pPrev->HasSwAttrSet() )
                    pTxtNode->SetAttr( *pPrev->GetpSwAttrSet() );

				if( &pPam->GetBound(sal_True).nNode.GetNode() == pPrev )
					pPam->GetBound(sal_True).nContent.Assign( pTxtNode, 0 );
				if( &pPam->GetBound(sal_False).nNode.GetNode() == pPrev )
					pPam->GetBound(sal_False).nContent.Assign( pTxtNode, 0 );

				pTxtNode->JoinPrev();
			}
		}

		// and prepare DocumentInfo
		if( IsNewDoc() )
		{
            SwDocShell *pDocShell(pDoc->GetDocShell());
            DBG_ASSERT(pDocShell, "no SwDocShell");
            if (pDocShell) {
                uno::Reference<document::XDocumentPropertiesSupplier> xDPS(
                    pDocShell->GetModel(), uno::UNO_QUERY_THROW);
                uno::Reference<document::XDocumentProperties> xDocProps(
                    xDPS->getDocumentProperties());
                DBG_ASSERT(xDocProps.is(), "DocumentProperties is null");
                if ( xDocProps.is() && (xDocProps->getAutoloadSecs() > 0) &&
                     xDocProps->getAutoloadURL().equalsAscii("") )
                {
                    xDocProps->setAutoloadURL(aPathToFile);
                }
            }
		}

		if( bUpdateDocStat )
		{
			SwDocStat aStat( pDoc->GetDocStat() );
			pDoc->UpdateDocStat( aStat );
		}
	}

	if( SVPAR_PENDING != GetStatus() )
		delete pSttNdIdx, pSttNdIdx = 0;

	// should the parser be the last one to hold the doc, 
	// no need to do anything here either, doc is about to be destroyed!
	if( 1 < pDoc->getReferenceCount() )
	{
		if( bWasUndo )
        {
            pDoc->GetIDocumentUndoRedo().DelAllUndoObj();
            pDoc->GetIDocumentUndoRedo().DoUndo(true);
        }
		else if( !pInitVSh )
		{
			// If there was no shell at the beginning of the Continue,
			// nevertheless one may have been created in the meantime.
			// In this case the bWasUndo flag is not correct and 
			// a switch on the undo is necessary.
			ViewShell *pTmpVSh = CheckActionViewShell();
			if( pTmpVSh )
            {
                pDoc->GetIDocumentUndoRedo().DoUndo(true);
            }
        }

		pDoc->SetOle2Link( aOLELink );
		if( !bModified )
			pDoc->ResetModified();
		if( bSetModEnabled && pDoc->GetDocShell() )
		{
			pDoc->GetDocShell()->EnableSetModified( sal_True );
			bSetModEnabled = sal_False; // this is unnecessary here
		}
	}


	// If the Dokuemnt-ViewShell still exists and an Action
	// is open (doesn't have to be on abort), terminate the action,
	// log out of the shell and finally reconstruct the old shell
	// again.

	CallEndAction( sal_True );

#ifdef DBG_UTIL
	nContinue--;
#endif
}

void SwHTMLParser::Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew )
{
	switch( pOld ? pOld->Which() : pNew ? pNew->Which() : 0 )
	{
	case RES_OBJECTDYING:
		if( ((SwPtrMsgPoolItem *)pOld)->pObject == GetRegisteredIn() )
		{
			// then finish ourselves
			GetRegisteredInNonConst()->Remove( this );
			ReleaseRef();					// otherwise we are done!
		}
		break;
	}
}

void SwHTMLParser::DocumentDetected()
{
	ASSERT( !bDocInitialized, "DocumentDetected called multiple times" );
	bDocInitialized = sal_True;
	if( IsNewDoc() )
	{
		if( IsInHeader() )
			FinishHeader( sal_True );

		CallEndAction( sal_True, sal_True );

        pDoc->GetIDocumentUndoRedo().DoUndo(false);
		// By DocumentDetected a ViewShell has been created. 
		// But it can also be that it is created later,
		// namely when the UI is is captured.
		CallStartAction();
	}
}

// is called for each token that is recognized in CallParser
void __EXPORT SwHTMLParser::NextToken( int nToken )
{
	if( ( pDoc->GetDocShell() && pDoc->GetDocShell()->IsAbortingImport() )
		|| 1 == pDoc->getReferenceCount() )
	{
		// was the import aborted by the SFX? If a pending stack
		// still exists to clean it up
		eState = SVPAR_ERROR;
		ASSERT( !pPendStack || pPendStack->nToken,
				"SwHTMLParser::NextToken: pending stack without token" );
		if( 1 == pDoc->getReferenceCount() || !pPendStack )
			return ;
	}

#ifdef DBG_UTIL
	if( pPendStack )
	{
		switch( nToken )
		{
			// tables are read via recusive method calls
		case HTML_TABLE_ON:
			// For CSS declarations you may have to wait for the
			// end of a file download.
		case HTML_LINK:
			// For controls, the size may still have to be set.
		case HTML_INPUT:
		case HTML_TEXTAREA_ON:
		case HTML_SELECT_ON:
		case HTML_SELECT_OFF:
			break;
		default:
			ASSERT( !pPendStack, "Unknown token for pending stack" );
			break;
		}
	}
#endif

	// The following special cases have to be handled before the filter detection
	// because the content of the title, etc. is not used for filter even in Netcape.
	if( !pPendStack )
	{
		if( bInTitle )
		{
			switch( nToken )
			{
			case HTML_TITLE_OFF:
				if( IsNewDoc() && sTitle.Len() )
				{
                    if( pDoc->GetDocShell() ) {
                        uno::Reference<document::XDocumentPropertiesSupplier>
                            xDPS(pDoc->GetDocShell()->GetModel(),
                            uno::UNO_QUERY_THROW);
                        uno::Reference<document::XDocumentProperties> xDocProps(
                            xDPS->getDocumentProperties());
                        DBG_ASSERT(xDocProps.is(), "no DocumentProperties");
                        if (xDocProps.is()) {
                            xDocProps->setTitle(sTitle);
                        }

						pDoc->GetDocShell()->SetTitle( sTitle );
                    }
				}
				bInTitle = sal_False;
				sTitle.Erase();
				break;

			case HTML_NONBREAKSPACE:
				sTitle += ' ';
				break;

			case HTML_SOFTHYPH:
				sTitle += '-';
				break;

			case HTML_TEXTTOKEN:
				sTitle += aToken;
				break;

			default:
				sTitle += '<';
				if( (HTML_TOKEN_ONOFF & nToken) && (1 & nToken) )
					sTitle += '/';
				sTitle += sSaveToken;
				if( aToken.Len() )
				{
					sTitle += ' ';
					sTitle += aToken;
				}
				sTitle += '>';
				break;
			}

			return;
		}
	}

	// If we don't know yet, what kind of document we have in front of us,
	// let's try to find out first. This must be done for controls in
	// case before inserting the control, because when inserting the control
	// a view is already needed.
	if( !bDocInitialized )
		DocumentDetected();

	sal_Bool bGetIDOption = sal_False, bInsertUnknown = sal_False;
	sal_Bool bUpperSpaceSave = bUpperSpace;
	bUpperSpace = sal_False;

	// The following special cases must or can be made after the
	// filter detection.
	if( !pPendStack )
	{
		if( bInFloatingFrame )
		{
			// <SCRIPT> is ignored here (by us), because it is also ignored in
			// applets is ignored!
			if( HTML_IFRAME_OFF == nToken )
			{
				bCallNextToken = sal_False;
				EndFloatingFrame();
			}

			return;
		}
		else if( bInNoEmbed )
		{
			switch( nToken )
			{
			case HTML_NOEMBED_OFF:
				aContents.ConvertLineEnd();
				InsertComment( aContents, OOO_STRING_SVTOOLS_HTML_noembed );
				aContents.Erase();
				bCallNextToken = sal_False;
				bInNoEmbed = sal_False;
				break;

			case HTML_RAWDATA:
				InsertCommentText( OOO_STRING_SVTOOLS_HTML_noembed );
				break;

			default:
				ASSERT( sal_False, "SwHTMLParser::NextToken: invalid day" );
				break;
			}

			return;
		}
		else if( pAppletImpl )
		{
			// in an applet we are (for now) only interested in <PARAM> tags.
			// and the </APPLET>.
			// <SCRIPT> is ignored here (by Netscape)!

			switch( nToken )
			{
			case HTML_APPLET_OFF:
				bCallNextToken = sal_False;
				EndApplet();
				break;
			case HTML_OBJECT_OFF:
				bCallNextToken = sal_False;
				EndObject();
				break;

			case HTML_PARAM:
				InsertParam();
				break;
			}

			return;
		}
		else if( bTextArea )
		{
			// in a TextArea everything up to </TEXTAREA> is inserted as text.
			// <SCRIPT> is ignored here (by Netscape)!

			switch( nToken )
			{
			case HTML_TEXTAREA_OFF:
				bCallNextToken = sal_False;
				EndTextArea();
				break;

			default:
                InsertTextAreaText( static_cast< sal_uInt16 >(nToken) );
				break;
			}

			return;
		}
		else if( bSelect )
		{
			// MUST come after bNoScript!
			switch( nToken )
			{
			case HTML_SELECT_OFF:
				bCallNextToken = sal_False;
				EndSelect();
				return;

			case HTML_OPTION:
				InsertSelectOption();
				return;

			case HTML_TEXTTOKEN:
				InsertSelectText();
				return;

			case HTML_INPUT:
			case HTML_SCRIPT_ON:
			case HTML_SCRIPT_OFF:
			case HTML_NOSCRIPT_ON:
			case HTML_NOSCRIPT_OFF:
			case HTML_RAWDATA:
				// handle in the normal switch
				break;

			default:
				// ignore
				return;
			}
		}
		else if( pMarquee )
		{
			// in a TextArea everything up to </TEXTAREA> is inserted as text.
			// The <SCRIPT> tags will be ignored by MS-IE, by us the
			// whole script
			switch( nToken )
			{
			case HTML_MARQUEE_OFF:
				bCallNextToken = sal_False;
				EndMarquee();
				break;

			case HTML_TEXTTOKEN:
				InsertMarqueeText();
				break;
			}

			return;
		}
		else if( bInField )
		{
			switch( nToken )
			{
			case HTML_SDFIELD_OFF:
				bCallNextToken = sal_False;
				EndField();
				break;

			case HTML_TEXTTOKEN:
				InsertFieldText();
				break;
			}

			return;
		}
		else if( bInFootEndNoteAnchor || bInFootEndNoteSymbol )
		{
			switch( nToken )
			{
			case HTML_ANCHOR_OFF:
				EndAnchor();
				bCallNextToken = sal_False;
				break;

			case HTML_TEXTTOKEN:
				InsertFootEndNoteText();
				break;
			}
			return;
		}
		else if( aUnknownToken.Len() )
		{
			// Unknown tokens in the header are only terminated by a matching
			// end token, </HEAD> or <BODY> again. In this text
			// is ignored.
			switch( nToken )
			{
			case HTML_UNKNOWNCONTROL_OFF:
				if( aUnknownToken.CompareTo(sSaveToken) != COMPARE_EQUAL )
					return;
			case HTML_FRAMESET_ON:
			case HTML_HEAD_OFF:
			case HTML_BODY_ON:
			case HTML_IMAGE:		// For whatever reason Netscape does this.
				aUnknownToken.Erase();
				break;
			case HTML_TEXTTOKEN:
				return;
			default:
				break;
			}
		}
	}

	switch( nToken )
	{
	case HTML_BODY_ON:
		if( aStyleSource.Len() )
		{
			pCSS1Parser->ParseStyleSheet( aStyleSource );
			aStyleSource.Erase();
		}
		if( IsNewDoc() )
		{
			InsertBodyOptions();
			// If there is a template for the first or right side,
			// we set it here.
			const SwPageDesc *pPageDesc = 0;
			if( pCSS1Parser->IsSetFirstPageDesc() )
				pPageDesc = pCSS1Parser->GetFirstPageDesc();
			else if( pCSS1Parser->IsSetRightPageDesc() )
				pPageDesc = pCSS1Parser->GetRightPageDesc();

			if( pPageDesc )
            {
                pDoc->InsertPoolItem( *pPam, SwFmtPageDesc( pPageDesc ), 0 );
            }
		}
		break;

	case HTML_LINK:
		InsertLink();
		break;

	case HTML_BASE:
		{
			const HTMLOptions *pHTMLOptions = GetOptions();
			for( sal_uInt16 i = pHTMLOptions->Count(); i; )
			{
				const HTMLOption *pOption = (*pHTMLOptions)[ --i ];
				switch( pOption->GetToken() )
				{
				case HTML_O_HREF:
					sBaseURL = pOption->GetString();
					break;
				case HTML_O_TARGET:
					if( IsNewDoc() )
					{
                        SwDocShell *pDocShell(pDoc->GetDocShell());
                        DBG_ASSERT(pDocShell, "no SwDocShell");
                        if (pDocShell) {
                            uno::Reference<document::XDocumentPropertiesSupplier> xDPS(
                                pDocShell->GetModel(), uno::UNO_QUERY_THROW);
                            uno::Reference<document::XDocumentProperties>
                                xDocProps(xDPS->getDocumentProperties());
                            DBG_ASSERT(xDocProps.is(),"no DocumentProperties");
                            if (xDocProps.is()) {
                                xDocProps->setDefaultTarget(
                                    pOption->GetString());
                            }
                        }
					}
					break;
				}
			}
		}
		break;

	case HTML_META:
		{
			SvKeyValueIterator *pHTTPHeader = 0;
			if( IsNewDoc() )
			{
				SwDocShell *pDocSh = pDoc->GetDocShell();
				if( pDocSh )
					pHTTPHeader = pDocSh->GetHeaderAttributes();
			}
            SwDocShell *pDocShell(pDoc->GetDocShell());
            DBG_ASSERT(pDocShell, "no SwDocShell");
            if (pDocShell)
            {
                uno::Reference<document::XDocumentProperties> xDocProps;
                if (IsNewDoc())
                {
                    const uno::Reference<document::XDocumentPropertiesSupplier>
                        xDPS( pDocShell->GetModel(), uno::UNO_QUERY_THROW );
                    xDocProps = xDPS->getDocumentProperties();
                    DBG_ASSERT(xDocProps.is(), "DocumentProperties is null");
                }
                ParseMetaOptions( xDocProps, pHTTPHeader );
			}
		}
		break;

	case HTML_TITLE_ON:
		bInTitle = sal_True;
		break;

	case HTML_SCRIPT_ON:
		NewScript();
		break;

	case HTML_SCRIPT_OFF:
		EndScript();
		break;

	case HTML_NOSCRIPT_ON:
	case HTML_NOSCRIPT_OFF:
		bInsertUnknown = sal_True;
		break;

	case HTML_STYLE_ON:
		NewStyle();
		break;

	case HTML_STYLE_OFF:
		EndStyle();
		break;

	case HTML_RAWDATA:
		if( !bIgnoreRawData )
		{
			if( IsReadScript() )
			{
				AddScriptSource();
			}
			else if( IsReadStyle() )
			{
				if( aStyleSource.Len() )
					aStyleSource += '\n';
				aStyleSource += aToken;
			}
		}
		break;

	case HTML_OBJECT_ON:
#ifdef SOLAR_JAVA
		NewObject();
		bCallNextToken = pAppletImpl!=0 && pTable!=0;
#endif
		break;

	case HTML_APPLET_ON:
#ifdef SOLAR_JAVA
		InsertApplet();
		bCallNextToken = pAppletImpl!=0 && pTable!=0;
#endif
		break;

	case HTML_IFRAME_ON:
		InsertFloatingFrame();
		bCallNextToken = bInFloatingFrame && pTable!=0;
		break;

	case HTML_LINEBREAK:
		if( !IsReadPRE() )
		{
			InsertLineBreak();
			break;
		}
		else
			bGetIDOption = sal_True;
			// <BR>s in <PRE> resemble real LFs, therefore no break

	case HTML_NEWPARA:
		// CR in PRE/LISTING/XMP
		{
			if( HTML_NEWPARA==nToken ||
				pPam->GetPoint()->nContent.GetIndex() )
			{
				AppendTxtNode(); // lf does not exist here, therefore uncritical
				SetTxtCollAttrs();
			}
			// progress bar
			if( !GetMedium() || !GetMedium()->IsRemote() )
				::SetProgressState( rInput.Tell(), pDoc->GetDocShell() );
		}
		break;

	case HTML_NONBREAKSPACE:
        pDoc->InsertString( *pPam, CHAR_HARDBLANK );
		break;

	case HTML_SOFTHYPH:
        pDoc->InsertString( *pPam, CHAR_SOFTHYPHEN );
		break;

	case HTML_LINEFEEDCHAR:
		if( pPam->GetPoint()->nContent.GetIndex() )
			AppendTxtNode();
		if( !pTable && !pDoc->IsInHeaderFooter( pPam->GetPoint()->nNode ) )
		{
            NewAttr( &aAttrTab.pBreak, SvxFmtBreakItem(SVX_BREAK_PAGE_BEFORE, RES_BREAK) );
			EndAttr( aAttrTab.pBreak, 0, sal_False );
		}
		break;

	case HTML_TEXTTOKEN:
        // then insert the string without adding any attributes to the end
		if( aToken.Len() && ' '==aToken.GetChar(0) && !IsReadPRE() )
		{
			xub_StrLen nPos = pPam->GetPoint()->nContent.GetIndex();
			if( nPos )
			{
				const String& rText =
                    pPam->GetPoint()->nNode.GetNode().GetTxtNode()->GetTxt();
				sal_Unicode cLast = rText.GetChar(--nPos);
				if( ' ' == cLast || '\x0a' == cLast)
					aToken.Erase(0,1);
			}
			else
				aToken.Erase(0,1);

			if( !aToken.Len() )
			{
				bUpperSpace = bUpperSpaceSave;
				break;
			}
		}

		if( aToken.Len() )
		{
			if( !bDocInitialized )
				DocumentDetected();
            pDoc->InsertString( *pPam, aToken );

            // if there are still preliminary paragraph attributes, but the paragraph
			// is not empty, then the paragraph attributes are invalid.
			if( aParaAttrs.Count() )
				aParaAttrs.Remove( 0, aParaAttrs.Count() );

			SetAttr();
		}
		break;

	case HTML_HORZRULE:
		InsertHorzRule();
		break;

	case HTML_IMAGE:
		InsertImage();
		// If the parser should be the last one to hold the doc, then abort here and set an error.
		if( 1 == pDoc->getReferenceCount() )
		{
			eState = SVPAR_ERROR;
		}
		break;

	case HTML_SPACER:
		InsertSpacer();
		break;

	case HTML_EMBED:
		InsertEmbed();
		break;

	case HTML_NOEMBED_ON:
		bInNoEmbed = sal_True;
		bCallNextToken = pTable!=0;
		ReadRawData( OOO_STRING_SVTOOLS_HTML_noembed );
		break;

	case HTML_DEFLIST_ON:
		if( nOpenParaToken )
			EndPara();
		NewDefList();
		break;
	case HTML_DEFLIST_OFF:
		if( nOpenParaToken )
			EndPara();
		EndDefListItem( 0, sal_False, 1==nDefListDeep );
		EndDefList();
		break;

	case HTML_DD_ON:
	case HTML_DT_ON:
		if( nOpenParaToken )
			EndPara();
		EndDefListItem( 0, sal_False );// <DD>/<DT> quit and set no preliminaries
		NewDefListItem( nToken );
		break;

	case HTML_DD_OFF:
	case HTML_DT_OFF:
		// see HTML_LI_OFF
		// actually a DD/DT should be terminated now. But since both
		// Netscape and Microsoft don't do that, we don't do that either.
		EndDefListItem( nToken, sal_False );
		break;

	// Areas
	case HTML_DIVISION_ON:
	case HTML_CENTER_ON:
		if( nOpenParaToken )
		{
			if( IsReadPRE() )
				nOpenParaToken = 0;
			else
				EndPara();
		}
		NewDivision( nToken );
		break;

	case HTML_DIVISION_OFF:
	case HTML_CENTER_OFF:
		if( nOpenParaToken )
		{
			if( IsReadPRE() )
				nOpenParaToken = 0;
			else
				EndPara();
		}
		EndDivision( nToken );
		break;

	case HTML_MULTICOL_ON:
		if( nOpenParaToken )
			EndPara();
		NewMultiCol();
		break;

	case HTML_MULTICOL_OFF:
		if( nOpenParaToken )
			EndPara();
		EndTag( HTML_MULTICOL_ON );
		break;

	case HTML_MARQUEE_ON:
		NewMarquee();
		bCallNextToken = pMarquee!=0 && pTable!=0;
		break;

	case HTML_FORM_ON:
		NewForm();
		break;
	case HTML_FORM_OFF:
		EndForm();
		break;

	// Templates:
	case HTML_PARABREAK_ON:
		if( nOpenParaToken )
			EndPara( sal_True );
		NewPara();
		break;

	case HTML_PARABREAK_OFF:
		EndPara( sal_True );
		break;

	case HTML_ADDRESS_ON:
		if( nOpenParaToken )
			EndPara();
		NewTxtFmtColl( HTML_ADDRESS_ON, RES_POOLCOLL_SENDADRESS );
		break;

	case HTML_ADDRESS_OFF:
		if( nOpenParaToken )
			EndPara();
		EndTxtFmtColl( HTML_ADDRESS_OFF );
		break;

	case HTML_BLOCKQUOTE_ON:
	case HTML_BLOCKQUOTE30_ON:
		if( nOpenParaToken )
			EndPara();
		NewTxtFmtColl( HTML_BLOCKQUOTE_ON, RES_POOLCOLL_HTML_BLOCKQUOTE );
		break;

	case HTML_BLOCKQUOTE_OFF:
	case HTML_BLOCKQUOTE30_OFF:
		if( nOpenParaToken )
			EndPara();
		EndTxtFmtColl( HTML_BLOCKQUOTE_ON );
		break;

	case HTML_PREFORMTXT_ON:
	case HTML_LISTING_ON:
	case HTML_XMP_ON:
		if( nOpenParaToken )
			EndPara();
		NewTxtFmtColl( nToken, RES_POOLCOLL_HTML_PRE );
		break;

	case HTML_PREFORMTXT_OFF:
		bNoParSpace = sal_True; // the last PRE paragraph must get a line spacing
		EndTxtFmtColl( HTML_PREFORMTXT_OFF );
		break;

	case HTML_LISTING_OFF:
	case HTML_XMP_OFF:
		EndTxtFmtColl( nToken );
		break;

	case HTML_HEAD1_ON:
	case HTML_HEAD2_ON:
	case HTML_HEAD3_ON:
	case HTML_HEAD4_ON:
	case HTML_HEAD5_ON:
	case HTML_HEAD6_ON:
		if( nOpenParaToken )
		{
			if( IsReadPRE() )
				nOpenParaToken = 0;
			else
				EndPara();
		}
		NewHeading( nToken );
		break;

	case HTML_HEAD1_OFF:
	case HTML_HEAD2_OFF:
	case HTML_HEAD3_OFF:
	case HTML_HEAD4_OFF:
	case HTML_HEAD5_OFF:
	case HTML_HEAD6_OFF:
		EndHeading();
		break;

	case HTML_TABLE_ON:
		if( pPendStack )
			BuildTable( SVX_ADJUST_END );
		else
		{
			if( nOpenParaToken )
				EndPara();
			ASSERT( !pTable, "Table in table must not occur here" );
			if( !pTable && (IsNewDoc() || !pPam->GetNode()->FindTableNode()) &&
				(pPam->GetPoint()->nNode.GetIndex() >
							pDoc->GetNodes().GetEndOfExtras().GetIndex() ||
				!pPam->GetNode()->FindFootnoteStartNode() ) )
			{
				if ( nParaCnt < 5 )
					Show();		// show up to this point

				SvxAdjust eAdjust = aAttrTab.pAdjust
					? ((const SvxAdjustItem&)aAttrTab.pAdjust->GetItem()).
											 GetAdjust()
					: SVX_ADJUST_END;
				BuildTable( eAdjust );
			}
			else
				bInsertUnknown = bKeepUnknown;
		}
		break;

	// Lists
	case HTML_DIRLIST_ON:
	case HTML_MENULIST_ON:
	case HTML_ORDERLIST_ON:
	case HTML_UNORDERLIST_ON:
		if( nOpenParaToken )
			EndPara();
		NewNumBulList( nToken );
		break;

	case HTML_DIRLIST_OFF:
	case HTML_MENULIST_OFF:
	case HTML_ORDERLIST_OFF:
	case HTML_UNORDERLIST_OFF:
		if( nOpenParaToken )
			EndPara();
		EndNumBulListItem( 0, sal_True, GetNumInfo().GetDepth()==1 );
		EndNumBulList( nToken );
		break;

	case HTML_LI_ON:
	case HTML_LISTHEADER_ON:
		if( nOpenParaToken &&
			(pPam->GetPoint()->nContent.GetIndex()
			|| HTML_PARABREAK_ON==nOpenParaToken) )
		{
			// only <P><LI> end paragraph, not with <DD><LI>
			EndPara();
		}

		EndNumBulListItem( 0, sal_False );// <LI>/<LH> quit and set no template
		NewNumBulListItem( nToken );
		break;

	case HTML_LI_OFF:
	case HTML_LISTHEADER_OFF:
		EndNumBulListItem( nToken, sal_False );
		break;

	// Attributes :
	case HTML_ITALIC_ON:
		{
            SvxPostureItem aPosture( ITALIC_NORMAL, RES_CHRATR_POSTURE );
			SvxPostureItem aPostureCJK( ITALIC_NORMAL, RES_CHRATR_CJK_POSTURE );
			SvxPostureItem aPostureCTL( ITALIC_NORMAL, RES_CHRATR_CTL_POSTURE );
			NewStdAttr( HTML_ITALIC_ON,
				   		&aAttrTab.pItalic, aPosture,
				   		&aAttrTab.pItalicCJK, &aPostureCJK,
				   		&aAttrTab.pItalicCTL, &aPostureCTL );
		}
		break;

	case HTML_BOLD_ON:
		{
            SvxWeightItem aWeight( WEIGHT_BOLD, RES_CHRATR_WEIGHT );
			SvxWeightItem aWeightCJK( WEIGHT_BOLD, RES_CHRATR_CJK_WEIGHT );
			SvxWeightItem aWeightCTL( WEIGHT_BOLD, RES_CHRATR_CTL_WEIGHT );
			NewStdAttr( HTML_BOLD_ON,
						&aAttrTab.pBold, aWeight,
						&aAttrTab.pBoldCJK, &aWeightCJK,
						&aAttrTab.pBoldCTL, &aWeightCTL );
		}
		break;


	case HTML_STRIKE_ON:
	case HTML_STRIKETHROUGH_ON:
		{
			NewStdAttr( HTML_STRIKE_ON, &aAttrTab.pStrike,
                        SvxCrossedOutItem(STRIKEOUT_SINGLE, RES_CHRATR_CROSSEDOUT) );
		}
		break;

	case HTML_UNDERLINE_ON:
		{
			NewStdAttr( HTML_UNDERLINE_ON, &aAttrTab.pUnderline,
                        SvxUnderlineItem(UNDERLINE_SINGLE, RES_CHRATR_UNDERLINE) );
		}
		break;

	case HTML_SUPERSCRIPT_ON:
		{
			NewStdAttr( HTML_SUPERSCRIPT_ON, &aAttrTab.pEscapement,
                        SvxEscapementItem(HTML_ESC_SUPER,HTML_ESC_PROP, RES_CHRATR_ESCAPEMENT) );
		}
		break;

	case HTML_SUBSCRIPT_ON:
		{
			NewStdAttr( HTML_SUBSCRIPT_ON, &aAttrTab.pEscapement,
                        SvxEscapementItem(HTML_ESC_SUB,HTML_ESC_PROP, RES_CHRATR_ESCAPEMENT) );
		}
		break;

	case HTML_BLINK_ON:
		{
			NewStdAttr( HTML_BLINK_ON, &aAttrTab.pBlink,
                        SvxBlinkItem( sal_True, RES_CHRATR_BLINK ) );
		}
		break;

	case HTML_SPAN_ON:
		NewStdAttr( HTML_SPAN_ON );
		break;


	case HTML_ITALIC_OFF:
	case HTML_BOLD_OFF:
	case HTML_STRIKE_OFF:
	case HTML_UNDERLINE_OFF:
	case HTML_SUPERSCRIPT_OFF:
	case HTML_SUBSCRIPT_OFF:
	case HTML_BLINK_OFF:
	case HTML_SPAN_OFF:
		EndTag( nToken );
		break;

	case HTML_STRIKETHROUGH_OFF:
		EndTag( HTML_STRIKE_OFF );
		break;

	case HTML_BASEFONT_ON:
		NewBasefontAttr();
		break;
	case HTML_BASEFONT_OFF:
		EndBasefontAttr();
		break;
	case HTML_FONT_ON:
	case HTML_BIGPRINT_ON:
	case HTML_SMALLPRINT_ON:
		NewFontAttr( nToken );
		break;
	case HTML_FONT_OFF:
	case HTML_BIGPRINT_OFF:
	case HTML_SMALLPRINT_OFF:
		EndFontAttr( nToken );
		break;

	case HTML_EMPHASIS_ON:
	case HTML_CITIATION_ON:
	case HTML_STRONG_ON:
	case HTML_CODE_ON:
	case HTML_SAMPLE_ON:
	case HTML_KEYBOARD_ON:
	case HTML_VARIABLE_ON:
	case HTML_DEFINSTANCE_ON:
	case HTML_SHORTQUOTE_ON:
	case HTML_LANGUAGE_ON:
	case HTML_AUTHOR_ON:
	case HTML_PERSON_ON:
	case HTML_ACRONYM_ON:
	case HTML_ABBREVIATION_ON:
	case HTML_INSERTEDTEXT_ON:
	case HTML_DELETEDTEXT_ON:

	case HTML_TELETYPE_ON:
		NewCharFmt( nToken );
		break;

	case HTML_SDFIELD_ON:
		NewField();
		bCallNextToken = bInField && pTable!=0;
		break;

	case HTML_EMPHASIS_OFF:
	case HTML_CITIATION_OFF:
	case HTML_STRONG_OFF:
	case HTML_CODE_OFF:
	case HTML_SAMPLE_OFF:
	case HTML_KEYBOARD_OFF:
	case HTML_VARIABLE_OFF:
	case HTML_DEFINSTANCE_OFF:
	case HTML_SHORTQUOTE_OFF:
	case HTML_LANGUAGE_OFF:
	case HTML_AUTHOR_OFF:
	case HTML_PERSON_OFF:
	case HTML_ACRONYM_OFF:
	case HTML_ABBREVIATION_OFF:
	case HTML_INSERTEDTEXT_OFF:
	case HTML_DELETEDTEXT_OFF:

	case HTML_TELETYPE_OFF:
		EndTag( nToken );
		break;

	case HTML_HEAD_OFF:
		if( aStyleSource.Len() )
		{
			pCSS1Parser->ParseStyleSheet( aStyleSource );
			aStyleSource.Erase();
		}
		break;

	case HTML_DOCTYPE:
	case HTML_BODY_OFF:
	case HTML_HTML_OFF:
	case HTML_HEAD_ON:
	case HTML_TITLE_OFF:
		break;		// do not go on, right???
	case HTML_HTML_ON:
		{
			const HTMLOptions *pHTMLOptions = GetOptions();
			for( sal_uInt16 i = pHTMLOptions->Count(); i; )
			{
				const HTMLOption *pOption = (*pHTMLOptions)[ --i ];
				if( HTML_O_DIR == pOption->GetToken() )
				{
					const String& rDir = pOption->GetString();
					SfxItemSet aItemSet( pDoc->GetAttrPool(),
										 pCSS1Parser->GetWhichMap() );
					SvxCSS1PropertyInfo aPropInfo;
					String aDummy;
					ParseStyleOptions( aDummy, aDummy, aDummy, aItemSet,
									   aPropInfo, 0, &rDir );

					pCSS1Parser->SetPageDescAttrs( 0, &aItemSet );
					break;
				}
			}
		}
		break;

	case HTML_INPUT:
		InsertInput();
		break;

	case HTML_TEXTAREA_ON:
		NewTextArea();
		bCallNextToken = bTextArea && pTable!=0;
		break;

	case HTML_SELECT_ON:
		NewSelect();
		bCallNextToken = bSelect && pTable!=0;
		break;

	case HTML_ANCHOR_ON:
		NewAnchor();
		break;

	case HTML_ANCHOR_OFF:
		EndAnchor();
		break;

	case HTML_COMMENT:
		if( ( aToken.Len() > 5 ) && ( ! bIgnoreHTMLComments ) )
		{
			// insert as Post-It
			// MIB 8.12.2000: If there are no space characters right behind
			// the <!-- and on front of the -->, leave the comment untouched.
			if( ' ' == aToken.GetChar( 3 ) &&
				' ' == aToken.GetChar( aToken.Len()-3 ) )
			{
				String aComment( aToken.Copy( 3, aToken.Len()-5 ) );
				aComment.EraseLeadingChars().EraseTrailingChars();
				InsertComment( aComment );
			}
			else
			{
				String aComment( '<' );
				(aComment += aToken) += '>';
				InsertComment( aComment );
			}
		}
		break;

	case HTML_MAP_ON:
		// Image Maps are read asynchron: At first setup only an
		// ImageMap. The Areas are added later. Never the less add
		// the ImageMap to the IMap-Array, because it could be used
		pImageMap = new ImageMap;
		if( ParseMapOptions( pImageMap) )
		{
			if( !pImageMaps )
				pImageMaps = new ImageMaps;
			pImageMaps->Insert( pImageMap, pImageMaps->Count() );
		}
		else
		{
			delete pImageMap;
			pImageMap = 0;
		}
		break;

	case HTML_MAP_OFF:
		// the ImageMap does not exist anymore
		// but do not delete it, since it has been added to the array
		pImageMap = 0;
		break;

	case HTML_AREA:
		if( pImageMap )
            ParseAreaOptions( pImageMap, sBaseURL, SFX_EVENT_MOUSEOVER_OBJECT,
										 SFX_EVENT_MOUSEOUT_OBJECT );
		break;

	case HTML_FRAMESET_ON:
		bInsertUnknown = bKeepUnknown;
		break;

	case HTML_NOFRAMES_ON:
		if( IsInHeader() )
			FinishHeader( sal_True );
		bInsertUnknown = bKeepUnknown;
		break;

	case HTML_UNKNOWNCONTROL_ON:
	    // Integrate unkown tokens to the header,
	    // exept token starts with '!'
		if( IsInHeader() && !IsReadPRE() && !aUnknownToken.Len() &&
			sSaveToken.Len() && '!' != sSaveToken.GetChar(0) &&
			'%' != sSaveToken.GetChar(0) )
			aUnknownToken = sSaveToken;
		// no break

	default:
		bInsertUnknown = bKeepUnknown;
		break;
	}

	if( bGetIDOption )
		InsertIDOption();

	if( bInsertUnknown )
	{
		String aComment(
			String::CreateFromAscii(TOOLS_CONSTASCII_STRINGPARAM("HTML: <")) );
		if( (HTML_TOKEN_ONOFF & nToken) != 0 && (1 & nToken) != 0 )
			aComment += '/';
		aComment += sSaveToken;
		if( aToken.Len() )
		{
			UnescapeToken();
			(aComment += ' ') += aToken;
		}
		aComment += '>';
		InsertComment( aComment );
	}

    // if temporary paragraph attributes exists and the paragraph is
	// not empty, then paragraph attributes are final
	if( aParaAttrs.Count() && pPam->GetPoint()->nContent.GetIndex() )
		aParaAttrs.Remove( 0, aParaAttrs.Count() );
}

/*  */

extern sal_Bool lcl_css1atr_equalFontItems( const SfxPoolItem& r1, const SfxPoolItem& r2 );

void lcl_swhtml_getItemInfo( const _HTMLAttr& rAttr,
								 sal_Bool& rScriptDependent, sal_Bool& rFont,
								 sal_uInt16& rScriptType )
{
	sal_uInt16 nWhich = rAttr.GetItem().Which();
	switch( nWhich )
	{
	case RES_CHRATR_FONT:
		rFont = sal_True;
	case RES_CHRATR_FONTSIZE:
	case RES_CHRATR_LANGUAGE:
	case RES_CHRATR_POSTURE:
	case RES_CHRATR_WEIGHT:
        rScriptType = i18n::ScriptType::LATIN;
		rScriptDependent = sal_True;
		break;
	case RES_CHRATR_CJK_FONT:
		rFont = sal_True;
	case RES_CHRATR_CJK_FONTSIZE:
	case RES_CHRATR_CJK_LANGUAGE:
	case RES_CHRATR_CJK_POSTURE:
	case RES_CHRATR_CJK_WEIGHT:
        rScriptType = i18n::ScriptType::ASIAN;
		rScriptDependent = sal_True;
		break;
	case RES_CHRATR_CTL_FONT:
		rFont = sal_True;
	case RES_CHRATR_CTL_FONTSIZE:
	case RES_CHRATR_CTL_LANGUAGE:
	case RES_CHRATR_CTL_POSTURE:
	case RES_CHRATR_CTL_WEIGHT:
        rScriptType = i18n::ScriptType::COMPLEX;
		rScriptDependent = sal_True;
		break;
	default:
		rScriptDependent = sal_False;
		rFont = sal_False;
		break;
	}
}

sal_Bool SwHTMLParser::AppendTxtNode( SwHTMLAppendMode eMode, sal_Bool bUpdateNum )
{
	// A hard line feed has to be always removed.
	// A second line feed is replaced by a paragraph spacing
	xub_StrLen nLFStripped = StripTrailingLF();
	if( (AM_NOSPACE==eMode || AM_SOFTNOSPACE==eMode) && nLFStripped > 1 )
		eMode = AM_SPACE;

	// hard attributes in this paragraph can not become invalid
	if( aParaAttrs.Count() )
		aParaAttrs.Remove( 0, aParaAttrs.Count() );

	if( AM_SPACE==eMode || AM_NOSPACE==eMode )
	{
		SwTxtNode *pTxtNode =
            pPam->GetPoint()->nNode.GetNode().GetTxtNode();

		const SvxULSpaceItem& rULSpace =
			(const SvxULSpaceItem&)pTxtNode->SwCntntNode::GetAttr( RES_UL_SPACE );

		sal_Bool bChange = AM_NOSPACE==eMode ? rULSpace.GetLower() > 0
										 : rULSpace.GetLower() == 0;

		if( bChange )
		{
			const SvxULSpaceItem& rCollULSpace =
				pTxtNode->GetAnyFmtColl().GetULSpace();

			sal_Bool bMayReset = AM_NOSPACE==eMode ? rCollULSpace.GetLower() == 0
											   : rCollULSpace.GetLower() > 0;

			if( bMayReset &&
				rCollULSpace.GetUpper() == rULSpace.GetUpper() )
			{
				pTxtNode->ResetAttr( RES_UL_SPACE );
			}
			else
			{
                pTxtNode->SetAttr(
					SvxULSpaceItem( rULSpace.GetUpper(),
                         AM_NOSPACE==eMode ? 0 : HTML_PARSPACE, RES_UL_SPACE ) );
			}
		}
	}
	bNoParSpace = AM_NOSPACE==eMode || AM_SOFTNOSPACE==eMode;

	SwPosition aOldPos( *pPam->GetPoint() );

	sal_Bool bRet = pDoc->AppendTxtNode( *pPam->GetPoint() );

	// split character attributes  and unset if they range over the
	// complete paragraph
	const SwNodeIndex& rEndIdx = aOldPos.nNode;
	xub_StrLen nEndCnt = aOldPos.nContent.GetIndex();
	const SwPosition& rPos = *pPam->GetPoint();

	_HTMLAttr** pTbl = (_HTMLAttr**)&aAttrTab;
	for( sal_uInt16 nCnt = sizeof( _HTMLAttrTable ) / sizeof( _HTMLAttr* );
		 nCnt--; ++pTbl )
	{
		_HTMLAttr *pAttr = *pTbl;
		if( pAttr && pAttr->GetItem().Which() < RES_PARATR_BEGIN )
		{
			sal_Bool bWholePara = sal_False;

			while( pAttr )
			{
				_HTMLAttr *pNext = pAttr->GetNext();
				if( pAttr->GetSttParaIdx() < rEndIdx.GetIndex() ||
					(!bWholePara &&
					 pAttr->GetSttPara() == rEndIdx &&
					 pAttr->GetSttCnt() != nEndCnt) )
				{
					bWholePara =
						pAttr->GetSttPara() == rEndIdx &&
						pAttr->GetSttCnt() == 0;

					xub_StrLen nStt = pAttr->nSttCntnt;
					sal_Bool bScript = sal_False, bFont = sal_False;
					sal_uInt16 nScriptItem;
					sal_Bool bInsert = sal_True;
				   	lcl_swhtml_getItemInfo( *pAttr, bScript, bFont,
											nScriptItem );
					// set parts which we have so far
					if( bInsert && bScript )
					{
						const SwTxtNode *pTxtNd =
							pAttr->GetSttPara().GetNode().GetTxtNode();
						ASSERT( pTxtNd, "No text node" );
						if( pTxtNd )
						{
							const String& rText = pTxtNd->GetTxt();
							sal_uInt16 nScriptTxt =
								pBreakIt->GetBreakIter()->getScriptType(
											rText, pAttr->GetSttCnt() );
							xub_StrLen nScriptEnd = (xub_StrLen)pBreakIt->GetBreakIter()
									->endOfScript( rText, nStt, nScriptTxt );
							while( nScriptEnd < nEndCnt )
							{
								if( nScriptItem == nScriptTxt )
								{
									_HTMLAttr *pSetAttr =
										pAttr->Clone( rEndIdx, nScriptEnd );
									pSetAttr->nSttCntnt = nStt;
									pSetAttr->ClearPrev();
									if( !pNext || bWholePara )
									{
										sal_uInt16 nTmp = pSetAttr->bInsAtStart ? 0
														: aSetAttrTab.Count();
										aSetAttrTab.Insert( pSetAttr, nTmp );
									}
									else
										pNext->InsertPrev( pSetAttr );
								}
								nStt = nScriptEnd;
								nScriptTxt = pBreakIt->GetBreakIter()->getScriptType(
												rText, nStt );
								nScriptEnd = (xub_StrLen)pBreakIt->GetBreakIter()
									->endOfScript( rText, nStt, nScriptTxt );
							}
							bInsert = nScriptItem == nScriptTxt;
						}
					}
					if( bInsert )
					{
						_HTMLAttr *pSetAttr =
							pAttr->Clone( rEndIdx, nEndCnt );
						pSetAttr->nSttCntnt = nStt;

						// If the attribute spans the entire paragraph, then
						// all other attributes are ignored. Therefore
						// it must not be added to the prev list of an outer
						// attribute, because this attribute is 
						// not set for the time being. This leads to shifts,
						// when fields are added to the race (see #51020#).
						if( !pNext || bWholePara )
						{
							sal_uInt16 nTmp = pSetAttr->bInsAtStart ? 0
															 : aSetAttrTab.Count();
							aSetAttrTab.Insert( pSetAttr, nTmp );
						}
						else
							pNext->InsertPrev( pSetAttr );
					}
					else
					{
						_HTMLAttr *pPrev = pAttr->GetPrev();
						if( pPrev )
						{
							// The Previous attributes must be set anyway.
							if( !pNext || bWholePara )
							{
								sal_uInt16 nTmp = pPrev->bInsAtStart ? 0 : aSetAttrTab.Count();
								aSetAttrTab.Insert( pPrev, nTmp );
							}
							else
								pNext->InsertPrev( pPrev );
						}
					}
					pAttr->ClearPrev();
				}

				pAttr->SetStart( rPos );
				pAttr = pNext;
			}
		}
	}

    if( bUpdateNum )
    {
        if( GetNumInfo().GetDepth() )
        {
            sal_uInt8 nLvl = GetNumInfo().GetLevel();
            SetNodeNum( nLvl, false );
        }
        else
            pPam->GetNode()->GetTxtNode()->ResetAttr( RES_PARATR_NUMRULE );
    }

    // because of JavaScript, Attributes in the paragraph should be set now
    SetAttr();

	// Now it is time to get rid of all script dependent hints that are
	// equal to the settings in the style
	SwTxtNode *pTxtNd = rEndIdx.GetNode().GetTxtNode();
	ASSERT( pTxtNd, "There is the txt node" );
	sal_uInt16 nCntAttr = (pTxtNd  && pTxtNd->GetpSwpHints())
							? pTxtNd->GetSwpHints().Count() : 0;
	if( nCntAttr )
	{
		// These are the end position of all script depenent hints.
		// If we find a hint that starts before the current end position,
		// we have to set it. If we finf a hint that start behind or at
		// that position, we have to take the hint's value into account.
		// If it is equal to the style, or in fact the paragarph's value
		// for that hint, the hint is removed. Otherwise it's end position
		// is remembered.
		xub_StrLen aEndPos[15] =
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		SwpHints& rHints = pTxtNd->GetSwpHints();
		for( sal_uInt16 i=0; i < nCntAttr; i++ )
		{
            SwTxtAttr *pHt = rHints.GetTextHint( i );
			sal_uInt16 nWhich = pHt->Which();
			sal_Int16 nIdx = -1;
			if( RES_CHRATR_CJK_FONT <= nWhich &&
					nWhich <= RES_CHRATR_CTL_WEIGHT )
			{
                nIdx = static_cast< sal_uInt16 >(nWhich - RES_CHRATR_CJK_FONT + 5);
			}
			else switch( nWhich )
			{
			case RES_CHRATR_FONT:		nIdx = 0;	break;
			case RES_CHRATR_FONTSIZE:	nIdx = 1;	break;
			case RES_CHRATR_LANGUAGE:	nIdx = 2;	break;
			case RES_CHRATR_POSTURE:	nIdx = 3;	break;
			case RES_CHRATR_WEIGHT:		nIdx = 4;	break;
			}
			if( nIdx != -1 )
			{
				xub_StrLen nStt = *pHt->GetStart();
				if( nStt >= aEndPos[nIdx] )
				{
					sal_Bool bFont = (nIdx % 5) == 0;
					const SfxPoolItem& rItem =
						((const SwCntntNode *)pTxtNd)->GetAttr( nWhich );
					if( bFont ? lcl_css1atr_equalFontItems(rItem,pHt->GetAttr())
							  : rItem == pHt->GetAttr() )
					{
						// The hint is the same as set in the paragraph and
						// therfor, it can be deleted
						// CAUTION!!! This WILL delete the hint and it MAY
						// also delete the SwpHints!!! To avoid any trouble
						// we leave the loop immediately if this is the last
						// hint.
                        pTxtNd->DeleteAttribute( pHt );
						if( 1 == nCntAttr )
							break;
						i--;
						nCntAttr--;
					}
					else
					{
						// The hint is deifferent. Therfor all hints within that
						// hint have to be ignored.
						aEndPos[nIdx] = pHt->GetEnd() ? *pHt->GetEnd() : nStt;
					}
				}
				else
				{
					// The hint starts before another one ends.
					// The hint in this case is not deleted
					ASSERT( pHt->GetEnd() && *pHt->GetEnd() <= aEndPos[nIdx],
							"hints aren't nested properly!" );
				}
			}
		}
	}


	if( !pTable && !--nParaCnt )
		Show();

	return bRet;
}

void SwHTMLParser::AddParSpace()
{
	if( !bNoParSpace )
		return;

	bNoParSpace = sal_False;

	sal_uLong nNdIdx = pPam->GetPoint()->nNode.GetIndex() - 1;

	SwTxtNode *pTxtNode = pDoc->GetNodes()[nNdIdx]->GetTxtNode();
	if( !pTxtNode )
		return;

	SvxULSpaceItem rULSpace =
		(const SvxULSpaceItem&)pTxtNode->SwCntntNode::GetAttr( RES_UL_SPACE );
	if( !rULSpace.GetLower() )
	{
		const SvxULSpaceItem& rCollULSpace =
			pTxtNode->GetAnyFmtColl().GetULSpace();
		if( rCollULSpace.GetLower() &&
			rCollULSpace.GetUpper() == rULSpace.GetUpper() )
		{
			pTxtNode->ResetAttr( RES_UL_SPACE );
		}
		else
		{
            pTxtNode->SetAttr(
                SvxULSpaceItem( rULSpace.GetUpper(), HTML_PARSPACE, RES_UL_SPACE )  );
		}
	}
}


void SwHTMLParser::Show()
{
	// Here
	// - an EndAction is called, which is formatted
	// - a Reschedule is called,
	// - the current view shell is set again
	// - and start action is called

	ASSERT( SVPAR_WORKING==eState, "Show not in working state - This can backfire" );
	ViewShell *pOldVSh = CallEndAction();

	GetpApp()->Reschedule();

	if( ( pDoc->GetDocShell() && pDoc->GetDocShell()->IsAbortingImport() )
		|| 1 == pDoc->getReferenceCount() )
	{
		// was the import aborted by SFX?
		eState = SVPAR_ERROR;
	}

	// Get the ViewShell again, because it could be destroyed during reschedule
	ViewShell *pVSh = CallStartAction( pOldVSh );

	// if the current node is no longer visible, then we use
	// a larger step size
	if( pVSh )
    {
        nParaCnt = (pPam->GetPoint()->nNode.GetNode().IsInVisibleArea(pVSh))
            ? 5 : 50;
    }
}

void SwHTMLParser::ShowStatline()
{
	// Here
	// - a reschedule is called, so that scrolling is possible
	// - the current view shell is set again
	// - a start/end action is called if scrolling was done.

	ASSERT( SVPAR_WORKING==eState, "ShowStatLine not in working state - This can go to the eye" );

	// progress bar
	if( !GetMedium() || !GetMedium()->IsRemote() )
	{
		::SetProgressState( rInput.Tell(), pDoc->GetDocShell() );
		CheckActionViewShell();
	}
	else
	{
		GetpApp()->Reschedule();

		if( ( pDoc->GetDocShell() && pDoc->GetDocShell()->IsAbortingImport() )
			|| 1 == pDoc->getReferenceCount() )
		// was the import aborted by SFX?
			eState = SVPAR_ERROR;

		ViewShell *pVSh = CheckActionViewShell();
		if( pVSh && pVSh->HasInvalidRect() )
		{
			CallEndAction( sal_False, sal_False );
			CallStartAction( pVSh, sal_False );
		}
	}
}

ViewShell *SwHTMLParser::CallStartAction( ViewShell *pVSh, sal_Bool bChkPtr )
{
	ASSERT( !pActionViewShell, "CallStartAction: ViewShell already set" );

	if( !pVSh || bChkPtr )
	{
#ifdef DBG_UTIL
		ViewShell *pOldVSh = pVSh;
#endif
		pDoc->GetEditShell( &pVSh );
		ASSERT( !pVSh || !pOldVSh || pOldVSh == pVSh, "CallStartAction: Who replaced the ViewShell?" );
#ifdef DBG_UTIL
		if( pOldVSh && !pVSh )
			pVSh = 0;
#endif
	}
	pActionViewShell = pVSh;

	if( pActionViewShell )
	{
		if( pActionViewShell->ISA( SwEditShell ) )
			((SwEditShell*)pActionViewShell)->StartAction();
		else
			pActionViewShell->StartAction();
	}

	return pActionViewShell;
}

ViewShell *SwHTMLParser::CallEndAction( sal_Bool bChkAction, sal_Bool bChkPtr )
{
	if( bChkPtr )
	{
		ViewShell *pVSh = 0;
		pDoc->GetEditShell( &pVSh );
		ASSERT( !pVSh || pActionViewShell == pVSh,
				"CallEndAction: Who replaced the ViewShell?" );
#if OSL_DEBUG_LEVEL > 1
		if( pActionViewShell && !pVSh )
			pVSh = 0;
#endif
		if( pVSh != pActionViewShell )
			pActionViewShell = 0;
	}

	if( !pActionViewShell || (bChkAction && !pActionViewShell->ActionPend()) )
		return pActionViewShell;

	if( bSetCrsr )
	{
		// at all CrsrEditShells set the cursors to the doc start.
		ViewShell *pSh = pActionViewShell;
		do {
			if( pSh->IsA( TYPE( SwCrsrShell ) ) )
				((SwCrsrShell*)pSh)->SttEndDoc(sal_True);
			pSh = (ViewShell *)pSh->GetNext();
		} while( pSh != pActionViewShell );

		bSetCrsr = sal_False;
	}
	if( pActionViewShell->ISA( SwEditShell ) )
	{
		//Scrolled already?, then make sure that the view does not move!
		const sal_Bool bOldLock = pActionViewShell->IsViewLocked();
		pActionViewShell->LockView( sal_True );
		const sal_Bool bOldEndActionByVirDev = pActionViewShell->IsEndActionByVirDev();
		pActionViewShell->SetEndActionByVirDev( sal_True );;
		((SwEditShell*)pActionViewShell)->EndAction();
		pActionViewShell->SetEndActionByVirDev( bOldEndActionByVirDev );
		pActionViewShell->LockView( bOldLock );

		// bChkJumpMark is only set if the object was found.
		if( bChkJumpMark )
		{
			const Point aVisSttPos( DOCUMENTBORDER, DOCUMENTBORDER );
			if( GetMedium() && aVisSttPos == pActionViewShell->VisArea().Pos() )
				::JumpToSwMark( pActionViewShell,
								GetMedium()->GetURLObject().GetMark() );
			bChkJumpMark = sal_False;
		}
	}
	else
		pActionViewShell->EndAction();

	// If the parser should be the last one to hold the doc, then you can
	// abort here and set an error.
	if( 1 == pDoc->getReferenceCount() )
	{
		eState = SVPAR_ERROR;
	}

	ViewShell *pVSh = pActionViewShell;
	pActionViewShell = 0;

	return pVSh;
}

ViewShell *SwHTMLParser::CheckActionViewShell()
{
	ViewShell *pVSh = 0;
	pDoc->GetEditShell( &pVSh );
	ASSERT( !pVSh || pActionViewShell == pVSh,
			"CheckActionViewShell: Who replaced the ViewShell?" );
#if OSL_DEBUG_LEVEL > 1
	if( pActionViewShell && !pVSh )
		pVSh = 0;
#endif
	if( pVSh != pActionViewShell )
		pActionViewShell = 0;

	return pActionViewShell;
}

/*  */

void SwHTMLParser::_SetAttr( sal_Bool bChkEnd, sal_Bool bBeforeTable,
							 _HTMLAttrs *pPostIts )
{
	SwPaM* pAttrPam = new SwPaM( *pPam->GetPoint() );
	const SwNodeIndex& rEndIdx = pPam->GetPoint()->nNode;
	xub_StrLen nEndCnt = pPam->GetPoint()->nContent.GetIndex();
	_HTMLAttr* pAttr;
	SwCntntNode* pCNd;
	sal_uInt16 n;

	_HTMLAttrs aFields( 5, 5 );

	for( n = aSetAttrTab.Count(); n; )
	{
		pAttr = aSetAttrTab[ --n ];
		sal_uInt16 nWhich = pAttr->pItem->Which();

		sal_uLong nEndParaIdx = pAttr->GetEndParaIdx();
		sal_Bool bSetAttr;
		if( bChkEnd )
		{
			// fix #42192#: set character attributes in current paragraph with end 
			// as early as possible, because of JavaScript and various chats.
			// Except attributes, which are spanned over the whole paragraph, because
			// they come from paragraph occurrences that cannot be set.
			// Because the attributes are inserted with SETATTR_DONTREPLACE
			// they should be able to set them subsequently.
			bSetAttr = ( nEndParaIdx < rEndIdx.GetIndex() &&
						 (RES_LR_SPACE != nWhich || !GetNumInfo().GetNumRule()) ) ||
					   ( !pAttr->IsLikePara() &&
						 nEndParaIdx == rEndIdx.GetIndex() &&
						 pAttr->GetEndCnt() < nEndCnt &&
                         (isCHRATR(nWhich) || isTXTATR_WITHEND(nWhich)) ) ||
					   ( bBeforeTable &&
						 nEndParaIdx == rEndIdx.GetIndex() &&
						 !pAttr->GetEndCnt() );
		}
		else
		{
			// Attributes in the content area may not be set in 
			// special areai, but if the opposite is true.
			sal_uLong nEndOfIcons = pDoc->GetNodes().GetEndOfExtras().GetIndex();
			bSetAttr = nEndParaIdx < rEndIdx.GetIndex() ||
					   rEndIdx.GetIndex() > nEndOfIcons ||
					   nEndParaIdx <= nEndOfIcons;
		}

		if( bSetAttr )
		{
			// The attribute must not be in the list of previous
			// paragraph attributes, otherwise it will be deleted.
			sal_uInt16 ii = aParaAttrs.Count();
			while( ii-- )
			{
				ASSERT( pAttr != aParaAttrs[ii],
						"SetAttr: Attribute may not be set yet" );
				aParaAttrs.Remove( ii );
			}


			// then set
			aSetAttrTab.Remove( n, 1 );

			while( pAttr )
			{
				_HTMLAttr *pPrev = pAttr->GetPrev();
				if( !pAttr->bValid )
				{
					// invalid attributes can be deleted
					delete pAttr;
					pAttr = pPrev;
					continue; //break;
				}


                pCNd = pAttr->nSttPara.GetNode().GetCntntNode();
				if( !pCNd )
				{
					// due to the miserable deletion of nodes also sometimes
					// an index can point to an end node :-(
                    if ( (pAttr->GetSttPara() == pAttr->GetEndPara()) &&
                         !isTXTATR_NOEND(nWhich) )
					{
						// if the end index also points to the node
						// we don't need to set an attribute anymore,
						// unless it is a text attribute.
						delete pAttr;
						pAttr = pPrev;
						continue; //break;
					}
					pCNd = pDoc->GetNodes().GoNext( &(pAttr->nSttPara) );
					if( pCNd )
						pAttr->nSttCntnt = 0;
					else
					{
						ASSERT( sal_False, "SetAttr: GoNext() failed!" );
						delete pAttr;
						pAttr = pPrev;
						continue; // break;
					}
				}
				pAttrPam->GetPoint()->nNode = pAttr->nSttPara;



				// by deleting BRs, the start index can
				// can also sometimes point behind the end of the text
				if( pAttr->nSttCntnt > pCNd->Len() )
					pAttr->nSttCntnt = pCNd->Len();
				pAttrPam->GetPoint()->nContent.Assign( pCNd, pAttr->nSttCntnt );

				pAttrPam->SetMark();
                if ( (pAttr->GetSttPara() != pAttr->GetEndPara()) &&
                         !isTXTATR_NOEND(nWhich) )
                {
                    pCNd = pAttr->nEndPara.GetNode().GetCntntNode();
					if( !pCNd )
					{
						pCNd = pDoc->GetNodes().GoPrevious( &(pAttr->nEndPara) );
						if( pCNd )
							pAttr->nEndCntnt = pCNd->Len();
						else
						{
							ASSERT( sal_False, "SetAttr: GoPrevious() failed!" );
							pAttrPam->DeleteMark();
							delete pAttr;
							pAttr = pPrev;
							continue; // break;
						}
					}

					pAttrPam->GetPoint()->nNode = pAttr->nEndPara;
				}
				else if( pAttr->IsLikePara() )
				{
					pAttr->nEndCntnt = pCNd->Len();
				}

				// by deleting BRs, the end index can
				// can also sometimes point behind the end of the text
				if( pAttr->nEndCntnt > pCNd->Len() )
					pAttr->nEndCntnt = pCNd->Len();

				pAttrPam->GetPoint()->nContent.Assign( pCNd, pAttr->nEndCntnt );
				if( bBeforeTable &&
					pAttrPam->GetPoint()->nNode.GetIndex() ==
						rEndIdx.GetIndex() )
				{
					// if we are about to insert a table
					// and the attribute is terminated in the current node,
					// we must end it in the node before or throw it away,
					// if it starts in the node before
					if( nWhich != RES_BREAK && nWhich != RES_PAGEDESC &&
                         !isTXTATR_NOEND(nWhich) )
					{
						if( pAttrPam->GetMark()->nNode.GetIndex() !=
							rEndIdx.GetIndex() )
						{
							ASSERT( !pAttrPam->GetPoint()->nContent.GetIndex(),
									"Content position before table not 0???" );
							pAttrPam->Move( fnMoveBackward );
						}
						else
						{
							pAttrPam->DeleteMark();
							delete pAttr;
							pAttr = pPrev;
							continue;
						}
					}
				}

				switch( nWhich )
				{
                case RES_FLTR_BOOKMARK: // insert bookmark
                    {
                        const String sName( ((SfxStringItem*)pAttr->pItem)->GetValue() );
                        IDocumentMarkAccess* const pMarkAccess = pDoc->getIDocumentMarkAccess();
                        IDocumentMarkAccess::const_iterator_t ppBkmk = pMarkAccess->findMark( sName );
                        if( ppBkmk != pMarkAccess->getAllMarksEnd() &&
                            ppBkmk->get()->GetMarkStart() == *pAttrPam->GetPoint() )
                            break; // do not generate duplicates on this position
                        pAttrPam->DeleteMark();
                        const ::sw::mark::IMark* const pNewMark = pMarkAccess->makeMark(
                            *pAttrPam,
                            sName,
                            IDocumentMarkAccess::BOOKMARK );

                        // jump to bookmark
                        if( JUMPTO_MARK == eJumpTo && pNewMark->GetName() == ::rtl::OUString(sJmpMark) )
                        {
                            bChkJumpMark = sal_True;
                            eJumpTo = JUMPTO_NONE;
                        }
                    }
                    break;
                case RES_TXTATR_FIELD:
                case RES_TXTATR_ANNOTATION:
                case RES_TXTATR_INPUTFIELD:
                    {
                        sal_uInt16 nFldWhich =
                            pPostIts
                            ? ((const SwFmtFld *)pAttr->pItem)->GetField()->GetTyp()->Which()
                            : 0;
                        if( pPostIts && (RES_POSTITFLD == nFldWhich ||
                                         RES_SCRIPTFLD == nFldWhich) )
                        {
                            pPostIts->Insert( pAttr, 0 );
                        }
                        else
                        {
                            aFields.Insert( pAttr, aFields.Count() );
                        }
                    }
                    pAttrPam->DeleteMark();
                    pAttr = pPrev;
                    continue;

				case RES_LR_SPACE:
					if( pAttrPam->GetPoint()->nNode.GetIndex() ==
						pAttrPam->GetMark()->nNode.GetIndex() &&
						pCNd )
					{
						// because of numbering this attribute directly
						// set at the node
						pCNd->SetAttr( *pAttr->pItem );
						break;
					}
					ASSERT(	sal_False,
							"LRSpace set over several paragraphs!" );
					// no break (we are never supposed to get here anyway;
				default:

					// jump to a bookmark if necessary
					if( RES_TXTATR_INETFMT == nWhich &&
						JUMPTO_MARK == eJumpTo &&
						sJmpMark == ((SwFmtINetFmt*)pAttr->pItem)->GetName() )
					{
						bChkJumpMark = sal_True;
						eJumpTo = JUMPTO_NONE;
					}

                    pDoc->InsertPoolItem( *pAttrPam, *pAttr->pItem, nsSetAttrMode::SETATTR_DONTREPLACE );
				}
				pAttrPam->DeleteMark();

				delete pAttr;
				pAttr = pPrev;
			}
		}
	}

	for( n = aMoveFlyFrms.Count(); n; )
	{
		SwFrmFmt *pFrmFmt = aMoveFlyFrms[ --n ];

		const SwFmtAnchor& rAnchor = pFrmFmt->GetAnchor();
        ASSERT( FLY_AT_PARA == rAnchor.GetAnchorId(),
				"Only car frames need special treatment" );
		const SwPosition *pFlyPos = rAnchor.GetCntntAnchor();
		sal_uLong nFlyParaIdx = pFlyPos->nNode.GetIndex();
		sal_Bool bMoveFly;
		if( bChkEnd )
		{
			bMoveFly = nFlyParaIdx < rEndIdx.GetIndex() ||
					   ( nFlyParaIdx == rEndIdx.GetIndex() &&
						 aMoveFlyCnts[n] < nEndCnt );
		}
		else
		{
			sal_uLong nEndOfIcons = pDoc->GetNodes().GetEndOfExtras().GetIndex();
			bMoveFly = nFlyParaIdx < rEndIdx.GetIndex() ||
					   rEndIdx.GetIndex() > nEndOfIcons ||
					   nFlyParaIdx <= nEndOfIcons;
		}
		if( bMoveFly )
		{
			pFrmFmt->DelFrms();
			*pAttrPam->GetPoint() = *pFlyPos;
			pAttrPam->GetPoint()->nContent.Assign( pAttrPam->GetCntntNode(),
												   aMoveFlyCnts[n] );
			SwFmtAnchor aAnchor( rAnchor );
            aAnchor.SetType( FLY_AT_CHAR );
			aAnchor.SetAnchor( pAttrPam->GetPoint() );
            pFrmFmt->SetFmtAttr( aAnchor );

			const SwFmtHoriOrient& rHoriOri = pFrmFmt->GetHoriOrient();
            if( text::HoriOrientation::LEFT == rHoriOri.GetHoriOrient() )
			{
				SwFmtHoriOrient aHoriOri( rHoriOri );
                aHoriOri.SetRelationOrient( text::RelOrientation::CHAR );
                pFrmFmt->SetFmtAttr( aHoriOri );
			}
			const SwFmtVertOrient& rVertOri = pFrmFmt->GetVertOrient();
            if( text::VertOrientation::TOP == rVertOri.GetVertOrient() )
			{
				SwFmtVertOrient aVertOri( rVertOri );
                aVertOri.SetRelationOrient( text::RelOrientation::CHAR );
                pFrmFmt->SetFmtAttr( aVertOri );
			}

			pFrmFmt->MakeFrms();
			aMoveFlyFrms.Remove( n, 1 );
			aMoveFlyCnts.erase( aMoveFlyCnts.begin() + n );
		}
	}
	while( aFields.Count() )
	{
		pAttr = aFields[0];

        pCNd = pAttr->nSttPara.GetNode().GetCntntNode();
		pAttrPam->GetPoint()->nNode = pAttr->nSttPara;
		pAttrPam->GetPoint()->nContent.Assign( pCNd, pAttr->nSttCntnt );

		if( bBeforeTable &&
			pAttrPam->GetPoint()->nNode.GetIndex() == rEndIdx.GetIndex() )
		{
			ASSERT( !bBeforeTable, "Aha, the case occurs after all" );
			ASSERT( !pAttrPam->GetPoint()->nContent.GetIndex(),
					"Content position before table not 0???" );
			// !!!
			pAttrPam->Move( fnMoveBackward );
		}

        pDoc->InsertPoolItem( *pAttrPam, *pAttr->pItem, 0 );

		aFields.Remove( 0, 1 );
		delete pAttr;
	}

	delete pAttrPam;
}

void SwHTMLParser::NewAttr( _HTMLAttr **ppAttr, const SfxPoolItem& rItem )
{
	// font-height and -color as well as escapement attributes must not be
	// be combined. Therefore they are stored in a list,
	// where the last expanded attribute is at the front and the count is always 1.
	// For all other attributes the count is simply incremented.
	if( *ppAttr )
	{
		_HTMLAttr *pAttr = new _HTMLAttr( *pPam->GetPoint(), rItem,
											ppAttr );
		pAttr->InsertNext( *ppAttr );
		(*ppAttr) = pAttr;
	}
	else
		(*ppAttr) = new _HTMLAttr( *pPam->GetPoint(), rItem, ppAttr );
}


void SwHTMLParser::EndAttr( _HTMLAttr* pAttr, _HTMLAttr **ppDepAttr,
							sal_Bool bChkEmpty )
{
	ASSERT( !ppDepAttr, "SwHTMLParser::EndAttr: untested ppDepAttr-Feature?" );
	// The list header is stored in the attribute
	_HTMLAttr **ppHead = pAttr->ppHead;

	ASSERT( ppHead, "no attribute list header found!" );

	// remember the current psoition as the end position
	const SwNodeIndex* pEndIdx = &pPam->GetPoint()->nNode;
	xub_StrLen nEndCnt = pPam->GetPoint()->nContent.GetIndex();

	// Is the last started or a previously started attribute
	// finished?
	_HTMLAttr *pLast = 0;
	if( ppHead && pAttr != *ppHead )
	{
		// The last started attribute is not terminated

		// Then we search for the attribute that was started immediately afterwards, which
		// has not been finished yet either (otherwise it would not be
		// more in the list
		pLast = *ppHead;
		while( pLast && pLast->GetNext() != pAttr )
			pLast = pLast->GetNext();

		ASSERT( pLast, "Attribute not found in own list!" );

		// Do not end the attribute at the PaM psoition, but there,
		// where the attribute started after that???
		//pEndIdx = &pPrev->GetSttPara();
		//nEndCnt = pPrev->GetSttCnt();
	}

	sal_Bool bMoveBack = sal_False;
	sal_uInt16 nWhich = pAttr->pItem->Which();
	if( /*!pLast &&*/ !nEndCnt && RES_PARATR_BEGIN <= nWhich &&
		*pEndIdx != pAttr->GetSttPara() )
	{
		// then one Cntnt position back!
		bMoveBack = pPam->Move( fnMoveBackward );
		nEndCnt = pPam->GetPoint()->nContent.GetIndex();
	}

	// end this Attrubut
	_HTMLAttr *pNext = pAttr->GetNext();


	sal_Bool bInsert;
    sal_uInt16 nScriptItem = 0;
	sal_Bool bScript = sal_False, bFont = sal_False;
	// an area ??
	if( !bChkEmpty || (RES_PARATR_BEGIN <= nWhich && bMoveBack) ||
		RES_PAGEDESC == nWhich || RES_BREAK == nWhich ||
		*pEndIdx != pAttr->GetSttPara() ||
		nEndCnt != pAttr->GetSttCnt() )
	{
		bInsert = sal_True;
		// We do some optimization for script depenedent attribtes here.
		if( *pEndIdx == pAttr->GetSttPara() )
		{
			lcl_swhtml_getItemInfo( *pAttr, bScript, bFont, nScriptItem );
		}
	}
	else
	{
		bInsert = sal_False;
	}

	if( bInsert && bScript )
	{
		const SwTxtNode *pTxtNd = pAttr->GetSttPara().GetNode()
											.GetTxtNode();
		ASSERT( pTxtNd, "No text node" );
		const String& rText = pTxtNd->GetTxt();
		sal_uInt16 nScriptTxt = pBreakIt->GetBreakIter()->getScriptType(
						rText, pAttr->GetSttCnt() );
		xub_StrLen nScriptEnd = (xub_StrLen)pBreakIt->GetBreakIter()
					->endOfScript( rText, pAttr->GetSttCnt(), nScriptTxt );
		while( nScriptEnd < nEndCnt )
		{
			if( nScriptItem == nScriptTxt )
			{
				_HTMLAttr *pSetAttr = pAttr->Clone( *pEndIdx, nScriptEnd );
				pSetAttr->ClearPrev();
				if( pNext )
					pNext->InsertPrev( pSetAttr );
				else
				{
					sal_uInt16 nTmp = pSetAttr->bInsAtStart ? 0
														: aSetAttrTab.Count();
					aSetAttrTab.Insert( pSetAttr, nTmp );
				}
			}
			pAttr->nSttCntnt = nScriptEnd;
			nScriptTxt = pBreakIt->GetBreakIter()->getScriptType(
							rText, nScriptEnd );
			nScriptEnd = (xub_StrLen)pBreakIt->GetBreakIter()
					->endOfScript( rText, nScriptEnd, nScriptTxt );
		}
		bInsert = nScriptItem == nScriptTxt;
	}
	if( bInsert )
	{
		pAttr->nEndPara = *pEndIdx;
		pAttr->nEndCntnt = nEndCnt;
		pAttr->bInsAtStart = RES_TXTATR_INETFMT != nWhich &&
							 RES_TXTATR_CHARFMT != nWhich;

		if( !pNext )
		{
			// there are no more open attributes of this type,
			// then all of them can be set, unless
			// they still depend on another attribute,
			// then they will be appended there
			if( ppDepAttr && *ppDepAttr )
				(*ppDepAttr)->InsertPrev( pAttr );
			else
			{
				sal_uInt16 nTmp = pAttr->bInsAtStart ? 0 : aSetAttrTab.Count();
				aSetAttrTab.Insert( pAttr, nTmp );
			}
		}
		else
		{
			// there are other open attributes of the type,
			// so the setting must be reset.
			// the current attribute is therefore appended to the end of the
			// previous list of the successor.
			pNext->InsertPrev( pAttr );
		}
	}
	else
	{
		// Then do not insert, but delete. Through "fakeing" of
		// templates by hard attributes, there may be other empty attributes in the
		// empty attributes in the Prev list, which then must be set anyway.
		_HTMLAttr *pPrev = pAttr->GetPrev();
		delete pAttr;

		if( pPrev )
		{
			// The Previous attributes must be set anyway.
			if( pNext )
				pNext->InsertPrev( pPrev );
			else
			{
				sal_uInt16 nTmp = pPrev->bInsAtStart ? 0 : aSetAttrTab.Count();
				aSetAttrTab.Insert( pPrev, nTmp );
			}
		}

	}

	// if the first attribute of the list has been set the
	// list header must be corrected.
	if( pLast )
		pLast->pNext = pNext;
	else if( ppHead )
		*ppHead = pNext;

	if( bMoveBack )
		pPam->Move( fnMoveForward );
}

void SwHTMLParser::DeleteAttr( _HTMLAttr* pAttr )
{
	// There must not be any leading paragraph attributes here, because the
	// could be set now and then the pointers are invalid!!!
	ASSERT( !aParaAttrs.Count(),
			"Highest risk: There are still non-final paragraph attributes" );
	if( aParaAttrs.Count() )
		aParaAttrs.Remove( 0, aParaAttrs.Count() );

	// The list header is stored in the attribute
	_HTMLAttr **ppHead = pAttr->ppHead;

	ASSERT( ppHead, "no attributes list header found!" );

	// Is the attribute that was started last or an attribute that was started earlier
	// removed?
	_HTMLAttr *pLast = 0;
	if( ppHead && pAttr != *ppHead )
	{
		// The last started attribute is not terminated

		// Then we search for the attribute that was started immediately afterwards, which
		// has not been finished yet either (otherwise it would not be
		// more in the list
		pLast = *ppHead;
		while( pLast && pLast->GetNext() != pAttr )
			pLast = pLast->GetNext();

		ASSERT( pLast, "Attribute not found in own list!" );
	}

	// now remove the attribute
	_HTMLAttr *pNext = pAttr->GetNext();
	_HTMLAttr *pPrev = pAttr->GetPrev();
	delete pAttr;

	if( pPrev )
	{
		// The Previous attributes must be set anyway.
		if( pNext )
			pNext->InsertPrev( pPrev );
		else
		{
			sal_uInt16 nTmp = pPrev->bInsAtStart ? 0 : aSetAttrTab.Count();
			aSetAttrTab.Insert( pPrev, nTmp );
		}
	}

	// if the first attribute of the list has been removed the
	// list header must be corrected.
	if( pLast )
		pLast->pNext = pNext;
	else if( ppHead )
		*ppHead = pNext;
}

void SwHTMLParser::SaveAttrTab( _HTMLAttrTable& rNewAttrTab )
{
	// There must not be any leading paragraph attributes here, because the
	// could be set now and then the pointers are invalid!!!
	ASSERT( !aParaAttrs.Count(),
			"Highest risk: There are still non-final paragraph attributes" );
	if( aParaAttrs.Count() )
		aParaAttrs.Remove( 0, aParaAttrs.Count() );

	_HTMLAttr** pTbl = (_HTMLAttr**)&aAttrTab;
	_HTMLAttr** pSaveTbl = (_HTMLAttr**)&rNewAttrTab;

	for( sal_uInt16 nCnt = sizeof( _HTMLAttrTable ) / sizeof( _HTMLAttr* );
		 nCnt--; (++pTbl, ++pSaveTbl) )
	{
		*pSaveTbl = *pTbl;

		_HTMLAttr *pAttr = *pSaveTbl;
		while( pAttr )
		{
			pAttr->SetHead( pSaveTbl );
			pAttr = pAttr->GetNext();
		}

		*pTbl = 0;
	}
}

void SwHTMLParser::SplitAttrTab( _HTMLAttrTable& rNewAttrTab,
								 sal_Bool bMoveEndBack )
{
	// There must not be any leading paragraph attributes here, because the
	// could be set now and then the pointers are invalid!!!
	ASSERT( !aParaAttrs.Count(),
			"Highest risk: There are still non-final paragraph attributes" );
	if( aParaAttrs.Count() )
		aParaAttrs.Remove( 0, aParaAttrs.Count() );

	const SwNodeIndex& nSttIdx = pPam->GetPoint()->nNode;
	SwNodeIndex nEndIdx( nSttIdx );

	// terminate all attributes still open and span behind the table
	// span again
	_HTMLAttr** pTbl = (_HTMLAttr**)&aAttrTab;
	_HTMLAttr** pSaveTbl = (_HTMLAttr**)&rNewAttrTab;
	sal_Bool bSetAttr = sal_True;
	xub_StrLen nSttCnt = pPam->GetPoint()->nContent.GetIndex();
	xub_StrLen nEndCnt = nSttCnt;

	if( bMoveEndBack )
	{
		sal_uLong nOldEnd = nEndIdx.GetIndex();
		sal_uLong nTmpIdx;
		if(	( nTmpIdx = pDoc->GetNodes().GetEndOfExtras().GetIndex()) >= nOldEnd ||
			( nTmpIdx = pDoc->GetNodes().GetEndOfAutotext().GetIndex()) >= nOldEnd )
		{
			nTmpIdx = pDoc->GetNodes().GetEndOfInserts().GetIndex();
		}
		SwCntntNode* pCNd = pDoc->GetNodes().GoPrevious(&nEndIdx);

		// Do not set attributes if the PaM has been moved out of the content area.
		// has been pushed out.
		bSetAttr = pCNd && nTmpIdx < nEndIdx.GetIndex();

		nEndCnt = (bSetAttr ? pCNd->Len() : 0);
	}
	for( sal_uInt16 nCnt = sizeof( _HTMLAttrTable ) / sizeof( _HTMLAttr* );
		 nCnt--; (++pTbl, ++pSaveTbl) )
	{
		_HTMLAttr *pAttr = *pTbl;
		*pSaveTbl = 0;
		while( pAttr )
		{
			_HTMLAttr *pNext = pAttr->GetNext();
			_HTMLAttr *pPrev = pAttr->GetPrev();

			if( bSetAttr &&
				( pAttr->GetSttParaIdx() < nEndIdx.GetIndex() ||
				  (pAttr->GetSttPara() == nEndIdx &&
				   pAttr->GetSttCnt() != nEndCnt) ) )
			{
				// the attribute must be set before the list. Since we
				// still need the original because pointers to the attribute
				// still exist in the contexts, we have to clone it.
				// The next list will be lost, but the
				// Previous list is preserved
				_HTMLAttr *pSetAttr = pAttr->Clone( nEndIdx, nEndCnt );

				if( pNext )
					pNext->InsertPrev( pSetAttr );
				else
				{
					sal_uInt16 nTmp = pSetAttr->bInsAtStart ? 0
														: aSetAttrTab.Count();
					aSetAttrTab.Insert( pSetAttr, nTmp );
				}
			}
			else if( pPrev )
			{
				// If the attribute is not set before the table
				// the Previous attribute must be set
				// must be set anyway.
				if( pNext )
					pNext->InsertPrev( pPrev );
				else
				{
					sal_uInt16 nTmp = pPrev->bInsAtStart ? 0 : aSetAttrTab.Count();
					aSetAttrTab.Insert( pPrev, nTmp );
				}
			}

			// reset the start of the attribute and the concatenations
			// break
			pAttr->Reset( nSttIdx, nSttCnt, pSaveTbl );

			if( *pSaveTbl )
			{
				_HTMLAttr *pSAttr = *pSaveTbl;
				while( pSAttr->GetNext() )
					pSAttr = pSAttr->GetNext();
				pSAttr->InsertNext( pAttr );
			}
			else
				*pSaveTbl = pAttr;

			pAttr = pNext;
		}

		*pTbl = 0;
	}
}

void SwHTMLParser::RestoreAttrTab( const _HTMLAttrTable& rNewAttrTab,
								   sal_Bool bSetNewStart )
{
	// There must not be any leading paragraph attributes here, because the
	// could be set now and then the pointers are invalid!!!
	ASSERT( !aParaAttrs.Count(),
			"Highest risk: There are still non-final paragraph attributes" );
	if( aParaAttrs.Count() )
		aParaAttrs.Remove( 0, aParaAttrs.Count() );

	_HTMLAttr** pTbl = (_HTMLAttr**)&aAttrTab;
	_HTMLAttr** pSaveTbl = (_HTMLAttr**)&rNewAttrTab;

	for( sal_uInt16 nCnt = sizeof( _HTMLAttrTable ) / sizeof( _HTMLAttr* );
		nCnt--; (++pTbl, ++pSaveTbl) )
	{
		ASSERT( !*pTbl, "The attribute table is not empty!" );

		const SwPosition *pPos = pPam->GetPoint();
		const SwNodeIndex& rSttPara = pPos->nNode;
		xub_StrLen nSttCnt = pPos->nContent.GetIndex();

		*pTbl = *pSaveTbl;

		_HTMLAttr *pAttr = *pTbl;
		while( pAttr )
		{
			ASSERT( !pAttr->GetPrev() || !pAttr->GetPrev()->ppHead,
					"Previous attribute still has a header" );
			pAttr->SetHead( pTbl );
			if( bSetNewStart )
			{
				pAttr->nSttPara = rSttPara;
				pAttr->nEndPara = rSttPara;
				pAttr->nSttCntnt = nSttCnt;
				pAttr->nEndCntnt = nSttCnt;
			}
			pAttr = pAttr->GetNext();
		}

		*pSaveTbl = 0;
	}
}

void SwHTMLParser::InsertAttr( const SfxPoolItem& rItem, sal_Bool bLikePara,
							   sal_Bool bInsAtStart )
{
	_HTMLAttr* pTmp = new _HTMLAttr( *pPam->GetPoint(),
									 rItem );
	if( bLikePara )
		pTmp->SetLikePara();
	sal_uInt16 nTmp = bInsAtStart ? 0 : aSetAttrTab.Count();
	aSetAttrTab.Insert( pTmp, nTmp );
}

void SwHTMLParser::InsertAttrs( _HTMLAttrs& rAttrs )
{
	while( rAttrs.Count() )
	{
		_HTMLAttr *pAttr = rAttrs[0];
		InsertAttr( pAttr->GetItem() );
		rAttrs.Remove( 0, 1 );
		delete pAttr;
	}
}

/*  */

void SwHTMLParser::NewStdAttr( int nToken )
{
	String aId, aStyle, aClass, aLang, aDir;

	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		switch( pOption->GetToken() )
		{
		case HTML_O_ID:
			aId = pOption->GetString();
			break;
		case HTML_O_STYLE:
			aStyle = pOption->GetString();
			break;
		case HTML_O_CLASS:
			aClass = pOption->GetString();
			break;
		case HTML_O_LANG:
			aLang = pOption->GetString();
			break;
		case HTML_O_DIR:
			aDir = pOption->GetString();
			break;
		}
	}

	// create a new context
    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( static_cast< sal_uInt16 >(nToken) );

	// parse styles
	if( HasStyleOptions( aStyle, aId, aClass, &aLang, &aDir ) )
	{
		SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
		SvxCSS1PropertyInfo aPropInfo;

		if( ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo, &aLang, &aDir ) )
		{
			if( HTML_SPAN_ON != nToken || !aClass.Len() ||
				!CreateContainer( aClass, aItemSet, aPropInfo, pCntxt ) )
				DoPositioning( aItemSet, aPropInfo, pCntxt );
			InsertAttrs( aItemSet, aPropInfo, pCntxt, sal_True );
		}
	}

	// memorize context
	PushContext( pCntxt );
}

void SwHTMLParser::NewStdAttr( int nToken,
							   _HTMLAttr **ppAttr, const SfxPoolItem & rItem,
							   _HTMLAttr **ppAttr2, const SfxPoolItem *pItem2,
							   _HTMLAttr **ppAttr3, const SfxPoolItem *pItem3 )
{
	String aId, aStyle, aClass, aLang, aDir;

	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		switch( pOption->GetToken() )
		{
		case HTML_O_ID:
			aId = pOption->GetString();
			break;
		case HTML_O_STYLE:
			aStyle = pOption->GetString();
			break;
		case HTML_O_CLASS:
			aClass = pOption->GetString();
			break;
		case HTML_O_LANG:
			aLang = pOption->GetString();
			break;
		case HTML_O_DIR:
			aDir = pOption->GetString();
			break;
		}
	}

	// create new context
    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( static_cast< sal_uInt16 >(nToken) );

	// parse Styles
	if( HasStyleOptions( aStyle, aId, aClass, &aLang, &aDir ) )
	{
		SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
		SvxCSS1PropertyInfo aPropInfo;

		aItemSet.Put( rItem );
		if( pItem2 )
			aItemSet.Put( *pItem2 );
		if( pItem3 )
			aItemSet.Put( *pItem3 );

		if( ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo, &aLang, &aDir ) )
			DoPositioning( aItemSet, aPropInfo, pCntxt );

		InsertAttrs( aItemSet, aPropInfo, pCntxt, sal_True );
	}
	else
	{
		InsertAttr( ppAttr ,rItem, pCntxt );
		if( pItem2 )
		{
			ASSERT( ppAttr2, "missing table entry for item2" );
			InsertAttr( ppAttr2, *pItem2, pCntxt );
		}
		if( pItem3 )
		{
			ASSERT( ppAttr3, "missing table entry for item3" );
			InsertAttr( ppAttr3, *pItem3, pCntxt );
		}
	}

	// memorize context
	PushContext( pCntxt );
}

void SwHTMLParser::EndTag( int nToken )
{
	// get context
    _HTMLAttrContext *pCntxt = PopContext( static_cast< sal_uInt16 >(nToken & ~1) );
	if( pCntxt )
	{
		// and terminate the attributes if necessary
		EndContext( pCntxt );
		delete pCntxt;
	}
}


void SwHTMLParser::NewBasefontAttr()
{
	String aId, aStyle, aClass, aLang, aDir;
	sal_uInt16 nSize = 3;

	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		switch( pOption->GetToken() )
		{
		case HTML_O_SIZE:
			nSize = (sal_uInt16)pOption->GetNumber();
			break;
		case HTML_O_ID:
			aId = pOption->GetString();
			break;
		case HTML_O_STYLE:
			aStyle = pOption->GetString();
			break;
		case HTML_O_CLASS:
			aClass = pOption->GetString();
			break;
		case HTML_O_LANG:
			aLang = pOption->GetString();
			break;
		case HTML_O_DIR:
			aDir = pOption->GetString();
			break;
		}
	}

	if( nSize < 1 )
		nSize = 1;

	if( nSize > 7 )
		nSize = 7;

	// set up a new context
	_HTMLAttrContext *pCntxt = new _HTMLAttrContext( HTML_BASEFONT_ON );

	// parse styles
	if( HasStyleOptions( aStyle, aId, aClass, &aLang, &aDir ) )
	{
		SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
		SvxCSS1PropertyInfo aPropInfo;

        SvxFontHeightItem aFontHeight( aFontHeights[nSize-1], 100, RES_CHRATR_FONTSIZE );
		aItemSet.Put( aFontHeight );
		aFontHeight.SetWhich( RES_CHRATR_CJK_FONTSIZE );
		aItemSet.Put( aFontHeight );
		aFontHeight.SetWhich( RES_CHRATR_CTL_FONTSIZE );
		aItemSet.Put( aFontHeight );

		if( ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo, &aLang, &aDir ) )
			DoPositioning( aItemSet, aPropInfo, pCntxt );

		InsertAttrs( aItemSet, aPropInfo, pCntxt, sal_True );
	}
	else
	{
        SvxFontHeightItem aFontHeight( aFontHeights[nSize-1], 100, RES_CHRATR_FONTSIZE );
		InsertAttr( &aAttrTab.pFontHeight, aFontHeight, pCntxt );
		aFontHeight.SetWhich( RES_CHRATR_CJK_FONTSIZE );
		InsertAttr( &aAttrTab.pFontHeightCJK, aFontHeight, pCntxt );
		aFontHeight.SetWhich( RES_CHRATR_CTL_FONTSIZE );
		InsertAttr( &aAttrTab.pFontHeightCTL, aFontHeight, pCntxt );
	}

	// memorize context
	PushContext( pCntxt );

	// memorize Font-Size
	aBaseFontStack.Insert( nSize, aBaseFontStack.Count() );
}

void SwHTMLParser::EndBasefontAttr()
{
	EndTag( HTML_BASEFONT_ON );

	// Avoid stack underflow in tables
	if( aBaseFontStack.Count() > nBaseFontStMin )
		aBaseFontStack.Remove( aBaseFontStack.Count()-1, 1 );
}

void SwHTMLParser::NewFontAttr( int nToken )
{
	sal_uInt16 nBaseSize =
		( aBaseFontStack.Count() > nBaseFontStMin
			? (aBaseFontStack[aBaseFontStack.Count()-1] & FONTSIZE_MASK)
			: 3 );
	sal_uInt16 nFontSize =
		( aFontStack.Count() > nFontStMin
			? (aFontStack[aFontStack.Count()-1] & FONTSIZE_MASK)
			: nBaseSize );

	String aFace, aId, aStyle, aClass, aLang, aDir;
	Color aColor;
	sal_uLong nFontHeight = 0;	// real set up font hight
	sal_uInt16 nSize = 0;		// Font size in Netscape-Notation (1-7)
	sal_Bool bColor = sal_False;

	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		switch( pOption->GetToken() )
		{
		case HTML_O_SIZE:
			if( HTML_FONT_ON==nToken && pOption->GetString().Len() )
			{
				sal_Int32 nSSize;
				if( '+' == pOption->GetString().GetChar(0) ||
					'-' == pOption->GetString().GetChar(0) )
					nSSize = nBaseSize + pOption->GetSNumber();
				else
					nSSize = (sal_Int32)pOption->GetNumber();

				if( nSSize < 1 )
					nSSize = 1;
				else if( nSSize > 7 )
					nSSize = 7;

				nSize = (sal_uInt16)nSSize;
				nFontHeight = aFontHeights[nSize-1];
			}
			break;
		case HTML_O_COLOR:
			if( HTML_FONT_ON==nToken )
			{
				pOption->GetColor( aColor );
				bColor = sal_True;
			}
			break;
		case HTML_O_FACE:
			if( HTML_FONT_ON==nToken )
				aFace = pOption->GetString();
			break;
		case HTML_O_ID:
			aId = pOption->GetString();
			break;
		case HTML_O_STYLE:
			aStyle = pOption->GetString();
			break;
		case HTML_O_CLASS:
			aClass = pOption->GetString();
			break;
		case HTML_O_LANG:
			aLang = pOption->GetString();
			break;
		case HTML_O_DIR:
			aDir = pOption->GetString();
			break;
		}
	}

	if( HTML_FONT_ON != nToken )
	{
		// HTML_BIGPRINT_ON or HTML_SMALLPRINT_ON

		// in headings, the current heading determines
		// the font height and not BASEFONT
		sal_uInt16 nPoolId = GetCurrFmtColl()->GetPoolFmtId();
		if( (nPoolId>=RES_POOLCOLL_HEADLINE1 &&
			 nPoolId<=RES_POOLCOLL_HEADLINE6) )
		{
			// if the font size in the heading is not yet changed
			// is not changed, take the one from the template.
			if( nFontStHeadStart==aFontStack.Count() )
                nFontSize = static_cast< sal_uInt16 >(6 - (nPoolId - RES_POOLCOLL_HEADLINE1));
		}
		else
			nPoolId = 0;

		if( HTML_BIGPRINT_ON == nToken )
			nSize = ( nFontSize<7 ? nFontSize+1 : 7 );
		else
			nSize = ( nFontSize>1 ? nFontSize-1 : 1 );

		// in headlines, the new font height will be taken from the
		// from the templates.
		if( nPoolId && nSize>=1 && nSize <=6 )
			nFontHeight =
				pCSS1Parser->GetTxtCollFromPool(
					RES_POOLCOLL_HEADLINE1+6-nSize )->GetSize().GetHeight();
		else
			nFontHeight = aFontHeights[nSize-1];
	}

	ASSERT( !nSize == !nFontHeight, "HTML-Font-Size != Font-Height" );

	String aFontName, aStyleName;
	FontFamily eFamily = FAMILY_DONTKNOW;	// Family and Pitch,
	FontPitch ePitch = PITCH_DONTKNOW;		// in case not found
	rtl_TextEncoding eEnc = gsl_getSystemTextEncoding();

	if( aFace.Len() && !pCSS1Parser->IsIgnoreFontFamily() )
	{
		const FontList *pFList = 0;
		SwDocShell *pDocSh = pDoc->GetDocShell();
		if( pDocSh )
		{
			const SvxFontListItem *pFListItem =
			   (const SvxFontListItem *)pDocSh->GetItem(SID_ATTR_CHAR_FONTLIST);
			if( pFListItem )
				pFList = pFListItem->GetFontList();
		}

		sal_Bool bFound = sal_False;
		xub_StrLen nStrPos = 0;
		while( nStrPos!=STRING_NOTFOUND )
		{
			String aFName = aFace.GetToken( 0, ',', nStrPos );
			aFName.EraseTrailingChars().EraseLeadingChars();
			if( aFName.Len() )
			{
				if( !bFound && pFList )
				{
					sal_Handle hFont = pFList->GetFirstFontInfo( aFName );
					if( 0 != hFont )
					{
						const FontInfo& rFInfo = pFList->GetFontInfo( hFont );
						if( RTL_TEXTENCODING_DONTKNOW != rFInfo.GetCharSet() )
						{
							bFound = sal_True;
							if( RTL_TEXTENCODING_SYMBOL == rFInfo.GetCharSet() )
								eEnc = RTL_TEXTENCODING_SYMBOL;
						}
					}
				}
				if( aFontName.Len() )
					aFontName += ';';
				aFontName += aFName;
			}
		}
	}


	// set up a new context
    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( static_cast< sal_uInt16 >(nToken) );

	// parse styles
	if( HasStyleOptions( aStyle, aId, aClass, &aLang, &aDir ) )
	{
		SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
		SvxCSS1PropertyInfo aPropInfo;

		if( nFontHeight )
		{
            SvxFontHeightItem aFontHeight( nFontHeight, 100, RES_CHRATR_FONTSIZE );
			aItemSet.Put( aFontHeight );
			aFontHeight.SetWhich( RES_CHRATR_CJK_FONTSIZE );
			aItemSet.Put( aFontHeight );
			aFontHeight.SetWhich( RES_CHRATR_CTL_FONTSIZE );
			aItemSet.Put( aFontHeight );
		}
		if( bColor )
            aItemSet.Put( SvxColorItem(aColor, RES_CHRATR_COLOR) );
		if( aFontName.Len() )
		{
            SvxFontItem aFont( eFamily, aFontName, aStyleName, ePitch, eEnc, RES_CHRATR_FONT );
			aItemSet.Put( aFont );
			aFont.SetWhich( RES_CHRATR_CJK_FONT );
			aItemSet.Put( aFont );
			aFont.SetWhich( RES_CHRATR_CTL_FONT );
			aItemSet.Put( aFont );
		}


		if( ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo, &aLang, &aDir ) )
			DoPositioning( aItemSet, aPropInfo, pCntxt );

		InsertAttrs( aItemSet, aPropInfo, pCntxt, sal_True );
	}
	else
	{
		if( nFontHeight )
		{
            SvxFontHeightItem aFontHeight( nFontHeight, 100, RES_CHRATR_FONTSIZE );
			InsertAttr( &aAttrTab.pFontHeight, aFontHeight, pCntxt );
			aFontHeight.SetWhich( RES_CHRATR_CJK_FONTSIZE );
			InsertAttr( &aAttrTab.pFontHeightCJK, aFontHeight, pCntxt );
			aFontHeight.SetWhich( RES_CHRATR_CTL_FONTSIZE );
			InsertAttr( &aAttrTab.pFontHeightCTL, aFontHeight, pCntxt );
		}
		if( bColor )
            InsertAttr( &aAttrTab.pFontColor, SvxColorItem(aColor, RES_CHRATR_COLOR), pCntxt );
		if( aFontName.Len() )
		{
            SvxFontItem aFont( eFamily, aFontName, aStyleName, ePitch, eEnc, RES_CHRATR_FONT );
			InsertAttr( &aAttrTab.pFont, aFont, pCntxt );
			aFont.SetWhich( RES_CHRATR_CJK_FONT );
			InsertAttr( &aAttrTab.pFontCJK, aFont, pCntxt );
			aFont.SetWhich( RES_CHRATR_CTL_FONT );
			InsertAttr( &aAttrTab.pFontCTL, aFont, pCntxt );
		}
	}

	// memorize context
	PushContext( pCntxt );

	aFontStack.Insert( nSize, aFontStack.Count() );
}

void SwHTMLParser::EndFontAttr( int nToken )
{
	EndTag( nToken );

	// Avoid stack underflow in tables
	if( aFontStack.Count() > nFontStMin )
		aFontStack.Remove( aFontStack.Count()-1, 1 );
}

/*  */

void SwHTMLParser::NewPara()
{
	if( pPam->GetPoint()->nContent.GetIndex() )
		AppendTxtNode( AM_SPACE );
	else
		AddParSpace();

	eParaAdjust = SVX_ADJUST_END;
	String aId, aStyle, aClass, aLang, aDir;

	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		switch( pOption->GetToken() )
		{
			case HTML_O_ID:
				aId = pOption->GetString();
				break;
			case HTML_O_ALIGN:
                eParaAdjust = (SvxAdjust)pOption->GetEnum( aHTMLPAlignTable, static_cast< sal_uInt16 >(eParaAdjust) );
				break;
			case HTML_O_STYLE:
				aStyle = pOption->GetString();
				break;
			case HTML_O_CLASS:
				aClass = pOption->GetString();
				break;
			case HTML_O_LANG:
				aLang = pOption->GetString();
				break;
			case HTML_O_DIR:
				aDir = pOption->GetString();
				break;
		}
	}

	// set up a new context
	_HTMLAttrContext *pCntxt =
		aClass.Len() ? new _HTMLAttrContext( HTML_PARABREAK_ON,
											 RES_POOLCOLL_TEXT, aClass )
					 : new _HTMLAttrContext( HTML_PARABREAK_ON );

	// parse styles (ignore Class. This only works as long as
	// none of the CSS1 properties of the class need to be hard formatted!!!)
	if( HasStyleOptions( aStyle, aId, aEmptyStr, &aLang, &aDir ) )
	{
		SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
		SvxCSS1PropertyInfo aPropInfo;

		if( ParseStyleOptions( aStyle, aId, aEmptyStr, aItemSet, aPropInfo, &aLang, &aDir ) )
		{
			ASSERT( !aClass.Len() || !pCSS1Parser->GetClass( aClass ),
					"Class will not be considered" );
			DoPositioning( aItemSet, aPropInfo, pCntxt );
			InsertAttrs( aItemSet, aPropInfo, pCntxt );
		}
	}

	if( SVX_ADJUST_END != eParaAdjust )
        InsertAttr( &aAttrTab.pAdjust, SvxAdjustItem(eParaAdjust, RES_PARATR_ADJUST), pCntxt );

	// push on stack
	PushContext( pCntxt );

	// set the current template or its attributes
	SetTxtCollAttrs( aClass.Len() ? pCntxt : 0 );

	// progress bar
	ShowStatline();

	ASSERT( !nOpenParaToken, "Now an open paragraph element is lost" );
	nOpenParaToken = HTML_PARABREAK_ON;
}

void SwHTMLParser::EndPara( sal_Bool bReal )
{
	if( HTML_LI_ON==nOpenParaToken && pTable )
	{
#ifdef DBG_UTIL
		const SwNumRule *pNumRule = pPam->GetNode()->GetTxtNode()->GetNumRule();
#endif
		ASSERT( pNumRule, "Where has the Numrule gone" );
	}

	// empty paragraphs are skipped by Netscape, now also by us
	if( bReal )
	{
		if( pPam->GetPoint()->nContent.GetIndex() )
			AppendTxtNode( AM_SPACE );
		else
			AddParSpace();
	}

	// if a DD or DT was open, it is an
	// implicit def list that must now be terminated.
	if( (nOpenParaToken==HTML_DT_ON || nOpenParaToken==HTML_DD_ON) &&
		nDefListDeep)
	{
		nDefListDeep--;
	}

	// get the context from the stack. It can also come from an implicitly
	// open definition list.
	_HTMLAttrContext *pCntxt =
        PopContext( static_cast< sal_uInt16 >(nOpenParaToken ? (nOpenParaToken & ~1)
                                   : HTML_PARABREAK_ON) );

	// Attribute beenden
	if( pCntxt )
	{
		EndContext( pCntxt );
		SetAttr();	// set paragraph-atts as fast as possible because of JavaScript
		delete pCntxt;
	}

	// and reset previous template
	if( bReal )
		SetTxtCollAttrs();

	nOpenParaToken = 0;
}


void SwHTMLParser::NewHeading( int nToken )
{
	eParaAdjust = SVX_ADJUST_END;

	String aId, aStyle, aClass, aLang, aDir;

	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		switch( pOption->GetToken() )
		{
			case HTML_O_ID:
				aId = pOption->GetString();
				break;
			case HTML_O_ALIGN:
                eParaAdjust = (SvxAdjust)pOption->GetEnum( aHTMLPAlignTable, static_cast< sal_uInt16 >(eParaAdjust) );
				break;
			case HTML_O_STYLE:
				aStyle = pOption->GetString();
				break;
			case HTML_O_CLASS:
				aClass = pOption->GetString();
				break;
			case HTML_O_LANG:
				aLang = pOption->GetString();
				break;
			case HTML_O_DIR:
				aDir = pOption->GetString();
				break;
		}
	}

	// create new paragraph
	if( pPam->GetPoint()->nContent.GetIndex() )
		AppendTxtNode( AM_SPACE );
	else
		AddParSpace();

	// search matching template
	sal_uInt16 nTxtColl;
	switch( nToken )
	{
	case HTML_HEAD1_ON: 		nTxtColl = RES_POOLCOLL_HEADLINE1;	break;
	case HTML_HEAD2_ON: 		nTxtColl = RES_POOLCOLL_HEADLINE2;	break;
	case HTML_HEAD3_ON: 		nTxtColl = RES_POOLCOLL_HEADLINE3;	break;
	case HTML_HEAD4_ON: 		nTxtColl = RES_POOLCOLL_HEADLINE4;	break;
	case HTML_HEAD5_ON: 		nTxtColl = RES_POOLCOLL_HEADLINE5;	break;
	case HTML_HEAD6_ON: 		nTxtColl = RES_POOLCOLL_HEADLINE6;	break;
	default:					nTxtColl = RES_POOLCOLL_STANDARD;	break;
	}

	// set context
    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( static_cast< sal_uInt16 >(nToken), nTxtColl, aClass );

	// parse styles (to Class see NewPara)
	if( HasStyleOptions( aStyle, aId, aEmptyStr, &aLang, &aDir ) )
	{
		SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
		SvxCSS1PropertyInfo aPropInfo;

		if( ParseStyleOptions( aStyle, aId, aEmptyStr, aItemSet, aPropInfo, &aLang, &aDir ) )
		{
			ASSERT( !aClass.Len() || !pCSS1Parser->GetClass( aClass ),
					"Class will not be considered" );
			DoPositioning( aItemSet, aPropInfo, pCntxt );
			InsertAttrs( aItemSet, aPropInfo, pCntxt );
		}
	}

	if( SVX_ADJUST_END != eParaAdjust )
        InsertAttr( &aAttrTab.pAdjust, SvxAdjustItem(eParaAdjust, RES_PARATR_ADJUST), pCntxt );

	// push to Stack
	PushContext( pCntxt );

	// and set the template or its attributes
	SetTxtCollAttrs( pCntxt );

	nFontStHeadStart = aFontStack.Count();

	// progress bar
	ShowStatline();
}

void SwHTMLParser::EndHeading()
{
    // create a new paragraph
	if( pPam->GetPoint()->nContent.GetIndex() )
		AppendTxtNode( AM_SPACE );
	else
		AddParSpace();

	// get context of current token from stack
	_HTMLAttrContext *pCntxt = 0;
	sal_uInt16 nPos = aContexts.Count();
	while( !pCntxt && nPos>nContextStMin )
	{
		switch( aContexts[--nPos]->GetToken() )
		{
		case HTML_HEAD1_ON:
		case HTML_HEAD2_ON:
		case HTML_HEAD3_ON:
		case HTML_HEAD4_ON:
		case HTML_HEAD5_ON:
		case HTML_HEAD6_ON:
			pCntxt = aContexts[nPos];
			aContexts.Remove( nPos, 1 );
			break;
		}
	}

	// and set attributes end
	if( pCntxt )
	{
		EndContext( pCntxt );
		SetAttr();	// set paragraph-atts as fast as possible because of JavaScript
		delete pCntxt;
	}

	// reset the previous template
	SetTxtCollAttrs();

	nFontStHeadStart = nFontStMin;
}

/*  */

void SwHTMLParser::NewTxtFmtColl( int nToken, sal_uInt16 nColl )
{
	String aId, aStyle, aClass, aLang, aDir;

	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		switch( pOption->GetToken() )
		{
			case HTML_O_ID:
				aId = pOption->GetString();
				break;
			case HTML_O_STYLE:
				aStyle = pOption->GetString();
				break;
			case HTML_O_CLASS:
				aClass = pOption->GetString();
				break;
			case HTML_O_LANG:
				aLang = pOption->GetString();
				break;
			case HTML_O_DIR:
				aDir = pOption->GetString();
				break;
		}
	}

	// create new paragraph
	SwHTMLAppendMode eMode = AM_NORMAL;
	switch( nToken )
	{
	case HTML_LISTING_ON:
	case HTML_XMP_ON:
		// These two tags are now mapped to the PRE template.
		// In the case that a CLASS is specified, we delete it
		// it so we don't get the CLASS of the PRE template.
		aClass = aEmptyStr;
	case HTML_BLOCKQUOTE_ON:
	case HTML_BLOCKQUOTE30_ON:
	case HTML_PREFORMTXT_ON:
		eMode = AM_SPACE;
		break;
	case HTML_ADDRESS_ON:
		eMode = AM_NOSPACE;	// ADDRESS may follow a <P> without </P>.
		break;
	case HTML_DT_ON:
	case HTML_DD_ON:
		eMode = AM_SOFTNOSPACE;
		break;
	default:
		ASSERT( sal_False, "unknown template" );
		break;
	}
	if( pPam->GetPoint()->nContent.GetIndex() )
		AppendTxtNode( eMode );
	else if( AM_SPACE==eMode )
		AddParSpace();

	// ... and remember in a context
    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( static_cast< sal_uInt16 >(nToken), nColl, aClass );

	// parse styles (to Class see NewPara)
	if( HasStyleOptions( aStyle, aId, aEmptyStr, &aLang, &aDir ) )
	{
		SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
		SvxCSS1PropertyInfo aPropInfo;

		if( ParseStyleOptions( aStyle, aId, aEmptyStr, aItemSet, aPropInfo, &aLang, &aDir ) )
		{
			ASSERT( !aClass.Len() || !pCSS1Parser->GetClass( aClass ),
					"Class will not be considered" );
			DoPositioning( aItemSet, aPropInfo, pCntxt );
			InsertAttrs( aItemSet, aPropInfo, pCntxt );
		}
	}

	PushContext( pCntxt );

	// set the new template
	SetTxtCollAttrs( pCntxt );

	// update progress bar
	ShowStatline();
}

void SwHTMLParser::EndTxtFmtColl( int nToken )
{
	SwHTMLAppendMode eMode = AM_NORMAL;
	switch( nToken & ~1 )
	{
	case HTML_BLOCKQUOTE_ON:
	case HTML_BLOCKQUOTE30_ON:
	case HTML_PREFORMTXT_ON:
	case HTML_LISTING_ON:
	case HTML_XMP_ON:
		eMode = AM_SPACE;
		break;
	case HTML_ADDRESS_ON:
	case HTML_DT_ON:
	case HTML_DD_ON:
		eMode = AM_SOFTNOSPACE;
		break;
	default:
		ASSERT( sal_False, "unknown template" );
		break;
	}
	if( pPam->GetPoint()->nContent.GetIndex() )
		AppendTxtNode( eMode );
	else if( AM_SPACE==eMode )
		AddParSpace();

	// get the current context from the stack
    _HTMLAttrContext *pCntxt = PopContext( static_cast< sal_uInt16 >(nToken & ~1) );

	// and close end Attribute
	if( pCntxt )
	{
		EndContext( pCntxt );
		SetAttr();	// set paragraph-atts as fast as possible because of JavaScript
		delete pCntxt;
	}

	// and set the previous template
	SetTxtCollAttrs();
}

/*  */

void SwHTMLParser::NewDefList()
{
	String aId, aStyle, aClass, aLang, aDir;

	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		switch( pOption->GetToken() )
		{
			case HTML_O_ID:
				aId = pOption->GetString();
				break;
			case HTML_O_STYLE:
				aStyle = pOption->GetString();
				break;
			case HTML_O_CLASS:
				aClass = pOption->GetString();
				break;
			case HTML_O_LANG:
				aLang = pOption->GetString();
				break;
			case HTML_O_DIR:
				aDir = pOption->GetString();
				break;
		}
	}

	// create new paragraph
	sal_Bool bSpace = (GetNumInfo().GetDepth() + nDefListDeep) == 0;
	if( pPam->GetPoint()->nContent.GetIndex() )
		AppendTxtNode( bSpace ? AM_SPACE : AM_SOFTNOSPACE );
	else if( bSpace )
		AddParSpace();

	// one more level
	nDefListDeep++;


	sal_Bool bInDD = sal_False, bNotInDD = sal_False;
	sal_uInt16 nPos = aContexts.Count();
	while( !bInDD && !bNotInDD && nPos>nContextStMin )
	{
		sal_uInt16 nCntxtToken = aContexts[--nPos]->GetToken();
		switch( nCntxtToken )
		{
		case HTML_DEFLIST_ON:
		case HTML_DIRLIST_ON:
		case HTML_MENULIST_ON:
		case HTML_ORDERLIST_ON:
		case HTML_UNORDERLIST_ON:
			bNotInDD = sal_True;
			break;
		case HTML_DD_ON:
			bInDD = sal_True;
			break;
		}
	}


	// ... and remember in a context
	_HTMLAttrContext *pCntxt = new _HTMLAttrContext( HTML_DEFLIST_ON );

	// remeber the borders, too
	sal_uInt16 nLeft=0, nRight=0;
	short nIndent=0;
	GetMarginsFromContext( nLeft, nRight, nIndent );

	// The intend, which already results from a DL-, corresponds to that
	// of a DT on the current level, and that corresponds to that of a
	// DD on the level before. So for a level >=2 a DD distance must be added
	if( !bInDD && nDefListDeep > 1 )
	{

		// and that of the DT template of the current level
		SvxLRSpaceItem rLRSpace =
			pCSS1Parser->GetTxtFmtColl( RES_POOLCOLL_HTML_DD, aEmptyStr )
					   ->GetLRSpace();
        nLeft = nLeft + static_cast< sal_uInt16 >(rLRSpace.GetTxtLeft());
	}

	pCntxt->SetMargins( nLeft, nRight, nIndent );

	// parse styles
	if( HasStyleOptions( aStyle, aId, aClass, &aLang, &aDir ) )
	{
		SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
		SvxCSS1PropertyInfo aPropInfo;

		if( ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo, &aLang, &aDir ) )
		{
			DoPositioning( aItemSet, aPropInfo, pCntxt );
			InsertAttrs( aItemSet, aPropInfo, pCntxt );
		}
	}

	PushContext( pCntxt );

	// set the attributes of the new template
	if( nDefListDeep > 1 )
		SetTxtCollAttrs( pCntxt );
}

void SwHTMLParser::EndDefList()
{
	sal_Bool bSpace = (GetNumInfo().GetDepth() + nDefListDeep) == 1;
	if( pPam->GetPoint()->nContent.GetIndex() )
		AppendTxtNode( bSpace ? AM_SPACE : AM_SOFTNOSPACE );
	else if( bSpace )
		AddParSpace();

	// reduce one level
	if( nDefListDeep > 0 )
		nDefListDeep--;

	// get the current context from the stack
	_HTMLAttrContext *pCntxt = PopContext( HTML_DEFLIST_ON );

	// and close end Attribute
	if( pCntxt )
	{
		EndContext( pCntxt );
		SetAttr();	// set paragraph-atts as fast as possible because of JavaScript
		delete pCntxt;
	}

	// and set Template
	SetTxtCollAttrs();
}

void SwHTMLParser::NewDefListItem( int nToken )
{
	// determine whether the DD/DT occurs in a DL
	sal_Bool bInDefList = sal_False, bNotInDefList = sal_False;
	sal_uInt16 nPos = aContexts.Count();
	while( !bInDefList && !bNotInDefList && nPos>nContextStMin )
	{
		sal_uInt16 nCntxtToken = aContexts[--nPos]->GetToken();
		switch( nCntxtToken )
		{
		case HTML_DEFLIST_ON:
			bInDefList = sal_True;
			break;
		case HTML_DIRLIST_ON:
		case HTML_MENULIST_ON:
		case HTML_ORDERLIST_ON:
		case HTML_UNORDERLIST_ON:
			bNotInDefList = sal_True;
			break;
		}
	}

	// if not, implicitly open a new DL
	if( !bInDefList )
	{
		nDefListDeep++;
		ASSERT( !nOpenParaToken,
				"Now an open paragraph element is lost" );
        nOpenParaToken = static_cast< sal_uInt16 >(nToken);
	}

    NewTxtFmtColl( nToken, static_cast< sal_uInt16 >(nToken==HTML_DD_ON ? RES_POOLCOLL_HTML_DD
                                              : RES_POOLCOLL_HTML_DT) );
}

void SwHTMLParser::EndDefListItem( int nToken, sal_Bool bSetColl,
								   sal_Bool /*bLastPara*/ )
{
	// create new paragraph
	if( !nToken && pPam->GetPoint()->nContent.GetIndex() )
		AppendTxtNode( AM_SOFTNOSPACE );

	// Search context to the token and get it from the stack
	nToken &= ~1;
	_HTMLAttrContext *pCntxt = 0;
	sal_uInt16 nPos = aContexts.Count();
	while( !pCntxt && nPos>nContextStMin )
	{
		sal_uInt16 nCntxtToken = aContexts[--nPos]->GetToken();
		switch( nCntxtToken )
		{
		case HTML_DD_ON:
		case HTML_DT_ON:
			if( !nToken || nToken == nCntxtToken  )
			{
				pCntxt = aContexts[nPos];
				aContexts.Remove( nPos, 1 );
			}
			break;
		case HTML_DEFLIST_ON:
			// do not consider DD/DT outside the current def list
		case HTML_DIRLIST_ON:
		case HTML_MENULIST_ON:
		case HTML_ORDERLIST_ON:
		case HTML_UNORDERLIST_ON:
			// and also not outside of another list
			nPos = nContextStMin;
			break;
		}
	}

	// and close end Attribute
	if( pCntxt )
	{
		EndContext( pCntxt );
		SetAttr();	// set paragraph-atts as fast as possible because of JavaScript
		delete pCntxt;
	}

	// and set the previous template
	if( bSetColl )
		SetTxtCollAttrs();
}

/*  */

sal_Bool SwHTMLParser::HasCurrentParaFlys( sal_Bool bNoSurroundOnly,
									   sal_Bool bSurroundOnly ) const
{
	// bNoSurroundOnly: The paragraph contains at least one frame
	//                  without text flow
	// bSurroundOnly:   The paragraph contains at least one frame
	//                  with a text flow but none without a text flow
	// else:            the paragraph contains some frame
	SwNodeIndex& rNodeIdx = pPam->GetPoint()->nNode;

	const SwSpzFrmFmts& rFrmFmtTbl = *pDoc->GetSpzFrmFmts();

	sal_Bool bFound = sal_False;
    for ( sal_uInt16 i=0; i<rFrmFmtTbl.Count(); i++ )
    {
        SwFrmFmt *const pFmt = rFrmFmtTbl[i];
        SwFmtAnchor const*const pAnchor = &pFmt->GetAnchor();
		// A frame has been found if
		// - it is paragraph-bound, and
		// - it is anchored in the current paragraph, and
		// - every paragraph-bound frame counts, or
		// - (only frames or text flow counts) the frame has no
		//   text flow
        SwPosition const*const pAPos = pAnchor->GetCntntAnchor();
        if (pAPos &&
            ((FLY_AT_PARA == pAnchor->GetAnchorId()) ||
             (FLY_AT_CHAR == pAnchor->GetAnchorId())) &&
			pAPos->nNode == rNodeIdx )
		{
			if( !(bNoSurroundOnly || bSurroundOnly) )
			{
				bFound = sal_True;
				break;
			}
			else
			{
				// fix #42282#: If frames with circulation are wanted,
				// do not consider any with circulation either. These are
				// are (still) HIDDEN controls, and it's better not to // avoid them.
				// better not to avoid them.
				SwSurround eSurround = pFmt->GetSurround().GetSurround();
				if( bNoSurroundOnly )
				{
					if( SURROUND_NONE==eSurround )
					{
						bFound = sal_True;
						break;
					}
				}
				if( bSurroundOnly )
				{
					if( SURROUND_NONE==eSurround )
					{
						bFound = sal_False;
						break;
					}
					else if( SURROUND_THROUGHT!=eSurround )
					{
						bFound = sal_True;
						// continue searching: There might still be some without
						// circulation ...
					}
				}
			}
		}
	}

	return bFound;
}

/*  */

// the special methods for inserting objects

const SwFmtColl *SwHTMLParser::GetCurrFmtColl() const
{
	const SwCntntNode* pCNd = pPam->GetCntntNode();
	return &pCNd->GetAnyFmtColl();
}


void SwHTMLParser::SetTxtCollAttrs( _HTMLAttrContext *pContext )
{
	SwTxtFmtColl *pCollToSet = 0;	// the template to be set
	SfxItemSet *pItemSet = 0;		// the set for hard attributes
	sal_uInt16 nTopColl = pContext ? pContext->GetTxtFmtColl() : 0;
	const String& rTopClass = pContext ? pContext->GetClass() : (const String&) aEmptyStr;
	sal_uInt16 nDfltColl = RES_POOLCOLL_TEXT;

	sal_Bool bInPRE=sal_False;							// some context info

	sal_uInt16 nLeftMargin = 0, nRightMargin = 0;	    // the margins and
	short nFirstLineIndent = 0;					        // Intend
	sal_uInt16 i;

	for( i = nContextStAttrMin; i < aContexts.Count(); i++ )
	{
		const _HTMLAttrContext *pCntxt = aContexts[i];

		sal_uInt16 nColl = pCntxt->GetTxtFmtColl();
		if( nColl )
		{
			// There is a template to be set. Then
			// a decision has to be made first,
			// whether the template can also be set
			sal_Bool bSetThis = sal_True;
			switch( nColl )
			{
			case sal_uInt16(RES_POOLCOLL_HTML_PRE):
				bInPRE = sal_True;
				break;
			case sal_uInt16(RES_POOLCOLL_TEXT):
				// <TD><P CLASS=xxx> must become TD.xxx
				if( nDfltColl==RES_POOLCOLL_TABLE ||
					nDfltColl==RES_POOLCOLL_TABLE_HDLN )
					nColl = nDfltColl;
				break;
			case sal_uInt16(RES_POOLCOLL_HTML_HR):
				// <HR> also set in <PRE> as template, otherwise you can't
				// no longer export
				break;
			default:
				if( bInPRE )
					bSetThis = sal_False;
				break;
			}

			SwTxtFmtColl *pNewColl =
				pCSS1Parser->GetTxtFmtColl( nColl, pCntxt->GetClass() );

			if( bSetThis )
			{
				// if now another template should be set than
				// previously, the previous template must be replaced by hard attributes
				// must be replaced
				if( pCollToSet )
				{
					// the attributes set by the previous template
					// insert hard
					if( !pItemSet )
						pItemSet = new SfxItemSet( pCollToSet->GetAttrSet() );
					else
					{
						const SfxItemSet& rCollSet = pCollToSet->GetAttrSet();
						SfxItemSet aItemSet( *rCollSet.GetPool(),
											 rCollSet.GetRanges() );
						aItemSet.Set( rCollSet );
						pItemSet->Put( aItemSet );
					}
					// but remove the attributes that the current template sets
					// because otherwise they will be overwritten later.
					pItemSet->Differentiate( pNewColl->GetAttrSet() );
				}

				pCollToSet = pNewColl;
			}
			else
			{
				// set attributes hard
				if( !pItemSet )
					pItemSet = new SfxItemSet( pNewColl->GetAttrSet() );
				else
				{
					const SfxItemSet& rCollSet = pNewColl->GetAttrSet();
					SfxItemSet aItemSet( *rCollSet.GetPool(),
										 rCollSet.GetRanges() );
					aItemSet.Set( rCollSet );
					pItemSet->Put( aItemSet );
				}
			}
		}
		else
		{
			// Maybe there is a default template?
			nColl = pCntxt->GetDfltTxtFmtColl();
			if( nColl )
				nDfltColl = nColl;
		}

		// get new paragraph inserts if necessary
		if( pCntxt->IsLRSpaceChanged() )
		{
			sal_uInt16 nLeft=0, nRight=0;

			pCntxt->GetMargins( nLeft, nRight, nFirstLineIndent );
			nLeftMargin = nLeft;
			nRightMargin = nRight;
		}
	}

	// if a new template is to be set in the current context,
	// its paragraph spacing must still be entered into the context.
	if( pContext && nTopColl )
	{
	    // <TD><P CLASS=xxx> must become TD.xxx
		if( nTopColl==RES_POOLCOLL_TEXT &&
			(nDfltColl==RES_POOLCOLL_TABLE ||
			 nDfltColl==RES_POOLCOLL_TABLE_HDLN) )
			nTopColl = nDfltColl;

		const SwTxtFmtColl *pTopColl =
			pCSS1Parser->GetTxtFmtColl( nTopColl, rTopClass );
		const SfxItemSet& rItemSet = pTopColl->GetAttrSet();
		const SfxPoolItem *pItem;
		if( SFX_ITEM_SET == rItemSet.GetItemState(RES_LR_SPACE,sal_True, &pItem) )
		{
			const SvxLRSpaceItem *pLRItem =
				(const SvxLRSpaceItem *)pItem;

			sal_Int32 nLeft = pLRItem->GetTxtLeft();
			sal_Int32 nRight = pLRItem->GetRight();
			nFirstLineIndent = pLRItem->GetTxtFirstLineOfst();

			// In definition lists, the distances also include those of the
			// previous level
			if( RES_POOLCOLL_HTML_DD == nTopColl )
			{
				const SvxLRSpaceItem& rDTLRSpace = pCSS1Parser
					->GetTxtFmtColl( RES_POOLCOLL_HTML_DT, aEmptyStr )
					->GetLRSpace();
				nLeft -= rDTLRSpace.GetTxtLeft();
				nRight -= rDTLRSpace.GetRight();
			}
			else if( RES_POOLCOLL_HTML_DT == nTopColl )
			{
				nLeft = 0;
				nRight = 0;
			}

			// the paragraph distances add up
            nLeftMargin = nLeftMargin + static_cast< sal_uInt16 >(nLeft);
            nRightMargin = nRightMargin + static_cast< sal_uInt16 >(nRight);

			pContext->SetMargins( nLeftMargin, nRightMargin,
								  nFirstLineIndent );
		}
		if( SFX_ITEM_SET == rItemSet.GetItemState(RES_UL_SPACE,sal_True, &pItem) )
		{
			const SvxULSpaceItem *pULItem =
				(const SvxULSpaceItem *)pItem;
			pContext->SetULSpace( pULItem->GetUpper(), pULItem->GetLower() );
		}
	}

	// if no template is set in context, take text body
	if( !pCollToSet )
	{
		pCollToSet = pCSS1Parser->GetTxtCollFromPool( nDfltColl );
		const SvxLRSpaceItem& rLRItem = pCollToSet->GetLRSpace();
		if( !nLeftMargin )
            nLeftMargin = static_cast< sal_uInt16 >(rLRItem.GetTxtLeft());
		if( !nRightMargin )
            nRightMargin = static_cast< sal_uInt16 >(rLRItem.GetRight());
		if( !nFirstLineIndent )
			nFirstLineIndent = rLRItem.GetTxtFirstLineOfst();
	}

	// remove previous hard attribution of the paragraph
	if( aParaAttrs.Count() )
	{
		for( i=0; i<aParaAttrs.Count(); i++ )
			aParaAttrs[i]->Invalidate();

		aParaAttrs.Remove( 0, aParaAttrs.Count() );
	}

	// Set the template
	pDoc->SetTxtFmtColl( *pPam, pCollToSet );

	// correct the paragraph indentation if necessary
	const SvxLRSpaceItem& rLRItem = pCollToSet->GetLRSpace();
	sal_Bool bSetLRSpace;

       	bSetLRSpace = nLeftMargin != rLRItem.GetTxtLeft() ||
					  nFirstLineIndent != rLRItem.GetTxtFirstLineOfst() ||
					  nRightMargin != rLRItem.GetRight();

	if( bSetLRSpace )
	{
		SvxLRSpaceItem aLRItem( rLRItem );
		aLRItem.SetTxtLeft( nLeftMargin );
		aLRItem.SetRight( nRightMargin );
		aLRItem.SetTxtFirstLineOfst( nFirstLineIndent );
		if( pItemSet )
			pItemSet->Put( aLRItem );
		else
		{
			NewAttr( &aAttrTab.pLRSpace, aLRItem );
			aAttrTab.pLRSpace->SetLikePara();
			aParaAttrs.Insert( aAttrTab.pLRSpace, aParaAttrs.Count() );
			EndAttr( aAttrTab.pLRSpace, 0, sal_False );
		}
	}

	// and now set the attributes
	if( pItemSet )
	{
		InsertParaAttrs( *pItemSet );
		delete pItemSet;
	}
}

/*  */

void SwHTMLParser::NewCharFmt( int nToken )
{
	String aId, aStyle, aClass, aLang, aDir;

	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		switch( pOption->GetToken() )
		{
		case HTML_O_ID:
			aId = pOption->GetString();
			break;
		case HTML_O_STYLE:
			aStyle = pOption->GetString();
			break;
		case HTML_O_CLASS:
			aClass = pOption->GetString();
			break;
		case HTML_O_LANG:
			aLang = pOption->GetString();
			break;
		case HTML_O_DIR:
			aDir = pOption->GetString();
			break;
		}
	}

	// set up a new context
    _HTMLAttrContext *pCntxt = new _HTMLAttrContext( static_cast< sal_uInt16 >(nToken) );

	// Set the template und im Kontext merken
    SwCharFmt* pCFmt = pCSS1Parser->GetChrFmt( static_cast< sal_uInt16 >(nToken), aClass );
	ASSERT( pCFmt, "no character template found for token" );


	// parse styles (to Class see NewPara)
	if( HasStyleOptions( aStyle, aId, aEmptyStr, &aLang, &aDir ) )
	{
		SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
		SvxCSS1PropertyInfo aPropInfo;

		if( ParseStyleOptions( aStyle, aId, aEmptyStr, aItemSet, aPropInfo, &aLang, &aDir ) )
		{
			ASSERT( !aClass.Len() || !pCSS1Parser->GetClass( aClass ),
					"Class wird nicht beruecksichtigt" );
			DoPositioning( aItemSet, aPropInfo, pCntxt );
			InsertAttrs( aItemSet, aPropInfo, pCntxt, sal_True );
		}
	}

	// Character templates are kept in their own stack and
	// can never be inserted by styles. The attribute is therefore
	// not included in the CSS1 Which range at all.
	if( pCFmt )
		InsertAttr( &aAttrTab.pCharFmts, SwFmtCharFmt( pCFmt ), pCntxt );

	// memorize context
	PushContext( pCntxt );
}


/*  */

void SwHTMLParser::InsertSpacer()
{
	// and change it by the options if necessary
	String aId;
    sal_Int16 eVertOri = text::VertOrientation::TOP;
    sal_Int16 eHoriOri = text::HoriOrientation::NONE;
	Size aSize( 0, 0);
	long nSize = 0;
	sal_Bool bPrcWidth = sal_False;
	sal_Bool bPrcHeight = sal_False;
	sal_uInt16 nType = HTML_SPTYPE_HORI;

	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		switch( pOption->GetToken() )
		{
		case HTML_O_ID:
			aId = pOption->GetString();
			break;
		case HTML_O_TYPE:
			pOption->GetEnum( nType, aHTMLSpacerTypeTable );
			break;
		case HTML_O_ALIGN:
			eVertOri =
                pOption->GetEnum( aHTMLImgVAlignTable,
                                  eVertOri );
			eHoriOri =
                pOption->GetEnum( aHTMLImgHAlignTable,
                                  eHoriOri );
			break;
		case HTML_O_WIDTH:
			// remember only as pixel values for now!
			bPrcWidth = (pOption->GetString().Search('%') != STRING_NOTFOUND);
			aSize.Width() = (long)pOption->GetNumber();
			break;
		case HTML_O_HEIGHT:
			// remember only as pixel values for now!
			bPrcHeight = (pOption->GetString().Search('%') != STRING_NOTFOUND);
			aSize.Height() = (long)pOption->GetNumber();
			break;
		case HTML_O_SIZE:
			// remember only as pixel values for now!
			nSize = pOption->GetNumber();
			break;
		}
	}

	switch( nType )
	{
	case HTML_SPTYPE_BLOCK:
		{
			// create an empty text frame

			// get the itemset
			SfxItemSet aFrmSet( pDoc->GetAttrPool(),
								RES_FRMATR_BEGIN, RES_FRMATR_END-1 );
			if( !IsNewDoc() )
				Reader::ResetFrmFmtAttrs( aFrmSet );

			// set the anchor and the alignment
			SetAnchorAndAdjustment( eVertOri, eHoriOri, aFrmSet );

			// also the size of the frame
			Size aDfltSz( MINFLY, MINFLY );
			Size aSpace( 0, 0 );
			SfxItemSet aDummyItemSet( pDoc->GetAttrPool(),
								 pCSS1Parser->GetWhichMap() );
			SvxCSS1PropertyInfo aDummyPropInfo;

			SetFixSize( aSize, aDfltSz, bPrcWidth, bPrcHeight,
						aDummyItemSet, aDummyPropInfo, aFrmSet );
			SetSpace( aSpace, aDummyItemSet, aDummyPropInfo, aFrmSet );

			// protect the content
            SvxProtectItem aProtectItem( RES_PROTECT) ;
			aProtectItem.SetCntntProtect( sal_True );
			aFrmSet.Put( aProtectItem );

			// create the frame
			RndStdIds eAnchorId =
				((const SwFmtAnchor &)aFrmSet.Get(RES_ANCHOR)).GetAnchorId();
			SwFrmFmt *pFlyFmt = pDoc->MakeFlySection( eAnchorId,
											pPam->GetPoint(), &aFrmSet );
			// If necessary, create frames and register auto-generated frames
			RegisterFlyFrm( pFlyFmt );
		}
		break;
	case HTML_SPTYPE_VERT:
		if( nSize > 0 )
		{
			if( nSize && Application::GetDefaultDevice() )
			{
				nSize = Application::GetDefaultDevice()
							->PixelToLogic( Size(0,nSize),
											MapMode(MAP_TWIP) ).Height();
			}

			// set a paragraph spacing
			SwTxtNode *pTxtNode = 0;
			if( !pPam->GetPoint()->nContent.GetIndex() )
			{
				// change the lower paragraph spacing of the previous node,
				// if possible

				SetAttr();	// Set more open paragraph attributes

				pTxtNode = pDoc->GetNodes()[pPam->GetPoint()->nNode.GetIndex()-1]
							   ->GetTxtNode();

				// If the paragraph in front of it is not a txttenode, then now
				// an empty paragraph is created which already creates a line height
				// spacing
				if( !pTxtNode )
					nSize = nSize>HTML_PARSPACE ? nSize-HTML_PARSPACE : 0;
			}

			if( pTxtNode )
			{
				SvxULSpaceItem aULSpace( (const SvxULSpaceItem&)pTxtNode
					->SwCntntNode::GetAttr( RES_UL_SPACE ) );
				aULSpace.SetLower( aULSpace.GetLower() + (sal_uInt16)nSize );
                pTxtNode->SetAttr( aULSpace );
			}
			else
			{
                NewAttr( &aAttrTab.pULSpace, SvxULSpaceItem( 0, (sal_uInt16)nSize, RES_UL_SPACE ) );
				EndAttr( aAttrTab.pULSpace, 0, sal_False );

				AppendTxtNode();	// do not change the distance!
			}
		}
		break;
	case HTML_SPTYPE_HORI:
		if( nSize > 0 )
		{
			// if the paragraph is still empty, set a first-line indentation
			// but span blocking font over a space.

			if( nSize && Application::GetDefaultDevice() )
			{
				nSize = Application::GetDefaultDevice()
							->PixelToLogic( Size(nSize,0),
											MapMode(MAP_TWIP) ).Width();
			}

			if( !pPam->GetPoint()->nContent.GetIndex() )
			{
				sal_uInt16 nLeft=0, nRight=0;
				short nIndent = 0;

				GetMarginsFromContextWithNumBul( nLeft, nRight, nIndent );
                nIndent = nIndent + (short)nSize;

                SvxLRSpaceItem aLRItem( RES_LR_SPACE );
				aLRItem.SetTxtLeft( nLeft );
				aLRItem.SetRight( nRight );
				aLRItem.SetTxtFirstLineOfst( nIndent );

				NewAttr( &aAttrTab.pLRSpace, aLRItem );
				EndAttr( aAttrTab.pLRSpace, 0, sal_False );
			}
			else
			{
                NewAttr( &aAttrTab.pKerning, SvxKerningItem( (short)nSize, RES_CHRATR_KERNING ) );
				String aTmp( ' ' );
                pDoc->InsertString( *pPam, aTmp );
				EndAttr( aAttrTab.pKerning );
			}
		}
	}
}

sal_uInt16 SwHTMLParser::ToTwips( sal_uInt16 nPixel ) const
{
	if( nPixel && Application::GetDefaultDevice() )
	{
		long nTwips = Application::GetDefaultDevice()->PixelToLogic(
					Size( nPixel, nPixel ), MapMode( MAP_TWIP ) ).Width();
		return nTwips <= USHRT_MAX ? (sal_uInt16)nTwips : USHRT_MAX;
	}
	else
		return nPixel;
}

SwTwips SwHTMLParser::GetCurrentBrowseWidth()
{
    const SwTwips nWidth = SwHTMLTableLayout::GetBrowseWidth( *pDoc );
	if( nWidth )
		return nWidth;

	if( !aHTMLPageSize.Width() )
	{
		const SwFrmFmt& rPgFmt = pCSS1Parser->GetMasterPageDesc()->GetMaster();

		const SwFmtFrmSize& rSz   = rPgFmt.GetFrmSize();
		const SvxLRSpaceItem& rLR = rPgFmt.GetLRSpace();
		const SvxULSpaceItem& rUL = rPgFmt.GetULSpace();
		const SwFmtCol& rCol = rPgFmt.GetCol();

		aHTMLPageSize.Width() = rSz.GetWidth() - rLR.GetLeft() - rLR.GetRight();
		aHTMLPageSize.Height() = rSz.GetHeight() - rUL.GetUpper() - rUL.GetLower();

		if( 1 < rCol.GetNumCols() )
			aHTMLPageSize.Width() /= rCol.GetNumCols();
	}

	return aHTMLPageSize.Width();
}


/*  */

void SwHTMLParser::InsertIDOption()
{
	String aId;
	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		if( HTML_O_ID==pOption->GetToken() )
		{
			aId = pOption->GetString();
			break;
		}
	}

	if( aId.Len() )
		InsertBookmark( aId );
}


/*  */


void SwHTMLParser::InsertLineBreak()
{
	// <BR CLEAR=xxx> is handled as follows:
	// 1.) Only paragraph-bound frames are considered, which are
	//     are anchored in the current paragraph.
	// 2.) For left-aligned frames at CLEAR=LEFT
	//     or ALL and for right oriented frames with
	//     CLEAR=RIGHT or ALL the pass is changed as follows:
	// 3.) If the paragraph contains no text, the frame gets no
	//     text flow
	// 4.) otherwise a left aligned frame receives a right
	//     "anchor only" text flow and right aligned frame receives a
	//      left "anchor only" text flow.
	// 5.) if in a non-empty paragraph the text flow of a frame
	//     is changed, a new paragraph is opened
	// 6.) If the text flow is not changed by any frame, a 
	//     hard line break is inserted

	String aId, aStyle, aClass;			  	// the ID of the bookmark
	sal_Bool bClearLeft = sal_False, bClearRight = sal_False;
	sal_Bool bCleared = sal_False;	// was a CLEAR executed?

	// then get the options
	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		switch( pOption->GetToken() )
		{
			case HTML_O_CLEAR:
				{
					const String &aClear = pOption->GetString();
					if( aClear.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_AL_all ) )
					{
						bClearLeft = sal_True;
						bClearRight = sal_True;
					}
					else if( aClear.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_AL_left ) )
						bClearLeft = sal_True;
					else if( aClear.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_AL_right ) )
						bClearRight = sal_True;
				}
				break;
			case HTML_O_ID:
				aId = pOption->GetString();
				break;
			case HTML_O_STYLE:
				aStyle = pOption->GetString();
				break;
			case HTML_O_CLASS:
				aClass = pOption->GetString();
				break;
		}
	}

	// CLEAR is supported only for the current paragraph
	if( bClearLeft || bClearRight )
	{
		SwNodeIndex& rNodeIdx = pPam->GetPoint()->nNode;
		SwTxtNode* pTxtNd = rNodeIdx.GetNode().GetTxtNode();
		if( pTxtNd )
		{
			const SwSpzFrmFmts& rFrmFmtTbl = *pDoc->GetSpzFrmFmts();

			for( sal_uInt16 i=0; i<rFrmFmtTbl.Count(); i++ )
			{
                SwFrmFmt *const pFmt = rFrmFmtTbl[i];
                SwFmtAnchor const*const pAnchor = &pFmt->GetAnchor();
                SwPosition const*const pAPos = pAnchor->GetCntntAnchor();
                if (pAPos &&
                    ((FLY_AT_PARA == pAnchor->GetAnchorId()) ||
                     (FLY_AT_CHAR == pAnchor->GetAnchorId())) &&
					pAPos->nNode == rNodeIdx &&
					pFmt->GetSurround().GetSurround() != SURROUND_NONE )
				{
                    sal_Int16 eHori = RES_DRAWFRMFMT == pFmt->Which()
                        ? text::HoriOrientation::LEFT
						: pFmt->GetHoriOrient().GetHoriOrient();

					SwSurround eSurround = SURROUND_PARALLEL;
					if( pPam->GetPoint()->nContent.GetIndex() )
					{
                        if( bClearLeft && text::HoriOrientation::LEFT==eHori )
							eSurround = SURROUND_RIGHT;
                        else if( bClearRight && text::HoriOrientation::RIGHT==eHori )
							eSurround = SURROUND_LEFT;
					}
                    else if( (bClearLeft && text::HoriOrientation::LEFT==eHori) ||
                             (bClearRight && text::HoriOrientation::RIGHT==eHori) )
					{
						eSurround = SURROUND_NONE;
					}

					if( SURROUND_PARALLEL != eSurround )
					{
						SwFmtSurround aSurround( eSurround );
						if( SURROUND_NONE != eSurround )
							aSurround.SetAnchorOnly( sal_True );
                        pFmt->SetFmtAttr( aSurround );
						bCleared = sal_True;
					}
				} // Anchor is not in the node
			} // Loop over fly frames
		} // no text node
	} // no CLEAR

	// parse styles
    SvxFmtBreakItem aBreakItem( SVX_BREAK_NONE, RES_BREAK );
	sal_Bool bBreakItem = sal_False;
	if( HasStyleOptions( aStyle, aId, aClass ) )
	{
		SfxItemSet aItemSet( pDoc->GetAttrPool(), pCSS1Parser->GetWhichMap() );
		SvxCSS1PropertyInfo aPropInfo;

		if( ParseStyleOptions( aStyle, aId, aClass, aItemSet, aPropInfo ) )
		{
			if( pCSS1Parser->SetFmtBreak( aItemSet, aPropInfo ) )
			{
				aBreakItem = (const SvxFmtBreakItem &)aItemSet.Get( RES_BREAK );
				bBreakItem = sal_True;
			}
			if( aPropInfo.aId.Len() )
				InsertBookmark( aPropInfo.aId );
		}
	}

	if( bBreakItem && SVX_BREAK_PAGE_AFTER==aBreakItem.GetBreak() )
	{
		NewAttr( &aAttrTab.pBreak, aBreakItem );
		EndAttr( aAttrTab.pBreak, 0, sal_False );
	}

	if( !bCleared && !bBreakItem )
	{
		// if no CLEAR should be executed or could be executed
		// a line break is inserted
		String sTmp( (sal_Unicode)0x0a );	// make the Mac happy :-)
        pDoc->InsertString( *pPam, sTmp );
	}
	else if( pPam->GetPoint()->nContent.GetIndex() )
	{
		// if a claer was executed in a non-empty paragraph,
		// a new paragraph must be opened afterwards.
		// MIB 21.02.97: Actually, the lower paragraph should be set to 0.
		// But this works with something like <BR ..><P>.
		// (>Netacpe). Therefore we leave it for now.
		AppendTxtNode( AM_NOSPACE );
	}
	if( bBreakItem && SVX_BREAK_PAGE_BEFORE==aBreakItem.GetBreak() )
	{
		NewAttr( &aAttrTab.pBreak, aBreakItem );
		EndAttr( aAttrTab.pBreak, 0, sal_False );
	}
}

void SwHTMLParser::InsertHorzRule()
{
	sal_uInt16 nSize = 0;
	sal_uInt16 nWidth = 0;

	SvxAdjust eAdjust = SVX_ADJUST_END;

	sal_Bool bPrcWidth = sal_False;
	sal_Bool bNoShade = sal_False;
	sal_Bool bColor = sal_False;

	Color aColor;
	String aId;

	// get the options
	const HTMLOptions *pHTMLOptions = GetOptions();
	for( sal_uInt16 i = pHTMLOptions->Count(); i; )
	{
		const HTMLOption *pOption = (*pHTMLOptions)[--i];
		switch( pOption->GetToken() )
		{
		case HTML_O_ID:
			aId = pOption->GetString();
			break;
		case HTML_O_SIZE:
			nSize = (sal_uInt16)pOption->GetNumber();
			break;
		case HTML_O_WIDTH:
			bPrcWidth = (pOption->GetString().Search('%') != STRING_NOTFOUND);
			nWidth = (sal_uInt16)pOption->GetNumber();
			if( bPrcWidth && nWidth>=100 )
			{
				// 100% lines are the default case (no attrs needed)
				nWidth = 0;
				bPrcWidth = sal_False;
			}
			break;
		case HTML_O_ALIGN:
			eAdjust =
                (SvxAdjust)pOption->GetEnum( aHTMLPAlignTable, static_cast< sal_uInt16 >(eAdjust) );
			break;
		case HTML_O_NOSHADE:
			bNoShade = sal_True;
			break;
		case HTML_O_COLOR:
			pOption->GetColor( aColor );
			bColor = sal_True;
			break;
		}
	}

	if( pPam->GetPoint()->nContent.GetIndex() )
		AppendTxtNode( AM_NOSPACE );
	if( nOpenParaToken )
		EndPara();
	AppendTxtNode();
	pPam->Move( fnMoveBackward );

	// ... and remember in a context
	_HTMLAttrContext *pCntxt =
		new _HTMLAttrContext( HTML_HORZRULE, RES_POOLCOLL_HTML_HR, aEmptyStr );

	PushContext( pCntxt );

	// set the new template
	SetTxtCollAttrs( pCntxt );

	// the hard attributes on this paragraph will never be invalid again
	if( aParaAttrs.Count() )
		aParaAttrs.Remove( 0, aParaAttrs.Count() );

	if( nSize>0 || bColor || bNoShade )
	{
		// Set color and/or width of the line
		if( !bColor )
			aColor.SetColor( COL_GRAY );

		SvxBorderLine aBorderLine( &aColor );
		if( nSize )
		{
			long nPWidth = 0;
			long nPHeight = (long)nSize;
			SvxCSS1Parser::PixelToTwip( nPWidth, nPHeight );
			SvxCSS1Parser::SetBorderWidth( aBorderLine, (sal_uInt16)nPHeight,
										   !bNoShade );
		}
		else if( bNoShade )
		{
			aBorderLine.SetOutWidth( DEF_LINE_WIDTH_2 );
		}
		else
		{
			aBorderLine.SetOutWidth( DEF_DOUBLE_LINE0_OUT );
			aBorderLine.SetInWidth( DEF_DOUBLE_LINE0_IN );
			aBorderLine.SetDistance( DEF_DOUBLE_LINE0_DIST );
		}

        SvxBoxItem aBoxItem(RES_BOX);
		aBoxItem.SetLine( &aBorderLine, BOX_LINE_BOTTOM );
		_HTMLAttr* pTmp = new _HTMLAttr( *pPam->GetPoint(), aBoxItem );
		aSetAttrTab.Insert( pTmp, aSetAttrTab.Count() );
	}
	if( nWidth )
	{
		// If we are not in a table, the width specification will be "killed" by
		// paragraph indents. In a table this makes little
		// sense. To avoid that the line is taken into account in the width calculation
		// width calculation, it will still be assigned the appropriate
		// LRSpace item to the line.
#ifdef FIX41370
		const SwFmtColl *pColl = GetCurrFmtColl();
		SvxLRSpaceItem aLRItem( pColl->GetLRSpace() );
#endif
		if( !pTable )
		{
			// "Tune" the length and alignment of the line using paragraph indents
			long nBrowseWidth = GetCurrentBrowseWidth();
			nWidth = bPrcWidth ? (sal_uInt16)((nWidth*nBrowseWidth) / 100)
							   : ToTwips( (sal_uInt16)nBrowseWidth );
			if( nWidth < MINLAY )
				nWidth = MINLAY;

			if( (long)nWidth < nBrowseWidth )
			{
#ifndef FIX41370
				const SwFmtColl *pColl = GetCurrFmtColl();
				SvxLRSpaceItem aLRItem( pColl->GetLRSpace() );
#endif
				long nDist = nBrowseWidth - nWidth;

				switch( eAdjust )
				{
				case SVX_ADJUST_RIGHT:
					aLRItem.SetTxtLeft( (sal_uInt16)nDist );
					break;
				case SVX_ADJUST_LEFT:
					aLRItem.SetRight( (sal_uInt16)nDist );
					break;
				case SVX_ADJUST_CENTER:
				default:
					nDist /= 2;
					aLRItem.SetTxtLeft( (sal_uInt16)nDist );
					aLRItem.SetRight( (sal_uInt16)nDist );
					break;
				}

#ifndef FIX41370
				_HTMLAttr* pTmp = new _HTMLAttr( *pPam->GetPoint(), aLRItem );
				aSetAttrTab.Insert( pTmp, aSetAttrTab.Count() );
#endif
			}
		}

#ifdef FIX41370
		_HTMLAttr* pTmp = new _HTMLAttr( *pPam->GetPoint(), aLRItem );
		aSetAttrTab.Insert( pTmp, aSetAttrTab.Count() );
#endif
	}

	// Bookmarks cannot be inserted into hyperlinks
	if( aId.Len() )
		InsertBookmark( aId );

	// get the current context from the stack
	_HTMLAttrContext *pPoppedContext = PopContext( HTML_HORZRULE );
	ASSERT( pPoppedContext==pCntxt, "where does an HR context come from?" );
	delete pPoppedContext;

	pPam->Move( fnMoveForward );

	// and in the paragraph after that set the template that is current there
	SetTxtCollAttrs();
}

void SwHTMLParser::ParseMoreMetaOptions()
{
    String aName, aContent;
    sal_Bool bHTTPEquiv = sal_False;

    const HTMLOptions *pHTMLOptions = GetOptions();
    for( sal_uInt16 i = pHTMLOptions->Count(); i; )
    {
        const HTMLOption *pOption = (*pHTMLOptions)[ --i ];
        switch( pOption->GetToken() )
        {
        case HTML_O_NAME:
            aName = pOption->GetString();
            bHTTPEquiv = sal_False;
            break;
        case HTML_O_HTTPEQUIV:
            aName = pOption->GetString();
            bHTTPEquiv = sal_True;
            break;
        case HTML_O_CONTENT:
            aContent = pOption->GetString();
            break;
        }
    }

    // Here's where it gets a little tricky: we know for a fact that the doc-info
    // has not been changed. Therefore it is sufficient to query on generator and
    // refresh to find tokens which have not been processed yet,
    // because those are the only ones that do not modify the doc info.
    if( aName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_META_generator ) ||
        aName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_META_refresh ) ||
        aName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_META_content_type ) ||
        aName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_META_content_script_type ) )
        return;

    aContent.EraseAllChars( _CR );
    aContent.EraseAllChars( _LF );

    if( aName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_META_sdendnote ) )
    {
        FillEndNoteInfo( aContent );
        return;
    }

    if( aName.EqualsIgnoreCaseAscii( OOO_STRING_SVTOOLS_HTML_META_sdfootnote ) )
    {
        FillFootNoteInfo( aContent );
        return;
    }

    String sText( String::CreateFromAscii(TOOLS_CONSTASCII_STRINGPARAM("HTML: <")) );
    sText.AppendAscii( TOOLS_CONSTASCII_STRINGPARAM(OOO_STRING_SVTOOLS_HTML_meta) );
    sText.Append( ' ' );
    if( bHTTPEquiv  )
        sText.AppendAscii( TOOLS_CONSTASCII_STRINGPARAM(OOO_STRING_SVTOOLS_HTML_O_httpequiv) );
    else
        sText.AppendAscii( TOOLS_CONSTASCII_STRINGPARAM(OOO_STRING_SVTOOLS_HTML_O_name) );
    sText.AppendAscii( TOOLS_CONSTASCII_STRINGPARAM("=\"") );
    sText.Append( aName );
    sText.AppendAscii( TOOLS_CONSTASCII_STRINGPARAM("\" ") );
    sText.AppendAscii( TOOLS_CONSTASCII_STRINGPARAM(OOO_STRING_SVTOOLS_HTML_O_content) );
    sText.AppendAscii( TOOLS_CONSTASCII_STRINGPARAM("=\"") );
    sText.Append( aContent );
    sText.AppendAscii( TOOLS_CONSTASCII_STRINGPARAM("\">") );

    SwPostItField aPostItFld(
        (SwPostItFieldType*)pDoc->GetSysFldType( RES_POSTITFLD ),
        sText,
        aEmptyStr,
        aEmptyStr,
        aEmptyStr,
        DateTime() );
    SwFmtFld aFmtFld( aPostItFld );
    InsertAttr( aFmtFld );
}

/*  */

_HTMLAttr::_HTMLAttr( const SwPosition& rPos, const SfxPoolItem& rItem,
					  _HTMLAttr **ppHd ) :
	nSttPara( rPos.nNode ),
	nEndPara( rPos.nNode ),
	nSttCntnt( rPos.nContent.GetIndex() ),
	nEndCntnt(rPos.nContent.GetIndex() ),
	bInsAtStart( sal_True ),
	bLikePara( sal_False ),
	bValid( sal_True ),
	nCount( 1 ),
	pNext( 0 ),
	pPrev( 0 ),
	ppHead( ppHd )
{
	pItem = rItem.Clone();
}

_HTMLAttr::_HTMLAttr( const _HTMLAttr &rAttr, const SwNodeIndex &rEndPara,
					  sal_uInt16 nEndCnt, _HTMLAttr **ppHd ) :
	nSttPara( rAttr.nSttPara ),
	nEndPara( rEndPara ),
	nSttCntnt( rAttr.nSttCntnt ),
	nEndCntnt( nEndCnt ),
	bInsAtStart( rAttr.bInsAtStart ),
	bLikePara( rAttr.bLikePara ),
	bValid( rAttr.bValid ),
	nCount( rAttr.nCount ),
	pNext( 0 ),
	pPrev( 0 ),
	ppHead( ppHd )
{
	pItem = rAttr.pItem->Clone();
}

_HTMLAttr::~_HTMLAttr()
{
	delete pItem;
}

_HTMLAttr *_HTMLAttr::Clone( const SwNodeIndex& rEndPara, sal_uInt16 nEndCnt ) const
{
	// recreate the attribute with the old start position
	_HTMLAttr *pNew = new _HTMLAttr( *this, rEndPara, nEndCnt, ppHead );

	// the Previous list must be taken over, the Next list must not!
	pNew->pPrev = pPrev;

	return pNew;
}

void _HTMLAttr::Reset( const SwNodeIndex& rSttPara, sal_uInt16 nSttCnt,
					   _HTMLAttr **ppHd )
{
	// reset the beginning (and the end)
	nSttPara = rSttPara;
	nSttCntnt = nSttCnt;
	nEndPara = rSttPara;
	nEndCntnt = nSttCnt;

	// correct the head and undo the concatenations
	pNext = 0;
	pPrev = 0;
	ppHead = ppHd;
}

void _HTMLAttr::InsertPrev( _HTMLAttr *pPrv )
{
	ASSERT( !pPrv->pNext || pPrv->pNext == this,
			"_HTMLAttr::InsertPrev: pNext wrong" );
	pPrv->pNext = 0;

	ASSERT( 0 == pPrv->ppHead || ppHead == pPrv->ppHead,
			"_HTMLAttr::InsertPrev: ppHead wrong" );
	pPrv->ppHead = 0;

	_HTMLAttr *pAttr = this;
	while( pAttr->GetPrev() )
		pAttr = pAttr->GetPrev();

	pAttr->pPrev = pPrv;
}

bool SwHTMLParser::ParseMetaOptions(
        const uno::Reference<document::XDocumentProperties> & i_xDocProps,
        SvKeyValueIterator *i_pHeader )
{
    // always call base ParseMetaOptions, it sets the encoding (#i96700#)
    bool ret( HTMLParser::ParseMetaOptions(i_xDocProps, i_pHeader) );
    if (!ret && IsNewDoc())
    {
        ParseMoreMetaOptions();
    }
    return ret;
}

// override so we can parse DOCINFO field subtypes INFO[1-4]
void SwHTMLParser::AddMetaUserDefined( ::rtl::OUString const & i_rMetaName )
{
    // unless we already have 4 names, append the argument to m_InfoNames
    ::rtl::OUString* pName // the first empty string in m_InfoNames
         (!m_InfoNames[0].getLength() ? &m_InfoNames[0] :
         (!m_InfoNames[1].getLength() ? &m_InfoNames[1] :
         (!m_InfoNames[2].getLength() ? &m_InfoNames[2] :
         (!m_InfoNames[3].getLength() ? &m_InfoNames[3] : 0 ))));
    if (pName)
    {
        (*pName) = i_rMetaName;
    }
}

