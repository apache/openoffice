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


#ifndef _CONTENT_HXX
#define _CONTENT_HXX

#include <bf_svtools/bf_solar.h>

#include <limits.h>		// USHRT_MAX
#ifndef _SWCONT_HXX
#include "swcont.hxx"
#endif

#ifndef _MENU_HXX //autogen
#include <vcl/menu.hxx>
#endif
namespace binfilter {

class SwWrtShell;
class SwContentArr;
class SwContentType;
class SwNavigationPI;
class SwFmtFld;
class SwTxtINetFmt;
class SwDocShell;
class SvDataObject;
class SwTOXBase;

//----------------------------------------------------------------------------
//	Hilfsklassen
//----------------------------------------------------------------------------

class SwPopup : public PopupMenu
{
	USHORT			nCurId;
	virtual void    Select(){nCurId = GetCurItemId();}

public:
	SwPopup() :
		PopupMenu(),
		nCurId(USHRT_MAX){}

	USHORT			GetCurId() const { return nCurId; }

};

//----------------------------------------------------------------------------

class SwOutlineContent : public SwContent
{
	USHORT	nOutlinePos;
	BYTE	nOutlineLevel;
	BOOL	bIsMoveable;
	public:
		SwOutlineContent(	const SwContentType* pCnt,
							const String& rName,
							USHORT nArrPos,
							BYTE nLevel,
							BOOL bMove,
							long nYPos) :
			SwContent(pCnt, rName, nYPos), nOutlineLevel(nLevel),
						nOutlinePos(nArrPos), bIsMoveable(bMove){}

	USHORT 	GetPos(){return nOutlinePos;}
	BYTE 	GetOutlineLevel(){return nOutlineLevel;}
	BOOL	IsMoveable(){return bIsMoveable;};
};

//----------------------------------------------------------------------------
class SwRegionContent : public SwContent
{

	BYTE	nRegionLevel;

	public:
		SwRegionContent(	const SwContentType* pCnt,
							const String& rName,
							BYTE nLevel,
							long nYPos) :
			SwContent(pCnt, rName, nYPos),
						nRegionLevel(nLevel){}
	BYTE 	GetRegionLevel() const {return nRegionLevel;}
};
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------



//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/*
	class ContentType enthaelt Informationen zu einer Inhaltsform
	Das MemberArray wird nur gefuellt, wenn der Inhalt mit GetMember
	angefordert wird. Es wird nach Invalidate() nur dann neu geladen,
	wenn der Inhalt erneut ausgelesen werden soll.
*/
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------






} //namespace binfilter
#endif

