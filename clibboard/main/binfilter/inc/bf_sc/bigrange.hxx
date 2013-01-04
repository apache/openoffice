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



#ifndef SC_BIGRANGE_HXX
#define SC_BIGRANGE_HXX

#include <bf_svtools/bf_solar.h>


#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
namespace binfilter {


static const INT32 nInt32Min = 0x80000000;
static const INT32 nInt32Max = 0x7fffffff;


class ScBigAddress
{
	INT32	nRow;
	INT32	nCol;
	INT32	nTab;

public:
			ScBigAddress() : nCol(0), nRow(0), nTab(0) {}
			ScBigAddress( INT32 nColP, INT32 nRowP, INT32 nTabP )
				: nCol( nColP ), nRow( nRowP ), nTab( nTabP ) {}
			ScBigAddress( const ScBigAddress& r )
				: nCol( r.nCol ), nRow( r.nRow ), nTab( r.nTab ) {}
			ScBigAddress( const ScAddress& r )
				: nCol( r.Col() ), nRow( r.Row() ), nTab( r.Tab() ) {}

	INT32	Col() const { return nCol; }
	INT32	Row() const { return nRow; }
	INT32	Tab() const { return nTab; }

	void	Set( INT32 nColP, INT32 nRowP, INT32 nTabP )
				{ nCol = nColP; nRow = nRowP; nTab = nTabP; }
	void	SetCol( INT32 nColP ) { nCol = nColP; }
	void	SetRow( INT32 nRowP ) { nRow = nRowP; }
	void	SetTab( INT32 nTabP ) { nTab = nTabP; }
	void	IncCol( INT32 n = 1 ) { nCol += n; }
	void	IncRow( INT32 n = 1 ) { nRow += n; }
	void	IncTab( INT32 n = 1 ) { nTab += n; }

	void	GetVars( INT32& nColP, INT32& nRowP, INT32& nTabP ) const
				{ nColP = nCol; nRowP = nRow; nTabP = nTab; }

	inline void		PutInOrder( ScBigAddress& r );
	inline BOOL		IsValid( const ScDocument* ) const;
	inline ScAddress	MakeAddress() const;

	ScBigAddress&	operator=( const ScBigAddress& r )
					{ nCol = r.nCol; nRow = r.nRow; nTab = r.nTab; return *this; }
	ScBigAddress&	operator=( const ScAddress& r )
					{ nCol = r.Col(); nRow = r.Row(); nTab = r.Tab(); return *this; }
	int				operator==( const ScBigAddress& r ) const
					{ return nCol == r.nCol && nRow == r.nRow && nTab == r.nTab; }
	int				operator!=( const ScBigAddress& r ) const
					{ return !operator==( r ); }

	friend inline SvStream& operator<< ( SvStream& rStream, const ScBigAddress& rAdr );
	friend inline SvStream& operator>> ( SvStream& rStream, ScBigAddress& rAdr );
};


inline void ScBigAddress::PutInOrder( ScBigAddress& r )
{
	INT32 nTmp;
	if ( r.nCol < nCol )
	{
		nTmp = r.nCol;
		r.nCol = nCol;
		nCol = nTmp;
	}
	if ( r.nRow < nRow )
	{
		nTmp = r.nRow;
		r.nRow = nRow;
		nRow = nTmp;
	}
	if ( r.nTab < nTab )
	{
		nTmp = r.nTab;
		r.nTab = nTab;
		nTab = nTmp;
	}
}


inline BOOL ScBigAddress::IsValid( const ScDocument* pDoc ) const
{	//! Min/Max sind ok, kennzeichnen ganze Col/Row/Tab
	return
		((0 <= nCol && nCol <= MAXCOL)
			|| nCol == nInt32Min || nCol == nInt32Max) &&
		((0 <= nRow && nRow <= MAXROW)
			|| nRow == nInt32Min || nRow == nInt32Max) &&
		((0 <= nTab && nTab < pDoc->GetTableCount())
			|| nTab == nInt32Min || nTab == nInt32Max)
		;
}


inline ScAddress ScBigAddress::MakeAddress() const
{
	USHORT nColA, nRowA, nTabA;

	if ( nCol < 0 )
		nColA = 0;
	else if ( nCol > MAXCOL )
		nColA = MAXCOL;
	else
		nColA = (USHORT) nCol;

	if ( nRow < 0 )
		nRowA = 0;
	else if ( nRow > MAXROW )
		nRowA = MAXROW;
	else
		nRowA = (USHORT) nRow;

	if ( nTab < 0 )
		nTabA = 0;
	else if ( nTab > MAXTAB )
		nTabA = MAXTAB;
	else
		nTabA = (USHORT) nTab;

	return ScAddress( nColA, nRowA, nTabA );
}


inline SvStream& operator<< ( SvStream& rStream, const ScBigAddress& rAdr )
{
	rStream << rAdr.nCol << rAdr.nRow << rAdr.nTab;
	return rStream;
}


inline SvStream& operator>> ( SvStream& rStream, ScBigAddress& rAdr )
{
	rStream >> rAdr.nCol >> rAdr.nRow >> rAdr.nTab;
	return rStream;
}


class ScBigRange
{
public:

