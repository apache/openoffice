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


#ifndef _EXCVFBFF_HXX
#define _EXCVFBFF_HXX

#include <bf_svtools/bf_solar.h>
class String;
namespace binfilter {



class ValueFormBuffer;

#if defined(BLC) || defined(MAC)
// BLC will es auf jeden Fall hier
typedef void				( ValueFormBuffer::*Fkt_rString )( String & );
typedef sal_uInt32          ( ValueFormBuffer::*Fkt_USHORT )( USHORT );
#endif

class ValueFormBuffer
	{
	private:
#if !defined(BLC) && !defined(MAC)
// MSC will es auf jeden Fall hier
		typedef void		( ValueFormBuffer::*Fkt_rString )( String & );
        typedef sal_uInt32  ( ValueFormBuffer::*Fkt_USHORT )( USHORT );
#endif

        sal_uInt32          *pHandles;      // Array mit Handles...
		USHORT				nMax;  			// Groesse des Arrays
		USHORT				nCount;			// Index des naechsten freien Eintrags
        sal_uInt32          nDefaultHandle;

		// nur fuer Excel5
		static const USHORT nAnzBuiltin;	// bekannte Formate
		static const USHORT nNewFormats;	// selbstdefinierte
		static const sal_Char	*pBuiltinFormats[];	// vordefinierte Formate

		Fkt_rString			_NewValueFormat;
		Fkt_USHORT			_GetValueFormat;
		// ----------------------------------------------------------
		void				Init( void );
							// fuer 1. Nutzung
		void				__NewValueFormat( String &rFormString );
        sal_uInt32          __GetValueFormat( USHORT nExcIndex );
							// fuer n-te Nutzung
		void				_NewValueFormatX( String &rFormString );
		void				_NewValueFormat5( String &rFormString );
        sal_uInt32          _GetValueFormatX5( USHORT nExcIndex );
	public:
		ValueFormBuffer( const USHORT nSize = 2048 );
		~ValueFormBuffer();

		inline void			NewValueFormat( String &rFormString );
		inline ULONG		GetValueFormat( USHORT nExcIndex );
#ifdef USED
		void				Reset( void );
#endif
	};

inline void ValueFormBuffer::NewValueFormat( String &rFormString )
	{
	( this->*_NewValueFormat )( rFormString );
	}

inline ULONG ValueFormBuffer::GetValueFormat( USHORT nExcIndex )
	{
	return ( this->*_GetValueFormat )( nExcIndex );
	}


} //namespace binfilter
#endif

