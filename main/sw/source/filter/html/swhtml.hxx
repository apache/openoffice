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


#ifndef _SWHTML_HXX
#define _SWHTML_HXX

#if !defined(_SVSTDARR_XUB_STRLEN_DECL) || !defined(_SVSTDARR_LONGS_DECL) || \
	!defined(_SVSTDARR_USHORTS_DECL) || !defined(_SVSTDARR_STRINGSDTOR_DECL)
#ifndef _SVSTDARR_XUB_STRLEN_DECL
#define _SVSTDARR_XUB_STRLEN
#endif
#ifndef _SVSTDARR_LONGS_DECL
#define _SVSTDARR_LONGS
#endif
#ifndef _SVSTDARR_USHORTS_DECL
#define _SVSTDARR_USHORTS
#endif
#ifndef _SVSTDARR_STRINGSDTOR_DECL
#define _SVSTDARR_STRINGSDTOR
#endif
#include <svl/svstdarr.hxx>
#endif
#include <tools/urlobj.hxx>
#include <sfx2/sfxhtml.hxx>
#include <svl/macitem.hxx>
#include <editeng/svxenum.hxx>
#include <fmtornt.hxx>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/form/XFormComponent.hpp>
#include <pam.hxx>

#include "calbck.hxx"
#include "htmlvsh.hxx"

class SfxMedium;
class SfxViewFrame;
class SdrObject;
class SvxMacroTableDtor;
class SvStringsDtor;
class SwDoc;
class SwPaM;
class ViewShell;
class SwStartNode;
class SwFmtColl;
class SwField;
class SwHTMLForm_Impl;
class SwApplet_Impl;
struct SwHTMLFootEndNote_Impl;
class HTMLTableCnts;
struct SwPendingStack;
class SvxCSS1PropertyInfo;

#define HTML_PARSPACE (MM50)

#define HTML_DFLT_IMG_WIDTH (MM50*4)
#define HTML_DFLT_IMG_HEIGHT (MM50*2)

// a few things that are needed from time to time
extern HTMLOptionEnum __FAR_DATA aHTMLPAlignTable[];
extern HTMLOptionEnum __FAR_DATA aHTMLImgHAlignTable[];
extern HTMLOptionEnum __FAR_DATA aHTMLImgVAlignTable[];


// the attribute stack:

class _HTMLAttr;
typedef _HTMLAttr *_HTMLAttrPtr;
SV_DECL_PTRARR( _HTMLAttrs, _HTMLAttrPtr, 5, 5 )

class _HTMLAttr
{
	friend class SwHTMLParser;
	friend class _CellSaveStruct;

	SwNodeIndex nSttPara, nEndPara;
	xub_StrLen nSttCntnt, nEndCntnt;
	sal_Bool bInsAtStart : 1;
	sal_Bool bLikePara : 1;// Set attribute over the entire paragraph
	sal_Bool bValid : 1;// is the attribute valid?

	SfxPoolItem* pItem;
	sal_uInt16 nCount;// open attrs, with different values
	_HTMLAttr *pNext;// Attrs with different values still to be closed. Values
	_HTMLAttr *pPrev;// Attrs already closed but not yet legislated
	_HTMLAttr **ppHead;// the list header

	_HTMLAttr( const SwPosition& rPos, const SfxPoolItem& rItem, _HTMLAttr **pHd=0 );

	_HTMLAttr( const _HTMLAttr &rAttr, const SwNodeIndex &rEndPara, xub_StrLen nEndCnt, _HTMLAttr **pHd );

public:

	~_HTMLAttr();

	_HTMLAttr *Clone( const SwNodeIndex& rEndPara, xub_StrLen nEndCnt ) const;
	void Reset( const SwNodeIndex& rSttPara, xub_StrLen nSttCnt, _HTMLAttr **pHd );
	inline void SetStart( const SwPosition& rPos );

	sal_uInt32 GetSttParaIdx() const { return nSttPara.GetIndex(); }
	sal_uInt32 GetEndParaIdx() const { return nEndPara.GetIndex(); }

	const SwNodeIndex& GetSttPara() const { return nSttPara; }
	const SwNodeIndex& GetEndPara() const { return nEndPara; }

	xub_StrLen GetSttCnt() const { return nSttCntnt; }
	xub_StrLen GetEndCnt() const { return nEndCntnt; }

	sal_Bool IsLikePara() const { return bLikePara; }
	void SetLikePara( sal_Bool bPara=sal_True ) { bLikePara = bPara; }

		  SfxPoolItem& GetItem()		{ return *pItem; }
	const SfxPoolItem& GetItem() const	{ return *pItem; }

	_HTMLAttr *GetNext() const { return pNext; }
	void InsertNext( _HTMLAttr *pNxt ) { pNext = pNxt; }

	_HTMLAttr *GetPrev() const { return pPrev; }
	void InsertPrev( _HTMLAttr *pPrv );
	void ClearPrev() { pPrev = 0; }

	void SetHead( _HTMLAttr **ppHd ) { ppHead = ppHd; }

	// When setting attributes from templates, it can happen,
	// that attributes should not be set after all. To delete them
	// would be very time-consuming, because you don't know exactly
	// where they are chained. Therefore, they are simply invalidated
	// and deleted with next _SetAttr().
	void Invalidate() { bValid = sal_False; }
	sal_Bool IsValid() const { return bValid; }
};

