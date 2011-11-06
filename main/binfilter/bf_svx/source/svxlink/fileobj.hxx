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


#ifndef _FILEOBJ_HXX
#define _FILEOBJ_HXX

#ifndef _LINKSRC_HXX //autogen
#include <bf_so3/linksrc.hxx>
#endif
#ifndef _SFXDOCFILE_HXX //autogen
#include <bf_sfx2/docfile.hxx>
#endif
class Graphic;
namespace binfilter {

struct Impl_DownLoadData;


class SvFileObject : public ::binfilter::SvLinkSource
{
	String sFileNm;
	String sFilter;
	String sReferer;
	SfxMediumRef xMed;
	Impl_DownLoadData* pDownLoadData;

	BYTE nType;

	BOOL bLoadAgain : 1;
	BOOL bSynchron : 1;
	BOOL bLoadError : 1;
	BOOL bWaitForData : 1;
	BOOL bInNewData : 1;
	BOOL bDataReady : 1;
	BOOL bMedUseCache : 1;
	BOOL bNativFormat : 1;
	BOOL bClearMedium : 1;
	BOOL bStateChangeCalled : 1;
	BOOL bInCallDownLoad : 1;

	BOOL GetGraphic_Impl( Graphic&, SvStream* pStream = 0 );
	BOOL LoadFile_Impl();
	void SendStateChg_Impl( USHORT nState );

	DECL_STATIC_LINK( SvFileObject, DelMedium_Impl, SfxMediumRef* );
	DECL_STATIC_LINK( SvFileObject, LoadGrfReady_Impl, void* );
	DECL_STATIC_LINK( SvFileObject, LoadGrfNewData_Impl, void* );
protected:
	virtual ~SvFileObject();

public:
	SvFileObject();

	virtual BOOL GetData( ::com::sun::star::uno::Any & rData /*out param*/,
							const String & rMimeType,
							BOOL bSynchron = FALSE );

	virtual BOOL Connect( ::binfilter::SvBaseLink* );

	// erfrage ob das man direkt auf die Daten zugreifen kann oder ob das
	// erst angestossen werden muss

	void CancelTransfers();
};


}//end of namespace binfilter
#endif

