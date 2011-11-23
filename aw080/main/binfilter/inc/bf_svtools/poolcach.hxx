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


#ifndef _SFXPOOLCACH_HXX
#define _SFXPOOLCACH_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#include <bf_svtools/bf_solar.h>

namespace binfilter
{

class SfxItemModifyArr_Impl;
class SfxItemPool;
class SfxItemSet;
class SfxPoolItem;
class SfxSetItem;

class  SfxItemPoolCache
{
	SfxItemPool 			*pPool;
	SfxItemModifyArr_Impl	*pCache;
	const SfxItemSet		*pSetToPut;
	const SfxPoolItem		*pItemToPut;

public:
							SfxItemPoolCache( SfxItemPool *pPool,
											  const SfxItemSet *pPutSet );
							~SfxItemPoolCache();

	const SfxSetItem&		ApplyTo( const SfxSetItem& rSetItem, BOOL bNew = FALSE );
};

}

#endif