// Table of attributes: Here the order is important: the attributes
// at the front of the table are also set first in EndAllAttrs.
struct _HTMLAttrTable
{
	_HTMLAttr
				*pKeep,// ::com::sun::star::frame::Frame-Attribute
				*pBox,
				*pBrush,
				*pBreak,
				*pPageDesc,

				*pLRSpace,// paragraph-attributes
				*pULSpace,
				*pLineSpacing,
				*pAdjust,
				*pDropCap,
				*pSplit,
				*pWidows,
				*pOrphans,
				*pDirection,

				*pCharFmts,// text-attributes
				*pINetFmt,

				*pBold,// character-attributes
				*pBoldCJK,
				*pBoldCTL,
				*pItalic,
				*pItalicCJK,
				*pItalicCTL,
				*pStrike,
				*pUnderline,
				*pBlink,
				*pFont,
				*pFontCJK,
				*pFontCTL,
				*pFontHeight,
				*pFontHeightCJK,
				*pFontHeightCTL,
				*pFontColor,
				*pEscapement,
				*pCaseMap,
				*pKerning,// (only for SPACER)
				*pCharBrush,// Character background
				*pLanguage,
				*pLanguageCJK,
				*pLanguageCTL
				;
};

class _HTMLAttrContext_SaveDoc;

enum SwHTMLAppendMode {
	 AM_NORMAL,// no paragraph spacing treatment
	 AM_NOSPACE,// Set distance hard to 0cm
	 AM_SPACE,// Set distance hard to 0.5cm
	 AM_SOFTNOSPACE,// Do not set distance but remember 0cm
	 AM_NONE// no append at all
};

class _HTMLAttrContext
{
	_HTMLAttrs aAttrs;// the attributes started in the context

	String aClass;// the class of the context

	_HTMLAttrContext_SaveDoc *pSaveDocContext;
	SfxItemSet *pFrmItemSet;

	sal_uInt16 	nToken;// the token to which the context belongs

	sal_uInt16 	nTxtFmtColl;// a template started in the context or 0

	sal_uInt16 	nLeftMargin;// a changed left edge
	sal_uInt16 	nRightMargin;// a changed right edge
	sal_uInt16	nFirstLineIndent;// a changed first line indent

	sal_uInt16	nUpperSpace;
	sal_uInt16	nLowerSpace;

	SwHTMLAppendMode eAppend;

	sal_Bool	bLRSpaceChanged : 1;// left/right edge, indent changed?
	sal_Bool	bULSpaceChanged : 1;// upper/lower edge changed?
	sal_Bool	bDfltTxtFmtColl : 1;// nTxtFmtColl is only a default
	sal_Bool	bSpansSection : 1;// The context spans a SwSection
	sal_Bool	bPopStack : 1;// Unstack stack elements above.
	sal_Bool	bFinishPREListingXMP : 1;
	sal_Bool	bRestartPRE : 1;
	sal_Bool	bRestartXMP : 1;
	sal_Bool	bRestartListing : 1;

public:
	void ClearSaveDocContext();

	_HTMLAttrContext( sal_uInt16 nTokn, sal_uInt16 nPoolId, const String& rClass,
					  sal_Bool bDfltColl=sal_False ) :
		aClass( rClass ),
		pSaveDocContext( 0 ),
		pFrmItemSet( 0 ),
		nToken( nTokn ),
		nTxtFmtColl( nPoolId ),
		nLeftMargin( 0 ),
		nRightMargin( 0 ),
		nFirstLineIndent( 0 ),
		nUpperSpace( 0 ),
		nLowerSpace( 0 ),
		eAppend( AM_NONE ),
		bLRSpaceChanged( sal_False ),
		bULSpaceChanged( sal_False ),
		bDfltTxtFmtColl( bDfltColl ),
		bSpansSection( sal_False ),
		bPopStack( sal_False ),
		bFinishPREListingXMP( sal_False ),
		bRestartPRE( sal_False ),
		bRestartXMP( sal_False ),
		bRestartListing( sal_False )
	{}

	_HTMLAttrContext( sal_uInt16 nTokn ) :
		pSaveDocContext( 0 ),
		pFrmItemSet( 0 ),
		nToken( nTokn ),
		nTxtFmtColl( 0 ),
		nLeftMargin( 0 ),
		nRightMargin( 0 ),
		nFirstLineIndent( 0 ),
		nUpperSpace( 0 ),
		nLowerSpace( 0 ),
		eAppend( AM_NONE ),
		bLRSpaceChanged( sal_False ),
		bULSpaceChanged( sal_False ),
		bDfltTxtFmtColl( sal_False ),
		bSpansSection( sal_False ),
		bPopStack( sal_False ),
		bFinishPREListingXMP( sal_False ),
		bRestartPRE( sal_False ),
		bRestartXMP( sal_False ),
		bRestartListing( sal_False )
	{}

	~_HTMLAttrContext() { ClearSaveDocContext(); delete pFrmItemSet; }

	sal_uInt16 GetToken() const { return nToken; }

	sal_uInt16 GetTxtFmtColl() const { return bDfltTxtFmtColl ? 0 : nTxtFmtColl; }
	sal_uInt16 GetDfltTxtFmtColl() const { return bDfltTxtFmtColl ? nTxtFmtColl : 0; }

