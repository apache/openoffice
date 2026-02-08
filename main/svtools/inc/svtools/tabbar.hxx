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

#ifndef _TABBAR_HXX
#define _TABBAR_HXX

#include "svtools/svtdllapi.h"
#include <tools/link.hxx>
#include <vcl/window.hxx>

class MouseEvent;
class TrackingEvent;
class DataChangedEvent;
class ImplTabBarList;
class ImplTabButton;
class ImplTabSizer;
class TabBarEdit;

// -----------------
// - Dokumentation -
// -----------------

/*

Erlaubte StyleBits
------------------

WB_SCROLL       - Die Tabs können über ein Extra-Feld gescrollt werden
WB_MINSCROLL    - Die Tabs können über 2 zusätzliche Buttons gescrollt werden
WB_RANGESELECT  - Zusammenhängende Bereiche können selektiert werden
WB_MULTISELECT  - Einzelne Tabs können selektiert werden
WB_BORDER       - Oben und unten wird ein Strich gezeichnet
WB_TOPBORDER    - Oben wird ein Border gezeichnet
WB_3DTAB        - Die Tabs und der Border werden in 3D gezeichnet
WB_DRAG         - Vom TabBar wird ein StartDrag-Handler gerufen, wenn
                  Drag and Drop gestartet werden soll. Es wird außerdem
                  im TabBar mit EnableDrop() Drag and Drop eingeschaltet.
WB_SIZEABLE     - Vom TabBar wird ein Split-Handler gerufen, wenn der Anwender
                  den TabBar in der Breite ändern will
WB_STDTABBAR    - WB_BORDER

Wenn man den TabBar zum Beispiel als Property-Bar benutzen möchte, sollten
die WinBits WB_TOPBORDER und WB_3DTAB anstatt WB_BORDER gesetzt werden.


Erlaubte PageBits
-----------------

TPB_SPECIAL     - Andere Darstellung des TabTextes, zum Beispiel für
                  Szenario-Seiten.

Handler
-------

Select          - Wird gerufen, wenn eine Tab selektiert oder
                  deselektiert wird
DoubleClick     - Wird gerufen, wenn ein DoubleClick im TabBar ausgelöst
                  wurde. Innerhalb des Handlers liefert GetCurPageId() die
                  angeklickte Tab zurück oder 0, wenn keine Tab angeklickt
                  wurde
ActivatePage    - Wird gerufen, wenn eine andere Seite aktiviert wird.
                  GetCurPageId() gibt die aktivierte Seite zurück.
DeactivatePage  - Wird gerufen, wenn eine Seite deaktiviert wird. Wenn
                  eine andere Seite aktiviert werden darf, muss sal_True
                  zurückgegeben werden, wenn eine andere Seite von
                  der Aktivierung ausgeschlossen werden soll, muss
                  sal_False zurückgegeben werden. GetCurPageId() gibt die
                  zu deaktivierende Seite zurück.


Drag and Drop
-------------

Für Drag and Drop muss das WinBit WB_DRAG gesetzt werden. Außerdem
muss der Command-, QueryDrop-Handler und der Drop-Handler überlagert
werden. Dabei muss in den Handlern folgendes implementiert werden:

Command         - Wenn in diesem Handler das Dragging gestartet werden
                  soll, muss StartDrag() gerufen werden. Diese Methode
                  selektiert dann den entsprechenden Eintrag oder gibt
                  sal_False zurück, wenn das Dragging nicht durchgefuhert
                  werden kann.

QueryDrop       - Dieser Handler wird von StarView immer dann gerufen, wenn
                  bei einem Drag-Vorgang die Maus über das Fenster gezogen
                  wird (siehe dazu auch SV-Doku). In diesem Handler muss
                  festgestellt werden, ob ein Drop möglich ist. Die
                  Drop-Position kann im TabBar mit ShowDropPos() angezeigt
                  werden. Beim Aufruf muss die Position vom Event übergeben
                  werden. Wenn sich die Position am linken oder rechten
                  Rand befindet, wird automatisch im TabBar gescrollt.
                  Diese Methode gibt auch die entsprechende Drop-Position
                  zurück, die auch für ein Drop gebraucht wird. Wenn das
                  Fenster beim Drag verlassen wird, kann mit HideDropPos()
                  die DropPosition wieder weggenommen werden. Es ist dadurch
                  auch möglich, ein von außerhalb des TabBars ausgelöstes
                  Drag zu verarbeiten.

Drop            - Im Drop-Handler müssen dann die Pages verschoben werden,
                  oder die neuen Pages eingefügt werden. Die entsprechende
                  Drop-Postion kann mit ShowDropPos() ermittelt werden.

Folgende Methoden werden für Drag and Drop gebraucht und müssen von
den Handlern gerufen werden:

StartDrag       - Muss aus dem Commnad-Handler gerufen werden. Als Parameter
                  muss der CommandEvent übergeben werden und eine Referenz
                  auf eine Region. Diese Region muss dann bei ExecuteDrag()
                  übergeben werden, wenn der Rückgabewert sagt, das
                  ExecuteDrag durchgeführt werden soll. Falls der Eintrag
                  nicht selektiert ist, wird er vorher als aktueller
                  Eintrag gesetzt. Es ist daher darauf zu achten, das aus
                  dieser Methode heraus der Select-Handler gerufen werden
                  kann.

ShowDropPos     - Diese Methode muss vom QueryDrop-Handler gerufen werden,
                  damit der TabBar anzeigt, wo die Tabs eingefügt werden.
                  Diese Methode kann auch im Drop-Handler benutzt werden,
                  um die Position zu ermitteln wo die Tabs eingefügt werden
                  sollen. In der Methode muss die Position vom Event
                  übergeben werden. Diese Methode gibt die Position zurück,
                  wo die Tabs eingefügt werden sollen.

HideDropPos     - Diese Methode nimmt die vorher mit ShowDropPos() angezeigte
                  DropPosition wieder zurück. Diese Methode sollte dann
                  gerufen werden, wenn bei QueryDrop() das Fenster verlassen
                  wird oder der Dragvorgang beendet wurde.

Folgende Methoden können eingesetzt werden, wenn bei D&D die Seiten
umgeschaltet werden sollen:

SwitchPage      - Diese Methode muss vom QueryDrop-Handler gerufen werden,
                  wenn die Seite über der sich der Mousepointer befindet,
                  umgeschaltet werden soll. Diese Methode sollte jedes mal
                  gerufen werden, wenn der QueryDrop-Handler gerufen wird.
                  Das umschalten der Seite passiert zeitverzögert (500 ms)
                  und wird automatisch von dieser Methode verwaltet.
                  In der Methode muss die Position vom Event übergeben
                  werden. Diese Methode gibt sal_True zurück, wenn die Page
                  umgeschaltet wurde.

EndSwitchPage   - Diese Methode setzt die Daten für das umschalten der
                  Seiten zurück. Diese Methode sollte dann gerufen werden,
                  wenn bei QueryDrop() das Fenster verlassen wird oder
                  der Dragvorgang beendet wurde.

IsInSwitching   - Mit dieser Methode kann im ActivatePage()/DeactivatePage()
                  abgefragt werden, ob dies durch SwitchPage() veranlasst
                  wurde. So kann dann beispielsweise in DeactivatePage()
                  das Umschalten ohne eine Fehlerbox verhindert werden.


Fenster-Resize
--------------

Wenn das Fenster vom Anwender in der Breite geändert werden kann, dann
muss das WinBit WB_SIZEABLE gesetzt werden. In diesem Fall muss noch
folgender Handler überlagert werden:

Split           - Wenn dieser Handler gerufen wird, sollte das Fenster
                  auf die Breite angepasst werden, die von GetSplitSize()
                  zurückgegeben wird. Dabei wird keine minimale und
                  maximale Breite berücksichtigt. Eine minimale Breite
                  kann mit GetMinSize() abgefragt werden und die maximale
                  Breite muss von der Anwendung selber berechnet werden.
                  Da nur Online-Resize unterstützt wird, muss das Fenster
                  innerhalb dieses Handlers in der Breite geändert
                  werden und eventuell abhängige Fenster ebenfalls. Für
                  diesen Handler kann auch mit SetSplitHdl() ein
                  Link gesetzt werden.

Folgende Methoden liefern beim Splitten weitere Informationen:

GetSplitSize()  - Liefert die Breite des TabBars zurück, auf die der
                  Anwender das Fenster resizen will. Dabei wird keine
                  minimale oder maximale Breite berücksichtigt. Es wird
                  jedoch nie eine Breite < 5 zurückgeliefert. Diese Methode
                  liefert nur solange richtige Werte, wie Splitten aktiv
                  ist.

GetMinSize()    - Mit dieser Methode kann eine minimale Fensterbreite
                  abgefragt werden, so das min. etwas eines Tabs sichtbar
                  ist. Jedoch kann der TabBar immer noch schmaler gesetzt
                  werden, als die Breite, die diese Methode zurückliefert.
                  Diese Methode kann auch aufgerufen werden, wenn kein
                  Splitten aktiv ist.


Edit-Modus
----------

Der Tabbar bietet auch Möglichkeiten, dass der Anwender in den Tabreitern
die Namen ändern kann.

EnableEditMode  - Damit kann eingestellt werden, das bei Alt+LeftClick
                  StartEditMode() automatisch vom TabBar gerufen wird.
                  Im StartRenaming()-Handler kann dann das Umbenennen
                  noch abgelehnt werden.
StartEditMode   - Mit dieser Methode wird der EditModus auf einem
                  Tab gestartet. sal_False wird zurückgegeben, wenn
                  der Editmodus schon aktiv ist, mit StartRenaming()
                  der Modus abgelehnt wurde oder kein Platz zum
                  Editieren vorhanden ist.
EndEditMode     - Mit dieser Methode wird der EditModus beendet.
SetEditText     - Mit dieser Methode kann der Text im AllowRenaming()-
                  Handler noch durch einen anderen Text ersetzt werden.
GetEditText     - Mit dieser Methode kann im AllowRenaming()-Handler
                  der Text abgefragt werden, den der Anwender eingegeben
                  hat.
IsInEditMode    - Mit dieser Methode kann abgefragt werden, ob der
                  Editmodus aktiv ist.
IsEditModeCanceled      - Mit dieser Methode kann im EndRenaming()-
                          Handler abgefragt werden, ob die Umbenennung
                          abgebrochen wurde.
GetEditPageId   - Mit dieser Methode wird in den Renaming-Handlern
                  abgefragt, welcher Tab umbenannt wird/wurde.

StartRenaming() - Dieser Handler wird gerufen, wenn über StartEditMode()
                  der Editmodus gestartet wurde. Mit GetEditPageId()
                  kann abgefragt werden, welcher Tab umbenannt werden
                  soll. sal_False sollte zurückgegeben werden, wenn
                  der Editmodus nicht gestartet werden soll.
AllowRenaming() - Dieser Handler wird gerufen, wenn der Editmodus
                  beendet wird (nicht bei Cancel). In diesem Handler
                  kann dann getestet werden, ob der Text OK ist.
                  Mit GetEditPageId() kann abgefragt werden, welcher Tab
                  umbenannt wurde.
                  Es sollte einer der folgenden Werte zurückgegeben
                  werden:
                  TAB_RENAMING_YES
                  Der Tab wird umbenannt.
                  TAB_RENAMING_NO
                  Der Tab wird nicht umbenannt, der Editmodus bleibt
                  jedoch aktiv, so das der Anwender den Namen
                  entsprechend anpassen kann.
                  TAB_RENAMING_CANCEL
                  Der Editmodus wird abgebrochen und der alte
                  Text wieder hergestellt.
EndRenaming()   - Dieser Handler wird gerufen, wenn der Editmodus
                  beendet wurde. Mit GetEditPageId() kann abgefragt
                  werden, welcher Tab umbenannt wurde. Mit
                  IsEditModeCanceled() kann abgefragt werden, ob der
                  Modus abgebrochen wurde und der Name dadurch nicht
                  geändert wurde.


Maximale Pagebreite
-------------------

Die Pagebreite der Tabs kann begrenzt werden, damit ein einfacheres
Navigieren über diese möglich ist. Wenn der Text dann nicht komplett
angezeigt werden kann, wird er mit ... abgekürzt und in der Tip-
oder der aktiven Hilfe (wenn kein Hilfetext gesetzt ist) wird dann der
ganze Text angezeigt. Mit EnableAutoMaxPageWidth() kann eingestellt
werden, ob die maximale Pagebreite sich nach der gerade sichtbaren
Breite richten soll (ist der default). Ansonsten kann auch die
maximale Pagebreite mit SetMaxPageWidth() (in Pixeln) gesetzt werden
(die AutoMaxPageWidth wird dann ignoriert).


KontextMenu
-----------

Wenn ein kontextsensitives PopupMenu anzeigt werden soll, muss der
Command-Handler überlagert werden. Mit GetPageId() und bei
Übergabe der Mausposition kann ermittelt werden, ob der Mausclick
über einem bzw. über welchem Item durchgeführt wurde.
*/