	ScBigAddress	aStart;
	ScBigAddress	aEnd;

					ScBigRange() : aStart(), aEnd() {}
					ScBigRange( const ScBigAddress& s, const ScBigAddress& e )
						: aStart( s ), aEnd( e ) { aStart.PutInOrder( aEnd ); }
					ScBigRange( const ScBigRange& r )
						: aStart( r.aStart ), aEnd( r.aEnd ) {}
					ScBigRange( const ScRange& r )
						: aStart( r.aStart ), aEnd( r.aEnd ) {}
					ScBigRange( const ScBigAddress& r )
						: aStart( r ), aEnd( r ) {}
					ScBigRange( const ScAddress& r )
						: aStart( r ), aEnd( r ) {}
					ScBigRange( INT32 nCol, INT32 nRow, INT32 nTab )
						: aStart( nCol, nRow, nTab ), aEnd( aStart ) {}
					ScBigRange( INT32 nCol1, INT32 nRow1, INT32 nTab1,
							INT32 nCol2, INT32 nRow2, INT32 nTab2 )
						: aStart( nCol1, nRow1, nTab1 ),
						aEnd( nCol2, nRow2, nTab2 ) {}

	void	Set( INT32 nCol1, INT32 nRow1, INT32 nTab1,
					 INT32 nCol2, INT32 nRow2, INT32 nTab2 )
				{ aStart.Set( nCol1, nRow1, nTab1 );
					aEnd.Set( nCol2, nRow2, nTab2 ); }

	void	GetVars( INT32& nCol1, INT32& nRow1, INT32& nTab1,
					 INT32& nCol2, INT32& nRow2, INT32& nTab2 ) const
				{ aStart.GetVars( nCol1, nRow1, nTab1 );
					aEnd.GetVars( nCol2, nRow2, nTab2 ); }

	BOOL 	IsValid( const ScDocument* pDoc ) const
				{ return aStart.IsValid( pDoc ) && aEnd.IsValid( pDoc ); }
	inline ScRange	MakeRange() const
					{ return ScRange( aStart.MakeAddress(),
						aEnd.MakeAddress() ); }

	inline BOOL In( const ScBigAddress& ) const;	// ist Address& in Range?
	inline BOOL In( const ScBigRange& ) const;		// ist Range& in Range?
	inline BOOL Intersects( const ScBigRange& ) const;	// ueberschneiden sich zwei Ranges?

	ScBigRange&		operator=( const ScBigRange& r )
						{ aStart = r.aStart; aEnd = r.aEnd; return *this; }
	int				operator==( const ScBigRange& r ) const
						{ return (aStart == r.aStart) && (aEnd == r.aEnd); }
	int				operator!=( const ScBigRange& r ) const
						{ return !operator==( r ); }

	friend inline SvStream& operator<< ( SvStream& rStream, const ScBigRange& rRange );
	friend inline SvStream& operator>> ( SvStream& rStream, ScBigRange& rRange );
};


inline BOOL ScBigRange::In( const ScBigAddress& rAddr ) const
{
	return
		aStart.Col() <= rAddr.Col() && rAddr.Col() <= aEnd.Col() &&
		aStart.Row() <= rAddr.Row() && rAddr.Row() <= aEnd.Row() &&
		aStart.Tab() <= rAddr.Tab() && rAddr.Tab() <= aEnd.Tab();
}


inline BOOL ScBigRange::In( const ScBigRange& r ) const
{
	return
		aStart.Col() <= r.aStart.Col() && r.aEnd.Col() <= aEnd.Col() &&
		aStart.Row() <= r.aStart.Row() && r.aEnd.Row() <= aEnd.Row() &&
		aStart.Tab() <= r.aStart.Tab() && r.aEnd.Tab() <= aEnd.Tab();
}


inline BOOL ScBigRange::Intersects( const ScBigRange& r ) const
{
	return !(
		Min( aEnd.Col(), r.aEnd.Col() ) < Max( aStart.Col(), r.aStart.Col() )
	 || Min( aEnd.Row(), r.aEnd.Row() ) < Max( aStart.Row(), r.aStart.Row() )
	 || Min( aEnd.Tab(), r.aEnd.Tab() ) < Max( aStart.Tab(), r.aStart.Tab() )
		);
}


inline SvStream& operator<< ( SvStream& rStream, const ScBigRange& rRange )
{
	rStream << rRange.aStart;
	rStream << rRange.aEnd;
	return rStream;
}


inline SvStream& operator>> ( SvStream& rStream, ScBigRange& rRange )
{
	rStream >> rRange.aStart;
	rStream >> rRange.aEnd;
	return rStream;
}



} //namespace binfilter
#endif
