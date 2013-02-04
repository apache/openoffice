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


#ifndef _SFX_WHMAP_HXX
#define _SFX_WHMAP_HXX

#include <solar.h>

namespace binfilter
{

struct SfxWhichMapEntry
{
	USHORT nWhichId;
	USHORT nSlotId;
};

class SfxWhichMap
{
	const SfxWhichMapEntry *pMap;

public:
	inline				SfxWhichMap( const SfxWhichMapEntry *pMappings = 0 );
	virtual             ~SfxWhichMap();

	int 				IsNull() const
						{ return 0 == pMap; }
	virtual USHORT		GetWhich(USHORT nSlot) const;
	virtual USHORT		GetSlot(USHORT nWhich) const;
	virtual SfxWhichMap*Clone() const;
};

inline SfxWhichMap::SfxWhichMap( const SfxWhichMapEntry *pMappings ):
	pMap( pMappings )
{
}

}

#endif
