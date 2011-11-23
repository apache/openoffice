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



#ifndef _E3D_E3DIOCMPT_HXX
#define _E3D_E3DIOCMPT_HXX


#ifndef _SVDIO_HXX
#include "svdio.hxx"
#endif
class SvStream;
namespace binfilter {


#define E3DIOCOMPAT_VERSIONDONTKNOW (UINT16) 0xffff

class E3dIOCompat : public SdrDownCompat
{
private:
	UINT16  nVersion;

public:
			// nNewMode: STREAM_READ oder STREAM_WRITE
			// nVer:     nur beim Schreiben angeben
			E3dIOCompat(SvStream& rNewStream, USHORT nNewMode,
						UINT16 nVer = E3DIOCOMPAT_VERSIONDONTKNOW);
			~E3dIOCompat() {};

	UINT16  GetVersion() const { return nVersion; }
};

}//end of namespace binfilter
#endif      // _E3D_E3DIOCMPT_HXX


