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



#ifndef _E3D_DLIGHT3D_HXX
#define _E3D_DLIGHT3D_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _E3D_LIGHT3D_HXX
#include <bf_svx/light3d.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|* gerichtete Lichtquelle, aDirection zeigt ZUR Lichtquelle (also den
|* Lichtstrahlen entgegensetzt); die Position (geerbt von E3dLight) hat
|* auf die Lichtberechnung keinen Einfluss, sondern dient nur der
|* Anordnung in der Szene (fuer evtl. spaetere interaktive Bearbeitung)
|*
\************************************************************************/

class E3dDistantLight : public E3dLight
{
 protected:
	Vector3D aDirection;

	virtual void CreateLightObj();

 public:
	TYPEINFO();
	E3dDistantLight(const Vector3D& rPos,
					const Vector3D& rDirection,
					const Color&    rColor,
					double          fLightIntensity = 1.0);

	E3dDistantLight() :
		aDirection(Vector3D(0, 1, 0))
	{
	}

	virtual ~E3dDistantLight();

	virtual UINT16 GetObjIdentifier() const;

	virtual FASTBOOL CalcLighting(Color& rNewColor,
								  const Vector3D& rPnt,
								  const Vector3D& rPntNormal,
								  const Color& rPntColor);

	const Vector3D& GetDirection() const { return aDirection; }
	void SetDirection(const Vector3D& rNewDir);

	virtual void WriteData(SvStream& rOut) const;
	virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);

};

}//end of namespace binfilter
#endif			// _E3D_DLIGHT3D_HXX
