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



#ifndef SC_DETFUNC_HXX
#define SC_DETFUNC_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif

#ifndef _TOOLS_COLOR_HXX 
#include <tools/color.hxx>
#endif
class String;
namespace binfilter {

class SdrObject;
class SdrPage;

class ScCommentData;
class ScDetectiveData;
class ScDocument;
class ScTripel;
class ScAddress;
class ScRange;

#define SC_DET_MAXCIRCLE	1000

enum ScDetectiveDelete { SC_DET_ALL, SC_DET_DETECTIVE, SC_DET_CIRCLES, SC_DET_COMMENTS, SC_DET_ARROWS };

enum ScDetectiveObjType
{
	SC_DETOBJ_NONE,
	SC_DETOBJ_ARROW,
	SC_DETOBJ_FROMOTHERTAB,
	SC_DETOBJ_TOOTHERTAB,
	SC_DETOBJ_CIRCLE
};

class ScDetectiveFunc
{
	static ColorData nArrowColor;
	static ColorData nErrorColor;
	static ColorData nCommentColor;
	static BOOL		 bColorsInitialized;

	ScDocument*		pDoc;
	USHORT			nTab;

	Point		GetDrawPos( USHORT nCol, USHORT nRow, BOOL bArrow );
	BOOL		HasArrow( USHORT nStartCol, USHORT nStartRow, USHORT nStartTab,
							USHORT nEndCol, USHORT nEndRow, USHORT nEndTab );

	void		DeleteArrowsAt( USHORT nCol, USHORT nRow, BOOL bDestPnt );
	void		DeleteBox( USHORT nCol1, USHORT nRow1, USHORT nCol2, USHORT nRow2 );

	BOOL		HasError( const ScTripel& rStart, const ScTripel& rEnd, ScTripel& rErrPos );

	void		FillAttributes( ScDetectiveData& rData );

				// called from DrawEntry/DrawAlienEntry and InsertObject
	BOOL		InsertArrow( USHORT nCol, USHORT nRow,
								USHORT nRefStartCol, USHORT nRefStartRow, 
								USHORT nRefEndCol, USHORT nRefEndRow,
								BOOL bFromOtherTab, BOOL bRed,
								ScDetectiveData& rData );
	BOOL		InsertToOtherTab( USHORT nStartCol, USHORT nStartRow,
								USHORT nEndCol, USHORT nEndRow, BOOL bRed,
								ScDetectiveData& rData );

				// DrawEntry / DrawAlienEntry check for existing arrows and errors
	BOOL		DrawEntry( USHORT nCol, USHORT nRow, const ScTripel& rRefStart, const ScTripel& rRefEnd,
								ScDetectiveData& rData );
	BOOL		DrawAlienEntry( const ScTripel& rRefStart, const ScTripel& rRefEnd,
								ScDetectiveData& rData );

	void		DrawCircle( USHORT nCol, USHORT nRow, ScDetectiveData& rData );


	USHORT		InsertPredLevel( USHORT nCol, USHORT nRow, ScDetectiveData& rData, USHORT nLevel );
	USHORT		InsertPredLevelArea( const ScTripel& rRefStart, const ScTripel& rRefEnd,
										ScDetectiveData& rData, USHORT nLevel );
	USHORT		FindPredLevel( USHORT nCol, USHORT nRow, USHORT nLevel, USHORT nDeleteLevel );
	USHORT		FindPredLevelArea( const ScTripel& rRefStart, const ScTripel& rRefEnd,
									USHORT nLevel, USHORT nDeleteLevel );

	USHORT		InsertErrorLevel( USHORT nCol, USHORT nRow, ScDetectiveData& rData, USHORT nLevel );

	USHORT		InsertSuccLevel( USHORT nCol1, USHORT nRow1, USHORT nCol2, USHORT nRow2,
										ScDetectiveData& rData, USHORT nLevel );
	USHORT		FindSuccLevel( USHORT nCol1, USHORT nRow1, USHORT nCol2, USHORT nRow2,
								USHORT nLevel, USHORT nDeleteLevel );

/*N*/ 	BOOL		FindFrameForObject( SdrObject* pObject, ScRange& rRange );


public:
				ScDetectiveFunc(ScDocument* pDocument, USHORT nTable) : pDoc(pDocument),nTab(nTable) {}

	BOOL		ShowSucc( USHORT nCol, USHORT nRow );
	BOOL		ShowPred( USHORT nCol, USHORT nRow );
	BOOL		ShowError( USHORT nCol, USHORT nRow );

	BOOL		DeleteSucc( USHORT nCol, USHORT nRow );
	BOOL		DeletePred( USHORT nCol, USHORT nRow );
	BOOL		DeleteAll( ScDetectiveDelete eWhat );

	BOOL		MarkInvalid(BOOL& rOverflow);

	SdrObject*	ShowComment( USHORT nCol, USHORT nRow, BOOL bForce, SdrPage* pDestPage = NULL );
	SdrObject*	ShowCommentUser( USHORT nCol, USHORT nRow, const String& rUserText,
									const Rectangle& rVisible, BOOL bLeft,
									BOOL bForce, SdrPage* pDestPage );
	BOOL		HideComment( USHORT nCol, USHORT nRow );

	void		UpdateAllComments();		// on all tables
/*N*/ 	void		UpdateAllArrowColors();		// on all tables


	ScDetectiveObjType GetDetectiveObjectType( SdrObject* pObject,
								ScAddress& rPosition, ScRange& rSource, BOOL& rRedLine );
	void		InsertObject( ScDetectiveObjType eType, const ScAddress& rPosition,
								const ScRange& rSource, BOOL bRedLine );

	static ColorData GetArrowColor();
	static ColorData GetErrorColor();
	static ColorData GetCommentColor();
	static void InitializeColors();
};



} //namespace binfilter
#endif
