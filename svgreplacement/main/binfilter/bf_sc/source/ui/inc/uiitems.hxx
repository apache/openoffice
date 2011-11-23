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



#ifndef SC_UIITEMS_HXX
#define SC_UIITEMS_HXX

#ifndef SC_CONDITIO_HXX
#include "conditio.hxx"
#endif
#ifndef SC_SORTPARAM_HXX
#include "sortparam.hxx"
#endif

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
namespace binfilter {

class ScEditEngineDefaulter;
class EditTextObject;
class ScViewData;
class ScDPSaveData;

// ---------------------------------------------------------------------------

//  Items



#define SC_TAB_INSERTED		1
#define SC_TAB_DELETED		2
#define SC_TAB_MOVED		3
#define SC_TAB_COPIED		4
#define SC_TAB_HIDDEN		5

class ScTablesHint : public SfxHint
{
	USHORT nId;
	USHORT nTab1;
	USHORT nTab2;

public:
					TYPEINFO();
					ScTablesHint(USHORT nNewId, USHORT nTable1, USHORT nTable2=0);
					~ScTablesHint();

	USHORT			GetId() const			{ return nId; }
	USHORT			GetTab1() const			{ return nTab1; }
	USHORT			GetTab2() const			{ return nTab2; }
};

class ScEditViewHint : public SfxHint
{
	ScEditEngineDefaulter*	pEditEngine;
	ScAddress       			aCursorPos;

public:
					TYPEINFO();
					~ScEditViewHint();

	USHORT          GetCol() const      { return aCursorPos.Col(); }
	USHORT          GetRow() const      { return aCursorPos.Row(); }
	USHORT          GetTab() const      { return aCursorPos.Tab(); }
	ScEditEngineDefaulter*	GetEngine() const   { return pEditEngine; }
};

class ScIndexHint : public SfxHint
{
	USHORT nId;
	USHORT nIndex;

public:
					TYPEINFO();
					~ScIndexHint();

	USHORT			GetId() const			{ return nId; }
	USHORT			GetIndex() const		{ return nIndex; }
};

//----------------------------------------------------------------------------
// Parameter-Item fuer den Sortierdialog:

class ScSortItem : public SfxPoolItem
{
public:
							TYPEINFO();
							ScSortItem( USHORT				nWhich,
										const ScSortParam*	pSortData );
							ScSortItem( const ScSortItem& rItem );
							~ScSortItem();

	virtual int             operator==( const SfxPoolItem& ) const{DBG_BF_ASSERT(0, "STRIP");return 0;}; //STRIP001 virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const{DBG_BF_ASSERT(0, "STRIP");return NULL;}; //STRIP001 virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;

	ScViewData*			GetViewData () const { return pViewData; }

private:
	ScViewData* 	pViewData;
	ScSortParam		theSortData;
};

//----------------------------------------------------------------------------
// Parameter-Item fuer den Filterdialog:

class ScQueryItem : public SfxPoolItem
{
public:
							TYPEINFO();
							ScQueryItem( USHORT					nWhich,
										 ScViewData*			ptrViewData,
										 const ScQueryParam*	pQueryData );
							ScQueryItem( const ScQueryItem& rItem );
							~ScQueryItem();

	virtual int             operator==( const SfxPoolItem& ) const{DBG_BF_ASSERT(0, "STRIP");return 0;}; //STRIP001 virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const{DBG_BF_ASSERT(0, "STRIP"); return NULL;}; //STRIP001 virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;

 	ScViewData*			GetViewData () const { return pViewData; }


private:
	ScViewData* 	pViewData;
	ScQueryParam	theQueryData;
	BOOL			bIsAdvanced;
	ScRange			aAdvSource;
};

//----------------------------------------------------------------------------
// Parameter-Item fuer den Zwischenergebnisdialog:

class ScSubTotalItem : public SfxPoolItem
{
public:
				TYPEINFO();
				ScSubTotalItem( USHORT					nWhich,
								ScViewData*				ptrViewData,
								const ScSubTotalParam*	pSubTotalData );
				ScSubTotalItem( const ScSubTotalItem&	rItem );
				~ScSubTotalItem();

