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
#include "precompiled_basic.hxx"

#include "sbcomp.hxx"
#include <com/sun/star/script/ModuleType.hpp>

struct SbiParseStack {				// "Stack" für Statement-Blocks
	SbiParseStack* pNext;			// Chain
	SbiExprNode* pWithVar;			// Variable für WITH
	SbiToken eExitTok;				// Exit-Token
	sal_uInt32  nChain;					// JUMP-Chain
};

struct SbiStatement {
	SbiToken eTok;
	void( SbiParser::*Func )();		// Verarbeitungsroutine
	sal_Bool  bMain;					// sal_True: ausserhalb SUBs OK
	sal_Bool  bSubr;					// sal_True: in SUBs OK
};

#define	Y	sal_True
#define	N	sal_False

static SbiStatement StmntTable [] = {
{ CALL,		&SbiParser::Call,		N, Y, }, // CALL
{ CLOSE,	&SbiParser::Close,		N, Y, }, // CLOSE
{ _CONST_,	&SbiParser::Dim, 		Y, Y, }, // CONST
{ DECLARE,	&SbiParser::Declare,	Y, N, }, // DECLARE
{ DEFBOOL,	&SbiParser::DefXXX,		Y, N, }, // DEFBOOL
{ DEFCUR,	&SbiParser::DefXXX,		Y, N, }, // DEFCUR
{ DEFDATE,	&SbiParser::DefXXX,		Y, N, }, // DEFDATE
{ DEFDBL,	&SbiParser::DefXXX,		Y, N, }, // DEFDBL
{ DEFERR,	&SbiParser::DefXXX,		Y, N, }, // DEFERR
{ DEFINT,	&SbiParser::DefXXX,		Y, N, }, // DEFINT
{ DEFLNG,	&SbiParser::DefXXX,		Y, N, }, // DEFLNG
{ DEFOBJ,	&SbiParser::DefXXX,		Y, N, }, // DEFOBJ
{ DEFSNG,	&SbiParser::DefXXX,		Y, N, }, // DEFSNG
{ DEFSTR,	&SbiParser::DefXXX,		Y, N, }, // DEFSTR
{ DEFVAR,	&SbiParser::DefXXX,		Y, N, }, // DEFVAR
{ DIM,		&SbiParser::Dim,		Y, Y, }, // DIM
{ DO,		&SbiParser::DoLoop,		N, Y, }, // DO
{ ELSE,		&SbiParser::NoIf,		N, Y, }, // ELSE
{ ELSEIF,	&SbiParser::NoIf,		N, Y, }, // ELSEIF
{ ENDIF,	&SbiParser::NoIf,		N, Y, }, // ENDIF
{ END,		&SbiParser::Stop,		N, Y, }, // END
{ ENUM,		&SbiParser::Enum,		Y, N, }, // TYPE
{ ERASE,	&SbiParser::Erase,		N, Y, }, // ERASE
{ _ERROR_,	&SbiParser::ErrorStmnt,	N, Y, }, // ERROR
{ EXIT,		&SbiParser::Exit,		N, Y, }, // EXIT
{ FOR,		&SbiParser::For,		N, Y, }, // FOR
{ FUNCTION,	&SbiParser::SubFunc,	Y, N, }, // FUNCTION
{ GOSUB,	&SbiParser::Goto,		N, Y, }, // GOSUB
{ GLOBAL,	&SbiParser::Dim,		Y, N, }, // GLOBAL
{ GOTO,		&SbiParser::Goto,		N, Y, }, // GOTO
{ IF,		&SbiParser::If,			N, Y, }, // IF
{ IMPLEMENTS, &SbiParser::Implements, Y, N, }, // IMPLEMENTS
{ INPUT,	&SbiParser::Input,		N, Y, }, // INPUT
{ LET,		&SbiParser::Assign,		N, Y, }, // LET
{ LINE,		&SbiParser::Line,		N, Y, }, // LINE, -> LINE INPUT (#i92642)
{ LINEINPUT,&SbiParser::LineInput,	N, Y, }, // LINE INPUT
{ LOOP,		&SbiParser::BadBlock,	N, Y, }, // LOOP
{ LSET,		&SbiParser::LSet,		N, Y, }, // LSET
{ NAME,		&SbiParser::Name,		N, Y, }, // NAME
{ NEXT,		&SbiParser::BadBlock,	N, Y, }, // NEXT
{ ON,		&SbiParser::On,			N, Y, }, // ON
{ OPEN,		&SbiParser::Open,		N, Y, }, // OPEN
{ OPTION,	&SbiParser::Option,		Y, N, }, // OPTION
{ PRINT,	&SbiParser::Print,		N, Y, }, // PRINT
{ PRIVATE,	&SbiParser::Dim,		Y, N, }, // PRIVATE
{ PROPERTY,	&SbiParser::SubFunc,	Y, N, }, // FUNCTION
{ PUBLIC,	&SbiParser::Dim,		Y, N, }, // PUBLIC
{ REDIM,	&SbiParser::ReDim,		N, Y, }, // DIM
{ RESUME,	&SbiParser::Resume,		N, Y, }, // RESUME
{ RETURN,	&SbiParser::Return,		N, Y, }, // RETURN
{ RSET,		&SbiParser::RSet,		N, Y, }, // RSET
{ SELECT,	&SbiParser::Select,		N, Y, }, // SELECT
{ SET,		&SbiParser::Set,		N, Y, }, // SET
{ STATIC,	&SbiParser::Static,		Y, Y, }, // STATIC
{ STOP,		&SbiParser::Stop,		N, Y, }, // STOP
{ SUB,		&SbiParser::SubFunc,	Y, N, }, // SUB
{ TYPE,		&SbiParser::Type,		Y, N, }, // TYPE
{ UNTIL,	&SbiParser::BadBlock,	N, Y, }, // UNTIL
{ WHILE,	&SbiParser::While,		N, Y, }, // WHILE
{ WEND,		&SbiParser::BadBlock,	N, Y, }, // WEND
{ WITH,		&SbiParser::With,		N, Y, }, // WITH
{ WRITE,	&SbiParser::Write,		N, Y, }, // WRITE
 { PP_IF,      &SbiParser::PPIf,           Y, Y, }, // #IF
 { PP_ELSEIF,  &SbiParser::PPIf,           Y, Y, }, // #ELSEIF
 { PP_ELSE,    &SbiParser::NoIf,           Y, Y, }, // #ELSE
 { PP_ENDIF,   &SbiParser::NoIf,           Y, Y, }, // #ENDIF
 { PP_CONST,   &SbiParser::PPConst,        Y, Y, }, // #CONST

{ NIL, NULL, N, N }
};


