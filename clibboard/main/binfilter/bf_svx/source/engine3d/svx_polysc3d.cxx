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




#ifndef _XDEF_HXX //autogen
#include "xdef.hxx"
#endif












#ifndef _E3D_GLOBL3D_HXX
#include "globl3d.hxx"
#endif

#ifndef _E3D_POLYSC3D_HXX
#include "polysc3d.hxx"
#endif








namespace binfilter {

#define ITEMVALUE(ItemSet,Id,Cast)	((const Cast&)(ItemSet).Get(Id)).GetValue()

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*N*/ TYPEINIT1(E3dPolyScene, E3dScene);

/*N*/ E3dPolyScene::E3dPolyScene()
/*N*/ :	E3dScene()
/*N*/ {
/*N*/ }

/*************************************************************************
|*
|* Identifier zurueckgeben
|*
\************************************************************************/

/*N*/ UINT16 E3dPolyScene::GetObjIdentifier() const
/*N*/ {
/*N*/ 	return E3D_POLYSCENE_ID;
/*N*/ }

/*************************************************************************
|*
|* Die Kontur fuer TextToContour
|*
\************************************************************************/


/*************************************************************************
|*
|* Objekt als Kontur in das Polygon einfuegen
|*
\************************************************************************/


/*************************************************************************
|*
|* Paint
|*
\************************************************************************/

/*N*/ FASTBOOL E3dPolyScene::Paint(ExtOutputDevice& rOut,
/*N*/ 	const SdrPaintInfoRec& rInfoRec) const
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); return FALSE;//STRIP001 
/*N*/ }


/*************************************************************************
|*
|* Geometrie zeichnen mit clipping Beruecksichtigung
|*
\************************************************************************/


/*************************************************************************
|*
|* Zeichenroutine fuer 3D
|*
\************************************************************************/


/*************************************************************************
|*
|* Schatten aller Objekte zeichnen
|*
\************************************************************************/


/*************************************************************************
|*
|* Verminderte Darstellungsqualitaet, zeichne als WireFrame OHNE renderer
|*
\************************************************************************/



}
