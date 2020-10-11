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



#ifndef __FRAMEWORK_THREADHELP_FAIRRWLOCK_HXX_
#define __FRAMEWORK_THREADHELP_FAIRRWLOCK_HXX_

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________

#include <threadhelp/inoncopyable.h>
#include <threadhelp/irwlock.h>
#include <macros/debug.hxx>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/uno/XInterface.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <osl/mutex.hxx>
#include <osl/conditn.hxx>

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________
//	const
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	declarations
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
	@short          implement a read/write lock with fairness between read/write accessors
	@descr			These implementation never should used as base class! Use it as a member every time.
					Use ReadGuard and/or WriteGuard in your methods (which work with these lock)
					to make your code threadsafe.
					Fair means: All reading or writing threads are synchronized AND serialzed by using one
					mutex. For reader this mutex is used to access internal variables of this lock only;
					for writer this mutex is used to have an exclusiv access on your class member!
					=> It's a multi-reader/single-writer lock, which no preferred accessor.

	@implements		IRWlock
    @base           INonCopyable
					IRWLock

	@devstatus		ready to use
*//*-*************************************************************************************************************/
class FairRWLock : public  IRWLock
                 , private INonCopyable
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------
	public:

        /*-****************************************************************************************************//**
            @short      standard ctor
            @descr      Initialize instance with right start values for correct working.
                        no reader could exist               =>  m_nReadCount   = 0
                        don't block first coming writer    =>  m_aWriteCondition.set()

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline FairRWLock()
            : m_nReadCount( 0 )
        {
            m_aWriteCondition.set();
        }

        inline virtual ~FairRWLock()
        {
        }

        /*-****************************************************************************************************//**
            @interface  IRWLock
            @short      set lock for reading
            @descr      A guard should call this method to acquire read access on your member.
                        Writing isn't allowed then - but nobody could check it for you!

            @seealso    method releaseReadAccess()

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline virtual void acquireReadAccess()
        {
            // Put call in "SERIALIZE"-queue!
            // After successful acquiring this mutex we are alone ...
            ::osl::MutexGuard aSerializeGuard( m_aSerializer );

            // ... but we should synchronize us with other reader!
            // May be - they will unregister himself by using releaseReadAccess()!
            ::osl::MutexGuard aAccessGuard( m_aAccessLock );

            // Now we must register us as reader by increasing counter.
            // If this the first writer we must close door for possible writer.
            // Other reader don't look for this barrier - they work parallel to us!
            if( m_nReadCount == 0 )
            {
                m_aWriteCondition.reset();
            }
            ++m_nReadCount;
        }

        /*-****************************************************************************************************//**
            @interface  IRWLock
            @short      reset lock for reading
            @descr      A guard should call this method to release read access on your member.

            @seealso    method acquireReadAccess()

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline virtual void releaseReadAccess()
        {
            // The access lock is enough at this point
            // because it's not allowed to wait for all reader or writer here!
            // That will cause a deadlock!
            ::osl::MutexGuard aAccessGuard( m_aAccessLock );

            // Unregister as reader first!
            // Open writer barrier then if it was the last reader.
            --m_nReadCount;
            if( m_nReadCount == 0 )
            {
                m_aWriteCondition.set();
            }
        }

        /*-****************************************************************************************************//**
            @interface  IRWLock
            @short      set lock for writing
            @descr      A guard should call this method to acquire write access on your member.
                        Reading is allowed too - of course.
                        After successfully calling of this method you are the only writer.

            @seealso    method releaseWriteAccess()

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline virtual void acquireWriteAccess()
        {
            // You have to stand in our serialize-queue till all reader
            // are registered (not for releasing them!) or writer finished their work!
            // Don't use a guard to do so - because you must hold the mutex till
            // you call releaseWriteAccess()!
            // After succesfull acquire you have to wait for current working reader.
            // Used condition will open by last gone reader object.
            m_aSerializer.acquire();
            m_aWriteCondition.wait();

            #ifdef ENABLE_MUTEXDEBUG
            // A writer is an exclusiv accessor!
            LOG_ASSERT2( m_nReadCount!=0, "FairRWLock::acquireWriteAccess()", "No threadsafe code detected ... : Read count != 0!" )
            #endif
        }

        /*-****************************************************************************************************//**
            @interface  IRWLock
            @short      reset lock for writing
            @descr      A guard should call this method to release write access on your member.

            @seealso    method acquireWriteAccess()

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline virtual void releaseWriteAccess()
        {
            // The only one you have to do here is to release
            // hold seriliaze-mutex. All other user of these instance are blocked
            // by these mutex!
            // You don't need any other mutex here - you are the only one in the moment!

            #ifdef ENABLE_MUTEXDEBUG
            // A writer is an exclusiv accessor!
            LOG_ASSERT2( m_nReadCount!=0, "FairRWLock::releaseWriteAccess()", "No threadsafe code detected ... : Read count != 0!" )
            #endif

            m_aSerializer.release();
        }

        /*-****************************************************************************************************//**
            @interface  IRWLock
            @short      downgrade a write access to a read access
            @descr      A guard should call this method to change a write to a read access.
                        New readers can work too - new writer are blocked!

            @attention  Don't call this method if you are not a writer!
                        Results are not defined then ...
                        An upgrade can't be really implemented ... because acquiring new access
                        will be the same - there no differences!

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline virtual void downgradeWriteAccess()
        {
            // You must be a writer to call this method!
            // We can't check it - but otherwise it's your problem ...
            // That's why you don't need any mutex here.

            #ifdef ENABLE_MUTEXDEBUG
            // A writer is an exclusiv accessor!
            LOG_ASSERT2( m_nReadCount!=0, "FairRWLock::downgradeWriteAccess()", "No threadsafe code detected ... : Read count != 0!" )
            #endif

            // Register himself as "new" reader.
            // This value must be 0 before - because we support single writer access only!
            ++m_nReadCount;
            // Close barrier for other writer!
            // Why?
            // You hold the serializer mutex - next one can be a reader OR a writer.
            // They must blocked then - because you will be a reader after this call
            // and writer use this condition to wait for current reader!
            m_aWriteCondition.reset();
            // Open door for next waiting thread in serialize queue!
            m_aSerializer.release();
        }

	//-------------------------------------------------------------------------------------------------------------
	//	private member
	//-------------------------------------------------------------------------------------------------------------
	private:

		::osl::Mutex		m_aAccessLock		;	/// regulate access on internal member of this instance
		::osl::Mutex		m_aSerializer		;	/// serialze incoming read/write access threads
		::osl::Condition	m_aWriteCondition	;	/// a writer must wait till current working reader are gone
		sal_Int32			m_nReadCount		;	/// every reader is registered - the last one open the door for waiting writer

};		//	class FairRWLock

}		//	namespace framework

#endif	//	#ifndef __FRAMEWORK_THREADHELP_FAIRRWLOCK_HXX_
