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



#ifndef _SFXITEMPOOL_HXX
#define _SFXITEMPOOL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef INCLUDED_LIMITS_H
#include <limits.h>
#define INCLUDED_LIMITS_H
#endif

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif

class SvStream;

namespace binfilter
{

class SfxBroadcaster;
struct SfxItemPool_Impl;

#define SFX_WHICH_MAX 4999

DBG_NAMEEX(SfxItemPool)

//====================================================================

#define SFX_ITEM_POOLABLE			0x0001
#define SFX_ITEM_NOT_POOLABLE		0x0002

#define SFX_ITEM_USERFLAG0  		0x0100
#define SFX_ITEM_USERFLAG1  		0x0200
#define SFX_ITEM_USERFLAG2  		0x0400
#define SFX_ITEM_USERFLAG3  		0x0800
#define SFX_ITEM_USERFLAG4  		0x1000
#define SFX_ITEM_USERFLAG5  		0x2000
#define SFX_ITEM_USERFLAG6  		0x4000
#define SFX_ITEM_USERFLAG7  		0x8000
#define SFX_ITEM_USERFLAG8  		0x0010
#define SFX_ITEM_USERFLAG9  		0x0020
#define SFX_ITEM_USERFLAGA  		0x0040
#define SFX_ITEM_USERFLAGB  		0x0080

//====================================================================

struct SfxItemInfo
{
	USHORT		_nSID;
	USHORT      _nFlags;
};

//====================================================================

class SfxStyleSheetIterator;
struct SfxPoolItemArray_Impl;

class  SfxItemPool

/*	[Beschreibung]

	Die von dieser Klasse abgeleiteten Klassen dienen der Bereitstellung von
	Defaults von SfxPoolItems und halten konkrete (konstante) Instanzen, die
	dann von mehreren Stellen (i.d.R. eines Dokuments) referenziert werden
	k�nnen.

	Dadurch ist jeder Wert nur einmalig gespeichert, was zu wenig Konstruktor
	und Destruktor-Aufrufen f�hrt, Vergleiche zwischen Items eines Dokuments
	beschleunigt und ein einfaches Laden und Speichern von Attributen
	bereitstellt.
*/

{
	 void readTheItems(SvStream & rStream, USHORT nCount, USHORT nVersion,
									 SfxPoolItem * pDefItem, SfxPoolItemArray_Impl ** pArr);

	UniString						aName;
	USHORT							nStart, nEnd;
	USHORT							_nFileFormatVersion;
#ifdef TF_POOLABLE
	const SfxItemInfo*				pItemInfos;
#else
	USHORT* 						pSlotIds;
#endif
	SfxItemPool_Impl*				pImp;
	SfxPoolItem**					ppStaticDefaults;
	SfxPoolItem**					ppPoolDefaults;
	SfxItemPool*					pSecondary;
	SfxItemPool*					pMaster;
	USHORT*							_pPoolRanges;
	FASTBOOL						bPersistentRefCounts;

	//---------------------------------------------------------------------
#ifndef _SFXITEMS_HXX

friend class SfxPoolWhichMap;

private:
	inline 	USHORT					GetIndex_Impl(USHORT nWhich) const;
	inline	USHORT					GetSize_Impl() const { return nEnd - nStart + 1; }

	 SvStream&						Load1_Impl( SvStream &rStream );
	 FASTBOOL						IsItemFlag_Impl( USHORT nWhich, USHORT nFlag ) const;

public:
	// fuer dflt. SfxItemSet::CTOR, setze dflt. WhichRanges
	void							FillItemIdRanges_Impl( USHORT*& pWhichRanges ) const;
	const USHORT*					GetFrozenIdRanges() const
									{ return _pPoolRanges; }
	FASTBOOL						IsVer2_Impl() const;

#endif
	//---------------------------------------------------------------------

protected:
	static inline void				SetRefCount( SfxPoolItem& rItem, ULONG n );
	static inline ULONG				AddRef( const SfxPoolItem& rItem, ULONG n = 1 );
	static inline ULONG				ReleaseRef( const SfxPoolItem& rItem, ULONG n = 1);

public:
									SfxItemPool( const SfxItemPool &rPool,
												 BOOL bCloneStaticDefaults = FALSE );
									SfxItemPool( const UniString &rName,
												 USHORT nStart, USHORT nEnd,
#ifdef TF_POOLABLE
												 const SfxItemInfo *pItemInfos,
#endif
												 SfxPoolItem **pDefaults = 0,
#ifndef TF_POOLABLE
												 USHORT *pSlotIds = 0,
#endif
												 FASTBOOL bLoadRefCounts = TRUE );
	virtual 						~SfxItemPool();

	void							SetPoolDefaultItem( const SfxPoolItem& );
	const SfxPoolItem*       		GetPoolDefaultItem( USHORT nWhich ) const;
	void                            ResetPoolDefaultItem( USHORT nWhich );

	void                            SetDefaults( SfxPoolItem **pDefaults );
	void							ReleaseDefaults( BOOL bDelete = FALSE );
	static void 					ReleaseDefaults( SfxPoolItem **pDefaults, USHORT nCount, BOOL bDelete = FALSE );

	virtual SfxMapUnit				GetMetric( USHORT nWhich ) const;
	void							SetDefaultMetric( SfxMapUnit eNewMetric );
	virtual SfxItemPresentation		GetPresentation( const SfxPoolItem&	rItem,
										SfxItemPresentation ePresentation,
										SfxMapUnit 			ePresentationMetric,
										XubString&				rText,
										const ::IntlWrapper * pIntlWrapper
										 = 0 ) const;
	virtual SfxItemPool*			Clone() const;
	UniString const &				GetName() const { return aName; }

	virtual const SfxPoolItem&		Put( const SfxPoolItem&, USHORT nWhich = 0 );
	virtual void					Remove( const SfxPoolItem& );
	virtual const SfxPoolItem&		GetDefaultItem( USHORT nWhich ) const;

	const SfxPoolItem*				LoadItem( SvStream &rStream,
											  FASTBOOL bDirect = FALSE,
											  const SfxItemPool *pRefPool = 0 );
	FASTBOOL						StoreItem( SvStream &rStream,
											   const SfxPoolItem &rItem,
											   FASTBOOL bDirect = FALSE ) const;

	USHORT							GetSurrogate(const SfxPoolItem *) const;
	const SfxPoolItem *				GetItem(USHORT nWhich, USHORT nSurrogate) const;
	USHORT							GetItemCount(USHORT nWhich) const;
	const SfxPoolItem*				LoadSurrogate(SvStream& rStream,
											USHORT &rWhich, USHORT nSlotId,
											const SfxItemPool* pRefPool = 0 );
	FASTBOOL						StoreSurrogate(SvStream& rStream,
											const SfxPoolItem *pItem ) const;

	virtual SvStream &				Load(SvStream &);
	virtual SvStream &				Store(SvStream &) const;
	int 							HasPersistentRefCounts() const {
										return bPersistentRefCounts; }
	void							LoadCompleted();

	USHORT							GetFirstWhich() const { return nStart; }
	USHORT							GetLastWhich() const { return nEnd; }
	FASTBOOL						IsInRange( USHORT nWhich ) const {
										return nWhich >= nStart &&
											   nWhich <= nEnd; }
	FASTBOOL						IsInVersionsRange( USHORT nWhich ) const;
	FASTBOOL						IsInStoringRange( USHORT nWhich ) const;
	void                			SetStoringRange( USHORT nFrom, USHORT nTo );
	void							SetSecondaryPool( SfxItemPool *pPool );
	SfxItemPool*					GetSecondaryPool() const {
										return pSecondary; }
	SfxItemPool*					GetMasterPool() const {
										return pMaster; }
	void							FreezeIdRanges();

	void							Delete();

#ifdef TF_POOLABLE
	FASTBOOL						IsItemFlag( USHORT nWhich, USHORT nFlag ) const;
	FASTBOOL                        IsItemFlag( const SfxPoolItem &rItem, USHORT nFlag ) const
									{ return IsItemFlag( rItem.Which(), nFlag ); }
	void                            SetItemInfos( const SfxItemInfo *pInfos )
									{ pItemInfos = pInfos; }
#else
	int 							HasMap() const { return 0 != pSlotIds; }
	void                            SetMap( USHORT *pNewSlotIds )
									{ pSlotIds = pNewSlotIds; }
#endif
	USHORT                          GetWhich( USHORT nSlot, BOOL bDeep = TRUE ) const;
	USHORT							GetSlotId( USHORT nWhich, BOOL bDeep = TRUE ) const;
	USHORT							GetTrueWhich( USHORT nSlot, BOOL bDeep = TRUE ) const;
	USHORT							GetTrueSlotId( USHORT nWhich, BOOL bDeep = TRUE ) const;

	void							SetVersionMap( USHORT nVer,
												   USHORT nOldStart, USHORT nOldEnd,
												   USHORT *pWhichIdTab );
	USHORT							GetNewWhich( USHORT nOldWhich ) const;
	USHORT							GetFileFormatVersion() const
									{ return _nFileFormatVersion; }
	void    						SetFileFormatVersion( USHORT nFileFormatVersion );
	USHORT							GetLoadingVersion() const;
	FASTBOOL						IsCurrentVersionLoading() const;

	static int						IsWhich(USHORT nId) {
										return nId && nId <= SFX_WHICH_MAX; }
	static int						IsSlot(USHORT nId) {
										return nId && nId > SFX_WHICH_MAX; }

	static const SfxItemPool* 		GetStoringPool();
	static void                     SetStoringPool( const SfxItemPool * );

private:
	const SfxItemPool&				operator=(const SfxItemPool &);   // n.i.!!
};

// --------------- Inline Implementierungen ------------------------------

// nur der Pool darf den Referenz-Zaehler manipulieren !!!
inline void SfxItemPool::SetRefCount( SfxPoolItem& rItem, ULONG n )
{
	rItem.SetRefCount(n);
}

// nur der Pool darf den Referenz-Zaehler manipulieren !!!
inline ULONG SfxItemPool::AddRef( const SfxPoolItem& rItem, ULONG n )
{
	return rItem.AddRef(n);
}

// nur der Pool darf den Referenz-Zaehler manipulieren !!!
inline ULONG SfxItemPool::ReleaseRef( const SfxPoolItem& rItem, ULONG n )
{
	return rItem.ReleaseRef(n);
}

}

#endif
