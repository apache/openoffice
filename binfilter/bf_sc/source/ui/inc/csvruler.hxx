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



// ============================================================================

#ifndef _SC_CSVRULER_HXX
#define _SC_CSVRULER_HXX

#ifndef _SV_VIRDEV_HXX
#include <vcl/virdev.hxx>
#endif

#ifndef _SC_CSVCONTROL_HXX
#include "csvcontrol.hxx"
#endif
#ifndef _SC_CSVSPLITS_HXX
#include "csvsplits.hxx"
#endif
namespace binfilter {


class ScAccessibleCsvControl;


// ============================================================================

/** A ruler control for the CSV import dialog. Supports setting and moving
    splits (which divide lines of data into several columns). */
class ScCsvRuler : public ScCsvControl
{
private:
    VirtualDevice               maBackgrDev;        /// Ruler background, scaling.
    VirtualDevice               maRulerDev;         /// Ruler with splits and cursor.

    Color                       maBackColor;        /// Background color.
    Color                       maActiveColor;      /// Color for active part of ruler.
    Color                       maTextColor;        /// Text and scale color.
    Color                       maSplitColor;       /// Split area color.

    ScCsvSplits                 maSplits;           /// Vector with split positions.
    ScCsvSplits                 maOldSplits;        /// Old state for cancellation.

    sal_Int32                   mnPosCursorLast;    /// Last valid position of cursor.
    sal_Int32                   mnPosMTStart;       /// Start position of mouse tracking.
    sal_Int32                   mnPosMTCurr;        /// Current position of mouse tracking.
    bool                        mbPosMTMoved;       /// Tracking: Anytime moved to another position?

    Size                        maWinSize;          /// Size of the control.
    Rectangle                   maActiveRect;       /// The active area of the ruler.
    sal_Int32                   mnSplitSize;        /// Size of a split circle.

    // ------------------------------------------------------------------------
public:
    explicit                    ScCsvRuler( ScCsvControl& rParent );

    // common ruler handling --------------------------------------------------

    // split handling ---------------------------------------------------------

    // event handling ---------------------------------------------------------

    // painting ---------------------------------------------------------------



};


// ============================================================================

} //namespace binfilter
#endif

