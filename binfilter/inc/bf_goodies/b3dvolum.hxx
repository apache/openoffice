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



#ifndef _B3D_VOLUM_HXX
#define _B3D_VOLUM_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SVX_VECTOR3D_HXX
#include "vector3d.hxx"
#endif

#ifndef _INC_FLOAT
#include <float.h>
#endif

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

/*************************************************************************
|*
|* dreidimensionales Volumen, symmetrisch zu den Koordinatenachsen
|*
\************************************************************************/

namespace binfilter {
class Vol3DPointIterator;

class B3dVolume
{
// friend class binfilter::Vol3DPointIterator; //STRIP008 
 friend class Vol3DPointIterator;
 protected:
	Vector3D aMinVec;
	Vector3D aMaxVec;

 public:
	B3dVolume(const Vector3D& rPos, const Vector3D& r3DSize,
		BOOL bPosIsCenter = TRUE);
	B3dVolume();

	void Reset();
	BOOL IsValid() const;

	B3dVolume& Union(const B3dVolume& rVol2);
	B3dVolume& Union(const Vector3D& rVec);

	const Vector3D& MinVec() const { return aMinVec; }
	Vector3D& MinVec() { return aMinVec; }

	const Vector3D& MaxVec() const { return aMaxVec; }
	Vector3D& MaxVec() { return aMaxVec; }

	double GetWidth()  const { return aMaxVec.X() - aMinVec.X(); }
	double GetHeight() const { return aMaxVec.Y() - aMinVec.Y(); }
	double GetDepth()  const { return aMaxVec.Z() - aMinVec.Z(); }

	friend SvStream& operator>>(SvStream& rIStream, B3dVolume&);
	friend SvStream& operator<<(SvStream& rOStream, const B3dVolume&);
};
}//end of namespace binfilter


#endif			// _B3D_VOLUM_HXX
