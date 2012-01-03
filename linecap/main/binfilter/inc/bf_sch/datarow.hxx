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



#ifndef _SCH_DATAROW_HXX
#define _SCH_DATAROW_HXX

#ifndef _SVDOBJ_HXX //autogen
#include <bf_svx/svdobj.hxx>
#endif
#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|* Datenreihen-Index
|*
\************************************************************************/

class SchDataRow : public SdrObjUserData
{
	short nRow;

public:
	SchDataRow();
	SchDataRow(short nR);

		virtual SdrObjUserData* Clone(SdrObject *pObj) const;

	virtual void WriteData(SvStream& rOut);
	virtual void ReadData(SvStream& rIn);

	void SetRow(short nR) { nRow = nR; }
	short GetRow() { return nRow; }
};

/*************************************************************************
|*
|* Tool-Funktion fuer Datenreihen-Index
|*
\************************************************************************/

extern SchDataRow* GetDataRow(const SdrObject& rObj);
extern SdrObject* GetObjWithRow(short nRow, const SdrObjList& rObjList,
								ULONG* pIndex = NULL);

} //namespace binfilter
#endif	// _SCH_DATAROW_HXX


