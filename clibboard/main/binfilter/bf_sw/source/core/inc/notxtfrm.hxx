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


#ifndef _NOTXTFRM_HXX
#define _NOTXTFRM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif
class OutputDevice;
class Bitmap;
namespace binfilter {

class SwNoTxtNode;

class SwBorderAttrs;

struct SwCrsrMoveState;

class SwNoTxtFrm: public SwCntntFrm
{
	friend void _FrmFinit();	//erase pErrorBmp and pReplaceBmp
	static Bitmap *pErrorBmp, *pReplaceBmp;

	short	 nWeight;					// "Wichtigkeit" der Grafik

	const Size& GetSize() const;

	void InitCtor();

	void Format ( const SwBorderAttrs *pAttrs = 0 );
	void PaintCntnt  ( OutputDevice*, const SwRect&, const SwRect& ) const;
    /// OD 25.09.2002 #99739# - delete unused 3rd parameter
    void PaintPicture( OutputDevice*, const SwRect& ) const;
protected:
	virtual void MakeAll();
public:
	SwNoTxtFrm( SwNoTxtNode * const );
	~SwNoTxtFrm();

	virtual void Modify( SfxPoolItem*, SfxPoolItem* );
	virtual void Paint(const SwRect& ) const;
	virtual BOOL GetCharRect( SwRect &, const SwPosition&,
							  SwCrsrMoveState* = 0) const;
	BOOL GetCrsrOfst(SwPosition* pPos, Point& aPoint,
							const SwCrsrMoveState* = 0) const;

	const Size &GetGrfSize() const	{ return GetSize(); }
	void GetGrfArea( SwRect &rRect, SwRect * = 0, BOOL bMirror = TRUE ) const;

	void StopAnimation( OutputDevice* = 0 ) const;
	BOOL HasAnimation()  const;

	// Routinen fuer den Grafik-Cache
	USHORT GetWeight() { return nWeight; }

	static const Bitmap& GetBitmap( BOOL bError );
};


} //namespace binfilter
#endif
