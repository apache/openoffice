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


#ifndef _PORFTN_HXX
#define _PORFTN_HXX

#include "porfld.hxx"
namespace binfilter {

class SwTxtFrm;
class SwTxtFtn;

/*************************************************************************
 *						class SwFtnPortion
 *************************************************************************/

class SwFtnPortion : public SwFldPortion
{
	SwTxtFrm *pFrm; 		// um im Dtor RemoveFtn rufen zu koennen.
	SwTxtFtn *pFtn;
	KSHORT	nOrigHeight;
public:
    SwFtnPortion( const XubString &rExpand, SwTxtFrm *pFrm, SwTxtFtn *pFtn,
                  KSHORT nOrig = KSHRT_MAX );
	inline KSHORT& Orig() {	return nOrigHeight; }

	virtual void Paint( const SwTxtPaintInfo &rInf ) const;
	virtual sal_Bool GetExpTxt( const SwTxtSizeInfo &rInf, XubString &rTxt ) const;
	virtual SwPosSize GetTxtSize( const SwTxtSizeInfo &rInfo ) const;
	virtual sal_Bool Format( SwTxtFormatInfo &rInf );

	const SwTxtFtn* GetTxtFtn() const { return pFtn; };
	OUTPUT_OPERATOR
};

/*************************************************************************
 *						class SwFtnNumPortion
 *************************************************************************/

class SwFtnNumPortion : public SwNumberPortion
{
public:
	inline SwFtnNumPortion( const XubString &rExpand, SwFont *pFnt )
		 : SwNumberPortion( rExpand, pFnt, sal_True, sal_False, 0 )
		 { SetWhichPor( POR_FTNNUM ); }
	sal_Bool DiffFont( SwFont* pFont );
	OUTPUT_OPERATOR
};

/*************************************************************************
 *						class SwQuoVadisPortion
 *************************************************************************/

class SwQuoVadisPortion : public SwFldPortion
{
public:
	SwQuoVadisPortion( const XubString &rExp, const XubString& rStr ): SwFldPortion( rExp ){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 			SwQuoVadisPortion( const XubString &rExp, const XubString& rStr );

	// Felder-Cloner fuer SplitGlue

    // Accessibility: pass information about this portion to the PortionHandler
	OUTPUT_OPERATOR
};

/*************************************************************************
 *						class SwErgoSumPortion
 *************************************************************************/

class SwErgoSumPortion : public SwFldPortion
{
public:
SwErgoSumPortion( const XubString &rExp, const XubString& rStr ):SwFldPortion( rExp ){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 			SwErgoSumPortion( const XubString &rExp, const XubString& rStr );
	OUTPUT_OPERATOR
};

CLASSIO( SwFtnPortion )
CLASSIO( SwFtnNumPortion )
CLASSIO( SwQuoVadisPortion )
CLASSIO( SwErgoSumPortion )


} //namespace binfilter
#endif
