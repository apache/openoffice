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



#ifndef _SVDIO_HXX
#include "svdio.hxx"
#endif


#ifndef _E3D_PNTOBJ3D_HXX
#include "pntobj3d.hxx"
#endif
namespace binfilter {

/*N*/ TYPEINIT1(E3dPointObj, E3dObject);

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

/*N*/ E3dPointObj::E3dPointObj(const Vector3D& rPos) :
/*N*/ 	aPosition(rPos),
/*N*/ 	bTransPosValid(FALSE)
/*N*/ {
/*N*/ 	aLocalBoundVol = Volume3D(aPosition, Vector3D(), FALSE);
/*N*/ 	bBoundVolValid = FALSE;
/*N*/ }

/*************************************************************************
|*
|* Leer-Konstruktor
|*
\************************************************************************/

/*N*/ E3dPointObj::E3dPointObj() :
/*N*/ 	aPosition(Vector3D()),
/*N*/ 	bTransPosValid(FALSE)
/*N*/ {
/*N*/ 	aLocalBoundVol = Volume3D(aPosition, Vector3D(), FALSE);
/*N*/ 	bBoundVolValid = FALSE;
/*N*/ }

/*************************************************************************
|*
|* bei Aenderung der Transformation
|*
\************************************************************************/

/*N*/ void E3dPointObj::SetTransformChanged()
/*N*/ {
/*N*/ 	E3dObject::SetTransformChanged();
/*N*/ 	bTransPosValid = FALSE;
/*N*/ }

/*************************************************************************
|*
|* Identifier zurueckgeben
|*
\************************************************************************/


/*************************************************************************
|*
|* neue Position setzen und Aenderung an Parents (Szene) melden
|*
\************************************************************************/

/*N*/ void E3dPointObj::SetPosition(const Vector3D& rNewPos)
/*N*/ {
/*N*/ 	if ( aPosition != rNewPos )
/*N*/ 	{
/*N*/ 		aPosition = rNewPos;
/*N*/ 		bBoundVolValid = FALSE;
/*N*/ 		bTransPosValid = FALSE;
/*N*/ 		StructureChanged(this);
/*N*/ 	}
/*N*/ }

/*************************************************************************
|*
|* Transformierte Position zurueckgeben
|*
\************************************************************************/

/*N*/ const Vector3D& E3dPointObj::GetTransPosition()
/*N*/ {
/*N*/ 	if ( !bTransPosValid )
/*N*/ 	{
/*N*/ 		aTransPos = GetFullTransform() * aPosition;
/*N*/ 		bTransPosValid = TRUE;
/*N*/ 	}
/*N*/ 	return aTransPos;
/*N*/ }

/*************************************************************************
|*
|* sichern
|*
\************************************************************************/

/*N*/ void E3dPointObj::WriteData(SvStream& rOut) const
/*N*/ {
/*N*/ #ifndef SVX_LIGHT
/*N*/ 	E3dObject::WriteData(rOut);
/*N*/ 
/*N*/ #ifdef E3D_STREAMING
/*N*/ 
/*N*/ 	SdrDownCompat aCompat(rOut, STREAM_WRITE);
/*N*/ #ifdef DBG_UTIL
/*N*/ 	aCompat.SetID("E3dPointObj");
/*N*/ #endif
/*N*/ 
/*N*/ 	rOut << aPosition;
/*N*/ #endif
/*N*/ #endif	// #ifndef SVX_LIGHT
/*N*/ }

/*************************************************************************
|*
|* laden
|*
\************************************************************************/

/*N*/ void E3dPointObj::ReadData(const SdrObjIOHeader& rHead, SvStream& rIn)
/*N*/ {
/*N*/ 	if (ImpCheckSubRecords (rHead, rIn))
/*N*/ 	{
/*N*/ 		E3dObject::ReadData(rHead, rIn);
/*N*/ 
/*N*/ 		SdrDownCompat aCompat(rIn, STREAM_READ);
/*N*/ #ifdef DBG_UTIL
/*N*/ 		aCompat.SetID("E3dPointObj");
/*N*/ #endif
/*N*/ 
/*N*/ 		rIn >> aPosition;
/*N*/ 
/*N*/ 		bTransPosValid = FALSE;
/*N*/ 	}
/*N*/ }

/*************************************************************************
|*
|* Zuweisungsoperator
|*
\************************************************************************/




}
