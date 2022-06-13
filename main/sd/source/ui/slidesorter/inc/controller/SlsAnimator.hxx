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



#ifndef SD_SLIDESORTER_CONTROLLER_ANIMATOR_HXX
#define SD_SLIDESORTER_CONTROLLER_ANIMATOR_HXX

#include "SlideSorter.hxx"
#include "view/SlideSorterView.hxx"
#include <canvas/elapsedtime.hxx>
#include <vcl/timer.hxx>
#include <sal/types.h>
#include <vector>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>


namespace sd { namespace slidesorter { namespace controller {

/** Experimental class for simple eye candy animations.
*/
class Animator
    : private ::boost::noncopyable
{
public:
    /** In some circumstances we have to avoid animation and jump to the
        final animation state immediately.  Use this enum instead of a bool
        to be more expressive.
    */
    enum AnimationMode { AM_Animated, AM_Immediate };

    Animator (SlideSorter& rSlideSorter);
    ~Animator (void);

    /** When disposed the animator will stop its work immediately and not
        process any timer events anymore.
    */
    void Dispose (void);

    /** An animation object is called with values between 0 and 1 as single
        argument to its operator() method.
    */
    typedef ::boost::function1<void, double> AnimationFunctor;
    typedef ::boost::function0<void> FinishFunctor;

    typedef sal_Int32 AnimationId;
    static const AnimationId NotAnAnimationId = -1;

    /** Schedule a new animation for execution.  The () operator of that
        animation will be called with increasing values between 0 and 1 for
        the specified duration.
        @param rAnimation
            The animation operation.
        @param nStartOffset
            Time in milliseconds before the animation is started.
        @param nDuration
            The duration in milliseconds.
    */
    AnimationId AddAnimation (
        const AnimationFunctor& rAnimation,
        const sal_Int32 nStartOffset,
        const sal_Int32 nDuration,
        const FinishFunctor& rFinishFunctor = FinishFunctor());

    AnimationId AddInfiniteAnimation (
        const AnimationFunctor& rAnimation,
        const double nDelta);

    /** Abort and remove an animation.  In order to reduce the bookkeeping
        on the caller side, it is OK to call this method with an animation
        function that is not currently being animated.  Such a call is
        silently ignored.
    */
    void RemoveAnimation (const AnimationId nAnimationId);

    /** A typical use case for this method is the temporary shutdown of the
        slidesorter when the slide sorter bar is put into a cache due to a
        change of the edit mode.
    */
    void RemoveAllAnimations (void);

private:
    SlideSorter& mrSlideSorter;
    Timer maTimer;
    bool mbIsDisposed;
    class Animation;
    typedef ::std::vector<boost::shared_ptr<Animation> > AnimationList;
    AnimationList maAnimations;
    ::canvas::tools::ElapsedTime maElapsedTime;

    ::boost::scoped_ptr<view::SlideSorterView::DrawLock> mpDrawLock;

    AnimationId mnNextAnimationId;

    DECL_LINK(TimeoutHandler, Timer*);

    /** Execute one step of every active animation.
        @param nTime
            Time measured in milliseconds with some arbitrary reference point.
        @return
            When one or more animation has finished then <TRUE/> is
            returned.  Call CleanUpAnimationList() in this case.
    */
    bool ProcessAnimations (const double nTime);

    /** Remove animations that have expired.
    */
    void CleanUpAnimationList (void);

    void RequestNextFrame (const double nFrameStart = 0);
};


} } } // end of namespace ::sd::slidesorter::controller

#endif
