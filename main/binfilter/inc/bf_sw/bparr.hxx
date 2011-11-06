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



#ifndef _BPARR_HXX
#define _BPARR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif
namespace binfilter {

struct BlockInfo;
class BigPtrArray;

class BigPtrEntry
{
	friend class BigPtrArray;
	BlockInfo* pBlock;
	USHORT nOffset;
protected:
	BigPtrEntry() : pBlock(0), nOffset(0) {}
	virtual ~BigPtrEntry() {}

	inline ULONG GetPos() const;
	inline BigPtrArray& GetArray() const;
};
typedef BigPtrEntry* ElementPtr;


typedef BOOL (*FnForEach)( const ElementPtr&, void* pArgs );

// 1000 Eintr„ge pro Block = etwas weniger als 4K
#define MAXENTRY 1000


// Anzahl Eintraege, die bei der Kompression frei bleiben duerfen
// dieser Wert ist fuer den Worst Case, da wir MAXBLOCK mit ca 25%
// Overhead definiert haben, reichen 80% = 800 Eintraege vollkommen aus
// Will mann voellige Kompression haben, muss eben 100 angegeben werden.

#define	COMPRESSLVL 80

struct BlockInfo {					// Block-Info:
	BigPtrArray* pBigArr;			// in diesem Array steht der Block
	ElementPtr*	pData;				// Datenblock
	ULONG nStart, nEnd;				// Start- und EndIndex
	USHORT nElem;					// Anzahl Elemente
};

class BigPtrArray
{
	BlockInfo** ppInf;				// Block-Infos
	ULONG		nSize;				// Anzahl Elemente
	USHORT		nMaxBlock;			// akt. max Anzahl Bloecke
	USHORT		nBlock;				// Anzahl Bloecke
	USHORT		nCur;				// letzter Block

	USHORT		Index2Block( ULONG ) const;	// Blocksuche
	BlockInfo*	InsBlock( USHORT );			// Block einfuegen
	void		BlockDel( USHORT );			// es wurden Bloecke geloescht
	void		UpdIndex( USHORT );			// Indexe neu berechnen

protected:
	// fuelle alle Bloecke auf.
	// Der short gibt in Prozent an, wie voll die Bloecke werden sollen.
	// Der ReturnWert besagt, das irgendetwas "getan" wurde
	USHORT Compress( short = COMPRESSLVL );

public:
	BigPtrArray();
	~BigPtrArray();

	ULONG Count() const { return nSize; }

	void Insert( const ElementPtr& r, ULONG pos );
//	void Insert( const ElementPtr* p, ULONG n, ULONG pos );
	void Remove( ULONG pos, ULONG n = 1 );
	void Replace( ULONG pos, const ElementPtr& r);

	ElementPtr operator[]( ULONG ) const;
	void ForEach( FnForEach fn, void* pArgs = NULL )
	{
		ForEach( 0, nSize, fn, pArgs );
	}
	void ForEach( ULONG nStart, ULONG nEnd, FnForEach fn, void* pArgs = NULL );
};



inline ULONG BigPtrEntry::GetPos() const
{
	DBG_ASSERT( this == pBlock->pData[ nOffset ], "Element nicht im Block" );
	return pBlock->nStart + nOffset;
}

inline BigPtrArray& BigPtrEntry::GetArray() const
{
	return *pBlock->pBigArr;
}


} //namespace binfilter
#endif
