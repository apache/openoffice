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



#ifndef _SCH_STLPOOL_HXX
#define _SCH_STLPOOL_HXX

#ifndef _SFXSTYLE_HXX //autogen
#include <bf_svtools/style.hxx>
#endif
namespace binfilter {


class SchStyleSheet;


class SchStyleSheetPool : public SfxStyleSheetPool
{
private:
	SfxStyleSheetBase* pActualStyleSheet;

protected:
	virtual SfxStyleSheetBase* Create(const String& rName,
									  SfxStyleFamily eFamily,
									  USHORT nMask);

public:
						SchStyleSheetPool(SfxItemPool& rPool);
	virtual 			~SchStyleSheetPool();

	void				SetActualStyleSheet(SfxStyleSheetBase* pActStyleSheet)
									   { pActualStyleSheet = pActStyleSheet; }
	SfxStyleSheetBase*	GetActualStyleSheet()	 { return pActualStyleSheet; }
};

} //namespace binfilter
#endif	   // _SCH_STLPOOL_HXX



