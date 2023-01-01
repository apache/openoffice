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
#include "precompiled_sc.hxx"


#define _ZFORLIST_DECLARE_TABLE
#include "scitems.hxx"
#include <editeng/eeitem.hxx>

#include <tools/string.hxx>
#include <editeng/editobj.hxx>
#include <editeng/editstat.hxx>
#include <editeng/frmdiritem.hxx>
#include <editeng/langitem.hxx>
#include <sfx2/linkmgr.hxx>
#include <editeng/scripttypeitem.hxx>
#include <editeng/unolingu.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/printer.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/viewsh.hxx>
#include <svl/flagitem.hxx>
#include <svl/intitem.hxx>
#define _SVSTDARR_USHORTS
#include <svl/svstdarr.hxx>
#include <svl/zforlist.hxx>
#include <svl/zformat.hxx>
#include <unotools/misccfg.hxx>
#include <sfx2/app.hxx>
#include <unotools/transliterationwrapper.hxx>
#include <unotools/securityoptions.hxx>

#include <vcl/virdev.hxx>
#include <vcl/msgbox.hxx>

#include <com/sun/star/i18n/TransliterationModulesExtra.hpp>

#include "inputopt.hxx"
#include "global.hxx"
#include "table.hxx"
#include "column.hxx"
#include "cell.hxx"
#include "poolhelp.hxx"
#include "docpool.hxx"
#include "stlpool.hxx"
#include "stlsheet.hxx"
#include "docoptio.hxx"
#include "viewopti.hxx"
#include "scextopt.hxx"
#include "rechead.hxx"
#include "ddelink.hxx"
#include "scmatrix.hxx"
#include "arealink.hxx"
#include "dociter.hxx"
#include "patattr.hxx"
#include "hints.hxx"
#include "editutil.hxx"
#include "progress.hxx"
#include "document.hxx"
#include "chartlis.hxx"
#include "chartlock.hxx"
#include "refupdat.hxx"
#include "validat.hxx"		// fuer HasMacroCalls
#include "markdata.hxx"
#include "scmod.hxx"
#include "printopt.hxx"
#include "externalrefmgr.hxx"
#include "globstr.hrc"
#include "sc.hrc"
#include "charthelper.hxx"
#include "dpobject.hxx"
#include "docuno.hxx"

#define GET_SCALEVALUE(set,id) 	((const SfxUInt16Item&)(set.Get( id ))).GetValue()

//	states for online spelling in the visible range (0 is set initially)
#define VSPL_START	0
#define VSPL_DONE	1


// STATIC DATA -----------------------------------------------------------

//------------------------------------------------------------------------

void ScDocument::ImplCreateOptions()
{
	pDocOptions  = new ScDocOptions();
	pViewOptions = new ScViewOptions();
}

//------------------------------------------------------------------------

void ScDocument::ImplDeleteOptions()
{
	delete pDocOptions;
	delete pViewOptions;
	delete pExtDocOptions;
}

//------------------------------------------------------------------------

SfxPrinter* ScDocument::GetPrinter(sal_Bool bCreateIfNotExist)
{
	if ( !pPrinter && bCreateIfNotExist )
	{
        SfxItemSet* pSet =
			new SfxItemSet( *xPoolHelper->GetDocPool(),
                            SID_PRINTER_NOTFOUND_WARN,  SID_PRINTER_NOTFOUND_WARN,
                            SID_PRINTER_CHANGESTODOC,   SID_PRINTER_CHANGESTODOC,
                            SID_PRINT_SELECTEDSHEET,    SID_PRINT_SELECTEDSHEET,
                            SID_SCPRINTOPTIONS,         SID_SCPRINTOPTIONS,
							NULL );

        ::utl::MiscCfg aMisc;
		sal_uInt16 nFlags = 0;
        if ( aMisc.IsPaperOrientationWarning() )
			nFlags |= SFX_PRINTER_CHG_ORIENTATION;
		if ( aMisc.IsPaperSizeWarning() )
			nFlags |= SFX_PRINTER_CHG_SIZE;
		pSet->Put( SfxFlagItem( SID_PRINTER_CHANGESTODOC, nFlags ) );
		pSet->Put( SfxBoolItem( SID_PRINTER_NOTFOUND_WARN, aMisc.IsNotFoundWarning() ) );

		pPrinter = new SfxPrinter( pSet );
		pPrinter->SetMapMode( MAP_100TH_MM );
		UpdateDrawPrinter();
		pPrinter->SetDigitLanguage( SC_MOD()->GetOptDigitLanguage() );
	}

	return pPrinter;
}

//------------------------------------------------------------------------

void ScDocument::SetPrinter( SfxPrinter* pNewPrinter )
{
	if ( pNewPrinter == pPrinter )
	{
		//	#i6706# SetPrinter is called with the same printer again if
		//	the JobSetup has changed. In that case just call UpdateDrawPrinter
		//	(SetRefDevice for drawing layer) because of changed text sizes.
		UpdateDrawPrinter();
	}
	else
	{
		SfxPrinter* pOld = pPrinter;
		pPrinter = pNewPrinter;
		UpdateDrawPrinter();
		pPrinter->SetDigitLanguage( SC_MOD()->GetOptDigitLanguage() );
		delete pOld;
	}
	InvalidateTextWidth(NULL, NULL, sal_False);     // in both cases
}

//------------------------------------------------------------------------

void ScDocument::SetPrintOptions()
{
	if ( !pPrinter ) GetPrinter(); // setzt pPrinter
	DBG_ASSERT( pPrinter, "Error in printer creation :-/" );

	if ( pPrinter )
	{
        ::utl::MiscCfg aMisc;
		SfxItemSet aOptSet( pPrinter->GetOptions() );

		sal_uInt16 nFlags = 0;
        if ( aMisc.IsPaperOrientationWarning() )
			nFlags |= SFX_PRINTER_CHG_ORIENTATION;
		if ( aMisc.IsPaperSizeWarning() )
			nFlags |= SFX_PRINTER_CHG_SIZE;
		aOptSet.Put( SfxFlagItem( SID_PRINTER_CHANGESTODOC, nFlags ) );
		aOptSet.Put( SfxBoolItem( SID_PRINTER_NOTFOUND_WARN, aMisc.IsNotFoundWarning() ) );

		pPrinter->SetOptions( aOptSet );
	}
}

//------------------------------------------------------------------------

VirtualDevice* ScDocument::GetVirtualDevice_100th_mm()
{
	if (!pVirtualDevice_100th_mm)
	{
//		pVirtualDevice_100th_mm = new VirtualDevice;
//		pVirtualDevice_100th_mm->SetMapMode( MAP_100TH_MM );

		pVirtualDevice_100th_mm = new VirtualDevice( 1 );
		pVirtualDevice_100th_mm->SetReferenceDevice(VirtualDevice::REFDEV_MODE_MSO1);
		MapMode aMapMode( pVirtualDevice_100th_mm->GetMapMode() );
		aMapMode.SetMapUnit( MAP_100TH_MM );
		pVirtualDevice_100th_mm->SetMapMode( aMapMode );
	}
	return pVirtualDevice_100th_mm;
}

OutputDevice* ScDocument::GetRefDevice()
{
	// Create printer like ref device, see Writer...
	OutputDevice* pRefDevice = NULL;
    if ( SC_MOD()->GetInputOptions().GetTextWysiwyg() )
		pRefDevice = GetPrinter();
	else
		pRefDevice = GetVirtualDevice_100th_mm();
	return pRefDevice;
}

