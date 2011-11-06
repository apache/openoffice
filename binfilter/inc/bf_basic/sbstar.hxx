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



#ifndef _SB_SBSTAR_HXX
#define _SB_SBSTAR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SBXCLASS_HXX //autogen
#include "sbx.hxx"
#endif
#ifndef _SBX_SBXOBJECT_HXX //autogen
#include <sbxobj.hxx>
#endif
#ifndef _RTL_USTRING_HXX
#include <rtl/ustring.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_ 
#include <osl/mutex.hxx>
#endif

#include "sbdef.hxx"
#include "sberrors.hxx"

namespace binfilter {


class SbModule;                     // fertiges Modul
class SbiInstance;                  // Laufzeit-Instanz
class SbiRuntime;                   // aktuell laufende Prozedur
class SbiImage;                     // compiliertes Image
class BasicLibInfo;					// Infoblock fuer Basic-Manager
class SbMethod;
class BasicManager;

class StarBASICImpl;

class StarBASIC : public SbxObject
{
	friend class SbiScanner;
	friend class SbiExpression;			// Zugriff auf RTL
	friend class SbiInstance;
	friend class SbiRuntime;

	StarBASICImpl*	mpStarBASICImpl;

	SbxArrayRef		pModules;			// Liste aller Module
	SbxObjectRef	pRtl;				// Runtime Library
	SbxArrayRef		xUnoListeners;		// Listener handled by CreateUnoListener
										// Handler-Support:
	Link			aErrorHdl;			// Fehlerbehandlung
	Link			aBreakHdl;			// Breakpoint-Handler
	BOOL			bNoRtl;				// TRUE: RTL nicht durchsuchen
	BOOL			bBreak;				// TRUE: Break, sonst Step
	BasicLibInfo*	pLibInfo;			// Infoblock fuer Basic-Manager
	SbLanguageMode	eLanguageMode;		// LanguageMode des Basic-Objekts
protected:
	BOOL 			CError( SbError, const String&, xub_StrLen, xub_StrLen, xub_StrLen );
private:
#if _SOLAR__PRIVATE
	BOOL 			RTError( SbError, xub_StrLen, xub_StrLen, xub_StrLen );
	BOOL 			RTError( SbError, const String& rMsg, xub_StrLen, xub_StrLen, xub_StrLen );
#endif
	virtual BOOL LoadData( SvStream&, USHORT );
	virtual BOOL StoreData( SvStream& ) const;

protected:

	virtual	BOOL  	ErrorHdl();
	virtual	USHORT	BreakHdl();
	virtual ~StarBASIC();

public:

	SBX_DECL_PERSIST_NODATA(SBXCR_SBX,SBXID_BASIC,1);
	TYPEINFO();

	StarBASIC( StarBASIC* pParent = NULL );

	// #51727 SetModified ueberladen, damit der Modified-
	// Zustand nicht an den Parent weitergegeben wird.
	virtual void SetModified( BOOL );

	void* operator 	new( size_t );
	void operator 	delete( void* );

	virtual void    Insert( SbxVariable* );
	using SbxObject::Remove;
	virtual void    Remove( SbxVariable* );
	virtual void	Clear();

	BasicLibInfo*	GetLibInfo()					{ return pLibInfo;	}
	void			SetLibInfo( BasicLibInfo* p )   { pLibInfo = p;		}

	// Compiler-Interface
    SbModule*   	MakeModule32( const String& rName, const ::rtl::OUString& rSrc );
	BOOL			Compile( SbModule* );
	static void 	Stop();
	static void 	Error( SbError );
	static void 	Error( SbError, const String& rMsg );
	static void 	FatalError( SbError );
	static BOOL 	IsRunning();

	virtual SbxVariable* Find( const String&, SbxClassType );
	virtual BOOL Call( const String&, SbxArray* = NULL );

	SbxArray*		GetModules() { return pModules; }
	SbxObject*		GetRtl()	 { return pRtl;		}
	SbModule*		FindModule( const String& );
	// Init-Code aller Module ausfuehren (auch in inserteten Doc-Basics)
	void			InitAllModules( StarBASIC* pBasicNotToInit = NULL );
	void			DeInitAllModules( void );

	// Abfragen fuer den Error-Handler und den Break-Handler:
	static void		SetErrorData( SbError nCode, USHORT nLine,
								  USHORT nCol1, USHORT nCol2 );

	// Spezifisch fuer den Error-Handler:
	static void		MakeErrorText( SbError, const String& aMsg );
	static USHORT	GetVBErrorCode( SbError nError );
	static SbError	GetSfxFromVBError( USHORT nError );
	// Lokale Einstellung
	void SetLanguageMode( SbLanguageMode eLangMode )
		{ eLanguageMode = eLangMode; }

	// Spezifisch fuer den Break-Handler:
	BOOL   			IsBreak() const				{ return bBreak; }

	Link 			GetErrorHdl() const { return aErrorHdl; }
	void 			SetErrorHdl( const Link& r ) { aErrorHdl = r; }

	Link 			GetBreakHdl() const { return aBreakHdl; }
	void 			SetBreakHdl( const Link& r ) { aBreakHdl = r; }

	// #91147 TRUE: Reschedule is enabled (default>, FALSE: No reschedule
	static void StaticEnableReschedule( BOOL bReschedule );

	SbxObjectRef getRTL( void ) { return pRtl; }
};


#ifndef __SB_SBSTARBASICREF_HXX
#define __SB_SBSTARBASICREF_HXX

SV_DECL_IMPL_REF(StarBASIC)

#endif

}

#endif

