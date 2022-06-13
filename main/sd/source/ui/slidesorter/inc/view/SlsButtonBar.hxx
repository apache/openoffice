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



#ifndef SD_SLIDESORTER_VIEW_BUTTON_BAR_HXX
#define SD_SLIDESORTER_VIEW_BUTTON_BAR_HXX

#include "model/SlsSharedPageDescriptor.hxx"
#include <tools/gen.hxx>
#include <rtl/ustring.hxx>
#include <vcl/bitmapex.hxx>
#include <vcl/bmpacc.hxx>
#include <boost/scoped_ptr.hpp>

namespace sd { namespace slidesorter {
class SlideSorter;
} }


namespace sd { namespace slidesorter { namespace view {

class Theme;

class Button;
typedef ::boost::shared_ptr<Button> SharedButton;

/** This is a container of buttons and a coordinating controller.
    The last means that it receives mouse events and forwards them to
    the right button.
*/
class ButtonBar
{
public:
    ButtonBar (SlideSorter& rSlideSorter);
    ~ButtonBar (void);

    void ProcessButtonDownEvent (
        const model::SharedPageDescriptor& rpDescriptor,
        const Point aMouseModelLocation);
    void ProcessButtonUpEvent (
        const model::SharedPageDescriptor& rpDescriptor,
        const Point aMouseModelLocation);
    void ProcessMouseMotionEvent (
        const model::SharedPageDescriptor& rpDescriptor,
        const Point aMouseModelLocation,
        const bool bIsMouseButtonDown);

    void ResetPage (void);

    /** Return the number of buttons that are to be displayed in page
        objects which the mouse hovers over.
        @param bIsExcluded
            When this flag is <TRUE/> then return the number of
            buttons that is to be displayed for pages that are
            excluded from the slide show.
    */
    sal_Int32 GetButtonCount (const bool bIsExcluded) const;

    /** Return the specified button.
        @param nIndex
            Valid values lie in the range [0,GetButtonCount()).
        @param bIsExcluded
            When this flag is <TRUE/> then return a button that is to
            be displayed for pages that are excluded from the slide
            show.
        @return
            Returns an empty pointer when the given index is not valid.
    */
    ::boost::shared_ptr<Button> GetButton (
        const bool bIsExcluded,
        const sal_Int32 nIndex) const;

    bool IsMouseOverBar (void) const;

    /** Paint the specified page object.  When this is not the same as the
        one under the mouse (mpDescriptor) then the buttons are all
        painted in their normal state.
    */
    void Paint (
        OutputDevice& rDevice,
        const model::SharedPageDescriptor& rpPageDescriptor);

    bool IsMouseOverButton (void) const;

    void RequestLayout (void);

    /** Return the help text for the button under the mouse.
        @return
            When the mouse is not over a button then an empty string
            is returned.
    */
    ::rtl::OUString GetButtonHelpText (void) const;

    /** Request the button bar to be shown.
        @param bAnimate
            This flag controls whether to just show the button bar (<FALSE/>)
            or to fade it in smoothly (<TRUE/>.)
    */
    void RequestFadeIn (
        const model::SharedPageDescriptor& rpDescriptor,
        const bool bAnimate);

    /** Request the button bar to be hidden.
        @param bAnimate
            This flag controls whether to just hide the button bar (<FALSE/>)
            or to fade it out smoothly (<TRUE/>.)
    */
    void RequestFadeOut (
        const model::SharedPageDescriptor& rpDescriptor,
        const bool bAnimate);

    /** Return whether the button bar is visible for the given descriptor (or
        being faded in).
    */
    bool IsVisible (const model::SharedPageDescriptor& rpDescriptor);

    void HandleDataChangeEvent (void);

    class BackgroundTheme;

    /** While at least one Lock object exists the button bar will not be
        displayed.  Used, e.g. during a mouse multiselection to avoid
        confusing and unhelpful visual signals.
    */
    class Lock
    {
    public:
        Lock (SlideSorter& rSlideSorter);
        ~Lock (void);
    private:
        ButtonBar& mrButtonBar;
    };

private:
    SlideSorter& mrSlideSorter;
    Size maPageObjectSize;
    Rectangle maButtonBoundingBox;
    Point maBackgroundLocation;
    model::SharedPageDescriptor mpDescriptor;
    bool mbIsExcluded;
    boost::shared_ptr<Button> mpButtonUnderMouse;
    // The button on which the mouse button was pressed.
    boost::shared_ptr<Button> mpDownButton;
    ::std::vector<SharedButton> maRegularButtons;
    ::std::vector<SharedButton> maExcludedButtons;
    BitmapEx maNormalBackground;
    BitmapEx maButtonDownBackground;
    bool mbIsMouseOverBar;
    ::boost::scoped_ptr<BackgroundTheme> mpBackgroundTheme;
    int mnLockCount;

