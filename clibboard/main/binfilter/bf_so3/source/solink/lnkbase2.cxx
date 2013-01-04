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




#include "bf_so3/lnkbase.hxx"
#include <sot/exchange.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <vcl/msgbox.hxx>

#include "bf_so3/linkmgr.hxx"
#include "svuidlg.hrc"
#include <bf_so3/iface.hxx>
#include <vcl/svapp.hxx>

#include <tools/debug.hxx>

#include <bf_svtools/svdde.hxx>

using namespace ::com::sun::star::uno;

namespace binfilter
{

TYPEINIT0( SvBaseLink )

// nur fuer die interne Verwaltung
struct ImplBaseLinkData
{
	struct tClientType
	{
		// gilt fuer alle Links
		ULONG				nCntntType; // Update Format
		// nicht Ole-Links
		BOOL 			bIntrnlLnk; // ist es ein interner Link
		USHORT 			nUpdateMode;// UpdateMode
	};

	tClientType ClientType;
	ImplBaseLinkData()
	{
		ClientType.nCntntType = 0;
		ClientType.bIntrnlLnk = FALSE;
		ClientType.nUpdateMode = 0;
	}
};


/************************************************************************
|*	  SvBaseLink::SvBaseLink()
|*
|*	  Beschreibung
*************************************************************************/

SvBaseLink::SvBaseLink( USHORT nUpdateMode, ULONG nContentType )
{
	nObjType = OBJECT_CLIENT_SO;
	pLinkMgr = 0;
	pImplData = new ImplBaseLinkData;
	bVisible = bSynchron = bUseCache = TRUE;

	// falls es ein Ole-Link wird,
	pImplData->ClientType.nUpdateMode = nUpdateMode;
	pImplData->ClientType.nCntntType = nContentType;
	pImplData->ClientType.bIntrnlLnk = FALSE;
}

/************************************************************************
|*	  SvBaseLink::~SvBaseLink()
|*
|*	  Beschreibung
*************************************************************************/

SvBaseLink::~SvBaseLink()
{
	Disconnect();

	delete pImplData;
}

/************************************************************************
|*	  SvBaseLink::SetObjType()
|*
|*	  Beschreibung
*************************************************************************/

void SvBaseLink::SetObjType( USHORT nObjTypeP )
{
	DBG_ASSERT( nObjType != OBJECT_CLIENT_DDE, "type already set" );
	DBG_ASSERT( !xObj.Is(), "object exist" );

	nObjType = nObjTypeP;
}

/************************************************************************
|*	  SvBaseLink::SetName()
|*
|*	  Beschreibung
*************************************************************************/

void SvBaseLink::SetName( const String & rNm )
{
	aLinkName = rNm;
}

/************************************************************************
|*	  SvBaseLink::SetLinkSourceName()
|*
|*	  Beschreibung
*************************************************************************/

void SvBaseLink::SetLinkSourceName( const String & rLnkNm )
{
	if( aLinkName == rLnkNm )
		return;

	AddNextRef(); // sollte ueberfluessig sein
	// Alte Verbindung weg
	Disconnect();

	aLinkName = rLnkNm;

	// Neu verbinden
	_GetRealObject();
	ReleaseRef(); // sollte ueberfluessig sein
}

/************************************************************************
|*	  SvBaseLink::GetLinkSourceName()
|*
|*	  Beschreibung
*************************************************************************/

String  SvBaseLink::GetLinkSourceName() const
{
	return aLinkName;
}


/************************************************************************
|*	  SvBaseLink::SetUpdateMode()
|*
|*	  Beschreibung
*************************************************************************/

void SvBaseLink::SetUpdateMode( USHORT nMode )
{
	if( ( OBJECT_CLIENT_SO & nObjType ) &&
		pImplData->ClientType.nUpdateMode != nMode )
	{
		AddNextRef();
		Disconnect();

		pImplData->ClientType.nUpdateMode = nMode;
		_GetRealObject();
		ReleaseRef();
	}
}


BOOL SvBaseLink::Update()
{
	if( OBJECT_CLIENT_SO & nObjType )
	{
		AddNextRef();
		Disconnect();

		_GetRealObject();
		ReleaseRef();
		if( xObj.Is() )
		{
			String sMimeType( SotExchange::GetFormatMimeType(
							pImplData->ClientType.nCntntType ));
			Any aData;

			if( xObj->GetData( aData, sMimeType ) )
			{
				DataChanged( sMimeType, aData );
				//JP 13.07.00: Bug 76817 - for manual Updates there is no
				//				need to hold the ServerObject
				if( OBJECT_CLIENT_DDE == nObjType &&
					LINKUPDATE_ONCALL == GetUpdateMode() && xObj.Is() )
					xObj->RemoveAllDataAdvise( this );
				return TRUE;
			}
			if( xObj.Is() )
			{
				// sollten wir asynschron sein?
				if( xObj->IsPending() )
					return TRUE;

				// dann brauchen wir das Object auch nicht mehr
				AddNextRef();
				Disconnect();
				ReleaseRef();
			}
		}
	}
	return FALSE;
}


USHORT SvBaseLink::GetUpdateMode() const
{
	return ( OBJECT_CLIENT_SO & nObjType )
			? pImplData->ClientType.nUpdateMode
			: (USHORT)LINKUPDATE_ONCALL;
}


void SvBaseLink::_GetRealObject( BOOL bConnect )
{
	if( !pLinkMgr )
		return;

	DBG_ASSERT( !xObj.Is(), "object already exist" );

	if( OBJECT_CLIENT_DDE == nObjType )
	{
		String sServer;
		if( pLinkMgr->GetDisplayNames( this, &sServer ) &&
			sServer == GetpApp()->GetAppName() )		// interner Link !!!
		{
			// damit der Internal - Link erzeugt werden kann !!!
			nObjType = OBJECT_INTERN;
			xObj = pLinkMgr->CreateObj( this );

			pImplData->ClientType.bIntrnlLnk = TRUE;
			nObjType = OBJECT_CLIENT_DDE;		// damit wir wissen was es mal war !!
		}
		else
		{
			pImplData->ClientType.bIntrnlLnk = FALSE;
			xObj = pLinkMgr->CreateObj( this );
		}
	}
	else if( (OBJECT_CLIENT_SO & nObjType) )
		xObj = pLinkMgr->CreateObj( this );

	if( bConnect && ( !xObj.Is() || !xObj->Connect( this ) ) )
		Disconnect();
}

ULONG SvBaseLink::GetContentType() const
{
	if( OBJECT_CLIENT_SO & nObjType )
		return pImplData->ClientType.nCntntType;

	return 0;		// alle Formate ?
}


BOOL SvBaseLink::SetContentType( ULONG nType )
{
	if( OBJECT_CLIENT_SO & nObjType )
	{
		pImplData->ClientType.nCntntType = nType;
		return TRUE;
	}
	return FALSE;
}


void SvBaseLink::Disconnect()
{
	if( xObj.Is() )
	{
		xObj->RemoveAllDataAdvise( this );
		xObj->RemoveConnectAdvise( this );
		xObj.Clear();
	}
}


void SvBaseLink::DataChanged( const String &, const ::com::sun::star::uno::Any & )
{
}


BOOL SvBaseLink::Edit( Window* pParent )
{
	BOOL bConnect = xObj.Is();
	if( !bConnect )
		_GetRealObject( xObj.Is() );

	String aNewNm;

	if( OBJECT_CLIENT_SO & nObjType &&
		pImplData->ClientType.bIntrnlLnk )
	{
		if( pLinkMgr )
		{
			SvLinkSourceRef ref = pLinkMgr->CreateObj( this );
			if( ref.Is() )
				aNewNm = ref->Edit( pParent, this );
		}
	}
	else
		aNewNm = xObj->Edit( pParent, this );

	if( aNewNm.Len() != 0 )
	{
		SetLinkSourceName( aNewNm );
		if( !Update() )
		{
			String sApp, sTopic, sItem, sError;
			pLinkMgr->GetDisplayNames( this, &sApp, &sTopic, &sItem );
			if( nObjType == OBJECT_CLIENT_DDE )
			{
				sError = String( ResId( STR_ERROR_DDE, *SOAPP->GetResMgr()));
				USHORT nFndPos = sError.Search( '%' );
				if( STRING_NOTFOUND != nFndPos )
				{
					sError.Erase( nFndPos, 1 ).Insert( sApp, nFndPos );
					nFndPos = nFndPos + sApp.Len();
				}
				if( STRING_NOTFOUND != ( nFndPos = sError.Search( '%', nFndPos )))
				{
					sError.Erase( nFndPos, 1 ).Insert( sTopic, nFndPos );
					nFndPos = nFndPos + sTopic.Len();
				}
				if( STRING_NOTFOUND != ( nFndPos = sError.Search( '%', nFndPos )))
					sError.Erase( nFndPos, 1 ).Insert( sItem, nFndPos );
			}
			else
				return FALSE;

			ErrorBox( pParent, WB_OK, sError ).Execute();
		}
	}
	else if( !bConnect )
		Disconnect();
	return aNewNm.Len() != 0;
}


void SvBaseLink::Closed()
{
	if( xObj.Is() )
		// beim Advise Abmelden
		xObj->RemoveAllDataAdvise( this );
}

}
