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



//#include <string.h>

#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "app.hxx"
#include "objface.hxx"
#include "sfxsids.hrc"
#include "docinf.hxx"
#include "docfile.hxx"
#include "openflag.hxx"
#include "dinfobj.hxx"
#include "fcontnr.hxx"

#ifndef _SBXCLASS_HXX //autogen
#include "bf_basic/sbx.hxx"
#endif
//========================================================================

#define DocumentInfo
namespace binfilter {
#include "sfxslots.hxx"

#define aTypeLibInfo aDocumentInfoTypeLibImpl

//=========================================================================

SfxDocumentInfoObject_Impl::SfxDocumentInfoObject_Impl()
:	SvDispatch( DEFINE_CONST_UNICODE( "DocumentInfo" ) ),
	pMedium(0)
{
	// Sbx-Flags
	SetType( SbxOBJECT );
	SetFlag( SBX_FIXED );
	SetFlag( SBX_DONTSTORE );
	ResetFlag( SBX_WRITE );
}

//--------------------------------------------------------------------

SfxDocumentInfoObject_Impl::~SfxDocumentInfoObject_Impl()
{
	delete pMedium;
}

//--------------------------------------------------------------------

SvGlobalName SfxDocumentInfoObject_Impl::GetTypeName() const
{
	SvGlobalName aName;
	aName.MakeId( String::CreateFromAscii( aTypeLibInfo.pShellUId ) );
	return aName;
}

//--------------------------------------------------------------------

BOOL SfxDocumentInfoObject_Impl::FillTypeLibInfo( String * pName,
									  USHORT * pMajor, USHORT * pMinor ) const
{
	*pName	= String::CreateFromAscii( aTypeLibInfo.pTypeLibName );
	*pMajor = aTypeLibInfo.nMajorVer;
	*pMinor = aTypeLibInfo.nMinorVer;
	return TRUE;
}

//------------------------------------------------------------------------

void SfxDocumentInfoObject_Impl::Notify( SfxBroadcaster& rBC,
										 const SfxHint& rHint )

/*	[Beschreibung]

	Diese Klasse ist nicht von SfxShellObject abgeleitet, da keine
	SfxShell die Verarbeitung "ubernimmt. Das Ansprechen der Properties
	und Methoden arbeitet daher auch via Notify().
*/

{
	// vor Selbstzerst"orung sch"utzen
	DBG_ASSERT( GetRefCount() != SV_NO_DELETE_REFCOUNT,
				"notify on unreferenced object => dying!!!" )
	SbxObjectRef xSelfRef( this );

	const SbxHint* pHint = PTR_CAST(SbxHint,&rHint);
	if( pHint )
	{
		// Art des Hint und SID auswerten
		ULONG nHint = pHint ? pHint->GetId() : 0;
		SbxVariable* pVar = pHint->GetVar();
		BOOL bRead = BOOL( nHint == SBX_HINT_DATAWANTED &&
						   pVar->GetClass() == SbxCLASS_PROPERTY );
		BOOL bWrite = BOOL( nHint == SBX_HINT_DATACHANGED &&
							pVar->GetClass() == SbxCLASS_PROPERTY );
		BOOL bCall = BOOL( nHint == SBX_HINT_DATAWANTED &&
						   pVar->GetClass() == SbxCLASS_METHOD );
		INT16 nSID = (INT16) ( pVar->GetUserData() & 0xFFFF );

		// Methode Load?
		if ( bCall && ( SID_DOCINFO_LOAD == nSID ) )
		{
			// ggf. altes Medium freigeben
			if ( pMedium )
				DELETEZ(pMedium);

			// 1. Parameter ist Dateiname
			SbxArray *pArgs = pVar->GetParameters();
			if ( !pArgs )
			{
				// Parameter fehlt
				pVar->PutBool( FALSE );
				return;
			}

			// Medium erstellen
			String aName( pArgs->Get(1)->GetString() );
			pMedium = new SfxMedium( aName, SFX_STREAM_READWRITE, TRUE );
            if ( !pMedium->GetStorage() )
			{
				// Datei existiert nicht oder ist kein Storage
				pVar->PutBool( FALSE );
				return;
			}

			// Storage "offnen
			SvStorageRef xStor = pMedium->GetStorage();
			if ( !xStor.Is() || SVSTREAM_OK != pMedium->GetError() )
			{
				// Fallback auf r/o
				delete pMedium;
				pMedium = new SfxMedium( aName, SFX_STREAM_READONLY, TRUE );
				xStor = pMedium->GetStorage();

				// konnte jetzt ge"offnet werden?
				if ( !xStor.Is() || SVSTREAM_OK != pMedium->GetError() )
				{
					pVar->PutBool( FALSE );
					return;
				}
			}

			// Filter-Detection wegen FileFormat-Version
			const SfxFilter *pFilter = 0;
			if ( 0 != SFX_APP()->GetFilterMatcher().GuessFilter( *pMedium, &pFilter ) ||
				 !pFilter )
			{
				// unbekannted Dateiformat
				pVar->PutBool( FALSE );
				return;
			}

			xStor->SetVersion( pFilter->GetVersion() );

			// DocInfo laden
			BOOL bOK = aDocInfo().Load( xStor );
			pVar->PutBool( bOK );
			if ( bOK )
				aDocInfo.SetValue( aName );
			return;
		}

		// Methode Save?
		else if ( bCall && ( SID_DOCINFO_SAVE == nSID ) )
		{
			// existiert ein Medium?
			if ( !pMedium )
			{
				// ohne Medium kein Speichern
				pVar->PutBool( FALSE );
				return;
			}

			// ggf. Filter-Detection wegen FileFormat-Version
			const SfxFilter *pFilter = 0;
			SFX_APP()->GetFilterMatcher().GuessFilter( *pMedium, &pFilter );

			// Storage "offnen
			SvStorageRef xStor = pMedium->GetStorage();
			xStor->SetVersion( pFilter
								? pFilter->GetVersion()
                                : SOFFICE_FILEFORMAT_CURRENT );
			// DocInfo speichern
			BOOL bOK = aDocInfo().Save( xStor ) && xStor->Commit();
			pVar->PutBool( bOK );
			return;
		}

		// Property auslesen?
		else if ( bRead && ( SID_DOCINFO == nSID ) )
		{
			aDocInfo.FillVariable( *pVar, SFX_MAPUNIT_MM, SFX_MAPUNIT_MM );
			return;
		}

		// Property setzen?
		else if ( bWrite && SID_DOCINFO == nSID )
		{
			aDocInfo.SetVariable( *pVar, SFX_MAPUNIT_MM, SFX_MAPUNIT_MM );
			return;
		}
	}

	SvDispatch::SFX_NOTIFY( rBC, rBCType, rHint, rHintType );
}


}
