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




#ifdef PCH
#endif

#ifndef _SVDPAGE_HXX //autogen
#include <bf_svx/svdpage.hxx>
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif


#include "scitems.hxx"
#include <bf_svtools/ctrltool.hxx>
#include <bf_svx/flstitem.hxx>
#include <bf_svx/drawitem.hxx>
#include <bf_sfx2/printer.hxx>
#include <bf_svx/svditer.hxx>
#include <bf_svx/svdoole2.hxx>
#include <bf_offmgr/app.hxx>
#include <bf_svx/asiancfg.hxx>
#include <bf_svx/forbiddencharacterstable.hxx>
#include <rtl/logfile.hxx>



// INCLUDE ---------------------------------------------------------------
/*
*/
#include "drwlayer.hxx"
#include "stlpool.hxx"
#include "docsh.hxx"
#include "docfunc.hxx"
#include "bf_sc.hrc"
namespace binfilter {

using namespace ::com::sun::star;

//------------------------------------------------------------------

/*N*/ BOOL __EXPORT ScDocShell::InitNew( SvStorage * pStor )
/*N*/ {
/*N*/ 	RTL_LOGFILE_CONTEXT_AUTHOR ( aLog, "sc", "nn93723", "ScDocShell::InitNew" );
/*N*/ 
/*N*/ 	BOOL bRet = SfxInPlaceObject::InitNew( pStor );
/*N*/ 
/*N*/ 	aDocument.MakeTable(0);
/*N*/ 	//	zusaetzliche Tabellen werden von der ersten View angelegt,
/*N*/ 	//	wenn bIsEmpty dann noch TRUE ist
/*N*/ 
/*N*/ 	if( bRet )
/*N*/ 	{
/*N*/ 		Size aSize( (long) ( STD_COL_WIDTH			 * HMM_PER_TWIPS * OLE_STD_CELLS_X ),
/*N*/ 					(long) ( ScGlobal::nStdRowHeight * HMM_PER_TWIPS * OLE_STD_CELLS_Y ) );
/*N*/ 		// hier muss auch der Start angepasst werden
/*N*/ 		SetVisAreaOrSize( Rectangle( Point(), aSize ), TRUE );
/*N*/ 	}
/*N*/ 
/*N*/ 	// InitOptions sets the document languages, must be called before CreateStandardStyles
/*N*/ 	InitOptions();
/*N*/ 
/*N*/ 	aDocument.GetStyleSheetPool()->CreateStandardStyles();
/*N*/ 	aDocument.UpdStlShtPtrsFrmNms();
/*N*/ 
/*N*/ 	//	SetDocumentModified ist in Load/InitNew nicht mehr erlaubt!
/*N*/ 
/*N*/ 	InitItems();
/*N*/ 	CalcOutputFactor();
/*N*/ 
/*N*/ 	return bRet;
/*N*/ }

//------------------------------------------------------------------

/*N*/ void ScDocShell::InitItems()
/*N*/ {
/*N*/ 	// AllItemSet fuer Controller mit benoetigten Items fuellen:
/*N*/ 
/*N*/ 	if ( pFontList )
/*N*/ 		delete pFontList;
/*N*/ 
/*N*/ 	//	Druck-Optionen werden beim Drucken und evtl. in GetPrinter gesetzt
/*N*/ 
/*N*/ 	pFontList = new FontList( GetPrinter(), Application::GetDefaultDevice() );
/*N*/ 	PutItem( SvxFontListItem( pFontList, SID_ATTR_CHAR_FONTLIST ) );
/*N*/ 
/*N*/ 	ScDrawLayer* pDrawLayer = aDocument.GetDrawLayer();
/*N*/ 	if (pDrawLayer)
/*N*/ 	{
/*N*/ 		PutItem( SvxColorTableItem	( pDrawLayer->GetColorTable() ) );
/*N*/ 		PutItem( SvxGradientListItem( pDrawLayer->GetGradientList() ) );
/*N*/ 		PutItem( SvxHatchListItem	( pDrawLayer->GetHatchList() ) );
/*N*/ 		PutItem( SvxBitmapListItem	( pDrawLayer->GetBitmapList() ) );
/*N*/ 		PutItem( SvxDashListItem	( pDrawLayer->GetDashList() ) );
/*N*/ 		PutItem( SvxLineEndListItem ( pDrawLayer->GetLineEndList() ) );
/*N*/ 
/*N*/ 			//	andere Anpassungen nach dem Anlegen des DrawLayers
/*N*/ 
/*N*/ 		//if (SfxObjectShell::HasSbxObject())
/*N*/ 		pDrawLayer->UpdateBasic();			// DocShell-Basic in DrawPages setzen
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		//	always use global color table instead of local copy
/*N*/ 
/*N*/ 		PutItem( SvxColorTableItem( OFF_APP()->GetStdColorTable() ) );
/*N*/ 	}
/*N*/ 
/*N*/ 	if ( !aDocument.GetForbiddenCharacters().isValid() ||
/*N*/ 			!aDocument.IsValidAsianCompression() || !aDocument.IsValidAsianKerning() )
/*N*/ 	{
/*N*/ 		//	get settings from SvxAsianConfig
/*N*/ 		SvxAsianConfig aAsian( sal_False );
/*N*/ 
/*N*/ 		if ( !aDocument.GetForbiddenCharacters().isValid() )
/*N*/ 		{
/*N*/ 			// set forbidden characters if necessary
/*N*/ 			uno::Sequence<lang::Locale> aLocales = aAsian.GetStartEndCharLocales();
/*N*/ 			if (aLocales.getLength())
/*N*/ 			{
/*?*/ 				vos::ORef<SvxForbiddenCharactersTable> xForbiddenTable =
/*?*/ 						new SvxForbiddenCharactersTable( aDocument.GetServiceManager() );
/*?*/ 
/*?*/ 				const lang::Locale* pLocales = aLocales.getConstArray();
/*?*/ 				for (sal_Int32 i = 0; i < aLocales.getLength(); i++)
/*?*/ 				{
/*?*/ 					DBG_BF_ASSERT(0, "STRIP"); //STRIP001 i18n::ForbiddenCharacters aForbidden;
/*?*/ 				}
/*?*/ 
/*?*/ 				aDocument.SetForbiddenCharacters( xForbiddenTable );
/*N*/ 			}
/*N*/ 		}
/*N*/ 
/*N*/ 		if ( !aDocument.IsValidAsianCompression() )
/*N*/ 		{
/*N*/ 			// set compression mode from configuration if not already set (e.g. XML import)
/*N*/ 			aDocument.SetAsianCompression( aAsian.GetCharDistanceCompression() );
/*N*/ 		}
/*N*/ 
/*N*/ 		if ( !aDocument.IsValidAsianKerning() )
/*N*/ 		{
/*N*/ 			// set asian punctuation kerning from configuration if not already set (e.g. XML import)
/*N*/ 			aDocument.SetAsianKerning( !aAsian.IsKerningWesternTextOnly() );	// reversed
/*N*/ 		}
/*N*/ 	}
/*N*/ }

//------------------------------------------------------------------

/*N*/ void ScDocShell::ResetDrawObjectShell()
/*N*/ {
/*N*/ 	ScDrawLayer* pDrawLayer = aDocument.GetDrawLayer();
/*N*/ 	if (pDrawLayer)
/*N*/ 		pDrawLayer->SetObjectShell( NULL );
/*N*/ }

//------------------------------------------------------------------




//------------------------------------------------------------------


/*N*/ ScDrawLayer* ScDocShell::MakeDrawLayer()
/*N*/ {
/*N*/ 	ScDrawLayer* pDrawLayer = aDocument.GetDrawLayer();
/*N*/ 	if (!pDrawLayer)
/*N*/ 	{
/*N*/ 		RTL_LOGFILE_CONTEXT_AUTHOR ( aLog, "sc", "nn93723", "ScDocShell::MakeDrawLayer" );
/*N*/ 
/*N*/ 		aDocument.InitDrawLayer(this);
/*N*/ 		pDrawLayer = aDocument.GetDrawLayer();
/*N*/ 		InitItems();											// incl. Undo und Basic
/*N*/ 		Broadcast( SfxSimpleHint( SC_HINT_DRWLAYER_NEW ) );
/*N*/ 		if (nDocumentLock)
/*N*/ 			pDrawLayer->setLock(TRUE);
/*N*/ 	}
/*N*/ 	return pDrawLayer;
/*N*/ }

//------------------------------------------------------------------


/*N*/ void ScDocShell::RemoveUnknownObjects()
/*N*/ {
/*N*/ 	//	OLE-Objekte loeschen, wenn kein Drawing-Objekt dazu existiert
/*N*/ 	//	Loeschen wie in SvPersist::CleanUp
/*N*/ 
/*N*/ 	ScDrawLayer* pDrawLayer = aDocument.GetDrawLayer();
/*N*/ 	const SvInfoObjectMemberList* pChildList = GetObjectList();
/*N*/ 
/*N*/ 	if( pChildList && pChildList->Count() )
/*N*/ 	{
/*N*/ 		for( ULONG i=0; i<pChildList->Count(); )
/*N*/ 		{
/*N*/ 			SvInfoObjectRef pEle = pChildList->GetObject(i);
/*N*/ 			String aObjName = pEle->GetObjName();
/*N*/ 			BOOL bFound = FALSE;
/*N*/ 			if ( pDrawLayer )
/*N*/ 			{
/*N*/ 				USHORT nTabCount = pDrawLayer->GetPageCount();
/*N*/ 				for (USHORT nTab=0; nTab<nTabCount && !bFound; nTab++)
/*N*/ 				{
/*N*/ 					SdrPage* pPage = pDrawLayer->GetPage(nTab);
/*N*/ 					DBG_ASSERT(pPage,"Page ?");
/*N*/ 					if (pPage)
/*N*/ 					{
/*N*/ 						SdrObjListIter aIter( *pPage, IM_DEEPNOGROUPS );
/*N*/ 						SdrObject* pObject = aIter.Next();
/*N*/ 						while (pObject && !bFound)
/*N*/ 						{
/*N*/ 							// name from InfoObject is PersistName
/*N*/ 							if ( pObject->ISA(SdrOle2Obj) &&
/*N*/ 									static_cast<SdrOle2Obj*>(pObject)->GetPersistName() == aObjName )
/*N*/ 								bFound = TRUE;
/*N*/ 							pObject = aIter.Next();
/*N*/ 						}
/*N*/ 					}
/*N*/ 				}
/*N*/ 			}
/*N*/ 
/*N*/ 			if (!bFound)
/*N*/ 			{
/*?*/ 				DBG_ASSERT(pEle->GetRefCount()==2, "Loeschen von referenziertem Storage");
/*?*/ 				String aStorName(pEle->GetStorageName());
/*?*/ 				SvPersist::Remove(pEle);
/*?*/ 				GetStorage()->Remove(aStorName);
/*N*/ 			}
/*N*/ 			else
/*N*/ 				i++;
/*N*/ 		}
/*N*/ 	}
/*N*/ }


}
