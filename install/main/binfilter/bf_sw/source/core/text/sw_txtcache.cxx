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

#include "txtcache.hxx"
#include "txtfrm.hxx"
#include "porlay.hxx"
namespace binfilter {

/*************************************************************************
|*
|*	SwTxtLine::SwTxtLine(), ~SwTxtLine()
|*
|*	Ersterstellung		MA 16. Mar. 94
|*	Letzte Aenderung	MA 16. Mar. 94
|*
|*************************************************************************/

/*N*/ SwTxtLine::SwTxtLine( SwTxtFrm *pFrm, SwParaPortion *pNew ) :
/*N*/ 	SwCacheObj( (void*)pFrm ),
/*N*/ 	pLine( pNew )
/*N*/ {
/*N*/ }
/*N*/ 
/*N*/ SwTxtLine::~SwTxtLine()
/*N*/ {
/*N*/ 	delete pLine;
/*N*/ }

/*************************************************************************
|*
|*	SwTxtLineAccess::NewObj()
|*
|*	Ersterstellung		MA 16. Mar. 94
|*	Letzte Aenderung	MA 16. Mar. 94
|*
|*************************************************************************/

/*N*/ SwCacheObj *SwTxtLineAccess::NewObj()
/*N*/ {
/*N*/ 	return new SwTxtLine( (SwTxtFrm*)pOwner );
/*N*/ }

/*************************************************************************
|*
|*	SwTxtLineAccess::GetPara()
|*
|*	Ersterstellung		MA 16. Mar. 94
|*	Letzte Aenderung	MA 16. Mar. 94
|*
|*************************************************************************/

/*N*/ SwParaPortion *SwTxtLineAccess::GetPara()
/*N*/ {
/*N*/ 	SwTxtLine *pRet;
/*N*/ 	if ( pObj )
/*N*/ 		pRet = (SwTxtLine*)pObj;
/*N*/ 	else
/*N*/ 	{
/*N*/ 		pRet = (SwTxtLine*)Get();
/*N*/ 		((SwTxtFrm*)pOwner)->SetCacheIdx( pRet->GetCachePos() );
/*N*/ 	}
/*N*/ 	if ( !pRet->GetPara() )
/*N*/ 		pRet->SetPara( new SwParaPortion );
/*N*/ 	return pRet->GetPara();
/*N*/ }


/*************************************************************************
|*
|*	SwTxtLineAccess::SwTxtLineAccess()
|*
|*	Ersterstellung		MA 16. Mar. 94
|*	Letzte Aenderung	MA 16. Mar. 94
|*
|*************************************************************************/

/*N*/ SwTxtLineAccess::SwTxtLineAccess( const SwTxtFrm *pOwner ) :
/*N*/ 	SwCacheAccess( *SwTxtFrm::GetTxtCache(), pOwner, pOwner->GetCacheIdx() )
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|*	SwTxtLineAccess::IsAvailable
|*
|*	Ersterstellung		MA 23. Mar. 94
|*	Letzte Aenderung	MA 23. Mar. 94
|*
|*************************************************************************/

/*N*/ sal_Bool SwTxtLineAccess::IsAvailable() const
/*N*/ {
/*N*/ 	if ( pObj )
/*N*/ 		return ((SwTxtLine*)pObj)->GetPara() != 0;
/*N*/ 	return sal_False;
/*N*/ }

/*************************************************************************
|*
|*	SwTxtFrm::HasPara()
|*
|*	Ersterstellung		MA 16. Mar. 94
|*	Letzte Aenderung	MA 22. Aug. 94
|*
|*************************************************************************/

/*N*/ sal_Bool SwTxtFrm::_HasPara() const
/*N*/ {
/*N*/ 	SwTxtLine *pTxtLine = (SwTxtLine*)SwTxtFrm::GetTxtCache()->
/*N*/ 											Get( this, GetCacheIdx(), sal_False );
/*N*/ 	if ( pTxtLine )
/*N*/ 	{
/*N*/ 		if ( pTxtLine->GetPara() )
/*N*/ 			return sal_True;
/*N*/ 	}
/*N*/ 	else
/*N*/ 		((SwTxtFrm*)this)->nCacheIdx = MSHRT_MAX;
/*N*/ 
/*N*/ 	return sal_False;
/*N*/ }

/*************************************************************************
|*
|*	SwTxtFrm::GetPara()
|*
|*	Ersterstellung		MA 16. Mar. 94
|*	Letzte Aenderung	MA 22. Aug. 94
|*
|*************************************************************************/

/*N*/ SwParaPortion *SwTxtFrm::GetPara()
/*N*/ {
/*N*/ 	if ( GetCacheIdx() != MSHRT_MAX )
/*N*/ 	{	SwTxtLine *pLine = (SwTxtLine*)SwTxtFrm::GetTxtCache()->
/*N*/ 										Get( this, GetCacheIdx(), sal_False );
/*N*/ 		if ( pLine )
/*N*/ 			return pLine->GetPara();
/*N*/ 		else
/*N*/ 			nCacheIdx = MSHRT_MAX;
/*N*/ 	}
/*N*/ 	return 0;
/*N*/ }


/*************************************************************************
|*
|*	SwTxtFrm::ClearPara()
|*
|*	Ersterstellung		MA 16. Mar. 94
|*	Letzte Aenderung	MA 22. Aug. 94
|*
|*************************************************************************/

/*N*/ void SwTxtFrm::ClearPara()
/*N*/ {
/*N*/ 	ASSERT( !IsLocked(), "+SwTxtFrm::ClearPara: this is locked." );
/*N*/ 	if ( !IsLocked() && GetCacheIdx() != MSHRT_MAX )
/*N*/ 	{
/*N*/ 		SwTxtLine *pTxtLine = (SwTxtLine*)SwTxtFrm::GetTxtCache()->
/*N*/ 										Get( this, GetCacheIdx(), sal_False );
/*N*/ 		if ( pTxtLine )
/*N*/ 		{
/*N*/ 			delete pTxtLine->GetPara();
/*N*/ 			pTxtLine->SetPara( 0 );
/*N*/ 		}
/*N*/ 		else
/*N*/ 			nCacheIdx = MSHRT_MAX;
/*N*/ 	}
/*N*/ }

/*************************************************************************
|*
|*	SwTxtFrm::SetPara()
|*
|*	Ersterstellung		MA 16. Mar. 94
|*	Letzte Aenderung	MA 22. Aug. 94
|*
|*************************************************************************/

/*N*/ void SwTxtFrm::SetPara( SwParaPortion *pNew, sal_Bool bDelete )
/*N*/ {
/*N*/ 	if ( GetCacheIdx() != MSHRT_MAX )
/*N*/ 	{
/*N*/ 		//Nur die Information Auswechseln, das CacheObj bleibt stehen.
/*N*/ 		SwTxtLine *pTxtLine = (SwTxtLine*)SwTxtFrm::GetTxtCache()->
/*N*/ 										Get( this, GetCacheIdx(), sal_False );
/*N*/ 		if ( pTxtLine )
/*N*/ 		{
/*N*/ 			if( bDelete )
/*N*/ 				delete pTxtLine->GetPara();
/*N*/ 			pTxtLine->SetPara( pNew );
/*N*/ 		}
/*N*/ 		else
/*N*/ 		{
/*?*/ 			ASSERT( !pNew, "+SetPara: Losing SwParaPortion" );
/*?*/ 			nCacheIdx = MSHRT_MAX;
/*N*/ 		}
/*N*/ 	}
/*N*/ 	else if ( pNew )
/*N*/ 	{	//Einen neuen einfuegen.
/*N*/ 		SwTxtLine *pTxtLine = new SwTxtLine( this, pNew );
/*N*/ 		if ( SwTxtFrm::GetTxtCache()->Insert( pTxtLine ) )
/*N*/ 			nCacheIdx = pTxtLine->GetCachePos();
/*N*/ 		else
/*N*/ 		{
/*?*/ 			ASSERT( sal_False, "+SetPara: InsertCache failed." );
/*N*/ 		}
/*N*/ 	}
/*N*/ }
/*N*/ 
/*N*/ 
}
