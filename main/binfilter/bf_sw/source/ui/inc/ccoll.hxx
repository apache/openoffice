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


#ifndef _CCOLL_HXX
#define _CCOLL_HXX

#include <bf_svtools/poolitem.hxx>
#include <tools/string.hxx>
#include <tools/resary.hxx>
#include "cmdid.h"

namespace binfilter {

class SwWrtShell;
class SwFmt;
class SwCollCondition;


struct CollName
{
	ULONG nCnd;
	ULONG nSubCond;
};

#define COND_COMMAND_COUNT 28

struct CommandStruct
{
	ULONG nCnd;
	ULONG nSubCond;
};

class SwCondCollItem : public SfxPoolItem
{
	static CommandStruct		aCmds[COND_COMMAND_COUNT];

	String 						sStyles[COND_COMMAND_COUNT];

public:
	SwCondCollItem(USHORT nWhich = FN_COND_COLL);
	~SwCondCollItem();

								TYPEINFO();

	virtual SfxPoolItem*    	Clone( SfxItemPool *pPool = 0 ) const;
	virtual int             	operator==( const SfxPoolItem& ) const;

	static const CommandStruct*	GetCmds();

	const String&				GetStyle(USHORT nPos) const;
	void						SetStyle( const String* pStyle, USHORT nPos);

};

} //namespace binfilter
#endif

