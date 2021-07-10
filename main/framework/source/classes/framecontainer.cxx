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
#include "precompiled_framework.hxx"

//_________________________________________________________________________________________________________________
// my own includes
//_________________________________________________________________________________________________________________

#ifndef __FRAMEWORK_FRAMECONTAINER_HXX_
#include <classes/framecontainer.hxx>
#endif
#include <threadhelp/writeguard.hxx>
#include <threadhelp/readguard.hxx>

#ifndef __FRAMEWORK_COMMANDS_HXX_
#include <commands.h>
#endif

//_________________________________________________________________________________________________________________
// interface includes
//_________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_FRAME_FRAMESEARCH_FLAG_HPP_
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#endif

//_________________________________________________________________________________________________________________
// includes of other projects
//_________________________________________________________________________________________________________________
#include <vcl/svapp.hxx>

//_________________________________________________________________________________________________________________
// namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________
// non exported const
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
// non exported definitions
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
// declarations
//_________________________________________________________________________________________________________________

/**-***************************************************************************************************************
    @short      initialize an empty container
    @descr      The container will be empty then - special features (e.g. the async quit mechanism) are disabled.

    @threadsafe not necessary - it's not a singleton
    @modified   01.07.2002 14:42,as96863
 *****************************************************************************************************************/
FrameContainer::FrameContainer()
        // initialize base classes first.
        // Order is necessary for right initialization of his and OUR member... m_aLock
        : ThreadHelpBase ( &Application::GetSolarMutex()                  )
/*DEPRECATEME
        , m_bAsyncQuit   ( sal_False                                      ) // default must be "disabled"!
        , m_aAsyncCall   ( LINK( this, FrameContainer, implts_asyncQuit ) )
*/
{
}

/**-***************************************************************************************************************
    @short      deinitialize may a filled container
    @descr      Special features (if the currently are running) will be disabled and we free all used other resources.

    @threadsafe not necessary - it's not a singleton
    @modified   01.07.2002 14:43,as96863
 *****************************************************************************************************************/
FrameContainer::~FrameContainer()
{
	// Don't forget to free memory!
    m_aContainer.clear();
    m_xActiveFrame.clear();
}

/**-***************************************************************************************************************
    @short      append a new frame to the container
    @descr      We accept the incoming frame only, if it's a valid reference and doesn't exist already.

    @param      xFrame
                    frame, which should be added to this container
                    Must be a valid reference.

    @threadsafe yes
    @modified   01.07.2002 14:44,as96863
 *****************************************************************************************************************/
void FrameContainer::append( const css::uno::Reference< css::frame::XFrame >& xFrame )
{
    if (xFrame.is() && ! exist(xFrame))
    {
        /* SAFE { */
        WriteGuard aWriteLock( m_aLock );
        m_aContainer.push_back( xFrame );
        aWriteLock.unlock();
        /* } SAFE */
    }
}

/**-***************************************************************************************************************
    @short      remove a frame from the container
    @descr      In case we remove the last frame and our internal special feature (the async quit mechanism)
                was enabled by the desktop instance, we start it.

    @param      xFrame
                    frame, which should be deleted from this container
                    Must be a valid reference.

    @threadsafe yes
    @modified   01.07.2002 14:52,as96863
 *****************************************************************************************************************/
void FrameContainer::remove( const css::uno::Reference< css::frame::XFrame >& xFrame )
{
    /* SAFE { */
    // write lock necessary for following erase()!
    WriteGuard aWriteLock( m_aLock );

    TFrameIterator aSearchedItem = ::std::find( m_aContainer.begin(), m_aContainer.end(), xFrame );
    if (aSearchedItem!=m_aContainer.end())
    {
        m_aContainer.erase( aSearchedItem );

        // If removed frame was the current active frame - reset state variable.
        if (m_xActiveFrame==xFrame)
            m_xActiveFrame = css::uno::Reference< css::frame::XFrame >();

        // We don't need the write lock any longer...
        // downgrade to read access.
        aWriteLock.downgrade();
/*DEPRECATEME
        // If last frame was removed and special quit mode is enabled by the desktop
        // we must terminate the application by using this asynchronous callback!
        if (m_aContainer.size()<1 && m_bAsyncQuit)
            m_aAsyncCall.Post(0);
*/
    }

    aWriteLock.unlock();
    // } SAFE
}

