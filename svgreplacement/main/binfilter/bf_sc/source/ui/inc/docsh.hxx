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


#ifndef SC_DOCSHELL_HXX
#define SC_DOCSHELL_HXX


#ifndef _SFX_OBJSH_HXX //autogen
#include <bf_sfx2/objsh.hxx>
#endif

#ifndef _SFX_INTERNO_HXX //autogen
#include <bf_sfx2/interno.hxx>
#endif

#ifndef _SFX_OBJFAC_HXX //autogen
#include <bf_sfx2/docfac.hxx>
#endif

#include <bf_sfx2/printer.hxx>
#include <bf_svtools/lstner.hxx>

#ifndef SC_SCDLL_HXX
#include "scdll.hxx"
#endif

#include "document.hxx"
#include "shellids.hxx"
#include "refreshtimer.hxx"
class PrintDialog;
class INetURLObject;
class VirtualDevice;
namespace binfilter {
class FontList;
class SfxStyleSheetBasePool;
class SfxStyleSheetHint;

class ScEditEngineDefaulter;
struct ChartSelectionInfo;

class ScPaintItem;
class ScViewData;
class ScDocFunc;
class ScPivot;
class ScDrawLayer;
class ScSbxDocHelper;
class ScAutoStyleList;
class ScRange;
class ScMarkData;
class ScPaintLockData;
class ScJobSetup;
class ScChangeAction;
class ScImportOptions;
class ScDocShellModificator;

//==================================================================

//enum ScDBFormat { SC_FORMAT_SDF, SC_FORMAT_DBF };

									// Extra-Flags fuer Repaint
#define SC_PF_LINES         1
#define SC_PF_TESTMERGE     2

class ScDocShell: public SfxObjectShell, public SfxInPlaceObject, public SfxListener
{
	static const sal_Char __FAR_DATA pStarCalcDoc[];
	static const sal_Char __FAR_DATA pStyleName[];

	ScDocument          aDocument;

	String				aDdeTextFmt;
	String				aConvFilterName; //@ #BugId	54198

	double				nPrtToScreenFactor;
	FontList*			pFontList;
	ScDocFunc*			pDocFunc;

	//SfxObjectCreateMode	eShellMode;

	BOOL				bIsInplace;			// wird von der View gesetzt
	BOOL				bHeaderOn;
	BOOL				bFooterOn;
	BOOL				bNoInformLost;
	BOOL				bIsEmpty;
	BOOL				bIsInUndo;
	BOOL				bDocumentModifiedPending;
	USHORT				nDocumentLock;
    sal_Int16           nCanUpdate;  // stores the UpdateDocMode from loading a document till update links
    BOOL                bUpdateEnabled;

	ScSbxDocHelper* 	pDocHelper;

	ScAutoStyleList*	pAutoStyleList;
	ScPaintLockData*	pPaintLockData;
	ScJobSetup*			pOldJobSetup;

	VirtualDevice*		pVirtualDevice_100th_mm;

    ScDocShellModificator* pModificator; // #109979#; is used to load XML (created in BeforeXMLLoading and destroyed in AfterXMLLoading)

    String              aUserData;      // #116578# loaded manually, as no view is created

	void			InitItems();
	void			InitOptions();
	void			ResetDrawObjectShell();

	BOOL			LoadCalc( SvStorage* pStor );
	BOOL			SaveXML( SfxMedium* pMedium, SvStorage* pStor );
	void			UpdateAllRowHeights();

	void			RemoveUnknownObjects();



	void			LockPaint_Impl(BOOL bDoc);
	void			UnlockPaint_Impl(BOOL bDoc);
	void			LockDocument_Impl(USHORT nNew);
	void			UnlockDocument_Impl(USHORT nNew);

protected:

	virtual void SFX_NOTIFY( SfxBroadcaster& rBC, const TypeId& rBCType,
						 const SfxHint& rHint, const TypeId& rHintType );

public:
					TYPEINFO();

					SFX_DECL_OBJECTFACTORY_DLL( ScDocShell, SC_DLL() );

					ScDocShell( const ScDocShell& rDocShell );
					ScDocShell( SfxObjectCreateMode eMode = SFX_CREATE_MODE_EMBEDDED );
					~ScDocShell();


	virtual void	FillClass( SvGlobalName * pClassName,
							   ULONG * pFormat,
							   String * pAppName,
							   String * pFullTypeName,
							   String * pShortTypeName,
							   long nFileFormat = SOFFICE_FILEFORMAT_CURRENT ) const;

	virtual BOOL    InitNew( SvStorage * );
	virtual BOOL    Load( SvStorage * );
	virtual BOOL    ConvertFrom( SfxMedium &rMedium );
	virtual void    HandsOff();
	virtual BOOL    Save();
	virtual BOOL    SaveAs( SvStorage * pNewStor );

	virtual BOOL    SaveCompleted( SvStorage * pNewStor );		// SfxInPlaceObject
	virtual BOOL	DoSaveCompleted( SfxMedium * pNewStor);		// SfxObjectShell


	virtual void    SetVisArea( const Rectangle & rVisArea );
	virtual Rectangle GetVisArea( USHORT nAspect ) const;

	virtual Printer* GetDocumentPrinter();

	virtual void	SetModified( BOOL = TRUE );

	void			SetVisAreaOrSize( const Rectangle& rVisArea, BOOL bModifyStart );



	ScDocument*     GetDocument()	{ return &aDocument; }
	ScDocFunc&		GetDocFunc()	{ return *pDocFunc; }

