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



// MARKER(update_precomp.py): autogen include statement, do not remove




#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#include "inetdef.hxx"
#include "inidef.hxx"

namespace binfilter
{

//========================================================================

class SfxStdIniDef_Impl: public SfxIniDefaulter
{
public:
					SfxStdIniDef_Impl( SfxIniDefaultManager *pDefMgr )
					:	SfxIniDefaulter( pDefMgr )
					{}
	virtual BOOL 	QueryDefault( String &rValue, const SfxIniEntry &rEntry );
};

//-------------------------------------------------------------------------

BOOL SfxStdIniDef_Impl::QueryDefault( String &rValue, const SfxIniEntry &rEntry )
{
	switch ( rEntry.GetKey() )
	{
		case SFX_KEY_BROWSERRESTORE:
		{
			rValue = "1";
			return TRUE;
		}

		case SFX_KEY_INET_HOME:
		{
			if ( System::GetLanguage() == LANGUAGE_GERMAN )
				rValue = "http://www.stardivision.de";
			else
				rValue = "http://www.stardivision.com";
			return TRUE;
		}

		case SFX_KEY_INET_MEMCACHE:
			rValue = "4";
			return TRUE;

		case SFX_KEY_INET_DISKCACHE:
			rValue = "2048";
			return TRUE;

		case SFX_KEY_INET_CACHEEXPIRATION:
			rValue = "3";
			return TRUE;

		case SFX_KEY_INET_MAXHTTPCONS:
			rValue = "4";
			return TRUE;

		case SFX_KEY_INET_MAXFTPCONS:
			rValue = "2";
			return TRUE;

//  		case SFX_KEY_INET_JAVAMINHEAP:
//  			rValue = "256";
//  			return TRUE;

//  		case SFX_KEY_INET_JAVAMAXHEAP:
//  			rValue = "";
//  			return TRUE;

		case SFX_KEY_INET_USERAGENT:
			rValue = INET_DEF_CALLERNAME;
			return TRUE;

		case SFX_KEY_INET_EXE_JAVASCRIPT:
#ifdef SOLAR_JAVA
			rValue = "0"; // noch "0", solange es noch soviel Bugs gibt
#else
			rValue = "0"; // immer "0"
#endif
			return TRUE;

		case SFX_KEY_INET_EXE_PLUGIN:
			rValue = "1";
			return TRUE;

/*		case SFX_KEY_INET_JAVA_ENABLE:
#ifdef SOLAR_JAVA
			rValue = "1";
#else
			rValue = "0";
#endif
			return TRUE; */

//  		case SFX_KEY_INET_NETACCESS:
//  			rValue = "2";
//  			return TRUE;

		case SFX_KEY_INET_CHANNELS:
			rValue = "1";
			return TRUE;

		case SFX_KEY_BASIC_ENABLE:
			rValue = "1";
			return TRUE;

		case SFX_KEY_INET_COOKIES:
			rValue = "1";
			return TRUE;

		case SFX_KEY_ICONGRID:
			rValue = "100;70;0";
			return TRUE;

		case SFX_KEY_METAFILEPRINT:
			rValue = "1";
			return TRUE;
	}

	return SfxIniDefaulter::QueryDefault( rValue, rEntry );
}

//=========================================================================

SfxIniDefaultManager::SfxIniDefaultManager()
:	_pList( new SfxIniDefaulterList )
{
	new SfxStdIniDef_Impl( this );
}

//-------------------------------------------------------------------------

SfxIniDefaultManager::~SfxIniDefaultManager()
{
	if ( _pList )
	{
		for ( USHORT n = _pList->Count(); n--; )
			delete _pList->GetObject(n);
		delete _pList;
	}
}

//-------------------------------------------------------------------------

BOOL SfxIniDefaultManager::QueryDefault
(
	String&				rValue, 	/* out:	Default-Wert f"ur 'rEntry'
									   (Default ist Leerstring) */
	const SfxIniEntry&	rEntry		// in:	Beschreibung des Eintrags
)

/*	[Beschreibung]

	"Uber diese interne Methode besorgt sich der <SfxIniManager> den
	Default f"ur einen in 'rEntry' beschriebenen Eintrag.
*/

{
	for ( USHORT n = _pList->Count(); n--; )
		if ( _pList->GetObject(n)->QueryDefault( rValue, rEntry ) )
			return TRUE;
	return FALSE;
}

//=========================================================================

SfxIniDefaulter::SfxIniDefaulter( SfxIniDefaultManager *pManager )

/*	[Beschreibung]

	Der Ctor dieser Klasse meldet die neue Instanz automatisch am
	<SfxiniDefaultManager> 'pManager' an.
*/

:	_pManager( pManager )

{
	pManager->Insert( this );
}

//-------------------------------------------------------------------------

SfxIniDefaulter::~SfxIniDefaulter()

/*	[Beschreibung]

	Der Dtor dieser Klasse meldet die neue Instanz automatisch am
	<SfxiniDefaultManager> ab, der im Ctor angegeben wurde.
*/

{
	_pManager->Remove( this );
}

//-------------------------------------------------------------------------

BOOL SfxIniDefaulter::QueryDefault
(
	String&				rValue, 	/* out:	Default-Wert f"ur 'rEntry'
									   (Default ist Leerstring) */
	const SfxIniEntry&	rEntry		// in:	Beschreibung des Eintrags
)

/*	[Beschreibung]

	Diese virtuelle Methode mu\s "uberladen werden. Sie soll dann in
	'rValue' einen Default-Wert f"ur den in 'rEntry' beschriebenen
	ini-Eintrag setzen, falls ihr dieser bekannt ist.


	[Returnwert]

	TRUE			In 'rValue' befindet sich der Default-Wert.

	FALSE			F"ur diesen Eintrag ist kein Default-Wert bekannt.

*/

{
	return FALSE;
};

//========================================================================

SfxIniEntry::SfxIniEntry
(
	const String&  	aGroup,
	const String&	aKey,
	SfxIniGroup		eGroup,
	SfxIniKey		eKey,
	USHORT		 	nIndex
)
:	_aGroup( aGroup ),
	_aKey( aKey ),
	_eGroup( eGroup ),
	_eKey( eKey ),
	_nIndex( nIndex )
{
}


}