#ifdef _MSC_VER
// 'this' : used in base member initializer list
#pragma warning( disable: 4355 )
#endif

SbiParser::SbiParser( StarBASIC* pb, SbModule* pm )
		: SbiTokenizer( pm->GetSource32(), pb ),
		  aGblStrings( this ),
		  aLclStrings( this ),
		  aGlobals( aGblStrings, SbGLOBAL ),
		  aPublics( aGblStrings, SbPUBLIC ),
		  aRtlSyms( aGblStrings, SbRTL ),
		  aGen( *pm, this, 1024 )
{
	pBasic	 = pb;
	eCurExpr = SbSYMBOL;
	eEndTok  = NIL;
	pProc    = NULL;
	pStack   = NULL;
	pWithVar = NULL;
	nBase	 = 0;
	bText	 =
	bGblDefs =
	bNewGblDefs =
	bSingleLineIf =
	bExplicit = sal_False;
	bClassModule = ( pm->GetModuleType() == com::sun::star::script::ModuleType::CLASS );
	OSL_TRACE("Parser - %s, bClassModule %d", rtl::OUStringToOString( pm->GetName(), RTL_TEXTENCODING_UTF8 ).getStr(), bClassModule );
	pPool	 = &aPublics;
	for( short i = 0; i < 26; i++ )
		eDefTypes[ i ] = SbxVARIANT; // Kein expliziter Defaulttyp

	aPublics.SetParent( &aGlobals );
	aGlobals.SetParent( &aRtlSyms );

	// Die globale Chainkette faengt bei Adresse 0 an:
	nGblChain = aGen.Gen( _JUMP, 0 );

	rTypeArray = new SbxArray; // Array fuer Benutzerdefinierte Typen
	rEnumArray = new SbxArray; // Array for Enum types
	bVBASupportOn = pm->IsVBACompat();
	if ( bVBASupportOn )
		EnableCompatibility();

}


// Ist Teil der Runtime-Library?
SbiSymDef* SbiParser::CheckRTLForSym( const String& rSym, SbxDataType eType )
{
	SbxVariable* pVar = GetBasic()->GetRtl()->Find( rSym, SbxCLASS_DONTCARE );
	SbiSymDef* pDef = NULL;
	if( pVar )
	{
		if( pVar->IsA( TYPE(SbxMethod) ) )
		{
			SbiProcDef* pProc_ = aRtlSyms.AddProc( rSym );
			SbxMethod* pMethod = (SbxMethod*) pVar;
			if ( pMethod && pMethod->IsRuntimeFunction() )
			{
				pProc_->SetType( pMethod->GetRuntimeFunctionReturnType() );
			}
			else
			{
				pProc_->SetType( pVar->GetType() );
			}
			pDef = pProc_;
		}
		else
		{
			pDef = aRtlSyms.AddSym( rSym );
			pDef->SetType( eType );
		}
	}
	return pDef;
}

// Globale Chainkette schliessen

sal_Bool SbiParser::HasGlobalCode()
{
	if( bGblDefs && nGblChain )
	{
		aGen.BackChain( nGblChain );
		aGen.Gen( _LEAVE );
		// aGen.Gen( _STOP );
		nGblChain = 0;
	}
	return bGblDefs;
}

void SbiParser::OpenBlock( SbiToken eTok, SbiExprNode* pVar )
{
	SbiParseStack* p = new SbiParseStack;
	p->eExitTok = eTok;
	p->nChain   = 0;
	p->pWithVar = pWithVar;
	p->pNext    = pStack;
	pStack      = p;
	pWithVar    = pVar;

	// #29955 for-Schleifen-Ebene pflegen
	if( eTok == FOR )
		aGen.IncForLevel();
}

void SbiParser::CloseBlock()
{
	if( pStack )
	{
		SbiParseStack* p = pStack;

		// #29955 for-Schleifen-Ebene pflegen
		if( p->eExitTok == FOR )
			aGen.DecForLevel();

		aGen.BackChain( p->nChain );
		pStack = p->pNext;
		pWithVar = p->pWithVar;
		delete p;
	}
}

// EXIT ...

