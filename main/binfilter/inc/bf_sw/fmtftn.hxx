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


#ifndef _FMTFTN_HXX
#define _FMTFTN_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
namespace binfilter {

class SwDoc;
class SwTxtFtn;

// ATT_FTN **********************************************************

class SwFmtFtn: public SfxPoolItem
{
	friend class SwTxtFtn;
	SwTxtFtn* pTxtAttr;		//mein TextAttribut
	String  aNumber;		//Benutzerdefinierte 'Nummer'
	USHORT  nNumber;		//Automatische Nummerierung
	BOOL	bEndNote;		//Ist es eine Endnote?

	// geschuetzter CopyCtor
	SwFmtFtn& operator=(const SwFmtFtn& rFtn);
	SwFmtFtn( const SwFmtFtn& );

public:
	SwFmtFtn( BOOL bEndNote = FALSE );
	~SwFmtFtn();

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;
	virtual USHORT			GetVersion( USHORT nFFVer ) const;

	const String &GetNumStr() const { return aNumber; }
	const USHORT &GetNumber() const { return nNumber; }
		  BOOL	  IsEndNote() const	{ return bEndNote;}

	void SetNumStr( const String& rStr )	{ aNumber = rStr; }
	void SetNumber( USHORT nNo ) 			{ nNumber = nNo; }
    void SetEndNote( BOOL b );

	void SetNumber( const SwFmtFtn& rFtn )
		{ nNumber = rFtn.nNumber; aNumber = rFtn.aNumber; }

	const SwTxtFtn *GetTxtFtn() const   { return pTxtAttr; }
		  SwTxtFtn *GetTxtFtn()	  		{ return pTxtAttr; }


	// returnt den anzuzeigenden String der Fuss-/Endnote
	String GetViewNumStr( const SwDoc& rDoc, BOOL bInclStrs = FALSE ) const;
};


} //namespace binfilter
#endif

