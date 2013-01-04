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


#ifndef __LOTPAR_HXX__
#define __LOTPAR_HXX__

#include "fltbase.hxx"
class SvStream; 
class String; 
namespace binfilter {

// einige Forward-Deklaratioen

class SwPaM;
class SwDoc;


enum WKTyp {
	WKUnknown = 0,	// = unbekanntes Format
	WKS,			// = 1-2-3/1, 1-2-3/1A
	WK1,			// = 1-2-3/2
	WK3,			// = 1-2-3/3
	WK4				// = 1-2-3/4
	};


class SwLotusParser : public SwFilterBase
{
	int 			bNewDoc : 1;

	WKTyp			eDateiTyp;

	// ---------------------------------------------------------------
	void Bof();					// 0x00
	void Dimensions();			// 0x06
	void Blank1();				// 0x0C
	void Integer1();			// 0x0D
	void Number1();				// 0x0E
	void Label1();				// 0x0F
	void Formula1();			// 0x10
	// ---------------------------------------------------------------
	void			PutCell( USHORT nCol, USHORT nRow, short nVal );
	void			PutCell( USHORT nCol, USHORT nRow, double fVal );
	void			PutCell( USHORT nCol, USHORT nRow, const String &rText,
							char cJusty );
	// ---------------------------------------------------------------
	void			Parse( void );
	void			Init( void );
public:
	// Parameter bReadNewDoc gibt an, ob in ein bestehendes Dokument ein-
	// gelesen wird. TRUE: in ein "neues" Dokument einfuegen.
	SwLotusParser( SwDoc& rDoc, const SwPaM & rCrsr, SvStream& pIn,
		int bReadNewDoc /*= TRUE*/, CharSet eQ  );

	~SwLotusParser();

	ULONG CallParser();
};



} //namespace binfilter
#endif
	// _EXLPAR_HXX