    /** Remember the specified page.  If it differs from mpDescriptor then
        the buttons are placed anew.
        @return
            The returned flag indicates whether the mpDescriptor member
            is set to a new value.
    */
    bool SetPage (const model::SharedPageDescriptor& rpDescriptor);
    SharedButton GetButtonAt (const Point aModelLocation);
    bool SetButtonUnderMouse (const SharedButton& rButton = SharedButton());
    void PaintButtonBackground (
        OutputDevice& rDevice,
        const model::SharedPageDescriptor& rpPageDescriptor,
        const Point aOffset);
    void LayoutButtons (const Size aPageModelSize);
    bool LayoutButtons (void);
    BitmapEx CreateBackground (
        const OutputDevice& rTemplateDevice,
        const bool bIsButtonDown) const;
    bool IsMouseOverBar (const Point aModelLocation) const;
    void StartFadeAnimation (
        const model::SharedPageDescriptor& rpDescriptor,
        const double nTargetAlpha,
        const bool bFadeIn);

    void AcquireLock (void);
    void ReleaseLock (void);
};




class Button
{
public:
    Button (
        SlideSorter& rSlideSorter,
        const ::rtl::OUString& rsHelpText);
    virtual ~Button (void);

    enum State { State_Normal, State_Hover, State_Down };
    enum IconSize { IconSize_Large, IconSize_Medium, IconSize_Small };

    /** Set a new state.
        @return
            When the new state is different from the old state
            then <TRUE/> is returned.
    */
    bool SetState (const State eState);
    State GetState (void) const;

    virtual void Place (const Rectangle aButtonBarBox) = 0;
    virtual void Paint (
        OutputDevice& rDevice,
        const Point aOffset,
        const double nAlpha,
        const ::boost::shared_ptr<Theme>& rpTheme) const = 0;
    virtual void ProcessClick (const model::SharedPageDescriptor& rpDescriptor) = 0;

    /** Return the bounding box of the layouted button.
    */
    Rectangle GetBoundingBox (void) const;
    /** Return the minimum size required to completely paint the
        button.
    */
    virtual Size GetSize (void) const = 0;
    virtual Size GetSize (const IconSize eIconSize) const = 0;
    ::rtl::OUString GetHelpText (void) const;
    bool IsDown (void) const;
    void SetActiveState (const bool bIsActive);
    bool IsActive (void) const;
    void SetIconSize (const IconSize eIconSize);
    IconSize GetIconSize (void) const;
    /** By default a button is always enabled.  Override to change this.
    */
    virtual bool IsEnabled (void) const;

protected:
    SlideSorter& mrSlideSorter;
    State meState;
    Rectangle maBoundingBox;
    const ::rtl::OUString msHelpText;
    // Buttons that lie (partly) outside the button bar are deactivated.
    bool mbIsActive;
    IconSize meIconSize;
};



class TextButton : public Button
{
public:
    TextButton (
        SlideSorter& rSlideSorter,
        const ::rtl::OUString& rsText,
        const ::rtl::OUString& rsHelpText);

    virtual void Place (const Rectangle aButtonBarBox);
    virtual void Paint (
        OutputDevice& rDevice,
        const Point aOffset,
        const double nAlpha,
        const ::boost::shared_ptr<Theme>& rpTheme) const;
    virtual Size GetSize (void) const;
    virtual Size GetSize (const IconSize eIconSize) const;

private:
    const ::rtl::OUString msText;
};



class ImageButton : public Button
{
public:
    ImageButton (
        SlideSorter& rSlideSorter,
        const BitmapEx& rLargeIcon,
        const BitmapEx& rLargeHoverIcon,
        const BitmapEx& rMediumIcon,
        const BitmapEx& rMediumHoverIcon,
        const BitmapEx& rSmallIcon,
        const BitmapEx& rSmallHoverIcon,
        const ::rtl::OUString& rsHelpText);

    virtual void Place (const Rectangle aButtonBarBox);
    virtual void Paint (
        OutputDevice& rDevice,
        const Point aOffset,
        const double nAlpha,
        const ::boost::shared_ptr<Theme>& rpTheme) const;
    virtual Size GetSize (void) const;
    virtual Size GetSize (const IconSize eIconSize) const;

private:
    const BitmapEx maLargeIcon;
    const BitmapEx maLargeHoverIcon;
    const BitmapEx maMediumIcon;
    const BitmapEx maMediumHoverIcon;
    const BitmapEx maSmallIcon;
    const BitmapEx maSmallHoverIcon;
};


class UnhideButton : public ImageButton
{
public:
    UnhideButton (SlideSorter& rSlideSorter);

protected:
    virtual void ProcessClick (const model::SharedPageDescriptor& rpDescriptor);
};


class StartShowButton : public ImageButton
{
public:
    StartShowButton (SlideSorter& rSlideSorter);
    virtual bool IsEnabled (void) const;

protected:
    virtual void ProcessClick (const model::SharedPageDescriptor& rpDescriptor);
};


class HideButton : public ImageButton
{
public:
    HideButton (SlideSorter& rSlideSorter);

protected:
    virtual void ProcessClick (const model::SharedPageDescriptor& rpDescriptor);
};


class DuplicateButton : public ImageButton
{
public:
    DuplicateButton (SlideSorter& rSlideSorter);
    virtual bool IsEnabled (void) const;

protected:
    virtual void ProcessClick (const model::SharedPageDescriptor& rpDescriptor);
};


} } } // end of namespace ::sd::slidesorter::view

#endif
