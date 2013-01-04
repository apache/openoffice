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



#ifndef _SB_INTERN_HXX
#define _SB_INTERN_HXX

#include <sbxfac.hxx>
#ifndef _UNOTOOLS_TRANSLITERATIONWRAPPER_HXX
#include <unotools/transliterationwrapper.hxx>
#endif
#include "sb.hxx"

namespace 
{
    class TransliterationWrapper;
}

namespace binfilter {

class SbUnoFactory;
class SbTypeFactory;
class SbOLEFactory;
class SbiInstance;
class SbModule;

class SbiFactory : public SbxFactory
{
public:
	virtual SbxBase* Create( UINT16 nSbxId, UINT32 = SBXCR_SBX );
	virtual SbxObject* CreateObject( const String& );
};

struct SbClassData
{
	SbxArrayRef		mxIfaces;

	~SbClassData( void )
		{ clear(); }
	void clear( void );
};

// #115824: Factory class to create class objects (type command)
// Implementation: sb.cxx
class SbClassFactory : public SbxFactory
{
	SbxObjectRef	xClassModules;

public:
	SbClassFactory( void );
	virtual ~SbClassFactory();

	virtual SbxBase* Create( UINT16 nSbxId, UINT32 = SBXCR_SBX );
	virtual SbxObject* CreateObject( const String& );
};

// Stack fuer die im Fehlerfall abgebaute SbiRuntime Kette
class SbErrorStackEntry
{
public:
	SbErrorStackEntry(SbMethodRef aM, xub_StrLen nL, xub_StrLen nC1, xub_StrLen nC2)
		: aMethod(aM), nLine(nL), nCol1(nC1), nCol2(nC2) {}
	SbMethodRef aMethod;
	xub_StrLen nLine;
	xub_StrLen nCol1, nCol2;
};

SV_DECL_PTRARR_DEL(SbErrorStack, SbErrorStackEntry*, 1, 1)



struct SbiGlobals
{
	SbiInstance*	pInst; 			// alle aktiven Runtime-Instanzen
	SbiFactory*		pSbFac;			// StarBASIC-Factory
	SbUnoFactory*	pUnoFac;		// Factory fuer Uno-Structs bei DIM AS NEW
	SbTypeFactory*	pTypeFac;		// Factory for user defined types
	SbClassFactory*	pClassFac;		// Factory for user defined classes (based on class modules)
	SbOLEFactory*	pOLEFac;		// Factory for OLE types
	SbModule*		pMod;			// aktuell aktives Modul
	SbModule*		pCompMod;		// aktuell compiliertes Modul
	short			nInst;			// Anzahl BASICs
	Link			aErrHdl;		// globaler Error-Handler
	Link			aBreakHdl;		// globaler Break-Handler
	SbError			nCode;			// aktueller Fehlercode
	xub_StrLen		nLine;			// aktuelle Zeile
	xub_StrLen		nCol1,nCol2;	// aktuelle Spalten (von,bis)
	BOOL			bCompiler;		// Flag fuer Compiler-Error
	BOOL			bGlobalInitErr;	// Beim GlobalInit trat ein Compiler-Fehler auf
	BOOL			bRunInit;		// TRUE, wenn RunInit vom Basic aktiv ist
	String			aErrMsg;		// Puffer fuer GetErrorText()
	SbLanguageMode	eLanguageMode;	// Flag fuer Visual-Basic-Script-Modus
	SbErrorStack*	pErrStack;		// Stack fuer die im Fehlerfall abgebaute SbiRuntime Kette
    ::utl::TransliterationWrapper* pTransliterationWrapper;    // For StrComp
	BOOL			bBlockCompilerError;
    BasicManager*   pAppBasMgr;

	SbiGlobals();
	~SbiGlobals();
};

// Utility-Makros und -Routinen

SbiGlobals* GetSbData();

#define	pINST		GetSbData()->pInst
#define pMOD		GetSbData()->pMod
#define pCMOD		GetSbData()->pCompMod
#define pSBFAC		GetSbData()->pSbFac
#define pUNOFAC		GetSbData()->pUnoFac
#define pTYPEFAC	GetSbData()->pTypeFac
#define pCLASSFAC	GetSbData()->pClassFac
#define pOLEFAC		GetSbData()->pOLEFac

}

#endif

