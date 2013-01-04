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



#ifndef _SWSERV_HXX
#define _SWSERV_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _LINKSRC_HXX
#include <bf_so3/linksrc.hxx>
#endif
namespace binfilter {

class SwBookmark;
class SwSectionNode;
class SwBaseLink;
class SwTableNode;
struct SwPosition;
class SwPaM;

class SwServerObject : public ::binfilter::SvLinkSource
{
protected:
	enum ServerModes { BOOKMARK_SERVER, TABLE_SERVER, SECTION_SERVER, NONE_SERVER } eType;
	union {
		SwBookmark* pBkmk;
		SwTableNode* pTblNd;
		SwSectionNode* pSectNd;
	} CNTNT_TYPE;

	SwServerObject();

public:
	SwServerObject( SwBookmark& rBookmark )
		: eType( BOOKMARK_SERVER )
	{
		CNTNT_TYPE.pBkmk = &rBookmark;
	}
	SwServerObject( SwTableNode& rTableNd )
		: eType( TABLE_SERVER )
	{
		CNTNT_TYPE.pTblNd = &rTableNd;
	}
	SwServerObject( SwSectionNode& rSectNd )
		: eType( SECTION_SERVER )
	{
		CNTNT_TYPE.pSectNd = &rSectNd;
	}
	virtual ~SwServerObject();




	BOOL IsLinkInServer( const SwBaseLink* ) const;

	void SetNoServer()
		{  CNTNT_TYPE.pBkmk = 0, eType = NONE_SERVER; }
};

#ifndef SW_DECL_SWSERVEROBJECT_DEFINED
#define SW_DECL_SWSERVEROBJECT_DEFINED
SV_DECL_REF( SwServerObject )
#endif

} //namespace binfilter
#endif	// _SWSERV_HXX

