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


#ifndef _WRT_FN_HXX
#define _WRT_FN_HXX

#include <bf_svtools/bf_solar.h>

#include "hintids.hxx"		// fuer die Konstanten

namespace binfilter {

class SfxPoolItem;
class SfxItemSet; 

// einige Forward-Deklarationen
class SwFmt;
class SwNode;
class SwCntntNode;
class Writer;


/* Funktionspointer auf die Attribut-Write-Funktionen */
typedef Writer& (*FnAttrOut)( Writer&, const SfxPoolItem& );
typedef FnAttrOut SwAttrFnTab[ POOLATTR_END - POOLATTR_BEGIN ];

Writer& Out( const SwAttrFnTab, const SfxPoolItem&, Writer& );
Writer& Out_SfxItemSet( const SwAttrFnTab, Writer&, const SfxItemSet&,
						 BOOL bDeep, BOOL bTstForDefault = TRUE );


/* Funktionspointer auf die Node-Write-Funktionen */

enum RES_NODE
{
RES_NODE_BEGIN = 0,
	RES_TXTNODE = RES_NODE_BEGIN,
	RES_GRFNODE,
	RES_OLENODE,
RES_NODE_END
};

typedef Writer& (*FnNodeOut)( Writer&, SwCntntNode& );
typedef FnNodeOut SwNodeFnTab[ RES_NODE_END - RES_NODE_BEGIN ];

Writer& Out( const SwNodeFnTab, SwNode&, Writer & rWrt );




} //namespace binfilter
#endif	//	_WRT_FN_HXX
