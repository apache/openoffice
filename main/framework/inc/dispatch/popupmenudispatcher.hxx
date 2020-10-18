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



#ifndef __FRAMEWORK_DISPATCH_POPUPMENUDISPATCHER_HXX_
#define __FRAMEWORK_DISPATCH_POPUPMENUDISPATCHER_HXX_

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________

#include <services/frame.hxx>
/*
#include <macros/generic.hxx>
*/
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>
/*
#include <macros/debug.hxx>
*/
#include <threadhelp/threadhelpbase.hxx>
#include <general.h>
#include <stdtypes.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/util/URL.hpp>
#include <com/sun/star/frame/DispatchDescriptor.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/frame/XFrameLoader.hpp>
#include <com/sun/star/frame/XLoadEventListener.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/FeatureStateEvent.hpp>
#include <com/sun/star/frame/XFrameActionListener.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#ifndef _COM_SUN_STAR_URI_XURLREFERENCEFACTORY_HPP_
#include <com/sun/star/uri/XUriReferenceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_URI_XURLREFERENCE_HPP_
#include <com/sun/star/uri/XUriReference.hpp>
#endif

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <cppuhelper/weak.hxx>
#include <cppuhelper/weakref.hxx>
#include <cppuhelper/interfacecontainer.h>

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

namespace framework{


//_________________________________________________________________________________________________________________
//	exported const
//_________________________________________________________________________________________________________________

//_________________________________________________________________________________________________________________
//	exported definitions
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
	We must save informations about our listener and URL for listening.
	We implement this as a hashtable for strings.
*//*-*************************************************************************************************************/

typedef ::cppu::OMultiTypeInterfaceContainerHelperVar<	::rtl::OUString			,
														rtl::OUStringHash,
														std::equal_to< ::rtl::OUString > > IMPL_ListenerHashContainer;


/*-************************************************************************************************************//**
	@short			helper for desktop only(!) to create new tasks on demand for dispatches
	@descr			Use this class as member only! Never use it as baseclass.
					XInterface will be ambiguous and we hold a weakcss::uno::Reference to our OWNER - not to our SUPERCLASS!

	@implements		XInterface
					XDispatch
					XLoadEventListener
					XFrameActionListener
					XEventListener
	@base			ThreadHelpBase
					OWeakObject

	@devstatus		ready to use
*//*-*************************************************************************************************************/
class PopupMenuDispatcher :   // interfaces
								public css::lang::XTypeProvider			,
                                public css::lang::XServiceInfo          ,
                                public css::frame::XDispatchProvider    ,
								public css::frame::XDispatch			,
								public css::frame::XFrameActionListener	,
                                public css::lang::XInitialization       ,
								// baseclasses
								// Order is necessary for right initialization!
								public ThreadHelpBase					,
								public cppu::OWeakObject
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------
	public:

        //	constructor / destructor
        PopupMenuDispatcher( const css::uno::Reference< css::lang::XMultiServiceFactory >& xFactory );

        // XInterface, XTypeProvider, XServiceInfo
        FWK_DECLARE_XINTERFACE
        FWK_DECLARE_XTYPEPROVIDER
        DECLARE_XSERVICEINFO

        // XInitialization
        virtual void SAL_CALL initialize( const css::uno::Sequence< css::uno::Any >& lArguments ) throw( css::uno::Exception       ,
                                                                                                         css::uno::RuntimeException);
        // XDispatchProvider
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > SAL_CALL queryDispatch( 
            const ::com::sun::star::util::URL&  aURL        ,
            const ::rtl::OUString& sTarget     ,
            sal_Int32              nFlags      )
        throw( ::com::sun::star::uno::RuntimeException );
        
        virtual css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > > SAL_CALL queryDispatches( 
            const css::uno::Sequence< css::frame::DispatchDescriptor >& lDescriptor ) 
        throw( css::uno::RuntimeException );

		//	XDispatch
		virtual void SAL_CALL dispatch(	const css::util::URL&                                  aURL,
										const css::uno::Sequence< css::beans::PropertyValue >& seqProperties ) throw( css::uno::RuntimeException );

		virtual void SAL_CALL addStatusListener( const css::uno::Reference< css::frame::XStatusListener >& xControl,
												 const css::util::URL&                                     aURL	) throw( css::uno::RuntimeException );

    	virtual void SAL_CALL removeStatusListener(	const css::uno::Reference< css::frame::XStatusListener >& xControl,
													const css::util::URL&                                     aURL	) throw( css::uno::RuntimeException );


		//	 XFrameActionListener
		virtual void SAL_CALL frameAction( const css::frame::FrameActionEvent& aEvent ) throw ( css::uno::RuntimeException );

		//	 XEventListener
        void SAL_CALL disposing( const css::lang::EventObject& aEvent ) throw( css::uno::RuntimeException );

	//	protected methods
	protected:
        virtual ~PopupMenuDispatcher();

        void impl_RetrievePopupControllerQuery();
        void impl_CreateUriRefFactory();

	//	private methods

	//	variables
	private:
		css::uno::WeakReference< css::frame::XFrame >			m_xWeakFrame		;	/// css::uno::WeakReference to frame (Don't use a hard css::uno::Reference. Owner can't delete us then!)
        css::uno::Reference< css::container::XNameAccess >      m_xPopupCtrlQuery   ;   /// reference to query for popup controller
        css::uno::Reference< css::uri::XUriReferenceFactory >   m_xUriRefFactory    ;   /// reference to the uri reference factory
		css::uno::Reference< css::lang::XMultiServiceFactory >	m_xFactory			;	/// factory shared with our owner to create new services!
		IMPL_ListenerHashContainer								m_aListenerContainer;	/// hash table for listener at specified URLs
		sal_Bool												m_bAlreadyDisposed	;	/// Protection against multiple disposing calls.
		sal_Bool												m_bActivateListener ;	/// dispatcher is listener for frame activation

}; //  class PopupMenuDispatcher

}		//	namespace framework

#endif  //  #ifndef __FRAMEWORK_DISPATCH_POPUPMENUDISPATCHER_HXX_
