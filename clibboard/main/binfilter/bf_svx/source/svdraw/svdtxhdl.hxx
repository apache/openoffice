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



#ifndef _SVDTXHDL_HXX
#define _SVDTXHDL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _VIRDEV_HXX //autogen
#include <vcl/virdev.hxx>
#endif


namespace binfilter {

//************************************************************
//   Vorausdeklarationen
//************************************************************

class SdrOutliner;
class DrawPortionInfo;
class SdrTextObj;
class SdrObjGroup;
class SdrModel;
class ExtOutputDevice;

//************************************************************
//   ImpTextPortionHandler
//************************************************************

class ImpTextPortionHandler
{
	VirtualDevice				aVDev;
	Rectangle					aFormTextBoundRect;
	SdrOutliner&				rOutliner;
	const SdrTextObj&			rTextObj;
	ExtOutputDevice*			pXOut;

	// Variablen fuer ConvertToPathObj
	SdrObjGroup*				pGroup;
	SdrModel*					pModel;
	FASTBOOL					bToPoly;

	// Variablen fuer DrawFitText
	Point						aPos;
	Fraction					aXFact;
	Fraction					aYFact;

	// Variablen fuer DrawTextToPath
	// #101498#
	//Polygon						aPoly;
	//long						nTextWidth;
	ULONG						nParagraph;
	BOOL						bToLastPoint;
	BOOL						bDraw;
	void*						mpRecordPortions;

private:
	// #101498#
	void SortedAddFormTextRecordPortion(DrawPortionInfo* pInfo);
	void DrawFormTextRecordPortions(Polygon aPoly);
	void ClearFormTextRecordPortions();
	sal_uInt32 GetFormTextPortionsLength(OutputDevice* pOut);

public:
	ImpTextPortionHandler(SdrOutliner& rOutln, const SdrTextObj& rTxtObj);

	void DrawTextToPath(ExtOutputDevice& rXOut, FASTBOOL bDrawEffect=TRUE);

	// wird von DrawTextToPath() gesetzt:
	const Rectangle& GetFormTextBoundRect() { return aFormTextBoundRect; }


	// #101498#
	DECL_LINK(FormTextRecordPortionHdl, DrawPortionInfo*);
	//DECL_LINK(FormTextWidthHdl,DrawPortionInfo*);
	//DECL_LINK(FormTextDrawHdl,DrawPortionInfo*);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif //_SVDTXHDL_HXX

