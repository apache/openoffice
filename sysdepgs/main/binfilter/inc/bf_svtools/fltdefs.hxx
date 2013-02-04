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



#include <bf_svtools/bf_solar.h>
#include <vcl/bitmap.hxx>

#include <string.h>

namespace binfilter
{

#ifndef _FLTDEFS_HXX
#define _FLTDEFS_HXX


#if defined ( WIN ) || defined ( WNT )

#define RGBQUAD	RGBQUADWIN

typedef struct RGBQUAD
{
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;

			RGBQUAD( const BYTE cRed = 0, const BYTE cGreen = 0, const BYTE cBlue = 0 ) :
				rgbBlue		( cBlue ),
				rgbGreen	( cGreen ),
				rgbRed		( cRed ),
				rgbReserved	( 0 ) {};
} RGBQUAD;


#ifdef WIN
typedef BYTE huge* PDIBBYTE;
#define MEMCPY hmemcpy
#define GLOBALALLOC(nSize) ((PDIBBYTE)GlobalLock(GlobalAlloc(GHND,(nSize))))
#define GLOBALHANDLE(pPointer) ((HGLOBAL)GlobalHandle((*((size_t*)&(pPointer)+1))))
#define GLOBALFREE(pPointer) (GlobalUnlock(GLOBALHANDLE((pPointer))))
#define MEMSET( pDst, cByte, nCount )	\
{										\
	PDIBBYTE pTmp = (PDIBBYTE) pDst;	\
	for ( ULONG i = 0; i < nCount; i++ )\
		*pTmp++ = cByte;				\
}

#else

typedef BYTE* PDIBBYTE;
#define MEMCPY memcpy
#define MEMSET memset
#define GLOBALALLOC(nSize) ((PDIBBYTE)GlobalAlloc(GMEM_FIXED,(nSize)))
#define GLOBALFREE(pPointer) (GlobalFree((HGLOBAL)pPointer))
#define GLOBALHANDLE(pPointer) ((HGLOBAL)(pPointer))

#endif
#else

typedef BYTE* PDIBBYTE;
#define MEMCPY memcpy
#define MEMSET memset
#define GLOBALALLOC(nSize) ((PDIBBYTE)new BYTE[(nSize)])
#define GLOBALFREE(pPointer) (delete[] (pPointer))

#endif


#if defined ( OS2 ) || defined ( UNX )
void ReadBitmap( SvStream& rIStream, Bitmap& rBmp, USHORT nDefaultHeight = 0, ULONG nOffBits = 0 );
void ReplaceInfoHeader( SvStream& rStm, BYTE* pBuffer );

#ifdef OS2
#define RGBQUAD				RGBQUADOS2
#define BITMAPFILEHEADER	BITMAPFILEHEADEROS2
#define PBITMAPFILEHEADER	PBITMAPFILEHEADEROS2
#define BITMAPINFOHEADER	BITMAPINFOHEADEROS2
#define PBITMAPINFOHEADER	PBITMAPINFOHEADEROS2
#define BITMAPINFO			BITMAPINFOOS2
#define PBITMAPINFO			PBITMAPINFOOS2
#endif

typedef struct RGBQUAD
{
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;

			RGBQUAD( const BYTE cRed = 0, const BYTE cGreen = 0, const BYTE cBlue = 0 ) :
				rgbBlue		( cBlue ),
				rgbGreen	( cGreen ),
				rgbRed		( cRed ),
				rgbReserved	( 0 ) {};
} RGBQUAD;

typedef struct BITMAPFILEHEADER
{
	UINT16	bfType;
	UINT32	bfSize;
	UINT16	bfReserved1;
	UINT16	bfReserved2;
	UINT32	bfOffBits;
} BITMAPFILEHEADER;
typedef BITMAPFILEHEADER* PBITMAPFILEHEADER;

typedef struct BITMAPINFOHEADER
{
	UINT32	biSize;
	UINT32	biWidth;
	UINT32	biHeight;
	UINT16	biPlanes;
	UINT16	biBitCount;
	UINT32	biCompression;
	UINT32	biSizeImage;
	UINT32	biXPelsPerMeter;
	UINT32	biYPelsPerMeter;
	UINT32	biClrUsed;
	UINT32	biClrImportant;
} BITMAPINFOHEADER;
typedef BITMAPINFOHEADER* PBITMAPINFOHEADER;

typedef struct BITMAPINFO
{
	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD				bmiColors[1];
} BITMAPINFO;
typedef BITMAPINFO* PBITMAPINFO;

#endif
}

#endif
