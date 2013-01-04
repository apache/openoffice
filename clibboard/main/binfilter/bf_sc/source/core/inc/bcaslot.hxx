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



#ifndef _SC_BCASLOT_HXX
#define _SC_BCASLOT_HXX

#ifndef _SFXBRDCST_HXX //autogen
#include <bf_svtools/brdcst.hxx>
#endif
#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif

#include "global.hxx"
#include "brdcst.hxx"
namespace binfilter {

class ScBroadcastArea;
class ScBroadcastAreaList;

#define BCA_INITGROWSIZE 16
typedef ScBroadcastArea* ScBroadcastAreaPtr;
SV_DECL_PTRARR_SORT( ScBroadcastAreas, ScBroadcastAreaPtr,
	BCA_INITGROWSIZE, BCA_INITGROWSIZE )//STRIP008 ;

// wird in einem nach Objekten sortierten SV_PTRARR_SORT benutzt
class ScBroadcastArea : public ScRange, public SfxBroadcaster
{
private:
	ScBroadcastArea*	pUpdateChainNext;
	USHORT				nRefCount;
	BOOL				bInUpdateChain;

public:
			ScBroadcastArea( const ScRange& rRange )
				: ScRange( rRange ), SfxBroadcaster(), pUpdateChainNext( NULL ),
				nRefCount( 0 ), bInUpdateChain( FALSE ) {}
	inline void			UpdateRange( const ScRange& rNewRange )
			{ aStart = rNewRange.aStart; aEnd = rNewRange.aEnd; }
	inline ScAddress	GetStart() const { return aStart; }
	inline ScAddress	GetEnd() const { return aEnd; }
	inline void			IncRef() { ++nRefCount; }
	inline USHORT		DecRef() { return --nRefCount; }
	inline ScBroadcastArea* GetUpdateChainNext() const { return pUpdateChainNext; }
	inline void			SetUpdateChainNext( ScBroadcastArea* p ) { pUpdateChainNext = p; }
	inline BOOL			IsInUpdateChain() const { return bInUpdateChain; }
	inline void			SetInUpdateChain( BOOL b ) { bInUpdateChain = b; }

			// zur Sortierung wird die linke obere Ecke herangezogen,
			// ist diese gleich, zaehlt auch die rechte untere Ecke
	BOOL	operator < ( const ScBroadcastArea& rArea ) const
				{ return aStart < rArea.aStart ||
					(aStart == rArea.aStart && aEnd < rArea.aEnd) ; }
	BOOL	operator ==( const ScBroadcastArea& rArea ) const
				{ return aStart == rArea.aStart && aEnd == rArea.aEnd; }
};
class ScBroadcastAreaSlotMachine;

// Sammlung von BroadcastAreas
class ScBroadcastAreaSlot
{
private:
	ScBroadcastAreas*	pBroadcastAreaTbl;
	ScBroadcastArea*	pTmpSeekBroadcastArea;			// fuer Seek_Entry
	ScDocument*			pDoc;
	ScBroadcastAreaSlotMachine* pBASM;

	USHORT				FindBroadcastArea( const ScRange& rRange ) const;
	ScBroadcastArea*	GetBroadcastArea( const ScRange& rRange ) const;

public:
						ScBroadcastAreaSlot( ScDocument* pDoc,
										ScBroadcastAreaSlotMachine* pBASM );
						~ScBroadcastAreaSlot();
	const ScBroadcastAreas&	GetBroadcastAreas() const
											{ return *pBroadcastAreaTbl; }
	void				StartListeningArea( const ScRange& rRange,
											SfxListener* pListener,
											ScBroadcastArea*& rpArea );
	void				EndListeningArea( const ScRange& rRange,
											SfxListener* pListener,
											ScBroadcastArea*& rpArea );
	BOOL				AreaBroadcast( const ScHint& rHint ) const;
		// return: mindestens ein Broadcast gewesen
    BOOL				AreaBroadcastInRange( const ScRange& rRange,
                                              const ScHint& rHint ) const;
/*N*/ 	void				DelBroadcastAreasInRange( const ScRange& rRange );
};


/*
	BroadcastAreaSlots und deren Verwaltung, einmal je Dokument

	+---+---+
	| 0 | 2 |	Anordnung Cols/Rows
	+---+---+
	| 1 | 3 |
	+---+---+
 */

class  ScBroadcastAreaSlotMachine
{
private:
	ScBroadcastAreaSlot**	ppSlots;
	ScDocument*			pDoc;
	ScBroadcastAreaList*	pBCAlwaysList;	// fuer den RC_ALWAYS Spezialbereich
	ScBroadcastArea*	pUpdateChain;
	ScBroadcastArea*	pEOUpdateChain;

	inline USHORT		ComputeSlotOffset( const ScAddress& rAddress ) const;
	void				ComputeAreaPoints( const ScRange& rRange,
											USHORT& nStart, USHORT& nEnd,
											USHORT& nRowBreak ) const;

public:
						ScBroadcastAreaSlotMachine( ScDocument* pDoc );
						~ScBroadcastAreaSlotMachine();
	void				StartListeningArea( const ScRange& rRange,
											SfxListener* pListener );
	void				EndListeningArea( const ScRange& rRange,
											SfxListener* pListener );
	BOOL				AreaBroadcast( const ScHint& rHint ) const;
		// return: mindestens ein Broadcast gewesen
    BOOL                AreaBroadcastInRange( const ScRange& rRange, const ScHint& rHint ) const;
/*N*/ 	void				DelBroadcastAreasInRange( const ScRange& rRange );
	inline ScBroadcastArea* GetUpdateChain() const { return pUpdateChain; }
	inline void SetUpdateChain( ScBroadcastArea* p ) { pUpdateChain = p; }
	inline ScBroadcastArea* GetEOUpdateChain() const { return pEOUpdateChain; }
	inline void SetEOUpdateChain( ScBroadcastArea* p ) { pEOUpdateChain = p; }
};



} //namespace binfilter
#endif
