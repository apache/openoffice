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


#ifndef _SVX_RULRITEM_HXX
#define _SVX_RULRITEM_HXX

#include <bf_svtools/bf_solar.h>

// include ---------------------------------------------------------------

#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif
#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
namespace binfilter {

// class SvxLongLRSpaceItem ----------------------------------------------

class SvxLongLRSpaceItem : public SfxPoolItem
{
	long	lLeft;         // nLeft oder der neg. Erstzeileneinzug
	long	lRight;        // der unproblematische rechte Rand

  protected:

	virtual int 			 operator==( const SfxPoolItem& ) const;


	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;

private:
	const SvxLongLRSpaceItem& operator=(const SvxLongLRSpaceItem &); // n.i.

public:
	TYPEINFO();
	SvxLongLRSpaceItem(long lLeft, long lRight, USHORT nId);
	SvxLongLRSpaceItem(const SvxLongLRSpaceItem &);

	long    GetLeft() const { return lLeft; }
	long    GetRight() const { return lRight; }
	void    SetLeft(long lArgLeft) {lLeft=lArgLeft;}
	void    SetRight(long lArgRight) {lRight=lArgRight;}
};

// class SvxLongULSpaceItem ----------------------------------------------

class SvxLongULSpaceItem : public SfxPoolItem
{
	long	lLeft;         // nLeft oder der neg. Erstzeileneinzug
	long	lRight;        // der unproblematische rechte Rand

  protected:

	virtual int 			 operator==( const SfxPoolItem& ) const;


	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;

private:
	const SvxLongULSpaceItem& operator=(const SvxLongULSpaceItem &); // n.i.

public:
	TYPEINFO();
	SvxLongULSpaceItem(long lUpper, long lLower, USHORT nId);
	SvxLongULSpaceItem(const SvxLongULSpaceItem &);

	long    GetUpper() const { return lLeft; }
	long    GetLower() const { return lRight; }
	void    SetUpper(long lArgLeft) {lLeft=lArgLeft;}
	void    SetLower(long lArgRight) {lRight=lArgRight;}
};

// class SvxPagePosSizeItem ----------------------------------------------

class SvxPagePosSizeItem : public SfxPoolItem
{
	Point aPos;
	long lWidth;
	long lHeight;
protected:
	virtual int 			 operator==( const SfxPoolItem& ) const;


	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;

private:
	const SvxPagePosSizeItem& operator=(const SvxPagePosSizeItem &); // n.i.
public:
	TYPEINFO();
	SvxPagePosSizeItem(const Point &rPos, long lWidth, long lHeight);
	SvxPagePosSizeItem(const SvxPagePosSizeItem &);

	const Point &GetPos() const { return aPos; }
	long    GetWidth() const { return lWidth; }
	long    GetHeight() const { return lHeight; }
};

// struct SvxColumnDescription -------------------------------------------

struct SvxColumnDescription
{
	USHORT nStart;					   /* Spaltenbeginn */
	USHORT nEnd;					   /* Spaltenende */
	BOOL   bVisible;				   /* Sichtbarkeit */
	SvxColumnDescription():
		nStart(0), nEnd(0), bVisible(TRUE) {}
	SvxColumnDescription(const SvxColumnDescription &rCopy) :
		nStart(rCopy.nStart), nEnd(rCopy.nEnd), bVisible(rCopy.bVisible) {}
	SvxColumnDescription(USHORT start, USHORT end, BOOL bVis = TRUE):
		nStart(start), nEnd(end), bVisible(bVis) {}
	int operator==(const SvxColumnDescription &rCmp) const {
		return nStart == rCmp.nStart &&
			bVisible == rCmp.bVisible &&
			nEnd == rCmp.nEnd;
	}
	int operator!=(const SvxColumnDescription &rCmp) const {
		return !operator==(rCmp);
	}
	USHORT GetWidth() const { return nEnd - nStart; }
};

// class SvxColumnItem ---------------------------------------------------

typedef SvPtrarr SvxColumns;

class SvxColumnItem : public SfxPoolItem
{
	SvxColumns aColumns;// Spaltenarray
	long	nLeft,		// Linker Rand bei Tabelle
		   nRight;		// Rechter Rand bei Tabelle; bei Spalten immer gleich
						// zum umgebenden Rahmen
	USHORT nActColumn;	// die aktuelle Spalte
	BOOL    bTable;		// Tabelle?
	BOOL	bOrtho;     // Gleichverteilte Spalten

	void DeleteAndDestroyColumns();

protected:
	virtual int 			 operator==( const SfxPoolItem& ) const;


	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;
public:
	TYPEINFO();
	// rechter Rand des umgebenden Rahmens
	// nLeft, nRight jeweils der Abstand zum umgebenden Rahmen
	SvxColumnItem(USHORT nAct = 0); // Spalten
	SvxColumnItem(const	SvxColumnItem &);
	~SvxColumnItem();


	USHORT Count() const { return aColumns.Count(); }
	SvxColumnDescription &operator[](USHORT i)
		{ return *(SvxColumnDescription*)aColumns[i]; }
	const SvxColumnDescription &operator[](USHORT i) const
		{ return *(SvxColumnDescription*)aColumns[i]; }
	void Insert(const SvxColumnDescription &rDesc, USHORT nPos) {
		SvxColumnDescription* pDesc = new SvxColumnDescription(rDesc);
		aColumns.Insert(pDesc, nPos);
	}
	void   Append(const SvxColumnDescription &rDesc) { Insert(rDesc, Count()); }
	void   SetLeft(long left) { nLeft = left; }
	void   SetRight(long right) { nRight = right; }
	void   SetActColumn(USHORT nCol) { nActColumn = nCol; }

	USHORT GetActColumn() const { return nActColumn; }
	BOOL   IsFirstAct() const { return nActColumn == 0; }
	BOOL   IsLastAct() const { return nActColumn == Count()-1; }
	long GetLeft() { return nLeft; }
	long GetRight() { return nRight; }

	BOOL   IsTable() const { return bTable; }

	void   SetOrtho(BOOL bVal) { bOrtho = bVal; }
	BOOL   IsOrtho () const { return FALSE ; }

	BOOL IsConsistent() const  { return nActColumn < aColumns.Count(); }
};

// class SvxObjectItem ---------------------------------------------------

class SvxObjectItem : public SfxPoolItem
{
private:
	long   nStartX;					   /* Beginn in X-Richtung */
	long   nEndX;					   /* Ende in X-Richtung */
	long   nStartY;                    /* Beginn in Y-Richtung */
	long   nEndY;                      /* Ende in Y-Richtung */
	BOOL   bLimits;					   /* Grenzwertkontrolle durch die Applikation */
protected:


private:
public:
	TYPEINFO();
	SvxObjectItem(long nStartX, long nEndX,
				  long nStartY, long nEndY,
				  BOOL bLimits = FALSE);



};

}//end of namespace binfilter
#endif

