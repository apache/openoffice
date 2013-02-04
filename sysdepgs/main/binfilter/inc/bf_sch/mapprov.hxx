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



#ifndef SCH_MAPPROV_HXX
#define SCH_MAPPROV_HXX

// header for SfxItemPropertyMap
#ifndef _SFX_ITEMPROP_HXX
#include <bf_svtools/itemprop.hxx>
#endif

namespace binfilter {

// ----------------------------------------------
// 					 Map-Id's
// ----------------------------------------------

#define CHMAP_NONE					 0
#define CHMAP_CHART					 1
#define CHMAP_AREA					 2
#define CHMAP_LINE					 3
#define CHMAP_DOC					 4
#define CHMAP_AXIS					 5
#define CHMAP_LEGEND				 6
#define CHMAP_TITLE					 7
#define CHMAP_DATAROW				 8
#define CHMAP_DATAPOINT				 9
#define CHMAP_END					10		// last one used + 1

class ChartModel;

// ----------------------------------------------
// 		Map-Provider ( to sort the maps )
// ----------------------------------------------

class SchUnoPropertyMapProvider
{
private:
	SfxItemPropertyMap*	pMapArr[ CHMAP_END ];
	void Sort( short nId );

public:
	SchUnoPropertyMapProvider();
	~SchUnoPropertyMapProvider();

	SfxItemPropertyMap* GetMap( short PropertyId, ChartModel* );
	SfxItemPropertyMap* CopyMap( const SfxItemPropertyMap* pMap1 );
};

} //namespace binfilter
#endif	// SCH_MAPPROV_HXX

