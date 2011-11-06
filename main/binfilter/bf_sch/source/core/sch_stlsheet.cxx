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

#ifndef _EEITEM_HXX //autogen
#include <bf_svx/eeitem.hxx>
#endif

#ifndef _XDEF_HXX //autogen
#include <bf_svx/xdef.hxx>
#endif


#ifndef _SFXITEMSET_HXX //autogen
#include <bf_svtools/itemset.hxx>
#endif

#ifndef _SFXSMPLHINT_HXX //autogen
#include <bf_svtools/smplhint.hxx>
#endif

#include "stlsheet.hxx"
namespace binfilter {

/*N*/ TYPEINIT1(SchStyleSheet, SfxStyleSheet);


/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/

/*N*/ SchStyleSheet::SchStyleSheet(const String& rName, SfxStyleSheetBasePool& rPool,
/*N*/ 							 SfxStyleFamily eFamily, USHORT nMask) :
/*N*/ 	SfxStyleSheet(rName, rPool, eFamily, nMask)
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/

/*N*/ SchStyleSheet::~SchStyleSheet()
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Load
|*
\************************************************************************/

/*N*/ void SchStyleSheet::Load (SvStream& rIn, USHORT nVersion)
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Store
|*
\************************************************************************/

/*N*/ void SchStyleSheet::Store(SvStream& rOut)
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Parent setzen
|*
\************************************************************************/

/*N*/ BOOL SchStyleSheet::SetParent(const String& rParentName)
/*N*/ {
/*N*/ 	if (SfxStyleSheet::SetParent(rParentName))
/*N*/ 	{
/*N*/ 		SfxStyleSheetBase* pStyle = rPool.Find(rParentName, nFamily);
/*N*/ 
/*N*/ 		if (pStyle)
/*N*/ 		{
/*?*/ 			SfxItemSet& rParentSet = pStyle->GetItemSet();
/*?*/ 			GetItemSet().SetParent(&rParentSet);
/*?*/ 			Broadcast(SfxSimpleHint(SFX_HINT_DATACHANGED));
/*?*/ 
/*?*/ 			return TRUE;
/*N*/ 		}
/*N*/ 	}
/*N*/ 
/*N*/ 	return FALSE;
/*N*/ }

/*************************************************************************
|*
|* ItemSet ggfs. erzeugen und herausreichen
|*
\************************************************************************/

/*N*/ SfxItemSet& SchStyleSheet::GetItemSet()
/*N*/ {
/*N*/ 	if (!pSet)
/*N*/ 	{
/*N*/ 		USHORT nWhichPairTable[] = { XATTR_LINE_FIRST, XATTR_LINE_LAST,
/*N*/ 									 XATTR_FILL_FIRST, XATTR_FILL_LAST,
/*N*/ 									 EE_PARA_START, EE_CHAR_END,
/*N*/ 									 (USHORT)0 };
/*N*/ 
/*N*/ 		pSet = new SfxItemSet(GetPool().GetPool(), nWhichPairTable);
/*N*/ 		bMySet = TRUE; //Eigentum erklaeren, damit der DTor der Basisklasse den
/*N*/ 					   //Set wieder abraeumt.
/*N*/ 	}
/*N*/ 
/*N*/ 	return *pSet;
/*N*/ }

/*************************************************************************
|*
|* IsUsed(), wird an Listeners erkannt
|*
\************************************************************************/


/*************************************************************************
|*
|*
|*
\************************************************************************/


/*************************************************************************
|*
|*
|*
\************************************************************************/




}
