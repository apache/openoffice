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
//	my own includes
//_________________________________________________________________________________________________________________
#include <dispatch/mailtodispatcher.hxx>
#include <threadhelp/readguard.hxx>
#include <general.h>
#include <services.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/system/SystemShellExecute.hpp>
#include <com/sun/star/system/SystemShellExecuteFlags.hpp>
#include <com/sun/star/frame/DispatchResultState.hpp>

//_________________________________________________________________________________________________________________
//	includes of other projects
//_________________________________________________________________________________________________________________

#include <vcl/svapp.hxx>

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

namespace framework{

//_________________________________________________________________________________________________________________
//	non exported const
//_________________________________________________________________________________________________________________

#define PROTOCOL_VALUE      "mailto:"
#define PROTOCOL_LENGTH     7

//_________________________________________________________________________________________________________________
//	non exported definitions
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	declarations
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
// XInterface, XTypeProvider, XServiceInfo

DEFINE_XINTERFACE_5(MailToDispatcher                                ,
                    OWeakObject                                     ,
                    DIRECT_INTERFACE(css::lang::XTypeProvider      ),
                    DIRECT_INTERFACE(css::lang::XServiceInfo       ),
                    DIRECT_INTERFACE(css::frame::XDispatchProvider ),
                    DIRECT_INTERFACE(css::frame::XNotifyingDispatch),
                    DIRECT_INTERFACE(css::frame::XDispatch         ))

DEFINE_XTYPEPROVIDER_5(MailToDispatcher              ,
                       css::lang::XTypeProvider      ,
                       css::lang::XServiceInfo       ,
                       css::frame::XDispatchProvider ,
                       css::frame::XNotifyingDispatch,
                       css::frame::XDispatch         )

DEFINE_XSERVICEINFO_MULTISERVICE_2(MailToDispatcher                   ,
                                   ::cppu::OWeakObject                ,
                                   SERVICENAME_PROTOCOLHANDLER        ,
                                   IMPLEMENTATIONNAME_MAILTODISPATCHER)

DEFINE_INIT_SERVICE(MailToDispatcher,
                    {
                        /*Attention
                            I think we don't need any mutex or lock here ... because we are called by our own static method impl_createInstance()
                            to create a new instance of this class by our own supported service factory.
                            see macro DEFINE_XSERVICEINFO_MULTISERVICE and "impl_initService()" for further informations!
                        */
                    }
                   )

//_________________________________________________________________________________________________________________

/**
    @short      standard ctor
    @descr      These initialize a new instance of this class with needed informations for work.

    @param      xContext
                    reference to uno component context

    @modified   30.04.2002 14:10, as96863
*/
MailToDispatcher::MailToDispatcher( const css::uno::Reference< css::uno::XComponentContext >& xContext )
		//	Init baseclasses first
        : ThreadHelpBase( &Application::GetSolarMutex() )
        , OWeakObject   (                               )
        // Init member
        , m_xContext    ( xContext                      )
{
}

//_________________________________________________________________________________________________________________

/**
    @short      standard dtor
    @descr      -

    @modified   30.04.2002 14:10, as96863
*/
MailToDispatcher::~MailToDispatcher()
{
    m_xContext = NULL;
}

//_________________________________________________________________________________________________________________

/**
    @short      decide if this dispatch implementation can be used for requested URL or not
    @descr      A protocol handler is registerd for an URL pattern inside configuration and will
                be asked by the generic dispatch mechanism inside framework, if he can handle this
                special URL which match his registration. He can agree by returning of a valid dispatch
                instance or disagree by returning <NULL/>.
                We don't create new dispatch instances here really - we return THIS as result to handle it
                at the same implementation.

    @modified   02.05.2002 15:25, as96863
*/
css::uno::Reference< css::frame::XDispatch > SAL_CALL MailToDispatcher::queryDispatch( const css::util::URL&  aURL    ,
                                                                                       const ::rtl::OUString& /*sTarget*/ ,
                                                                                             sal_Int32        /*nFlags*/  ) throw( css::uno::RuntimeException )
{
    css::uno::Reference< css::frame::XDispatch > xDispatcher;
    if (aURL.Complete.compareToAscii(PROTOCOL_VALUE,PROTOCOL_LENGTH)==0)
        xDispatcher = this;
    return xDispatcher;
}

//_________________________________________________________________________________________________________________

/**
    @short      do the same like dispatch() but for multiple requests at the same time
    @descr      -

    @modified   02.05.2002 15:27, as96863
*/
css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > > SAL_CALL MailToDispatcher::queryDispatches( const css::uno::Sequence< css::frame::DispatchDescriptor >& lDescriptor ) throw( css::uno::RuntimeException )
{
    sal_Int32 nCount = lDescriptor.getLength();
    css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > > lDispatcher( nCount );
    for( sal_Int32 i=0; i<nCount; ++i )
    {
        lDispatcher[i] = this->queryDispatch(
                            lDescriptor[i].FeatureURL,
                            lDescriptor[i].FrameName,
                            lDescriptor[i].SearchFlags);
    }
    return lDispatcher;
}

//_________________________________________________________________________________________________________________

/**
    @short      dispatch URL with arguments
    @descr      We use threadsafe internal method to do so. It returns a state value - but we ignore it.
                Because we don't support status listener notifications here. Status events are not guaranteed -
                and we call another service internally which doesn't return any notifications too.

    @param      aURL
                    mail URL which should be executed
    @param      lArguments
                    list of optional arguments for this mail request

    @modified   30.04.2002 14:15, as96863
*/
void SAL_CALL MailToDispatcher::dispatch( const css::util::URL&                                  aURL       ,
                                          const css::uno::Sequence< css::beans::PropertyValue >& lArguments ) throw( css::uno::RuntimeException )
{
    // dispatch() is an [oneway] call ... and may our user release his reference to us immediately.
    // So we should hold us self alive till this call ends.
    css::uno::Reference< css::frame::XNotifyingDispatch > xSelfHold(static_cast< ::cppu::OWeakObject* >(this), css::uno::UNO_QUERY);
    implts_dispatch(aURL,lArguments);
    // No notification for status listener!
}

//_________________________________________________________________________________________________________________

/**
    @short      dispatch with guaranteed notifications about success
    @descr      We use threadsafe internal method to do so. Return state of this function will be used
                for notification if an optional listener is given.

    @param      aURL
                    mail URL which should be executed
    @param      lArguments
                    list of optional arguments for this mail request
    @param      xListener
                    reference to a valid listener for state events

    @modified   30.04.2002 14:49, as96863
*/
void SAL_CALL MailToDispatcher::dispatchWithNotification( const css::util::URL&                                             aURL      ,
                                                          const css::uno::Sequence< css::beans::PropertyValue >&            lArguments,
                                                          const css::uno::Reference< css::frame::XDispatchResultListener >& xListener ) throw( css::uno::RuntimeException )
{
    // This class was designed to die by reference. And if user release his reference to us immediately after calling this method
    // we can run into some problems. So we hold us self alive till this method ends.
    // Another reason: We can use this reference as source of sending event at the end too.
    css::uno::Reference< css::frame::XNotifyingDispatch > xThis(static_cast< ::cppu::OWeakObject* >(this), css::uno::UNO_QUERY);

    sal_Bool bState = implts_dispatch(aURL,lArguments);
    if (xListener.is())
    {
        css::frame::DispatchResultEvent aEvent;
        if (bState)
            aEvent.State = css::frame::DispatchResultState::SUCCESS;
        else
            aEvent.State = css::frame::DispatchResultState::FAILURE;
        aEvent.Source = xThis;

        xListener->dispatchFinished( aEvent );
    }
}

//_________________________________________________________________________________________________________________

/**
    @short      threadsafe helper for dispatch calls
    @descr      We support two interfaces for the same process - dispatch URLs. That the reason for this internal
                function. It implements the real dispatch operation and returns a state value which inform caller
                about success. He can notify listener then by using this return value.

    @param      aURL
                    mail URL which should be executed
    @param      lArguments
                    list of optional arguments for this mail request

    @return     <TRUE/> if dispatch could be started successfully
                Note: Our internal used shell executor doesn't return any state value - so we must
                believe that call was successfully.
                <FALSE/> if necessary ressource couldn't be created or an exception was thrown.

    @modified   30.04.2002 14:49, as96863
*/
sal_Bool MailToDispatcher::implts_dispatch( const css::util::URL&                                  aURL       ,
                                            const css::uno::Sequence< css::beans::PropertyValue >& /*lArguments*/ ) throw( css::uno::RuntimeException )
{
    sal_Bool bSuccess = sal_False;

    css::uno::Reference< css::uno::XComponentContext > xContext;
    /* SAFE */{
        ReadGuard aReadLock( m_aLock );
        xContext = m_xContext;
    /* SAFE */}

    css::uno::Reference< css::system::XSystemShellExecute > xSystemShellExecute(
        css::system::SystemShellExecute::create( xContext ) );
    if (xSystemShellExecute.is())
    {
        try
        {
            // start mail client
            // Because there is no notification about success - we use case of
            // no detected exception as SUCCESS - FAILED otherwise.
            xSystemShellExecute->execute( aURL.Complete, ::rtl::OUString(), css::system::SystemShellExecuteFlags::DEFAULTS );
            bSuccess = sal_True;
        }
        catch (css::lang::IllegalArgumentException&)
        {
        }
        catch (css::system::SystemShellExecuteException&)
        {
        }
    }

    return bSuccess;
}

//_________________________________________________________________________________________________________________

/**
    @short      add/remove listener for state events
    @descr      Because we use an external process to forward such mail URLs, and this process doesn't
                return any notifications about success or failed state - we doesn't support such status
                listener. We have no status to send.

    @param      xListener
                    reference to a valid listener for state events
    @param      aURL
                    URL about listener will be informed, if something occurred

    @modified   30.04.2002 14:49, as96863
*/
void SAL_CALL MailToDispatcher::addStatusListener( const css::uno::Reference< css::frame::XStatusListener >& /*xListener*/ ,
                                                   const css::util::URL&                                     /*aURL*/      ) throw( css::uno::RuntimeException )
{
    // not suported yet
}

//_________________________________________________________________________________________________________________

void SAL_CALL MailToDispatcher::removeStatusListener( const css::uno::Reference< css::frame::XStatusListener >& /*xListener*/ ,
                                                      const css::util::URL&                                     /*aURL*/      ) throw( css::uno::RuntimeException )
{
    // not suported yet
}

} //  namespace framework
