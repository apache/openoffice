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



#ifndef _XPMREAD_HXX
#define _XPMREAD_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _BITMAP_HXX
#include <vcl/bitmap.hxx>
#endif

class BitmapWriteAccess;
class Graphic;

namespace binfilter
{

#ifdef _XPMPRIVATE

#define XPMTEMPBUFSIZE		0x00008000
#define XPMSTRINGBUF		0x00008000

#define XPMIDENTIFIER		0x00000001			// mnIdentifier includes on of the six phases
#define XPMDEFINITION		0x00000002			// the XPM format consists of
#define XPMVALUES			0x00000003
#define XPMCOLORS			0x00000004
#define XPMPIXELS			0x00000005
#define XPMEXTENSIONS		0x00000006
#define XPMENDEXT			0x00000007


#define XPMREMARK			0x00000001			// defines used by mnStatus
#define XPMDOUBLE			0x00000002
#define XPMSTRING			0x00000004
#define XPMFINISHED			0x00000008

#define XPMCASESENSITIVE	0x00000001
#define XPMCASENONSENSITIVE 0x00000002

// ---------
// - Enums -
// ---------

enum ReadState
{
	XPMREAD_OK,
	XPMREAD_ERROR,
	XPMREAD_NEED_MORE
};

// -------------
// - XPMReader -
// -------------

class XPMReader : public GraphicReader
{
private:

	SvStream&			mrIStm;
	Bitmap				maBmp;
	BitmapWriteAccess*	mpAcc;
	Bitmap				maMaskBmp;
	BitmapWriteAccess*	mpMaskAcc;
	long				mnLastPos;

	ULONG				mnWidth;
	ULONG				mnHeight;
	ULONG				mnColors;
	ULONG				mnCpp;								// characters per pix
	BOOL				mbTransparent;
	BOOL				mbStatus;
	ULONG				mnStatus;
	ULONG				mnIdentifier;
	BYTE				mcThisByte;
	BYTE				mcLastByte;
	ULONG				mnTempAvail;
	BYTE*				mpTempBuf;
	BYTE*				mpTempPtr;
	BYTE*				mpFastColorTable;
	BYTE*				mpColMap;
	ULONG				mnStringSize;
	BYTE*				mpStringBuf;
	ULONG				mnParaSize;
	BYTE*				mpPara;

	BOOL				ImplGetString( void );
	BOOL				ImplGetColor( ULONG );
	BOOL				ImplGetScanLine( ULONG );
	BOOL				ImplGetColSub( BYTE* );
	BOOL				ImplGetColKey( BYTE );
	void				ImplGetRGBHex( BYTE*, ULONG );
	BOOL				ImplGetPara( ULONG numb );
	BOOL				ImplCompare( BYTE*, BYTE*, ULONG, ULONG nmode = XPMCASENONSENSITIVE );
	ULONG				ImplGetULONG( ULONG nPara );

public:
						XPMReader( SvStream& rStm );
	virtual				~XPMReader();

	ReadState			ReadXPM( Graphic& rGraphic );
};

#endif // _XPMPRIVATE

// -------------
// - ImportXPM -
// -------------

BOOL ImportXPM( SvStream& rStream, Graphic& rGraphic );

}

#endif // _XPMREAD_HXX
