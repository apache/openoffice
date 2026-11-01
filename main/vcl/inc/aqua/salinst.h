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



#ifndef _SV_SALINST_H
#define _SV_SALINST_H

#include "vos/mutex.hxx"
#include "vos/thread.hxx"
#include "osl/conditn.h"

#include "aqua/aquavcltypes.h"

#include "salinst.hxx"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <exception>
#include <list>
#include <pthread.h>

class AquaSalFrame;
class ApplicationEvent;
class Image;

// -----------------
// - SalYieldMutex -
// -----------------

class SalYieldMutex : public vos::OMutex
{
	sal_uLong										mnCount;
	vos::OThread::TThreadIdentifier             mnThreadId;
    vos::OThread::TThreadIdentifier             mnMainThreadId;

    // state for running code on the main thread on behalf of the owner
    pthread_mutex_t                             maMainThreadMutex;
    pthread_cond_t                              maMainThreadCond;
    pthread_cond_t                              maResultCond;
    bool                                        mbWakeUpMain;
    bool                                        mbMainWaiting;
    bool                                        mbResultReady;
    const boost::function0<void>*               mpMainThreadFunc;
    std::exception_ptr                          maMainThreadException;
    bool                                        mbRunningForOwner;  // main thread only

    bool                                        isMainThread() const;
    void                                        acquireOnMainThread();
    void                                        runForOwner( const boost::function0<void>* pFunc );

public:
												SalYieldMutex();
                                                ~SalYieldMutex();
	virtual void								acquire();
	virtual void								release();
	virtual sal_Bool							tryToAcquire();
	sal_uLong										GetAcquireCount() const { return mnCount; }
	vos::OThread::TThreadIdentifier             GetThreadId() const { return mnThreadId; }

    void                                        setMainThread( vos::OThread::TThreadIdentifier nId ) { mnMainThreadId = nId; }
    // runs rFunc on the main thread while the calling thread keeps owning the mutex
    void                                        runOnMainThread( const boost::function0<void>& rFunc );
    // true while the main thread runs code for the owner; the main thread then counts as owner
    bool                                        isRunningForOwner() const;
    // runs a function handed over by runOnMainThread(), if any; main thread only
    void                                        runPendingOnMainThread();
};

#define YIELD_GUARD vos::OGuard aGuard( GetSalData()->mpFirstInstance->GetYieldMutex() )


// -------------------
// - SalInstanceData -
// -------------------

//struct SalInstanceData
//{
//public:
//};

// ------------------
// - AquaSalInstance -
// ------------------

class AquaSalInstance : public SalInstance
{
    struct SalUserEvent
    {
        AquaSalFrame*   mpFrame;
        void*           mpData;
        sal_uInt16          mnType;

        SalUserEvent( AquaSalFrame* pFrame, void* pData, sal_uInt16 nType ) :
            mpFrame( pFrame ), mpData( pData ), mnType( nType )
        {}
    };

public:
	SalYieldMutex*		                    mpSalYieldMutex;		// Sal-Yield-Mutex
    rtl::OUString                           maDefaultPrinter;
    vos::OThread::TThreadIdentifier         maMainThread;
    bool                                    mbWaitingYield;
    int                                     mnActivePrintJobs;
    std::list< SalUserEvent >               maUserEvents;
    oslMutex                                maUserEventListMutex;
    oslCondition                            maWaitingYieldCond;

    typedef std::list<const ApplicationEvent*> AppEventList;
    static AppEventList aAppEventList;

public:
    AquaSalInstance();
    virtual ~AquaSalInstance();

    virtual SalSystem*      CreateSystem();
    virtual void            DestroySystem(SalSystem*);
    virtual SalFrame*      	CreateChildFrame( SystemParentData* pParent, sal_uLong nStyle );
    virtual SalFrame*      	CreateFrame( SalFrame* pParent, sal_uLong nStyle );
    virtual void			DestroyFrame( SalFrame* pFrame );
    virtual SalObject*		CreateObject( SalFrame* pParent, SystemWindowData* pWindowData, sal_Bool bShow = sal_True );
    virtual void			DestroyObject( SalObject* pObject );
    virtual SalVirtualDevice*	CreateVirtualDevice( SalGraphics* pGraphics,
                                                     long nDX, long nDY,
                                                     sal_uInt16 nBitCount, const SystemGraphicsData *pData );
    virtual void			DestroyVirtualDevice( SalVirtualDevice* pDevice );