	SfxPrinter*		GetPrinter();
	USHORT			SetPrinter( SfxPrinter* pNewPrinter, USHORT nDiffFlags = SFX_PRINTER_ALL );



	ScDrawLayer*	MakeDrawLayer();


                    /// Protect/unprotect ChangeTrack and return <TRUE/> if
                    /// protection was successfully changed.
                    /// If bJustQueryIfProtected==TRUE protection is not
                    /// changed and <TRUE/> is returned if not protected or


	void			LoadStylesArgs( ScDocShell& rSource, BOOL bReplace, BOOL bCellStyles, BOOL bPageStyles );

	void			PageStyleModified( const String& rStyleName, BOOL bApi );

	void			NotifyStyle( const SfxStyleSheetHint& rHint );

	Window*			GetDialogParent();
	void			ErrorMessage( USHORT nGlobStrId );
	BOOL			IsEditable() const;

	BOOL			AdjustRowHeight( USHORT nStartRow, USHORT nEndRow, USHORT nTab );

	void			DoConsolidate( const ScConsolidateParam& rParam, BOOL bRecord = TRUE );
	void			UseScenario( USHORT nTab, const String& rName, BOOL bRecord = TRUE );
	USHORT			MakeScenario( USHORT nTab, const String& rName, const String& rComment,
									const Color& rColor, USHORT nFlags,
									ScMarkData& rMark, BOOL bRecord = TRUE );
	void			ModifyScenario( USHORT nTab, const String& rName, const String& rComment,
									const Color& rColor, USHORT nFlags );
	BOOL			MoveTable( USHORT nSrcTab, USHORT nDestTab, BOOL bCopy, BOOL bRecord );

	void			DoRecalc( BOOL bApi );
	void			DoHardRecalc( BOOL bApi );


	void			DBAreaDeleted( USHORT nTab, USHORT nX1, USHORT nY1, USHORT nX2, USHORT nY2 );
	ScDBData*		GetDBData( const ScRange& rMarked, ScGetDBMode eMode, BOOL bForceMark );

	void			UpdateLinks();			// Link-Eintraege aktuallisieren


	void            PostPaint( USHORT nStartCol, USHORT nStartRow, USHORT nStartTab,
							USHORT nEndCol, USHORT nEndRow, USHORT nEndTab, USHORT nPart,
							USHORT nExtFlags = 0 );
	void            PostPaint( const ScRange& rRange, USHORT nPart, USHORT nExtFlags = 0 );

	void            PostPaintCell( USHORT nCol, USHORT nRow, USHORT nTab );
	void            PostPaintGridAll();
	void            PostPaintExtras();

	void            PostDataChanged();

	void			SetDocumentModified( BOOL bIsModified = TRUE );
	void			SetDrawModified( BOOL bIsModified = TRUE );

	void			LockPaint();
	void			UnlockPaint();
	USHORT			GetLockCount() const;
	void			SetLockCount(USHORT nNew);

	void			LockDocument();
	void			UnlockDocument();


	virtual SfxStyleSheetBasePool*	GetStyleSheetPool();

	BOOL			IsInUndo() const				{ return bIsInUndo; }

	void			CalcOutputFactor();
	double			GetOutputFactor() const;


	const String& GetDdeTextFmt() const { return aDdeTextFmt; }

	ScSbxDocHelper* GetDocHelperObject() { return pDocHelper; }

	void			SetDocumentModifiedPending( BOOL bVal )
						{ bDocumentModifiedPending = bVal; }
	BOOL			IsDocumentModifiedPending() const
						{ return bDocumentModifiedPending; }

    BOOL            IsUpdateEnabled() const
                        { return bUpdateEnabled; }
    void            SetUpdateEnabled(BOOL bValue)
                        { bUpdateEnabled = bValue; }

	VirtualDevice*	GetVirtualDevice_100th_mm();

	static ScViewData* GetViewData();
	static USHORT	   GetCurTab();

	static String	GetOwnFilterName();
	static String	GetWebQueryFilterName();
	static String	GetAsciiFilterName();
	static String	GetLotusFilterName();
	static String	GetDBaseFilterName();
	static String	GetDifFilterName();
	static BOOL		HasAutomaticTableName( const String& rFilter );

	DECL_LINK( RefreshDBDataHdl, ScDBData* );

    void            BeforeXMLLoading();
    void            AfterXMLLoading(sal_Bool bRet);

    const String&   GetUserData() const { return aUserData; }   // #116578#
};

SO2_DECL_REF(ScDocShell)
SO2_IMPL_REF(ScDocShell)


// Vor Modifizierungen des Dokuments anlegen und danach zerstoeren.
// Merkt sich im Ctor AutoCalcShellDisabled und IdleDisabled, schaltet sie ab
// und stellt sie im Dtor wieder her, AutoCalcShellDisabled ggbf. auch vor
// einem ScDocShell SetDocumentModified.
// SetDocumentModified hierdran aufrufen statt an der ScDocShell.
// Im Dtor wird wenn ScDocShell bDocumentModifiedPending gesetzt ist und
// bAutoCalcShellDisabled nicht gesetzt ist ein SetDocumentModified gerufen.
class ScDocShellModificator
{
			ScDocShell&		rDocShell;
	ScRefreshTimerProtector	aProtector;
			BOOL			bAutoCalcShellDisabled;
			BOOL			bIdleDisabled;

							// not implemented
							ScDocShellModificator( const ScDocShellModificator& );
	ScDocShellModificator&	operator=( const ScDocShellModificator& );

public:
							ScDocShellModificator( ScDocShell& );
							~ScDocShellModificator();
			void			SetDocumentModified();
};



} //namespace binfilter
#endif


