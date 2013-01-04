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



#ifndef _SFXENUMITEM_HXX
#define _SFXENUMITEM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _SVTOOLS_CENUMITM_HXX
#include <bf_svtools/cenumitm.hxx>
#endif

namespace binfilter
{

//============================================================================
class  SfxEnumItem: public CntEnumItem
{
protected:
	SfxEnumItem(USHORT which = 0, USHORT nValue = 0):
		CntEnumItem(which, nValue) {}

	SfxEnumItem(USHORT which, SvStream & rStream):
		CntEnumItem(which, rStream) {}

public:
	TYPEINFO();

};

//============================================================================
class  SfxBoolItem: public CntBoolItem
{
public:
	TYPEINFO();

	SfxBoolItem(USHORT which = 0, BOOL bValue = FALSE):
		CntBoolItem(which, bValue) {}

	SfxBoolItem(USHORT which, SvStream & rStream):
		CntBoolItem(which, rStream) {}

	virtual SfxPoolItem * Create(SvStream & rStream, USHORT) const
	{ return new SfxBoolItem(Which(), rStream); }

	virtual SfxPoolItem * Clone(SfxItemPool * = 0) const
	{ return new SfxBoolItem(*this); }
};

}

#endif //  _SFXENUMITEM_HXX

