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



#ifndef _EERDLL2_HXX
#define _EERDLL2_HXX

#include <forbiddencharacterstable.hxx>
#include <vos/ref.hxx>
namespace binfilter {

class SfxPoolItem; 
class SvxAutoCorrect;

class GlobalEditData
{
private:
	SfxPoolItem** 	ppDefItems;
	OutputDevice*	pStdRefDevice;

	Link			aGetAutoCorrectHdl;

	vos::ORef<SvxForbiddenCharactersTable>	xForbiddenCharsTable;

public:
					GlobalEditData();
					~GlobalEditData();

	SfxPoolItem**	GetDefItems();
	OutputDevice*	GetStdRefDevice();

	void			SetGetAutoCorrectHdl( const Link& rHdl ) { aGetAutoCorrectHdl = rHdl; }
	SvxAutoCorrect*	GetAutoCorrect() const { return (SvxAutoCorrect*) aGetAutoCorrectHdl.Call( NULL ); }

	vos::ORef<SvxForbiddenCharactersTable>	GetForbiddenCharsTable();
	void			SetForbiddenCharsTable( vos::ORef<SvxForbiddenCharactersTable> xForbiddenChars ) { xForbiddenCharsTable = xForbiddenChars; }
};


}//end of namespace binfilter
#endif //_EERDLL2_HXX

