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


#ifndef _SFX_WHITER_HXX
#define _SFX_WHITER_HXX

#ifndef INCLUDED_SVLDLLAPI_H

#endif

#ifndef INCLUDED_LIMITS_H
#include <limits.h>
#define INCLUDED_LIMITS_H
#endif

#include <bf_svtools/bf_solar.h>

namespace binfilter
{

class SfxItemSet;

// INCLUDE ---------------------------------------------------------------

class  SfxWhichIter
{
	const USHORT *pRanges, *pStart;
	USHORT nOfst, nFrom, nTo;

public:
	SfxWhichIter( const SfxItemSet& rSet, USHORT nFrom = 0, USHORT nTo = USHRT_MAX );
	~SfxWhichIter();

	USHORT  GetCurWhich() const { return *pRanges + nOfst; }
	USHORT  NextWhich();

	USHORT	FirstWhich();
};

}

#endif
