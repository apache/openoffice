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



#ifndef _XPOOL_HXX
#define _XPOOL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXITEMPOOL_HXX //autogen
#include <bf_svtools/itempool.hxx>
#endif
#ifndef _XDEF_HXX
#include <bf_svx/xdef.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|* Klassendeklaration
|*
\************************************************************************/

class XOutdevItemPool: public SfxItemPool
{
protected:
	SfxPoolItem**   ppPoolDefaults;
	SfxItemInfo*    pItemInfos;
/*    USHORT*         pVersion1Map;
	USHORT*         pVersion2Map;
	USHORT*         pVersion3Map;
	USHORT*         pVersion4Map;*/
	USHORT          nStart, nEnd;

private:
	void Ctor(SfxItemPool* pMaster, USHORT nAttrStart, USHORT nAttrEnd);

public:
	// Kontstruktion des Pools als MasterPool
	XOutdevItemPool(USHORT nAttrStart = XATTR_START,
					USHORT nAttrEnd = XATTR_END,
					FASTBOOL bLoadRefCounts = TRUE);

	// Damit meine SetItems mit dem MasterPool konstruiert werden koennen.
	// Der SdrItemPool wird dabei automatisch als Secondary an den
	// uebergebenen MasterPool bzw. an's Ende einer bestehenden Pool-Kette
	// angehaengt.
	XOutdevItemPool(SfxItemPool* pMaster,
					USHORT nAttrStart = XATTR_START,
					USHORT nAttrEnd = XATTR_END,
					FASTBOOL bLoadRefCounts = TRUE);

	XOutdevItemPool(const XOutdevItemPool& rPool);

	virtual SfxItemPool* Clone() const;

	~XOutdevItemPool();
};

}//end of namespace binfilter
#endif      // _XPOOL_HXX
