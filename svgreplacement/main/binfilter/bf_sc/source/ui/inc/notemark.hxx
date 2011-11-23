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



#ifndef SC_NOTEMARK_HXX
#define SC_NOTEMARK_HXX

#ifndef _MAPMOD_HXX //autogen
#include <vcl/mapmod.hxx>
#endif
#ifndef _TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
namespace binfilter {

class ScNoteMarker
{
public:
	ScNoteMarker( Window* pWin,							//STRIP001 				ScNoteMarker( Window* pWin,
		Window* pRight, Window* pBottom, Window* pDiagonal,//STRIP001 								Window* pRight, Window* pBottom, Window* pDiagonal,
		ScDocument* pD, ScAddress aPos,						//STRIP001 								ScDocument* pD, ScAddress aPos,
		const String& rUser, const MapMode& rMap,			//STRIP001 								const String& rUser, const MapMode& rMap,
		BOOL bLeftEdge, BOOL bForce, BOOL bKeyboard ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 								BOOL bLeftEdge, BOOL bForce, BOOL bKeyboard );
};



} //namespace binfilter
#endif

