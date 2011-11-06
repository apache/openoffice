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



#ifndef SC_DRWLAYER_HXX
#define SC_DRWLAYER_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SV_GRAPH_HXX //autogen
#include <vcl/graph.hxx>
#endif
#ifndef _FM_FMMODEL_HXX
#include <bf_svx/fmmodel.hxx>
#endif
#ifndef _SVSTOR_HXX
#include <bf_so3/svstor.hxx>
#endif
#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
namespace binfilter {

class ScDocument;
class SfxViewShell;
class ScDrawObjData;
class ScIMapInfo;
class IMapObject;
class ScMarkData;
class SdrOle2Obj;

// -----------------------------------------------------------------------

class ScTabDeletedHint : public SfxHint
{
private:
	USHORT	nTab;
public:
			TYPEINFO();
	virtual	~ScTabDeletedHint();

	USHORT	GetTab()	{ return nTab; }
};

class ScTabSizeChangedHint : public SfxHint
{
private:
	USHORT	nTab;
public:
			TYPEINFO();
			ScTabSizeChangedHint( USHORT nTabNo = USHRT_MAX );
	virtual	~ScTabSizeChangedHint();

	USHORT	GetTab()	{ return nTab; }
};

// -----------------------------------------------------------------------


class ScDrawLayer: public FmFormModel
{
private:
	SvStorageRef	xPictureStorage;
	String			aName;
	ScDocument*		pDoc;
	SdrUndoGroup*	pUndoGroup;
	BOOL			bRecording;
	BOOL			bAdjustEnabled;
	BOOL			bHyphenatorSet;

private:
	void			MoveAreaTwips( USHORT nTab, const Rectangle& rArea, const Point& rMove,
								const Point& rTopLeft );

public:
					ScDrawLayer( ScDocument* pDocument, const String& rName );
	virtual			~ScDrawLayer();

	virtual SdrPage*  AllocPage(FASTBOOL bMasterPage);
	virtual void	SetChanged( FASTBOOL bFlg = TRUE );

	virtual SvStream* GetDocumentStream(SdrDocumentStreamInfo& rStreamInfo) const;


	void			ReleasePictureStorage();


	void			ScAddPage( USHORT nTab );
	void			ScRenamePage( USHORT nTab, const String& rNewName );
					// inkl. Inhalt, bAlloc=FALSE -> nur Inhalt

	ScDocument*		GetDocument() const { return pDoc; }

	void			UpdateBasic();				// DocShell-Basic in DrawPages setzen

	void			Load( SvStream& rStream );
	void			Store( SvStream& rStream ) const;

	BOOL			GetPrintArea( ScRange& rRange, BOOL bSetHor, BOOL bSetVer ) const;

					//		automatische Anpassungen

	void			EnableAdjust( BOOL bSet = TRUE )	{ bAdjustEnabled = bSet; }

	BOOL			IsRecording()			{ return bRecording; }
	void			AddCalcUndo( SdrUndoAction* pUndo );

	void			WidthChanged( USHORT nTab, USHORT nCol, long nDifTwips );
	void			HeightChanged( USHORT nTab, USHORT nRow, long nDifTwips );

	BOOL			HasObjectsInRows( USHORT nTab, USHORT nStartRow, USHORT nEndRow );



	void			SetPageSize( USHORT nPageNo, const Size& rSize );

					//	GetVisibleName: name for navigator etc: GetPersistName or GetName
					//	(ChartListenerCollection etc. must use GetPersistName directly)

	SdrObject*		GetNamedObject( const String& rName, USHORT nId, USHORT& rFoundTab ) const;
                    // if pnCounter != NULL, the search for a name starts with this index + 1,
                    // and the index really used is returned.

	// Verankerung setzen und ermitteln
	static void		SetAnchor( SdrObject*, ScAnchorType );
	static ScAnchorType	GetAnchor( const SdrObject* );

	// Positionen fuer Detektivlinien
	static ScDrawObjData* GetObjData( SdrObject* pObj, BOOL bCreate=FALSE );

	// Image-Map
	static ScIMapInfo* GetIMapInfo( SdrObject* pObj );


private:
	static SvPersist* pGlobalDrawPersist;			// fuer AllocModel
public:
};


} //namespace binfilter
#endif


