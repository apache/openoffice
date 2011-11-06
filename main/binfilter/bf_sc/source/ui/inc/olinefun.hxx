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



#ifndef SC_OLINEFUN_HXX
#define SC_OLINEFUN_HXX

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
namespace binfilter {

class ScDocShell;
class ScRange;


// ---------------------------------------------------------------------------

class ScOutlineDocFunc
{
private:
	ScDocShell&	rDocShell;

public:
				ScOutlineDocFunc( ScDocShell& rDocSh ): rDocShell(rDocSh) {}
				~ScOutlineDocFunc() {}

	BOOL		MakeOutline( const ScRange& rRange, BOOL bColumns, BOOL bRecord, BOOL bApi );
	BOOL		RemoveOutline( const ScRange& rRange, BOOL bColumns, BOOL bRecord, BOOL bApi );
	BOOL		RemoveAllOutlines( USHORT nTab, BOOL bRecord, BOOL bApi );
	BOOL		AutoOutline( const ScRange& rRange, BOOL bRecord, BOOL bApi );

	BOOL		SelectLevel( USHORT nTab, BOOL bColumns, USHORT nLevel,
									BOOL bRecord, BOOL bPaint, BOOL bApi );

	BOOL		ShowMarkedOutlines( const ScRange& rRange, BOOL bRecord, BOOL bApi );
	BOOL		HideMarkedOutlines( const ScRange& rRange, BOOL bRecord, BOOL bApi );
};



} //namespace binfilter
#endif

