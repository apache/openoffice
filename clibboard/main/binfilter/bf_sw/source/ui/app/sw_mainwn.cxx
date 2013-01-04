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


#ifndef _SFX_PROGRESS_HXX //autogen
#include <bf_sfx2/progress.hxx>
#endif

#ifndef _SWDOCSH_HXX //autogen
#include <docsh.hxx>
#endif
#ifndef _SWMODULE_HXX //autogen
#include <swmodule.hxx>
#endif
#include "swtypes.hxx"
namespace binfilter {

class SwDocShell;

struct SwProgress
{
	long nStartValue,
		 nStartCount;
	SwDocShell  *pDocShell;
	SfxProgress *pProgress;
};

static SvPtrarr *pProgressContainer = 0;

/*N*/ SwProgress *lcl_SwFindProgress( SwDocShell *pDocShell )
/*N*/ {
/*N*/ 	for ( USHORT i = 0; i < pProgressContainer->Count(); ++i )
/*N*/ 	{
/*N*/ 		SwProgress *pTmp = (SwProgress*)(*pProgressContainer)[i];
/*N*/ 		if ( pTmp->pDocShell == pDocShell )
/*N*/ 			return pTmp;
/*N*/ 	}
/*N*/ 	return 0;
/*N*/ }


/*N*/ void StartProgress( USHORT nMessResId, long nStartValue, long nEndValue,
/*N*/ 					SwDocShell *pDocShell )
/*N*/ {
/*N*/ 	if( !SW_MOD()->IsEmbeddedLoadSave() )
/*N*/ 	{
/*N*/ 		SwProgress *pProgress = 0;
/*N*/ 
/*N*/ 		if ( !pProgressContainer )
/*N*/ 			pProgressContainer = new SvPtrarr( 2, 2 );
/*N*/ 		else
/*N*/ 		{
/*?*/ 			if ( 0 != (pProgress = lcl_SwFindProgress( pDocShell )) )
/*?*/ 				++pProgress->nStartCount;
/*N*/ 		}
/*N*/ 		if ( !pProgress )
/*N*/ 		{
/*N*/ 			pProgress = new SwProgress;
/*N*/ 			pProgress->pProgress = new SfxProgress( pDocShell,
/*N*/ 													SW_RESSTR(nMessResId),
/*N*/ 													nEndValue - nStartValue,
/*N*/ 													FALSE,
/*N*/ 													TRUE );
/*N*/ 			pProgress->nStartCount = 1;
/*N*/ 			pProgress->pDocShell = pDocShell;
/*N*/ 			pProgressContainer->Insert( (void*)pProgress, 0 );
/*N*/ 		}
/*N*/ 		pProgress->nStartValue = nStartValue;
/*N*/ 	}
/*N*/ }


/*N*/ void SetProgressState( long nPosition, SwDocShell *pDocShell )
/*N*/ {
/*N*/ 	if( pProgressContainer && !SW_MOD()->IsEmbeddedLoadSave() )
/*N*/ 	{
/*N*/ 		SwProgress *pProgress = lcl_SwFindProgress( pDocShell );
/*N*/ 		if ( pProgress )
/*N*/ 			pProgress->pProgress->SetState(nPosition - pProgress->nStartValue);
/*N*/ 	}
/*N*/ }


/*N*/ void EndProgress( SwDocShell *pDocShell )
/*N*/ {
/*N*/ 	if( pProgressContainer && !SW_MOD()->IsEmbeddedLoadSave() )
/*N*/ 	{
/*N*/ 		SwProgress *pProgress = 0;
/*N*/ 		USHORT i;
/*N*/ 		for ( i = 0; i < pProgressContainer->Count(); ++i )
/*N*/ 		{
/*N*/ 			SwProgress *pTmp = (SwProgress*)(*pProgressContainer)[i];
/*N*/ 			if ( pTmp->pDocShell == pDocShell )
/*N*/ 			{
/*N*/ 				pProgress = pTmp;
/*N*/ 				break;
/*N*/ 			}
/*N*/ 		}
/*N*/ 
/*N*/ 		if ( pProgress && 0 == --pProgress->nStartCount )
/*N*/ 		{
/*N*/ 			pProgress->pProgress->Stop();
/*N*/ 			pProgressContainer->Remove( i );
/*N*/ 			delete pProgress->pProgress;
/*N*/ 			delete pProgress;
/*N*/ 			if ( !pProgressContainer->Count() )
/*N*/ 				delete pProgressContainer, pProgressContainer = 0;
/*N*/ 		}
/*N*/ 	}
/*N*/ }




/*N*/ void RescheduleProgress( SwDocShell *pDocShell )
/*N*/ {
/*N*/ 	if( pProgressContainer && !SW_MOD()->IsEmbeddedLoadSave() )
/*N*/ 	{
/*N*/ 		SwProgress *pProgress = lcl_SwFindProgress( pDocShell );
/*N*/ 		if ( pProgress )
/*N*/ 			pProgress->pProgress->Reschedule();
/*N*/ 	}
/*N*/ }

}
