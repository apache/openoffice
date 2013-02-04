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



#ifndef _SVDITER_HXX
#include "svditer.hxx"
#endif



// #99190#
#ifndef _E3D_SCENE3D_HXX
#include "scene3d.hxx"
#endif
namespace binfilter {

/*N*/ SdrObjListIter::SdrObjListIter(const SdrObjList& rObjList, SdrIterMode eMode, BOOL bReverse)
/*N*/ :	maObjList(1024, 64, 64),
/*N*/ 	mnIndex(0L),
/*N*/ 	mbReverse(bReverse)
/*N*/ {
/*N*/ 	ImpProcessObjectList(rObjList, eMode);
/*N*/ 	Reset();
/*N*/ }

/*N*/ SdrObjListIter::SdrObjListIter(const SdrObject& rGroup, SdrIterMode eMode, BOOL bReverse)
/*N*/ :	maObjList(1024, 64, 64),
/*N*/ 	mnIndex(0L),
/*N*/ 	mbReverse(bReverse)
/*N*/ {
/*N*/ 	ImpProcessObjectList(*rGroup.GetSubList(), eMode);
/*N*/ 	Reset();
/*N*/ }

/*N*/ void SdrObjListIter::ImpProcessObjectList(const SdrObjList& rObjList, SdrIterMode eMode)
/*N*/ {
/*N*/ 	for(sal_uInt32 a(0L); a < rObjList.GetObjCount(); a++)
/*N*/ 	{
/*N*/ 		SdrObject* pObj = rObjList.GetObj(a);
/*N*/ 		sal_Bool bIsGroup(pObj->IsGroupObject());
/*N*/ 
/*N*/ 		// #99190# 3D objects are no group objects, IsGroupObject()
/*N*/ 		// only tests if pSub is not null ptr :-(
/*N*/ 		if(bIsGroup && pObj->ISA(E3dObject) && !pObj->ISA(E3dScene))
/*N*/ 			bIsGroup = sal_False;
/*N*/ 
/*N*/ 		if(eMode != IM_DEEPNOGROUPS || !bIsGroup)
/*N*/ 			maObjList.Insert(pObj, LIST_APPEND);
/*N*/ 
/*N*/ 		if(bIsGroup && eMode != IM_FLAT)
/*N*/ 			ImpProcessObjectList(*pObj->GetSubList(), eMode);
/*N*/ 	}
/*N*/ }

}
