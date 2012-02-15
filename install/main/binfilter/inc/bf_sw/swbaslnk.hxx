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


#ifndef _SWBASLNK_HXX
#define _SWBASLNK_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _LNKBASE_HXX //autogen
#include <bf_so3/lnkbase.hxx>
#endif
namespace binfilter {

class SwNode;
class SwCntntNode;
class SwNodeIndex;

class SwBaseLink : public ::binfilter::SvBaseLink
{
	SwCntntNode* pCntntNode;
	BOOL bSwapIn : 1;
	BOOL bNoDataFlag : 1;
	BOOL bIgnoreDataChanged : 1;

public:
	TYPEINFO();

	SwBaseLink( USHORT nMode, USHORT nFormat, SwCntntNode* pNode = 0 )
		: ::binfilter::SvBaseLink( nMode, nFormat ), pCntntNode( pNode ),
		bSwapIn( FALSE ), bNoDataFlag( FALSE ), bIgnoreDataChanged( FALSE )
	{}
	virtual ~SwBaseLink();

	virtual void DataChanged( const String& rMimeType,
								const ::com::sun::star::uno::Any & rValue );



	SwCntntNode *GetCntntNode() { return pCntntNode; }

	// nur fuer Grafiken
	FASTBOOL SwapIn( BOOL bWaitForData = FALSE, BOOL bNativFormat = FALSE );

	FASTBOOL IsShowQuickDrawBmp() const;				// nur fuer Grafiken

	FASTBOOL Connect() { return 0 != SvBaseLink::GetRealObject(); }

	// nur fuer Grafik-Links ( zum Umschalten zwischen DDE / Grf-Link)
	void SetObjType( USHORT nType )	{ SvBaseLink::SetObjType( nType ); }

	BOOL IsRecursion( const SwBaseLink* pChkLnk ) const;
	virtual BOOL IsInRange( ULONG nSttNd, ULONG nEndNd, xub_StrLen nStt = 0,
							xub_StrLen nEnd = STRING_NOTFOUND ) const;

	void SetNoDataFlag()	{ bNoDataFlag = TRUE; }
	BOOL ChkNoDataFlag()	{ return bNoDataFlag ? !(bNoDataFlag = FALSE) : FALSE; }
	BOOL IsNoDataFlag()	const			{ return bNoDataFlag; }
};


} //namespace binfilter
#endif

