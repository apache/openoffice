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


#ifndef _SCRRECT_HXX
#define _SCRRECT_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif
#include "swrect.hxx"
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
namespace binfilter {

class SwStripe
{
	SwTwips nY;
	SwTwips nHeight;
public:
	inline SwStripe( SwTwips nPos, SwTwips nHght )
		: nY( nPos ), nHeight( nHght ) {}
	inline SwTwips GetY() const { return nY; }
	inline SwTwips& Y() { return nY; }
	inline SwTwips GetHeight() const { return nHeight; }
	inline SwTwips& Height() { return nHeight; }
	inline SwTwips GetBottom() const { return nHeight ? nY + nHeight - 1 : nY; }
	inline BOOL operator<( const SwStripe &rTst ) const
		{ return nY < rTst.nY || ( nY == rTst.nY && nHeight > rTst.nHeight ); }
	inline BOOL operator==( const SwStripe& rTst ) const
		{ return nY == rTst.nY && nHeight == rTst.nHeight; }
};

SV_DECL_VARARR( SwStripeArr, SwStripe, 1, 4 )//STRIP008 ;

class SwStripes : public SwStripeArr, public SwStripe
{
	SwTwips nMin;       // Left minimum
	SwTwips nMax;       // Right maximum
public:
#ifdef VERTICAL_LAYOUT
    SwStripes( SwTwips nPos, SwTwips nSize, SwTwips nMn, SwTwips nMx )
        : SwStripe( nPos, nSize ), nMin( nMn ), nMax( nMx ) {}
#else
	SwStripes( const SwRect& rRect ) : SwStripe( rRect.Top(), rRect.Height() ),
		nMin(rRect.Left()), nMax(rRect.Left() + rRect.Width()) {}
#endif
	inline SwTwips GetMin() const { return nMin; }
	inline SwTwips GetMax() const { return nMax; }
	inline void SetMin( const SwTwips nNew ) { nMin = nNew; }
	inline void SetMax( const SwTwips nNew ) { nMax = nNew; }
	inline void ChkMin( const SwTwips nNew ) { if( nNew < nMin ) nMin = nNew; }
	inline void ChkMax( const SwTwips nNew ) { if( nNew > nMax ) nMax = nNew; }
	inline SwTwips GetRight() const { return nMax > nMin ? nMax - 1 : nMax; }
	inline SwTwips GetWidth() const { return nMax - nMin; }
};

class SwScrollColumn
{
	SwTwips nX;
	SwTwips nWidth;
	SwTwips nOffs;
#ifdef VERTICAL_LAYOUT
    BOOL bVertical;
public:
    inline SwScrollColumn( SwTwips nPos, SwTwips nSz, SwTwips nOff, BOOL bVert )
        : nX( nPos ), nWidth( nSz ), nOffs( nOff ), bVertical( bVert ) {}
	inline SwScrollColumn( const SwScrollColumn& rCol )
        : nX( rCol.nX ), nWidth( rCol.nWidth ),
          nOffs( rCol.nOffs ), bVertical( rCol.bVertical ) {}
    BOOL IsVertical() const { return bVertical; }
	inline BOOL operator<( const SwScrollColumn &rTst ) const
        { return bVertical < rTst.bVertical || ( bVertical == rTst.bVertical &&
          ( nX < rTst.nX || ( nX == rTst.nX && ( nWidth < rTst.nWidth ||
          ( nWidth == rTst.nWidth && nOffs < rTst.nOffs ) ) ) ) ); }
	inline BOOL operator==( const SwScrollColumn &rTst ) const
        { return bVertical == rTst.bVertical && nX == rTst.nX &&
                 nWidth == rTst.nWidth && nOffs == rTst.nOffs;}
#else
public:
	inline SwScrollColumn( const SwRect& rRect, SwTwips nOff )
		: nX( rRect.Left() ), nWidth( rRect.Width() ), nOffs( nOff ) {}
	inline SwScrollColumn( const SwScrollColumn& rCol )
		: nX( rCol.nX ), nWidth( rCol.nWidth ), nOffs( rCol.nOffs ) {}
	inline BOOL operator<( const SwScrollColumn &rTst ) const
		{ return nX < rTst.nX || ( nX == rTst.nX && ( nWidth < rTst.nWidth ||
		  ( nWidth == rTst.nWidth && nOffs < rTst.nOffs ) ) ); }
	inline BOOL operator==( const SwScrollColumn &rTst ) const
		{ return nX == rTst.nX && nWidth == rTst.nWidth && nOffs == rTst.nOffs;}
#endif
	inline SwTwips GetX() const { return nX; }
	inline SwTwips GetWidth() const { return nWidth; }
	inline SwTwips GetOffs() const { return nOffs; }
	inline void ClrOffs() { nOffs = 0; }
	inline SwTwips GetRight() const { return nWidth ? nX + nWidth - 1 : nX; }
};

typedef SwStripes* SwStripesPtr;
SV_DECL_PTRARR_SORT(SwScrollStripes, SwStripesPtr, 1, 4)

class SwScrollArea : public SwScrollColumn, public SwScrollStripes
{
public:
	inline SwScrollArea( const SwScrollColumn &rCol, SwStripes* pStripes )
		: SwScrollColumn( rCol )
		{ Insert( pStripes ); }
};

typedef SwScrollArea* SwScrollAreaPtr;
SV_DECL_PTRARR_SORT(SScrAreas,SwScrollAreaPtr,1,2)//STRIP008 ;

class SwScrollAreas : public SScrAreas
{
public:
};


} //namespace binfilter
#endif //_SCRRECT_HXX
