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


#ifndef _FMTFTNTX_HXX
#define _FMTFTNTX_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif
#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif
#ifndef _NUMRULE_HXX
#include <numrule.hxx>
#endif
namespace binfilter {


enum SwFtnEndPosEnum
{
	FTNEND_ATPGORDOCEND,				// at page or document end
	FTNEND_ATTXTEND,					// at end of the current text end
	FTNEND_ATTXTEND_OWNNUMSEQ,			// -""- and with own number sequence
	FTNEND_ATTXTEND_OWNNUMANDFMT,		// -""- and with onw numberformat
	FTNEND_ATTXTEND_END
};


class SwFmtFtnEndAtTxtEnd : public SfxEnumItem
{
	String 		sPrefix;
	String 		sSuffix;
	SvxNumberType aFmt;
	USHORT 	  	nOffset;

protected:
	SwFmtFtnEndAtTxtEnd( USHORT nWhich, SwFtnEndPosEnum ePos )
		: SfxEnumItem( nWhich, ePos ), nOffset( 0 )
	{}
	SwFmtFtnEndAtTxtEnd( const SwFmtFtnEndAtTxtEnd& rAttr )
		: SfxEnumItem( rAttr ), nOffset( rAttr.nOffset ), aFmt( rAttr.aFmt ),
		sPrefix( rAttr.sPrefix ), sSuffix( rAttr.sSuffix )
	{}

public:
	virtual USHORT 			GetValueCount() const{DBG_BF_ASSERT(0, "STRIP"); return 0;} //STRIP001 virtual USHORT 			GetValueCount() const;
    virtual SfxPoolItem*    Create(SvStream &, USHORT nVer ) const;
    virtual SvStream&       Store(SvStream &, USHORT nIVer) const;
	virtual USHORT			GetVersion( USHORT nFFVer ) const;

    virtual int             operator==( const SfxPoolItem& ) const;

    virtual BOOL             QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
    virtual BOOL             PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

// will be used at time??
//	void					FillVariable( SbxVariable &rVar,
//										  SfxMapUnit eCoreMetric,
//										  SfxMapUnit eUserMetric ) const;
//	virtual SfxArgumentError SetVariable( const SbxVariable &rVal,
//										  SfxMapUnit eCoreMetric,
//										  SfxMapUnit eUserMetric );

	inline BOOL IsAtEnd() const { return FTNEND_ATPGORDOCEND != GetValue(); }

    SwFmtFtnEndAtTxtEnd & operator=( const SwFmtFtnEndAtTxtEnd & rAttr );

	sal_Int16 GetNumType() const 		{ return aFmt.GetNumberingType(); }
	void SetNumType( sal_Int16 eType )	{ aFmt.SetNumberingType(eType); }

	const SvxNumberType& GetSwNumType() const 	{ return aFmt; }

	USHORT GetOffset() const 				{ return nOffset; }
	void SetOffset( USHORT nOff ) 			{ nOffset = nOff; }

	const String& GetPrefix() const 		{ return sPrefix; }
	void SetPrefix(const String& rSet)		{ sPrefix = rSet; }

	const String& GetSuffix() const 		{ return sSuffix; }
	void SetSuffix(const String& rSet)		{ sSuffix = rSet; }
};

class SwFmtFtnAtTxtEnd : public SwFmtFtnEndAtTxtEnd
{
public:
	SwFmtFtnAtTxtEnd( SwFtnEndPosEnum ePos = FTNEND_ATPGORDOCEND )
		: SwFmtFtnEndAtTxtEnd( RES_FTN_AT_TXTEND, ePos )
	{}

    virtual SfxPoolItem* Clone( SfxItemPool *pPool = 0 ) const;
};

class SwFmtEndAtTxtEnd : public SwFmtFtnEndAtTxtEnd
{
public:
	SwFmtEndAtTxtEnd( SwFtnEndPosEnum ePos = FTNEND_ATPGORDOCEND )
		: SwFmtFtnEndAtTxtEnd( RES_END_AT_TXTEND, ePos )
	{
		SetNumType( SVX_NUM_ROMAN_LOWER );
	}

    virtual SfxPoolItem* Clone( SfxItemPool *pPool = 0 ) const;
};

#if !(defined(MACOSX) && ( __GNUC__ < 3 ))
// GrP moved to gcc_outl.cxx; revisit with gcc3
inline const SwFmtFtnAtTxtEnd &SwAttrSet::GetFtnAtTxtEnd(BOOL bInP) const
	{ return (const SwFmtFtnAtTxtEnd&)Get( RES_FTN_AT_TXTEND, bInP); }
inline const SwFmtEndAtTxtEnd &SwAttrSet::GetEndAtTxtEnd(BOOL bInP) const
	{ return (const SwFmtEndAtTxtEnd&)Get( RES_END_AT_TXTEND, bInP); }


inline const SwFmtFtnAtTxtEnd &SwFmt::GetFtnAtTxtEnd(BOOL bInP) const
	{ return aSet.GetFtnAtTxtEnd(bInP); }
inline const SwFmtEndAtTxtEnd &SwFmt::GetEndAtTxtEnd(BOOL bInP) const
	{ return aSet.GetEndAtTxtEnd(bInP); }
#endif

} //namespace binfilter
#endif