void SbiParser::Exit()
{
	SbiToken eTok = Next();
	for( SbiParseStack* p = pStack; p; p = p->pNext )
	{
		SbiToken eExitTok = p->eExitTok;
		if( eTok == eExitTok ||
			(eTok == PROPERTY && (eExitTok == GET || eExitTok == LET) ) )	// #i109051
		{
			p->nChain = aGen.Gen( _JUMP, p->nChain );
			return;
		}
	}
	if( pStack )
		Error( SbERR_EXPECTED, pStack->eExitTok );
	else
		Error( SbERR_BAD_EXIT );
}

sal_Bool SbiParser::TestSymbol( sal_Bool bKwdOk )
{
	Peek();
	if( eCurTok == SYMBOL || ( bKwdOk && IsKwd( eCurTok ) ) )
	{
		Next(); return sal_True;
	}
	Error( SbERR_SYMBOL_EXPECTED );
	return sal_False;
}

// Testen auf ein bestimmtes Token

sal_Bool SbiParser::TestToken( SbiToken t )
{
	if( Peek() == t )
	{
		Next(); return sal_True;
	}
	else
	{
		Error( SbERR_EXPECTED, t );
		return sal_False;
	}
}

// Testen auf Komma oder EOLN

sal_Bool SbiParser::TestComma()
{
	SbiToken eTok = Peek();
	if( IsEoln( eTok ) )
	{
		Next();
		return sal_False;
	}
	else if( eTok != COMMA )
	{
		Error( SbERR_EXPECTED, COMMA );
		return sal_False;
	}
	Next();
	return sal_True;
}

// Testen, ob EOLN vorliegt

void SbiParser::TestEoln()
{
	if( !IsEoln( Next() ) )
	{
		Error( SbERR_EXPECTED, EOLN );
		while( !IsEoln( Next() ) ) {}
	}
}

// Parsing eines Statement-Blocks
// Das Parsing läuft bis zum Ende-Token.

void SbiParser::StmntBlock( SbiToken eEnd )
{
	SbiToken xe = eEndTok;
	eEndTok = eEnd;
	while( !bAbort && Parse() ) {}
	eEndTok = xe;
	if( IsEof() )
	{
		Error( SbERR_BAD_BLOCK, eEnd );
		bAbort = sal_True;
	}
}

// Die Hauptroutine. Durch wiederholten Aufrufs dieser Routine wird
// die Quelle geparst. Returnwert sal_False bei Ende/Fehlern.

sal_Bool SbiParser::Parse()
{
	if( bAbort ) return sal_False;

	EnableErrors();

	Peek();
	// Dateiende?
	if( IsEof() )
	{
		// AB #33133: Falls keine Sub angelegt wurde, muss hier
		// der globale Chain abgeschlossen werden!
		// AB #40689: Durch die neue static-Behandlung kann noch
		// ein nGblChain vorhanden sein, daher vorher abfragen
		if( bNewGblDefs && nGblChain == 0 )
			nGblChain = aGen.Gen( _JUMP, 0 );
		return sal_False;
	}

	// Leerstatement?
	if( IsEoln( eCurTok ) )
	{
		Next(); return sal_True;
	}

	if( !bSingleLineIf && MayBeLabel( sal_True ) )
	{
		// Ist ein Label
		if( !pProc )
			Error( SbERR_NOT_IN_MAIN, aSym );
		else
			pProc->GetLabels().Define( aSym );
		Next(); Peek();
		// Leerstatement?
		if( IsEoln( eCurTok ) )
		{
			Next(); return sal_True;
		}
	}

	// Ende des Parsings?
	if( eCurTok == eEndTok ||
		( bVBASupportOn	&&		// #i109075
		  (eCurTok == ENDFUNC || eCurTok == ENDPROPERTY || eCurTok == ENDSUB) &&
		  (eEndTok == ENDFUNC || eEndTok == ENDPROPERTY || eEndTok == ENDSUB) ) )
	{
		Next();
		if( eCurTok != NIL )
			aGen.Statement();
		return sal_False;
	}

	// Kommentar?
	if( eCurTok == REM )
	{
		Next(); return sal_True;
	}

	// Kommt ein Symbol, ist es entweder eine Variable( LET )
	// oder eine SUB-Prozedur( CALL ohne Klammern )
	// DOT für Zuweisungen im WITH-Block: .A=5
	if( eCurTok == SYMBOL || eCurTok == DOT )
	{
		if( !pProc )
			Error( SbERR_EXPECTED, SUB );
		else
		{
			// Damit Zeile & Spalte stimmen...
			Next();
			Push( eCurTok );
			aGen.Statement();
				Symbol();
		}
	}
	else
	{
		Next();

		// Hier folgen nun die Statement-Parser.

		SbiStatement* p;
		for( p = StmntTable; p->eTok != NIL; p++ )
			if( p->eTok == eCurTok )
				break;
		if( p->eTok != NIL )
		{
			if( !pProc && !p->bMain )
				Error( SbERR_NOT_IN_MAIN, eCurTok );
			else if( pProc && !p->bSubr )
				Error( SbERR_NOT_IN_SUBR, eCurTok );
			else
			{
				// globalen Chain pflegen
				// AB #41606/#40689: Durch die neue static-Behandlung kann noch
				// ein nGblChain vorhanden sein, daher vorher abfragen
				if( bNewGblDefs && nGblChain == 0 &&
					( eCurTok == SUB || eCurTok == FUNCTION || eCurTok == PROPERTY ) )
				{
					nGblChain = aGen.Gen( _JUMP, 0 );
					bNewGblDefs = sal_False;
				}
				// Statement-Opcode bitte auch am Anfang einer Sub
				if( ( p->bSubr && (eCurTok != STATIC || Peek() == SUB || Peek() == FUNCTION ) ) ||
						eCurTok == SUB || eCurTok == FUNCTION )
					aGen.Statement();
				(this->*( p->Func ) )();
				SbxError nSbxErr = SbxBase::GetError();
				if( nSbxErr )
					SbxBase::ResetError(), Error( (SbError)nSbxErr );
			}
		}
		else
			Error( SbERR_UNEXPECTED, eCurTok );
	}

	// Test auf Ende des Statements:
	// Kann auch ein ELSE sein, da vor dem ELSE kein : stehen muss!

	if( !IsEos() )
	{
		Peek();
		if( !IsEos() && eCurTok != ELSE && eCurTok != PP_ENDIF && eCurTok != PP_ELSE && eCurTok != PP_ELSEIF )
		{
			// falls das Parsing abgebrochen wurde, bis zum ":" vorgehen:
			Error( SbERR_UNEXPECTED, eCurTok );
			while( !IsEos() ) Next();
		}
	}
	// Der Parser bricht am Ende ab, das nächste Token ist noch nicht
	// geholt!
	return sal_True;
}

