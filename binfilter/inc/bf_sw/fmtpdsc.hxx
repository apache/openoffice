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


#ifndef _FMTPDSC_HXX
#define _FMTPDSC_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif
class IntlWrapper;
namespace binfilter {

class SwPageDesc;
class SwHistory;
class SwPaM;


//Pagedescriptor
//Client vom SwPageDesc der durch das Attribut "beschrieben" wird.

#define	IVER_FMTPAGEDESC_NOAUTO	((USHORT)0x0001)
#define IVER_FMTPAGEDESC_LONGPAGE	((USHORT)0x0002)

class SwFmtPageDesc : public SfxPoolItem, public SwClient
{
	// diese "Doc"-Funktion ist friend, um nach dem kopieren das
	// Auto-Flag setzen zu koennen !!
	friend BOOL InsAttr( SwDoc*, const SwPaM &, const SfxItemSet&, USHORT,
						SwHistory* );
	USHORT nNumOffset;			// Seitennummer Offset
	USHORT nDescNameIdx;		// SW3-Reader: Stringpool-Index des Vorlagennamens
	SwModify* pDefinedIn;		// Verweis auf das Objekt, in dem das
								// Attribut gesetzt wurde (CntntNode/Format)

public:
	SwFmtPageDesc( const SwPageDesc *pDesc = 0 );
	SwFmtPageDesc( const SwFmtPageDesc &rCpy );
	SwFmtPageDesc &operator=( const SwFmtPageDesc &rCpy );
	~SwFmtPageDesc();

	TYPEINFO();

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;
	virtual USHORT			GetVersion( USHORT nFFVer ) const;
	virtual	BOOL        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	BOOL			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	virtual void Modify( SfxPoolItem *pOld, SfxPoolItem *pNew );

		  SwPageDesc *GetPageDesc() { return (SwPageDesc*)GetRegisteredIn(); }
	const SwPageDesc *GetPageDesc() const { return (SwPageDesc*)GetRegisteredIn(); }

	USHORT	GetNumOffset() const		{ return nNumOffset; }
	void	SetNumOffset( USHORT nNum ) { nNumOffset = nNum; }

	// erfrage/setze, wo drin das Attribut verankert ist
	inline const SwModify* GetDefinedIn() const { return pDefinedIn; }
	void ChgDefinedIn( const SwModify* pNew ) { pDefinedIn = (SwModify*)pNew; }

	// fuer den SW3-Reader:
	USHORT GetDescNameIdx() const { return nDescNameIdx; }
	void SetDescNameIdx( USHORT n ) { nDescNameIdx = n;  }
};


#if !(defined(MACOSX) && ( __GNUC__ < 3 ))
// GrP moved to gcc_outl.cxx; revisit with gcc3
inline const SwFmtPageDesc &SwAttrSet::GetPageDesc(BOOL bInP) const
	{ return (const SwFmtPageDesc&)Get( RES_PAGEDESC,bInP); }

inline const SwFmtPageDesc &SwFmt::GetPageDesc(BOOL bInP) const
	{ return aSet.GetPageDesc(bInP); }
#endif

} //namespace binfilter
#endif

