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


#ifndef _FINALTHREADMANAGER_HXX
#define _FINALTHREADMANAGER_HXX

#include "sal/config.h"
#include "cppuhelper/factory.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "cppuhelper/implbase3.hxx"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/util/XJobManager.hpp"
#include "com/sun/star/frame/XTerminateListener2.hpp"


#include <osl/mutex.hxx>

#include <list>

class CancelJobsThread;
class TerminateOfficeThread;
class SwPauseThreadStarting;

// service helper namespace
namespace comp_FinalThreadManager {

// component and service helper functions:
::rtl::OUString SAL_CALL _getImplementationName();
com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL _getSupportedServiceNames();
com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SAL_CALL _create(
    com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext > const & context );

} // closing service helper namespace


class FinalThreadManager : public ::cppu::WeakImplHelper3< com::sun::star::lang::XServiceInfo,
                                                           com::sun::star::util::XJobManager,
                                                           com::sun::star::frame::XTerminateListener2 >
{
public:
    explicit FinalThreadManager(com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext > const & context);

    // ::com::sun::star::lang::XServiceInfo:
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual ::sal_Bool SAL_CALL supportsService(const ::rtl::OUString & ServiceName);
    virtual com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

    // ::com::sun::star::util::XJobManager:
    virtual void SAL_CALL registerJob(const com::sun::star::uno::Reference< com::sun::star::util::XCancellable > & Job);
    virtual void SAL_CALL releaseJob(const com::sun::star::uno::Reference< com::sun::star::util::XCancellable > & Job);
    virtual void SAL_CALL cancelAllJobs();

    // ::com::sun::star::frame::XTerminateListener2
    virtual void SAL_CALL cancelTermination( const ::com::sun::star::lang::EventObject& Event );

    // ::com::sun::star::frame::XTerminateListener (inherited via com::sun::star::frame::XTerminateListener2)
    virtual void SAL_CALL queryTermination( const ::com::sun::star::lang::EventObject& Event );
    virtual void SAL_CALL notifyTermination( const ::com::sun::star::lang::EventObject& Event );

    // ::com::sun:star::lang::XEventListener (inherited via com::sun::star::frame::XTerminateListener)
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source );

private:
    FinalThreadManager(FinalThreadManager &); // not defined
    void operator =(FinalThreadManager &); // not defined

    virtual ~FinalThreadManager();

    void registerAsListenerAtDesktop();

    com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext > m_xContext;

    osl::Mutex maMutex;

    std::list< com::sun::star::uno::Reference< com::sun::star::util::XCancellable > > maThreads;
    CancelJobsThread* mpCancelJobsThread;
    TerminateOfficeThread* mpTerminateOfficeThread;
    SwPauseThreadStarting* mpPauseThreadStarting;

    bool mbRegisteredAtDesktop;
};
#endif