// Innerste With-Variable liefern
SbiExprNode* SbiParser::GetWithVar()
{
	if( pWithVar )
		return pWithVar;

	// Sonst im Stack suchen
	SbiParseStack* p = pStack;
	while( p )
	{
		// LoopVar kann zur Zeit nur für with sein
		if( p->pWithVar )
			return p->pWithVar;
		p = p->pNext;
	}
	return NULL;
}


// Zuweisung oder Subroutine Call

void SbiParser::Symbol( const KeywordSymbolInfo* pKeywordSymbolInfo )
{
	SbiExprMode eMode = bVBASupportOn ? EXPRMODE_STANDALONE : EXPRMODE_STANDARD;
	SbiExpression aVar( this, SbSYMBOL, eMode, pKeywordSymbolInfo );

	bool bEQ = ( Peek() == EQ );
	if( !bEQ && bVBASupportOn && aVar.IsBracket() )
		Error( SbERR_EXPECTED, "=" );

	RecursiveMode eRecMode = ( bEQ ? PREVENT_CALL : FORCE_CALL );
	bool bSpecialMidHandling = false;
	SbiSymDef* pDef = aVar.GetRealVar();
	if( bEQ && pDef && pDef->GetScope() == SbRTL )
	{
		String aRtlName = pDef->GetName();
		if( aRtlName.EqualsIgnoreCaseAscii("Mid") )
		{
			SbiExprNode* pExprNode = aVar.GetExprNode();
			// SbiNodeType eNodeType;
			if( pExprNode && pExprNode->GetNodeType() == SbxVARVAL )
			{
				SbiExprList* pPar = pExprNode->GetParameters();
				short nParCount = pPar ? pPar->GetSize() : 0;
				if( nParCount == 2 || nParCount == 3 )
				{
					if( nParCount == 2 )
						pPar->addExpression( new SbiExpression( this, -1, SbxLONG ) );

					TestToken( EQ );
					pPar->addExpression( new SbiExpression( this ) );

					bSpecialMidHandling = true;
				}
			}
		}
	}
	aVar.Gen( eRecMode );
	if( !bSpecialMidHandling )
	{
		if( !bEQ )
		{
			aGen.Gen( _GET );
		}
		else
		{
			// Dann muss es eine Zuweisung sein. Was anderes gibt es nicht!
			if( !aVar.IsLvalue() )
				Error( SbERR_LVALUE_EXPECTED );
			TestToken( EQ );
			SbiExpression aExpr( this );
			aExpr.Gen();
			SbiOpcode eOp = _PUT;
			// SbiSymDef* pDef = aVar.GetRealVar();
			if( pDef )
			{
				if( pDef->GetConstDef() )
					Error( SbERR_DUPLICATE_DEF, pDef->GetName() );
				if( pDef->GetType() == SbxOBJECT )
				{
					eOp = _SET;
					if( pDef->GetTypeId() )
					{
						aGen.Gen( _SETCLASS, pDef->GetTypeId() );
						return;
					}
				}
			}
			aGen.Gen( eOp );
		}
	}
}

// Zuweisungen

void SbiParser::Assign()
{
	SbiExpression aLvalue( this, SbLVALUE );
	TestToken( EQ );
	SbiExpression aExpr( this );
	aLvalue.Gen();
	aExpr.Gen();
	sal_uInt16 nLen = 0;
	SbiSymDef* pDef = aLvalue.GetRealVar();
	{
		if( pDef->GetConstDef() )
			Error( SbERR_DUPLICATE_DEF, pDef->GetName() );
		nLen = aLvalue.GetRealVar()->GetLen();
	}
	if( nLen )
		aGen.Gen( _PAD, nLen );
	aGen.Gen( _PUT );
}

// Zuweisungen einer Objektvariablen

