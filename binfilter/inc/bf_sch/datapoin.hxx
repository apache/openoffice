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



#ifndef _SCH_DATAPOIN_HXX
#define _SCH_DATAPOIN_HXX

#ifndef _SVDOBJ_HXX //autogen
#include <bf_svx/svdobj.hxx>
#endif
#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|* Datenpunkt
|*
\************************************************************************/

class SchDataPoint : public SdrObjUserData
{
	short nCol;
	short nRow;

public:
	SchDataPoint();
	SchDataPoint(short nC, short nR);

		virtual SdrObjUserData* Clone(SdrObject *pObj) const;

	virtual void WriteData(SvStream& rOut);
	virtual void ReadData(SvStream& rIn);

	void SetCol(short nC) { nCol = nC; }
	void SetRow(short nR) { nRow = nR; }
	short GetCol() { return nCol; }
	short GetRow() { return nRow; }
};

/*************************************************************************
|*
|* Tool-Funktion fuer Datenpunkt
|*
\************************************************************************/

extern SchDataPoint* GetDataPoint(const SdrObject& rObj);
extern SdrObject* GetObjWithColRow(short nCol, short nRow,
								   const SdrObjList& rObjList,
								   ULONG* pIndex = NULL);

} //namespace binfilter
#endif	// _SCH_DATAPOIN_HXX


