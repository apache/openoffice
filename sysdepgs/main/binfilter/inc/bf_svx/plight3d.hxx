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



#ifndef _E3D_PLIGHT3D_HXX
#define _E3D_PLIGHT3D_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _E3D_LIGHT3D_HXX
#include "light3d.hxx"
#endif
namespace binfilter {

/*************************************************************************
|*
|* Punktlichtquelle, die gleichmaessig in alle Richtungen strahlt, und
|* zwar von der von E3dLight geerbten Position aus
|*
\************************************************************************/

class E3dPointLight : public E3dLight
{
 protected:
	virtual void CreateLightObj();

 public:
	TYPEINFO();
	E3dPointLight(const Vector3D& rPos,
				  const Color&    rColor,
				  double          fLightIntensity = 1.0);

	E3dPointLight()
	{
	}

	virtual ~E3dPointLight();


	virtual FASTBOOL CalcLighting(Color& rNewColor,
								  const Vector3D& rPnt,
								  const Vector3D& rPntNormal,
								  const Color& rPntColor);
};



}//end of namespace binfilter
#endif			// _E3D_PLIGHT3D_HXX
