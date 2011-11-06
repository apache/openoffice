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



#ifndef _SVDITER_HXX
#define _SVDITER_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _LIST_HXX
#include <tools/list.hxx>
#endif
namespace binfilter {

class SdrObjList;
class SdrObject;

// SdrObjListIter methods:
// IM_FLAT				: Flach ueber die Liste
// IM_DEEPWITHGROUPS	: Mit rekursivem Abstieg, Next() liefert auch Gruppenobjekte
// IM_DEEPNOGROUPS		: Mit rekursivem Abstieg, Next() liefert keine Gruppenobjekte
enum SdrIterMode { IM_FLAT, IM_DEEPWITHGROUPS, IM_DEEPNOGROUPS};

class SdrObjListIter
{
	List						maObjList;
	sal_uInt32					mnIndex;
	BOOL						mbReverse;

	void ImpProcessObjectList(const SdrObjList& rObjList, SdrIterMode eMode);

public:
	SdrObjListIter(const SdrObjList& rObjList, SdrIterMode eMode = IM_DEEPNOGROUPS, BOOL bReverse = FALSE);
	SdrObjListIter(const SdrObject& rGroup, SdrIterMode eMode = IM_DEEPNOGROUPS, BOOL bReverse = FALSE);

	void Reset() { mnIndex = (mbReverse ? maObjList.Count() : 0L); }
	BOOL IsMore() const { return (mbReverse ? mnIndex != 0 : ( mnIndex < maObjList.Count())); }
	SdrObject* Next() { return (SdrObject*)maObjList.GetObject(mbReverse ? --mnIndex : mnIndex++); }
};

}//end of namespace binfilter
#endif //_SVDITER_HXX