void SbiParser::Set()
{
	SbiExpression aLvalue( this, SbLVALUE );
	SbxDataType eType = aLvalue.GetType();
	if( eType != SbxOBJECT && eType != SbxEMPTY && eType != SbxVARIANT )
		Error( SbERR_INVALID_OBJECT );
	TestToken( EQ );
	SbiSymDef* pDef = aLvalue.GetRealVar();
	if( pDef && pDef->GetConstDef() )
		Error( SbERR_DUPLICATE_DEF, pDef->GetName() );

	SbiToken eTok = Peek();
	if( eTok == NEW )
	{
		Next();
		String aStr;
		SbiSymDef* pTypeDef = new SbiSymDef( aStr );
		TypeDecl( *pTypeDef, sal_True );

		aLvalue.Gen();
		// aGen.Gen( _CLASS, pDef->GetTypeId() | 0x8000 );
		aGen.Gen( _CREATE, pDef->GetId(), pTypeDef->GetTypeId() );
		aGen.Gen( _SETCLASS, pDef->GetTypeId() );
	}
	else
	{
		SbiExpression aExpr( this );
		aLvalue.Gen();
		aExpr.Gen();
		// It's a good idea to distinguish between
		// set something = another &
		// something = another
		// ( it's necessary for vba objects where set is object
		// specific and also doesn't involve processing default params )
		if( pDef->GetTypeId() )
		{
			if ( bVBASupportOn )
				aGen.Gen( _VBASETCLASS, pDef->GetTypeId() );
			else
				aGen.Gen( _SETCLASS, pDef->GetTypeId() );
		}
		else
		{
			if ( bVBASupportOn )
				aGen.Gen( _VBASET );
			else
				aGen.Gen( _SET );
		}
	}
	// aGen.Gen( _SET );
}

// JSM 07.10.95
void SbiParser::LSet()
{
	SbiExpression aLvalue( this, SbLVALUE );
	if( aLvalue.GetType() != SbxSTRING )
		Error( SbERR_INVALID_OBJECT );
	TestToken( EQ );
	SbiSymDef* pDef = aLvalue.GetRealVar();
	if( pDef && pDef->GetConstDef() )
		Error( SbERR_DUPLICATE_DEF, pDef->GetName() );
	SbiExpression aExpr( this );
	aLvalue.Gen();
	aExpr.Gen();
	aGen.Gen( _LSET );
}

// JSM 07.10.95
void SbiParser::RSet()
{
	SbiExpression aLvalue( this, SbLVALUE );
	if( aLvalue.GetType() != SbxSTRING )
		Error( SbERR_INVALID_OBJECT );
	TestToken( EQ );
	SbiSymDef* pDef = aLvalue.GetRealVar();
	if( pDef && pDef->GetConstDef() )
		Error( SbERR_DUPLICATE_DEF, pDef->GetName() );
	SbiExpression aExpr( this );
	aLvalue.Gen();
	aExpr.Gen();
	aGen.Gen( _RSET );
}

// DEFINT, DEFLNG, DEFSNG, DEFDBL, DEFSTR und so weiter

void SbiParser::DefXXX()
{
	sal_Unicode ch1, ch2;
	SbxDataType t = SbxDataType( eCurTok - DEFINT + SbxINTEGER );

	while( !bAbort )
	{
		if( Next() != SYMBOL ) break;
		ch1 = aSym.ToUpperAscii().GetBuffer()[0];
		ch2 = 0;
		if( Peek() == MINUS )
		{
			Next();
			if( Next() != SYMBOL ) Error( SbERR_SYMBOL_EXPECTED );
			else
			{
				ch2 = aSym.ToUpperAscii().GetBuffer()[0];
				//ch2 = aSym.Upper();
				if( ch2 < ch1 ) Error( SbERR_SYNTAX ), ch2 = 0;
			}
		}
		if (!ch2) ch2 = ch1;
		ch1 -= 'A'; ch2 -= 'A';
		for (; ch1 <= ch2; ch1++) eDefTypes[ ch1 ] = t;
		if( !TestComma() ) break;
	}
}

// STOP/SYSTEM

void SbiParser::Stop()
{
	aGen.Gen( _STOP );
	Peek();		// #35694: Nur Peek(), damit EOL in Single-Line-If erkannt wird
}

// IMPLEMENTS

void SbiParser::Implements()
{
	if( !bClassModule )
	{
		Error( SbERR_UNEXPECTED, IMPLEMENTS );
		return;
	}

	Peek();
	if( eCurTok != SYMBOL )
	{
		Error( SbERR_SYMBOL_EXPECTED );
		return;
	}

	String aImplementedIface = aSym;
	Next();
	if( Peek() == DOT )
	{
		String aDotStr( '.' );
		while( Peek() == DOT )
		{
			aImplementedIface += aDotStr;
			Next();
			SbiToken ePeekTok = Peek();
			if( ePeekTok == SYMBOL || IsKwd( ePeekTok ) )
			{
				Next();
				aImplementedIface += aSym;
			}
			else
			{
				Next();
				Error( SbERR_SYMBOL_EXPECTED );
				break;
			}
		}
	}
	aIfaceVector.push_back( aImplementedIface );
}

void SbiParser::EnableCompatibility()
{
	if( !bCompatible )
		AddConstants();
	bCompatible = sal_True;
}

// OPTION

