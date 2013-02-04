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


#ifndef _SWPVIEW_HXX
#define _SWPVIEW_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXVIEWFRM_HXX //autogen
#include <bf_sfx2/viewfrm.hxx>
#endif
#ifndef _VIEWFAC_HXX //autogen
#include <bf_sfx2/viewfac.hxx>
#endif
#ifndef _LINK_HXX //autogen
#include <tools/link.hxx>
#endif
#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif
#ifndef _SFXVIEWSH_HXX //autogen
#include <bf_sfx2/viewsh.hxx>
#endif
#include "shellid.hxx"
#ifndef _SVX_ZOOMITEM_HXX
#include <bf_svx/zoomitem.hxx>
#endif
class ImageButton; 
class Button; 
class SvtAccessibilityOptions; 
namespace binfilter {

class SwViewOption;
class SwDocShell;
class SwScrollbar;
class ViewShell;
class SwPagePreView;
class SwRect;
class DataChangedEvent;
class CommandEvent;

// OD 12.12.2002 #103492#
class SwPagePreviewLayout;

// OD 24.03.2003 #108282# - delete member <mnVirtPage> and its accessor.
class SwPagePreViewWin : public Window
{
    ViewShell*          mpViewShell;
    USHORT              mnSttPage;
    BYTE                mnRow, mnCol;
    Size                maPxWinSize;
    Fraction            maScale;
    SwPagePreView&      mrView;
    // OD 02.12.2002 #103492#
    bool                mbCalcScaleForPreviewLayout;
    Rectangle           maPaintedPreviewDocRect;
    // OD 12.12.2002 #103492#
    SwPagePreviewLayout* mpPgPrevwLayout;


public:
	SwPagePreViewWin( Window* pParent, SwPagePreView& rView );
	~SwPagePreViewWin();

	//Ruft ViewShell::Paint


    ViewShell* GetViewShell() const { return mpViewShell; }


    /** get selected page number of document preview

        OD 13.12.2002 #103492#

        @author OD

        @return selected page number
    */

    /** set selected page number in document preview

        OD 13.12.2002 #103492#

        @author OD

        @param _nSelectedPageNum
        input parameter - physical page number of page that will be the selected one.
    */

    //JP 19.08.98: bei Einspaltigkeit gibt es keine 0. Seite!



    // OD 18.12.2002 #103492# - add <MV_SELPAGE>, <MV_SCROLL>

	// erzeuge den String fuer die StatusLeiste


    /** method to adjust preview to a new zoom factor

        OD 02.12.2002 #103492#
        paint of preview is prepared for a new zoom factor

        @author OD
    */
#ifdef ACCESSIBLE_LAYOUT
#endif
};


/*--------------------------------------------------------------------
	Beschreibung:   Sicht auf ein Dokument
 --------------------------------------------------------------------*/

class SwPagePreView: public SfxViewShell
{
	// ViewWindow und Henkel zur Core
	// aktuelle Dispatcher-Shell
 	SwPagePreViewWin        aViewWin;
	//viewdata of the previous SwView and the new crsrposition
	String 					sSwViewData,
	//and the new cursor position if the user double click in the PagePreView
							sNewCrsrPos;
    // to support keyboard the number of the page to go to can be set too
    USHORT                  nNewPage;
   // Sichtbarer Bereich
	String                  sPageStr;
	Size                    aDocSz;
 	Rectangle               aVisArea;
 
	// MDI Bedienelemente
	SwScrollbar             *pHScrollbar;
	SwScrollbar             *pVScrollbar;
 	ImageButton             *pPageUpBtn,
							*pPageDownBtn;
 	// Dummy-Window zum FÅllen der rechten unteren Ecke, wenn beide Scrollbars
	// aktiv sind
 	Window					*pScrollFill;

    USHORT                  mnPageCount;
	BOOL                    bNormalPrint;

    // OD 09.01.2003 #106334#
    // new members to reset design mode at draw view for form shell on switching
    // back from writer page preview to normal view.
     sal_Bool                mbResetFormDesignMode:1;
    sal_Bool                mbFormDesignModeToReset:1;






    /** help method to execute SfxRequest FN_PAGE_UP and FN_PAGE_DOWN

        OD 04.03.2003 #107369#

        @param _bPgUp
        input parameter - boolean that indicates, if FN_PAGE_UP or FN_PAGE_DOWN
        has to be executed.

        @param _pReq
        optional input parameter - pointer to the <SfxRequest> instance, if existing.

        @author OD
    */

protected:

    void         SetZoom(SvxZoomType eSet, USHORT nFactor);

public:
	SFX_DECL_VIEWFACTORY(SwPagePreView);
	SFX_DECL_INTERFACE(SW_PAGEPREVIEW);
	TYPEINFO();

     inline ViewShell*       GetViewShell() const { return aViewWin.GetViewShell(); }








        // Handler
	void            Execute(SfxRequest&){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void            Execute(SfxRequest&);
	void            GetState(SfxItemSet&){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void            GetState(SfxItemSet&);
	void            StateUndo(SfxItemSet&){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void            StateUndo(SfxItemSet&);

	SwDocShell*     GetDocShell();

    //apply Accessiblity options

    // OD 09.01.2003 #106334# - inline method to request values of new members
    // <mbResetFormDesignMode> and <mbFormDesignModeToReset>


    /** adjust position of vertical scrollbar

        OD 19.02.2003 #107369
        Currently used, if the complete preview layout rows fit into to the given
        window, if a new page is selected and this page is visible.

        @author OD

        @param _nNewThumbPos
        input parameter - new position, which will be assigned to the vertical
        scrollbar.
    */
    void SetVScrollbarThumbPos( const sal_uInt16 _nNewThumbPos );

    SwPagePreView( SfxViewFrame* pFrame, SfxViewShell* );
 	~SwPagePreView();
};

// ----------------- inline Methoden ----------------------



} //namespace binfilter
#endif
