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



#ifndef _SBIMAGE_HXX
#define _SBIMAGE_HXX

#include "sbintern.hxx"
#ifndef _RTL_USTRING_HXX
#include <rtl/ustring.hxx>
#endif
#include "filefmt.hxx"

// Diese Klasse liest das vom Compiler erzeugte Image ein und verwaltet
// den Zugriff auf die einzelnen Elemente.

namespace binfilter {

struct SbPublicEntry;

class SbiImage {
	friend class SbiCodeGen;			// Compiler-Klassen, die die private-

	SbxArrayRef    rTypes;			// User defined types
	SbxArrayRef    rEnums;			// Enum types
	UINT32*		   pStringOff;		// StringId-Offsets
	sal_Unicode*   pStrings;		// StringPool
	char* 		   pCode;			// Code-Image
	char* 		   pLegacyPCode;		// Code-Image
	BOOL		   bError;			// TRUE: Fehler
	USHORT		   nFlags;			// Flags (s.u.)
	short		   nStrings;   		// Anzahl Strings
	UINT32		   nStringSize;		// Groesse des String-Puffers
	UINT32		   nCodeSize;		// Groesse des Code-Blocks
	UINT16		   nLegacyCodeSize;		// Groesse des Code-Blocks
	UINT16		   nDimBase;		// OPTION BASE-Wert
	rtl_TextEncoding eCharSet;		// Zeichensatz fuer Strings
									// temporaere Verwaltungs-Variable:
	short		   nStringIdx;		// aktueller String-Index
	UINT32		   nStringOff;		// aktuelle Pos im Stringpuffer
									// Routinen fuer Compiler:
	void MakeStrings( short );		// StringPool einrichten

public:
	String aName;					// Makroname
    ::rtl::OUString aOUSource;		// Quellcode
	String aComment;				// Kommentar
	BOOL   bInit;					// TRUE: Init-Code ist gelaufen
	BOOL   bFirstInit;      		// TRUE, wenn das Image das erste mal nach
                                    // dem Compilieren initialisiert wird.
	SbiImage();
   ~SbiImage();
	void Clear();					// Inhalt loeschen
	BOOL Load( SvStream&, UINT32& nVer );		// Loads image from stream
							// nVer is set to version
							// of image
	BOOL Save( SvStream&, UINT32 = B_CURVERSION );
	BOOL IsError() 					{ return bError;    }

	const char* GetCode() const 	{ return pCode; 	}
	UINT32		GetCodeSize() const	{ return nCodeSize;	}
    ::rtl::OUString& GetSource32() 	{ return aOUSource; }
	USHORT		GetBase() const		{ return nDimBase;	}
	const SbxObject*  FindType (String aTypeName) const;

	SbxArrayRef GetEnums()			{ return rEnums; }

	void		SetFlag( USHORT n )	{ nFlags |= n;		}
	USHORT		GetFlag( USHORT n )	const { return nFlags & n; }
	UINT16		CalcLegacyOffset( INT32 nOffset );
	UINT32		CalcNewOffset( INT16 nOffset );
	void		ReleaseLegacyBuffer();
	BOOL		ExceedsLegacyLimits();
	
};

#define	SBIMG_EXPLICIT		0x0001	// OPTION EXPLICIT ist aktiv
#define	SBIMG_COMPARETEXT	0x0002	// OPTION COMPARE TEXT ist aktiv
#define	SBIMG_INITCODE		0x0004	// Init-Code vorhanden
#define	SBIMG_CLASSMODULE	0x0008	// OPTION ClassModule is active
#define	SBIMG_VBASUPPORT	0x0020	// OPTION VBASupport is 1

}

#endif