//------------------------------------------------------------------------

void ScDocument::ModifyStyleSheet( SfxStyleSheetBase& rStyleSheet,
								   const SfxItemSet&  rChanges )
{
	SfxItemSet& rSet = rStyleSheet.GetItemSet();

	switch ( rStyleSheet.GetFamily() )
	{
		case SFX_STYLE_FAMILY_PAGE:
			{
				const sal_uInt16 nOldScale		  = GET_SCALEVALUE(rSet,ATTR_PAGE_SCALE);
				const sal_uInt16 nOldScaleToPages = GET_SCALEVALUE(rSet,ATTR_PAGE_SCALETOPAGES);
				rSet.Put( rChanges );
				const sal_uInt16 nNewScale		  = GET_SCALEVALUE(rSet,ATTR_PAGE_SCALE);
				const sal_uInt16 nNewScaleToPages = GET_SCALEVALUE(rSet,ATTR_PAGE_SCALETOPAGES);

				if ( (nOldScale != nNewScale) || (nOldScaleToPages != nNewScaleToPages) )
					InvalidateTextWidth( rStyleSheet.GetName() );

                if( SvtLanguageOptions().IsCTLFontEnabled() )
                {
                    const SfxPoolItem *pItem = NULL;
                    if( rChanges.GetItemState(ATTR_WRITINGDIR, sal_True, &pItem ) == SFX_ITEM_SET )
                        ScChartHelper::DoUpdateAllCharts( this );
                }
			}
			break;

		case SFX_STYLE_FAMILY_PARA:
			{
				sal_Bool bNumFormatChanged;
				if ( ScGlobal::CheckWidthInvalidate( bNumFormatChanged,
						rSet, rChanges ) )
					InvalidateTextWidth( NULL, NULL, bNumFormatChanged );

                for (SCTAB nTab=0; nTab<=MAXTAB; ++nTab)
                    if (pTab[nTab] && pTab[nTab]->IsStreamValid())
                        pTab[nTab]->SetStreamValid( sal_False );

				sal_uLong nOldFormat =
					((const SfxUInt32Item*)&rSet.Get(
					ATTR_VALUE_FORMAT ))->GetValue();
				sal_uLong nNewFormat =
					((const SfxUInt32Item*)&rChanges.Get(
					ATTR_VALUE_FORMAT ))->GetValue();
				LanguageType eNewLang, eOldLang;
				eNewLang = eOldLang = LANGUAGE_DONTKNOW;
				if ( nNewFormat != nOldFormat )
				{
					SvNumberFormatter* pFormatter = GetFormatTable();
					eOldLang = pFormatter->GetEntry( nOldFormat )->GetLanguage();
					eNewLang = pFormatter->GetEntry( nNewFormat )->GetLanguage();
				}

				// Bedeutung der Items in rChanges:
				//	Item gesetzt	- Aenderung uebernehmen
				//	Dontcare		- Default setzen
				//	Default			- keine Aenderung
				// ("keine Aenderung" geht nicht mit PutExtended, darum Schleife)
				for (sal_uInt16 nWhich = ATTR_PATTERN_START; nWhich <= ATTR_PATTERN_END; nWhich++)
				{
					const SfxPoolItem* pItem;
					SfxItemState eState = rChanges.GetItemState( nWhich, sal_False, &pItem );
					if ( eState == SFX_ITEM_SET )
						rSet.Put( *pItem );
					else if ( eState == SFX_ITEM_DONTCARE )
						rSet.ClearItem( nWhich );
					// bei Default nichts
				}

				if ( eNewLang != eOldLang )
					rSet.Put(
						SvxLanguageItem( eNewLang, ATTR_LANGUAGE_FORMAT ) );
			}
			break;
        default:
        {
            // added to avoid warnings
        }
	}
}

//------------------------------------------------------------------------

void ScDocument::CopyStdStylesFrom( ScDocument* pSrcDoc )
{
    // #b5017505# number format exchange list has to be handled here, too
    NumFmtMergeHandler aNumFmtMergeHdl(this, pSrcDoc);
	xPoolHelper->GetStylePool()->CopyStdStylesFrom( pSrcDoc->xPoolHelper->GetStylePool() );
}

//------------------------------------------------------------------------

void ScDocument::InvalidateTextWidth( const String& rStyleName )
{
	const SCTAB nCount = GetTableCount();
	for ( SCTAB i=0; i<nCount && pTab[i]; i++ )
		if ( pTab[i]->GetPageStyle() == rStyleName )
			InvalidateTextWidth( i );
}

//------------------------------------------------------------------------

void ScDocument::InvalidateTextWidth( SCTAB nTab )
{
	ScAddress aAdrFrom( 0,	  0,        nTab );
	ScAddress aAdrTo  ( MAXCOL, MAXROW, nTab );
    InvalidateTextWidth( &aAdrFrom, &aAdrTo, sal_False );
}

//------------------------------------------------------------------------

sal_Bool ScDocument::IsPageStyleInUse( const String& rStrPageStyle, SCTAB* pInTab )
{
	sal_Bool		 bInUse = sal_False;
	const SCTAB nCount = GetTableCount();
	SCTAB i;

	for ( i = 0; !bInUse && i < nCount && pTab[i]; i++ )
		bInUse = ( pTab[i]->GetPageStyle() == rStrPageStyle );

	if ( pInTab )
		*pInTab = i-1;

	return bInUse;
}

//------------------------------------------------------------------------

sal_Bool ScDocument::RemovePageStyleInUse( const String& rStyle )
{
	sal_Bool bWasInUse = sal_False;
	const SCTAB nCount = GetTableCount();

	for ( SCTAB i=0; i<nCount && pTab[i]; i++ )
		if ( pTab[i]->GetPageStyle() == rStyle )
		{
			bWasInUse = sal_True;
			pTab[i]->SetPageStyle( ScGlobal::GetRscString(STR_STYLENAME_STANDARD) );
		}

	return bWasInUse;
}

sal_Bool ScDocument::RenamePageStyleInUse( const String& rOld, const String& rNew )
{
	sal_Bool bWasInUse = sal_False;
	const SCTAB nCount = GetTableCount();

	for ( SCTAB i=0; i<nCount && pTab[i]; i++ )
		if ( pTab[i]->GetPageStyle() == rOld )
		{
			bWasInUse = sal_True;
			pTab[i]->SetPageStyle( rNew );
		}

	return bWasInUse;
}

//------------------------------------------------------------------------

sal_uInt8 ScDocument::GetEditTextDirection(SCTAB nTab) const
{
	EEHorizontalTextDirection eRet = EE_HTEXTDIR_DEFAULT;

	String aStyleName = GetPageStyle( nTab );
	SfxStyleSheetBase* pStyle = xPoolHelper->GetStylePool()->Find( aStyleName, SFX_STYLE_FAMILY_PAGE );
	if ( pStyle )
	{
		SfxItemSet& rStyleSet = pStyle->GetItemSet();
		SvxFrameDirection eDirection = (SvxFrameDirection)
			((const SvxFrameDirectionItem&)rStyleSet.Get( ATTR_WRITINGDIR )).GetValue();

		if ( eDirection == FRMDIR_HORI_LEFT_TOP )
			eRet = EE_HTEXTDIR_L2R;
		else if ( eDirection == FRMDIR_HORI_RIGHT_TOP )
			eRet = EE_HTEXTDIR_R2L;
		// else (invalid for EditEngine): keep "default"
	}

    return sal::static_int_cast<sal_uInt8>(eRet);
}

