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



#ifndef SC_IMPEX_HXX
#define SC_IMPEX_HXX

#ifndef _SOT_EXCHANGE_HXX //autogen
#include <sot/exchange.hxx>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
#ifndef _OSL_ENDIAN_H_
#include <osl/endian.h>
#endif
class SvStream;
namespace binfilter {

class ScDocShell;
class ScDocument;
class SfxMedium;
class ScAsciiOptions;

class ScImportExport
{
	ScDocShell* pDocSh;
	ScDocument* pDoc;
	ScDocument* pUndoDoc;
	ScRange     aRange;
	String		aStreamPath;
    String      aNonConvertibleChars;
	ULONG		nSizeLimit;
	sal_Unicode	cSep;					// Separator
	sal_Unicode	cStr;					// String Delimiter
	BOOL		bFormulas;				// Formeln im Text?
	BOOL		bIncludeFiltered;		// include filtered rows? (default true)
	BOOL		bAll;					// keine Selektion
	BOOL		bSingle;				// Einfachselektion
	BOOL		bUndo;					// Mit Undo?
	BOOL		bOverflow;				// zuviele Zeilen/Spalten

	ScAsciiOptions*	pExtOptions;		// erweiterte Optionen

	BOOL ExtText2Doc( SvStream& );		// mit pExtOptions

	//! only if stream is only used in own (!) memory

public:
	ScImportExport( ScDocument* );					// Gesamtdokument
   ~ScImportExport();

	void SetExtOptions( const ScAsciiOptions& rOpt );

	BOOL IsDoubleRef() const { return BOOL( !( bAll || bSingle ) ); }
	BOOL IsSingleRef() const { return bSingle; }
	BOOL IsNoRef() const	 { return bAll;    }
	BOOL IsRef() const	 	 { return BOOL( !bAll ); }

	const ScRange& GetRange() const { return aRange; }

	BOOL IsUndo() const		 { return bUndo; }
	void SetUndo( BOOL b )	 { bUndo = b;	 }

	static const sal_Unicode* ScanNextFieldFromString( const sal_Unicode* p,
			String& rField, sal_Unicode cStr, const sal_Unicode* pSeps, BOOL bMergeSeps );
	static	inline	BOOL	IsEndianSwap( const SvStream& rStrm );

	sal_Unicode GetSeparator() const { return cSep; }
	void SetSeparator( sal_Unicode c ) { cSep = c; }
	sal_Unicode GetDelimiter() const { return cStr; }
	void SetDelimiter( sal_Unicode c ) { cStr = c; }
	BOOL IsFormulas() const { return bFormulas; }
	void SetFormulas( BOOL b ) { bFormulas = b; }
	BOOL IsIncludeFiltered() const { return bIncludeFiltered; }
	void SetIncludeFiltered( BOOL b ) { bIncludeFiltered = b; }

	void SetSizeLimit( ULONG nNew ) { nSizeLimit = nNew; }	// momentan nur fuer Ascii

	void			SetStreamPath( const String& rPath ) { aStreamPath = rPath; }
	const String&	GetStreamPath() const { return aStreamPath; }


	BOOL ImportStream( SvStream&, ULONG=FORMAT_STRING );


	BOOL IsOverflow() const	{ return bOverflow; }		// nach dem Importieren

    const String& GetNonConvertibleChars() const { return aNonConvertibleChars; }
};


// static
inline BOOL ScImportExport::IsEndianSwap( const SvStream& rStrm )
{
#ifdef OSL_BIGENDIAN
	return rStrm.GetNumberFormatInt() != NUMBERFORMAT_INT_BIGENDIAN;
#else
	return rStrm.GetNumberFormatInt() != NUMBERFORMAT_INT_LITTLEENDIAN;
#endif
}


} //namespace binfilter
#endif

