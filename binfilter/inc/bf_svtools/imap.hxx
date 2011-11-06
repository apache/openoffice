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



#ifndef _IMAP_HXX
#define _IMAP_HXX

#include <bf_svtools/bf_solar.h>

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif

class Point;
class Rectangle;
class Size;
class Fraction;

namespace binfilter
{

class IMapObject;

/******************************************************************************
|*
|*
|*
\******************************************************************************/

class  ImageMap
{
	List				maList;
	String				aName;

protected:

	// Binaer laden/speichern
    void                ImpWriteImageMap( SvStream& rOStm, const String& ) const ;
    void                ImpReadImageMap( SvStream& rIStm, USHORT nCount, const String& );

public:

	TYPEINFO();

						ImageMap() {};
						ImageMap( const ImageMap& rImageMap );

						// Der Dtor gibt den intern belegten
						// Speicher wieder frei;
						//  alle IMapObjekte werden im Dtor zerstoert;
	virtual				~ImageMap();

	// Zuweisungsoperator
	ImageMap&			operator=( const ImageMap& rImageMap );

	// Vergleichsoperator (es wird alles auf Gleichheit geprueft)
	BOOL				operator==( const ImageMap& rImageMap );
	BOOL				operator!=( const ImageMap& rImageMap );

	// In die Map wird ein neues IMap-Obkekt ans Ende eingefuegt
	void				InsertIMapObject( const IMapObject& rIMapObject );

	// Zugriff auf einzelne IMapObjekte; die Objekte
	// duerfen von aussen _nicht_ zerstoert werden
	IMapObject*			GetFirstIMapObject() { return (IMapObject*) maList.First(); }
	IMapObject*			GetNextIMapObject() { return (IMapObject*) maList.Next(); }
	IMapObject*			GetLastIMapObject() { return (IMapObject*) maList.Last(); }
	IMapObject*			GetPrevIMapObject() { return (IMapObject*) maList.Prev(); }
	IMapObject*			GetIMapObject( USHORT nPos ) const { return (IMapObject*) maList.GetObject( nPos ); }

	// Gibt die Gesamtanzahl der IMap-Objekte zurueck
	UINT16				GetIMapObjectCount() const { return (UINT16) maList.Count(); }

	// Loescht alle internen Objekte
	void				ClearImageMap();

	// liefert die aktuelle Versionsnummer
	UINT16				GetVersion() const;

	// liefert / setzt den Namen der ImageMap
	const String&		GetName() const { return aName; }
	void				SetName( const String& rName ) { aName = rName; }

	// skaliert alle Objekte der ImageMap entpr. dem uebergebenen Faktor
	void				Scale( const Fraction& rFractX, const Fraction& rFracY );

	// Im-/Export
    void                Write ( SvStream& rOStm, const String& rBaseURL ) const;
    void                Read( SvStream& rIStm, const String& rBaseURL );
};

class IMapCompat
{
	SvStream*		pRWStm;
	ULONG			nCompatPos;
	ULONG			nTotalSize;
	USHORT			nStmMode;

					IMapCompat() {}
					IMapCompat( const IMapCompat& ) {}
	IMapCompat&		operator=( const IMapCompat& ) { return *this; }
	BOOL			operator==( const IMapCompat& ) { return FALSE; }

public:

					IMapCompat( SvStream& rStm, const USHORT nStreamMode );
					~IMapCompat();
};

}

#endif // _IMAP_HXX