//------------------------------------------------------------------------

void ScDocument::InvalidateTextWidth( const ScAddress* pAdrFrom, const ScAddress* pAdrTo,
                                      sal_Bool bNumFormatChanged )
{
    sal_Bool bBroadcast = (bNumFormatChanged && GetDocOptions().IsCalcAsShown() && !IsImportingXML() && !IsClipboard());
	if ( pAdrFrom && !pAdrTo )
	{
		const SCTAB nTab = pAdrFrom->Tab();

		if ( pTab[nTab] )
            pTab[nTab]->InvalidateTextWidth( pAdrFrom, NULL, bNumFormatChanged, bBroadcast );
	}
	else
	{
		const SCTAB nTabStart = pAdrFrom ? pAdrFrom->Tab() : 0;
		const SCTAB nTabEnd   = pAdrTo   ? pAdrTo->Tab()   : MAXTAB;

		for ( SCTAB nTab=nTabStart; nTab<=nTabEnd; nTab++ )
			if ( pTab[nTab] )
                pTab[nTab]->InvalidateTextWidth( pAdrFrom, pAdrTo, bNumFormatChanged, bBroadcast );
	}
}

//------------------------------------------------------------------------

#define CALCMAX					1000	// Berechnungen
#define ABORT_EVENTS			(INPUT_ANY & ~INPUT_TIMER & ~INPUT_OTHER)

sal_Bool ScDocument::IdleCalcTextWidth()			// sal_True = demnaechst wieder versuchen
{
    // #i75610# if a printer hasn't been set or created yet, don't create one for this
    if ( bIdleDisabled || IsInLinkUpdate() || GetPrinter(sal_False) == NULL )
		return sal_False;
	bIdleDisabled = sal_True;

// sal_uLong nMs = 0;
// sal_uInt16 nIter = 0;

	const sal_uLong			nStart	 = Time::GetSystemTicks();
	double				nPPTX	 = 0.0;
	double 				nPPTY	 = 0.0;
	OutputDevice*		pDev	 = NULL;
	MapMode				aOldMap;
	ScStyleSheet*		pStyle	 = NULL;
	ScColumnIterator*	pColIter = NULL;
	ScTable*			pTable	 = NULL;
	ScColumn*			pColumn	 = NULL;
	ScBaseCell*			pCell	 = NULL;
	SCTAB				nTab  	 = aCurTextWidthCalcPos.Tab();
	SCROW				nRow   	 = aCurTextWidthCalcPos.Row();
	SCsCOL				nCol  	 = aCurTextWidthCalcPos.Col();
	sal_uInt16				nRestart = 0;
	sal_uInt16 				nZoom	 = 0;
	sal_Bool				bNeedMore= sal_False;

	if ( !ValidRow(nRow) )
		nRow = 0, nCol--;
	if ( nCol < 0 )
		nCol = MAXCOL, nTab++;
	if ( !ValidTab(nTab) || !pTab[nTab] )
		nTab = 0;

//	DBG_ERROR( String("Start = ") + String(nTab) + String(',') + String(nCol) + String(',') + String(nRow)  );

	//	SearchMask/Family muss gemerkt werden,
	//	damit z.B. der Organizer nicht durcheinanderkommt, wenn zwischendurch eine
	//	Query-Box aufgemacht wird !!!

	ScStyleSheetPool* pStylePool = xPoolHelper->GetStylePool();
	sal_uInt16 nOldMask = pStylePool->GetSearchMask();
	SfxStyleFamily eOldFam = pStylePool->GetSearchFamily();

	pTable = pTab[nTab];
	pStylePool->SetSearchMask( SFX_STYLE_FAMILY_PAGE, SFXSTYLEBIT_ALL );
	pStyle = (ScStyleSheet*)pStylePool->Find( pTable->aPageStyle,
											  SFX_STYLE_FAMILY_PAGE );

    DBG_ASSERT( pStyle, "Missing StyleSheet :-/" );

	sal_Bool bProgress = sal_False;
	if ( pStyle && 0 == GET_SCALEVALUE(pStyle->GetItemSet(),ATTR_PAGE_SCALETOPAGES) )
	{
		sal_uInt16 nCount = 0;

		nZoom	 = GET_SCALEVALUE(pStyle->GetItemSet(),ATTR_PAGE_SCALE);
		Fraction aZoomFract( nZoom, 100 );
		pColumn  = &pTable->aCol[nCol];
		pColIter = new ScColumnIterator( pColumn, nRow, MAXROW );

		while ( (nZoom > 0) && (nCount < CALCMAX) && (nRestart < 2) )
		{
			if ( pColIter->Next( nRow, pCell ) )
			{
				if ( TEXTWIDTH_DIRTY ==	pCell->GetTextWidth() )
				{
					if ( !pDev )
					{
						pDev = GetPrinter();
						aOldMap = pDev->GetMapMode();
						pDev->SetMapMode( MAP_PIXEL );	// wichtig fuer GetNeededSize

						Point aPix1000 = pDev->LogicToPixel( Point(1000,1000), MAP_TWIP );
						nPPTX = aPix1000.X() / 1000.0;
						nPPTY = aPix1000.Y() / 1000.0;
					}
					if ( !bProgress && pCell->GetCellType() == CELLTYPE_FORMULA
					  && ((ScFormulaCell*)pCell)->GetDirty() )
					{
						ScProgress::CreateInterpretProgress( this, sal_False );
						bProgress = sal_True;
					}

//					DBG_ERROR( String("t,c,r = ") + String(nTab) + String(',') + String(nCol) + String(',') + String(nRow)  );
//					DBG_ERROR( String("nOldWidth = ") + String(pCell->GetTextWidth()) );

					sal_uInt16 nNewWidth = (sal_uInt16)GetNeededSize( nCol, nRow, nTab,
															  pDev, nPPTX, nPPTY,
															  aZoomFract,aZoomFract, sal_True,
															  sal_True );	// bTotalSize

//					DBG_ERROR( String("nNewWidth = ") + String(nNewWidth) );

					pCell->SetTextWidth( nNewWidth );

					bNeedMore = sal_True;
				}
			}
			else
			{
				sal_Bool bNewTab = sal_False;

				nRow = 0;
				nCol--;

				if ( nCol < 0 )
				{
					nCol = MAXCOL;
					nTab++;
					bNewTab = sal_True;
				}

				if ( !ValidTab(nTab) || !pTab[nTab] )
				{
					nTab = 0;
					nRestart++;
					bNewTab = sal_True;
				}

				if ( nRestart < 2 )
				{
					if ( bNewTab )
					{
						pTable = pTab[nTab];
						pStyle = (ScStyleSheet*)pStylePool->Find( pTable->aPageStyle,
																  SFX_STYLE_FAMILY_PAGE );

						if ( pStyle )
						{
							SfxItemSet& rSet = pStyle->GetItemSet();
							if ( GET_SCALEVALUE( rSet, ATTR_PAGE_SCALETOPAGES ) == 0 )
								nZoom = GET_SCALEVALUE(rSet, ATTR_PAGE_SCALE );
							else
								nZoom = 0;
						}
						else
						{
							DBG_ERROR( "Missing StyleSheet :-/" );
						}
					}

					if ( nZoom > 0 )
					{
						delete pColIter;

						pColumn  = &pTable->aCol[nCol];
						pColIter = new ScColumnIterator( pColumn, nRow, MAXROW );
					}
					else
						nTab++; // Tabelle nicht mit absolutem Zoom -> naechste
				}
			}

// nIter = nCount;

			nCount++;

			// Idle Berechnung abbrechen, wenn Berechnungen laenger als
			// 50ms dauern, oder nach 32 Berechnungen mal nachschauen, ob
			// bestimmte Events anstehen, die Beachtung wuenschen:

// nMs = SysTicksToMs( GetSysTicks() - nStart );

			if (   ( 50L < Time::GetSystemTicks() - nStart )
				|| ( !(nCount&31) && Application::AnyInput( ABORT_EVENTS ) ) )
				nCount = CALCMAX;
		}
	}
	else
		nTab++; // Tabelle nicht mit absolutem Zoom -> naechste

	if ( bProgress )
		ScProgress::DeleteInterpretProgress();

	delete pColIter;

//	DBG_ERROR( String(nCount) + String(" End = ") + String(nTab) + String(',') + String(nCol) + String(',') + String(nRow)  );

	if (pDev)
		pDev->SetMapMode(aOldMap);

	aCurTextWidthCalcPos.SetTab( nTab );
	aCurTextWidthCalcPos.SetRow( nRow );
	aCurTextWidthCalcPos.SetCol( (SCCOL)nCol );

// DBG_ERROR( String(nMs) + String(" ms (") + String(nIter) + String(')') );

	pStylePool->SetSearchMask( eOldFam, nOldMask );
	bIdleDisabled = sal_False;

	return bNeedMore;
}

