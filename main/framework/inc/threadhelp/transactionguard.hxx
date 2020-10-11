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



#ifndef __FRAMEWORK_THREADHELP_TRANSACTIONGUARD_HXX_
#define __FRAMEWORK_THREADHELP_TRANSACTIONGUARD_HXX_

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________

#include <threadhelp/inoncopyable.h>
#include <threadhelp/itransactionmanager.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________

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
    @short          implement a guard to support non breakable transactions
    @descr          If you whish to support non breakable method calls without lockingf any mutex, rw-lock or
                    something like that - you should use this guard implementation.
                    Initialize it at first in your method and don't release it till end of your function!
                    Your "transaction" is registered in ctor and automatically released in dtor.
                    Use set/get of working mode to enable/disable further transactions.
                    It's possible too, to enable automatically throwing of some exceptions for illegal
                    transaction requests ... e.g. interface call for already disposed objects.

	@attention		To prevent us against wrong using, the default ctor, copy ctor and the =operator are marked private!

	@implements		-
    @base           INonCopyable

    @devstatus      draft
*//*-*************************************************************************************************************/
class TransactionGuard : private INonCopyable
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------
	public:

        /*-****************************************************************************************************//**
            @short      ctors
            @descr      Use these ctor methods to initialize the guard right.
                        Given reference must be valid - otherwise crashes could occur!

            @attention  It's not necessary to lock any mutex here! Because a ctor should not be called
                        from different threads at the same time ... this class use no refcount mechanism!

            @seealso    -

            @param      "rManager"  reference to transaction manager for using to register a request
            @param      "eMode"     enable/disable throwing of exceptions for rejected calls
            @param      "eReason"   returns reason for rejected calls if "eMode=E_NOEXCEPTIONS"!
            @return     -

            @onerror    -
        *//*-*****************************************************************************************************/
        inline TransactionGuard( ITransactionManager& rManager, EExceptionMode eMode, ERejectReason* eReason = NULL )
            : m_pManager( &rManager )
        {
            // If exception mode is set to E_HARDEXCETIONS we don't need a buffer to return reason!
            // We handle it private. If a call is rejected, our manager throw some exceptions ... and the reason
            // could be ignorable ...
            if( eReason == NULL )
            {
                ERejectReason eMyReason;
                m_pManager->registerTransaction( eMode, eMyReason );
            }
            else
            {
                m_pManager->registerTransaction( eMode, *eReason );
            }
        }

        /*-************************************************************************************************************//**
            @short      dtor
            @descr      We must release the transaction manager and can forget his pointer.

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*************************************************************************************************************/
        inline ~TransactionGuard()
        {
            stop();
        }

        /*-************************************************************************************************************//**
            @short      stop current transaction
            @descr      We must release the transaction manager and can forget his pointer.

            @attention  We don't support any start() method here - because it is not easy to
                        detect if a transaction already started or not!
                        (combination of EExceptionMode and ERejectReason)

            @seealso    -

            @param      -
            @return     -

            @onerror    -
        *//*-*************************************************************************************************************/
        inline void stop()
        {
            if( m_pManager != NULL )
            {
                m_pManager->unregisterTransaction();
                m_pManager = NULL;
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
        TransactionGuard();

	//-------------------------------------------------------------------------------------------------------------
	//	private member
	//-------------------------------------------------------------------------------------------------------------
	private:

        ITransactionManager*   m_pManager   ;   /// pointer to safed transaction manager

};      //  class TransactionGuard

}		//	namespace framework

#endif  //  #ifndef __FRAMEWORK_THREADHELP_TRANSACTIONGUARD_HXX_
