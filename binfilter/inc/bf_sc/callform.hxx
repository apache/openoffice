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



#ifndef SC_CALLFORM_HXX
#define SC_CALLFORM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef SC_COLLECT_HXX
#include "collect.hxx"
#endif
namespace binfilter {

//------------------------------------------------------------------------
#define MAXFUNCPARAM	16
#define MAXARRSIZE		0xfffe

//------------------------------------------------------------------------
#ifndef WIN
#ifndef WNT
#define CALLTYPE
#else
#define CALLTYPE			__cdecl
#endif
#else
#define PASCAL				_pascal
#define FAR					_far
#define CALLTYPE			FAR PASCAL
#endif

extern "C" {
typedef void (CALLTYPE* AdvData)( double& nHandle, void* pData );
}

//------------------------------------------------------------------------
enum ParamType
{
	PTR_DOUBLE,
	PTR_STRING,
	PTR_DOUBLE_ARR,
	PTR_STRING_ARR,
	PTR_CELL_ARR,
	NONE
};

//------------------------------------------------------------------------
class ModuleData;
class FuncData : public DataObject
{
friend class FuncCollection;
	const ModuleData* pModuleData;
	String		aInternalName;
	String		aFuncName;
	USHORT      nNumber;
	USHORT		nParamCount;
	ParamType	eAsyncType;
	ParamType	eParamType[MAXFUNCPARAM];
private:
	FuncData(const String& rIName);
public:
	FuncData(const FuncData& rData);
	virtual	DataObject*	Clone() const { return new FuncData(*this); }

	const	String&		GetInternalName() const { return aInternalName; }
	const	String&		GetFuncName() const { return aFuncName; }
			USHORT		GetParamCount() const { return nParamCount; }
			ParamType	GetParamType(USHORT nIndex) const { return eParamType[nIndex]; }
			ParamType	GetReturnType() const { return eParamType[0]; }
			ParamType	GetAsyncType() const { return eAsyncType; }
			BOOL        Call(void** ppParam);

						// Name und Beschreibung des Parameters nParam.
						// nParam==0 => Desc := Funktions-Beschreibung,
						// Name := n/a
};


//------------------------------------------------------------------------
class FuncCollection : public SortedCollection
{
public:
	FuncCollection(USHORT nLim = 4, USHORT nDel = 4, BOOL bDup = FALSE) : SortedCollection ( nLim, nDel, bDup ) {}
	FuncCollection(const FuncCollection& rFuncCollection) : SortedCollection ( rFuncCollection ) {}

	virtual	DataObject*	Clone() const { return new FuncCollection(*this); }
	virtual	short		Compare(DataObject* pKey1, DataObject* pKey2) const;
			BOOL 		SearchFunc( const String& rName, USHORT& rIndex ) const;
};


void ExitExternalFunc();

} //namespace binfilter
#endif
