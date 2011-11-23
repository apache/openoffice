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



#ifndef _E3D_PNTOBJ3D_HXX
#define _E3D_PNTOBJ3D_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _E3D_OBJ3D_HXX
#include <bf_svx/obj3d.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|* Klasse fuer einzelne dreidimensionale Punkte
|*
\************************************************************************/

class E3dPointObj : public E3dObject
{
 protected:
	Vector3D	aPosition;
	Vector3D	aTransPos;

	FASTBOOL	bTransPosValid	: 1;

	virtual void SetTransformChanged();

 public:
	TYPEINFO();
	E3dPointObj(const Vector3D& rPos);
	E3dPointObj();


	virtual void SetPosition(const Vector3D& rNewPos);
	const Vector3D& GetPosition() const { return aPosition; }
	const Vector3D& GetTransPosition();

	virtual void WriteData(SvStream& rOut) const;
	virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);

};

}//end of namespace binfilter
#endif			// _E3D_PNTOBJ3D_HXX
