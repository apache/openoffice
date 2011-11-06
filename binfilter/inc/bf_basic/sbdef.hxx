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



#ifndef _SB_SBDEF_HXX
#define _SB_SBDEF_HXX


#ifndef _SBXDEF_HXX
#include <sbxdef.hxx>
#endif

#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif

#if _SOLAR__PRIVATE
#define _BASIC_TEXTPORTIONS
#endif

namespace binfilter {

enum SbTextType {					// Typ eines Textteils (Syntax Hilite)
	SB_KEYWORD = 1,					// Keywords
	SB_SYMBOL,						// Symbole
	SB_STRING,						// Strings
	SB_NUMBER,						// Zahlen
	SB_PUNCTUATION,					// Klammern, Punkte etc
	SB_COMMENT,						// Kommentare
    SB_DUMMY = 255                  // workaround for #i31479
};

enum SbLanguageMode {				// Aktive Sprache
	SB_LANG_GLOBAL,					// wie in SbiGlobals-Struktur
	SB_LANG_BASIC,					// StarBasic (Default)
	SB_LANG_VBSCRIPT,				// Visual-Basic-Script
	SB_LANG_JAVASCRIPT				// Java-Script
};

#ifdef _BASIC_TEXTPORTIONS
struct SbTextPortion
{									// Syntax Hiliting: eine Text-Portion
	xub_StrLen nLine;					// Zeilennummer
	xub_StrLen nStart, nEnd;			// 1. und letzte Spalte
	SbTextType eType;				// Type der Portion
};
#endif

// Returns type name for Basic type, array flag is ignored
// implementation: basic/source/runtime/methods.cxx
String getBasicTypeName( SbxDataType eType );

// Returns type name for Basic objects, especially
// important for SbUnoObj instances
// implementation: basic/source/classes/sbunoobj.cxx
class SbxObject;

// Allows Basic IDE to set watch mode to suppress errors
// implementation: basic/source/runtime/runtime.cxx
void setBasicWatchMode( bool bOn );

// Debug-Flags:

#define SbDEBUG_BREAK       0x0001          // Break-Callback
#define SbDEBUG_STEPINTO    0x0002          // Single Step-Callback
#define SbDEBUG_STEPOVER    0x0004          // Zusatzflag Step Over
#define	SbDEBUG_CONTINUE	0x0008			// Flags nicht aendern
#define SbDEBUG_STEPOUT		0x0010          // Aus Sub raus

#define SBXID_BASIC			0x6273			// sb: StarBASIC
#define	SBXID_BASICMOD		0x6d62			// bm: StarBASIC-Modul
#define SBXID_BASICPROP		0x7262			// pr: StarBASIC-Property
#define	SBXID_BASICMETHOD	0x6d65          // me: StarBASIC-Methode
#define	SBXID_JSCRIPTMOD	0x6a62			// jm: JavaScript-Modul
#define	SBXID_JSCRIPTMETH	0x6a64			// jm: JavaScript-Modul


#define SBX_HINT_BASICSTART		SFX_HINT_USER04
#define SBX_HINT_BASICSTOP		SFX_HINT_USER05

// #115826
enum PropertyMode
{
	PROPERTY_MODE_NONE,
	PROPERTY_MODE_GET,
	PROPERTY_MODE_LET,
	PROPERTY_MODE_SET
};

}

#endif
