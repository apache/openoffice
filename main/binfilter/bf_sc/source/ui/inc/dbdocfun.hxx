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



#ifndef SC_DBDOCFUN_HXX
#define SC_DBDOCFUN_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

class String;
namespace binfilter {

struct ScImportParam;
struct ScQueryParam;
struct ScSortParam;
struct ScSubTotalParam;

class SfxViewFrame;
class SbaSelectionList;
class ScDBData;
class ScDocShell;
class ScAddress;
class ScRange;
class ScDPObject;
} //namespace binfilter
namespace com { namespace sun { namespace star {
    namespace beans {
	    struct PropertyValue;
    }
    namespace sdbc {
        class XResultSet;
    }
} } }
namespace binfilter {//STRIP009
// ---------------------------------------------------------------------------

class ScDBDocFunc
{
friend class ScDBFunc;

private:
	ScDocShell&		rDocShell;

public:
					ScDBDocFunc( ScDocShell& rDocSh ): rDocShell(rDocSh) {}
					~ScDBDocFunc() {}

    BOOL			DoImport( USHORT nTab, const ScImportParam& rParam,
                        const ::com::sun::star::uno::Reference<
                        ::com::sun::star::sdbc::XResultSet >& xResultSet,
                        const SbaSelectionList* pSelection, BOOL bRecord,
                        BOOL bAddrInsert = FALSE );

	BOOL			Sort( USHORT nTab, const ScSortParam& rSortParam,
							BOOL bRecord, BOOL bPaint, BOOL bApi );

	BOOL			Query( USHORT nTab, const ScQueryParam& rQueryParam,
							const ScRange* pAdvSource, BOOL bRecord, BOOL bApi );

	BOOL			DoSubTotals( USHORT nTab, const ScSubTotalParam& rParam,
									const ScSortParam* pForceNewSort,
									BOOL bRecord, BOOL bApi );

	BOOL			AddDBRange( const String& rName, const ScRange& rRange, BOOL bApi );
	BOOL			DeleteDBRange( const String& rName, BOOL bApi );
	BOOL			RenameDBRange( const String& rOld, const String& rNew, BOOL bApi );
	BOOL			ModifyDBData( const ScDBData& rNewData, BOOL bApi );	// Name unveraendert

	BOOL			RepeatDB( const String& rDBName, BOOL bRecord, BOOL bApi );

	BOOL			DataPilotUpdate( ScDPObject* pOldObj, const ScDPObject* pNewObj, 
										BOOL bRecord, BOOL bApi );
};



} //namespace binfilter
#endif
