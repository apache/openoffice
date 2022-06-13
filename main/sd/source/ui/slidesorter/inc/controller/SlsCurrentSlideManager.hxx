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



#ifndef SD_SLIDESORTER_CURRENT_SLIDE_MANAGER_HXX
#define SD_SLIDESORTER_CURRENT_SLIDE_MANAGER_HXX

#include "model/SlsSharedPageDescriptor.hxx"
#include <vcl/timer.hxx>
#include <tools/link.hxx>
#include <com/sun/star/drawing/XDrawPage.hpp>

class SdPage;

namespace sd { namespace slidesorter {
class SlideSorter;
} }


namespace sd { namespace slidesorter { namespace controller {

/** Manage the current slide.  This includes setting the according flags at
    the PageDescriptor objects and setting the current slide at the main
    view shell.

    Switching pages is triggered only after a little delay.  This allows
    fast traveling through a larger set of slides without having to wait
    for the edit view to update its content after every slide change.
*/
class CurrentSlideManager
{
public:
    /** Create a new CurrentSlideManager object that manages the current
        slide for the given SlideSorter.
    */
    CurrentSlideManager (SlideSorter& rSlideSorter);

    ~CurrentSlideManager (void);

    /** Call this when the current page of the main view shell has been
        switched.  Use SwitchCurrentSlide() to initiate such a switch.
    */
    void NotifyCurrentSlideChange (const sal_Int32 nSlideIndex);
    void NotifyCurrentSlideChange (const SdPage* pPage);

    /** Call this method to switch the current page of the main view shell
        to the given slide.  Use CurrentSlideHasChanged() when the current
        slide change has been initiated by someone else.
        @param nSlideIndex
            Zero based index in the range [0,number-of-slides).
        @param bUpdateSelection
            When <TRUE/> then the page selection is cleared and only the new
            current slide is selected.
    */
    void SwitchCurrentSlide (
        const sal_Int32 nSlideIndex,
        const bool bUpdateSelection = false);
    void SwitchCurrentSlide (
        const model::SharedPageDescriptor& rpSlide,
        const bool bUpdateSelection = false);

    /** Return the page descriptor for the current slide.  Note, that when
        there is no current slide then the returned pointer is empty.
    */
    model::SharedPageDescriptor GetCurrentSlide (void);

    /** Release all references to model data.
    */
    void PrepareModelChange (void);

    /** Modify inner state in reaction to a change of the SlideSorterModel.
    */
    void HandleModelChange (void);

private:
    SlideSorter& mrSlideSorter;
    sal_Int32 mnCurrentSlideIndex;
    model::SharedPageDescriptor mpCurrentSlide;
    /** Timer to control the delay after which to ask
        XController/ViewShellBase to switch to another slide.
    */
    Timer maSwitchPageDelayTimer;

    bool IsCurrentSlideIsValid (void);
    void SetCurrentSlideAtViewShellBase (const model::SharedPageDescriptor& rpSlide);
    void SetCurrentSlideAtTabControl (const model::SharedPageDescriptor& rpSlide);
    void SetCurrentSlideAtXController (const model::SharedPageDescriptor& rpSlide);

    /** When switching from one slide to a new current slide then this
        method releases all ties to the old slide.
    */
    void ReleaseCurrentSlide (void);

    /** When switching from one slide to a new current slide then this
        method connects to the new current slide.
    */
    void AcquireCurrentSlide (const sal_Int32 nSlideIndex);

    DECL_LINK(SwitchPageCallback,void*);
};


} } } // end of namespace ::sd::slidesorter::controller

#endif
