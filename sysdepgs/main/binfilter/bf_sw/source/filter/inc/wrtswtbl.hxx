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



#ifndef  _WRTSWTBL_HXX
#define  _WRTSWTBL_HXX

#include <bf_svtools/bf_solar.h>
#ifndef _TOOLS_COLOR_HXX
#include <tools/color.hxx>
#endif
#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif

#ifndef _ORNTENUM_HXX
#include <orntenum.hxx>
#endif
#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif
class Color; 
namespace binfilter {


class SwTableBox;
class SwTableBoxes;
class SwTableLine;
class SwTableLines;
class SwTable;
class SwFrmFmt;
class SwHTMLTableLayout;
class SvxBrushItem;
class SvxBoxItem;
class SvxBorderLine;


//---------------------------------------------------------------------------
// 		 Code aus dem HTML-Filter fuers schreiben von Tabellen
//---------------------------------------------------------------------------

#define COLFUZZY 20
#define ROWFUZZY 20
#define COL_DFLT_WIDTH ((2*COLFUZZY)+1)
#define ROW_DFLT_HEIGHT (2*ROWFUZZY)+1


//-----------------------------------------------------------------------




//-----------------------------------------------------------------------






//-----------------------------------------------------------------------

class SwWriteTableCol
{
	USHORT nPos;						// End Position der Spalte

	USHORT nWidthOpt;

	BOOL bRelWidthOpt : 1;
	BOOL bOutWidth : 1;					// Spaltenbreite ausgeben?

public:
	BOOL bLeftBorder : 1;				// Welche Umrandungen sind da?
	BOOL bRightBorder : 1;

	SwWriteTableCol( USHORT nPosition );

	USHORT GetPos() const 						{ return nPos; }

	void SetLeftBorder( BOOL bBorder ) 			{ bLeftBorder = bBorder; }
	BOOL HasLeftBorder() const 					{ return bLeftBorder; }

	void SetRightBorder( BOOL bBorder )			{ bRightBorder = bBorder; }
	BOOL HasRightBorder() const					{ return bRightBorder; }

	void SetOutWidth( BOOL bSet ) 				{ bOutWidth = bSet; }
	BOOL GetOutWidth() const 					{ return bOutWidth; }

	inline int operator==( const SwWriteTableCol& rCol ) const;
	inline int operator<( const SwWriteTableCol& rCol ) const;

	void SetWidthOpt( USHORT nWidth, BOOL bRel )
	{
		nWidthOpt = nWidth; bRelWidthOpt = bRel;
	}
	USHORT GetWidthOpt() const					{ return nWidthOpt; }
	BOOL HasRelWidthOpt() const 				{ return bRelWidthOpt; }
};

inline int SwWriteTableCol::operator==( const SwWriteTableCol& rCol ) const
{
	// etwas Unschaerfe zulassen
	return (nPos >= rCol.nPos ? nPos - rCol.nPos
									 : rCol.nPos - nPos ) <= COLFUZZY;
}

inline int SwWriteTableCol::operator<( const SwWriteTableCol& rCol ) const
{
	// Da wir hier nur die Wahrheits-Grade 0 und 1 kennen, lassen wir lieber
	// auch nicht zu, dass x==y und x<y gleichzeitig gilt ;-)
	return nPos < rCol.nPos - COLFUZZY;
}


typedef SwWriteTableCol *SwWriteTableColPtr;
SV_DECL_PTRARR_SORT_DEL( SwWriteTableCols, SwWriteTableColPtr, 5, 5 )

//-----------------------------------------------------------------------

class SwWriteTable
{








public:
	static long GetBoxWidth( const SwTableBox *pBox );






};




} //namespace binfilter
#endif