// -----------
// - WinBits -
// -----------

#define WB_RANGESELECT      ((WinBits)0x00200000)
#define WB_MULTISELECT      ((WinBits)0x00400000)
#define WB_TOPBORDER        ((WinBits)0x04000000)
#define WB_3DTAB            ((WinBits)0x08000000)
#define WB_MINSCROLL        ((WinBits)0x20000000)
#define WB_STDTABBAR        WB_BORDER

// ------------------
// - TabBarPageBits -
// ------------------

typedef sal_uInt16 TabBarPageBits;

// ------------------------
// - Bits for TabBarPages -
// ------------------------

#define TPB_SPECIAL         ((TabBarPageBits)0x0001)

// ----------------
// - TabBar-Types -
// ----------------

#define TABBAR_RENAMING_YES    ((long)sal_True)
#define TABBAR_RENAMING_NO     ((long)sal_False)
#define TABBAR_RENAMING_CANCEL ((long)2)

// ----------
// - TabBar -
// ----------
struct TabBar_Impl;

class SVT_DLLPUBLIC TabBar : public Window
{
	friend class    ImplTabButton;
	friend class    ImplTabSizer;

private:
	ImplTabBarList* mpItemList;
	ImplTabButton*  mpFirstBtn;
	ImplTabButton*  mpPrevBtn;
	ImplTabButton*  mpNextBtn;
	ImplTabButton*  mpLastBtn;
	TabBar_Impl*    mpImpl;
	TabBarEdit*     mpEdit;
	XubString       maEditText;
	Color           maSelColor;
	Color           maSelTextColor;
	Size            maWinSize;
	long            mnMaxPageWidth;
	long            mnCurMaxWidth;
	long            mnOffX;
	long            mnOffY;
	long            mnLastOffX;
	long            mnSplitSize;
	sal_uLong           mnSwitchTime;
	WinBits         mnWinStyle;
	sal_uInt16          mnCurPageId;
	sal_uInt16          mnFirstPos;
	sal_uInt16          mnDropPos;
	sal_uInt16          mnSwitchId;
	sal_uInt16          mnEditId;
	sal_Bool            mbFormat;
	sal_Bool            mbFirstFormat;
	sal_Bool            mbSizeFormat;
	sal_Bool            mbAutoMaxWidth;
	sal_Bool            mbInSwitching;
	sal_Bool            mbAutoEditMode;
	sal_Bool            mbEditCanceled;
	sal_Bool            mbDropPos;
	sal_Bool            mbInSelect;
	sal_Bool            mbSelColor;
	sal_Bool            mbSelTextColor;
	sal_Bool            mbMirrored;
	Link            maSelectHdl;
	Link            maDoubleClickHdl;
	Link            maSplitHdl;
	Link            maActivatePageHdl;
	Link            maDeactivatePageHdl;
	Link            maStartRenamingHdl;
	Link            maAllowRenamingHdl;
	Link            maEndRenamingHdl;