	const String& GetClass() const { return aClass; }

	inline void SetMargins( sal_uInt16 nLeft, sal_uInt16 nRight, short nIndent );

	inline sal_Bool IsLRSpaceChanged() const { return bLRSpaceChanged; }
	inline void GetMargins( sal_uInt16& nLeft, sal_uInt16& nRight,
							short &nIndent ) const;

	inline void SetULSpace( sal_uInt16 nUpper, sal_uInt16 nLower );
	inline sal_Bool IsULSpaceChanged() const { return bULSpaceChanged; }
	inline void GetULSpace( sal_uInt16& rUpper, sal_uInt16& rLower ) const;

	sal_Bool HasAttrs() const { return aAttrs.Count() != 0; }
	const _HTMLAttrs& GetAttrs() const { return aAttrs; }
	_HTMLAttrs& GetAttrs() { return aAttrs; }

	void SetSpansSection( sal_Bool bSet ) { bSpansSection = bSet; }
	sal_Bool GetSpansSection() const { return bSpansSection; }

	void SetPopStack( sal_Bool bSet ) { bPopStack = bSet; }
	sal_Bool GetPopStack() const { return bPopStack; }

	sal_Bool HasSaveDocContext() const { return pSaveDocContext!=0; }
	_HTMLAttrContext_SaveDoc *GetSaveDocContext( sal_Bool bCreate=sal_False );

	const SfxItemSet *GetFrmItemSet() const { return pFrmItemSet; }
	SfxItemSet *GetFrmItemSet( SwDoc *pCreateDoc );

	void SetFinishPREListingXMP( sal_Bool bSet ) { bFinishPREListingXMP = bSet; }
	sal_Bool IsFinishPREListingXMP() const { return bFinishPREListingXMP; }

	void SetRestartPRE( sal_Bool bSet ) { bRestartPRE = bSet; }
	sal_Bool IsRestartPRE() const { return bRestartPRE; }

	void SetRestartXMP( sal_Bool bSet ) { bRestartXMP = bSet; }
	sal_Bool IsRestartXMP() const { return bRestartXMP; }

	void SetRestartListing( sal_Bool bSet ) { bRestartListing = bSet; }
	sal_Bool IsRestartListing() const { return bRestartListing; }

	void SetAppendMode( SwHTMLAppendMode eMode=AM_NORMAL ) { eAppend = eMode; }
	SwHTMLAppendMode GetAppendMode() const { return eAppend; }
};

typedef _HTMLAttrContext *_HTMLAttrContextPtr;
SV_DECL_PTRARR( _HTMLAttrContexts, _HTMLAttrContextPtr, 5, 5 )

class HTMLTable;
class SwCSS1Parser;
class SwHTMLNumRuleInfo;

typedef ImageMap *ImageMapPtr;
SV_DECL_PTRARR_DEL( ImageMaps, ImageMapPtr, 1, 1 )
typedef SwFrmFmt *SwFrmFmtPtr;
SV_DECL_PTRARR( SwHTMLFrmFmts, SwFrmFmtPtr, 2, 2 )

#define HTML_CNTXT_PROTECT_STACK	0x0001
#define HTML_CNTXT_STRIP_PARA		0x0002
#define HTML_CNTXT_KEEP_NUMRULE		0x0004
#define HTML_CNTXT_HEADER_DIST		0x0008
#define HTML_CNTXT_FOOTER_DIST	 	0x0010
#define HTML_CNTXT_KEEP_ATTRS		0x0020

#define CONTEXT_FLAGS_ABSPOS	\
	(HTML_CNTXT_PROTECT_STACK | \
	 HTML_CNTXT_STRIP_PARA)

#define HTML_FF_BOX		 		0x0001
#define HTML_FF_BACKGROUND			0x0002
#define HTML_FF_PADDING				0x0004
#define HTML_FF_DIRECTION			0x0008

class SwHTMLParser : public SfxHTMLParser, public SwClient
{
	friend class _SectionSaveStruct;
	friend class _CellSaveStruct;
	friend class _CaptionSaveStruct;

	String		aPathToFile;
	String		sBaseURL;
	String		sSaveBaseURL;
	String		aBasicLib;
	String		aBasicModule;
	String		aScriptSource;// Content of the current script block
	String		aScriptType;// Type of the read script (StarBasic/VB/JAVA)
	String		aScriptURL;// URL of a script
	String		aStyleSource;// Contents of the current style sheet
	String		aContents;// Text of the actual marquee, field, etc.
	String		sTitle;
	String		aUnknownToken;// a started unknown token
	String		aBulletGrfs[MAXLEVEL];
	String		sJmpMark;

	SvUShorts	aBaseFontStack;// Stack for <BASEFONT>
                                       // Bit 0-2: Font size (1-7)
	SvUShorts	aFontStack;// Stack for <FONT>, <BIG>, <SMALL>
				   // Bit 0-2: Font size (1-7)
				   // Bit 15: Font color was set

