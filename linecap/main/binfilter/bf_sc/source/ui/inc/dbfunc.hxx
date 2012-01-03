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



#ifndef SC_DBFUNC_HXX
#define SC_DBFUNC_HXX

#include "viewfunc.hxx"
namespace binfilter {

struct ScSortParam;
struct ScQueryParam;
class ScDBData;
class ScDBCollection;
class ScDPObject;
class ScDPSaveData;

// ---------------------------------------------------------------------------

class ScDBFunc : public ScViewFunc
{
public:
					ScDBFunc( Window* pParent, ScDocShell& rDocSh, ScTabViewShell* pViewShell );
 					ScDBFunc( Window* pParent, const ScDBFunc& rDBFunc, ScTabViewShell* pViewShell );
					~ScDBFunc();

					//	nur UISort wiederholt bei Bedarf die Teilergebnisse







					// DB-Bereich vom Cursor
	ScDBData* 		GetDBData( BOOL bMarkArea = TRUE, ScGetDBMode eMode = SC_DB_MAKE );




	void			TestRemoveOutline( BOOL& rCol, BOOL& rRow );




	void			UpdateCharts(BOOL bAllCharts = FALSE);		// Default: am Cursor
};



} //namespace binfilter
#endif