//------------------------------------------------------------------------

class ScSpellStatus
{
public:
	sal_Bool	bModified;

	ScSpellStatus() : bModified(sal_False) {};

	DECL_LINK (EventHdl, EditStatus*);
};

IMPL_LINK( ScSpellStatus, EventHdl, EditStatus *, pStatus )
{
	sal_uLong nStatus = pStatus->GetStatusWord();
	if ( nStatus & EE_STAT_WRONGWORDCHANGED )
		bModified = sal_True;

	return 0;
}

//	SPELL_MAXCELLS muss mindestens 256 sein, solange am Iterator keine
//	Start-Spalte gesetzt werden kann

//!	SPELL_MAXTEST fuer Timer und Idle unterschiedlich ???

//	SPELL_MAXTEST now divided between visible and rest of document

#define SPELL_MAXTEST_VIS	1
#define SPELL_MAXTEST_ALL	3
#define SPELL_MAXCELLS		256

sal_Bool ScDocument::OnlineSpellInRange( const ScRange& rSpellRange, ScAddress& rSpellPos,
									 sal_uInt16 nMaxTest )
{
	ScEditEngineDefaulter* pEngine = NULL;				//! am Dokument speichern
	SfxItemSet* pDefaults = NULL;
	ScSpellStatus aStatus;

	sal_uInt16 nCellCount = 0;			// Zellen insgesamt
	sal_uInt16 nTestCount = 0;			// Aufrufe Spelling
	sal_Bool bChanged = sal_False;			// Aenderungen?

	SCCOL nCol = rSpellRange.aStart.Col();		// iterator always starts on the left edge
	SCROW nRow = rSpellPos.Row();
	SCTAB nTab = rSpellPos.Tab();
	if ( !pTab[nTab] )							// sheet deleted?
	{
		nTab = rSpellRange.aStart.Tab();
		nRow = rSpellRange.aStart.Row();
		if ( !pTab[nTab] )
		{
			//	may happen for visible range
			return sal_False;
		}
	}
	ScHorizontalCellIterator aIter( this, nTab,
									rSpellRange.aStart.Col(), nRow,
									rSpellRange.aEnd.Col(), rSpellRange.aEnd.Row() );
	ScBaseCell* pCell = aIter.GetNext( nCol, nRow );
	//	skip everything left of rSpellPos:
	while ( pCell && nRow == rSpellPos.Row() && nCol < rSpellPos.Col() )
		pCell = aIter.GetNext( nCol, nRow );

	for (; pCell; pCell = aIter.GetNext(nCol, nRow))
	{
        if (pDPCollection && pDPCollection->HasDPTable(nCol, nRow, nTab))
            // Don't spell check within datapilot table.
            continue;

		CellType eType = pCell->GetCellType();
		if ( eType == CELLTYPE_STRING || eType == CELLTYPE_EDIT )
		{
			if (!pEngine)
			{
				//	#71154# ScTabEditEngine is needed
				//	because MapMode must be set for some old documents
				pEngine = new ScTabEditEngine( this );
				pEngine->SetControlWord( pEngine->GetControlWord() |
							( EE_CNTRL_ONLINESPELLING | EE_CNTRL_ALLOWBIGOBJS ) );
				pEngine->SetStatusEventHdl( LINK( &aStatus, ScSpellStatus, EventHdl ) );
				//	Delimiters hier wie in inputhdl.cxx !!!
				pEngine->SetWordDelimiters(
							ScEditUtil::ModifyDelimiters( pEngine->GetWordDelimiters() ) );
				pDefaults = new SfxItemSet( pEngine->GetEmptyItemSet() );

                com::sun::star::uno::Reference<com::sun::star::linguistic2::XSpellChecker1> xXSpellChecker1( LinguMgr::GetSpellChecker() );

				pEngine->SetSpeller( xXSpellChecker1 );
			}

			const ScPatternAttr* pPattern = GetPattern( nCol, nRow, nTab );
			pPattern->FillEditItemSet( pDefaults );
			pEngine->SetDefaults( pDefaults, sal_False );				//! noetig ?

			sal_uInt16 nCellLang = ((const SvxLanguageItem&)
									pPattern->GetItem(ATTR_FONT_LANGUAGE)).GetValue();
			if ( nCellLang == LANGUAGE_SYSTEM )
                nCellLang = Application::GetSettings().GetLanguage();   // never use SYSTEM for spelling
			pEngine->SetDefaultLanguage( nCellLang );

			if ( eType == CELLTYPE_STRING )
			{
				String aText;
				((ScStringCell*)pCell)->GetString(aText);
				pEngine->SetText( aText );
			}
			else
				pEngine->SetText( *((ScEditCell*)pCell)->GetData() );

			aStatus.bModified = sal_False;
			pEngine->CompleteOnlineSpelling();
			if ( aStatus.bModified )				// Fehler dazu oder weggekommen?
			{
				sal_Bool bNeedEdit = sal_True;						//	Test auf einfachen Text
				if ( !pEngine->HasOnlineSpellErrors() )
				{
					ScEditAttrTester aTester( pEngine );
					bNeedEdit = aTester.NeedsObject();
				}

				if ( bNeedEdit )
				{
					EditTextObject* pNewData = pEngine->CreateTextObject();
					if ( eType == CELLTYPE_EDIT )
						((ScEditCell*)pCell)->SetData( pNewData,
							pEngine->GetEditTextObjectPool() );
					else
						PutCell( nCol, nRow, nTab, new ScEditCell( pNewData,
							this, pEngine->GetEditTextObjectPool() ) );
					delete pNewData;
				}
				else					// einfacher String
					PutCell( nCol, nRow, nTab, new ScStringCell( pEngine->GetText() ) );

				//	Paint
				if (pShell)
				{
					//	#47751# Seitenvorschau ist davon nicht betroffen
					//	(sollte jedenfalls nicht)
					ScPaintHint aHint( ScRange( nCol, nRow, nTab ), PAINT_GRID );
					aHint.SetPrintFlag( sal_False );
					pShell->Broadcast( aHint );
				}

				bChanged = sal_True;
			}

			if ( ++nTestCount >= nMaxTest )				// checked enough text?
				break;
		}

		if ( ++nCellCount >= SPELL_MAXCELLS )			// seen enough cells?
			break;
	}

	if ( pCell )
	{
		++nCol;											// continue after last cell
		if ( nCol > rSpellRange.aEnd.Col() )
		{
			nCol = rSpellRange.aStart.Col();
			++nRow;
			if ( nRow > rSpellRange.aEnd.Row() )
				pCell = NULL;
		}
	}

	if (!pCell)			// end of range reached -> next sheet
	{
		++nTab;
		if ( nTab > rSpellRange.aEnd.Tab() || !pTab[nTab] )
			nTab = rSpellRange.aStart.Tab();
		nCol = rSpellRange.aStart.Col();
		nRow = rSpellRange.aStart.Row();

		nVisSpellState = VSPL_DONE;		//! only if this is for the visible range
	}
	rSpellPos.Set( nCol, nRow, nTab );

	delete pDefaults;
	delete pEngine;			// bevor aStatus out of scope geht

	return bChanged;
}


