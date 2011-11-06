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


#ifdef USED

#ifndef __EXCDOCUM_HXX__
#define __EXCDOCUM_HXX__

// auto strip #ifndef _STRING_HXX //autogen
// auto strip #include <tools/string.hxx>
// auto strip #endif
namespace binfilter {


// Q&D defines statt consts
#define MAX_TABS_EXCDOK 1024

class ExcDokument

// -----------------------------------------------------------------------
// beschreibt eine Excel-Tabelle
class ExcTabelle
	{
	private:
		friend ExcDokument;
		String				aName;
		USHORT				nCS;
		USHORT				nCE;
		USHORT				nRS;
		USHORT				nRE;
		ULONG				nStrPos;

		ExcTabelle( ULONG );							// mit Stream-Pos
		ExcTabelle( const String & );					// mit Name
		ExcTabelle( USHORT, USHORT, USHORT, USHORT );	// mit Range
	public:
		ExcTabelle();
		~ExcTabelle();

		void GetRange( USHORT& rCS, USHORT& rCE, USHORT& rRS, USHORT& rRE ) const
			{
			rCS = nCS;
			rCE = nCE;
			rRS = nRS;
			rRE = nRE;
			}

		ULONG GetStrPos( void ) const
			{
			return nStrPos;
			}

		const String &GetName( void ) const
			{
			return aName;
			}
	};

// -----------------------------------------------------------------------
// beschreibt ein Excel-Dokument
class ExcDokument
	{
	private:
		ExcTabelle			**ppTabellen;
		INT32				nAktGetNext;		// aktueller Index fuer GetNext
		USHORT				nAktTabIndex;		// aktuelle Tabelle in ppTabellen
	public:
		ExcDokument();
		~ExcDokument();

#ifdef USED
		USHORT				GetAnzTabs( void ) const;
		ExcTabelle			*operator[]( USHORT );
		void				NewTab( const String &rName,
								ULONG nStrPos = 0xFFFFFFFF );
		void				Set( USHORT nNewCS, USHORT nNewCE,
								USHORT nNewRS, USHORT nNewRE,
								USHORT nIndex = 0xFFFF );
		ExcTabelle			*GetNextTab( void );
		ExcTabelle			*GetFirstTab( void );
		void				Set( ULONG nStrPos, USHORT nIndex = 0xFFFF );
		void				Set( const String &rName,
								USHORT nIndex = 0xFFFF );
#endif
	};

#endif

} //namespace binfilter
#endif

