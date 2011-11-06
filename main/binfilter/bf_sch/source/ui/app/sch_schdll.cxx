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



#ifdef _MSC_VER
#pragma hdrstop
#endif

#include <bf_svx/svxids.hrc>

#include "docshell.hxx"
#include "schmod.hxx"
#include "app.hrc"
#include "schresid.hxx"
#include "memchrt.hxx"
#include "res_bmp.hrc"	// contains define for RID_MENU, RID_ACCEL

#include "globopt.hxx"
namespace binfilter {

/*************************************************************************
|*
|* Initialisierung aus SfxApplicaton::Init()
|*
\************************************************************************/

/*N*/ void __EXPORT SchDLL::Init()
/*N*/ {
/*N*/ 	// called directly after loading the DLL
/*N*/ 	// do whatever you want, you may use Sd-DLL too
/*N*/ 
/*N*/ 	// the SchModule must be created (may loaded by doc-fac or direct)
/*N*/ 	SchModuleDummy** ppShlPtr = (SchModuleDummy**) GetAppData(BF_SHL_SCH);
/*N*/ 	if ( (*ppShlPtr)->IsLoaded() ) return;
/*N*/ 
/*N*/ 	SvFactory* pFact = PTR_CAST(SvFactory,(*ppShlPtr)->pSchChartDocShellFactory);
/*N*/ 
/*N*/ 	delete (*ppShlPtr);
/*N*/ 	(*ppShlPtr) = new SchModule(pFact);
/*N*/ 
/*N*/ 	SfxModule* pMod = SCH_MOD();
/*N*/ 
/*N*/   pMod->PutItem( SfxUInt16Item( SID_ATTR_METRIC, ::binfilter::sch::util::GetMeasureUnit() ));
/*N*/ }

/*************************************************************************
|*
|* Deinitialisierung
|*
\************************************************************************/

/*N*/ void __EXPORT SchDLL::Exit()
/*N*/ {
/*N*/ 	// the SchModule must be destroyed
/*N*/ 	SchModuleDummy** ppShlPtr = (SchModuleDummy**) GetAppData(BF_SHL_SCH);
/*N*/ 	delete (*ppShlPtr);
/*N*/ 	(*ppShlPtr) = NULL;
/*N*/ }


/*N*/ extern_c void __LOADONCALLAPI SchSetTransparent( SvInPlaceObjectRef aIPObj, BOOL bTransp )
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/

/*N*/ extern_c void __LOADONCALLAPI SchUpdate( SvInPlaceObjectRef aIPObj,
/*N*/ 										 SchMemChart* pData, OutputDevice* pOut )
/*N*/ {
/*N*/ 	SchChartDocShellRef aSchChartDocShellRef = &aIPObj;
/*N*/ 
/*N*/ 	if( aSchChartDocShellRef.Is() )
/*N*/ 	{
/*N*/ #ifdef DBG_UTIL
/*N*/ 		if( !pData )
/*N*/ 			DBG_TRACE( "SchUpdate was called without MemChart" );
/*N*/ #endif
/*N*/ 		ChartModel& rDoc = aSchChartDocShellRef->GetDoc();
/*N*/ 
/*N*/ 		if( pData )
/*N*/ 		{
/*N*/ 			rDoc.SetChartDataBuffered( *pData, FALSE );		// don't update titles. This can't be done via SchUpdate
/*N*/ 			rDoc.CheckForNewAxisNumFormat();
/*N*/ 			rDoc.SetChanged();								// #72576#
/*N*/ 
/*N*/ 			::com::sun::star::chart::ChartDataChangeEvent aEvent;
/*N*/ 			aEvent.Type = ::com::sun::star::chart::ChartDataChangeType_ALL;
/*N*/ 			aEvent.StartColumn = 0;
/*N*/ 			aEvent.EndColumn = 0;
/*N*/ 			aEvent.StartRow = 0;
/*N*/ 			aEvent.EndRow = 0;
/*N*/ 
/*N*/ 			aSchChartDocShellRef->DataModified( aEvent );
/*N*/ 		}
/*N*/ 		else
/*N*/ 		{
/*N*/ 			rDoc.BuildChart( FALSE );
/*N*/ 		}
/*N*/ 	}
/*N*/ 
/*N*/ 	aIPObj->SendViewChanged();
/*N*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/

/*N*/ extern_c void __LOADONCALLAPI SchUpdateAttr( SvInPlaceObjectRef aIPObj,
/*N*/ 											 SchMemChart* pData,
/*N*/ 											 const SfxItemSet& rAttr,
/*N*/ 											 OutputDevice* pOut )
/*N*/ {
/*N*/ 	DBG_ASSERT(pData, "Kein Chart-Datenobjekt angegeben!");
/*N*/ 	if(!pData)return;
/*N*/ 
/*N*/ 	SchChartDocShellRef aSchChartDocShellRef = &aIPObj;
/*N*/ 
/*N*/ 	if (aSchChartDocShellRef.Is())
/*N*/ 	{
/*N*/ 
/*N*/ 		ChartModel& rDoc = aSchChartDocShellRef->GetDoc();
/*N*/ 
/*N*/ 		rDoc.SetChartData(*new SchMemChart(*pData));
/*N*/ 		rDoc.PutAttr(rAttr);
/*N*/ 
/*N*/ 		if( pOut )
/*N*/ 		{DBG_BF_ASSERT(0, "STRIP");} //STRIP001 	aSchChartDocShellRef->UpdateChart(pOut);
/*N*/ 		else
/*N*/ 			rDoc.BuildChart(TRUE);
/*N*/ 	}
/*N*/ 
/*N*/ 	aIPObj->SendViewChanged();
/*N*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/

/*N*/ extern_c SchMemChart* __LOADONCALLAPI SchGetChartData (SvInPlaceObjectRef aIPObj)
/*N*/ {
/*N*/ 	SchChartDocShellRef aSchChartDocShellRef = &aIPObj;
/*N*/ 
/*N*/ 	if (aSchChartDocShellRef.Is())
/*N*/ 	{
/*N*/ 		ChartModel& rDoc = aSchChartDocShellRef->GetDoc();
/*N*/ 		SchMemChart* pMemChart = rDoc.GetChartData ();
/*N*/ 
/*N*/ 		if (pMemChart)
/*N*/ 		{
/*N*/ 			pMemChart->SetMainTitle(rDoc.MainTitle());
/*N*/ 			pMemChart->SetSubTitle(rDoc.SubTitle());
/*N*/ 			pMemChart->SetXAxisTitle(rDoc.XAxisTitle());
/*N*/ 			pMemChart->SetYAxisTitle(rDoc.YAxisTitle());
/*N*/ 			pMemChart->SetZAxisTitle(rDoc.ZAxisTitle());
/*N*/ 		}
/*N*/ 
/*N*/ 		return pMemChart;
/*N*/ 	}
/*N*/ 	else return 0;
/*N*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/

/*N*/ extern_c SchMemChart* __LOADONCALLAPI SchNewMemChartNone ()
/*N*/ {
/*N*/ 	DBG_BF_ASSERT(0, "STRIP");return NULL; //STRIP001 return new SchMemChart;
/*N*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/

/*N*/ extern_c SchMemChart* __LOADONCALLAPI SchNewMemChartXY (short nCols, short nRows)
/*N*/ {
/*N*/ 	return new SchMemChart (nCols, nRows);
/*N*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/

/*N*/ extern_c SchMemChart* __LOADONCALLAPI SchNewMemChartCopy (const SchMemChart &rMemChart)
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); return NULL;//STRIP001 return new SchMemChart (rMemChart);
/*N*/ }

/*************************************************************************
|*
|*
|*
\************************************************************************/

/*N*/ extern_c ChartModel* __LOADONCALLAPI SchGetModel (SvInPlaceObjectRef aIPObj)
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); return 0; //STRIP001 
/*N*/ }

/*N*/ extern_c void __LOADONCALLAPI SchGetDefaultForColumnText(
/*N*/ 				const SchMemChart& rMemChart, sal_Int32 nCol, String& aResult )
/*N*/ {
/*N*/ 	aResult = rMemChart.GetDefaultColumnText( nCol );
/*N*/ }

/*N*/ extern_c void __LOADONCALLAPI SchGetDefaultForRowText(
/*N*/ 			const SchMemChart& rMemChart, sal_Int32 nRow, String& aResult )
/*N*/ {
/*N*/ 	aResult = rMemChart.GetDefaultRowText( nRow );
/*N*/ }

/*N*/ extern_c void __LOADONCALLAPI SchConvertChartRangeForWriter(
/*N*/ 							SchMemChart& rMemChart, BOOL bOldToNew )
/*N*/ {
/*N*/ 	rMemChart.ConvertChartRangeForWriter( bOldToNew );
/*N*/ }

/*N*/ extern_c void __LOADONCALLAPI SchConvertChartRangeForCalc(
/*N*/ 							SchMemChart& rMemChart, BOOL bOldToNew )
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 rMemChart.ConvertChartRangeForCalc( bOldToNew );
/*N*/ }

/*N*/ extern_c void __LOADONCALLAPI SchMemChartResetTranslation(
/*N*/ 							SchMemChart& rMemChart, long *pTable,long nCnt )
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 rMemChart.ResetTranslation( pTable, nCnt );
/*N*/ }

