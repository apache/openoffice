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



#ifndef _SWEVENT_HXX
#define _SWEVENT_HXX

#include <bf_svtools/bf_solar.h>

// #include *****************************************************************

#ifndef _SFX_HRC
#include <bf_sfx2/sfx.hrc>
#endif
namespace binfilter {

#define     SW_EVENT_OBJECT_SELECT        ( EVENT_APP_START + 0 )
#define     SW_EVENT_START_INS_GLOSSARY   ( EVENT_APP_START + 1 )
#define     SW_EVENT_END_INS_GLOSSARY     ( EVENT_APP_START + 2 )
#define     SW_EVENT_MAIL_MERGE           ( EVENT_APP_START + 3 )
#define     SW_EVENT_FRM_KEYINPUT_ALPHA   ( EVENT_APP_START + 4 )
#define     SW_EVENT_FRM_KEYINPUT_NOALPHA ( EVENT_APP_START + 5 )
#define     SW_EVENT_FRM_RESIZE           ( EVENT_APP_START + 6 )
#define     SW_EVENT_FRM_MOVE             ( EVENT_APP_START + 7 )
#define     SW_EVENT_PAGE_COUNT			  ( EVENT_APP_START + 8 )
#define     SW_EVENT_MAIL_MERGE_END		  ( EVENT_APP_START + 9 )


class SwFrmFmt;
class SwFmtINetFmt;
class IMapObject;

// enum fuer Objecte die Events ins Basic oder in JavaScript Callen
enum SwCallEventObjectType
{
	EVENT_OBJECT_NONE = 0,			// Null ist garnichts
	EVENT_OBJECT_IMAGE,
	EVENT_OBJECT_INETATTR,
	EVENT_OBJECT_URLITEM,
	EVENT_OBJECT_IMAGEMAP
};

// structur fuer den Austausch zwischen UI/CORE

struct SwCallMouseEvent
{
	SwCallEventObjectType eType;
	union
	{
		// EVENT_OBJECT_IMAGE/EVENT_OBJECT_URLITEM
		const SwFrmFmt* pFmt;

		// EVENT_OBJECT_INETATTR
		const SwFmtINetFmt* pINetAttr;

		// EVENT_OBJECT_IMAGEMAP
		struct
		{
			const SwFrmFmt* pFmt;
			const IMapObject* pIMapObj;
		} IMAP;
	} PTR;

	SwCallMouseEvent()
		: eType( EVENT_OBJECT_NONE )
		{ PTR.pFmt = 0; PTR.IMAP.pIMapObj = 0; }

	void Set( SwCallEventObjectType eTyp, const SwFrmFmt* pFmt )
		{ eType = eTyp; PTR.pFmt = pFmt; PTR.IMAP.pIMapObj = 0; }

	void Set( const SwFrmFmt* pFmt, const IMapObject* pIMapObj )
		{ eType = EVENT_OBJECT_IMAGEMAP; PTR.pFmt = pFmt; PTR.IMAP.pIMapObj = pIMapObj; }

	void Set( const SwFmtINetFmt* pINetAttr )
		{ eType = EVENT_OBJECT_INETATTR; PTR.pINetAttr = pINetAttr; PTR.IMAP.pIMapObj = 0; }

	int operator==( const SwCallMouseEvent& rEvent ) const
		{
			return eType == rEvent.eType &&
					PTR.pFmt == rEvent.PTR.pFmt &&
					PTR.IMAP.pIMapObj == rEvent.PTR.IMAP.pIMapObj;
		}
	int operator!=( const SwCallMouseEvent& rEvent ) const
		{	return !( *this == rEvent );	}

	void Clear()
		{ eType = EVENT_OBJECT_NONE; PTR.pFmt = 0; PTR.IMAP.pIMapObj = 0; }

	BOOL HasEvent() const { return EVENT_OBJECT_NONE != eType; }
};


} //namespace binfilter
#endif