void SbiParser::Option()
{
	switch( Next() )
	{
		case BASIC_EXPLICIT:
			bExplicit = sal_True; break;
		case BASE:
			if( Next() == NUMBER )
			{
				if( nVal == 0 || nVal == 1 )
				{
					nBase = (short) nVal;
					break;
				}
			}
			Error( SbERR_EXPECTED, "0/1" );
			break;
		case PRIVATE:
		{
			String aString = SbiTokenizer::Symbol(Next());
			if( !aString.EqualsIgnoreCaseAscii("Module") )
				Error( SbERR_EXPECTED, "Module" );
			break;
		}
		case COMPARE:
		{
			SbiToken eTok = Next();
			if( eTok == BINARY )
				bText = sal_False;
			else if( eTok == SYMBOL && GetSym().EqualsIgnoreCaseAscii("text") )
				bText = sal_True;
			else
				Error( SbERR_EXPECTED, "Text/Binary" );
			break;
		}
		case COMPATIBLE:
			EnableCompatibility();
			break;

		case CLASSMODULE:
			bClassModule = sal_True;
			aGen.GetModule().SetModuleType( com::sun::star::script::ModuleType::CLASS );
			break;
		case VBASUPPORT:
			if( Next() == NUMBER )
			{
				if ( nVal == 1 || nVal == 0 )
				{
					bVBASupportOn = ( nVal == 1 );
					if ( bVBASupportOn )
						EnableCompatibility();
					// if the module setting is different
					// reset it to what the Option tells us
					if ( bVBASupportOn != aGen.GetModule().IsVBACompat() )
						aGen.GetModule().SetVBACompat( bVBASupportOn );
					break;
				}
			}
			Error( SbERR_EXPECTED, "0/1" );
			break;
		default:
			Error( SbERR_BAD_OPTION, eCurTok );
	}
}

void addStringConst( SbiSymPool& rPool, const char* pSym, const String& rStr )
{
	SbiConstDef* pConst = new SbiConstDef( String::CreateFromAscii( pSym ) );
	pConst->SetType( SbxSTRING );
	pConst->Set( rStr );
	rPool.Add( pConst );
}

inline void addStringConst( SbiSymPool& rPool, const char* pSym, const char* pStr )
{
	addStringConst( rPool, pSym, String::CreateFromAscii( pStr ) );
}

void SbiParser::AddConstants( void )
{
	// #113063 Create constant RTL symbols
	addStringConst( aPublics, "vbCr", "\x0D" );
	addStringConst( aPublics, "vbCrLf", "\x0D\x0A" );
	addStringConst( aPublics, "vbFormFeed", "\x0C" );
	addStringConst( aPublics, "vbLf", "\x0A" );
#if defined(UNX)
	addStringConst( aPublics, "vbNewLine", "\x0A" );
#else
	addStringConst( aPublics, "vbNewLine", "\x0D\x0A" );
#endif
	addStringConst( aPublics, "vbNullString", "" );
	addStringConst( aPublics, "vbTab", "\x09" );
	addStringConst( aPublics, "vbVerticalTab", "\x0B" );

	// Force length 1 and make char 0 afterwards
	String aNullCharStr( String::CreateFromAscii( " " ) );
	aNullCharStr.SetChar( 0, 0 );
	addStringConst( aPublics, "vbNullChar", aNullCharStr );
}

// ERROR n

void SbiParser::ErrorStmnt()
{
	SbiExpression aPar( this );
	aPar.Gen();
	aGen.Gen( _ERROR );
}

// MS Excel VBA: #If conditional compilation directive.
// Supports #If, #ElseIf, #Else, #End If with simple expressions:
// True, False, identifier references, Not, And, Or, =, numbers.
void SbiParser::PPIf()
{
	Next();
	bool bCondition = evaluatePPCondition();

	if( eCurTok == THEN )
		Next();

	sal_uInt16 nNesting = 0;
	SbiToken eTok = Peek();
	while( !bAbort && !IsEof() &&
		   !( eTok == PP_ELSEIF || eTok == PP_ELSE || eTok == PP_ENDIF ) )
	{
		if( eCurTok == PP_IF )
			nNesting++;
		else if( eCurTok == PP_ENDIF )
		{
			if( nNesting > 0 )
				nNesting--;
		}

		if( bCondition )
			Parse();
		else
			skipStatement();

		// After Parse/skipStatement, check if we hit a branch delimiter
		if( nNesting == 0 &&
			( eCurTok == PP_ELSEIF || eCurTok == PP_ELSE || eCurTok == PP_ENDIF ) )
		{
			eTok = eCurTok;
			if( ePush == NIL )
				ePush = eCurTok;
			break;
		}
		if( nNesting == 0 && eCurTok == PP_IF )
		{
			eTok = eCurTok;
			if( ePush == NIL )
				ePush = eCurTok;
			break;
		}
		eTok = Peek();
	}

	// After loop, handle branch transitions
	if( bCondition )
	{
		if( eTok == PP_ELSE )
		{
			Next();
			skipUntilEndIf();
		}
		else if( eTok == PP_ELSEIF )
		{
			Next();
			skipUntilEndIf();
		}
		else if( eTok == PP_ENDIF || eTok == PP_IF )
		{
			Next();
		}
	}
	else
	{
		if( eTok == PP_ELSEIF )
		{
			Next();
			if( evaluatePPCondition() )
			{
				if( eCurTok == THEN )
					Next();
				SbiToken eTok2 = Peek();
				while( !bAbort && !IsEof() &&
					   !( eTok2 == PP_ELSEIF || eTok2 == PP_ELSE || eTok2 == PP_ENDIF ) )
				{
					Parse();
					if( eCurTok == PP_ELSEIF || eCurTok == PP_ELSE || eCurTok == PP_ENDIF )
					{
						eTok2 = eCurTok;
						if( ePush == NIL )
							ePush = eCurTok;
						break;
					}
					eTok2 = Peek();
				}
				if( eTok2 == PP_ENDIF )
					Next();
				else if( eTok2 == PP_ELSE )
				{
					Next();
					skipUntilEndIf();
				}
			}
			else
			{
				if( eCurTok == THEN )
					Next();
				skipUntilEndIf();
				if( eCurTok == PP_ELSE )
				{
					Next();
					SbiToken eTok2 = Peek();
					while( !bAbort && !IsEof() &&
						   !( eTok2 == PP_ELSEIF || eTok2 == PP_ELSE || eTok2 == PP_ENDIF ) )
					{
						Parse();
						if( eCurTok == PP_ELSEIF || eCurTok == PP_ELSE || eCurTok == PP_ENDIF )
						{
							eTok2 = eCurTok;
							if( ePush == NIL )
								ePush = eCurTok;
							break;
						}
						eTok2 = Peek();
					}
					if( eTok2 == PP_ENDIF )
						Next();
				}
				else if( eCurTok == PP_ENDIF )
					Next();
			}
			return;
		}
		else if( eTok == PP_ELSE )
		{
			Next();
			SbiToken eTok2 = Peek();
			while( !bAbort && !IsEof() &&
				   !( eTok2 == PP_ELSEIF || eTok2 == PP_ELSE || eTok2 == PP_ENDIF ) )
			{
				Parse();
				if( eCurTok == PP_ELSEIF || eCurTok == PP_ELSE || eCurTok == PP_ENDIF )
				{
					eTok2 = eCurTok;
					if( ePush == NIL )
						ePush = eCurTok;
					break;
				}
				eTok2 = Peek();
			}
			if( eTok2 == PP_ENDIF )
				Next();
		}
		else if( eTok == PP_ENDIF )
		{
			Next();
		}
	}
}

