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


#ifndef _FMTRUBY_HXX
#define _FMTRUBY_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif
namespace binfilter {

class SwTxtRuby;

class SwFmtRuby : public SfxPoolItem
{
	friend class SwTxtRuby;

	String sRubyTxt;					// the ruby txt
	String sCharFmtName;				// name of the charformat
	SwTxtRuby* pTxtAttr;				// the TextAttribut
	USHORT nCharFmtId;					// PoolId of the charformat
	USHORT nPosition;					// Position of the Ruby-Character
	USHORT nAdjustment;					// specific adjustment of the Ruby-Ch.

public:
	SwFmtRuby( const String& rRubyTxt );
	SwFmtRuby( const SwFmtRuby& rAttr );
	virtual ~SwFmtRuby();

	SwFmtRuby& operator=( const SwFmtRuby& rAttr );

	// "pure virtual Methoden" vom SfxPoolItem
    virtual int             operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem*    Clone( SfxItemPool* pPool = 0 ) const;
    virtual SfxPoolItem*    Create(SvStream &, USHORT nVer) const;
    virtual SvStream&       Store(SvStream &, USHORT nIVer) const;


    virtual BOOL            QueryValue( ::com::sun::star::uno::Any& rVal,
                                    BYTE nMemberId = 0 ) const;
	virtual	BOOL			PutValue( const ::com::sun::star::uno::Any& rVal,
										BYTE nMemberId = 0 );

	virtual USHORT			GetVersion( USHORT nFFVer ) const;

	const SwTxtRuby* GetTxtRuby() const			{ return pTxtAttr; }
	SwTxtRuby* GetTxtRuby()						{ return pTxtAttr; }

	const String& GetText() const				{ return sRubyTxt; }
	void SetText( const String& rTxt )			{ sRubyTxt = rTxt; }

	const String& GetCharFmtName() const		{ return sCharFmtName; }
	void SetCharFmtName( const String& rNm )	{ sCharFmtName = rNm; }

	USHORT GetCharFmtId() const					{ return nCharFmtId; }
	void SetCharFmtId( USHORT nNew )			{ nCharFmtId = nNew; }

	USHORT GetPosition() const					{ return nPosition; }
	void SetPosition( USHORT nNew )				{ nPosition = nNew; }

	USHORT GetAdjustment() const				{ return nAdjustment; }
	void SetAdjustment( USHORT nNew )			{ nAdjustment = nNew; }
};


} //namespace binfilter
#endif

