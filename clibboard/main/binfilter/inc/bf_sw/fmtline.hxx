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


#ifndef SW_FMTLINE_HXX
#define SW_FMTLINE_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif
class IntlWrapper; 
namespace binfilter {


class SwFmtLineNumber: public SfxPoolItem
{
	ULONG nStartValue	:24; //Startwert fuer den Absatz, 0 == kein Startwert
	ULONG bCountLines	:1;	 //Zeilen des Absatzes sollen mitgezaehlt werden.

public:
	SwFmtLineNumber();
	~SwFmtLineNumber();

    TYPEINFO();

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;
	virtual USHORT			 GetVersion( USHORT nFFVer ) const;
	virtual	BOOL        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	BOOL			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	ULONG GetStartValue() const { return nStartValue; }
	BOOL  IsCount()		  const { return bCountLines != 0; }

	void SetStartValue( ULONG nNew ) { nStartValue = nNew; }
	void SetCountLines( BOOL b )     { bCountLines = b;	   }
};

#if !(defined(MACOSX) && ( __GNUC__ < 3 ))
// GrP moved to gcc_outl.cxx; revisit with gcc3
inline const SwFmtLineNumber &SwAttrSet::GetLineNumber(BOOL bInP) const
	{ return (const SwFmtLineNumber&)Get( RES_LINENUMBER,bInP); }

inline const SwFmtLineNumber &SwFmt::GetLineNumber(BOOL bInP) const
	{ return aSet.GetLineNumber(bInP); }
#endif

} //namespace binfilter
#endif