sal_Bool ScDocument::ContinueOnlineSpelling()
{
	if ( bIdleDisabled || !pDocOptions->IsAutoSpell() || (pShell && pShell->IsReadOnly()) )
		return sal_False;

    // #i48433# set bInsertingFromOtherDoc flag so there are no broadcasts when PutCell is called
    // (same behavior as in RemoveAutoSpellObj: just transfer the broadcaster)
    sal_Bool bOldInserting = IsInsertingFromOtherDoc();
    SetInsertingFromOtherDoc( sal_True );

	//!	use one EditEngine for both calls

	//	#41504# first check visible range
	sal_Bool bResult = OnlineSpellInRange( aVisSpellRange, aVisSpellPos, SPELL_MAXTEST_VIS );

	//	during first pass through visible range, always continue
	if ( nVisSpellState == VSPL_START )
		bResult = sal_True;

	if (bResult)
	{
		//	if errors found, continue there
		OnlineSpellInRange( aVisSpellRange, aVisSpellPos, SPELL_MAXTEST_ALL );
	}
	else
	{
		//	if nothing found there, continue with rest of document
		ScRange aTotalRange( 0,0,0, MAXCOL,MAXROW,MAXTAB );
		bResult = OnlineSpellInRange( aTotalRange, aOnlineSpellPos, SPELL_MAXTEST_ALL );
	}

    SetInsertingFromOtherDoc( bOldInserting );

	return bResult;
}


void ScDocument::SetOnlineSpellPos( const ScAddress& rPos )
{
	aOnlineSpellPos = rPos;

	//	skip visible area for aOnlineSpellPos
	if ( aVisSpellRange.In( aOnlineSpellPos ) )
		aOnlineSpellPos = aVisSpellRange.aEnd;
}

sal_Bool ScDocument::SetVisibleSpellRange( const ScRange& rNewRange )
{
	sal_Bool bChange = ( aVisSpellRange != rNewRange );
	if (bChange)
	{
		//	continue spelling through visible range when scrolling down
		sal_Bool bContDown = ( nVisSpellState == VSPL_START && rNewRange.In( aVisSpellPos ) &&
							rNewRange.aStart.Row() >  aVisSpellRange.aStart.Row() &&
							rNewRange.aStart.Col() == aVisSpellRange.aStart.Col() &&
							rNewRange.aEnd.Col()   == aVisSpellRange.aEnd.Col() );

		aVisSpellRange = rNewRange;

		if ( !bContDown )
		{
			aVisSpellPos = aVisSpellRange.aStart;
			nVisSpellState = VSPL_START;
		}

		//	skip visible area for aOnlineSpellPos
		if ( aVisSpellRange.In( aOnlineSpellPos ) )
			aOnlineSpellPos = aVisSpellRange.aEnd;
	}
	return bChange;
}

void ScDocument::RemoveAutoSpellObj()
{
	//	alle Spelling-Informationen entfernen

	for (SCTAB nTab=0; nTab<=MAXTAB && pTab[nTab]; nTab++)
		pTab[nTab]->RemoveAutoSpellObj();
}

void ScDocument::RepaintRange( const ScRange& rRange )
{
    if ( bIsVisible && pShell )
    {
        ScModelObj* pModel = ScModelObj::getImplementation( pShell->GetModel() );
        if ( pModel )
            pModel->RepaintRange( rRange );     // locked repaints are checked there
    }
}

//------------------------------------------------------------------------

sal_Bool ScDocument::IdleCheckLinks()			// sal_True = demnaechst wieder versuchen
{
	sal_Bool bAnyLeft = sal_False;

    if (GetLinkManager())
    {
        const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
        sal_uInt16 nCount = rLinks.Count();
        for (sal_uInt16 i=0; i<nCount; i++)
        {
            ::sfx2::SvBaseLink* pBase = *rLinks[i];
            if (pBase->ISA(ScDdeLink))
            {
                ScDdeLink* pDdeLink = (ScDdeLink*)pBase;
                if (pDdeLink->NeedsUpdate())
                {
                    pDdeLink->TryUpdate();
                    if (pDdeLink->NeedsUpdate())        // war nix?
                        bAnyLeft = sal_True;
                }
            }
        }
    }

	return bAnyLeft;
}

void ScDocument::SaveDdeLinks(SvStream& rStream) const
{
	//	bei 4.0-Export alle mit Modus != DEFAULT weglassen
	sal_Bool bExport40 = ( rStream.GetVersion() <= SOFFICE_FILEFORMAT_40 );

	const ::sfx2::SvBaseLinks& rLinks = GetLinkManager()->GetLinks();
	sal_uInt16 nCount = rLinks.Count();

	//	erstmal zaehlen...

	sal_uInt16 nDdeCount = 0;
	sal_uInt16 i;
	for (i=0; i<nCount; i++)
	{
		::sfx2::SvBaseLink* pBase = *rLinks[i];
		if (pBase->ISA(ScDdeLink))
			if ( !bExport40 || ((ScDdeLink*)pBase)->GetMode() == SC_DDE_DEFAULT )
				++nDdeCount;
	}

	//	Header

	ScMultipleWriteHeader aHdr( rStream );
	rStream << nDdeCount;

	//	Links speichern

	for (i=0; i<nCount; i++)
	{
		::sfx2::SvBaseLink* pBase = *rLinks[i];
		if (pBase->ISA(ScDdeLink))
		{
			ScDdeLink* pLink = (ScDdeLink*)pBase;
			if ( !bExport40 || pLink->GetMode() == SC_DDE_DEFAULT )
				pLink->Store( rStream, aHdr );
		}
	}
}

void ScDocument::LoadDdeLinks(SvStream& rStream)
{
	ScMultipleReadHeader aHdr( rStream );

    GetLinkManager();
	sal_uInt16 nCount;
	rStream >> nCount;
	for (sal_uInt16 i=0; i<nCount; i++)
	{
		ScDdeLink* pLink = new ScDdeLink( this, rStream, aHdr );
		pLinkManager->InsertDDELink( pLink,
							pLink->GetAppl(), pLink->GetTopic(), pLink->GetItem() );
	}
}