    using Window::ImplInit;
    SVT_DLLPRIVATE void            ImplInit( WinBits nWinStyle );
    SVT_DLLPRIVATE void            ImplInitSettings( sal_Bool bFont, sal_Bool bBackground );
    SVT_DLLPRIVATE void            ImplGetColors( Color& rFaceColor, Color& rFaceTextColor,
                                   Color& rSelectColor, Color& rSelectTextColor );
    SVT_DLLPRIVATE void            ImplShowPage( sal_uInt16 nPos );
    SVT_DLLPRIVATE sal_Bool            ImplCalcWidth();
    SVT_DLLPRIVATE void            ImplFormat();
    SVT_DLLPRIVATE sal_uInt16          ImplGetLastFirstPos();
    SVT_DLLPRIVATE void            ImplInitControls();
    SVT_DLLPRIVATE void            ImplEnableControls();
    SVT_DLLPRIVATE void			ImplSelect();
    SVT_DLLPRIVATE void			ImplActivatePage();
    SVT_DLLPRIVATE long			ImplDeactivatePage();
                    DECL_DLLPRIVATE_LINK( ImplClickHdl, ImplTabButton* );

public:
	static const sal_uInt16 APPEND;
	static const sal_uInt16 PAGE_NOT_FOUND;

                    TabBar( Window* pParent, WinBits nWinStyle = WB_STDTABBAR );
    virtual         ~TabBar();

