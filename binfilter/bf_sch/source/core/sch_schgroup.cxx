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


#include "glob.hxx"
#include "schgroup.hxx"
#include <math.h>
namespace binfilter {

/*N*/ TYPEINIT1(SchObjGroup, SdrObjGroup);


/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

/*N*/ SchObjGroup::SchObjGroup(ChartModel *pChmodel) :
/*N*/ 	SdrObjGroup(),
/*N*/ 	bAskForLogicRect(TRUE),
/*N*/     mbUseChartInventor( true )
/*N*/ {
/*N*/ 	// FG: Damit soll es Objekten im chart ermoeglicht werden sich wie ein
/*N*/ 	//     Objekt im Draw zu verhalten falls gewünscht. Nicht alles was
/*N*/ 	//     prinzipiell geht soll man auch koennen.
/*N*/ 	aInfo.bResizeFreeAllowed    = TRUE;
/*N*/ 	aInfo.bResizePropAllowed    = TRUE;
/*N*/ 	aInfo.bRotateFreeAllowed    = TRUE;
/*N*/ 	aInfo.bRotate90Allowed      = TRUE;
/*N*/ 	aInfo.bMirrorFreeAllowed    = FALSE;
/*N*/ 	aInfo.bMirror45Allowed      = FALSE;
/*N*/ 	aInfo.bMirror90Allowed      = FALSE;
/*N*/ 	aInfo.bShearAllowed         = TRUE;
/*N*/ 
/*N*/ 	eChartGroupType = NOTHING;
/*N*/ 	pChartmodel = pChmodel;
/*N*/ 	SetModel( pChartmodel );
/*N*/ }

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/

/*N*/ SchObjGroup::~SchObjGroup()
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Identifier zurueckgeben
|*
\************************************************************************/

/*N*/ UINT32 SchObjGroup::GetObjInventor() const
/*N*/ {
/*N*/     if( mbUseChartInventor )
/*N*/         return SchInventor;
/*N*/     else
/*?*/         return SdrInventor;
/*N*/ }

/*************************************************************************
|*
|* Identifier zurueckgeben
|*
\************************************************************************/

/*N*/ UINT16 SchObjGroup::GetObjIdentifier() const
/*N*/ {
/*N*/ 	return SCH_OBJGROUP_ID;
/*N*/ }

/*************************************************************************
|*
|* Handle-Anzahl bestimmen
|*
\************************************************************************/


/*************************************************************************
|*
|* Handle erzeugen
|*
\************************************************************************/


/*************************************************************************
|*
|* Handle-Liste fuellen
|*
\************************************************************************/


/*************************************************************************
|*
|* Identifier zurueckgeben
|*
\************************************************************************/



/*************************************************************************
|*
|* Faehigkeiten der Chart-Gruppe feststellen
|*
\************************************************************************/


/*************************************************************************
|*
|* Faehigkeiten der Chart-Gruppe zuweisen
|*
\************************************************************************/

/*N*/ void	SchObjGroup::SetObjInfo(SdrObjTransformInfoRec aMyInfo)
/*N*/ {
/*N*/ 	// FG: Damit soll es Objekten im chart ermoeglicht werden sich wie ein
/*N*/ 	//     Objekt im Draw zu verhalten falls gewünscht.
/*N*/ 	aInfo.bResizeFreeAllowed    = aMyInfo.bResizeFreeAllowed;
/*N*/ 	aInfo.bResizePropAllowed    = aMyInfo.bResizePropAllowed;
/*N*/ 	aInfo.bRotateFreeAllowed    = aMyInfo.bRotateFreeAllowed;
/*N*/ 	aInfo.bRotate90Allowed      = aMyInfo.bRotate90Allowed;
/*N*/ 	aInfo.bMirrorFreeAllowed    = aMyInfo.bMirrorFreeAllowed;
/*N*/ 	aInfo.bMirror45Allowed      = aMyInfo.bMirror45Allowed;
/*N*/ 	aInfo.bMirror90Allowed      = aMyInfo.bMirror90Allowed;
/*N*/ 	aInfo.bShearAllowed         = aMyInfo.bShearAllowed;
/*N*/ }

/*************************************************************************
|*
|* Wirkliches Resize einer Chart-Gruppe (ohne Broadcast)
|* Bei manchen Chart-Gruppen ist eine Neuberechnung bei einem Resize
|* besser als ein hartes Resize.
|*
\************************************************************************/


/*************************************************************************
|*
|* Um das Verhalten bei einem Resize in Abhaehngigkeit des selektierten
|* Objektes zu ernoeglichen. Bei Diagramme werden nicht die Teilobjekte
|* Resized sondern das Chart in den neuen Grenzen aufgebaut.
|*
\************************************************************************/


/*************************************************************************
|*
|* Um mitzubekommen welche Objekte bewegt werden. Leider wird das
|* nicht nur vom Benutzer verursacht.
|*
\************************************************************************/

/*N*/ void SchObjGroup::Move(const Size& rSiz)
/*N*/ {
/*N*/ 
/*N*/ 	if (eChartGroupType == DIAGRAM)
/*N*/ 	{
/*?*/ 		DBG_ASSERT( pChartmodel, "No Model" );
/*?*/ 		if (pChartmodel != NULL)
/*?*/ 		{
/*?*/ 			Rectangle aRectChart = pChartmodel->GetChartRect();
/*?*/ 			Rectangle aRect = GetBoundRect();
/*?*/ 			Point aPointBottomLeftRectChart = aRectChart.BottomLeft();
/*?*/ 			Point aPointBottomLeftBoundRect = aRect.BottomLeft();
/*?*/ 			Point aPointTopRightRectChart = aRectChart.TopRight();
/*?*/ 			Point aPointTopRightBoundRect = aRect.TopRight();
/*?*/ 			Point aDifferenceLeftBottom = aPointBottomLeftRectChart - aPointBottomLeftBoundRect;
/*?*/ 			Point aDifferenceTopRight = aPointTopRightRectChart - aPointTopRightBoundRect;
/*?*/ 			aRect.Left() += rSiz.Width();
/*?*/ 			aRect.Right() += rSiz.Width();
/*?*/ 			aRect.Top() += rSiz.Height();
/*?*/ 			aRect.Bottom() += rSiz.Height();
/*?*/ 			bAskForLogicRect = FALSE;
/*?*/ 			aRect.Left() += aDifferenceLeftBottom.X();
/*?*/ 			aRect.Bottom() += aDifferenceLeftBottom.Y();
/*?*/ 			aRect.Right() += aDifferenceTopRight.X();
/*?*/ 			aRect.Top() += aDifferenceTopRight.Y();
/*?*/ 			pChartmodel->SetDiagramRectangle(aRect);
/*?*/ 		}
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		SdrObjGroup::Move(rSiz);
/*N*/ 	}
/*N*/ }

/*************************************************************************
|*
|* Um mitzubekommen welche Objekte bewegt werden. Leider wird das
|* nicht nur vom Benutzer verursacht.
|*
\************************************************************************/


/*N*/ void SchObjGroup::SetUseChartInventor( bool bUseChartInventor )
/*N*/ {
/*N*/     mbUseChartInventor = bUseChartInventor;
/*N*/ }

}
