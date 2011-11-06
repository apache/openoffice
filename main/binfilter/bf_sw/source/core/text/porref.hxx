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


#ifndef _PORREF_HXX
#define _PORREF_HXX

#include "portxt.hxx"
namespace binfilter {

/*************************************************************************
 *                      class SwRefPortion
 *************************************************************************/

class SwRefPortion : public SwTxtPortion
{
public:
	inline	SwRefPortion(){	SetWhichPor( POR_REF ); }
//STRIP001 	virtual void Paint( const SwTxtPaintInfo &rInf ) const;
	OUTPUT_OPERATOR
};

/*************************************************************************
 *                      class SwIsoRefPortion
 *************************************************************************/

class SwIsoRefPortion : public SwRefPortion
{
//STRIP001 	KSHORT nViewWidth;
//STRIP001 
public:
	SwIsoRefPortion(){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 //STRIP001 			SwIsoRefPortion();
//STRIP001 	virtual sal_Bool Format( SwTxtFormatInfo &rInf );
//STRIP001 	virtual void Paint( const SwTxtPaintInfo &rInf ) const;
//STRIP001 	virtual SwLinePortion *Compress();
//STRIP001 	virtual KSHORT GetViewWidth( const SwTxtSizeInfo &rInf ) const;

    // Accessibility: pass information about this portion to the PortionHandler
//STRIP001     virtual void HandlePortion( SwPortionHandler& rPH ) const;

	OUTPUT_OPERATOR
};

/*************************************************************************
 *					inline - Implementations
 *************************************************************************/

CLASSIO( SwRefPortion )
//STRIP001 CLASSIO( SwIsoRefPortion )


} //namespace binfilter
#endif