	virtual void    MouseMove( const MouseEvent& rMEvt );
	virtual void    MouseButtonDown( const MouseEvent& rMEvt );
	virtual void    MouseButtonUp( const MouseEvent& rMEvt );
	virtual void    Paint( const Rectangle& rRect );
	virtual void    Resize();
	virtual void    RequestHelp( const HelpEvent& rHEvt );
	virtual void    StateChanged( StateChangedType nStateChange );
	virtual void    DataChanged( const DataChangedEvent& rDCEvt );

	virtual void    Select();
	virtual void    DoubleClick();
	virtual void    Split();
	virtual void    ActivatePage();
	virtual long    DeactivatePage();
	virtual long    StartRenaming();
	virtual long    AllowRenaming();
	virtual void    EndRenaming();
	virtual void    Mirror();

    void            InsertPage( sal_uInt16 nPageId, const XubString& rText,
                                TabBarPageBits nBits = 0,
                                sal_uInt16 nPos = TabBar::APPEND );
    void            RemovePage( sal_uInt16 nPageId );
    void            MovePage( sal_uInt16 nPageId, sal_uInt16 nNewPos );

	Color           GetTabBgColor( sal_uInt16 nPageId ) const;
	void            SetTabBgColor( sal_uInt16 nPageId, const Color& aTabBgColor );
	sal_Bool        IsDefaultTabBgColor( sal_uInt16 nPageId );