    virtual SalInfoPrinter*	CreateInfoPrinter( SalPrinterQueueInfo* pQueueInfo,
                                               ImplJobSetup* pSetupData );
    virtual void			DestroyInfoPrinter( SalInfoPrinter* pPrinter );
    virtual SalPrinter*		CreatePrinter( SalInfoPrinter* pInfoPrinter );
    virtual void			DestroyPrinter( SalPrinter* pPrinter );
    virtual void			GetPrinterQueueInfo( ImplPrnQueueList* pList );
    virtual void			GetPrinterQueueState( SalPrinterQueueInfo* pInfo );
    virtual void			DeletePrinterQueueInfo( SalPrinterQueueInfo* pInfo );
    virtual String             GetDefaultPrinter();
    virtual SalTimer*			CreateSalTimer();
    virtual SalI18NImeStatus*	CreateI18NImeStatus();
    virtual SalSystem*			CreateSalSystem();
    virtual SalBitmap*			CreateSalBitmap();
    virtual vos::IMutex*		GetYieldMutex();
    virtual sal_uLong				ReleaseYieldMutex();
    virtual void				AcquireYieldMutex( sal_uLong nCount );
    virtual bool                CheckYieldMutex();
    virtual void				Yield( bool bWait, bool bHandleAllCurrentEvents );
    virtual bool				AnyInput( sal_uInt16 nType );
    virtual SalMenu*			CreateMenu( sal_Bool bMenuBar, Menu* pVCLMenu );
    virtual void				DestroyMenu( SalMenu* );
    virtual SalMenuItem*		CreateMenuItem( const SalItemParams* pItemData );
    virtual void				DestroyMenuItem( SalMenuItem* );
    virtual SalSession*         CreateSalSession();
    virtual void*				GetConnectionIdentifier( ConnectionIdentifierType& rReturnedType, int& rReturnedBytes );
    virtual void                AddToRecentDocumentList(const rtl::OUString& rFileUrl, const rtl::OUString& rMimeType);
    virtual void            SetEventCallback( void* pInstance, bool(*pCallback)(void*,void*,int) );
    virtual void            SetErrorEventCallback( void* pInstance, bool(*pCallback)(void*,void*,int) );

    // dtrans implementation
    virtual com::sun::star::uno::Reference< com::sun::star::uno::XInterface >
        CreateClipboard( const com::sun::star::uno::Sequence< com::sun::star::uno::Any >& i_rArguments );
    virtual com::sun::star::uno::Reference< com::sun::star::uno::XInterface > CreateDragSource();
    virtual com::sun::star::uno::Reference< com::sun::star::uno::XInterface > CreateDropTarget();

    static void handleAppDefinedEvent( NSEvent* pEvent );

    // check whether a particular string is passed on the command line
    // this is needed to avoid duplicate open events through a) command line and b) NSApp's openFile
    static bool isOnCommandLine( const rtl::OUString& );

    void wakeupYield();

    template< typename R > static void assignResult( R* pResult, const boost::function0<R>& rFunc )
    {
        *pResult = rFunc();
    }

 public:
    friend class AquaSalFrame;

    void PostUserEvent( AquaSalFrame* pFrame, sal_uInt16 nType, void* pData );
    void delayedSettingsChanged( bool bInvalidate );

    bool isNSAppThread() const;

    // AppKit raises or deadlocks when used off the main thread (e.g. by remote UNO calls):
    // off the main thread these run rFunc there and return true, on it they return false
    bool forwardToMainThread( const boost::function0<void>& rFunc );
    template< typename R > R callOnMainThread( const boost::function0<R>& rFunc )
    {
        R aResult = R();
        mpSalYieldMutex->runOnMainThread( boost::bind( &AquaSalInstance::assignResult<R>, &aResult, boost::cref( rFunc ) ) );
        return aResult;
    }

    void startedPrintJob() { mnActivePrintJobs++; }
    void endedPrintJob() { mnActivePrintJobs--; }

    // event subtypes for NSApplicationDefined events
    static const short AppExecuteSVMain   = 0x7fff;
    static const short AppEndLoopEvent    = 1;
    static const short AppStartTimerEvent = 10;
    static const short AppleRemoteEvent   = 15;
    static const short YieldWakeupEvent   = 20;

    static NSMenu* GetDynamicDockMenu();
};

// helper class: inverted solar guard
class YieldMutexReleaser
{
    sal_uLong mnCount;
    public:
    YieldMutexReleaser();
    ~YieldMutexReleaser();
};

// helper class
rtl::OUString GetOUString( CFStringRef );
rtl::OUString GetOUString( NSString* );
CFStringRef CreateCFString( const rtl::OUString& );
NSString* CreateNSString( const rtl::OUString& );

CGImageRef CreateCGImage( const Image& );
NSImage*   CreateNSImage( const Image& );

#endif // _SV_SALINST_H
