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



#include "svdetc.hxx"
#include "svdmrkv.hxx"







#include "svdstr.hrc"


// #105678#
namespace binfilter {

/*N*/ SdrHdl::~SdrHdl()
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ class ImpSdrHdlListSorter: public ContainerSorter {
/*N*/ public:
/*N*/     ImpSdrHdlListSorter(Container& rNewCont): ContainerSorter(rNewCont) {}
/*N*/     virtual int Compare(const void* pElem1, const void* pElem2) const;
/*N*/ };

/*N*/ int ImpSdrHdlListSorter::Compare(const void* pElem1, const void* pElem2) const
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); return 0;//STRIP001 
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////
// #97016# II

/*N*/ class ImplHdlListData
/*N*/ {
/*N*/ public:
/*N*/ 	sal_uInt32					mnFocusIndex;
/*N*/ 	SdrMarkView*				pView;
/*N*/ 
/*N*/ 	ImplHdlListData(SdrMarkView* pV): mnFocusIndex(CONTAINER_ENTRY_NOTFOUND), pView(pV) {}
/*N*/ };


// #105678# Help struct for re-sorting handles
struct ImplHdlAndIndex
{
	SdrHdl*						mpHdl;
	sal_uInt32					mnIndex;
};

// #105678# Help method for sorting handles taking care of OrdNums, keeping order in
// single objects and re-sorting polygon handles intuitively

////////////////////////////////////////////////////////////////////////////////////////////////////
// #97016# II


/*N*/ SdrHdl* SdrHdlList::GetFocusHdl() const
/*N*/ {
/*N*/ 	if(pImpl->mnFocusIndex != CONTAINER_ENTRY_NOTFOUND && pImpl->mnFocusIndex < GetHdlCount())
/*?*/ 		return GetHdl(pImpl->mnFocusIndex);
/*N*/ 	else
/*?*/ 		return 0L;
/*N*/ }

////////////////////////////////////////////////////////////////////////////////////////////////////

/*N*/ SdrHdlList::SdrHdlList(SdrMarkView* pV)
/*N*/ :	aList(1024,32,32),
/*N*/ 	pImpl(new ImplHdlListData(pV))
/*N*/ 	//#97016# II
/*N*/ 	//pView(pV)
/*N*/ { 
/*N*/ 	nHdlSize = 3; 
/*N*/ 	bRotateShear = FALSE; 
/*N*/ 	bMoveOutside = FALSE; 
/*N*/ 	bDistortShear = FALSE; 
/*N*/ 	bFineHandles = FALSE;
/*N*/ }
/*N*/ 
/*N*/ SdrHdlList::~SdrHdlList() 
/*N*/ { 
/*N*/ 	Clear(); 
/*N*/ 	//#97016# II
/*N*/ 	delete pImpl;
/*N*/ }



/*N*/ void SdrHdlList::SetRotateShear(BOOL bOn)              
/*N*/ { 
/*N*/ 	bRotateShear = bOn; 
/*N*/ }

/*N*/ void SdrHdlList::SetDistortShear(BOOL bOn)             
/*N*/ { 
/*N*/ 	bDistortShear = bOn; 
/*N*/ }

/*N*/ void SdrHdlList::SetFineHdl(BOOL bOn)                 
/*N*/ { 
/*N*/ 	if(bFineHandles != bOn)
/*N*/ 	{
/*N*/ 		// remember new state
/*N*/ 		bFineHandles = bOn; 
/*N*/ 
/*N*/ 		// propagate change to IAOs
/*N*/ 		for(UINT32 i=0; i<GetHdlCount(); i++) 
/*N*/ 		{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ 		}
/*N*/ 	}
/*N*/ }


/*N*/ void SdrHdlList::Clear()
/*N*/ {
/*N*/ 	for (ULONG i=0; i<GetHdlCount(); i++) 
/*N*/ 	{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ 	}
/*N*/ 	aList.Clear();
/*N*/ 
/*N*/ 	// immediately remove from display
//STRIP012/*N*/ 	if(pImpl->pView)
//STRIP012/*N*/ 		pImpl->pView->RefreshAllIAOManagers();
/*N*/ 
/*N*/ 	bRotateShear=FALSE;
/*N*/ 	bDistortShear=FALSE;
/*N*/ }

/*N*/ void SdrHdlList::Sort()
/*N*/ {
/*N*/ 	// #97016# II: remember current focused handle
/*N*/ 	SdrHdl* pPrev = GetFocusHdl();
/*N*/ 
/*N*/     ImpSdrHdlListSorter aSort(aList);
/*N*/     aSort.DoSort();

	// #97016# II: get now and compare
/*N*/ 	SdrHdl* pNow = GetFocusHdl();
/*N*/ 
/*N*/ 	if(pPrev != pNow)
/*N*/ 	{DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ 	}
/*N*/ }

}
