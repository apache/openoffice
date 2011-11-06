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


#ifndef _EXCFNTBF_HXX
#define _EXCFNTBF_HXX

#include <bf_svtools/bf_solar.h>
class String; 
namespace binfilter {

class SvxColorItem;
class SvxFontItem;
class SvxFontHeightItem;


//------------------------------------------------------------------------

enum ExcScript {
	EXCSCR_None = 0x00,
	EXCSCR_Super = 0x01,
	EXCSCR_Sub = 0x02 };

enum ExcUnderline {
	EXCUNDER_None = 0x00,
	EXCUNDER_Single = 0x01,
	EXCUNDER_Double = 0x02,
	EXCUNDER_SingleAccount = 0x21,
	EXCUNDER_DoubleAccount = 0x22,
	EXCUNDER_Attr = 0xFF };

struct ExcFont{
	SvxFontItem				*pFont;
	SvxFontHeightItem		*pHeight;
	UINT16					nColor;
	UINT16					nWeight;
	ExcScript				eScript;
	ExcUnderline			eUnderline;
	INT16					bItalic : 1;
	INT16					bStrikeout : 1;
	INT16					bOutline : 1;
	INT16					bShadow : 1;
	};


//------------------------------------------------------------------------
class ColorBuffer
	{
	private:
		SvxColorItem		**pArray;
		SvxColorItem		*pDefault;
		UINT16				nMax;
		UINT16				nCount;
		BOOL				bAuto;
		// ----------------------------------------------------------
	public:
							ColorBuffer();
							~ColorBuffer();

#ifdef USED
		void				Reset( void );
#endif
		BOOL				NewColor( UINT16 nR, UINT16 nG, UINT16 nB );
		const SvxColorItem	*GetColor( UINT16 nIndex );

		BOOL				Auto( void ) const { return bAuto; }
	};

//------------------------------------------------------------------------
class FontBuffer
	{
	private:
		ExcFont				**ppFonts;	// Array mit Fontbeschreibungen
		UINT16				nMax;		// Groesse des Arrays
		UINT16				nCount;		// akt. Speichermarke im Array
		ExcFont				aDefaultFont;
	public:
							FontBuffer( UINT16 nNewMax = 128 );
							~FontBuffer();

		void 				NewFont( UINT16 nHeight, BYTE nAttr0,
								UINT16 nIndexCol, const String &rName );

		void				NewFont( UINT16 nHeight, BYTE nAttr0,
								BYTE nUnderline, UINT16 nIndexCol,
								UINT16 nBoldness, BYTE nFamily, BYTE nCharset,
								const String &rName );

		const ExcFont		&GetFont( UINT16 nIndex );

#ifdef USED
		void				Reset( void );
#endif
	};




} //namespace binfilter
#endif