	_HTMLAttrs		aSetAttrTab;// "closed", not yet set Attr.
	_HTMLAttrs		aParaAttrs;// leading paragraph attributes
	_HTMLAttrTable	aAttrTab;// "open" attributes
	_HTMLAttrContexts aContexts;// the current attribute/token context
	SwHTMLFrmFmts	aMoveFlyFrms;// Fly-Frames, der Anker wird verschoben
	SvXub_StrLens	aMoveFlyCnts;// und die Content-Positionen

	SwApplet_Impl *pAppletImpl;// the current applet

	SwCSS1Parser 	*pCSS1Parser;// the style sheet parser
	SwHTMLNumRuleInfo *pNumRuleInfo;
	SwPendingStack	*pPendStack;

	SwDoc			*pDoc;
	SwPaM			*pPam;// SwPosition should be enough, right?
	ViewShell		*pActionViewShell;// ViewShell, at which the StartAction
										// was called.
	SwNodeIndex		*pSttNdIdx;

	HTMLTable		*pTable;// the current "outermost" table
	SwHTMLForm_Impl *pFormImpl;// the current form
	SdrObject		*pMarquee;// current marquee
	SwField			*pField;// current field
	ImageMap		*pImageMap;// current image map
	ImageMaps		*pImageMaps;// all read image maps
	SwHTMLFootEndNote_Impl *pFootEndNoteImpl;

	Size 	aHTMLPageSize;// the page size of the HTML template

	sal_uInt32 	aFontHeights[7];// the font heights 1-7
	sal_uInt32	nScriptStartLineNr;// Line number of a script block
	sal_uLong		nEventId;

	sal_uInt16	nBaseFontStMin;
	sal_uInt16	nFontStMin; 
	sal_uInt16	nDefListDeep;
	sal_uInt16	nFontStHeadStart;// Elements in the font stack at <Hn>.
	sal_uInt16	nSBModuleCnt;// Counter for Basic modules
	sal_uInt16	nMissingImgMaps;// How many image maps are missing?
	sal_uInt16 	nParaCnt;
	sal_uInt16	nContextStMin;// Lower limit for PopContext
	sal_uInt16	nContextStAttrMin;// Lower limit for attribution
	sal_uInt16	nSelectEntryCnt;// Number of entries in the current list box
	sal_uInt16	nOpenParaToken;// an opened paragraph element

	enum JumpToMarks { JUMPTO_NONE, JUMPTO_MARK, JUMPTO_TABLE, JUMPTO_FRAME,
						JUMPTO_REGION, JUMPTO_GRAPHIC } eJumpTo;

#ifdef DBG_UTIL
	sal_uInt16	nContinue;// Depth of Continue calls
#endif

	SvxAdjust	eParaAdjust;// Alignment of the current paragraph
	HTMLScriptLanguage eScriptLang;// the current script language

	sal_Bool bOldIsHTMLMode : 1;// Was it once an HTML document?

	sal_Bool bDocInitialized : 1;// Document or shell were initialized
                                     // Flag by double init by recursion
                                     // to prevent.
	sal_Bool bViewCreated : 1;// the view was already created (asynchronous)
	sal_Bool bSetCrsr : 1;// Set Crsr back to the beginning
	sal_Bool bSetModEnabled : 1;

	sal_Bool bInFloatingFrame : 1;// We are in a floating ::com::sun::star::frame::Frame
	sal_Bool bInField : 1;
	sal_Bool bKeepUnknown : 1;// unknown/unsupported tokens treatment
	// 8
	sal_Bool bCallNextToken : 1;// In tables: Call NextToken in any case
	sal_Bool bIgnoreRawData : 1;// Ignore content of a script/style.
	sal_Bool bLBEntrySelected : 1;// Is the current listbox entry selected.
	sal_Bool bTAIgnoreNewPara : 1;// ignore next LF in TextArea?
	sal_Bool bFixMarqueeWidth : 1;// Adjust the size of a ticker?
	sal_Bool bFixMarqueeHeight : 1;

	sal_Bool bUpperSpace : 1;// upper paragraph spacing is required
	sal_Bool bNoParSpace : 1;
	// 16

	sal_Bool bAnyStarBasic : 1;// there is a StarBasic module at all
	sal_Bool bInNoEmbed : 1;// We are in a NOEMBED area

	sal_Bool bInTitle : 1;// We are in the title

	sal_Bool bChkJumpMark : 1;// jump to a given marker if necessary
	sal_Bool bUpdateDocStat : 1;
	sal_Bool bFixSelectWidth : 1;// Reset the width of a select?
	sal_Bool bFixSelectHeight : 1;// Reset the height of a select?
	sal_Bool bTextArea : 1;
	// 24
	sal_Bool bSelect : 1;
	sal_Bool bInFootEndNoteAnchor : 1;
	sal_Bool bInFootEndNoteSymbol : 1;
        sal_Bool bIgnoreHTMLComments : 1;
        sal_Bool bRemoveHidden : 1;// the filter implementation might set the hidden flag

        // the names corresponding to the DOCINFO field subtypes INFO[1-4]
        ::rtl::OUString m_InfoNames[4];

	SfxViewFrame* pTempViewFrame;

	void DeleteFormImpl();

	void DocumentDetected();
	void Show();
	void ShowStatline();
	ViewShell *CallStartAction( ViewShell *pVSh = 0, sal_Bool bChkPtr = sal_True );
	ViewShell *CallEndAction( sal_Bool bChkAction = sal_False, sal_Bool bChkPtr = sal_True );
	ViewShell *CheckActionViewShell();

