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



#ifndef _SFXINIDEF_HXX
#define _SFXINIDEF_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _INIMGR_HXX
#include <iniman.hxx>
#endif

namespace binfilter
{

class SfxIniEntry;
class SfxIniDefaulterList;
class SfxIniDefaultManager;
}

#if _SOLAR__PRIVATE
#include <bf_svtools/svarray.hxx>

namespace binfilter
{
class SfxIniDefaulter;
SV_DECL_PTRARR( SfxIniDefaulterList, SfxIniDefaulter*, 0, 2 );
}
#endif

namespace binfilter
{

//========================================================================

class SfxIniDefaulter

/*	[Beschreibung]

	Instanzen dieser Klasse k"onnen (f"ur die Laufzeit eines Moduls, z.B.
	einer DLL-Nutzung oder einer Applikation) angelegt werden, um Defaults
	f"ur eine Anzahl von ini-Eintr"agen zu liefern. Diese Defaults werden
	erfragt, wenn ein <SfxIniManager> einen Eintrag nicht in den ini-Files
	finden kann.

	Defaults verschiedener <SfxIniDefaulter>-Instanzen d"urfen sich nicht
	unterscheiden, da die Reihenfolge der Abarbeitung nicht definiert ist.

	Die Instanzen brauchen nicht zerst"ort zu werden, au\ser wenn der
	dazugeh"orige Code entladen wird (load-on-demand DLLs). Sonst geschieht
	dies automatisch beim Zerst"oren des <SfxIniDefaultManager>.
*/

{
	SfxIniDefaultManager*	_pManager;

public:
							SfxIniDefaulter( SfxIniDefaultManager *pManager );
							~SfxIniDefaulter();

	virtual BOOL			QueryDefault( String &aValue,
										  const SfxIniEntry &rEntry ) = 0;
};

//========================================================================

class SfxIniDefaultManager

/*	[Beschreibung]

	Genau eine Instanz dieser Klasse mu\s in jeder Application-Subklasse,
	die diesen Mechanismus nutzen m"ochte, in deren Ctor angelegt werden
	und sollte in deren Dtor zerst"ort werden.
*/

{
	SfxIniDefaulterList*	_pList;

public:
							SfxIniDefaultManager();
							~SfxIniDefaultManager();

#if _SOLAR__PRIVATE
	void					Insert( SfxIniDefaulter *pDefaulter )
							{ _pList->C40_INSERT( SfxIniDefaulter, pDefaulter, _pList->Count() ); }
	void					Remove( SfxIniDefaulter *pDefaulter )
							{ _pList->Remove( _pList->C40_GETPOS( SfxIniDefaulter, pDefaulter ) ); }
	BOOL					QueryDefault( String &aValue,
										  const SfxIniEntry &rEntry );
#endif
};

//========================================================================

class SfxIniEntry

/*	[Beschreibung]

	Instanzen dieser Klasse beschreiben einen Eintrag eines Ini-Files,
	um mit der Klasse <SfxIniDefaulter> erfragt werden zu k"onnen, falls
	der <SfxIniManager> einen Eintrag nicht in den Ini-Files auffindet.
*/

{
friend class SfxIniManager;

	const String&   		_aGroup;
	const String&			_aKey;
	SfxIniGroup 			_eGroup;
	SfxIniKey				_eKey;
	USHORT					_nIndex;

private:
							SfxIniEntry( const String&  aGroup,
										 const String&	aKey,
										 SfxIniGroup	eGroup,
										 SfxIniKey		eKey,
										 USHORT		 	nIndex );
							SfxIniEntry( const SfxIniEntry & ); // n.i.
	SfxIniEntry&			operator=( const SfxIniEntry & ); // n.i.

public:
	const String&			GetGroupName() const { return _aGroup; }
	SfxIniGroup 			GetGroup() const { return _eGroup; }
	const String&			GetKeyName() const { return _aKey; }
	SfxIniKey				GetKey() const { return _eKey; }
	USHORT 					GetIndex() const { return _nIndex; }
};

}

#endif

