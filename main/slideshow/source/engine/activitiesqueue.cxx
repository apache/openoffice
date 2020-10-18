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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_slideshow.hxx"

// must be first
#include <canvas/debug.hxx>
#include <canvas/verbosetrace.hxx>

#include <comphelper/anytostring.hxx>
#include <cppuhelper/exc_hlp.hxx>

#include "slideshowexceptions.hxx"
#include "activity.hxx"
#include "activitiesqueue.hxx"

#include <boost/bind.hpp>
#include <algorithm>


using namespace ::com::sun::star;

namespace slideshow
{
    namespace internal
    {
        ActivitiesQueue::ActivitiesQueue( 
          const ::boost::shared_ptr< ::canvas::tools::ElapsedTime >& pPresTimer ) :
            mpTimer( pPresTimer ),
            maCurrentActivitiesWaiting(),
            maCurrentActivitiesReinsert(),
            maDequeuedActivities()
        {
        }

        ActivitiesQueue::~ActivitiesQueue()
        {
            // dispose all queue entries
            try 
            {
                std::for_each( maCurrentActivitiesWaiting.begin(),
                               maCurrentActivitiesWaiting.end(),
                               boost::mem_fn( &Disposable::dispose ) );
                std::for_each( maCurrentActivitiesReinsert.begin(),
                               maCurrentActivitiesReinsert.end(),
                               boost::mem_fn( &Disposable::dispose ) );
            }
            catch (uno::Exception &) 
            {
                OSL_ENSURE( false, rtl::OUStringToOString(
                                comphelper::anyToString(
                                    cppu::getCaughtException() ),
                                RTL_TEXTENCODING_UTF8 ).getStr() );
            }
        }

        bool ActivitiesQueue::addActivity( const ActivitySharedPtr& pActivity )
        {
            OSL_ENSURE( pActivity, "ActivitiesQueue::addActivity: activity ptr NULL" );

            if( !pActivity )
                return false;

            // add entry to waiting list
            maCurrentActivitiesWaiting.push_back( pActivity );

            return true;
        }

        void ActivitiesQueue::process()
        {
            VERBOSE_TRACE( "ActivitiesQueue: outer loop heartbeat" );

            // accumulate time lag for all activities, and lag time
            // base if necessary:
            ActivityQueue::const_iterator iPos(
                maCurrentActivitiesWaiting.begin() );
            const ActivityQueue::const_iterator iEnd(
                maCurrentActivitiesWaiting.end() );
            double fLag = 0.0;
            for ( ; iPos != iEnd; ++iPos )
                fLag = std::max<double>( fLag, (*iPos)->calcTimeLag() );
            if (fLag > 0.0) 
            {
                mpTimer->adjustTimer( -fLag );
            }
            
            // process list of activities
            while( !maCurrentActivitiesWaiting.empty() )
            {
                // process topmost activity
                ActivitySharedPtr pActivity( maCurrentActivitiesWaiting.front() );
                maCurrentActivitiesWaiting.pop_front();
                
                bool bReinsert( false );

                try
                {
                    // fire up activity
                    bReinsert = pActivity->perform();
                }
                catch( uno::RuntimeException& )
                {
                    throw;
                }
                catch( uno::Exception& )
                {
                    // catch anything here, we don't want
                    // to leave this scope under _any_
                    // circumstance. Although, do _not_
                    // reinsert an activity that threw
                    // once.
                    
                    // NOTE: we explicitly don't catch(...) here,
                    // since this will also capture segmentation
                    // violations and the like. In such a case, we
                    // still better let our clients now...
                    OSL_ENSURE( false,
                                rtl::OUStringToOString(
                                    comphelper::anyToString( cppu::getCaughtException() ),
                                    RTL_TEXTENCODING_UTF8 ).getStr() );
                }
                catch( SlideShowException& )
                {
                    // catch anything here, we don't want
                    // to leave this scope under _any_
                    // circumstance. Although, do _not_
                    // reinsert an activity that threw
                    // once.
                    
                    // NOTE: we explicitly don't catch(...) here,
                    // since this will also capture segmentation
                    // violations and the like. In such a case, we
                    // still better let our clients now...
                    OSL_TRACE( "::presentation::internal::ActivitiesQueue: Activity threw a SlideShowException, removing from ring" );
                }

                if( bReinsert )
                    maCurrentActivitiesReinsert.push_back( pActivity );
                else
                    maDequeuedActivities.push_back( pActivity );

                VERBOSE_TRACE( "ActivitiesQueue: inner loop heartbeat" );
            }

            if( !maCurrentActivitiesReinsert.empty() )
            {
                // reinsert all processed, but not finished
                // activities back to waiting queue. With swap(),
                // we kill two birds with one stone: we reuse the
                // list nodes, and we clear the
                // maCurrentActivitiesReinsert list
                maCurrentActivitiesWaiting.swap( maCurrentActivitiesReinsert );
            }
        }

        void ActivitiesQueue::processDequeued()
        {
            // notify all dequeued activities from last round
            ::std::for_each( maDequeuedActivities.begin(),
                             maDequeuedActivities.end(),
                             ::boost::mem_fn( &Activity::dequeued ) );
            maDequeuedActivities.clear();
        }

        bool ActivitiesQueue::isEmpty() const
        {
            return maCurrentActivitiesWaiting.empty() && maCurrentActivitiesReinsert.empty();
        }

        void ActivitiesQueue::clear()
        {
            // dequeue all entries:
            std::for_each( maCurrentActivitiesWaiting.begin(),
                           maCurrentActivitiesWaiting.end(),
                           boost::mem_fn( &Activity::dequeued ) );
            ActivityQueue().swap( maCurrentActivitiesWaiting );
            
            std::for_each( maCurrentActivitiesReinsert.begin(),
                           maCurrentActivitiesReinsert.end(),
                           boost::mem_fn( &Activity::dequeued ) );
            ActivityQueue().swap( maCurrentActivitiesReinsert );
        }
    }
}
