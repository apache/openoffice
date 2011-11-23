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



#ifndef _GIFREAD_HXX
#define _GIFREAD_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _GRAPH_HXX
#include <vcl/graph.hxx>
#endif
#ifndef _BMPACC_HXX
#include <vcl/bmpacc.hxx>
#endif

class SvStream;

namespace binfilter
{

#ifdef _GIFPRIVATE

// ---------
// - Enums -
// ---------

enum GIFAction
{
	GLOBAL_HEADER_READING,
	MARKER_READING,
	EXTENSION_READING,
	LOCAL_HEADER_READING,
	FIRST_BLOCK_READING,
	NEXT_BLOCK_READING,
	ABORT_READING,
	END_READING
};

// ------------------------------------------------------------------------

enum ReadState
{
	GIFREAD_OK,
	GIFREAD_ERROR,
	GIFREAD_NEED_MORE
};

// -------------
// - GIFReader -
// -------------

class GIFLZWDecompressor;

class GIFReader : public GraphicReader
{
	Graphic				aImGraphic;
	Animation			aAnimation;
	Bitmap				aBmp8;
	Bitmap				aBmp1;
	BitmapPalette		aGPalette;
	BitmapPalette		aLPalette;
	SvStream&			rIStm;
	void*				pCallerData;
	HPBYTE				pSrcBuf;
	GIFLZWDecompressor*	pDecomp;
	BitmapWriteAccess*	pAcc8;
	BitmapWriteAccess*	pAcc1;
	long				nYAcc;
	long				nLastPos;
	sal_uInt32			nLogWidth100;
	sal_uInt32			nLogHeight100;
	USHORT				nTimer;
	USHORT				nGlobalWidth;			// maximale Bildbreite aus Header
	USHORT				nGlobalHeight;			// maximale Bildhoehe aus Header
	USHORT				nImageWidth;			// maximale Bildbreite aus Header
	USHORT				nImageHeight;			// maximale Bildhoehe aus Header
	USHORT				nImagePosX;
	USHORT				nImagePosY;
	USHORT				nImageX;				// maximale Bildbreite aus Header
	USHORT				nImageY;				// maximale Bildhoehe aus Header
	USHORT				nLastImageY;
	USHORT				nLastInterCount;
	USHORT				nLoops;
	GIFAction			eActAction;
	BOOL				bStatus;
	BOOL				bGCTransparent;			// Ob das Bild Transparent ist, wenn ja:
	BOOL				bInterlaced;
	BOOL				bOverreadBlock;
	BOOL				bImGraphicReady;
	BOOL				bGlobalPalette;
	BYTE				nBackgroundColor;		// Hintergrundfarbe
	BYTE				nGCTransparentIndex;	// Pixel von diesem Index sind durchsichtig
	BYTE				nGCDisposalMethod;		// 'Disposal Method' (siehe GIF-Doku)
	BYTE				cTransIndex1;
	BYTE				cNonTransIndex1;

	void				ReadPaletteEntries( BitmapPalette* pPal, ULONG nCount );
	void				ClearImageExtensions();
	BOOL				CreateBitmaps( long nWidth, long nHeight, BitmapPalette* pPal, BOOL bWatchForBackgroundColor );
	BOOL				ReadGlobalHeader();
	BOOL				ReadExtension();
	BOOL				ReadLocalHeader();
	ULONG				ReadNextBlock();
	void				FillImages( HPBYTE pBytes, ULONG nCount );
	void				CreateNewBitmaps();
	BOOL				ProcessGIF();

public:

	ReadState			ReadGIF( Graphic& rGraphic );
	const Graphic&		GetIntermediateGraphic();

						GIFReader( SvStream& rStm );
	virtual				~GIFReader();
};

#endif // _GIFPRIVATE

// -------------
// - ImportGIF -
// -------------

 BOOL ImportGIF( SvStream& rStream, Graphic& rGraphic );

 }

#endif // _GIFREAD_HXX
