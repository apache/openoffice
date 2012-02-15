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


#ifndef _FMTORNT_HXX
#define _FMTORNT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SWTYPES_HXX //autogen
#include <swtypes.hxx>
#endif
#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif
#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif
#ifndef _ORNTENUM_HXX
#include <orntenum.hxx>
#endif
class IntlWrapper;
namespace binfilter {



#define IVER_VERTORIENT_REL ((USHORT)0x0001)

class SwFmtVertOrient: public SfxPoolItem
{
	SwTwips			 nYPos;	//Enthaelt _immer_ die aktuelle RelPos.
	SwVertOrient	 eOrient;
	SwRelationOrient eRelation;
public:
	TYPEINFO();
	SwFmtVertOrient( SwTwips nY = 0, SwVertOrient eVert = VERT_NONE,
					 SwRelationOrient eRel = PRTAREA );
	inline SwFmtVertOrient &operator=( const SwFmtVertOrient &rCpy );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;
	virtual USHORT			GetVersion( USHORT nFFVer ) const;
	virtual	BOOL        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	BOOL			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	SwVertOrient GetVertOrient() const { return eOrient; }
	SwRelationOrient GetRelationOrient() const { return eRelation; }
	void   SetVertOrient( SwVertOrient eNew ) { eOrient = eNew; }
	void   SetRelationOrient( SwRelationOrient eNew ) { eRelation = eNew; }

	SwTwips GetPos() const { return nYPos; }
	void    SetPos( SwTwips nNew ) { nYPos = nNew; }

	SwTwips	GetPosConvertedToSw31( const SvxULSpaceItem *pULSpace ) const;
	SwTwips	GetPosConvertedFromSw31( const SvxULSpaceItem *pULSpace ) const;
};

//SwFmtHoriOrient, wie und woran orientiert --
//	sich der FlyFrm in der Hoizontalen ----------

#define IVER_HORIORIENT_TOGGLE ((USHORT)0x0001)
#define IVER_HORIORIENT_REL ((USHORT)0x0002)

class SwFmtHoriOrient: public SfxPoolItem
{
	SwTwips			 nXPos;	//Enthaelt _immer_ die aktuelle RelPos.
	SwHoriOrient	 eOrient;
	SwRelationOrient eRelation;
	BOOL			 bPosToggle : 1; // auf geraden Seiten Position spiegeln
public:
	TYPEINFO();
	SwFmtHoriOrient( SwTwips nX = 0, SwHoriOrient eHori = HORI_NONE,
		SwRelationOrient eRel = PRTAREA, BOOL bPos = FALSE );
	inline SwFmtHoriOrient &operator=( const SwFmtHoriOrient &rCpy );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;
	virtual USHORT			GetVersion( USHORT nFFVer ) const;
	virtual	BOOL        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	BOOL			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	SwHoriOrient GetHoriOrient() const { return eOrient; }
	SwRelationOrient GetRelationOrient() const { return eRelation; }
	void SetHoriOrient( SwHoriOrient eNew ) { eOrient = eNew; }
	void SetRelationOrient( SwRelationOrient eNew ) { eRelation = eNew; }

	SwTwips GetPos() const { return nXPos; }
	void    SetPos( SwTwips nNew ) { nXPos = nNew; }

	SwTwips	GetPosConvertedToSw31( const SvxLRSpaceItem *pLRSpace ) const;
	SwTwips	GetPosConvertedFromSw31( const SvxLRSpaceItem *pLRSpace ) const;

	BOOL IsPosToggle() const { return bPosToggle; }
	void SetPosToggle( BOOL bNew ) { bPosToggle = bNew; }
};

inline SwFmtVertOrient &SwFmtVertOrient::operator=( const SwFmtVertOrient &rCpy )
{
	nYPos = rCpy.GetPos();
	eOrient = rCpy.GetVertOrient();
	eRelation = rCpy.GetRelationOrient();
	return *this;
}
inline SwFmtHoriOrient &SwFmtHoriOrient::operator=( const SwFmtHoriOrient &rCpy )
{
	nXPos = rCpy.GetPos();
	eOrient = rCpy.GetHoriOrient();
	eRelation = rCpy.GetRelationOrient();
	bPosToggle = rCpy.IsPosToggle();
	return *this;
}

#if !(defined(MACOSX) && ( __GNUC__ < 3 ))
// GrP moved to gcc_outl.cxx
inline const SwFmtVertOrient &SwAttrSet::GetVertOrient(BOOL bInP) const
	{ return (const SwFmtVertOrient&)Get( RES_VERT_ORIENT,bInP); }
inline const SwFmtHoriOrient &SwAttrSet::GetHoriOrient(BOOL bInP) const
	{ return (const SwFmtHoriOrient&)Get( RES_HORI_ORIENT,bInP); }

inline const SwFmtVertOrient &SwFmt::GetVertOrient(BOOL bInP) const
	{ return aSet.GetVertOrient(bInP); }
inline const SwFmtHoriOrient &SwFmt::GetHoriOrient(BOOL bInP) const
	{ return aSet.GetHoriOrient(bInP); }
#endif

} //namespace binfilter
#endif

