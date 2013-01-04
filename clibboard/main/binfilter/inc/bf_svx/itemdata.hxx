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


#ifndef _SVX_ITEMDATA_HXX
#define _SVX_ITEMDATA_HXX

#ifndef _SHL_HXX //autogen
#include <tools/shl.hxx>
#endif
#ifndef _PSTM_HXX //autogen
#include <tools/pstm.hxx>
#endif
namespace binfilter {

class SvxGlobalItemData
{
private:
	SvClassManager*	pClassMgr;

public:
					SvxGlobalItemData();
					~SvxGlobalItemData() 	{ delete pClassMgr; }

	SvClassManager&	GetClassManager();
};

#define ITEMDATA() (*(SvxGlobalItemData**)GetAppData( BF_SHL_ITEM ) )

inline SvxGlobalItemData::SvxGlobalItemData()
{
	pClassMgr = 0;
	*(SvxGlobalItemData**)GetAppData(BF_SHL_ITEM) = this;
}

inline SvClassManager& SvxGlobalItemData::GetClassManager()
{
	if ( !pClassMgr )
		pClassMgr = new SvClassManager;
	return *pClassMgr;
}

}//end of namespace binfilter
#endif

