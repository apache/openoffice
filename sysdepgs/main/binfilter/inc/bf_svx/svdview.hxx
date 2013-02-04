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



#ifndef _SVDVIEW_HXX
#define _SVDVIEW_HXX

#include <bf_svtools/bf_solar.h>

// HACK to avoid too deep includes and to have some
// levels free in svdmark itself (MS compiler include depth limit)
#ifndef _SVDHDL_HXX
#include <bf_svx/svdhdl.hxx>
#endif

#include <bf_svx/svdcrtv.hxx>
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Klassenhierarchie der View:
//         SfxListener
//         SdrPaintView    PntV   Action            ModChg   Attr   Notify
//         SdrSnapView     SnpV   Action
//
//         SdrMarkView     MrkV   Action   MrkChg   ModChg          Notify
//
//         SdrEditView     EdtV            MrkChg   ModChg   Attr
//         SdrPolyEditView PoEV
//         SdrGlueEditView GlEV
//         SdrObjEditView  EdxV   Action            ModChg   Attr   Notify
//
//         SdrExchangeView XcgV
//         SdrDragView     DrgV   Action
//
//         SdrCreateView   CrtV   Action
//         SdrView         View
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************
//   Vorausdeklarationen
//************************************************************

class SvxURLField;

//************************************************************
//   Defines
//************************************************************

enum SdrViewContext {SDRCONTEXT_STANDARD,
					 SDRCONTEXT_POINTEDIT,
					 SDRCONTEXT_GLUEPOINTEDIT,
					 SDRCONTEXT_TEXTEDIT,
					 SDRCONTEXT_GRAPHIC};

enum SdrEventKind  {SDREVENT_NONE,
					SDREVENT_TEXTEDIT,
					SDREVENT_MOVACTION,
					SDREVENT_ENDACTION,
					SDREVENT_BCKACTION,
					SDREVENT_BRKACTION,
					SDREVENT_ENDCREATE,
					SDREVENT_ENDDRAG,
					SDREVENT_MARKOBJ,
					SDREVENT_MARKPOINT,
					SDREVENT_MARKGLUEPOINT,
					SDREVENT_BEGMARK,
					SDREVENT_BEGINSOBJPOINT,
					SDREVENT_ENDINSOBJPOINT,
					SDREVENT_BEGINSGLUEPOINT,
					SDREVENT_BEGDRAGHELPLINE,
					SDREVENT_BEGDRAGOBJ,
					SDREVENT_BEGCREATEOBJ,
					SDREVENT_BEGMACROOBJ,
					SDREVENT_BEGTEXTEDIT,
					SDREVENT_ENDMARK,
					SDREVENT_BRKMARK,
					SDREVENT_EXECUTEURL};

