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

#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif
class Graphic; 
class SvStream; 
class Bitmap; 
class OutputDevice; 
namespace binfilter {


class RGBQuad;
class BmpFileHd;
class BmpInfoHd;
struct WmfFileHd;
class BmpFileHd;


class SwW4WGraf
{
	Point      aVectPos;          // W4W-Vector: aktuelle Position MoveTo/LineTo
	SvStream&  rInp;            // aus diesem Stream lesen
	Graphic*   pGraph;
	Bitmap*    pBmp;
	RGBQuad*   pPal;
	BmpFileHd* pBmpHead;
	BmpInfoHd* pBmpInfo;
	USHORT     nPalColors;
	USHORT 	   nVectMinX;
	USHORT 	   nVectMaxX;
	USHORT 	   nVectMinY;
	USHORT 	   nVectMaxY;
//	Size       aVectSiz;          // Maximale Ausdehnung der W4W-Vectorgrafik (wird von ReadHexPoint() gesezt)

	short      nStat;             // 1 = File-Grafik, 2 = Mem-Grafik
	BYTE       nError;

	void   ReadHexPoint(Point& aPnt);
	int    GetNextRecord();
	int    GetNextVectRec(OutputDevice& rOut);
	short  GetHexByte();
	USHORT GetHexUShort();
	ULONG  GetHexULong();
	long   GetVarSize();
	short  ReadPalette( long nVarSize );
	short  DefaultPalette( USHORT nColors );
	short  ReadBitmap( long nVarSize );
	void   SkipBytes( register long nBytes );

	ULONG ReadWmfHeader( WmfFileHd& rHd );
	short ReadW4WGrafBMap( long nTyp, long nWidth, long nHeight );
	short ReadW4WGrafVect( long nTyp, long nWidth, long nHeight );
	short ReadGrafFile( long nTyp, long nWidth, long nHeight );

	BOOL  CheckW4WVector();       // enth„lt die W4W-Grafik Vektordaten ? (Joe)

	sal_Char ReadChar() 	{ sal_Char c = 0; rInp.Read( &c, 1 ); return c; }

public:
	SwW4WGraf( SvStream& rIstream );
	~SwW4WGraf();
	short Read( long nGrafPDSType, long nGrWidthTw, long nGrHeightTw );
	Graphic* GetGraphic() const { return pGraph; };
};

} //namespace binfilter