sal_Bool ScDocument::HasDdeLinks() const
{
	if (GetLinkManager())			// Clipboard z.B. hat keinen LinkManager
	{
		const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
		sal_uInt16 nCount = rLinks.Count();
		for (sal_uInt16 i=0; i<nCount; i++)
			if ((*rLinks[i])->ISA(ScDdeLink))
				return sal_True;
	}

	return sal_False;
}

void ScDocument::SetInLinkUpdate(sal_Bool bSet)
{
	//	called from TableLink and AreaLink

	DBG_ASSERT( bInLinkUpdate != bSet, "SetInLinkUpdate twice" );
	bInLinkUpdate = bSet;
}

sal_Bool ScDocument::IsInLinkUpdate() const
{
    return bInLinkUpdate || IsInDdeLinkUpdate();
}

void ScDocument::UpdateExternalRefLinks()
{
    if (!GetLinkManager())
        return;

    const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
    sal_uInt16 nCount = rLinks.Count();

    bool bAny = false;
    for (sal_uInt16 i = 0; i < nCount; ++i)
    {
        ::sfx2::SvBaseLink* pBase = *rLinks[i];
        ScExternalRefLink* pRefLink = dynamic_cast<ScExternalRefLink*>(pBase);
        if (pRefLink)
        {
            pRefLink->Update();
            bAny = true;
        }
    }
    if (bAny)
    {
        TrackFormulas();
        pShell->Broadcast( SfxSimpleHint(FID_DATACHANGED) );
        ResetChanged( ScRange(0, 0, 0, MAXCOL, MAXROW, MAXTAB) );

        // #i101960# set document modified, as in TrackTimeHdl for DDE links
        if (!pShell->IsModified())
        {
            pShell->SetModified( sal_True );
            SfxBindings* pBindings = GetViewBindings();
            if (pBindings)
            {
                pBindings->Invalidate( SID_SAVEDOC );
                pBindings->Invalidate( SID_DOC_MODIFIED );
            }
        }
    }
}

void ScDocument::UpdateDdeLinks()
{
    if (GetLinkManager())
    {
        const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
        sal_uInt16 nCount = rLinks.Count();
        sal_uInt16 i;

        //  #49226# falls das Updaten laenger dauert, erstmal alle Werte
        //  zuruecksetzen, damit nichts altes (falsches) stehen bleibt
        sal_Bool bAny = sal_False;
        for (i=0; i<nCount; i++)
        {
            ::sfx2::SvBaseLink* pBase = *rLinks[i];
            if (pBase->ISA(ScDdeLink))
            {
                ((ScDdeLink*)pBase)->ResetValue();
                bAny = sal_True;
            }
        }
        if (bAny)
        {
            //  Formeln berechnen und painten wie im TrackTimeHdl
            TrackFormulas();
            pShell->Broadcast( SfxSimpleHint( FID_DATACHANGED ) );
            ResetChanged( ScRange(0,0,0,MAXCOL,MAXROW,MAXTAB) );

            //  wenn FID_DATACHANGED irgendwann mal asynchron werden sollte
            //  (z.B. mit Invalidate am Window), muss hier ein Update erzwungen werden.
        }

        //  nun wirklich updaten...
        for (i=0; i<nCount; i++)
        {
            ::sfx2::SvBaseLink* pBase = *rLinks[i];
            if (pBase->ISA(ScDdeLink))
                ((ScDdeLink*)pBase)->TryUpdate();       // bei DDE-Links TryUpdate statt Update
        }
    }
}

sal_Bool ScDocument::UpdateDdeLink( const String& rAppl, const String& rTopic, const String& rItem )
{
	//	fuer refresh() per StarOne Api
	//	ResetValue() fuer einzelnen Link nicht noetig
	//!	wenn's mal alles asynchron wird, aber auch hier

	sal_Bool bFound = sal_False;
    if (GetLinkManager())
    {
        const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
        sal_uInt16 nCount = rLinks.Count();
        for (sal_uInt16 i=0; i<nCount; i++)
        {
            ::sfx2::SvBaseLink* pBase = *rLinks[i];
            if (pBase->ISA(ScDdeLink))
            {
                ScDdeLink* pDdeLink = (ScDdeLink*)pBase;
                if ( pDdeLink->GetAppl() == rAppl &&
                     pDdeLink->GetTopic() == rTopic &&
                     pDdeLink->GetItem() == rItem )
                {
                    pDdeLink->TryUpdate();
                    bFound = sal_True;          // koennen theoretisch mehrere sein (Mode), darum weitersuchen
                }
            }
        }
    }
	return bFound;
}

void ScDocument::DisconnectDdeLinks()
{
	if (GetLinkManager())
	{
		const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
		sal_uInt16 nCount = rLinks.Count();
		for (sal_uInt16 i=0; i<nCount; i++)
		{
			::sfx2::SvBaseLink* pBase = *rLinks[i];
			if (pBase->ISA(ScDdeLink))
				pBase->Disconnect();			// bleibt im LinkManager eingetragen
		}
	}
}

void ScDocument::CopyDdeLinks( ScDocument* pDestDoc ) const
{
	if (bIsClip)		// aus Stream erzeugen
	{
		if (pClipData)
		{
			pClipData->Seek(0);
			pDestDoc->LoadDdeLinks(*pClipData);
		}
	}
    else if (GetLinkManager())              // Links direkt kopieren
	{
		const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
		sal_uInt16 nCount = rLinks.Count();
		for (sal_uInt16 i=0; i<nCount; i++)
		{
			::sfx2::SvBaseLink* pBase = *rLinks[i];
			if (pBase->ISA(ScDdeLink))
			{
				ScDdeLink* pNew = new ScDdeLink( pDestDoc, *(ScDdeLink*)pBase );

				pDestDoc->pLinkManager->InsertDDELink( pNew,
								pNew->GetAppl(), pNew->GetTopic(), pNew->GetItem() );
			}
		}
	}
}

sal_uInt16 ScDocument::GetDdeLinkCount() const
{
	sal_uInt16 nDdeCount = 0;
	if (GetLinkManager())
	{
		const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
		sal_uInt16 nCount = rLinks.Count();
		for (sal_uInt16 i=0; i<nCount; i++)
			if ((*rLinks[i])->ISA(ScDdeLink))
				++nDdeCount;
	}
	return nDdeCount;
}

// ----------------------------------------------------------------------------

namespace {

/** Tries to find the specified DDE link.
    @param pnDdePos  (out-param) if not 0, the index of the DDE link is returned here
                     (does not include other links from link manager).
    @return  The DDE link, if it exists, otherwise 0. */
ScDdeLink* lclGetDdeLink(
        const sfx2::LinkManager* pLinkManager,
        const String& rAppl, const String& rTopic, const String& rItem, sal_uInt8 nMode,
        sal_uInt16* pnDdePos = NULL )
{
    if( pLinkManager )
    {
        const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
        sal_uInt16 nCount = rLinks.Count();
        if( pnDdePos ) *pnDdePos = 0;
        for( sal_uInt16 nIndex = 0; nIndex < nCount; ++nIndex )
        {
            ::sfx2::SvBaseLink* pLink = *rLinks[ nIndex ];
            if( ScDdeLink* pDdeLink = PTR_CAST( ScDdeLink, pLink ) )
            {
                if( (pDdeLink->GetAppl() == rAppl) &&
                    (pDdeLink->GetTopic() == rTopic) &&
                    (pDdeLink->GetItem() == rItem) &&
                    ((nMode == SC_DDE_IGNOREMODE) || (nMode == pDdeLink->GetMode())) )
                    return pDdeLink;
                if( pnDdePos ) ++*pnDdePos;
            }
        }
    }
    return NULL;
}

/** Returns a pointer to the specified DDE link.
    @param nDdePos  Index of the DDE link (does not include other links from link manager).
    @return  The DDE link, if it exists, otherwise 0. */
ScDdeLink* lclGetDdeLink( const sfx2::LinkManager* pLinkManager, sal_uInt16 nDdePos )
{
    if( pLinkManager )
    {
        const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
        sal_uInt16 nCount = rLinks.Count();
        sal_uInt16 nDdeIndex = 0;       // counts only the DDE links
        for( sal_uInt16 nIndex = 0; nIndex < nCount; ++nIndex )
        {
            ::sfx2::SvBaseLink* pLink = *rLinks[ nIndex ];
            if( ScDdeLink* pDdeLink = PTR_CAST( ScDdeLink, pLink ) )
            {
                if( nDdeIndex == nDdePos )
                    return pDdeLink;
                ++nDdeIndex;
            }
        }
    }
    return NULL;
}

} // namespace

