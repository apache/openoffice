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



#ifndef SC_UNDODAT_HXX
#define SC_UNDODAT_HXX

#ifndef SC_UNDOBASE_HXX
#include "undobase.hxx"
#endif

#ifndef SC_RANGEUTL_HXX
#include "rangeutl.hxx"		// ScArea
#endif

#ifndef SC_RANGELST_HXX
#include "rangelst.hxx"		// ScRangeListRef
#endif

#ifndef SC_MARKDATA_HXX
#include "markdata.hxx"
#endif

#ifndef SC_SORTPARAM_HXX
#include "sortparam.hxx"
#endif
namespace binfilter {


class ScDocShell;
class ScDocument;
class ScOutlineTable;
class ScRangeName;
class ScDBData;
class ScDBCollection;
class ScPivot;
class ScDPObject;

//----------------------------------------------------------------------------



















class ScUndoDBData: public ScSimpleUndo
{
public:
					ScUndoDBData( ScDocShell* pNewDocShell,
							ScDBCollection* pNewUndoColl, ScDBCollection* pNewRedoColl );
	virtual 		~ScUndoDBData();

	virtual BOOL	CanRepeat(SfxRepeatTarget& rTarget) const;

	virtual String	GetComment() const;

private:
	ScDBCollection* pUndoColl;
	ScDBCollection* pRedoColl;
};












class ScUndoChartData: public ScSimpleUndo
{
public:
						ScUndoChartData( ScDocShell* pNewDocShell,
								const String& rName, const ScRange& rNew,
								BOOL bColHdr, BOOL bRowHdr, BOOL bAdd );
						ScUndoChartData( ScDocShell* pNewDocShell,
								const String& rName, const ScRangeListRef& rNew,
								BOOL bColHdr, BOOL bRowHdr, BOOL bAdd );
	virtual 			~ScUndoChartData();



private:
	String				aChartName;
	ScRangeListRef		aOldRangeListRef;
	BOOL				bOldColHeaders;
	BOOL				bOldRowHeaders;
//	ScRange				aNewRange;
	ScRangeListRef		aNewRangeListRef;
	BOOL				bNewColHeaders;
	BOOL				bNewRowHeaders;
	BOOL				bAddRange;

};




} //namespace binfilter
#endif

