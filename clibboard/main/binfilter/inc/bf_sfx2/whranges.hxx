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


#ifndef _SFX_WHRANGES_HXX
#define _SFX_WHRANGES_HXX

#include <limits.h>
#include <stdarg.h>
#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif
namespace binfilter {

DBG_NAMEEX(SfxWhichRanges)//STRIP008

// -----------------------------------------------------------------------

class SfxWhichRanges
{
	SvUShorts		aUShorts;

private:
	void			InitRanges(va_list pArgs, USHORT nWhich);

public:
					SfxWhichRanges();
					SfxWhichRanges( const SfxWhichRanges &rRanges );
					SfxWhichRanges( const USHORT *pIniRanges );
					SfxWhichRanges( USHORT nFrom, USHORT nTo );
					SfxWhichRanges( USHORT nFrom, USHORT nTo, USHORT nEnd, ... );
					~SfxWhichRanges();

	SfxWhichRanges& operator= ( const SfxWhichRanges &rRanges );

	SfxWhichRanges& operator+=( const SfxWhichRanges &rRanges );
	SfxWhichRanges& operator+=( USHORT nWhich );
	SfxWhichRanges& operator+=( const USHORT *pAddRanges );

	const USHORT*	GetRanges() const { return aUShorts.GetStart(); }
};

}//end of namespace binfilter
#endif