// ----------------------------------------------------------------------------

bool ScDocument::FindDdeLink( const String& rAppl, const String& rTopic, const String& rItem, sal_uInt8 nMode, sal_uInt16& rnDdePos )
{
    return lclGetDdeLink( GetLinkManager(), rAppl, rTopic, rItem, nMode, &rnDdePos ) != NULL;
}

bool ScDocument::GetDdeLinkData( sal_uInt16 nDdePos, String& rAppl, String& rTopic, String& rItem ) const
{
    if( const ScDdeLink* pDdeLink = lclGetDdeLink( GetLinkManager(), nDdePos ) )
    {
        rAppl  = pDdeLink->GetAppl();
        rTopic = pDdeLink->GetTopic();
        rItem  = pDdeLink->GetItem();
        return true;
	}
    return false;
}

bool ScDocument::GetDdeLinkMode( sal_uInt16 nDdePos, sal_uInt8& rnMode ) const
{
    if( const ScDdeLink* pDdeLink = lclGetDdeLink( GetLinkManager(), nDdePos ) )
    {
        rnMode = pDdeLink->GetMode();
        return true;
    }
    return false;
}

const ScMatrix* ScDocument::GetDdeLinkResultMatrix( sal_uInt16 nDdePos ) const
{
    const ScDdeLink* pDdeLink = lclGetDdeLink( GetLinkManager(), nDdePos );
    return pDdeLink ? pDdeLink->GetResult() : NULL;
}

bool ScDocument::CreateDdeLink( const String& rAppl, const String& rTopic, const String& rItem, sal_uInt8 nMode, ScMatrix* pResults )
{
    /*  Create a DDE link without updating it (i.e. for Excel import), to prevent
        unwanted connections. First try to find existing link. Set result array
        on existing and new links. */
    //! store DDE links additionally at document (for efficiency)?
    DBG_ASSERT( nMode != SC_DDE_IGNOREMODE, "ScDocument::CreateDdeLink - SC_DDE_IGNOREMODE not allowed here" );
    if( GetLinkManager() && (nMode != SC_DDE_IGNOREMODE) )
    {
        ScDdeLink* pDdeLink = lclGetDdeLink( pLinkManager, rAppl, rTopic, rItem, nMode );
        if( !pDdeLink )
        {
            // create a new DDE link, but without TryUpdate
            pDdeLink = new ScDdeLink( this, rAppl, rTopic, rItem, nMode );
            pLinkManager->InsertDDELink( pDdeLink, rAppl, rTopic, rItem );
        }

        // insert link results
        if( pResults )
            pDdeLink->SetResult( pResults );

        return true;
    }
    return false;
}

bool ScDocument::SetDdeLinkResultMatrix( sal_uInt16 nDdePos, ScMatrix* pResults )
{
    if( ScDdeLink* pDdeLink = lclGetDdeLink( GetLinkManager(), nDdePos ) )
    {
        pDdeLink->SetResult( pResults );
        return true;
    }
    return false;
}

//------------------------------------------------------------------------

sal_Bool ScDocument::HasAreaLinks() const
{
	if (GetLinkManager())			// Clipboard z.B. hat keinen LinkManager
	{
		const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
		sal_uInt16 nCount = rLinks.Count();
		for (sal_uInt16 i=0; i<nCount; i++)
			if ((*rLinks[i])->ISA(ScAreaLink))
				return sal_True;
	}

	return sal_False;
}

void ScDocument::UpdateAreaLinks()
{
    if (GetLinkManager())
    {
        const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
        sal_uInt16 nCount = rLinks.Count();
        for (sal_uInt16 i=0; i<nCount; i++)
        {
            ::sfx2::SvBaseLink* pBase = *rLinks[i];
            if (pBase->ISA(ScAreaLink))
                pBase->Update();
        }
    }
}

void ScDocument::DeleteAreaLinksOnTab( SCTAB nTab )
{
    if (GetLinkManager())
    {
        const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
        sal_uInt16 nPos = 0;
        while ( nPos < rLinks.Count() )
        {
            const ::sfx2::SvBaseLink* pBase = *rLinks[nPos];
            if ( pBase->ISA(ScAreaLink) &&
                 static_cast<const ScAreaLink*>(pBase)->GetDestArea().aStart.Tab() == nTab )
                pLinkManager->Remove( nPos );
            else
                ++nPos;
        }
    }
}

void ScDocument::UpdateRefAreaLinks( UpdateRefMode eUpdateRefMode,
							 const ScRange& rRange, SCsCOL nDx, SCsROW nDy, SCsTAB nDz )
{
    if (GetLinkManager())
    {
        bool bAnyUpdate = false;

        const ::sfx2::SvBaseLinks& rLinks = pLinkManager->GetLinks();
        sal_uInt16 nCount = rLinks.Count();
        for (sal_uInt16 i=0; i<nCount; i++)
        {
            ::sfx2::SvBaseLink* pBase = *rLinks[i];
            if (pBase->ISA(ScAreaLink))
            {
                ScAreaLink* pLink = (ScAreaLink*) pBase;
                ScRange aOutRange = pLink->GetDestArea();

                SCCOL nCol1 = aOutRange.aStart.Col();
                SCROW nRow1 = aOutRange.aStart.Row();
                SCTAB nTab1 = aOutRange.aStart.Tab();
                SCCOL nCol2 = aOutRange.aEnd.Col();
                SCROW nRow2 = aOutRange.aEnd.Row();
                SCTAB nTab2 = aOutRange.aEnd.Tab();

                ScRefUpdateRes eRes =
                    ScRefUpdate::Update( this, eUpdateRefMode,
                        rRange.aStart.Col(), rRange.aStart.Row(), rRange.aStart.Tab(),
                        rRange.aEnd.Col(), rRange.aEnd.Row(), rRange.aEnd.Tab(), nDx, nDy, nDz,
                        nCol1, nRow1, nTab1, nCol2, nRow2, nTab2 );
                if ( eRes != UR_NOTHING )
                {
                    pLink->SetDestArea( ScRange( nCol1, nRow1, nTab1, nCol2, nRow2, nTab2 ) );
                    bAnyUpdate = true;
                }
            }
        }

        if ( bAnyUpdate )
        {
            // #i52120# Look for duplicates (after updating all positions).
            // If several links start at the same cell, the one with the lower index is removed
            // (file format specifies only one link definition for a cell).

            sal_uInt16 nFirstIndex = 0;
            while ( nFirstIndex < nCount )
            {
                bool bFound = false;
                ::sfx2::SvBaseLink* pFirst = *rLinks[nFirstIndex];
                if ( pFirst->ISA(ScAreaLink) )
                {
                    ScAddress aFirstPos = static_cast<ScAreaLink*>(pFirst)->GetDestArea().aStart;
                    for ( sal_uInt16 nSecondIndex = nFirstIndex + 1; nSecondIndex < nCount && !bFound; ++nSecondIndex )
                    {
                        ::sfx2::SvBaseLink* pSecond = *rLinks[nSecondIndex];
                        if ( pSecond->ISA(ScAreaLink) &&
                             static_cast<ScAreaLink*>(pSecond)->GetDestArea().aStart == aFirstPos )
                        {
                            // remove the first link, exit the inner loop, don't increment nFirstIndex
                            pLinkManager->Remove( pFirst );
                            nCount = rLinks.Count();
                            bFound = true;
                        }
                    }
                }
                if (!bFound)
                    ++nFirstIndex;
            }
        }
    }
}

