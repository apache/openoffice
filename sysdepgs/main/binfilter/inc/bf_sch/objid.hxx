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



#ifndef _SCH_OBJID_HXX
#define _SCH_OBJID_HXX

#ifndef _SVDOBJ_HXX //autogen
#include <bf_svx/svdobj.hxx>
#endif
#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
#ifndef _SVDITER_HXX //autogen
#include <bf_svx/svditer.hxx>
#endif
namespace binfilter {


/*************************************************************************
|*
|* Id-Objekt fuer Chart-Grafik-Objekte
|*
\************************************************************************/

class SchObjectId : public SdrObjUserData
{
	UINT16 nObjId;	// Id-Wert

public:
	SchObjectId();
	SchObjectId(UINT16 nId);

		virtual SdrObjUserData* Clone(SdrObject *pObj) const;

	virtual void WriteData(SvStream& rOut);
	virtual void ReadData(SvStream& rIn);

		void SetObjId(UINT16 nId) {  nObjId = nId; }
	UINT16 GetObjId() { return nObjId; }
};

/*************************************************************************
|*
|* Tool-Funktionen fuer Objekt-Ids
|*
\************************************************************************/

extern SchObjectId* GetObjectId(const SdrObject& rObj);

extern SdrObject* GetObjWithId(UINT16 nObjId, const SdrObjList& rObjList,
														   ULONG* pIndex = NULL,
														   SdrIterMode eMode = IM_FLAT);

//Die hier folgenden Funktionen dienen AUSSCHLIESSLICH zu DEBUG-Zwecken
#ifdef DBG_UTIL
extern char *GetCHOBJIDName(const long id);
#endif

} //namespace binfilter
#endif	// _SCH_OBJID_HXX