	DECL_LINK( AsyncCallback, void* );

	// Attribute am Dok setzen
	void _SetAttr( sal_Bool bChkEnd, sal_Bool bBeforeTable, _HTMLAttrs *pPostIts );
	inline void SetAttr( sal_Bool bChkEnd = sal_True, sal_Bool bBeforeTable = sal_False,
						 _HTMLAttrs *pPostIts = 0 )
	{
		if( aSetAttrTab.Count() || aMoveFlyFrms.Count() )
			_SetAttr( bChkEnd, bBeforeTable, pPostIts );
	}

	_HTMLAttr **GetAttrTabEntry( sal_uInt16 nWhich );

	// Create a new text node at PaM position
	sal_Bool AppendTxtNode( SwHTMLAppendMode eMode=AM_NORMAL, sal_Bool bUpdateNum=sal_True );
	void AddParSpace();

	// Start/end an attribute
	// ppDepAttr specifies an attribute table entry whose attributes  
	// must be set before the attribute can be terminated.
	void NewAttr( _HTMLAttr **ppAttr, const SfxPoolItem& rItem );
	void EndAttr( _HTMLAttr *pAttr, _HTMLAttr **ppDepAttr=0,
				  sal_Bool bChkEmpty=sal_True );
	void DeleteAttr( _HTMLAttr* pAttr );

	void EndContextAttrs( _HTMLAttrContext *pContext, sal_Bool bRemove=sal_False );
	void SaveAttrTab( _HTMLAttrTable& rNewAttrTab );
	void SplitAttrTab( const SwPosition& rNewPos );
	void SplitAttrTab( _HTMLAttrTable& rNewAttrTab, sal_Bool bMoveEndBack = sal_True );
	void RestoreAttrTab( const _HTMLAttrTable& rNewAttrTab,
						 sal_Bool bSetNewStart = sal_False );
	void InsertAttr( const SfxPoolItem& rItem, sal_Bool bLikePara = sal_False,
					 sal_Bool bInsAtStart=sal_False );
	void InsertAttrs( _HTMLAttrs& rAttrs );

	sal_Bool DoPositioning( SfxItemSet &rItemSet,
						SvxCSS1PropertyInfo &rPropInfo,
						_HTMLAttrContext *pContext );
	sal_Bool CreateContainer( const String& rClass, SfxItemSet &rItemSet,
						  SvxCSS1PropertyInfo &rPropInfo,
						  _HTMLAttrContext *pContext );
	sal_Bool EndSection( sal_Bool bLFStripped=sal_False );

	void InsertAttrs( SfxItemSet &rItemSet, SvxCSS1PropertyInfo &rPropInfo,
					  _HTMLAttrContext *pContext, sal_Bool bCharLvl=sal_False );
	void InsertAttr( _HTMLAttr **ppAttr, const SfxPoolItem & rItem,
					 _HTMLAttrContext *pCntxt );
	void SplitPREListingXMP( _HTMLAttrContext *pCntxt );
	void FixHeaderFooterDistance( sal_Bool bHeader,	const SwPosition *pOldPos );

	void EndContext( _HTMLAttrContext *pContext );
	void ClearContext( _HTMLAttrContext *pContext );

	const SwFmtColl *GetCurrFmtColl() const;

	SwTwips GetCurrentBrowseWidth();

	SwHTMLNumRuleInfo& GetNumInfo() { return *pNumRuleInfo; }
        // --> OD 2008-04-02 #refactorlists#
        // add parameter <bCountedInList>
        void SetNodeNum( sal_uInt8 nLevel, bool bCountedInList );
        // <--

	// Manage paragraph templates

	// set the templates on the stack or their attributes.
	void SetTxtCollAttrs( _HTMLAttrContext *pContext = 0 );

	void InsertParaAttrs( const SfxItemSet& rItemSet );

	// Manage the attribute context

	// remember current context
	inline void PushContext( _HTMLAttrContext *pCntxt );

	// get the topmost/specified context, but do not search outside
	// of the context with token nLimit. If bRemove is set,
	// it is removed
	_HTMLAttrContext *PopContext( sal_uInt16 nToken=0, sal_uInt16 nLimit=0,
								  sal_Bool bRemove=sal_True );
	inline const _HTMLAttrContext *GetTopContext() const;

	sal_Bool GetMarginsFromContext( sal_uInt16 &nLeft, sal_uInt16 &nRight, short& nIndent,
								sal_Bool bIgnoreCurrent=sal_False ) const;
	sal_Bool GetMarginsFromContextWithNumBul( sal_uInt16 &nLeft, sal_uInt16 &nRight,
										  short& nIndent ) const;
	void GetULSpaceFromContext( sal_uInt16 &rUpper, sal_uInt16 &rLower ) const;


	void MovePageDescAttrs( SwNode *pSrcNd, sal_uLong nDestIdx, sal_Bool bFmtBreak );

	// Handling tags at paragraph level

	// <P> and <H1> to <H6>
	void NewPara();
	void EndPara( sal_Bool bReal = sal_False );
	void NewHeading( int nToken );
	void EndHeading();

