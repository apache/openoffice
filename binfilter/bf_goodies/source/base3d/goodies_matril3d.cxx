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



#ifndef _B3D_MATRIL3D_HXX
#include "matril3d.hxx"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

namespace binfilter {

/*************************************************************************
|*
|* Konstruktor B3dMaterial
|*
\************************************************************************/

B3dMaterial::B3dMaterial()
:	aAmbient(COL_BLACK),			// kein lokales Umgebungslicht
	aDiffuse(0x00, 0xb8, 0xff),		// Blau7
	aSpecular(COL_WHITE),			// Weisser Glanzpunkt
	aEmission(COL_BLACK),			// Keine Selbstleuchtfarbe
	nExponent(15)					// Glanzpunktbuendelung
{
}

void B3dMaterial::WriteData(SvStream& rOut) const
{
	rOut << aAmbient;
	rOut << aDiffuse;
	rOut << aSpecular;
	rOut << aEmission;
	rOut << nExponent;
}

void B3dMaterial::ReadData(SvStream& rIn)
{
	rIn >> aAmbient;
	rIn >> aDiffuse;
	rIn >> aSpecular;
	rIn >> aEmission;
	rIn >> nExponent;
}

/*************************************************************************
|*
|* Vergleichsoperator
|*
\************************************************************************/

BOOL B3dMaterial::operator==(const B3dMaterial& rMat)
{
	 if(aAmbient == rMat.aAmbient
		 && aDiffuse == rMat.aDiffuse
		 && aSpecular == rMat.aSpecular
		 && aEmission == rMat.aEmission
		 && nExponent == rMat.nExponent)
		 return TRUE;
	 return FALSE;
}

}//end of namespace binfilter

// eof
