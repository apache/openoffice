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



#ifndef _SVX_XBITMAP_HXX
#define _SVX_XBITMAP_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COLOR_HXX //autogen
#include <tools/color.hxx>
#endif
#ifndef _BITMAP_HXX //autogen
#include <vcl/bitmap.hxx>
#endif
#ifndef _BF_GOODIES_GRAPHICOBJECT_HXX //autogen
#include <bf_goodies/graphicobject.hxx>
#endif
#include <bf_svx/xenum.hxx>
namespace binfilter {

//---------------
// class XOBitmap
//---------------

#if defined HP9000 || defined SINIX
static Size aXOBitmapDefaultSize( 8, 8 );
#endif

class XOBitmap
{
protected:
	XBitmapType     eType;
	XBitmapStyle    eStyle;
	BfGraphicObject aGraphicObject;
	USHORT*         pPixelArray;
	Size            aArraySize;
	Color           aPixelColor;
	Color           aBckgrColor;
	BOOL            bGraphicDirty;

public:
	XOBitmap();
	XOBitmap( const BfGraphicObject& rGraphicObject, XBitmapStyle eStyle = XBITMAP_TILE );
	XOBitmap( const Bitmap& rBitmap, XBitmapStyle eStyle = XBITMAP_TILE );
#if defined HP9000 || defined SINIX
	XOBitmap( const USHORT* pArray, const Color& aPixelColor,
			 const Color& aBckgrColor, const Size& rSize = aXOBitmapDefaultSize,
			 XBitmapStyle eStyle = XBITMAP_TILE );
#else
	XOBitmap( const USHORT* pArray, const Color& aPixelColor,
			 const Color& aBckgrColor, const Size& rSize = Size( 8, 8 ),
			 XBitmapStyle eStyle = XBITMAP_TILE );
#endif
	XOBitmap( const XOBitmap& rXBmp );
	~XOBitmap();

	XOBitmap& operator=( const XOBitmap& rXOBitmap );
	int      operator==( const XOBitmap& rXOBitmap ) const;

	void Bitmap2Array();
	void Array2Bitmap();

	void SetGraphicObject( const BfGraphicObject& rObj )	{ aGraphicObject = rObj; bGraphicDirty = FALSE; }
	void SetBitmap( const Bitmap& rBmp )				{ aGraphicObject = BfGraphicObject( Graphic( rBmp ) ); bGraphicDirty = FALSE; }
	void SetBitmapType( XBitmapType eNewType )			{ eType = eNewType; }
	void SetBitmapStyle( XBitmapStyle eNewStyle )		{ eStyle = eNewStyle; }
	void SetPixelArray( const USHORT* pArray );
	void SetPixelSize( const Size& rSize )				{ aArraySize  = rSize;  bGraphicDirty = TRUE; }
	void SetPixelColor( const Color& rColor )			{ aPixelColor = rColor; bGraphicDirty = TRUE; }
	void SetBackgroundColor( const Color& rColor )		{ aBckgrColor = rColor; bGraphicDirty = TRUE; }

	XBitmapType				GetBitmapType() const				{ return eType; }
	XBitmapStyle			GetBitmapStyle() const				{ return eStyle; }
	const BfGraphicObject&	GetGraphicObject() const;
	Bitmap					GetBitmap() const;
	USHORT*					GetPixelArray() const				{ return pPixelArray; }
	Color					GetPixelColor() const				{ return aPixelColor; }
	Color					GetBackgroundColor() const			{ return aBckgrColor; }
};

}//end of namespace binfilter
#endif

