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


#ifndef _SW3MARKS_HXX
#define _SW3MARKS_HXX

#include <bf_svtools/bf_solar.h>
#ifndef _CNTNRSRT_HXX
#include <bf_svtools/cntnrsrt.hxx>
#endif
namespace binfilter {

enum Sw3MarkType {
	SW3_TOX_POINT, SW3_TOX_MARK,
	SW3_BOOK_POINT, SW3_BOOK_MARK,
	SW3_REDLINE_START, SW3_REDLINE_END,
	SW3_MARKTYPE_END
};

class Sw3Mark
{
	friend int sw3mark_compare( const Sw3Mark& r1, const Sw3Mark& r2 );

	Sw3MarkType	eType;					// Art des Eintrags
	ULONG		nNodePos;				// Node-Index
	xub_StrLen	nNodeOff;				// Position-Index
	USHORT		nId;					// ID des Eintrags

public:

	Sw3Mark() : eType(SW3_TOX_POINT), nNodePos(0),nNodeOff(0), nId(0) {}
	Sw3Mark( Sw3Mark& r ) : eType(r.eType), nNodePos(r.nNodePos),
							nNodeOff(r.nNodeOff), nId(r.nId) {}

	Sw3MarkType	GetType() const 	{ return  eType; }
	ULONG		GetNodePos() const 	{ return  nNodePos; }
	xub_StrLen	GetNodeOff() const 	{ return  nNodeOff; }
	USHORT		GetId() const 		{ return  nId; }

	void SetType( Sw3MarkType nSet ){ eType = nSet; }
	void SetNodePos( ULONG nSet ) 	{ nNodePos = nSet; }
	void SetNodeOff( xub_StrLen nSet ) 	{ nNodeOff = nSet; }
	void SetId( USHORT nSet ) 		{ nId = nSet; }
};

DECLARE_CONTAINER_SORT_DEL( Sw3Marks, Sw3Mark )

} //namespace binfilter
#endif
