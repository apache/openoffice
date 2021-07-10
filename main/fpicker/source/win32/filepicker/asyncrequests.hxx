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



#ifndef FPICKER_WIN32_VISTA_ASYNCREQUESTS_HXX
#define FPICKER_WIN32_VISTA_ASYNCREQUESTS_HXX

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <cppuhelper/basemutex.hxx>
#include <comphelper/sequenceashashmap.hxx>
#include <osl/conditn.hxx>
#include <osl/thread.hxx>
#include <osl/time.h>
#include <queue>
#include <boost/shared_ptr.hpp>

//-----------------------------------------------------------------------------
// namespace
//-----------------------------------------------------------------------------

#ifdef css
    #error "Clash on using CSS as namespace define."
#else
    #define css ::com::sun::star
#endif

namespace fpicker{
namespace win32{
namespace vista{

//-----------------------------------------------------------------------------
/** @todo document me
 */
class Request
{
    //-------------------------------------------------------------------------
    public:
        
        static const ::sal_Int32 WAIT_INFINITE = 0;
    
    //-------------------------------------------------------------------------
    // interface
    //-------------------------------------------------------------------------
    
    public:
        
        //---------------------------------------------------------------------
        explicit Request()
            : m_aJoiner   (  )
            , m_nRequest  (-1)
            , m_lArguments(  )
        {
            m_aJoiner.reset();
        }
    
        //---------------------------------------------------------------------
        virtual ~Request() {};
    
        //---------------------------------------------------------------------
        void setRequest(::sal_Int32 nRequest)
        {
            m_nRequest = nRequest;
        }
    
        //---------------------------------------------------------------------
        ::sal_Int32 getRequest()
        {
            return m_nRequest;
        }
    
        //---------------------------------------------------------------------
        void clearArguments()
        {
            m_lArguments.clear();
        }
    
        //---------------------------------------------------------------------
        template< class TArgumentType >
        void setArgument(const ::rtl::OUString& sName ,
                         const TArgumentType&   aValue)
        {
            m_lArguments[sName] <<= aValue;
        }
    
        //---------------------------------------------------------------------
        template< class TArgumentType >
        TArgumentType getArgumentOrDefault(const ::rtl::OUString& sName   ,
                                           const TArgumentType&   aDefault)
        {
            return m_lArguments.getUnpackedValueOrDefault(sName, aDefault);
        }
    
        //---------------------------------------------------------------------
        void wait(::sal_Int32 nMilliSeconds = WAIT_INFINITE);
		
		void waitProcessMessages();
    
        //---------------------------------------------------------------------
        void notify();

    //-------------------------------------------------------------------------
    // member
    //-------------------------------------------------------------------------
    
    private:
        
        ::osl::Condition m_aJoiner;
        ::sal_Int32 m_nRequest;
        ::comphelper::SequenceAsHashMap m_lArguments;
};

typedef ::boost::shared_ptr< Request > RequestRef;
typedef ::std::queue< RequestRef >  RequestQueue;

//-----------------------------------------------------------------------------
class RequestHandler
{
    public:
        virtual void before() = 0;
        virtual void doRequest(const RequestRef& rRequest) = 0;
        virtual void after() = 0;
};

typedef ::boost::shared_ptr< RequestHandler > RequestHandlerRef;

//-----------------------------------------------------------------------------
/** @todo docuemnt me
 */
class AsyncRequests : private ::cppu::BaseMutex
                    , public  ::osl::Thread
{
    public:
		static const ::sal_Int16  PROCESS_MESSAGES = 2;	
        static const ::sal_Int16  BLOCKED       = 1;
        static const ::sal_Int16  NON_BLOCKED   = 0;

        //---------------------------------------------------------------------
        /** creates the new asynchronous request executor.
         */
        explicit AsyncRequests(const RequestHandlerRef& rHandler);
    
        void setHandler(const RequestHandlerRef& rHandler)
        {
            m_rHandler = rHandler;
        }
    
        //---------------------------------------------------------------------
        /** does nothing special / except to make sure our class won't be inline .-)
         */
        virtual ~AsyncRequests();
        
		//---------------------------------------------------------------------
        /** @todo document me
         */
		void triggerRequestProcessMessages (const RequestRef& rRequest);
    
        //---------------------------------------------------------------------
        /** @todo document me
         */
        void triggerRequestBlocked(const RequestRef& rRequest);
    
        //---------------------------------------------------------------------
        /** @todo document me
         */
        void triggerRequestNonBlocked(const RequestRef& rRequest);
    
        //---------------------------------------------------------------------
        /** @todo document me
         */
        void triggerRequestDirectly(const RequestRef& rRequest);
    
        //---------------------------------------------------------------------
        /** @todo document me
         */
        void triggerRequestThreadAware(const RequestRef& rRequest,
                                             ::sal_Int16  nWait   );
        
    private:
        
        //---------------------------------------------------------------------
        /** our STA .-)
         *  Will run between start() & finish(). Internally it runs a loop ... 
         *  waiting for requests. Every request will be executed synchronously
         *  in blocked mode.
         */
        virtual void SAL_CALL run();
    
    private:
        
        ::sal_Bool m_bFinish;
        RequestHandlerRef m_rHandler;
        RequestQueue m_lRequests;
};

} // namespace vista
} // namespace win32
} // namespace fpicker

#undef css

#endif // FPICKER_WIN32_VISTA_ASYNCREQUESTS_HXX
