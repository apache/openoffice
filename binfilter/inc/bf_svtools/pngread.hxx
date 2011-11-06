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



#ifndef _PNGREAD_HXX
#define _PNGREAD_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _GEN_HXX
#include <tools/gen.hxx>
#endif
#ifndef _SV_GRAPH_H
#include <vcl/graph.h>
#endif

class SvStream;
class Graphic;
class ZCodec;
class Bitmap;
class BitmapColor;
class AlphaMask;
class BitmapWriteAccess;

namespace binfilter
{

// ------------------------------------------------------------------------

enum ReadState
{
	PNGREAD_OK,
	PNGREAD_ERROR,
	PNGREAD_NEED_MORE
};

// -------------
// - PNGReader -
// -------------

#define CHUNK_IS_OPEN		1
#define CHUNK_IS_CLOSED		2
#define CHUNK_IS_IN_USE		4

class PNGReader : public GraphicReader
{
	ReadState			meReadState;
	SvStream*			mpIStm;
	void*				mpCallerData;
	USHORT				mnIStmOldMode;
	BOOL				mbStatus;
	BOOL				mbFinished;
	BOOL				mbFirstEntry;
	UINT32				mnChunkStatus;
	UINT32				mnChunkStartPosition;
	UINT32				mnIDATCRCCount;
	UINT32				mnLatestStreamPos;
	static const BYTE	mnBlockHeight[ 8 ];
	static const BYTE	mnBlockWidth[ 8 ];
	static const BYTE	mpDefaultColorTable[ 256 ];
	Bitmap*				mpBmp;
	BitmapWriteAccess*	mpAcc;
	Bitmap*				mpMaskBmp;
	AlphaMask*			mpAlphaMask;
	BitmapWriteAccess*	mpMaskAcc;
	

	ZCodec* 			mpZCodec;
	BYTE*				mpInflateInBuf; // as big as the size of a scanline + alphachannel + 1
	BYTE*				mpScanprior;	// pointer to the latest scanline
	BYTE*				mpTransTab;		//
	BYTE				mnTransRed;
	BYTE				mnTransGreen;
	BYTE				mnTransBlue;
	BYTE				mnDummy;

	ULONG				mnChunkType;	// Chunk which is currently open
	ULONG				mnCRC;
	long				mnChunkDatSizeOrg;
	long				mnChunkDatSize;

	ULONG				mnWidth;
	ULONG				mnHeight;
	sal_uInt32			mnPrefWidth;	// preferred width in meter
	sal_uInt32			mnPrefHeight;	// preferred Height in meter
	BYTE				mnBitDepth;		// sample depth
	BYTE				mnColorType;
	BYTE				mnCompressionType;
	BYTE				mnFilterType;
	BYTE				mnInterlaceType;

	USHORT				mnDepth;		// pixel depth
	ULONG				mnBBP;			// number of bytes per pixel
	ULONG				mnScansize;		// max size of scanline
	BOOL				mbTransparent;	// graphic includes an tRNS Chunk or an alpha Channel
	BOOL				mbAlphaChannel;
	BOOL				mbRGBTriple;
	BOOL				mbPalette;		// FALSE if we need a Palette
	BOOL				mbGrayScale;
	BOOL				mbzCodecInUse;
	BOOL				mbIDAT;			// TRUE if finished with the complete IDAT...
	BOOL				mbGamma;		// TRUE if Gamma Correction available
	BOOL				mbpHYs;			// TRUE if pysical size of pixel available
	BYTE				mnPass;			// if interlaced the latest pass ( 1..7 ) else 7
	ULONG				mnYpos;			// latest y position;
	BYTE*				mpScan;			// pointer in the current scanline
	BYTE*				mpColorTable;	//
	BYTE				cTransIndex1;
	BYTE				cNonTransIndex1;

	
	void				ImplSetPixel( ULONG y, ULONG x, const BitmapColor &, BOOL bTrans );
	void				ImplSetPixel( ULONG y, ULONG x, BYTE nPalIndex, BOOL bTrans );
	void				ImplSetAlphaPixel( ULONG y, ULONG x, const BitmapColor&, BYTE nAlpha );
	void				ImplReadIDAT();
	void				ImplResizeScanline();
	void				ImplGetFilter( ULONG nXStart=0, ULONG nXAdd=1 );
	void				ImplReadTransparent();
	void				ImplGetGamma();
	void				ImplGetBackground();
	BYTE				ImplScaleColor();
	BOOL				ImplReadHeader();
	BOOL				ImplReadPalette();
	void				ImplGetGrayPalette( ULONG );
	void				ImplOpenChunk();
	BYTE				ImplReadBYTE();
	ULONG				ImplReadULONG();
	void				ImplReadDAT( unsigned char* pSource, long nDatSize );
	BOOL				ImplCloseChunk();
	void				ImplSkipChunk();
	sal_Bool			ImplIsPending( sal_uInt32 nPos, sal_uInt32 nSize );

public:

	ReadState			ReadPNG( Graphic& rGraphic );

						PNGReader( SvStream& rStm, void* pCallData );
	virtual				~PNGReader();
};


// -------------
// - ImportPNG -
// -------------

 BOOL ImportPNG( SvStream& rStream, Graphic& rGraphic, void* pCallerData );

 }

#endif // _PNGREAD_HXX