//------------------------------------------------------------------------

// TimerDelays etc.
void ScDocument::KeyInput( const KeyEvent& )
{
	if ( pChartListenerCollection->GetCount() )
		pChartListenerCollection->StartTimer();
    if( apTemporaryChartLock.get() )
        apTemporaryChartLock->StartOrContinueLocking();
}

//	----------------------------------------------------------------------------

sal_Bool ScDocument::CheckMacroWarn()
{
	//	The check for macro configuration, macro warning and disabling is now handled
	//	in SfxObjectShell::AdjustMacroMode, called by SfxObjectShell::CallBasic.

	return sal_True;
}

//------------------------------------------------------------------------

SfxBindings* ScDocument::GetViewBindings()
{
	//	used to invalidate slots after changes to this document

	if ( !pShell )
		return NULL;		// no ObjShell -> no view

	//	first check current view
	SfxViewFrame* pViewFrame = SfxViewFrame::Current();
	if ( pViewFrame && pViewFrame->GetObjectShell() != pShell )		// wrong document?
		pViewFrame = NULL;

	//	otherwise use first view for this doc
	if ( !pViewFrame )
		pViewFrame = SfxViewFrame::GetFirst( pShell );

	if (pViewFrame)
		return &pViewFrame->GetBindings();
	else
		return NULL;
}

//------------------------------------------------------------------------

void ScDocument::TransliterateText( const ScMarkData& rMultiMark, sal_Int32 nType )
{
	DBG_ASSERT( rMultiMark.IsMultiMarked(), "TransliterateText: no selection" );

	utl::TransliterationWrapper aTranslitarationWrapper( xServiceManager, nType );
	sal_Bool bConsiderLanguage = aTranslitarationWrapper.needLanguageForTheMode();
	sal_uInt16 nLanguage = LANGUAGE_SYSTEM;

	ScEditEngineDefaulter* pEngine = NULL;		// not using pEditEngine member because of defaults

	SCTAB nCount = GetTableCount();
	for (SCTAB nTab = 0; nTab < nCount; nTab++)
		if ( pTab[nTab] && rMultiMark.GetTableSelect(nTab) )
		{
			SCCOL nCol = 0;
			SCROW nRow = 0;

			sal_Bool bFound = rMultiMark.IsCellMarked( nCol, nRow );
			if (!bFound)
				bFound = GetNextMarkedCell( nCol, nRow, nTab, rMultiMark );

			while (bFound)
			{
				const ScBaseCell* pCell = GetCell( ScAddress( nCol, nRow, nTab ) );
				CellType eType = pCell ? pCell->GetCellType() : CELLTYPE_NONE;

                // #i115128# TITLE_CASE/SENTENCE_CASE need the extra handling in EditEngine (loop over words/sentences).
                // Still use TransliterationWrapper directly for text cells with other transliteration types,
                // for performance reasons.

                if ( eType == CELLTYPE_EDIT ||
                     ( eType == CELLTYPE_STRING && ( nType == com::sun::star::i18n::TransliterationModulesExtra::SENTENCE_CASE ||
                                                     nType == com::sun::star::i18n::TransliterationModulesExtra::TITLE_CASE ) ) )
                {
                    if (!pEngine)
                        pEngine = new ScFieldEditEngine( GetEnginePool(), GetEditPool() );

                    // defaults from cell attributes must be set so right language is used
                    const ScPatternAttr* pPattern = GetPattern( nCol, nRow, nTab );
                    SfxItemSet* pDefaults = new SfxItemSet( pEngine->GetEmptyItemSet() );
                    pPattern->FillEditItemSet( pDefaults );
                    pEngine->SetDefaults( pDefaults, sal_True );

                    if ( eType == CELLTYPE_STRING )
                        pEngine->SetText( static_cast<const ScStringCell*>(pCell)->GetString() );
                    else
                    {
                        const EditTextObject* pData = static_cast<const ScEditCell*>(pCell)->GetData();
                        pEngine->SetText( *pData );
                    }
                    pEngine->ClearModifyFlag();

                    sal_uInt32 nLastPar = pEngine->GetParagraphCount();
                    if (nLastPar)
                        --nLastPar;
                    xub_StrLen nTxtLen = pEngine->GetTextLen(nLastPar);
                    ESelection aSelAll( 0, 0, nLastPar, nTxtLen );

                    pEngine->TransliterateText( aSelAll, nType );

                    if ( pEngine->IsModified() )
                    {
                        ScEditAttrTester aTester( pEngine );
                        if ( aTester.NeedsObject() )
                        {
                            // remove defaults (paragraph attributes) before creating text object
                            SfxItemSet* pEmpty = new SfxItemSet( pEngine->GetEmptyItemSet() );
                            pEngine->SetDefaults( pEmpty, sal_True );

                            EditTextObject* pNewData = pEngine->CreateTextObject();
                            PutCell( nCol, nRow, nTab,
                                new ScEditCell( pNewData, this, pEngine->GetEditTextObjectPool() ) );
                            delete pNewData;
                        }
                        else
                        {
                            String aNewStr = pEngine->GetText();
                            PutCell( nCol, nRow, nTab, new ScStringCell( aNewStr ) );
                        }
                    }
                }
                else if ( eType == CELLTYPE_STRING )
				{
					String aOldStr;
					((const ScStringCell*)pCell)->GetString(aOldStr);
					xub_StrLen nOldLen = aOldStr.Len();

					if ( bConsiderLanguage )
					{
						sal_uInt8 nScript = GetStringScriptType( aOldStr );		//! cell script type?
						sal_uInt16 nWhich = ( nScript == SCRIPTTYPE_ASIAN ) ? ATTR_CJK_FONT_LANGUAGE :
										( ( nScript == SCRIPTTYPE_COMPLEX ) ? ATTR_CTL_FONT_LANGUAGE :
																				ATTR_FONT_LANGUAGE );
						nLanguage = ((const SvxLanguageItem*)GetAttr( nCol, nRow, nTab, nWhich ))->GetValue();
					}

					com::sun::star::uno::Sequence<sal_Int32> aOffsets;
					String aNewStr = aTranslitarationWrapper.transliterate( aOldStr, nLanguage, 0, nOldLen, &aOffsets );

					if ( aNewStr != aOldStr )
						PutCell( nCol, nRow, nTab, new ScStringCell( aNewStr ) );
				}

				bFound = GetNextMarkedCell( nCol, nRow, nTab, rMultiMark );
			}
		}

	delete pEngine;
}

