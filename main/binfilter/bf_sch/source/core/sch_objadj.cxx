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

#include "objadj.hxx"
#include "glob.hxx"

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

namespace binfilter {




/*************************************************************************
|*
|* Objekt-Ausrichting ermitteln
|*
\************************************************************************/

/*N*/ SchObjectAdjust* GetObjectAdjust(const SdrObject& rObj)
/*N*/ {
/*N*/ 	for (USHORT i = 0; i < rObj.GetUserDataCount(); i++)
/*N*/ 	{
/*N*/ 		SdrObjUserData *pData = rObj.GetUserData(i);
/*N*/ 		if (pData && pData->GetId() == SCH_OBJECTADJUST_ID)
/*N*/ 			return (SchObjectAdjust*)pData;
/*N*/ 	}
/*N*/ 
/*?*/ 	return NULL;
/*N*/ }

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

/*N*/ SchObjectAdjust::SchObjectAdjust() :
/*N*/ 	SdrObjUserData(SchInventor, SCH_OBJECTADJUST_ID, 1),
/*N*/ 	eAdjust(CHADJUST_TOP_LEFT),
/*N*/ 	eOrient(CHTXTORIENT_STANDARD)
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

/*N*/ SchObjectAdjust::SchObjectAdjust(ChartAdjust eAdj,
/*N*/ 								 SvxChartTextOrient eOr) :
/*N*/ 	SdrObjUserData(SchInventor, SCH_OBJECTADJUST_ID, 1),
/*N*/ 	eAdjust(eAdj),
/*N*/ 	eOrient(eOr)
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Kopier-Konstruktor
|*
\************************************************************************/


/*************************************************************************
|*
|* Kopie erzeugen
|*
\************************************************************************/

/*N*/ SdrObjUserData* SchObjectAdjust::Clone(SdrObject *pObj) const
/*N*/ {
/*?*/  	DBG_BF_ASSERT(0, "STRIP"); return NULL;//STRIP001 return new SchObjectAdjust(*this);
/*N*/ }

/*************************************************************************
|*
|* Daten in Stream schreiben
|*
\************************************************************************/

/*N*/ void SchObjectAdjust::WriteData(SvStream& rOut)
/*N*/ {
/*N*/ 	SdrObjUserData::WriteData(rOut);
/*N*/ 
/*N*/ 	rOut << (INT16)eAdjust;
/*N*/ 	rOut << (INT16)eOrient;
/*N*/ }

/*************************************************************************
|*
|* Daten aus Stream lesen
|*
\************************************************************************/

/*N*/ void SchObjectAdjust::ReadData(SvStream& rIn)
/*N*/ {
/*N*/ 	SdrObjUserData::ReadData(rIn);
/*N*/ 
/*N*/ 	INT16 nInt16;
/*N*/ 
/*N*/ 	rIn >> nInt16; eAdjust = (ChartAdjust)nInt16;
/*N*/ 
/*N*/ 	if (nVersion < 1)
/*?*/ 		eOrient = CHTXTORIENT_STANDARD;
/*N*/ 	else
/*N*/ 	{
/*N*/ 		rIn >> nInt16; eOrient = (SvxChartTextOrient)nInt16;
/*N*/ 	}
/*N*/ 
/*N*/ }



}
