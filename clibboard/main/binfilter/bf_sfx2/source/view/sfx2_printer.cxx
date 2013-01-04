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



#include <bf_svtools/itemset.hxx>
#include <bf_svtools/svarray.hxx>

#include "bf_sfx2/printer.hxx"
namespace binfilter {

//--------------------------------------------------------------------

/*N*/ SV_DECL_PTRARR_DEL(SfxFontArr_Impl,SfxFont*,10,5)

// struct SfxPrinter_Impl ------------------------------------------------

/*N*/ struct SfxPrinter_Impl
/*N*/ {
/*N*/ 	SfxFontArr_Impl*	mpFonts;
/*N*/ 	BOOL				mbAll;
/*N*/ 	BOOL				mbSelection;
/*N*/ 	BOOL				mbFromTo;
/*N*/ 	BOOL				mbRange;
/*N*/
/*N*/ 	SfxPrinter_Impl() :
/*N*/ 		mpFonts		( NULL ),
/*N*/ 		mbAll		( TRUE ),
/*N*/ 		mbSelection ( TRUE ),
/*N*/ 		mbFromTo	( TRUE ),
/*N*/ 		mbRange 	( TRUE ) {}
/*N*/ 	~SfxPrinter_Impl() { delete mpFonts; }
/*N*/ };

/*N*/ #define FONTS()	pImpl->mpFonts

//--------------------------------------------------------------------

/*N*/ SfxFont::SfxFont( const FontFamily eFontFamily, const String& aFontName,
/*N*/ 				  const FontPitch eFontPitch, const CharSet eFontCharSet ):
/*N*/ 	aName( aFontName ),
/*N*/ 	eFamily( eFontFamily ),
/*N*/ 		ePitch( eFontPitch ),
/*N*/ 	eCharSet( eFontCharSet )
/*N*/ {
/*N*/ }

// class SfxPrinter ------------------------------------------------------

/*N*/ SfxPrinter* SfxPrinter::Create( SvStream& rStream, SfxItemSet* pOptions )

/* 	[Beschreibung]

	Erzeugt einen <SfxPrinter> aus dem Stream. Geladen wird genaugenommen
	nur ein JobSetup. Falls ein solcher Drucker auf dem System nicht
	verf"augbar ist, wird das Original als Orig-JobSetup gemerkt und
	ein "anhlicher exisitierender Drucker genommen.

	Die 'pOptions' werden in den erzeugten SfxPrinter "ubernommen,
	der Returnwert geh"ort dem Caller.
*/

/*N*/ {
/*N*/ 	// JobSetup laden
/*N*/ 	JobSetup aFileJobSetup;
/*N*/ 	rStream >> aFileJobSetup;
/*N*/
/*N*/ 	// Drucker erzeugen
/*N*/ 	SfxPrinter *pPrinter = new SfxPrinter( pOptions, aFileJobSetup );
/*N*/ 	return pPrinter;
/*N*/ }

//--------------------------------------------------------------------

/*N*/ SvStream& SfxPrinter::Store( SvStream& rStream ) const

/*	[Beschreibung]

	Speichert das verwendete JobSetup des <SfxPrinter>s.
*/

/*N*/ {
/*N*/ 	return ( rStream << GetJobSetup() );
/*N*/ }

//--------------------------------------------------------------------

/*N*/ SfxPrinter::SfxPrinter( SfxItemSet* pTheOptions ) :

/*  [Beschreibung]

	Dieser Ctor erzeugt einen Standard-Drucker.
*/

/*N*/ 	pOptions( pTheOptions ),
/*N*/ 	bKnown(sal_True)

/*N*/ {
/*N*/ 	pImpl = new SfxPrinter_Impl;
/*N*/ }

//--------------------------------------------------------------------

/*N*/ SfxPrinter::SfxPrinter( SfxItemSet* pTheOptions,
/*N*/ 						const JobSetup& rTheOrigJobSetup ) :

/*N*/ 	Printer			( rTheOrigJobSetup.GetPrinterName() ),
/*N*/ 	pOptions		( pTheOptions )

/*N*/ {
/*N*/ 	pImpl = new SfxPrinter_Impl;
/*N*/ 	bKnown = GetName() == rTheOrigJobSetup.GetPrinterName();
/*N*/
/*N*/   if ( bKnown )
        {
/*N*/ 		SetJobSetup( rTheOrigJobSetup );
        }
        
        // --> FME 2006-09-19 #b6449032# Use old XPrinter emulation. rTheOrigJobSetup
        // already has this setting (see SfxPrinter::Create()).
        JobSetup aJobSetup( GetJobSetup() );
        aJobSetup.SetValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "StrictSO52Compatibility" ) ),
                            rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "true" ) ) );
        SetJobSetup( aJobSetup );
        // <--
/*N*/ }

//--------------------------------------------------------------------

/*N*/ SfxPrinter::SfxPrinter( SfxItemSet* pTheOptions,
/*N*/ 						const String& rPrinterName ) :
/*N*/
/*N*/ 	Printer			( rPrinterName ),
/*N*/ 	pOptions		( pTheOptions ),
/*N*/ 	bKnown			( GetName() == rPrinterName )
/*N*/
/*N*/ {
/*N*/ 	pImpl = new SfxPrinter_Impl;
/*N*/ }

//--------------------------------------------------------------------


//--------------------------------------------------------------------


//--------------------------------------------------------------------

/*N*/ SfxPrinter::~SfxPrinter()
/*N*/ {
/*N*/ 	delete pOptions;
/*N*/ 	delete pImpl;
/*N*/ }

//--------------------------------------------------------------------

/*N*/ void SfxPrinter::SetOptions( const SfxItemSet &rNewOptions )
/*N*/ {
/*N*/ 	pOptions->Set(rNewOptions);
/*N*/ }

//--------------------------------------------------------------------

/*N*/ SV_IMPL_PTRARR(SfxFontArr_Impl,SfxFont*)

//--------------------------------------------------------------------

/*?*/ const SfxFont* SfxPrinter::GetFontByName( const String &rFontName )
/*?*/ {DBG_BF_ASSERT(0, "STRIP"); return NULL;//STRIP001
/*?*/ }

}