	virtual int             operator==( const SfxPoolItem& ) const{DBG_BF_ASSERT(0, "STRIP"); return 0;}; //STRIP001 virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const{DBG_BF_ASSERT(0, "STRIP"); return NULL;}; //STRIP001 virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;

	ScViewData*				GetViewData () const { return pViewData; }
	const ScSubTotalParam&	GetSubTotalData() const { return theSubTotalData; }

private:
	ScViewData* 	pViewData;
	ScSubTotalParam	theSubTotalData;
};

//----------------------------------------------------------------------------
// Parameter-Item fuer die Benutzerlisten-TabPage:

class ScUserListItem : public SfxPoolItem
{
public:
				TYPEINFO();
				ScUserListItem( USHORT nWhich );
				ScUserListItem( const ScUserListItem& rItem );
				~ScUserListItem();

	virtual int             operator==( const SfxPoolItem& ) const{DBG_BF_ASSERT(0, "STRIP");return 0;}; //STRIP001 virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const{DBG_BF_ASSERT(0, "STRIP"); return NULL;}; //STRIP001 virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;


private:
	ScUserList*	pUserList;
};

//----------------------------------------------------------------------------
// Parameter-Item fuer die Konsolidieren-Dialog

class ScConsolidateItem : public SfxPoolItem
{
public:
				TYPEINFO();
				ScConsolidateItem( USHORT					 nWhich,
								   const ScConsolidateParam* pParam );
				ScConsolidateItem( const ScConsolidateItem&	rItem );
				~ScConsolidateItem();

	virtual int             operator==( const SfxPoolItem& ) const{DBG_BF_ASSERT(0, "STRIP"); return 0;}; //STRIP001 virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const{DBG_BF_ASSERT(0, "STRIP"); return NULL;}; //STRIP001 virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;


private:
	ScConsolidateParam	theConsData;
};

//----------------------------------------------------------------------------
// Parameter-Item fuer den Pivot-Dialog

class ScPivotItem : public SfxPoolItem
{
public:
				TYPEINFO();
				ScPivotItem( USHORT nWhich, const ScDPSaveData* pData,
							 const ScRange* pRange, BOOL bNew );
				ScPivotItem( const ScPivotItem&	rItem );
				~ScPivotItem();

	virtual int             operator==( const SfxPoolItem& ) const{DBG_BF_ASSERT(0, "STRIP");return 0;}; //STRIP001 virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const{DBG_BF_ASSERT(0, "STRIP");return NULL;}; //STRIP001 virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;


private:
	ScDPSaveData*	pSaveData;
	ScRange			aDestRange;
	BOOL			bNewSheet;
};

//----------------------------------------------------------------------------
// Parameter-Item fuer den Solver-Dialog

class ScSolveItem : public SfxPoolItem
{
public:
				TYPEINFO();
				ScSolveItem( USHORT				 nWhich,
							 const ScSolveParam* pParam );
				ScSolveItem( const ScSolveItem&	rItem );
				~ScSolveItem();

	virtual int             operator==( const SfxPoolItem& ) const{DBG_BF_ASSERT(0, "STRIP");return 0;}; //STRIP001 virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const{DBG_BF_ASSERT(0, "STRIP"); return NULL;}; //STRIP001 virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;


private:
	ScSolveParam	theSolveData;
};

//----------------------------------------------------------------------------
// Parameter-Item fuer den Mehrfachoperationen-Dialog


//----------------------------------------------------------------------------
// Parameter-Item fuer den Dialog bedingte Formatierung

class ScCondFrmtItem : public SfxPoolItem
{
public:
				TYPEINFO();
				ScCondFrmtItem( USHORT nWhich,
//!								const ScConditionalFormat* pCondFrmt );
								const ScConditionalFormat& rCondFrmt );
				ScCondFrmtItem( const ScCondFrmtItem& rItem );
				~ScCondFrmtItem();

	virtual String          GetValueText() const;
	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;

	const ScConditionalFormat&	GetData() const { return theCondFrmtData; }

private:
	ScConditionalFormat	theCondFrmtData;
};



} //namespace binfilter
#endif

