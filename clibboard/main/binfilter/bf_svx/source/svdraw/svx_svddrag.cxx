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



#ifndef _SVDDRAG_HXX //autogen
#include "svddrag.hxx"
#endif
namespace binfilter {

/*N*/ void SdrDragStat::Clear(FASTBOOL bLeaveOne)
/*N*/ {
/*N*/ 	void* pP=aPnts.First();
/*N*/ 	while (pP!=NULL) {
/*N*/ 		delete (Point*)pP;
/*N*/ 		pP=aPnts.Next();
/*N*/ 	}
/*N*/ 	if (pUser!=NULL) delete pUser;
/*N*/ 	pUser=NULL;
/*N*/ 	aPnts.Clear();
/*N*/ 	if (bLeaveOne) {
/*N*/ 		aPnts.Insert(new Point,CONTAINER_APPEND);
/*N*/ 	}
/*N*/ }

/*N*/ void SdrDragStat::Reset()
/*N*/ {
/*N*/ 	pView=NULL;
/*N*/ 	pPageView=NULL;
/*N*/ 	bShown=FALSE;
/*N*/ 	nMinMov=1;
/*N*/ 	bMinMoved=FALSE;
/*N*/ 	bHorFixed=FALSE;
/*N*/ 	bVerFixed=FALSE;
/*N*/ 	bWantNoSnap=FALSE;
/*N*/ 	pHdl=NULL;
/*N*/ 	bOrtho4=FALSE;
/*N*/ 	bOrtho8=FALSE;
/*N*/ 	pDragMethod=NULL;
/*N*/ 	bEndDragChangesAttributes=FALSE;
/*N*/ 	bEndDragChangesGeoAndAttributes=FALSE;
/*N*/ 	bMouseIsUp=FALSE;
/*N*/ 	Clear(TRUE);
/*N*/ 	aActionRect=Rectangle();
/*N*/ }










}