	void            Clear();

	void            EnablePage( sal_uInt16 nPageId, sal_Bool bEnable = sal_True );
	sal_Bool            IsPageEnabled( sal_uInt16 nPageId ) const;

	void            SetPageBits( sal_uInt16 nPageId, TabBarPageBits nBits = 0 );
	TabBarPageBits  GetPageBits( sal_uInt16 nPageId ) const;

	sal_uInt16          GetPageCount() const;
	sal_uInt16          GetPageId( sal_uInt16 nPos ) const;
	sal_uInt16          GetPagePos( sal_uInt16 nPageId ) const;
	sal_uInt16          GetPageId( const Point& rPos ) const;
	Rectangle       GetPageRect( sal_uInt16 nPageId ) const;
	// returns the rectangle in which page tabs are drawn
	Rectangle		GetPageArea() const;

	void            SetCurPageId( sal_uInt16 nPageId );
	sal_uInt16          GetCurPageId() const { return mnCurPageId; }

	void            SetFirstPageId( sal_uInt16 nPageId );
	sal_uInt16          GetFirstPageId() const { return GetPageId( mnFirstPos ); }
	void            MakeVisible( sal_uInt16 nPageId );

    void            SelectPage( sal_uInt16 nPageId, sal_Bool bSelect = sal_True );
    void            SelectPageRange( sal_Bool bSelect = sal_False,
                                     sal_uInt16 nStartPos = 0,
                                     sal_uInt16 nEndPos = TabBar::APPEND );
    sal_uInt16          GetSelectPage( sal_uInt16 nSelIndex = 0 ) const;
    sal_uInt16          GetSelectPageCount() const;
    sal_Bool            IsPageSelected( sal_uInt16 nPageId ) const;

	void            EnableAutoMaxPageWidth( sal_Bool bEnable = sal_True ) { mbAutoMaxWidth = bEnable; }
	sal_Bool            IsAutoMaxPageWidthEnabled() const { return mbAutoMaxWidth; }
	void            SetMaxPageWidth( long nMaxWidth );
	long            GetMaxPageWidth() const { return mnMaxPageWidth; }
	void            ResetMaxPageWidth() { SetMaxPageWidth( 0 ); }
	sal_Bool            IsMaxPageWidth() const { return mnMaxPageWidth != 0; }

	void            EnableEditMode( sal_Bool bEnable = sal_True ) { mbAutoEditMode = bEnable; }
	sal_Bool            IsEditModeEnabled() const { return mbAutoEditMode; }
	sal_Bool            StartEditMode( sal_uInt16 nPageId );
	void            EndEditMode( sal_Bool bCancel = sal_False );
	void            SetEditText( const XubString& rText ) { maEditText = rText; }
	const XubString& GetEditText() const { return maEditText; }
	sal_Bool            IsInEditMode() const { return (mpEdit != NULL); }
	sal_Bool            IsEditModeCanceled() const { return mbEditCanceled; }
	sal_uInt16          GetEditPageId() const { return mnEditId; }