/*N*/ extern_c void __LOADONCALLAPI SchMemChartUpdateTranslation(
/*N*/ 							SchMemChart& rMemChart, long *pTable, long nCnt )
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 rMemChart.UpdateTranslation( pTable, nCnt );
/*N*/ }

/*N*/ extern_c void __LOADONCALLAPI SchMemChartInsertCols( SchMemChart& rMemChart,
/*N*/ 												short nAtCol, short nCount)
/*N*/ {
/*N*/ 	rMemChart.InsertCols( nAtCol, nCount );
/*N*/ }

/*N*/ extern_c void __LOADONCALLAPI SchMemChartRemoveCols( SchMemChart& rMemChart,
/*N*/ 												short nAtCol, short nCount)
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 rMemChart.RemoveCols( nAtCol, nCount );
/*N*/ }

/*N*/ extern_c void __LOADONCALLAPI SchMemChartInsertRows( SchMemChart& rMemChart,
/*N*/ 												short nAtRow, short nCount)
/*N*/ {
/*N*/ 	rMemChart.InsertRows( nAtRow, nCount );
/*N*/ }

/*N*/ extern_c void __LOADONCALLAPI SchMemChartRemoveRows( SchMemChart& rMemChart,
/*N*/ 												short nAtRow, short nCount)
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 rMemChart.RemoveRows( nAtRow, nCount );
/*N*/ }

/*N*/ extern_c void __LOADONCALLAPI SchMemChartSwapCols( SchMemChart& rMemChart,
/*N*/ 								  				int nAtCol1, int nAtCol2)
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 rMemChart.SwapCols( nAtCol1, nAtCol2 );
/*N*/ }

/*N*/ extern_c void __LOADONCALLAPI SchMemChartSwapRows( SchMemChart& rMemChart,
/*N*/ 								  				int nAtRow1,int nAtRow2)
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 rMemChart.SwapRows( nAtRow1, nAtRow2 );
/*N*/ }

}
