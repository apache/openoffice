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


#ifndef _ERRHDL_HXX
#define _ERRHDL_HXX

#include <bf_svtools/bf_solar.h>

#ifdef DBG_UTIL

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

namespace binfilter {
// -----------------------------------------------------------------------
// Ausgabe einer Fehlermeldung inkl. Dateiname und Zeilennummer
// wo der Fehler auftrat.
// Die Funktion darf nicht direkt benutzt werden!
// -----------------------------------------------------------------------
extern void AssertFail( const sal_Char*, const sal_Char*, USHORT );
extern void AssertFail( USHORT, const sal_Char*, USHORT );

extern BOOL bAssert;				// TRUE, wenn eine ASSERT-Box hochkam
#ifdef WIN
//MSC (7.0) machts nur mit static, ZTC in inlines nur ohne...
#define ASSERT( cond, message ) \
	if( !(cond) ) { \
		static const char __FAR_DATA _pErrorText[] = #message; \
		static const char __FAR_DATA _pFileName[]  = __FILE__; \
		::binfilter::AssertFail( _pErrorText, _pFileName, __LINE__ ); \
	}
#else
#define ASSERT( cond, message ) \
	if( !(cond) ) { \
		const char	 *_pErrorText = #message; \
		const char	 *_pFileName  = __FILE__; \
	   ::binfilter::AssertFail( _pErrorText, _pFileName, __LINE__ ); \
	}
#endif

// -----------------------------------------------------------------------
// Prueft ob die angegebene Bedingung wahr ist, wenn nicht wird eine
// Fehlermeldung die ueber die ID Identifiziert wird, ausgegeben.
// -----------------------------------------------------------------------
#ifdef WIN
//MSC (7.0) machts nur mit static, ZTC in inlines nur ohne...
#define ASSERT_ID( cond, id )	\
	if( !(cond) ) { \
		static const char __FAR_DATA _pFileName[]  = __FILE__;	\
		::binfilter::AssertFail( (USHORT)id, _pFileName, __LINE__ );	\
	}
#else
#define ASSERT_ID( cond, id ) \
	if( !(cond) ) { \
		const char	 *_pFileName  = __FILE__; \
	   ::binfilter::AssertFail( (USHORT)id, _pFileName, __LINE__ ); \
	}
#endif
} //namespace binfilter

// -----------------------------------------------------------------------
// Beim Bilden der Produktversion werden alle Debug-Utilities automatisch
// ignoriert
// -----------------------------------------------------------------------
#else // PRODUCT

namespace binfilter {
#define ASSERT( cond, message ) 	;
#define ASSERT_ID( cond, id )		;
} //namespace binfilter

#endif // PRODUCT

#endif
