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



#ifndef SD_SLIDESORTER_PAGE_OBJECT_LAYOUTER_HXX
#define SD_SLIDESORTER_PAGE_OBJECT_LAYOUTER_HXX

#include "SlideSorter.hxx"
#include "model/SlsSharedPageDescriptor.hxx"
#include "tools/gen.hxx"
#include <vcl/image.hxx>

namespace sd { namespace slidesorter { namespace view {


/** In contrast to the Layouter that places page objects in the view, the
    PageObjectLayouter places the parts of individual page objects like page
    number area, borders, preview.
*/
class PageObjectLayouter
{
public:
    /** Create a new PageObjectLayouter object.
        @param rPageObjectSize
            In general either the width or the height will be 0 in order to
            signal that this size component has to be calculated from the other.
            This calculation will make the preview as large as possible.
        @param nPageCount
            The page count is used to determine how wide the page number
            area has to be, how many digits to except for the largest page number.
    */
    PageObjectLayouter(
        const ::boost::shared_ptr<Theme>& rpTheme,
        const Size& rPageObjectWindowSize,
        const Size& rPreviewModelSize,
        const SharedSdWindow& rpWindow,
        const sal_Int32 nPageCount);
    ~PageObjectLayouter(void);

    enum Part {
        // The focus indicator is painted outside the actual page object.
        FocusIndicator,
        // This is the outer bounding box that includes the preview, page
        // number, title.
        PageObject,
        // Bounding box of the actual preview.
        Preview,
        // Bounding box of the mouse indicator indicator frame.
        MouseOverIndicator,
        // Bounding box of the page number.
        PageNumber,
        // Bounding box of the pane name.
        Name,
        // Indicator whether or not there is a slide transition associated
        // with this slide.
        TransitionEffectIndicator
    };
    /** Two coordinate systems are supported.  They differ only in
        translation not in scale.  Both relate to pixel values in the window.
        A position in the model coordinate system does not change when the window content is
        scrolled up or down.  In the window coordinate system (relative
        to the top left point of the window) scrolling leads to different values.
    */
    enum CoordinateSystem {
        WindowCoordinateSystem,
        ModelCoordinateSystem
    };

    /** Return the bounding box of the page object or one of its graphical
        parts.
        @param rWindow
            This device is used to translate between model and window
            coordinates.
        @param rpPageDescriptor
            The page for which to calculate the bounding box.  This may be
            NULL.  When it is NULL then a generic bounding box is calculated
            for the location (0,0).
        @param ePart
            The part of the page object for which to return the bounding
            box.
        @param eCoodinateSystem
            The bounding box can be returned in model and in pixel
            (window) coordinates.
    */
    Rectangle GetBoundingBox (
        const model::SharedPageDescriptor& rpPageDescriptor,
        const Part ePart,
        const CoordinateSystem eCoordinateSystem);
    Rectangle GetBoundingBox (
        const Point& rPageObjectLocation,
        const Part ePart,
        const CoordinateSystem eCoordinateSystem);
    Size GetSize (
        const Part ePart,
        const CoordinateSystem eCoordinateSystem);

    Image GetTransitionEffectIcon (void) const;

private:
    SharedSdWindow mpWindow;
    Size maPageObjectSize;
    double mnModelToWindowScale;
    Rectangle maFocusIndicatorBoundingBox;
    Rectangle maPageObjectBoundingBox;
    Rectangle maPageNumberAreaBoundingBox;
    Rectangle maPreviewBoundingBox;
    Rectangle maTransitionEffectBoundingBox;
    const Image maTransitionEffectIcon;
    const ::boost::shared_ptr<Font> mpPageNumberFont;

    Size GetPageNumberAreaSize (const int nPageCount);
    Rectangle CalculatePreviewBoundingBox (
        Size& rPageObjectSize,
        const Size& rPreviewModelSize,
        const sal_Int32 nPageNumberAreaWidth,
        const sal_Int32 nFocusIndicatorWidth);
};


} } } // end of namespace ::sd::slidesorter::view

#endif
