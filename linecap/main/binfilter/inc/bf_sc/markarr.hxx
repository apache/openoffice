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



#ifndef SC_MARKARR_HXX
#define SC_MARKARR_HXX

#include <bf_svtools/bf_solar.h>
namespace binfilter {

#define SC_MARKARRAY_DELTA    4

struct ScMarkEntry
{
	USHORT			nRow;
	BOOL			bMarked;
};

class ScMarkArray
{
	USHORT			nCount;
    USHORT          nLimit;
	ScMarkEntry*	pData;

friend class ScMarkArrayIter;
friend class ScDocument;				// fuer FillInfo

public:
			ScMarkArray();
			~ScMarkArray();
	void	Reset( BOOL bMarked = FALSE );
	BOOL	GetMark( USHORT nRow ) const;
	void	SetMarkArea( USHORT nStartRow, USHORT nEndRow, BOOL bMarked );
	BOOL	IsAllMarked( USHORT nStartRow, USHORT nEndRow ) const;
	BOOL	HasOneMark( USHORT& rStartRow, USHORT& rEndRow ) const;
	BOOL	HasMarks() const;
	void	CopyMarksTo( ScMarkArray& rDestMarkArray ) const;

	BOOL 	Search( USHORT nRow, short& nIndex ) const;

	short	GetNextMarked( short nRow, BOOL bUp ) const;		// inkl. aktuelle
	USHORT	GetMarkEnd( USHORT nRow, BOOL bUp ) const;
};


class ScMarkArrayIter					// selektierte Bereiche durchgehen
{
	const ScMarkArray*	pArray;
	USHORT				nPos;
public:
				ScMarkArrayIter( const ScMarkArray* pNewArray );
				~ScMarkArrayIter();

	BOOL		Next( USHORT& rTop, USHORT& rBottom );
};



} //namespace binfilter
#endif

