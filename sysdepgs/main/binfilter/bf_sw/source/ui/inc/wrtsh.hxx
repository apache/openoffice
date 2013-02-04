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


#ifndef _WRTSH_HXX
#define _WRTSH_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _FESH_HXX
#include <fesh.hxx>
#endif
#ifndef _SORTOPT_HXX
#include <sortopt.hxx>
#endif
#include <tools/debug.hxx>
namespace com { namespace sun { namespace star { namespace util {
	struct SearchOptions;
} } } }
class Window;
class SbxArray;
class SvGlobalName;
class Timer;

namespace binfilter {
class SvInPlaceObjectRef;
class SvEmbeddedObjectRef;
class SvxMacro;
class SwDoc;
class SpellCheck;
class SwViewOption;
class SwFlyFrmAttrMgr;
class SwField;
class SwTOXBase;
class SdrView;
class SwView;
class SwInputFieldList;
class SwSection;
class SwFmtINetFmt;
class SvxINetBookmark;
class SvxAutoCorrect;
class NaviContentBookmark;
struct SwCallMouseEvent;
class DropEvent;
struct ChartSelectionInfo;
class SfxStringListItem;

//STRIP008 namespace com { namespace sun { namespace star { namespace util {
//STRIP008 	struct SearchOptions;
//STRIP008 } } } }


class SwWrtShell: public SwFEShell
{
public:
	typedef long (SwWrtShell::*SELECTFUNC)(const Point *, BOOL bProp );

	SELECTFUNC	fnDrag;
	SELECTFUNC	fnSetCrsr;
	SELECTFUNC	fnEndDrag;
	SELECTFUNC	fnKillSel;

	// Alle Selektionen aufheben

	//setzt den Cursorstack nach dem Bewegen mit PageUp/-Down
	//zurueck, falls ein Stack aufgebaut ist
	inline void ResetCursorStack();

	enum SelectionType
	{
		SEL_TXT 	  = CNT_TXT, // Text, niemals auch Rahmen	0x0001
		SEL_GRF 	  = CNT_GRF, // Grafik						0x0002
		SEL_OLE 	  = CNT_OLE, // OLE 						0x0010
		SEL_FRM 	  = 0x0020,	// Rahmen, keine Inhaltsform
		SEL_NUM 	  = 0x0040,	// NumListe
		SEL_TBL 	  = 0x0080,	// Cursor steht in Tabelle
		SEL_TBL_CELLS = 0x0100,	// Tabellenzellen sind selektiert
		SEL_DRW 	  = 0x0200,	// Zeichenobjekte (Rechteck, Kreis...)
		SEL_DRW_TXT   = 0x0400,	// Draw-Textobjekte im Editmode
		SEL_BEZ       = 0x0800,	// Bezierobjekte editieren
		SEL_DRW_FORM  = 0x1000	// Zeichenobjekte: DB-Forms
	};
	BOOL	IsModePushed() const { return 0 != pModeStack; }

	BOOL	IsInSelect() const { return bInSelect; }
	void	SetInSelect(BOOL bSel = TRUE) { bInSelect = bSel; }

	BOOL	IsStdMode() const { return !bExtMode && !bAddMode; }

	BOOL	IsExtMode() const { return bExtMode; }

	BOOL	IsAddMode() const { return bAddMode; }

	BOOL	IsInsMode() const { return bIns; }

	BOOL	IsSelFrmMode() const { return bLayoutMode; }

	// Tabellenzellen selektieren fuer Bearbeiten von Formeln in der Ribbonbar
	inline void SelTblCells( const Link &rLink, BOOL bMark = TRUE );
	inline void	EndSelTblCells();

	//Wortweisen oder zeilenweisen Selektionsmodus verlassen. Wird
	//in der Regel in MB-Up gerufen.
	BOOL	IsExtSel() const { return bSelWrd || bSelLn; }

	//Basiscursortravelling
typedef FASTBOOL (SwWrtShell:: *FNSimpleMove)();

	//setzen des Cursors; merken der alten Position fuer Zurueckblaettern.
	DECL_LINK( ExecFlyMac, void * );


	// Felder Update

	BOOL	IsNoEdit() const { return bNoEdit; }

    // Loeschen

	// loescht auch Rahmen bzw. setzt den Cursor in den Rahmen,
	// wenn bDelFrm == FALSE ist

	// Prueft, ob eine Wortselektion vorliegt.
	// Gemaess den Regeln fuer intelligentes Cut / Paste
	// werden umgebende Spaces rausgeschnitten.
	// Liefert Art der Wortselektion zurueck (siehe enum)
	enum {
			NO_WORD = 0,
			WORD_SPACE_BEFORE = 1,
			WORD_SPACE_AFTER = 2,
			WORD_NO_SPACE = 3
		};

	virtual void CalcAndSetScale( SvEmbeddedObjectRef xObj,
								  const SwRect *pFlyPrtRect = 0,
								  const SwRect *pFlyFrmRect = 0 );
	virtual void ConnectObj( SvInPlaceObjectRef xIPObj, const SwRect &rPrt,
						const SwRect &rFrm );

