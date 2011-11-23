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


#ifndef _SFX_MAP_HXX
#define _SFX_MAP_HXX

#include <bf_svtools/bf_solar.h>

#include <tools.hxx>
#include <sfxitems.hxx>
namespace binfilter {

class SfxInterface;
class SfxDispatcher;
class SfxShell;

class SfxMap: public SfxWhichMap
{
	const SfxInterface	*pInterface;
	const SfxDispatcher *pDispatcher;
	BOOL				bRecursive;
public:
	inline				SfxMap();
	inline              SfxMap(const SfxDispatcher &rDispatcher );
						SfxMap(const SfxShell &, BOOL bRecursiveSearch = FALSE );
	inline				SfxMap( const SfxMap & rMap);

	virtual USHORT		GetWhich(USHORT nSlot) const;
	virtual USHORT		GetSlot(USHORT nWhich) const;
	virtual SfxWhichMap*Clone() const;
};

//--------------------------------------------------------------------

inline SfxMap::SfxMap():
		SfxWhichMap( FALSE ),
		pInterface( 0 ),
		pDispatcher( 0 ),
		bRecursive( FALSE )
{
}

inline SfxMap::SfxMap( const SfxMap & rMap):
		SfxWhichMap( TRUE ),
		pInterface( rMap.pInterface ),
		pDispatcher( rMap.pDispatcher ),
		bRecursive( rMap.bRecursive )
{
}

inline SfxMap::SfxMap(const SfxDispatcher &rDispatcher ):
		SfxWhichMap( TRUE ),
		pInterface( 0 ),
		pDispatcher( &rDispatcher ),
		bRecursive( 0 )
{
}

}//end of namespace binfilter
#endif

