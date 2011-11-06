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



#ifndef _POREXP_HXX
#define _POREXP_HXX

#include <bf_svtools/bf_solar.h>

#include "portxt.hxx"
namespace binfilter {

/*************************************************************************
 *                      class SwExpandPortion
 *************************************************************************/

class SwExpandPortion : public SwTxtPortion
{
public:
	inline	SwExpandPortion() { SetWhichPor( POR_EXP ); }
	virtual sal_Bool Format( SwTxtFormatInfo &rInf );
	virtual SwPosSize GetTxtSize( const SwTxtSizeInfo &rInfo ) const;

    // Accessibility: pass information about this portion to the PortionHandler

	OUTPUT_OPERATOR
};


/*************************************************************************
 *						class SwBlankPortion
 *************************************************************************/

class SwBlankPortion : public SwExpandPortion
{
	xub_Unicode cChar;
	BOOL bMulti;		// For multiportion brackets
public:
	inline	SwBlankPortion( xub_Unicode cCh, BOOL bMult = sal_False )
		: cChar( cCh ), bMulti( bMult )
		{ cChar = cCh; SetLen(1); SetWhichPor( POR_BLANK ); }

	BOOL IsMulti() const { return bMulti; }
	void SetMulti( BOOL bNew ) { bMulti = bNew; }

	virtual SwLinePortion *Compress();
	virtual sal_Bool GetExpTxt( const SwTxtSizeInfo &rInf, XubString &rTxt ) const;
	virtual sal_Bool Format( SwTxtFormatInfo &rInf );
	MSHORT MayUnderFlow( const SwTxtFormatInfo &rInf, xub_StrLen nIdx,
		sal_Bool bUnderFlow ) const;

    // Accessibility: pass information about this portion to the PortionHandler

	OUTPUT_OPERATOR
};

/*************************************************************************
 *                      class SwPostItsPortion
 *************************************************************************/

class SwPostItsPortion : public SwExpandPortion
{
public:
	SwPostItsPortion( sal_Bool bScrpt ){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 //STRIP001 			SwPostItsPortion( sal_Bool bScrpt );
	OUTPUT_OPERATOR
};


CLASSIO( SwExpandPortion )
CLASSIO( SwBlankPortion )


} //namespace binfilter
#endif
