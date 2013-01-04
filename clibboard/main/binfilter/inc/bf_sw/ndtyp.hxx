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



#ifndef _NDTYP_HXX
#define _NDTYP_HXX

#include <bf_svtools/bf_solar.h>
namespace binfilter {

// Ids fuer die verschiedenden Nodes; in der Basisklasse steht der Member,
// der angibt, um was fuer einen es sich handelt
const BYTE ND_ENDNODE		= 0x01;
const BYTE ND_STARTNODE		= 0x02;
const BYTE ND_TABLENODE		= 0x06;
const BYTE ND_TEXTNODE 		= 0x08;
const BYTE ND_GRFNODE 		= 0x10;
const BYTE ND_OLENODE 		= 0x20;

const BYTE ND_CONTENTNODE	= 0x38;	// ContentNode (eines von den 3 Bits)
const BYTE ND_NOTXTNODE		= 0x30;	// NoTxtNode (eines von den 2 Bits)

const BYTE ND_SECTIONNODE   = 0x42;
// nur fuer internen Gebrauch!!
const BYTE ND_SECTIONDUMMY  = 0x40; //(ND_SECTIONNODE & ~ND_STARTNODE);

// spezielle Types der StartNodes, die keine Ableitungen sind, aber
// "Bereiche" zusammenhalten.
enum SwStartNodeType
{
	SwNormalStartNode = 0,
	SwTableBoxStartNode,
	SwFlyStartNode,
	SwFootnoteStartNode,
	SwHeaderStartNode,
	SwFooterStartNode
};

// is the node the first and/or last node of a section?
// This information is used for the export filters. Our layout never have a
// distance before or after if the node is the first or last in a section.
const BYTE ND_HAS_PREV_LAYNODE = 0x01;
const BYTE ND_HAS_NEXT_LAYNODE = 0x02;



} //namespace binfilter
#endif
