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



#ifndef _SCH_OBJADJ_HXX
#define _SCH_OBJADJ_HXX

#ifndef _SCH_ADJUST_HXX
#include "adjust.hxx"
#endif

#ifndef _SVX_CHRTITEM_HXX //autogen
#include <bf_svx/chrtitem.hxx>
#endif
#ifndef _SVDOBJ_HXX //autogen
#include <bf_svx/svdobj.hxx>
#endif
#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
namespace binfilter {


/*************************************************************************
|*
|* Ausrichtung von Chart-Objekten
|*
\************************************************************************/

class SchObjectAdjust : public SdrObjUserData
{
	ChartAdjust			eAdjust;	// Ausrichtung
	SvxChartTextOrient	eOrient;	// Orientierung

public:
	SchObjectAdjust();
	SchObjectAdjust(ChartAdjust eAdj, SvxChartTextOrient eOr);

	virtual SdrObjUserData* Clone(SdrObject *pObj) const;

	virtual void WriteData(SvStream& rOut);
	virtual void ReadData(SvStream& rIn);

	void SetAdjust(ChartAdjust eAdj) { eAdjust = eAdj; }
	ChartAdjust GetAdjust() { return eAdjust; }

	void SetOrient(SvxChartTextOrient eOr) { eOrient = eOr; }
	SvxChartTextOrient GetOrient() { return eOrient; }
};

/*************************************************************************
|*
|* Tool-Funktion fuer Objekt-Ausrichtung
|*
\************************************************************************/

extern SchObjectAdjust* GetObjectAdjust(const SdrObject& rObj);

} //namespace binfilter
#endif	// _SCH_OBJADJ_HXX