	// <ADDRESS>, <BLOCKQUOTE> and <PRE>
	void NewTxtFmtColl( int nToken, sal_uInt16 nPoolId );
	void EndTxtFmtColl( int nToken );

	// <DIV> and <CENTER>
	void NewDivision( int nToken );
	void EndDivision( int nToken );

	// Insert/exit fly frames
	void InsertFlyFrame( const SfxItemSet& rItemSet, _HTMLAttrContext *pCntxt,
						 const String& rId, sal_uInt16 nFlags );

	void SaveDocContext( _HTMLAttrContext *pCntxt, sal_uInt16 nFlags,
					   const SwPosition *pNewPos );
	void RestoreDocContext( _HTMLAttrContext *pCntxt );

	// leave all areas spanned by <DIV>.
	sal_Bool EndSections( sal_Bool bLFStripped );

	// <MULTICOL>
	void NewMultiCol();
	void EndMultiCol();

	// <MARQUEE>
	void NewMarquee( HTMLTable *pCurTable=0 );
	void EndMarquee();
	void InsertMarqueeText();

	// List handling

	// Numbering <OL> and Enumeration Lists <UL> with <LI>
	void NewNumBulList( int nToken );
	void EndNumBulList( int nToken=0 );
	void NewNumBulListItem( int nToken );
	void EndNumBulListItem( int nToken=0, sal_Bool bSetColl=sal_True,
							sal_Bool bLastPara=sal_False );

	// Definition lists <DL> with <DD>, <DT>
	void NewDefList();
	void EndDefList();
	void NewDefListItem( int nToken );
	void EndDefListItem( int nToken=0, sal_Bool bSetColl=sal_True,
						 sal_Bool bLastPara=sal_False );


	// Handling tags at character level

	// handle tags like <B>, <I> etc that turn a specific attribute
	// on and off, or which like SPAN only fetch attributes from styles.
	void NewStdAttr( int nToken );
	void NewStdAttr( int nToken,
					 _HTMLAttr **ppAttr, const SfxPoolItem & rItem,
					 _HTMLAttr **ppAttr2=0, const SfxPoolItem *pItem2=0,
					 _HTMLAttr **ppAttr3=0, const SfxPoolItem *pItem3=0 );
	void EndTag( int nToken );

	// Handle font attributes
	void NewBasefontAttr(); 			// for <BASEFONT>
	void EndBasefontAttr();
	void NewFontAttr( int nToken );	// for <FONT>, <BIG> and <SMALL>
	void EndFontAttr( int nToken );

	// Tags realized by character templates
	void NewCharFmt( int nToken );

	// <SDFIELD>
public:
	static sal_uInt16 GetNumType( const String& rStr, sal_uInt16 eDfltType );
private: 
        void NewField();
	void EndField();
	void InsertFieldText();

	// <SPACER>
	void InsertSpacer();

	// Inserting graphics, plugins and applets

	// Search image maps and connect them with graphic nodes
	ImageMap *FindImageMap( const String& rURL ) const;
	void ConnectImageMaps();

        // Determine anchoring of a fly frame and set corresponding attributes 
        // set in the attrset (htmlgrin.cxx)
        void SetAnchorAndAdjustment( sal_Int16 eVertOri,
                                     sal_Int16 eHoriOri,
                                     const SfxItemSet &rItemSet,
                                     const SvxCSS1PropertyInfo &rPropInfo,
                                     SfxItemSet& rFrmSet );
        void SetAnchorAndAdjustment( sal_Int16 eVertOri,
                                     sal_Int16 eHoriOri,
                                     SfxItemSet& rFrmSet,
                                     sal_Bool bDontAppend=sal_False );
	void SetAnchorAndAdjustment( const SfxItemSet &rItemSet,
                                     const SvxCSS1PropertyInfo &rPropInfo,
                                     SfxItemSet &rFrmItemSet );

	void SetFrmFmtAttrs( SfxItemSet &rItemSet, SvxCSS1PropertyInfo &rPropInfo,
						 sal_uInt16 nFlags, SfxItemSet &rFrmItemSet );

	// Create frames and register auto-bound frames
	void RegisterFlyFrm( SwFrmFmt *pFlyFrm );

	// Adapt the size of the fly-frame to the specifications and conditions.
	// (not for graphics, therefore htmlplug.cxx)
	void SetFixSize( const Size& rPixSize, const Size& rTwipDfltSize,
					 sal_Bool bPrcWidth, sal_Bool bPrcHeight,
					 SfxItemSet &rItemSet, SvxCSS1PropertyInfo &rPropInfo,
					 SfxItemSet& rFlyItemSet );
	void SetVarSize( SfxItemSet &rItemSet, SvxCSS1PropertyInfo &rPropInfo,
					 SfxItemSet& rFlyItemSet, SwTwips nDfltWidth=MINLAY,
					 sal_uInt8 nDltPrcWidth=0 );
	void SetSpace( const Size& rPixSpace, SfxItemSet &rItemSet,
				   SvxCSS1PropertyInfo &rPropInfo, SfxItemSet& rFlyItemSet );

	sal_uInt16 IncGrfsThatResizeTable();

	void GetDefaultScriptType( ScriptType& rType,
							   String& rTypeStr ) const;

