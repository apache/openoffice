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


#ifndef _PORTXT_HXX
#define _PORTXT_HXX

#ifdef GCC
#include <sys/types.h>
#else
#include <new.h>	//fuer size_t, FIXEDMEM aus tools
#endif
#ifndef _SVMEMPOOL_HXX //autogen
#include <tools/mempool.hxx>
#endif

#include "porlin.hxx"
/*N*/ #include <tools/debug.hxx> //for stripping
namespace binfilter {
class SwTxtGuess;

/*************************************************************************
 *						class SwTxtPortion
 *************************************************************************/

class SwTxtPortion : public SwLinePortion
{
	void BreakCut( SwTxtFormatInfo &rInf, const SwTxtGuess &rGuess );
	void BreakUnderflow( SwTxtFormatInfo &rInf );
	sal_Bool _Format( SwTxtFormatInfo &rInf );

public:
	inline SwTxtPortion(){ SetWhichPor( POR_TXT ); }
	SwTxtPortion( const SwLinePortion &rPortion );
	virtual void Paint( const SwTxtPaintInfo &rInf ) const{DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 	virtual void Paint( const SwTxtPaintInfo &rInf ) const;
	virtual sal_Bool Format( SwTxtFormatInfo &rInf );
	virtual void FormatEOL( SwTxtFormatInfo &rInf );
	virtual SwPosSize GetTxtSize( const SwTxtSizeInfo &rInfo ) const;
	virtual sal_Bool GetExpTxt( const SwTxtSizeInfo &rInf, XubString &rTxt ) const;

	// zaehlt die Spaces fuer Blocksatz
	xub_StrLen GetSpaceCnt( const SwTxtSizeInfo &rInf, xub_StrLen& rCnt ) const;

	sal_Bool CreateHyphen( SwTxtFormatInfo &rInf, SwTxtGuess &rGuess );

    // Accessibility: pass information about this portion to the PortionHandler

 	OUTPUT_OPERATOR
	DECL_FIXEDMEMPOOL_NEWDEL(SwTxtPortion)
};

/*************************************************************************
 *						class SwHolePortion
 *************************************************************************/

class SwHolePortion : public SwLinePortion
{
	KSHORT nBlankWidth;
public:
			SwHolePortion( const SwTxtPortion &rPor );
	inline KSHORT GetBlankWidth( ) const { return nBlankWidth; }
	inline void SetBlankWidth( const KSHORT nNew ) { nBlankWidth = nNew; }
	virtual SwLinePortion *Compress();
	virtual void Paint( const SwTxtPaintInfo &rInf ) const{DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	virtual void Paint( const SwTxtPaintInfo &rInf ) const;

    // Accessibility: pass information about this portion to the PortionHandler

	OUTPUT_OPERATOR
	DECL_FIXEDMEMPOOL_NEWDEL(SwHolePortion)
};

CLASSIO( SwTxtPortion )
CLASSIO( SwHolePortion )


} //namespace binfilter
#endif
