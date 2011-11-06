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



// ============================================================================

#ifndef SC_ASCIIOPT_HXX
#define SC_ASCIIOPT_HXX

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _COMBOBOX_HXX //autogen
#include <vcl/combobox.hxx>
#endif
#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
#ifndef _SVX_TXENCBOX_HXX
#include <bf_svx/txencbox.hxx>
#endif

#ifndef _SC_CSVTABLEBOX_HXX
#include "csvtablebox.hxx"
#endif
namespace binfilter {


// ============================================================================

class ScAsciiOptions
{
private:
	BOOL		bFixedLen;
	String		aFieldSeps;
	BOOL		bMergeFieldSeps;
	sal_Unicode	cTextSep;
	CharSet		eCharSet;
	BOOL		bCharSetSystem;
	long		nStartRow;
	USHORT		nInfoCount;
    xub_StrLen* pColStart;  //! TODO replace with vector
    BYTE*       pColFormat; //! TODO replace with vector

public:
					ScAsciiOptions();
					ScAsciiOptions(const ScAsciiOptions& rOpt);
					~ScAsciiOptions();



	void			ReadFromString( const String& rString );


	CharSet				GetCharSet() const		{ return eCharSet; }
	BOOL				GetCharSetSystem() const	{ return bCharSetSystem; }
	const String&		GetFieldSeps() const	{ return aFieldSeps; }
	BOOL				IsMergeSeps() const		{ return bMergeFieldSeps; }
	sal_Unicode			GetTextSep() const		{ return cTextSep; }
	BOOL				IsFixedLen() const		{ return bFixedLen; }
	USHORT				GetInfoCount() const	{ return nInfoCount; }
	const xub_StrLen*	GetColStart() const		{ return pColStart; }
	const BYTE*			GetColFormat() const	{ return pColFormat; }
	long				GetStartRow() const		{ return nStartRow; }

	void	SetCharSet( CharSet eNew )			{ eCharSet = eNew; }
	void	SetCharSetSystem( BOOL bSet )		{ bCharSetSystem = bSet; }
	void	SetFixedLen( BOOL bSet )			{ bFixedLen = bSet; }
	void	SetFieldSeps( const String& rStr )	{ aFieldSeps = rStr; }
	void	SetMergeSeps( BOOL bSet )			{ bMergeFieldSeps = bSet; }
	void	SetTextSep( sal_Unicode c )			{ cTextSep = c; }
	void	SetStartRow( long nRow)				{ nStartRow= nRow; }

};


// ============================================================================



// ============================================================================

} //namespace binfilter
#endif

