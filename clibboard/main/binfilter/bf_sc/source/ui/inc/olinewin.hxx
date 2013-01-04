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



#ifndef SC_OLINEWIN_HXX
#define SC_OLINEWIN_HXX

#ifndef SC_VIEWDATA_HXX
#include "viewdata.hxx"
#endif
namespace binfilter {

class ScOutlineArray;


// ============================================================================

enum ScOutlineMode { SC_OUTLINE_HOR, SC_OUTLINE_VER };


// ----------------------------------------------------------------------------

/** The window left of or above the spreadsheet containing the outline groups
    and controls to expand/collapse them. */
class ScOutlineWindow : public Window
{
private:
    ScViewData&                 mrViewData;         /// View data containing the document.
    ScSplitPos                  meWhich;            /// Which area in split window.
    bool                        mbHoriz;            /// true = Horizontal orientation.
    bool                        mbMirrorHdr;        /// true = Header at end of the window (bottom or right).

    ImageList*                  mpSymbols;          /// Symbols for buttons.
    Color                       maLineColor;        /// Line color for expanded groups.
    sal_Int32                   mnHeaderSize;       /// Size of the header area in entry direction.
    sal_Int32                   mnHeaderPos;        /// Position of the header area in entry direction.
    sal_Int32                   mnMainFirstPos;     /// First position of main area in entry direction.
    sal_Int32                   mnMainLastPos;      /// Last position of main area in entry direction.

    sal_uInt16                  mnMTLevel;          /// Mouse tracking: Level of active button.
    sal_uInt16                  mnMTEntry;          /// Mouse tracking: Entry index of active button.
    bool                        mbMTActive;         /// Mouse tracking active?
    bool                        mbMTPressed;        /// Mouse tracking: Button currently drawed pressed?

    Rectangle                   maFocusRect;        /// Focus rectangle on screen.
    sal_uInt16                  mnFocusLevel;       /// Level of focused button.
    sal_uInt16                  mnFocusEntry;       /// Entry index of focused button.
    bool                        mbDontDrawFocus;    /// Do not redraw focus in next Paint().

public:
                                ScOutlineWindow(
                                    Window* pParent,
                                    ScOutlineMode eMode,
                                    ScViewData* pViewData,
                                    ScSplitPos eWhich );
    virtual                     ~ScOutlineWindow();

    /** Sets the size of the header area (width/height dep. on window type). */
    void                        SetHeaderSize( sal_Int32 nNewSize );
    /** Returns the width/height the window needs to show all levels. */
    sal_Int32                   GetDepthSize() const;


private:
    /** Initializes color and image settings. */
    void                        InitSettings();

};


// ============================================================================

} //namespace binfilter
#endif

