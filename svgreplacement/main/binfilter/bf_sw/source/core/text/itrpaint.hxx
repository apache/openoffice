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


#ifndef _ITRPAINT_HXX
#define _ITRPAINT_HXX

#include "itrtxt.hxx"
namespace binfilter {

class SwSaveClip;          // SwTxtPainter
class SwMultiPortion;

/*************************************************************************
 *                      class SwTxtPainter
 *************************************************************************/

class SwTxtPainter : public SwTxtCursor
{
	sal_Bool bPaintDrop;

protected:
	void CtorInit( SwTxtFrm *pFrm, SwTxtPaintInfo *pInf );
	inline SwTxtPainter() { }
public:
	inline SwTxtPainter( SwTxtFrm *pFrm, SwTxtPaintInfo *pInf )
		   { CtorInit( pFrm, pInf ); }
    // if PaintMultiPortion is called recursively, we have to pass the
    // surrounding SwBidiPortion
	inline void SetPaintDrop( const sal_Bool bNew ) { bPaintDrop = bNew; }
	inline int IsPaintDrop() const { return bPaintDrop; }
	inline SwTxtPaintInfo &GetInfo()
		{ return (SwTxtPaintInfo&)SwTxtIter::GetInfo(); }
	inline const SwTxtPaintInfo &GetInfo() const
		{ return (const SwTxtPaintInfo&)SwTxtIter::GetInfo(); }
};



} //namespace binfilter
#endif
