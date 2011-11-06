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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

//------------------------------------------------------------------

#include <tools/debug.hxx>


#include "inputopt.hxx"
#include "rechead.hxx"
#include "global.hxx"
#include "bf_sc.hrc"
#include "miscuno.hxx"
namespace binfilter {

using namespace utl;
using namespace rtl;
using namespace ::com::sun::star::uno;

//------------------------------------------------------------------

//	Version, ab der das Item kompatibel ist
#define SC_VERSION ((USHORT)351)


//========================================================================
//      ScInputOptions - Eingabe-Optionen
//========================================================================

/*N*/ ScInputOptions::ScInputOptions()
/*N*/ {
/*N*/ 	SetDefaults();
/*N*/ }

//------------------------------------------------------------------------

/*N*/ ScInputOptions::ScInputOptions( const ScInputOptions& rCpy )
/*N*/ {
/*N*/ 	*this = rCpy;
/*N*/ }

//------------------------------------------------------------------------

/*N*/ ScInputOptions::~ScInputOptions()
/*N*/ {
/*N*/ }

//------------------------------------------------------------------------

/*N*/ void ScInputOptions::SetDefaults()
/*N*/ {
/*N*/ 	nMoveDir		= DIR_BOTTOM;
/*N*/ 	bMoveSelection	= TRUE;
/*N*/ 	bEnterEdit		= FALSE;
/*N*/ 	bExtendFormat	= FALSE;
/*N*/ 	bRangeFinder	= TRUE;
/*N*/ 	bExpandRefs		= FALSE;
/*N*/ 	bMarkHeader		= TRUE;
/*N*/ 	bUseTabCol		= FALSE;
/*N*/ 	bTextWysiwyg	= FALSE;
/*N*/     bReplCellsWarn  = TRUE;
/*N*/ }

//------------------------------------------------------------------------

/*N*/ const ScInputOptions& ScInputOptions::operator=( const ScInputOptions& rCpy )
/*N*/ {
/*N*/ 	nMoveDir		= rCpy.nMoveDir;
/*N*/ 	bMoveSelection	= rCpy.bMoveSelection;
/*N*/ 	bEnterEdit		= rCpy.bEnterEdit;
/*N*/ 	bExtendFormat	= rCpy.bExtendFormat;
/*N*/ 	bRangeFinder	= rCpy.bRangeFinder;
/*N*/ 	bExpandRefs		= rCpy.bExpandRefs;
/*N*/ 	bMarkHeader		= rCpy.bMarkHeader;
/*N*/ 	bUseTabCol		= rCpy.bUseTabCol;
/*N*/ 	bTextWysiwyg	= rCpy.bTextWysiwyg;
/*N*/     bReplCellsWarn  = rCpy.bReplCellsWarn;
/*N*/ 
/*N*/ 	return *this;
/*N*/ }

//------------------------------------------------------------------------

/*N*/ SvStream& operator>>( SvStream& rStream, ScInputOptions& rOpt )
/*N*/ {
/*N*/ 	rOpt.SetDefaults();
/*N*/ 
/*N*/ 	ScReadHeader aHdr( rStream );
/*N*/ 
/*N*/ 	rStream >> rOpt.nMoveDir;
/*N*/ 	rStream >> rOpt.bMoveSelection;
/*N*/ 	rStream >> rOpt.bEnterEdit;
/*N*/ 	rStream >> rOpt.bExtendFormat;
/*N*/ 
/*N*/ 	if (aHdr.BytesLeft())
/*N*/ 		rStream >> rOpt.bRangeFinder;			// ab 363
/*N*/ 
/*N*/ 	if (aHdr.BytesLeft())
/*N*/ 		rStream >> rOpt.bExpandRefs;			// ab 364v
/*N*/ 
/*N*/ 	if (aHdr.BytesLeft())
/*N*/ 		rStream >> rOpt.bMarkHeader;			// ab 364irgendwas
/*N*/ 
/*N*/ 	if (aHdr.BytesLeft())
/*N*/ 		rStream >> rOpt.bUseTabCol;				// ab 373d
/*N*/ 
/*N*/ 	// newer additions are not in old file format
/*N*/ 
/*N*/ 	return rStream;
/*N*/ }

//------------------------------------------------------------------------

/*N*/ SvStream& operator<<( SvStream& rStream, const ScInputOptions& rOpt )
/*N*/ {
/*N*/ 	ScWriteHeader aHdr( rStream, 6 );
/*N*/ 
/*N*/ 	rStream << rOpt.nMoveDir;
/*N*/ 	rStream << rOpt.bMoveSelection;
/*N*/ 	rStream << rOpt.bEnterEdit;
/*N*/ 	rStream << rOpt.bExtendFormat;
/*N*/ 	rStream << rOpt.bRangeFinder;
/*N*/ 	rStream << rOpt.bExpandRefs;
/*N*/ 	rStream << rOpt.bMarkHeader;
/*N*/ 	rStream << rOpt.bUseTabCol;
/*N*/ 
/*N*/ 	// newer additions are not in old file format
/*N*/ 
/*N*/ 	return rStream;
/*N*/ }

//==================================================================
//	Config Item containing input options
//==================================================================

#define CFGPATH_INPUT			"Office.Calc/Input"

#define SCINPUTOPT_MOVEDIR			0
#define SCINPUTOPT_MOVESEL			1
#define SCINPUTOPT_EDTEREDIT		2
#define SCINPUTOPT_EXTENDFMT		3
#define SCINPUTOPT_RANGEFIND		4
#define SCINPUTOPT_EXPANDREFS		5
#define SCINPUTOPT_MARKHEADER		6
#define SCINPUTOPT_USETABCOL		7
#define SCINPUTOPT_TEXTWYSIWYG		8
#define SCINPUTOPT_REPLCELLSWARN    9
#define SCINPUTOPT_COUNT            10

/*N*/ Sequence<OUString> ScInputCfg::GetPropertyNames()
/*N*/ {
/*N*/ 	static const char* aPropNames[] =
/*N*/ 	{
/*N*/ 		"MoveSelectionDirection",	// SCINPUTOPT_MOVEDIR
/*N*/ 		"MoveSelection",			// SCINPUTOPT_MOVESEL
/*N*/ 		"SwitchToEditMode",			// SCINPUTOPT_EDTEREDIT
/*N*/ 		"ExpandFormatting",			// SCINPUTOPT_EXTENDFMT
/*N*/ 		"ShowReference",			// SCINPUTOPT_RANGEFIND
/*N*/ 		"ExpandReference",			// SCINPUTOPT_EXPANDREFS
/*N*/ 		"HighlightSelection",		// SCINPUTOPT_MARKHEADER
/*N*/ 		"UseTabCol",				// SCINPUTOPT_USETABCOL
/*N*/         "UsePrinterMetrics",        // SCINPUTOPT_TEXTWYSIWYG
/*N*/         "ReplaceCellsWarning"       // SCINPUTOPT_REPLCELLSWARN
/*N*/ 	};
/*N*/ 	Sequence<OUString> aNames(SCINPUTOPT_COUNT);
/*N*/ 	OUString* pNames = aNames.getArray();
/*N*/ 	for(int i = 0; i < SCINPUTOPT_COUNT; i++)
/*N*/ 		pNames[i] = OUString::createFromAscii(aPropNames[i]);
/*N*/ 
/*N*/ 	return aNames;
/*N*/ }

/*N*/ ScInputCfg::ScInputCfg() :
/*N*/ 	ConfigItem( OUString::createFromAscii( CFGPATH_INPUT ) )
/*N*/ {
/*N*/ 	sal_Int32 nIntVal;
/*N*/ 
/*N*/ 	Sequence<OUString> aNames = GetPropertyNames();
/*N*/ 	Sequence<Any> aValues = GetProperties(aNames);
/*N*/ 	EnableNotification(aNames);
/*N*/ 	const Any* pValues = aValues.getConstArray();
/*N*/ 	DBG_ASSERT(aValues.getLength() == aNames.getLength(), "GetProperties failed");
/*N*/ 	if(aValues.getLength() == aNames.getLength())
/*N*/ 	{
/*N*/ 		for(int nProp = 0; nProp < aNames.getLength(); nProp++)
/*N*/ 		{
/*N*/ 			DBG_ASSERT(pValues[nProp].hasValue(), "property value missing");
/*N*/ 			if(pValues[nProp].hasValue())
/*N*/ 			{
/*N*/ 				switch(nProp)
/*N*/ 				{
/*N*/ 					case SCINPUTOPT_MOVEDIR:
/*N*/ 						if ( pValues[nProp] >>= nIntVal )
/*N*/ 							SetMoveDir( (USHORT)nIntVal );
/*N*/ 						break;
/*N*/ 					case SCINPUTOPT_MOVESEL:
/*N*/ 						SetMoveSelection( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
/*N*/ 						break;
/*N*/ 					case SCINPUTOPT_EDTEREDIT:
/*N*/ 						SetEnterEdit( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
/*N*/ 						break;
/*N*/ 					case SCINPUTOPT_EXTENDFMT:
/*N*/ 						SetExtendFormat( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
/*N*/ 						break;
/*N*/ 					case SCINPUTOPT_RANGEFIND:
/*N*/ 						SetRangeFinder( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
/*N*/ 						break;
/*N*/ 					case SCINPUTOPT_EXPANDREFS:
/*N*/ 						SetExpandRefs( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
/*N*/ 						break;
/*N*/ 					case SCINPUTOPT_MARKHEADER:
/*N*/ 						SetMarkHeader( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
/*N*/ 						break;
/*N*/ 					case SCINPUTOPT_USETABCOL:
/*N*/ 						SetUseTabCol( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
/*N*/ 						break;
/*N*/ 					case SCINPUTOPT_TEXTWYSIWYG:
/*N*/ 						SetTextWysiwyg( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
/*N*/ 						break;
/*N*/                     case SCINPUTOPT_REPLCELLSWARN:
/*N*/                         SetReplaceCellsWarn( ScUnoHelpFunctions::GetBoolFromAny( pValues[nProp] ) );
/*N*/                         break;
/*N*/ 				}
/*N*/ 			}
/*N*/ 		}
/*N*/ 	}
/*N*/ }


void ScInputCfg::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames ) {}
void ScInputCfg::Commit() {}




}
