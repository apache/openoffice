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


#ifndef _SW3IO_HXX
#define _SW3IO_HXX

#include <bf_svtools/bf_solar.h>

class UniString; 

namespace binfilter {
class SvStorage; 
class SvStrings; 
class SvxMacroTableDtor; 

class Sw3IoImp;
class SwPaM;
class SwDoc;

class SwgReaderOption;

#define DRAWING_STREAM_NAME 	"DrawingLayer"

class Sw3Io
{
	friend class Sw3IoImp;
	Sw3IoImp* pImp;					// Implementationsdaten

	// nur ueber diese beiden Klassen darf ein Load/Save erfolgen!!
	friend class Sw3Reader;
	friend class Sw3Writer;

	ULONG Load( SvStorage*, SwPaM* = NULL );
	ULONG Save( SwPaM* = NULL, BOOL = TRUE );
	ULONG SaveAs( SvStorage*, SwPaM* = NULL, BOOL = TRUE );

public:
	Sw3Io( SwDoc& );
   ~Sw3Io();
	Sw3IoImp* GetImp() { return pImp; }
	void  SetReadOptions( const SwgReaderOption& rOpt, BOOL bOverwrite=TRUE );

 	SvStorage* GetStorage();
	void  HandsOff();
	BOOL  SaveCompleted( SvStorage* );

 	ULONG SaveStyles();
	// Erzeugen eines eindeutigen Streamnamens im gegebenen Storage
	static UniString UniqueName( SvStorage*, const sal_Char* pPrefix );

	// Ermitteln aller Sections eines Dokuments
};

} //namespace binfilter
#endif
