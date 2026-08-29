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



#ifndef __FRAMEWORK_HANDLER_SOUNDHANDLER_HXX_
#define __FRAMEWORK_HANDLER_SOUNDHANDLER_HXX_

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________

#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/frame/XNotifyingDispatch.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/document/XExtendedFilterDetection.hpp>
#include <com/sun/star/media/XPlayer.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/util/URL.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <cppuhelper/weak.hxx>

#include <vcl/timer.hxx>
#include <tools/link.hxx>
#include <avmedia/mediawindow.hxx>
#include <vos/mutex.hxx>

namespace css = ::com::sun::star;

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

namespace avmedia{

//_________________________________________________________________________________________________________________
//	exported const
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	exported definitions
//_________________________________________________________________________________________________________________

struct ThreadHelpBase
{
    public:
        mutable ::vos::OMutex m_aLock;
};

/*-************************************************************************************************************//**
    @short          handler to detect and play sounds ("wav" and "au" only!)
    @descr          Register this implementation as a content handler to detect and/or play wav- and au-sounds.
                    It doesn't depend from the target platform. But one instance of this class
                    can play one sound at the same time only. Means every new dispatch request will stop the
                    might still running one. So we support one operation/one URL/one listener at the same time
                    only.

    @devstatus      ready
    @threadsafe     yes
*//*-*************************************************************************************************************/
class SoundHandler  :   // interfaces
                        public  css::lang::XTypeProvider
                    ,   public  css::lang::XServiceInfo
                    ,   public  css::frame::XNotifyingDispatch // => XDispatch
                    ,   public  css::document::XExtendedFilterDetection
                        // baseclasses
                        // Order is necessary for right initialization!
                    ,   private ThreadHelpBase
                    ,   public  ::cppu::OWeakObject
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------
	public:

		//---------------------------------------------------------------------------------------------------------
		//	constructor / destructor
		//---------------------------------------------------------------------------------------------------------
                 SoundHandler( const css::uno::Reference< css::uno::XComponentContext >& xContext );
        virtual ~SoundHandler(                                                                    );

		//---------------------------------------------------------------------------------------------------------
        //  XInterface, XTypeProvider, XServiceInfo
		//---------------------------------------------------------------------------------------------------------
        virtual css::uno::Any  SAL_CALL queryInterface( const css::uno::Type& aType   );
        virtual void SAL_CALL acquire() throw();
        virtual void SAL_CALL release() throw();
        virtual css::uno::Sequence< css::uno::Type >  SAL_CALL getTypes ();
        virtual css::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();


    /* interface XServiceInfo */
       virtual ::rtl::OUString                                        SAL_CALL getImplementationName              (                                                                               );
       virtual sal_Bool                                               SAL_CALL supportsService                    ( const ::rtl::OUString&                                        sServiceName    );
       virtual css::uno::Sequence< ::rtl::OUString >                  SAL_CALL getSupportedServiceNames           (                                                                               );
    /* Helper for XServiceInfo */
       static css::uno::Sequence< ::rtl::OUString >                   SAL_CALL impl_getStaticSupportedServiceNames(                                                                               );
       static ::rtl::OUString                                         SAL_CALL impl_getStaticImplementationName   (                                                                               );
    /* Helper for registry */
       static css::uno::Reference< css::uno::XInterface >             SAL_CALL impl_createInstance                ( const css::uno::Reference< css::uno::XComponentContext >& xContext );
    /* Helper for initialization of service by using own reference! */
       virtual void                                                   SAL_CALL impl_initService                   (                                                                               );

        //---------------------------------------------------------------------------------------------------------
        //  XNotifyingDispatch
        //---------------------------------------------------------------------------------------------------------
        virtual void SAL_CALL dispatchWithNotification(const css::util::URL&                                             aURL      ,
                                                       const css::uno::Sequence< css::beans::PropertyValue >&            lArguments,
                                                       const css::uno::Reference< css::frame::XDispatchResultListener >& xListener );

		//---------------------------------------------------------------------------------------------------------
		//	XDispatch
		//---------------------------------------------------------------------------------------------------------
        virtual void SAL_CALL dispatch              (   const   css::util::URL&                                     aURL        ,
                                                        const   css::uno::Sequence< css::beans::PropertyValue >&    lArguments  );
        // not supported !
        virtual void SAL_CALL addStatusListener     (   const   css::uno::Reference< css::frame::XStatusListener >& /*xListener*/   ,
                                                        const   css::util::URL&                                     /*aURL*/        ) {};
        virtual void SAL_CALL removeStatusListener  (   const   css::uno::Reference< css::frame::XStatusListener >& /*xListener*/   ,
                                                        const   css::util::URL&                                     /*aURL*/        ) {};

		//---------------------------------------------------------------------------------------------------------
        //  XExtendedFilterDetection
		//---------------------------------------------------------------------------------------------------------
        virtual ::rtl::OUString SAL_CALL detect     (           css::uno::Sequence< css::beans::PropertyValue >&    lDescriptor );

	//-------------------------------------------------------------------------------------------------------------
	//	protected methods
	//-------------------------------------------------------------------------------------------------------------
	protected:

	//-------------------------------------------------------------------------------------------------------------
	//	private methods
	//-------------------------------------------------------------------------------------------------------------
	private:
        DECL_LINK( implts_PlayerNotify, void* );

	//-------------------------------------------------------------------------------------------------------------
	//	variables
	//	(should be private everyway!)
	//-------------------------------------------------------------------------------------------------------------
	private:

		bool m_bError;
        css::uno::Reference< css::uno::XComponentContext >         m_xContext          ;   /// component context to create new services
        css::uno::Reference< css::uno::XInterface >                m_xSelfHold         ;   /// we must protect against dying during async(!) dispatch() call!
        css::uno::Reference< css::media::XPlayer >                 m_xPlayer           ;   /// uses avmedia player to play sounds ...

        css::uno::Reference< css::frame::XDispatchResultListener > m_xListener         ;
        Timer m_aUpdateTimer;

};      //  class SoundHandler

}		//	namespace avmedia

#endif  //  #ifndef __FRAMEWORK_HANDLER_SOUNDHANDLER_HXX_
