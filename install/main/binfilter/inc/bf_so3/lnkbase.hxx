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


#ifndef _LNKBASE_HXX
#define _LNKBASE_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SOT_EXCHANGE_HXX //autogen
#include <sot/exchange.hxx>
#endif
#ifndef _TOOLS_REF_HXX
#include <tools/ref.hxx>
#endif
#ifndef _LINKSOURCE_HXX
#include <bf_so3/linksrc.hxx>
#endif

#ifndef INCLUDED_SO3DLLAPI_H
#include "bf_so3/so3dllapi.h"
#endif

namespace com { namespace sun { namespace star { namespace uno
{
	class Any;
	//class Type;
}}}}

namespace binfilter
{

struct ImplBaseLinkData;
class SvLinkManager;
class SvLinkSource;

#ifndef OBJECT_DDE_EXTERN
#define	OBJECT_INTERN		0x00
//#define	OBJECT_SO_EXTERN	0x01
#define	OBJECT_DDE_EXTERN	0x02
#endif

#define	OBJECT_CLIENT_SO			0x80 // ein Link
#define	OBJECT_CLIENT_DDE			0x81
//#define	OBJECT_CLIENT_OLE			0x82 // ein Ole-Link
//#define	OBJECT_CLIENT_OLE_CACHE  	0x83 // ein Ole-Link mit SvEmbeddedObject
#define	OBJECT_CLIENT_FILE			0x90
#define	OBJECT_CLIENT_GRF			0x91

enum so3link {
	// Ole2 compatibel und persistent
	LINKUPDATE_ALWAYS = 1,
	LINKUPDATE_ONCALL = 3,

	LINKUPDATE_END		// dummy!
};

class SO3_DLLPUBLIC SvBaseLink : public SvRefBase
{
	friend class SvLinkManager;
	friend class SvLinkSource;
private:
	SvLinkSourceRef			xObj;
	String					aLinkName;
	SvLinkManager* 			pLinkMgr;
	USHORT 					nObjType;
	BOOL					bVisible : 1;
	BOOL					bSynchron : 1;
	BOOL					bUseCache : 1;		// fuer GrafikLinks!

protected:
	void			SetObjType( USHORT );

					// setzen des LinkSourceName ohne aktion
	SO3_DLLPRIVATE void			SetName( const String & rLn );

	ImplBaseLinkData* pImplData;

					SvBaseLink( USHORT nLinkType, ULONG nContentType = FORMAT_STRING );
	virtual 		~SvBaseLink();

	void 			_GetRealObject( BOOL bConnect = TRUE );

	SO3_DLLPRIVATE SvLinkSource*	GetRealObject()
					{
						if( !xObj.Is() )
							_GetRealObject();
						return xObj;
					}

public:
					TYPEINFO();
					// ask JP
	virtual void    Closed();

	USHORT			GetObjType() const { return nObjType; }

	SvLinkSource*	GetObj() const	{ return xObj; }

	void    		SetLinkSourceName( const String & rName );
	String		 	GetLinkSourceName() const;

	virtual void 	DataChanged( const String & rMimeType,
								const ::com::sun::star::uno::Any & rValue );

	void			SetUpdateMode( USHORT );
	USHORT 			GetUpdateMode() const;
	ULONG  			GetContentType() const;
	BOOL 			SetContentType( ULONG nType );

	SvLinkManager*	GetLinkManager()			{ return pLinkMgr; }
	const SvLinkManager* GetLinkManager() const	{ return pLinkMgr; }

	BOOL			Update();
	void			Disconnect();

	virtual BOOL 	Edit( Window* );

		// soll der Link im Dialog angezeigt werden ? (Links im Link im ...)
	BOOL 	        IsVisible() const   		{ return bVisible; }
	void 	        SetVisible( BOOL bFlag )	{ bVisible = bFlag; }
		// soll der Link synchron oder asynchron geladen werden?
	BOOL 	        IsSynchron() const   		{ return bSynchron; }
	void 	        SetSynchron( BOOL bFlag )	{ bSynchron = bFlag; }

	BOOL 	        IsUseCache() const   		{ return bUseCache; }
	void 			SetUseCache( BOOL bFlag )	{ bUseCache = bFlag; }
};

SV_DECL_IMPL_REF(SvBaseLink);

}

#endif
