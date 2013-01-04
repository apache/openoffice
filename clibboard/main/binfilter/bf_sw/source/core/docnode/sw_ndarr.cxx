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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _INDEX_HXX
#include <index.hxx>
#endif
#ifndef _NDARR_HXX
#include <ndarr.hxx>
#endif
namespace binfilter {


void SwNds::Insert(const SwNodePtr &aElement, const SwIndex & aPos)
{
	theArr.Insert((const ElementPtr&) aElement, aPos.GetIndex());
	SwIndexReg::Update(aPos, 1);
}

void SwNds::Insert(const SwNodePtr* pElement, USHORT nLen, const SwIndex & aPos)
{
	theArr.Insert((const ElementPtr*) pElement, nLen, aPos.GetIndex());
	SwIndexReg::Update(aPos, nLen);
}

void SwNds::Remove(const SwIndex & aPos, USHORT nLen)
{
	if(nLen)
	{
		theArr.Remove(aPos.GetIndex(), nLen);
		SwIndexReg::Update(aPos, nLen, TRUE);
	}
}

BOOL SwNds::Move( const SwIndex & rOldPos, const SwIndex & rNewPos )
{
	register USHORT nDelPos = rOldPos.GetIndex(),
					nInsPos = rNewPos.GetIndex();
	if( nDelPos == nInsPos || nDelPos +1 == nInsPos )
		return FALSE;
	theArr.Move( nDelPos, nInsPos );
	SwIndexReg::MoveIdx( rOldPos, rNewPos );
	return TRUE;
}

}
