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


#include "sdpage.hxx"
#include "sdobjfac.hxx"
#include "anminfo.hxx"
#include "imapinfo.hxx"
namespace binfilter {


SdObjectFactory aSdObjectFactory;

/*************************************************************************
|*
|* void SdObjectFactory::MakeUserData()
|*
\************************************************************************/

/*N*/ IMPL_LINK( SdObjectFactory, MakeUserData, SdrObjFactory *, pObjFactory )
/*N*/ {
/*N*/ 	if ( pObjFactory->nInventor == SdUDInventor )
/*N*/ 	{
/*N*/ 		SdrObject* pObj = pObjFactory->pObj;
/*N*/ 
/*N*/ 		switch( pObjFactory->nIdentifier )
/*N*/ 		{
/*N*/ 			case( SD_ANIMATIONINFO_ID ):
/*N*/ 			{
/*N*/ 				SdDrawDocument* pDoc = NULL;
/*N*/ 
/*N*/ 				if ( pObj )
/*N*/ 					pDoc = (SdDrawDocument*) pObj->GetModel();
/*N*/ 
/*N*/ 				DBG_ASSERT(pDoc, "kein Model gefunden");
/*N*/ 
/*N*/ 				pObjFactory->pNewData = new SdAnimationInfo(pDoc);
/*N*/ 			}
/*N*/ 			break;
/*?*/ 
/*?*/ 			case( SD_IMAPINFO_ID ):
/*?*/ 				pObjFactory->pNewData = new SdIMapInfo;
/*?*/ 			break;
/*N*/ 
/*N*/ 			default:
/*N*/ 			break;
/*N*/ 		}
/*N*/ 	}
/*N*/ 
/*N*/ 	if ( pObjFactory->pNewData )
/*N*/ 		return 0;
/*N*/ 
/*N*/ 	if( aOldMakeUserDataLink.IsSet() )
/*?*/ 		aOldMakeUserDataLink.Call( this );
/*N*/ 
/*N*/ 	return 0;
/*N*/ }


}
