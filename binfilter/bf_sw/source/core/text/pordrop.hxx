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


#ifndef _PORDROP_HXX
#define _PORDROP_HXX

#include "portxt.hxx"
namespace binfilter {

class SwFont;

/*************************************************************************
 *                      class SwDropPortionPart
 *
 * A drop portion can consist of one or more parts in order to allow
 * attribute changes inside them.
 *************************************************************************/

class SwDropPortionPart
{
    SwDropPortionPart* pFollow;
    SwFont* pFnt;
    xub_StrLen nLen;
    USHORT nWidth;

public:
    SwDropPortionPart( SwFont& rFont, const xub_StrLen nL )
            : pFollow( 0 ), pFnt( &rFont ), nLen( nL ), nWidth( 0 ) {};
    ~SwDropPortionPart();

    inline SwDropPortionPart* GetFollow() const { return pFollow; };
    inline void SetFollow( SwDropPortionPart* pNew ) { pFollow = pNew; };
    inline SwFont& GetFont() const { return *pFnt; }
    inline xub_StrLen GetLen() const { return nLen; }
    inline USHORT GetWidth() const { return nWidth; }
    inline void SetWidth( USHORT nNew )  { nWidth = nNew; }
};

/*************************************************************************
 *						class SwDropPortion
 *************************************************************************/

class SwDropPortion : public SwTxtPortion
{
    SwDropPortionPart* pPart; // due to script / attribute changes
    MSHORT nLines;          // Anzahl der Zeilen
	KSHORT nDropHeight; 	// Hoehe
	KSHORT nDropDescent; 	// Abstand zur naechsten Zeile
	KSHORT nDistance;		// Abstand zum Text
	KSHORT nFix;			// Fixposition
	short nX;				// X-PaintOffset
    short nY;               // Y-Offset

	inline void Fix( const KSHORT nNew ) { nFix = nNew; }
public:
    SwDropPortion( const MSHORT nLineCnt,
                   const KSHORT nDropHeight,
                   const KSHORT nDropDescent,
                   const KSHORT nDistance );
	virtual ~SwDropPortion();


	inline MSHORT GetLines() const { return nLines; }
	inline KSHORT GetDistance() const { return nDistance; }
	inline KSHORT GetDropHeight() const { return nDropHeight; }
	inline KSHORT GetDropDescent() const { return nDropDescent; }
	inline KSHORT GetDropLeft() const { return Width() + nFix; }

    inline SwDropPortionPart* GetPart() const { return pPart; }
    inline void SetPart( SwDropPortionPart* pNew ) { pPart = pNew; }

    inline void SetY( short nNew )  { nY = nNew; }

    inline SwFont* GetFnt() { return pPart ? &pPart->GetFont() : NULL; }

	static void DeleteDropCapCache();

	OUTPUT_OPERATOR
};


} //namespace binfilter
#endif