	// Vorlagen und Formate

	// enum gibt an, was geschehen soll, wenn das Style nicht gefunden wurde
	enum GetStyle { GETSTYLE_NOCREATE, 			// keins anlegen
					GETSTYLE_CREATESOME,        // falls auf PoolId mapt anlegen
					GETSTYLE_CREATEANY };		// ggfs Standard returnen



	// Aktuelle Vorlage anhand der geltenden Attribute aendern

	enum DoType { UNDO, REDO, REPEAT };

	String	GetDoString( DoType eDoType ) const;

	//Suchen oder Ersetzen




	// Aktion vor Cursorbewegung
	// Hebt gfs. Selektionen auf, triggert Timer und GCAttr()

	// Eingabefelder updaten

	//"Handler" fuer Anederungen an der DrawView - fuer Controls.
	virtual void DrawSelChanged( SdrView * );

	// jump to the next / previous hyperlink - inside text and also
	// on graphics

	// Zugehoerige SwView ermitteln
	const SwView&		GetView() const { return rView; }
	SwView&				GetView() { return rView; }

	//Weil es sonst keiner macht, gibt es hier eine ExecMacro()
	// rufe ins dunkle Basic/JavaScript


	// ein Klick aus das angegebene Feld. Der Cursor steht auf diesem.
	// Fuehre die vor definierten Aktionen aus.
	inline BOOL IsInClickToEdit() const ;

	SwWrtShell(SwDoc&, Window*, SwView&, SwRootFrm*, SwViewOption const*);
	virtual ~SwWrtShell();

private:


	String	GetWrdDelim();
	String	GetSDelim();
	String	GetBothDelim();

	struct ModeStack
	{
		ModeStack	*pNext;
		BOOL		bAdd,
					bExt,
					bIns;
		ModeStack(ModeStack *pNextMode, BOOL _bIns, BOOL _bExt, BOOL _bAdd):
			pNext(pNextMode),
			bIns(_bIns),
			bExt(_bExt),
			bAdd(_bAdd) {}
	} *pModeStack;

	// Cursor bei PageUp / -Down mitnehmen
	enum PageMove
	{
		MV_NO,
		MV_PAGE_UP,
		MV_PAGE_DOWN
	}  ePageMove;

	struct CrsrStack
	{
		Point aDocPos;
		CrsrStack *pNext;
		BOOL bValidCurPos : 1;
		BOOL bIsFrmSel : 1;
		SwTwips lOffset;

		CrsrStack( BOOL bValid, BOOL bFrmSel, const Point &rDocPos,
					SwTwips lOff, CrsrStack *pN )
			: aDocPos(rDocPos), lOffset(lOff), pNext(pN)
		{
			bValidCurPos = bValid;
			bIsFrmSel = bFrmSel;
		}

	} *pCrsrStack;

	SwView	&rView;

	Point	aDest;
	BOOL	bDestOnStack;
	BOOL	HasCrsrStack() const { return 0 != pCrsrStack; }
	BOOL	PopCrsr(BOOL bUpdate, BOOL bSelect = FALSE);

	// ENDE Cursor bei PageUp / -Down mitnehmen

		//	Selektionen
	BOOL	bIns			:1;
	BOOL	bInSelect		:1;
	BOOL	bExtMode		:1;
	BOOL	bAddMode		:1;
	BOOL	bLayoutMode		:1;
	BOOL	bNoEdit			:1;
	BOOL	bCopy			:1;
	BOOL	bSelWrd			:1;
	BOOL	bSelLn			:1;
	BOOL	bIsInClickToEdit:1;
	BOOL 	bClearMark		:1;		// Selektion fuer ChartAutoPilot nicht loeschen

	Point	aStart;
	Link	aSelTblLink;

	SELECTFUNC	fnLeaveSelect;

	//setzt den Cursorstack nach dem Bewegen mit PageUp/-Down zurueck.

	void	SttDragDrop(Timer *);

	long	StdSelect(const Point *, BOOL bProp=FALSE );


	//Verschieben von Text aus Drag and Drop; Point ist
	//Destination fuer alle Selektionen.

	void	LeaveExtSel() { bSelWrd = bSelLn = FALSE;}


	enum BookMarkMove
	{
		BOOKMARK_INDEX,
		BOOKMARK_NEXT,
		BOOKMARK_PREV,
		BOOKMARK_LAST_LAST_ENTRY
	};


};

 inline void SwWrtShell::ResetCursorStack()
 {
 	if ( HasCrsrStack() )
	{DBG_BF_ASSERT(0, "STRIP"); }//STRIP001 _ResetCursorStack();
 }

inline void SwWrtShell::SelTblCells(const Link &rLink, BOOL bMark )
{
	SetSelTblCells( TRUE );
	bClearMark = bMark;
	aSelTblLink = rLink;
}
inline void SwWrtShell::EndSelTblCells()
{
	SetSelTblCells( FALSE );
	bClearMark = TRUE;
}

inline BOOL SwWrtShell::IsInClickToEdit() const { return bIsInClickToEdit; }


} //namespace binfilter
#endif