// Helper: Check if identifier is a predefined VBA constant
bool SbiParser::isVBAPredefinedConst( const String& aName, bool& bValue )
{
	if( aName.EqualsIgnoreCaseAscii( "VBA7" ) )
	{
		bValue = true;
		return true;
	}
	if( aName.EqualsIgnoreCaseAscii( "Win32" ) ||
	    aName.EqualsIgnoreCaseAscii( "Win64" ) )
	{
		bValue = false;
		return true;
	}
	if( aName.EqualsIgnoreCaseAscii( "Win16" ) ||
	    aName.EqualsIgnoreCaseAscii( "Mac" ) ||
	    aName.EqualsIgnoreCaseAscii( "Win" ) )
	{
		bValue = false;
		return true;
	}
	return false;
}

// Helper: Evaluate a single term in a #If condition (no And/Or).
// Handles: True, False, identifier, Not expr, expr = expr, numbers.
// The caller (PPIf or evaluatePPCondition's And/Or loop) must have already
// called Next() to read the operand token into eCurTok.
// After returning, eCurTok points to the token following the term
// (Then, And, Or, etc.) so evaluatePPCondition can continue.
bool SbiParser::evaluatePPTerm()
{
	bool bResult = false;
	bool bNegate = false;

	// Handle "Not" - aSym is already set by the caller's Next()
	if( aSym.EqualsIgnoreCaseAscii( "Not" ) )
	{
		bNegate = true;
		Next(); // read the negated operand
	}

	// Check aSym BEFORE Peek(), since Peek() overwrites it
	if( aSym.EqualsIgnoreCaseAscii( "True" ) )
	{
		bResult = true;
		Next(); // advance past "True", read next token (Then or And/Or)
	}
	else if( aSym.EqualsIgnoreCaseAscii( "False" ) )
	{
		bResult = false;
		Next(); // advance past "False", read next token
	}
	else if( eCurTok == SYMBOL )
	{
		// Identifier - save before Peek() overwrites aSym
		String aName = aSym;
		Peek(); // look ahead to check for comparison

		if( eCurTok == EQ || eCurTok == NE || eCurTok == LT || eCurTok == GT ||
			eCurTok == LE || eCurTok == GE )
		{
			// Comparison: identifier <op> value
			SbiToken eOp = eCurTok;
			Next(); // consume operator
			Peek(); // read RHS

			// Resolve left-hand value
			bool bLVal = false;
			double nLVal = 0.0;
			bool bLIsNum = false;
			if( maConstNumMap.count( aName ) > 0 )
			{
				nLVal = maConstNumMap[ aName ];
				bLIsNum = true;
				bLVal = (nLVal != 0.0);
			}
			else if( maConstMap.count( aName ) > 0 )
				bLVal = maConstMap[ aName ];
			else
				isVBAPredefinedConst( aName, bLVal );

			bool bRVal = false;
			double nRVal = 0.0;
			bool bRIsNum = false;

			if( eCurTok == SYMBOL )
			{
				// RHS is True/False/identifier
				String aVal = aSym;
				if( aVal.EqualsIgnoreCaseAscii( "True" ) )
					bRVal = true;
				else if( aVal.EqualsIgnoreCaseAscii( "False" ) )
					bRVal = false;
				else if( maConstNumMap.count( aVal ) > 0 )
				{
					nRVal = maConstNumMap[ aVal ];
					bRIsNum = true;
					bRVal = (nRVal != 0.0);
				}
				else if( maConstMap.count( aVal ) > 0 )
					bRVal = maConstMap[ aVal ];
				else
					isVBAPredefinedConst( aVal, bRVal );
				Next(); // consume RHS
			}
			else if( eCurTok == NUMBER )
			{
				bRVal = (nVal != 0.0);
				nRVal = nVal;
				bRIsNum = true;
				Next(); // consume number
			}
			else
			{
				bResult = false;
			}

			bool bNumCmp = bLIsNum || bRIsNum;
			if( eOp == EQ )
				bResult = bNumCmp ? (nLVal == nRVal) : (bLVal == bRVal);
			else if( eOp == NE )
				bResult = bNumCmp ? (nLVal != nRVal) : (bLVal != bRVal);
			else if( eOp == LT )
				bResult = bNumCmp ? (nLVal < nRVal) : (!bLVal && bRVal);
			else if( eOp == GT )
				bResult = bNumCmp ? (nLVal > nRVal) : (bLVal && !bRVal);
			else if( eOp == LE )
				bResult = bNumCmp ? (nLVal <= nRVal) : !bLVal;
			else if( eOp == GE )
				bResult = bNumCmp ? (nLVal >= nRVal) : bRVal;

			// Read next token (THEN, AND, OR, etc.) for caller
			Peek();
		}
		else if( eCurTok == THEN )
		{
			// Just a constant: #If DEBUG Then
			if( maConstMap.count( aName ) > 0 )
				bResult = maConstMap[ aName ];
			else
				isVBAPredefinedConst( aName, bResult );
			// eCurTok is THEN (from Peek) - leave for caller
		}
		else
		{
			// End of expression or other token - use the constant directly
			if( maConstMap.count( aName ) > 0 )
				bResult = maConstMap[ aName ];
			else
				isVBAPredefinedConst( aName, bResult );
			// eCurTok is the peeked token - leave for caller
		}
	}
	else if( eCurTok == NUMBER )
	{
		bResult = (nVal != 0.0);
		Next(); // advance past number, read next token
	}

	if( bNegate )
		bResult = !bResult;

	return bResult;
}

