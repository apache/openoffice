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



#ifndef __FRAMEWORK_THREADHELP_READGUARD_HXX_
#define __FRAMEWORK_THREADHELP_READGUARD_HXX_

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________

#include <threadhelp/inoncopyable.h>
#include <threadhelp/irwlock.h>

//#ifndef __FRAMEWORK_THREADHELP_THREADHELPBASE_HXX_
//#include <threadhelp/threadhelpbase.hxx>
//#endif

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <sal/types.h>

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
	@short          implement a guard to set read locks
	@descr			This guard should be used to set a lock for reading object internal member.
					Nobody can control it but don't use member after successfully locking for writing!
					We never need a own mutex to safe our internal member access - because
					a guard is used as function-local member only. There exists no multithreaded access to it really ...

    @attention      a) To prevent us against wrong using, the default ctor, copy ctor and the =operator are marked private!
                    b) Use interface "IRWLock" of set LockHelper only - because we must support a finer granularity of locking.
                       Interface "IMutex" should be used by easier guard implementations ... like "ResetableGuard"!

	@implements		-
    @base           INonCopyable

	@devstatus		ready to use
*//*-*************************************************************************************************************/
class ReadGuard : private INonCopyable
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------
	public:

        /*-****************************************************************************************************//**
            @short      ctor
            @descr      These ctors initialize the guard with a reference to used lock member of object to protect.
                        Null isn't allowed as value!

            @seealso    -

            @param      "pLock" ,reference to used lock member of object to protect
            @param      "rLock" ,reference to used lock member of object to protect
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline ReadGuard( IRWLock* pLock )
            :   m_pLock     ( pLock     )
            ,   m_bLocked   ( sal_False )
        {
            lock();
        }

        //*********************************************************************************************************
        inline ReadGuard( IRWLock& rLock )
            :   m_pLock     ( &rLock    )
            ,   m_bLocked   ( sal_False )
        {
            lock();
        }

        /*-****************************************************************************************************//**
            @short      dtor
            @descr      We unlock the used lock member automatically if user forgets it.

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline ~ReadGuard()
        {
            unlock();
        }

        /*-****************************************************************************************************//**
            @short      set read lock
            @descr      Call this method to set the read lock. The call will block till all current threads are synchronized!

            @seealso    method unlock()

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline void lock()
        {
            if( m_bLocked == sal_False )
            {
                m_pLock->acquireReadAccess();
                m_bLocked = sal_True;
            }
        }

        /*-****************************************************************************************************//**
            @short      unset read lock
            @descr      Call this method to unlock the rw-lock temp.!
                        Normally we do it at dtor automatically for you ...

            @seealso    method lock()

            @param      -
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline void unlock()
        {
            if( m_bLocked == sal_True )
            {
                m_pLock->releaseReadAccess();
                m_bLocked = sal_False;
            }
        }

	//-------------------------------------------------------------------------------------------------------------
	//	private methods
	//-------------------------------------------------------------------------------------------------------------
	private:

		/*-****************************************************************************************************//**
			@short		disable using of these functions!
			@descr		It's not allowed to use this methods. Different problems can occur otherwise.
						That's why we disable it by making it private.

			@seealso	other ctor

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/
		ReadGuard();

	//-------------------------------------------------------------------------------------------------------------
	//	private member
	//-------------------------------------------------------------------------------------------------------------
	private:

        IRWLock*    m_pLock     ;   /// reference to lock-member of protected object
		sal_Bool	m_bLocked	;	/// protection against multiple lock calls without unlock!

};		//	class ReadGuard

}		//	namespace framework

#endif	//	#ifndef __FRAMEWORK_THREADHELP_READGUARD_HXX_
