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



#ifndef _SCH_STLSHEET_HXX
#define _SCH_STLSHEET_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXSTYLE_HXX //autogen
#include <bf_svtools/style.hxx>
#endif
namespace binfilter {




class SchStyleSheet : public SfxStyleSheet
{
protected:
	virtual void Load (SvStream& rIn, USHORT nVersion);
	virtual void Store(SvStream& rOut);

public:
	TYPEINFO();
	SchStyleSheet(const String& rName, SfxStyleSheetBasePool& rPool,
				  SfxStyleFamily eFamily, USHORT nMask);
	virtual ~SchStyleSheet();

	virtual BOOL SetParent (const String& rParentName);
	virtual SfxItemSet& GetItemSet();
};

} //namespace binfilter
#endif	   // _SCH_STLSHEET_HXX

