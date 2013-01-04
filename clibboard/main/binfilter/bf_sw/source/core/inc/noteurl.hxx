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



#ifndef	_NOTEURL_HXX
#define	_NOTEURL_HXX


#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _SWRECT_HXX
#include "swrect.hxx"
#endif
class MapMode;

namespace binfilter {

class ImageMap;

class SwURLNote
{
	String aURL;
	String aTarget;
	SwRect aRect;
public:
	SwURLNote( const String& rURL, const String& rTarget, const SwRect& rRect )
	: aURL( rURL ), aTarget( rTarget ), aRect( rRect )
	{}
	const String& GetURL() 		const { return aURL; }
	const String& GetTarget() 	const { return aTarget; }
	const SwRect& GetRect() 	const { return aRect; }
	BOOL operator==( const SwURLNote& rSwURLNote ) const
	{ return aRect == rSwURLNote.aRect; }
};

typedef SwURLNote* SwURLNotePtr;
SV_DECL_PTRARR_DEL(SwURLNoteList, SwURLNotePtr, 0, 5)

class SwNoteURL
{
	SwURLNoteList aList;
public:
	SwNoteURL() {}
	USHORT Count() const { return aList.Count(); }
	const SwURLNote& GetURLNote( USHORT nPos ) const
		{ return *aList.GetObject( nPos ); }
};

// globale Variable, in NoteURL.Cxx angelegt
extern SwNoteURL *pNoteURL;


} //namespace binfilter
#endif