	// the actual insert methods for <IMG>, <EMBED>, and <APPLET>
	// and <PARAM>
	void InsertImage();// htmlgrin.cxx
	void InsertEmbed();// htmlplug.cxx

#ifdef SOLAR_JAVA
	void NewObject();// htmlplug.cxx
#endif
	void EndObject();// CommandLine with Applet link. (htmlplug.cxx)
#ifdef SOLAR_JAVA
	void InsertApplet();// htmlplug.cxx
#endif
	void EndApplet();// CommandLine with Applet link. (htmlplug.cxx)
	void InsertParam();// htmlplug.cxx

	void InsertFloatingFrame();
	void EndFloatingFrame() { bInFloatingFrame = sal_False; }

	// <BODY> tag evaluate: set background graphics and colors (htmlgrin.cxx)
	void InsertBodyOptions();


	// Inserting links and ::com::sun::star::text::Bookmarks (htmlgrin.cxx)

	// Evaluate <A> tag: insert a link or ::com::sun::star::text::bookmark.
	void NewAnchor();
	void EndAnchor();

	// insert a ::com::sun::star::text::bookmark
	void InsertBookmark( const String& rName );


	void InsertCommentText( const sal_Char *pTag = 0 );
	void InsertComment( const String& rName, const sal_Char *pTag = 0 );

	// are there ::com::sun::star::text::bookmarks in the current paragraph?
	sal_Bool HasCurrentParaBookmarks( sal_Bool bIgnoreStack=sal_False ) const;


	// Inserting Script/Basic elements

	// parse the last Basic module (htmlbas.cxx) that was loaded
	void NewScript();
	void EndScript();

	void AddScriptSource();

	// add an event to the SFX configuration (htmlbas.cxx)
	void InsertBasicDocEvent( rtl::OUString aEventName, const String& rName,
							  ScriptType eScrType, const String& rScrType );

	// attach an event to a VC control (htmlform.cxx)
	void InsertBasicCtrlEvent( sal_uInt16 nEvent, const String& rName );

	// Inserting styles

	// <STYLE>
	void NewStyle();
	void EndStyle();

	inline sal_Bool HasStyleOptions( const String &rStyle, const String &rId,const String &rClass, const String *pLang=0,const String *pDir=0 );
	sal_Bool ParseStyleOptions( const String &rStyle, const String &rId,
							const String &rClass, SfxItemSet &rItemSet,
							SvxCSS1PropertyInfo &rPropInfo,
							 const String *pLang=0, const String *pDir=0 );


	// Inserting controls and ::com::sun::star::form::Forms (htmlform.cxx)

	// Add a Draw object to the docuement
	void InsertDrawObject( SdrObject* pNewDrawObj, const Size& rSpace,
                               sal_Int16 eVertOri,
                               sal_Int16 eHoriOri,
                               SfxItemSet& rCSS1ItemSet,
                               SvxCSS1PropertyInfo& rCSS1PropInfo,
                               sal_Bool bHidden=sal_False );
        ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >  InsertControl( const ::com::sun::star::uno::Reference< ::com::sun::star::form::XFormComponent > & rFormComp,
        const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > & rFCompPropSet,
        const Size& rSize,
        sal_Int16 eVertOri,
        sal_Int16 eHoriOri,
						SfxItemSet& rCSS1ItemSet,
						SvxCSS1PropertyInfo& rCSS1PropInfo,
						const SvxMacroTableDtor& rMacroTbl,
						const SvStringsDtor& rUnoMacroTbl,
						const SvStringsDtor& rUnoMacroParamTbl,
						sal_Bool bSetPropSet = sal_True,
						sal_Bool bHidden = sal_False );
	void SetControlSize( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > & rShape, const Size& rTextSz,sal_Bool bMinWidth, sal_Bool bMinHeight, int nToken );
	void SetPendingControlSize( int nToken );

public:
	void ResizeDrawObject( SdrObject* pObj, SwTwips nWidth );
private:
	void RegisterDrawObjectToTable( HTMLTable *pCurTable, SdrObject* pObj, sal_uInt8 nWidth );


	// start a new shape
	void NewForm( sal_Bool bAppend=sal_True );
	void EndForm( sal_Bool bAppend=sal_True );

	// the insertion methods for <INPUT>, <TEXTAREA> and <SELECT>
	void InsertInput();

	void NewTextArea();
	void InsertTextAreaText( sal_uInt16 nToken );
	void EndTextArea();

	void NewSelect();
	void InsertSelectOption();
	void InsertSelectText();
	void EndSelect();

	// Insert tables (htmltab.cxx)

public:// is needed in tables

	// insert a box content after the specified node
	const SwStartNode *InsertTableSection( const SwStartNode *pPrevStNd );

	// Insert a box content at the end of the table containing the PaM
	// and move the PaM into the cell
	const SwStartNode *InsertTableSection( sal_uInt16 nPoolId );

	// Insert methods for the various table tags
	HTMLTableCnts *InsertTableContents( sal_Bool bHead );

private:
	// Create a section for the prefix of the table heading
	// create
	SwStartNode *InsertTempTableCaptionSection();