#define SDRMOUSEBUTTONDOWN 1
#define SDRMOUSEMOVE       2
#define SDRMOUSEBUTTONUP   3

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@ @@ @@ @@@@@ @@   @@
//  @@ @@ @@ @@    @@   @@
//  @@ @@ @@ @@    @@ @ @@
//  @@@@@ @@ @@@@  @@@@@@@
//   @@@  @@ @@    @@@@@@@
//   @@@  @@ @@    @@@ @@@
//    @   @@ @@@@@ @@   @@
//
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class SdrView: public SdrCreateView
{
	friend class				SdrPageView;

	unsigned					bNoExtendedMouseDispatcher : 1;
	unsigned					bNoExtendedKeyDispatcher : 1;
	unsigned					bNoExtendedCommandDispatcher : 1;
	unsigned					bTextEditOnObjectsWithoutTextIfTextTool : 1;

protected:
	// Abgeleitete Klassen ueberladen lediglich die Methoden ReadData() und
	// WriteData(). Diese werden von den Streamoperatoren der View gerufen.
	// Wichtig ist, dass die ueberladen Methoden der abgeleiteten Klasse
	// als erstes die Methode der Basisklasse rufen, da sonst der CharSet
	// am Stream nicht korrekt gesetzt wird, ...
	virtual void WriteRecords(SvStream& rOut) const;
	virtual BOOL ReadRecord(const SdrIOHeader& rViewHead, const SdrNamedSubRecord& rSubHead, SvStream& rIn);

	virtual void SFX_NOTIFY(SfxBroadcaster& rBC, const TypeId& rBCType, const SfxHint& rHint, const TypeId& rHintType);

public:
	TYPEINFO();
	SdrView(SdrModel* pModel1, OutputDevice* pOut);
	SdrView(SdrModel* pModel1);
	virtual ~SdrView();

	// Default sind alle Dispatcher aktiviert. Will die App z.B. fuer
	// Sonderbehandlungen im MouseDispatcher eingreifen, so muss sie
	// den erweiterten MouseDispather mit unten stehender Methode deaktivieren
	// und selbst nachimplementieren. Beispiel fuer MouseButtonDown:
	//      SdrHitKind eHit=pSdrView->PickAnything(rMEvt,SDRMOUSEBUTTONDOWN,aVEvt);
	//      ... hier Applikationsspezifischer Eingriff ...
	//      pSdrView->DoMouseEvent(aVEvt);
	//      SetPointer(GetPreferedPointer(...))
	//      CaptureMouse(...)
	void EnableExtendedMouseEventDispatcher(BOOL bOn) { bNoExtendedMouseDispatcher = !bOn; }
	BOOL IsExtendedMouseEventDispatcherEnabled() const { return bNoExtendedMouseDispatcher; }

	void EnableExtendedKeyInputDispatcher(BOOL bOn) { bNoExtendedKeyDispatcher=!bOn; }
	BOOL IsExtendedKeyInputDispatcherEnabled() const { return bNoExtendedKeyDispatcher; }

	void EnableExtendedCommandEventDispatcher(BOOL bOn) { bNoExtendedCommandDispatcher=!bOn; }
	BOOL IsExtendedCommandEventDispatcherEnabled() const { return bNoExtendedCommandDispatcher; }

	void EnableTextEditOnObjectsWithoutTextIfTextTool(BOOL bOn) { bTextEditOnObjectsWithoutTextIfTextTool=bOn; }
	BOOL IsEnableTextEditOnObjectsWithoutTextIfTextToolEnabled() const { return bTextEditOnObjectsWithoutTextIfTextTool; }


	// Die Methoden beruecksichtigen den jeweiligen Kontex:
	// - Einfaches Zeichnen
	// - Punktbearbeitungs-Mode
	// - Klebepunkt-Editmode
	// - TextEdit
	// - ... to be continued
	BOOL HasMarked() const { return HasMarkedObj(); }
	void UnmarkAll();


	virtual void DeleteMarked();

	// Strings werden beim rausstreamen in den am Stream eingestellten
	// StreamCharSet konvertiert.
	// Abgeleitete Klassen mit eigenen persistenten Membern ueberladen
	// die virtuellen Methoden ReadData() und WriteData().
	friend SvStream& operator<<(SvStream& rOut, const SdrView& rView);

	// Wenn das Model im Stream in einem fremden CharSet vorliegt
	// wird beim einstreamen implizit auf den SystemCharSet konvertiert.
	// Abgeleitete Klassen mit eigenen persistenten Membern ueberladen
	// die virtuellen Methoden ReadData() und WriteData().
	friend SvStream& operator>>(SvStream& rIn, SdrView& rView);


	virtual void onAccessibilityOptionsChanged();
};

}//end of namespace binfilter
#endif //_SVDVIEW_HXX

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Die App macht sich zunaechst ein SdrModel.
// Anschliessend oeffnet sie ein Win und erzeugt dann eine SdrView.
// An der SdrView meldet sie dann mit der Methode ShowPage() eine Seite an.
// Eine SdrView kann in beliebig vielen Fenstern gleichzeitig angezeigt werden.
// Intern:
// Eine SdrView kann beliebig viele Seiten gleichzeitig anzeigen. Seiten
// werden an- und abgemeldet mit ShowPage()/HidePage(). Fuer jede angemeldete
// Seite wird eine SdrPageView-Instanz im Container aPages angelegt. Bei
// gleichzeitiger Anzeige mehrerer Seiten ist darauf zu achten, dass der Offset-
// Parameter von ShowPage() der Seitengroesse angepasst ist, da sich sonst die
// Seiten ueberlappen koennten.
//
// Elementare Methoden:
// ~~~~~~~~~~~~~~~~~~~~
//   Einfache Events:
//   ~~~~~~~~~~~~~~~~
//     BOOL KeyInput(const KeyEvent& rKEvt, Window* pWin);
//     BOOL MouseButtonDown(const MouseEvent& rMEvt, Window* pWin);
//     BOOL MouseButtonUp(const MouseEvent& rMEvt, Window* pWin);
//     BOOL MouseMove(const MouseEvent& rMEvt, Window* pWin);
//     BOOL Command(const CommandEvent& rCEvt, Window* pWin);
//
//   Exchange (Clipboard derzeit noch ohne SdrPrivateData):
//   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//     BOOL Cut(ULONG nFormat=SDR_ANYFORMAT);
//     BOOL Yank(ULONG nFormat=SDR_ANYFORMAT);
//     BOOL Paste(Window* pWin=NULL, ULONG nFormat=SDR_ANYFORMAT);
//
//   SfxItems:
//   ~~~~~~~~~
//     BOOL GetAttributes(SfxItemSet& rTargetSet, BOOL bOnlyHardAttr=FALSE) const;
//     BOOL SetAttributes(const SfxItemSet& rSet, BOOL bReplaceAll);
//     SfxStyleSheet* GetStyleSheet() const;
//     BOOL SetStyleSheet(SfxStyleSheet* pStyleSheet, BOOL bDontRemoveHardAttr);
//
///////////////////////////////////////////////////////////////////////////////////////////////// */