// Helper: Evaluate a conditional compilation expression.
// Supports: True, False, identifier, Not, And, Or, = comparisons, numbers
bool SbiParser::evaluatePPCondition()
{
	bool bResult = evaluatePPTerm();

	// Handle And/Or operators for compound expressions
	while( eCurTok == AND || eCurTok == OR )
	{
		bool bIsAnd = (eCurTok == AND);
		Next(); // consume And/Or
		bool bTerm = evaluatePPTerm();
		if( bIsAnd )
			bResult = bResult && bTerm;
		else
			bResult = bResult || bTerm;
	}

	return bResult;
}
// Helper: Skip statements without generating code
// Tracks nested #If blocks so inactive branches are fully skipped.
// Continues past EOLN/EOS to skip to the next line (unlike a single
// statement terminator check).
void SbiParser::skipStatement()
{
	sal_uInt16 nNesting = 0;
	while( !bAbort && !IsEof() )
	{
		if( eCurTok == PP_IF )
		{
			nNesting++;
			Next();
		}
		else if( eCurTok == PP_ENDIF )
		{
			if( nNesting > 0 )
			{
				nNesting--;
				Next();
			}
			else
				break;
		}
		else if( nNesting == 0 && ( eCurTok == PP_ELSEIF || eCurTok == PP_ELSE ) )
		{
			break;
		}
		else
			Next();
	}
}
// MS Excel VBA: #Const conditional compilation constant declaration.
// Status: implemented - stores name=value in the conditional compilation map
void SbiParser::PPConst()
{
	// #Const name = value
	// Parse: name
	Peek();
	if( eCurTok != SYMBOL )
	{
		Error( SbERR_SYMBOL_EXPECTED );
		Next();
		TestToken( ASSIGN );
		return;
	}

	String aConstName = aSym;
	Next();
	TestToken( EQ );
	Next();

	// Parse the value - expect True, False, or a number
	// eCurTok has the value token already (from Next above)
	if( aSym.EqualsIgnoreCaseAscii( "True" ) )
	{
		maConstMap[ aConstName ] = true;
	}
	else if( aSym.EqualsIgnoreCaseAscii( "False" ) )
	{
		maConstMap[ aConstName ] = false;
	}
	else if( eCurTok == NUMBER )
	{
		maConstMap[ aConstName ] = (nVal != 0.0);
		maConstNumMap[ aConstName ] = nVal;
	}
	else
	{
		// Unknown constant value - default to false
		maConstMap[ aConstName ] = false;
		SbiExpression aExpr( this ); // consume the expression
	}

	// Expect end of statement
	TestEoln();
}

// Helper: Skip all statements until matching #End If
void SbiParser::skipUntilEndIf()
{
	sal_uInt16 nNesting = 0;
	SbiToken eTok = Peek();
	while( !bAbort && !IsEof() )
	{
		if( eTok == PP_IF )
		{
			nNesting++;
			Next();
		}
		else if( eTok == PP_ENDIF )
		{
			if( nNesting == 0 )
			{
				Next();
				return;
			}
			nNesting--;
			Next();
		}
		else
		{
			Next();
		}
		eTok = Peek();
	}
}
