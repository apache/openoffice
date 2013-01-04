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



#ifndef SC_COLLECT_HXX
#define SC_COLLECT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif

#include <limits.h>
class SvStream;
namespace binfilter {

#define MAXCOLLECTIONSIZE 		16384
#define MAXDELTA				1024
#define SCPOS_INVALID			USHRT_MAX

#define SC_STRTYPE_VALUE		0
#define SC_STRTYPE_STANDARD		1

class ScDocument;

class DataObject
{
public:
							DataObject() {}
	virtual					~DataObject();
	virtual	DataObject*		Clone() const = 0;
};

class Collection : public DataObject
{
protected:
	USHORT 			nCount;
	USHORT 			nLimit;
	USHORT			nDelta;
	DataObject** 	pItems;
public:
						Collection(USHORT nLim = 4, USHORT nDel = 4);
						Collection(const Collection& rCollection);
	virtual				~Collection();

	virtual	DataObject*	Clone() const;

			void		AtFree(USHORT nIndex);
			void		Free(DataObject* pDataObject);
			void		FreeAll();

			BOOL		AtInsert(USHORT nIndex, DataObject* pDataObject);
	virtual	BOOL		Insert(DataObject* pDataObject);

			DataObject*	At(USHORT nIndex) const;
	virtual	USHORT 		IndexOf(DataObject* pDataObject) const;
			USHORT		GetCount() const { return nCount; }

			DataObject* operator[]( const USHORT nIndex) const {return At(nIndex);}
			Collection&	operator=( const Collection& rCol );
};


class SortedCollection : public Collection
{
private:
	BOOL	bDuplicates;
protected:
						// fuer StrCollection Load/Store
			void		SetDups( BOOL bVal ) { bDuplicates = bVal; }
			BOOL		IsDups() const { return bDuplicates; }
public:
						SortedCollection(USHORT nLim = 4, USHORT nDel = 4, BOOL bDup = FALSE);
						SortedCollection(const SortedCollection& rSortedCollection) :
							Collection(rSortedCollection),
							bDuplicates(rSortedCollection.bDuplicates) {}

	virtual	short		Compare(DataObject* pKey1, DataObject* pKey2) const = 0;
			BOOL		Search(DataObject* pDataObject, USHORT& rIndex) const;
	virtual	BOOL		Insert(DataObject* pDataObject);

};



//------------------------------------------------------------------------
class StrData : public DataObject
{
friend class StrCollection;
	String aStr;
public:
						StrData(const String& rStr) : aStr(rStr) {}
						StrData(const StrData& rData) :aStr(rData.aStr) {}
	virtual	DataObject*	Clone() const;
	const String&		GetString() const { return aStr; }
	// SetString nur, wenn StrData nicht in StrCollection ist! !!!
	// z.B. fuer Searcher
	void				SetString( const String& rNew ) { aStr = rNew; }
};

//------------------------------------------------------------------------


class StrCollection : public SortedCollection
{
public:
	StrCollection(USHORT nLim = 4, USHORT nDel = 4, BOOL bDup = FALSE) :
						SortedCollection	( nLim, nDel, bDup ) {}
	StrCollection(const StrCollection& rStrCollection) :
						SortedCollection	( rStrCollection ) {}

	virtual	DataObject*	Clone() const;
			StrData*	operator[]( const USHORT nIndex) const {return (StrData*)At(nIndex);}
	virtual	short		Compare(DataObject* pKey1, DataObject* pKey2) const;

/*N*/ 			void		Load( SvStream& );
/*N*/ 			void		Store( SvStream& ) const;
};

//------------------------------------------------------------------------
// TypedStrCollection: wie StrCollection, nur, dass Zahlen vor Strings
// 					   sortiert werden

class TypedStrData : public DataObject
{
public:
			TypedStrData( const String&	rStr, double nVal = 0.0,
						  USHORT nType = SC_STRTYPE_STANDARD )
				: aStrValue(rStr),
				  nValue(nVal),
				  nStrType(nType) {}

			TypedStrData( ScDocument* pDoc, USHORT nCol, USHORT nRow, USHORT nTab,
							BOOL bAllStrings );

			TypedStrData( const TypedStrData& rCpy )
				: aStrValue(rCpy.aStrValue),
				  nValue(rCpy.nValue),
				  nStrType(rCpy.nStrType) {}

	virtual	DataObject*	Clone() const;

	const String&		GetString() const { return aStrValue; }

private:
	friend class TypedStrCollection;
	friend class PivotStrCollection;

	String	aStrValue;
	double	nValue;
	USHORT	nStrType;			// 0 = Value
};

class TypedStrCollection : public SortedCollection
{
private:
	BOOL	bCaseSensitive;

public:
			TypedStrCollection( USHORT nLim = 4, USHORT nDel = 4, BOOL bDup = FALSE )
				: SortedCollection( nLim, nDel, bDup ) { bCaseSensitive = FALSE; }

			TypedStrCollection( const TypedStrCollection& rCpy )
				: SortedCollection( rCpy ) { bCaseSensitive = rCpy.bCaseSensitive; }

	virtual	short			Compare( DataObject* pKey1, DataObject* pKey2 ) const;



};

} //namespace binfilter
#endif


