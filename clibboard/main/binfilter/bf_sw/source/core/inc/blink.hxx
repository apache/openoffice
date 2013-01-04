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



#ifndef	_BLINK_HXX
#define	_BLINK_HXX

class SwLinePortion;
class SwRootFrm;
class SwTxtFrm;

#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif
#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif
namespace binfilter {

class SwBlinkPortion
{
    Point               aPos;
	const SwLinePortion	*pPor;
	const SwRootFrm		*pFrm;
    USHORT              nDir;
public:
    SwBlinkPortion( const SwLinePortion* pPortion, USHORT nDirection )
            { pPor = pPortion; nDir = nDirection; }
	SwBlinkPortion( const SwBlinkPortion* pBlink, const SwLinePortion* pPort )
    {   pPor = pPort; pFrm = pBlink->pFrm; aPos = pBlink->aPos; nDir = pBlink->nDir; }
	void SetPos( const Point& aNew ){ aPos = aNew; }
	const Point& GetPos() const{ return aPos; }
	void SetRootFrm( const SwRootFrm* pNew ){ pFrm = pNew; }
	const SwRootFrm* GetRootFrm() const{ return pFrm; }
	const SwLinePortion *GetPortion() const{ return pPor; }
    USHORT GetDirection() const { return nDir; }
	BOOL operator<( const SwBlinkPortion& rBlinkPortion ) const
	{ return (long)pPor < (long)rBlinkPortion.pPor; }
	BOOL operator==( const SwBlinkPortion& rBlinkPortion ) const
	{ return (long)pPor == (long)rBlinkPortion.pPor; }
};

typedef SwBlinkPortion* SwBlinkPortionPtr;
SV_DECL_PTRARR_SORT_DEL(SwBlinkList, SwBlinkPortionPtr, 0, 10)

} //namespace binfilter
#endif

