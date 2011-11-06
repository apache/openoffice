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


#ifndef _FCHRFMT_HXX
#define _FCHRFMT_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif
class IntlWrapper; 
namespace binfilter {

class SwCharFmt;
class SwTxtCharFmt;


// ATT_CHARFMT *********************************************


class SwFmtCharFmt: public SfxPoolItem, public SwClient
{
	friend class SwTxtCharFmt;
	SwTxtCharFmt* pTxtAttr;		// mein TextAttribut
public:
	SwFmtCharFmt( SwCharFmt *pFmt );
	SwFmtCharFmt( const SwFmtCharFmt& rAttr );
	~SwFmtCharFmt();        // fuer SEXPORT
	TYPEINFO();

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer ) const;
	virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
									SfxMapUnit eCoreMetric,
									SfxMapUnit ePresMetric,
									String &rText,
                                    const ::IntlWrapper*    pIntl = 0 ) const;

	virtual	BOOL        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;

	// an das SwTxtCharFmt weiterleiten (vom SwClient)
	virtual BOOL 	GetInfo( SfxPoolItem& rInfo ) const;

	void SetCharFmt( SwFmt* pFmt ) { pFmt->Add(this); }
	SwCharFmt* GetCharFmt() const { return (SwCharFmt*)GetRegisteredIn(); }
};
} //namespace binfilter
#endif

