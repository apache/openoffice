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


#ifndef _FMTFLD_HXX
#define _FMTFLD_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _CALBCK_HXX //autogen
#include <calbck.hxx>
#endif
namespace binfilter {

class SwField;
class SwTxtFld;
// ATT_FLD ***********************************

class SwFmtFld : public SfxPoolItem, public SwClient
{
	friend class SwTxtFld;
	friend void _InitCore();

	SwField *pField;
	SwTxtFld* pTxtAttr;		// mein TextAttribut
	SwFmtFld();				// das default-Attibut

	// geschuetzter CopyCtor
	SwFmtFld& operator=(const SwFmtFld& rFld);
public:
	TYPEINFO();

	SwFmtFld( const SwField &rFld );
	SwFmtFld( const SwFmtFld& rAttr );
	~SwFmtFld();

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;

	virtual void Modify( SfxPoolItem* pOld, SfxPoolItem* pNew );
	virtual BOOL GetInfo( SfxPoolItem& rInfo ) const;

	const SwField *GetFld() const	{ return pField; }
	SwField *GetFld()				{ return pField; }

	const SwTxtFld *GetTxtFld() const	{ return pTxtAttr; }
	SwTxtFld *GetTxtFld()				{ return pTxtAttr; }

	BOOL IsFldInDoc() const;
    BOOL IsProtect() const;
};


} //namespace binfilter
#endif