/**-***************************************************************************************************************
    @short      check if the given frame currently exists inside the container
    @descr      -

    @param      xFrame
                    reference to the queried frame

    @return     <TRUE/> if frame is part of this container
                <FALSE/> otherwise

    @threadsafe yes
    @modified   01.07.2002 14:55,as96863
 *****************************************************************************************************************/
sal_Bool FrameContainer::exist( const css::uno::Reference< css::frame::XFrame >& xFrame ) const
{
    /* SAFE { */
    ReadGuard aReadLock( m_aLock );
    return( ::std::find( m_aContainer.begin(), m_aContainer.end(), xFrame ) != m_aContainer.end() );
    /* } SAFE */
}

/**-***************************************************************************************************************
    @short      delete all existing items of the container
    @descr      -

    @threadsafe yes
    @modified   01.07.2002 15:00,as96863
 *****************************************************************************************************************/
void FrameContainer::clear()
{
    // SAFE {
    WriteGuard aWriteLock( m_aLock );

	// Clear the container...
    m_aContainer.clear();
	// ...and don't forget to reset the active frame.
	// It's a reference to a valid container item.
	// But no container item => no active frame!
    m_xActiveFrame = css::uno::Reference< css::frame::XFrame >();
/*DEPRECATEME
    // If special quit mode is used - we must terminate the desktop.
	// He is the owner of this container and can't work without any visible tasks/frames!
    if (m_bAsyncQuit)
        m_aAsyncCall.Post(0);
*/

    aWriteLock.unlock();
    // } SAFE
}

/**-***************************************************************************************************************
    @short      returns count of all current existing frames
    @descr      -

    @deprecated This value can't be guaranteed for multithreading environments.
                So it will be marked as deprecated and should be replaced by "getAllElements()".

    @return     the count of existing container items

    @threadsafe yes
    @modified   01.07.2002 15:00,as96863
 *****************************************************************************************************************/
sal_uInt32 FrameContainer::getCount() const
{
    /* SAFE { */
    ReadGuard aReadLock( m_aLock );
    return( (sal_uInt32)m_aContainer.size() );
    /* } SAFE */
}

/**-***************************************************************************************************************
    @short      returns one item of this container
    @descr      -

    @deprecated This value can't be guaranteed for multithreading environments.
                So it will be marked as deprecated and should be replaced by "getAllElements()".

    @param      nIndex
                    a value between 0 and (getCount()-1) to address one container item

    @return     a reference to a frame inside the container, which matches with given index

    @threadsafe yes
    @modified   01.07.2002 15:03,as96863
 *****************************************************************************************************************/
css::uno::Reference< css::frame::XFrame > FrameContainer::operator[]( sal_uInt32 nIndex ) const
{

    css::uno::Reference< css::frame::XFrame > xFrame;
    try
    {
        // Get element form container WITH automatic test of ranges!
        // If index not valid, a out_of_range exception is thrown.
        /* SAFE { */
        ReadGuard aReadLock( m_aLock );
        xFrame = m_aContainer.at( nIndex );
        aReadLock.unlock();
        /* } SAFE */
    }
    catch( std::out_of_range& )
    {
        // The index is not valid for current container-content - we must handle this case!
        // We can return the default value...
        LOG_EXCEPTION( "FrameContainer::operator[]", "Exception caught...", DECLARE_ASCII("::std::out_of_range") )
    }
	return xFrame;
}

/**-***************************************************************************************************************
    @short      returns a snapshot of all currently existing frames inside this container
    @descr      Should be used to replace the deprecated functions getCount()/operator[]!

    @return     a list of all frame references inside this container

    @threadsafe yes
    @modified   01.07.2002 15:09,as96863
 *****************************************************************************************************************/
