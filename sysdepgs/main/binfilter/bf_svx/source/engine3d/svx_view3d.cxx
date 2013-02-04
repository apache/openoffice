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



#define ITEMID_COLOR 0











#ifndef _XOUTX_HXX
#include "xoutx.hxx"
#endif

#ifndef _SVX_SVXIDS_HRC
#include <svxids.hrc>
#endif


#ifndef _XTABLE_HXX
#include "xtable.hxx"
#endif


#ifndef _SVX_DIALOGS_HRC
#include "dialogs.hrc"
#endif










#ifndef _E3D_POLYSC3D_HXX
#include "polysc3d.hxx"
#endif


#ifndef _E3D_VIEW3D_HXX
#include "view3d.hxx"
#endif







namespace binfilter {

#define ITEMVALUE(ItemSet,Id,Cast)	((const Cast&)(ItemSet).Get(Id)).GetValue()

/*N*/ TYPEINIT1(E3dView, SdrView);

long Scalar (Point aPoint1,
			 Point aPoint2);

Point ScaleVector (Point  aPoint,
				   double nScale);

double NormVector (Point aPoint);

BOOL LineCutting (Point aP1,
				  Point aP2,
				  Point aP3,
				  Point aP4);

long Point2Line (Point aP1,
				 Point aP2,
				 Point aP3);

long DistPoint2Line (Point u,
					 Point v1,
					 Point v);

/*************************************************************************
|*
|* Konstruktor 1
|*
\************************************************************************/

/*N*/ E3dView::E3dView(SdrModel* pModel, OutputDevice* pOut) :
/*N*/     SdrView(pModel, pOut)
/*N*/ {
/*N*/ 	InitView ();
/*N*/ }

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

/*N*/ E3dView::~E3dView ()
/*N*/ {
/*N*/ 	/*#90353#*/delete [] pMirrorPolygon;
/*N*/ 	/*#90353#*/delete [] pMirroredPolygon;
/*N*/ 	/*#90353#*/delete [] pMarkedObjs;
/*N*/ }

/*************************************************************************
|*
|* Klasse initialisieren
|*
\************************************************************************/

/*N*/ void E3dView::InitView ()
/*N*/ {
/*N*/ 	eDragConstraint 		 = E3DDRAG_CONSTR_XYZ;
/*N*/ 	eDragDetail 			 = E3DDETAIL_ONEBOX;
/*N*/ 	b3dCreationActive		 = FALSE;
/*N*/ 	pMirrorPolygon			 = 0;
/*N*/ 	pMirroredPolygon		 = 0;
/*N*/ 	nPolyCnt				 = 0;
/*N*/ 	pMyPV					 = 0;
/*N*/ 	pMarkedObjs 			 = 0;
/*N*/ 	fDefaultScaleX			 =
/*N*/ 	fDefaultScaleY			 =
/*N*/ 	fDefaultScaleZ			 = 1.0;
/*N*/ 	fDefaultRotateX 		 =
/*N*/ 	fDefaultRotateY 		 =
/*N*/ 	fDefaultRotateZ 		 = 0.0;
/*N*/ 	fDefaultExtrusionDeepth  = 1000; // old: 2000;
/*N*/ 	fDefaultLightIntensity	 = 0.8; // old: 0.6;
/*N*/ 	fDefaultAmbientIntensity = 0.4;
/*N*/     nHDefaultSegments        = 12;
/*N*/     nVDefaultSegments        = 12;
/*N*/     aDefaultLightColor       = RGB_Color(COL_WHITE);
/*N*/     aDefaultAmbientColor     = RGB_Color(COL_BLACK);
/*N*/ 	aDefaultLightPos		 = Vector3D (1, 1, 1); // old: Vector3D (0, 0, 1);
/*N*/ 	aDefaultLightPos.Normalize();
/*N*/     bDoubleSided             = FALSE;
/*N*/ }

/*************************************************************************
|*
|* Zeige eine Hilfsfigur
|*
\************************************************************************/


/*************************************************************************
|*
|* Verdecke eine Hilfsfigur
|*
\************************************************************************/


/*************************************************************************
|*
|* Zeige eine Hilfsfigur
|*
\************************************************************************/


/*************************************************************************
|*
|* Koennen die selektierten Objekte aufgebrochen werden?
|*
\************************************************************************/


/*************************************************************************
|*
|* Selektierte Lathe-Objekte aufbrechen
|*
\************************************************************************/



/*************************************************************************
|*
|* Szenen mischen
|*
\************************************************************************/
// Wird bisher noch nirgenswo (weder im Draw oder Chart) aufgerufen


/*************************************************************************
|*
|* Possibilities, hauptsaechlich gruppieren/ungruppieren
|*
\************************************************************************/
/*N*/ void E3dView::CheckPossibilities()
/*N*/ {
/*N*/ 	// call parent
/*N*/ 	SdrView::CheckPossibilities();
/*N*/ 
/*N*/ 	// Weitere Flags bewerten
/*N*/ 	if(bGroupPossible || bUnGroupPossible || bGrpEnterPossible)
/*N*/ 	{
/*?*/ 		INT32 nMarkCnt = aMark.GetMarkCount();
/*?*/ 		BOOL bCoumpound = FALSE;
/*?*/ 		BOOL b3DObject = FALSE;
/*?*/ 		for(INT32 nObjs = 0L; (nObjs < nMarkCnt) && !bCoumpound; nObjs++)
/*?*/ 		{
/*?*/ 			SdrObject *pObj = aMark.GetMark(nObjs)->GetObj();
/*?*/ 			if(pObj && pObj->ISA(E3dCompoundObject))
/*?*/ 				bCoumpound = TRUE;
/*?*/ 			if(pObj && pObj->ISA(E3dObject))
/*?*/ 				b3DObject = TRUE;
/*?*/ 		}
/*?*/ 
/*?*/ 		// Bisher: Es sind ZWEI oder mehr beliebiger Objekte selektiert.
/*?*/ 		// Nachsehen, ob CompoundObjects beteiligt sind. Falls ja,
/*?*/ 		// das Gruppieren verbieten.
/*?*/ 		if(bGroupPossible && bCoumpound)
/*?*/ 			bGroupPossible = FALSE;
/*?*/ 
/*?*/ 		if(bUnGroupPossible && b3DObject)
/*?*/ 			bUnGroupPossible = FALSE;
/*?*/ 
/*?*/ 		if(bGrpEnterPossible && bCoumpound)
/*?*/ 			bGrpEnterPossible = FALSE;
/*N*/ 	}

//	bGroupPossible
//	bCombinePossible
//	bUnGroupPossible
//	bGrpEnterPossible
/*N*/ }



}
