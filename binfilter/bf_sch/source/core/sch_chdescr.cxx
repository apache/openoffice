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


#ifndef _SFXENUMITEM_HXX
#include <bf_svtools/eitem.hxx>
#endif

#include "chdescr.hxx"

#include "float.h"
#include "schgroup.hxx"
#include "chtscene.hxx"
namespace binfilter {

/*N*/ ChartDataDescription::ChartDataDescription(long nCols, long nRows, SdrObjList *pList,
/*N*/ 										   ChartModel* pModel, BOOL bEnable) :
/*N*/ 		mnRows(nRows),
/*N*/ 		mnCols(nCols),
/*N*/ 		mpList(pList),
/*N*/ 		mpModel(pModel),
/*N*/ 		mbEnable(bEnable),
/*N*/ 		mpDescrLists(NULL),
/*N*/ 		mpDescrArray(NULL),
/*N*/ 		mpDescrGroups(NULL)
/*N*/ {
/*N*/ }

/*N*/ void ChartDataDescription::Build( BOOL bRowDescr )
/*N*/ {
/*N*/ 	Dirty2D( bRowDescr );
/*N*/ 	if(mpList && mpDescrGroups)
/*N*/ 	{
/*?*/ 		for( long nRow=0; nRow<mnRows; nRow++ )
/*?*/ 		{
/*?*/ 			if(mpDescrGroups[nRow])
/*?*/ 				mpList->NbcInsertObject( mpDescrGroups[nRow] );
/*?*/ 		}
/*?*/ 	}
/*N*/ }

/*N*/ void ChartDataDescription::Build3D( ChartScene *pScene, Matrix4D* pMatrix )
/*N*/ {
/*N*/ 	if(pScene && mpDescrGroups)
/*N*/ 	{
/*?*/ 		for( long nRow=0; nRow<mnRows; nRow++ )
/*?*/ 		{
/*?*/ 			for( long nCol=0; nCol<mnCols; nCol++ )
/*?*/ 			{
/*?*/ 				long nIndex = nCol + nRow * mnCols;
/*?*/ 				if( mpDescrArray[nIndex].fValue != DBL_MIN )
/*?*/ 				{
/*?*/ 					mpDescrArray[nIndex].pLabelObj->SetMarkProtect(TRUE);
/*?*/ 					E3dLabelObj* pLabel=new E3dLabelObj(mpDescrArray[nIndex].aTextPos3D,
/*?*/ 														mpDescrArray[nIndex].pLabelObj);
/*?*/ 
/*?*/ 					CHART_TRACE3( "Descr::Build3D TextPos = (%ld, %ld, %ld)",
/*?*/ 								  mpDescrArray[ nIndex ].aTextPos3D.X(),
/*?*/ 								  mpDescrArray[ nIndex ].aTextPos3D.Y(),
/*?*/ 								  mpDescrArray[ nIndex ].aTextPos3D.Z() );
/*?*/ 					
/*?*/ 					pLabel->SetMarkProtect(TRUE);
/*?*/ 					pScene->Insert3DObj(pLabel);
/*?*/ 					if(pMatrix)
/*?*/ 						pLabel->NbcSetTransform(*pMatrix);
/*?*/ 				}
/*?*/ 			}
/*?*/ 		}
/*?*/ 	}
/*N*/ }

/*N*/ ChartDataDescription::~ChartDataDescription()
/*N*/ {
/*N*/ 	if(mpDescrLists)
/*?*/ 		delete[] mpDescrLists;
/*N*/ 	if(mpDescrGroups)
/*?*/ 		delete[] mpDescrGroups;
/*N*/ 	if(mpDescrArray)
/*?*/ 		delete[] mpDescrArray;
/*N*/ }


/*N*/ DataDescription* ChartDataDescription::Insert( long nCol, long nRow, const SfxItemSet& rAttr, Point aPos,
/*N*/ 											   BOOL bPercent, ChartAdjust eAdjust, ChartAxis* pAxis )
/*N*/ {
/*N*/ 	SvxChartDataDescr eDescr = ((const SvxChartDataDescrItem&)rAttr.Get(SCHATTR_DATADESCR_DESCR)).GetValue();
/*N*/ 
/*N*/ 	if(mbEnable && eDescr != CHDESCR_NONE)
/*N*/ 	{
/*?*/ 		DBG_BF_ASSERT(0, "STRIP"); //STRIP001 Create(nRow); //evtl. Array und Liste erstellen
/*N*/ 	}
/*N*/ 	return NULL;
/*N*/ }

/*N*/ void ChartDataDescription::Dirty2D( BOOL bRowDescr )
/*N*/ {
/*N*/ 
/*N*/ 	if(mpDescrLists)
/*N*/ 	{
/*?*/ 		for( long nRow = 0; nRow < mnRows; nRow ++ )
/*?*/ 		{
/*?*/ 			if( mpDescrLists[nRow] )
/*?*/ 			{
/*?*/ 				for( long nCol = 0; nCol < mnCols; nCol++ )
/*?*/ 				{
/*?*/ 						long nIndex = nCol + nRow * mnCols;
/*?*/ 						if (mpDescrArray[nIndex].fValue != DBL_MIN)
/*?*/ 							if (mpDescrArray[nIndex].pLabelObj)
/*?*/ 								mpDescrLists[nRow]->NbcInsertObject(mpDescrArray[nIndex].pLabelObj);
/*?*/ 				}
/*?*/ 			}
/*?*/ 		}
/*N*/ 	}
/*N*/ }

}
