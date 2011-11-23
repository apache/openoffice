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



#ifndef _EERDLL_HXX
#define _EERDLL_HXX
namespace binfilter {
class GlobalEditData;
}//end of namespace binfilter
#ifndef _TOOLS_RESID_HXX //autogen
#include <tools/resid.hxx>
#endif

namespace binfilter {

class EditResId: public ResId
{
public:
	EditResId( USHORT nId );
};

class EditDLL
{
	ResMgr*			pResMgr;
	GlobalEditData*	pGlobalData;

public:
					EditDLL();
					~EditDLL();

	ResMgr*			GetResMgr() const 		{ return pResMgr; }
	GlobalEditData*	GetGlobalData() const	{ return pGlobalData; }
};

#define EE_DLL() (*(EditDLL**)GetAppData( BF_SHL_EDIT ) )

}//end of namespace binfilter
#endif //_EERDLL_HXX