css::uno::Sequence< css::uno::Reference< css::frame::XFrame > > FrameContainer::getAllElements() const
{
    /* SAFE { */
    ReadGuard aReadLock( m_aLock );

    sal_Int32                                                       nPosition = 0;
    css::uno::Sequence< css::uno::Reference< css::frame::XFrame > > lElements ( (sal_uInt32)m_aContainer.size() );
    for (TConstFrameIterator pItem=m_aContainer.begin(); pItem!=m_aContainer.end(); ++pItem)
        lElements[nPosition++] = *pItem;

    aReadLock.unlock();
    /* } SAFE */

	return lElements;
}

/**-***************************************************************************************************************
    @short      set the given frame as the new active one inside this container
    @descr      We accept this frame only, if it's already a part of this container.

    @param      xFrame
                    reference to the new active frame
                    Must be a valid reference and already part of this container.

    @threadsafe yes
    @modified   01.07.2002 15:11,as96863
 *****************************************************************************************************************/
void FrameContainer::setActive( const css::uno::Reference< css::frame::XFrame >& xFrame )
{
    if ( !xFrame.is() || exist(xFrame) )
    {
        /* SAFE { */
        WriteGuard aWriteLock( m_aLock );
        m_xActiveFrame = xFrame;
        aWriteLock.unlock();
        /* } SAFE */
    }
}

/**-***************************************************************************************************************
    @short      returns the current active frame of this container
    @descr      Value can be null in case the frame was removed from the container and nobody
                from outside decides which of all others should be the new one...

    @return     a reference to the current active frame
                Value can be NULL!

    @threadsafe yes
    @modified   01.07.2002 15:11,as96863
 *****************************************************************************************************************/
css::uno::Reference< css::frame::XFrame > FrameContainer::getActive() const
{
    /* SAFE { */
    ReadGuard aReadLock( m_aLock );
    return m_xActiveFrame;
    /* } SAFE */
}

/**-***************************************************************************************************************
    @short      implements a simple search based on current container items
    @descr      It can be used for findFrame() and implements a deep down search.

    @param      sName
                    target name, which is searched

    @return     reference to the found frame or NULL if not.

    @threadsafe yes
    @modified   01.07.2002 15:22,as96863
 *****************************************************************************************************************/
css::uno::Reference< css::frame::XFrame > FrameContainer::searchOnAllChildrens( const ::rtl::OUString& sName ) const
{
    /* SAFE { */
    ReadGuard aReadLock( m_aLock );

    // Step over all child frames. But if direct child isn't the right one search on his children first - before
    // you go to next direct child of this container!
    css::uno::Reference< css::frame::XFrame > xSearchedFrame;
    for( TConstFrameIterator pIterator=m_aContainer.begin(); pIterator!=m_aContainer.end(); ++pIterator )
    {
        if ((*pIterator)->getName()==sName)
        {
            xSearchedFrame = *pIterator;
            break;
        }
        else
        {
            xSearchedFrame = (*pIterator)->findFrame( sName, css::frame::FrameSearchFlag::CHILDREN );
            if (xSearchedFrame.is())
                break;
        }
    }
    aReadLock.unlock();
    /* } SAFE */
	return xSearchedFrame;
}

/**-***************************************************************************************************************
    @short      implements a simple search based on current container items
    @descr      It can be used for findFrame() and search on members of this container only!

    @param      sName
                    target name, which is searched

    @return     reference to the found frame or NULL if not.

    @threadsafe yes
    @modified   01.07.2002 15:22,as96863
 *****************************************************************************************************************/
css::uno::Reference< css::frame::XFrame > FrameContainer::searchOnDirectChildrens( const ::rtl::OUString& sName ) const
{
    /* SAFE { */
    ReadGuard aReadLock( m_aLock );

    css::uno::Reference< css::frame::XFrame > xSearchedFrame;
    for( TConstFrameIterator pIterator=m_aContainer.begin(); pIterator!=m_aContainer.end(); ++pIterator )
    {
        if ((*pIterator)->getName()==sName)
        {
            xSearchedFrame = *pIterator;
            break;
        }
    }
    aReadLock.unlock();
    /* } SAFE */
	return xSearchedFrame;
}

} // namespace framework
