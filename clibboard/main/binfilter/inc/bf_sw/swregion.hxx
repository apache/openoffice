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


#ifndef _SWREGION_HXX
#define _SWREGION_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif

#include "swrect.hxx"
namespace binfilter {

SV_DECL_VARARR( SwRects, SwRect, 20, 8 )//STRIP008 ;

class SwRegionRects : public SwRects
{
	SwRect aOrigin;	// die Kopie des StartRects

	inline void InsertRect( const SwRect &rRect, const USHORT nPos, FASTBOOL &rDel);

public:
	SwRegionRects( const SwRect& rStartRect, USHORT nInit = 20,
											 USHORT nGrow = 8 );
	// Zum Ausstanzen aus aOrigin.
	void operator-=( const SwRect& rRect );

	// Aus Loechern werden Flaechen, aus Flaechen werden Loecher.
	// Benachbarte Rechtecke zusammenfassen.

	inline const SwRect &GetOrigin() const { return aOrigin; }
	inline void ChangeOrigin( const SwRect &rRect ) { aOrigin = rRect; }
};

} //namespace binfilter
#endif	//_SWREGION_HXX