    /** Mirrors the entire control including position of buttons and splitter.
        Mirroring is done relative to the current direction of the GUI.
        @param bMirrored  sal_True = the control will draw itself RTL in LTR GUI,
            and vice versa; sal_False = the control behaves according to the
            current direction of the GUI. */
    void            SetMirrored( sal_Bool bMirrored = sal_True );
    /** Returns sal_True, if the control is set to mirrored mode (see SetMirrored()). */
    sal_Bool            IsMirrored() const { return mbMirrored; }

    /** Sets the control to LTR or RTL mode regardless of the GUI direction.
        @param bRTL  sal_False = the control will draw from left to right;
            sal_True = the control will draw from right to left. */
    void            SetEffectiveRTL( sal_Bool bRTL );
    /** Returns sal_True, if the control draws from right to left (see SetEffectiveRTL()). */
    sal_Bool            IsEffectiveRTL() const;

	sal_Bool            StartDrag( const CommandEvent& rCEvt, Region& rRegion );
	sal_uInt16          ShowDropPos( const Point& rPos );
	void            HideDropPos();
	sal_Bool            SwitchPage( const Point& rPos );
	void            EndSwitchPage();
	sal_Bool            IsInSwitching() { return mbInSwitching; }

	void            SetSelectColor();
	void            SetSelectColor( const Color& rColor );
	const Color&    GetSelectColor() const { return maSelColor; }
	sal_Bool            IsSelectColor() const { return mbSelColor; }
	void            SetSelectTextColor();
	void            SetSelectTextColor( const Color& rColor );
	const Color&    GetSelectTextColor() const { return maSelTextColor; }
	sal_Bool            IsSelectTextColor() const { return mbSelTextColor; }

	void            SetPageText( sal_uInt16 nPageId, const XubString& rText );
	XubString       GetPageText( sal_uInt16 nPageId ) const;
	void            SetHelpText( sal_uInt16 nPageId, const XubString& rText );
	XubString       GetHelpText( sal_uInt16 nPageId ) const;
	void            SetHelpId( sal_uInt16 nPageId, const rtl::OString& nHelpId );
	rtl::OString    GetHelpId( sal_uInt16 nPageId ) const;

	long            GetSplitSize() const { return mnSplitSize; }
	long            GetMinSize() const;

    void            SetHelpText( const XubString& rText )
                        { Window::SetHelpText( rText ); }
    XubString       GetHelpText() const
                        { return Window::GetHelpText(); };
    void            SetHelpId( const rtl::OString& rId )
                        { Window::SetHelpId( rId ); }
    const rtl::OString& GetHelpId() const
                        { return Window::GetHelpId(); }

	void            SetStyle( WinBits nStyle );
	WinBits         GetStyle() const { return mnWinStyle; }

	Size            CalcWindowSizePixel() const;

	void            SetSelectHdl( const Link& rLink ) { maSelectHdl = rLink; }
	const Link&     GetSelectHdl() const { return maSelectHdl; }
	void            SetDoubleClickHdl( const Link& rLink ) { maDoubleClickHdl = rLink; }
	const Link&     GetDoubleClickHdl() const { return maDoubleClickHdl; }
	void            SetSplitHdl( const Link& rLink ) { maSplitHdl = rLink; }
	const Link&     GetSplitHdl() const { return maSplitHdl; }
	void            SetActivatePageHdl( const Link& rLink ) { maActivatePageHdl = rLink; }
	const Link&     GetActivatePageHdl() const { return maActivatePageHdl; }
	void            SetDeactivatePageHdl( const Link& rLink ) { maDeactivatePageHdl = rLink; }
	const Link&     GetDeactivatePageHdl() const { return maDeactivatePageHdl; }
	void            SetStartRenamingHdl( const Link& rLink ) { maStartRenamingHdl = rLink; }
	const Link&     GetStartRenamingHdl() const { return maStartRenamingHdl; }
	void            SetAllowRenamingHdl( const Link& rLink ) { maAllowRenamingHdl = rLink; }
	const Link&     GetAllowRenamingHdl() const { return maAllowRenamingHdl; }
	void            SetEndRenamingHdl( const Link& rLink ) { maEndRenamingHdl = rLink; }
	const Link&     GetEndRenamingHdl() const { return maEndRenamingHdl; }

	// accessibility
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > CreateAccessible();
};

#endif  // _TABBAR_HXX

/* vim: set noet sw=4 ts=4: */
