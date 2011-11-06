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


#ifndef _FMTHBSH_HXX
#define _FMTHBSH_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
namespace binfilter {

// ATT_SOFTHYPH ******************************
// Attribut fuer benutzerdefinierte Trennstellen.

class SwFmtSoftHyph : public SfxPoolItem
{
public:
	SwFmtSoftHyph();

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int operator==( const SfxPoolItem& ) const{DBG_BF_ASSERT(0, "STRIP"); return 0;}; //STRIP001 	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const{DBG_BF_ASSERT(0, "STRIP");return NULL; }; //STRIP001 	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;

	inline SwFmtSoftHyph& operator=(const SwFmtSoftHyph& rSH) {
			return *this;
		}
};

// ATT_HARDBLANK ******************************
// Attribut fuer geschuetzte Leerzeichen.

class SwFmtHardBlank : public SfxPoolItem
{
	sal_Unicode cChar;
public:
	SwFmtHardBlank( sal_Unicode cCh, BOOL bCheck = TRUE );

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int             operator==( const SfxPoolItem& ) const{DBG_BF_ASSERT(0, "STRIP"); return 0;}; //STRIP001 	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const{DBG_BF_ASSERT(0, "STRIP"); return NULL;}; //STRIP001 	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;

	virtual USHORT			 GetVersion( USHORT nFFVer ) const;

	inline sal_Unicode GetChar() const { return cChar; }
	inline SwFmtHardBlank& operator=(const SwFmtHardBlank& rHB)
	{ cChar = rHB.GetChar(); return *this; }
};


} //namespace binfilter
#endif

