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



#ifndef _E3D_POLYSC3D_HXX
#define _E3D_POLYSC3D_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVDPAGE_HXX
#include <bf_svx/svdpage.hxx>
#endif

#ifndef _E3D_SCENE3D_HXX
#include <bf_svx/scene3d.hxx>
#endif

#ifndef _B3D_HMATRIX_HXX
#include <bf_goodies/hmatrix.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|* 3D-Szene mit Darstellung durch 2D-Polygone
|*
\************************************************************************/

class E3dPolyScene : public E3dScene
{
public:
	TYPEINFO();
	E3dPolyScene();

	virtual UINT16 GetObjIdentifier() const;

	// Zeichenmethode
	virtual FASTBOOL Paint(ExtOutputDevice&, const SdrPaintInfoRec&) const;

	// Die Kontur fuer TextToContour


protected:



};

}//end of namespace binfilter
#endif          // _E3D_POLYSC3D_HXX
