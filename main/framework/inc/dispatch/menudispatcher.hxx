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



#ifndef __FRAMEWORK_DISPATCH_MENUDISPATCHER_HXX_
#define __FRAMEWORK_DISPATCH_MENUDISPATCHER_HXX_

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________

#include <classes/taskcreator.hxx>
#include <services/frame.hxx>
#include <macros/generic.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/debug.hxx>
#include <threadhelp/threadhelpbase.hxx>
#include <classes/menumanager.hxx>
#include <general.h>
#include <stdtypes.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/util/URL.hpp>
#include <com/sun/star/frame/DispatchDescriptor.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/frame/XFrameLoader.hpp>
#include <com/sun/star/frame/XLoadEventListener.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/FeatureStateEvent.hpp>
#include <com/sun/star/frame/XFrameActionListener.hpp>

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
class MenuDispatcher   :   // interfaces
								public css::lang::XTypeProvider			,
								public css::frame::XDispatch			,
								public css::frame::XFrameActionListener	,
								// baseclasses
								// Order is necessary for right initialization!
								public ThreadHelpBase						,
								public cppu::OWeakObject
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------

	public:

		//---------------------------------------------------------------------------------------------------------
		//	constructor / destructor
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		standard ctor
			@descr		These initialize a new instance of this class with needed informations for work.

			@seealso	using at owner

			@param		"xFactory"	, css::uno::Reference to servicemanager for creation of new services
			@param		"xOwner"	, css::uno::Reference to our owner, the Desktop!!!
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

        MenuDispatcher(    const   css::uno::Reference< css::lang::XMultiServiceFactory >& xFactory    ,
							const	css::uno::Reference< css::frame::XFrame >&				xOwner		);

		//---------------------------------------------------------------------------------------------------------
		//	XInterface
		//---------------------------------------------------------------------------------------------------------

		FWK_DECLARE_XINTERFACE
		FWK_DECLARE_XTYPEPROVIDER

		//---------------------------------------------------------------------------------------------------------
		//	XDispatch
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short      dispatch URL with arguments
			@descr		Every dispatch create a new task. If load of URL failed task will deleted automatically!

			@seealso	-

			@param		"aURL"			, URL to dispatch.
			@param		"seqArguments"	, list of optional arguments.
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		virtual void SAL_CALL dispatch(	const	css::util::URL&										aURL			,
										const	css::uno::Sequence< css::beans::PropertyValue >&	seqProperties	) throw( css::uno::RuntimeException );

		/*-****************************************************************************************************//**
			@short      add listener for state events
			@descr		You can add a listener to get information about status of dispatch: OK or Failed.

			@seealso	method loadFinished()
			@seealso	method loadCancelled()

			@param		"xControl"	, css::uno::Reference to a valid listener for state events.
			@param		"aURL"		, URL about listener will be informed, if something occurred.
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		virtual void SAL_CALL addStatusListener(	const	css::uno::Reference< css::frame::XStatusListener >&	xControl,
													const	css::util::URL&										aURL	) throw( css::uno::RuntimeException );

		/*-****************************************************************************************************//**
			@short      remove listener for state events
			@descr		You can remove a listener if information of dispatch isn't important for you any longer.

			@seealso	method loadFinished()
			@seealso	method loadCancelled()

			@param		"xControl"	, css::uno::Reference to a valid listener.
			@param		"aURL"		, URL on which listener has registered.
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

    	virtual void SAL_CALL removeStatusListener(	const	css::uno::Reference< css::frame::XStatusListener >&	xControl,
													const	css::util::URL&										aURL	) throw( css::uno::RuntimeException );


		//---------------------------------------------------------------------------------------------------------
		//	 XFrameActionListener
		//---------------------------------------------------------------------------------------------------------

		virtual void SAL_CALL frameAction( const css::frame::FrameActionEvent& aEvent ) throw ( css::uno::RuntimeException );

		//---------------------------------------------------------------------------------------------------------
		//	 XEventListener
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		dispose current instance
			@descr		If service helper isn't required any longer call this method to release all used ressources.

			@seealso	-

			@param		"aEvent", information about source of this event.
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		void SAL_CALL disposing( const EVENTOBJECT& aEvent ) throw( css::uno::RuntimeException );

	//-------------------------------------------------------------------------------------------------------------
	//	protected methods
	//-------------------------------------------------------------------------------------------------------------

	protected:

		/*-****************************************************************************************************//**
			@short		standard destructor
			@descr		This method destruct an instance of this class and clear some member.
						This method is protected, because its not allowed to use an instance of this class as a member!
						You MUST use a pointer.

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

        virtual ~MenuDispatcher();

	//-------------------------------------------------------------------------------------------------------------
	//	private methods
	//-------------------------------------------------------------------------------------------------------------

	private:

        DECL_LINK( Close_Impl, void* );

		/*-****************************************************************************************************//**
			@short		-
			@descr		-

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

		sal_Bool impl_setMenuBar( MenuBar* pMenuBar, sal_Bool bMenuFromResource = sal_False );

		/*-****************************************************************************************************//**
			@short		-
			@descr		-

			@seealso	-

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

        void impl_setAccelerators( Menu* pMenu, const Accelerator& aAccel );

	//-------------------------------------------------------------------------------------------------------------
	//	debug methods
	//	(should be private everyway!)
	//-------------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		debug-method to check incoming parameter of some other mehods of this class
			@descr		The following methods are used to check parameters for other methods
						of this class. The return value is used directly for an ASSERT(...).

			@seealso	ASSERTs in implementation!

			@param		css::uno::References to checking variables
			@return		sal_False on invalid parameter<BR>
						sal_True  otherway

			@onerror	-
		*//*-*****************************************************************************************************/

	#ifdef ENABLE_ASSERTIONS

	private:

        static sal_Bool impldbg_checkParameter_MenuDispatcher      (   const   css::uno::Reference< css::lang::XMultiServiceFactory >& xFactory        ,
																		const	css::uno::Reference< css::frame::XFrame >&				xOwner			);
		static sal_Bool impldbg_checkParameter_addStatusListener	(	const	css::uno::Reference< css::frame::XStatusListener >&		xControl		,
																		const	css::util::URL&											aURL			);
		static sal_Bool impldbg_checkParameter_removeStatusListener	(	const	css::uno::Reference< css::frame::XStatusListener >&		xControl		,
																		const	css::util::URL&											aURL			);
	#endif	// #ifdef ENABLE_ASSERTIONS

	//-------------------------------------------------------------------------------------------------------------
	//	variables
	//	(should be private everyway!)
	//-------------------------------------------------------------------------------------------------------------

	private:

		css::uno::WeakReference< css::frame::XFrame >			m_xOwnerWeak		;	/// css::uno::WeakReference to owner (Don't use a hard css::uno::Reference. Owner can't delete us then!)
		css::uno::Reference< css::lang::XMultiServiceFactory >	m_xFactory			;	/// factory shared with our owner to create new services!
		IMPL_ListenerHashContainer								m_aListenerContainer;	/// hash table for listener at specified URLs
		sal_Bool												m_bAlreadyDisposed	;	/// Protection against multiple disposing calls.
		sal_Bool												m_bActivateListener ;	/// dispatcher is listener for frame activation
		MenuManager*											m_pMenuManager		;	/// menu manager controlling menu dispatches

};      //  class MenuDispatcher

}		//	namespace framework

#endif  //  #ifndef __FRAMEWORK_DISPATCH_MENUDISPATCHER_HXX_