	void BuildTableCell( HTMLTable *pTable, sal_Bool bReadOptions, sal_Bool bHead );
	void BuildTableRow( HTMLTable *pTable, sal_Bool bReadOptions,SvxAdjust eGrpAdjust, sal_Int16 eVertOri );
	void BuildTableSection( HTMLTable *pTable, sal_Bool bReadOptions, sal_Bool bHead );
	void BuildTableColGroup( HTMLTable *pTable, sal_Bool bReadOptions );
	void BuildTableCaption( HTMLTable *pTable );
	HTMLTable *BuildTable( SvxAdjust eCellAdjust, sal_Bool bIsParentHead = sal_False, sal_Bool bHasParentSection=sal_True, sal_Bool bIsInMulticol = sal_False, sal_Bool bHasToFlow = sal_False );


// miscellaneous ...
	void ParseMoreMetaOptions();

	sal_Bool FileDownload( const String& rURL, String& rStr );
	void InsertLink();

	void InsertIDOption();
	void InsertLineBreak();
	void InsertHorzRule();

	void FillEndNoteInfo( const String& rContent );
	void FillFootNoteInfo( const String& rContent );
	void InsertFootEndNote( const String& rName, sal_Bool bEndNote, sal_Bool bFixed );
	void FinishFootEndNote();
	void InsertFootEndNoteText();
	SwNodeIndex *GetFootEndNoteSection( const String& rName );
	void DeleteFootEndNoteImpl();

	// Remove line break at the end of a paragraph
	xub_StrLen StripTrailingLF();


	// Remove an empty paragraph at the PaM position.
	sal_Bool HasCurrentParaFlys( sal_Bool bNoSurroundOnly = sal_False, sal_Bool bSurroundOnly = sal_False ) const;

public:// is needed in tables

	// generate a BrushItem (with new) or 0
	SvxBrushItem* CreateBrushItem( const Color *pColor,
                                       const String &rImageURL,
                                       const String &rStyle,
                                       const String &rId,
                                       const String &rClass );

protected:
	// is called for every token that is recognized in CallParser
	virtual void NextToken( int nToken );
	virtual ~SwHTMLParser();

	// if the doc is deleted, the parser has to be deleted as well
	virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew );

    virtual void AddMetaUserDefined( ::rtl::OUString const & i_rMetaName );

public:

	SwHTMLParser( SwDoc* pD, const SwPaM& rCrsr, SvStream& rIn,
                    const String& rFileName,
                    const String& rBaseURL,
                    int bReadNewDoc = sal_True,
                    SfxMedium* pMed = 0, sal_Bool bReadUTF8 = sal_False,
                    sal_Bool bIgnoreHTMLComments = sal_False );

	virtual SvParserState CallParser();// Calling the parser


	sal_uInt16 ToTwips( sal_uInt16 nPixel ) const;

	// for asynchronous reading from the SvStream
	virtual void Continue( int nToken );

    virtual bool ParseMetaOptions( const ::com::sun::star::uno::Reference<::com::sun::star::document::XDocumentProperties>&, SvKeyValueIterator* );
};


struct SwPendingStackData
{
	virtual ~SwPendingStackData() {}
};

struct SwPendingStack
{
	int nToken;
	SwPendingStackData* pData;
	SwPendingStack* pNext;

	SwPendingStack( int nTkn, SwPendingStack* pNxt )
		: nToken( nTkn ), pData( 0 ), pNext( pNxt )
		{}
};

inline void _HTMLAttr::SetStart( const SwPosition& rPos )
{
	nSttPara = rPos.nNode;
	nSttCntnt = rPos.nContent.GetIndex();
	nEndPara = nSttPara;
	nEndCntnt = nSttCntnt;
}

inline void _HTMLAttrContext::SetMargins( sal_uInt16 nLeft, sal_uInt16 nRight, short nIndent )
{
	nLeftMargin = nLeft;
	nRightMargin = nRight;
	nFirstLineIndent = nIndent;
	bLRSpaceChanged = sal_True;
}

inline void _HTMLAttrContext::GetMargins( sal_uInt16& nLeft,
                                          sal_uInt16& nRight,
                                          short& nIndent ) const
{
	if( bLRSpaceChanged )
	{
		nLeft = nLeftMargin;
		nRight = nRightMargin;
		nIndent = nFirstLineIndent;
	}
}

inline void _HTMLAttrContext::SetULSpace( sal_uInt16 nUpper, sal_uInt16 nLower )
{
	nUpperSpace = nUpper;
	nLowerSpace = nLower;
	bULSpaceChanged = sal_True;
}

inline void _HTMLAttrContext::GetULSpace( sal_uInt16& rUpper,
										  sal_uInt16& rLower ) const
{
	if( bULSpaceChanged )
	{
		rUpper = nUpperSpace;
		rLower = nLowerSpace;
	}
}

inline sal_Bool SwHTMLParser::HasStyleOptions( const String &rStyle,
                                               const String &rId,
                                               const String &rClass,
                                               const String *pLang,
                                               const String *pDir )
{
	return rStyle.Len() || rId.Len() || rClass.Len() ||
		   (pLang && pLang->Len()) || (pDir && pDir->Len());
}

inline const _HTMLAttrContext *SwHTMLParser::GetTopContext() const
{
	return aContexts.Count() > nContextStMin
			? aContexts[aContexts.Count()-1] : 0;
}

inline void SwHTMLParser::PushContext( _HTMLAttrContext *pCntxt )
{
	aContexts.Insert( pCntxt, aContexts.Count() );
}


#endif


