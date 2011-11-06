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



#ifndef _SFX_EVENTSUPPLIER_HXX_
#define _SFX_EVENTSUPPLIER_HXX_

#ifndef  _COM_SUN_STAR_LANG_XTYPEPROVIDER_HPP_
#include <com/sun/star/lang/XTypeProvider.hpp>
#endif
#ifndef  _COM_SUN_STAR_CONTAINER_XNAMEREPLACE_HPP_
#include <com/sun/star/container/XNameReplace.hpp>
#endif
#ifndef  _COM_SUN_STAR_DOCUMENT_XEVENTLISTENER_HPP_
#include <com/sun/star/document/XEventListener.hpp>
#endif
#ifndef  _COM_SUN_STAR_DOCUMENT_XEVENTBROADCASTER_HPP_
#include <com/sun/star/document/XEventBroadcaster.hpp>
#endif
#ifndef  _COM_SUN_STAR_DOCUMENT_XEVENTSSUPPLIER_HPP_
#include <com/sun/star/document/XEventsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XJOBEXECUTOR_HPP_
#include <com/sun/star/task/XJobExecutor.hpp>
#endif

#ifndef  _COM_SUN_STAR_UNO_REFERENCE_HXX_
#include <com/sun/star/uno/Reference.hxx>
#endif
#ifndef  _COM_SUN_STAR_UNO_TYPE_HXX_
#include <com/sun/star/uno/Type.hxx>
#endif

#ifndef  _SAL_TYPES_H_
#include <sal/types.h>
#endif
#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef  _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE3_HXX_
#include <cppuhelper/implbase3.hxx>
#endif

#ifndef  _SFX_SFXUNO_HXX
#include <sfxuno.hxx>
#endif

#include <vos/mutex.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <bf_svtools/lstner.hxx>
namespace binfilter {

class SvxMacro;

//--------------------------------------------------------------------------------------------------------

#define NOSUCHELEMENTEXCEPTION		::com::sun::star::container::NoSuchElementException
#define XNAMEREPLACE				::com::sun::star::container::XNameReplace
#define DOCEVENTOBJECT				::com::sun::star::document::EventObject
#define	XEVENTBROADCASTER			::com::sun::star::document::XEventBroadcaster
#define XDOCEVENTLISTENER			::com::sun::star::document::XEventListener
#define	XEVENTSSUPPLIER				::com::sun::star::document::XEventsSupplier
#define XJOBEXECUTOR                ::com::sun::star::task::XJobExecutor
#define EVENTOBJECT					::com::sun::star::lang::EventObject
#define ILLEGALARGUMENTEXCEPTION	::com::sun::star::lang::IllegalArgumentException
#define WRAPPEDTARGETEXCEPTION		::com::sun::star::lang::WrappedTargetException
#define ANY							::com::sun::star::uno::Any
#define	REFERENCE					::com::sun::star::uno::Reference
#define WEAKREFERENCE               ::com::sun::star::uno::WeakReference
#define RUNTIMEEXCEPTION			::com::sun::star::uno::RuntimeException
#define SEQUENCE					::com::sun::star::uno::Sequence
#define UNOTYPE						::com::sun::star::uno::Type
#define OUSTRING					::rtl::OUString
#define	OINTERFACECONTAINERHELPER   ::cppu::OInterfaceContainerHelper

//--------------------------------------------------------------------------------------------------------

class SfxObjectShell;
class SfxBaseModel;

//--------------------------------------------------------------------------------------------------------

class SfxEvents_Impl : public ::cppu::WeakImplHelper2< ::com::sun::star::container::XNameReplace, ::com::sun::star::document::XEventListener  >
{
	SEQUENCE< OUSTRING >			maEventNames;
	SEQUENCE< ANY >					maEventData;
	REFERENCE< XEVENTBROADCASTER >	mxBroadcaster;
	::osl::Mutex					maMutex;
	SfxObjectShell				   *mpObjShell;


public:
								SfxEvents_Impl( SfxObjectShell* pShell,
												REFERENCE< XEVENTBROADCASTER > xBroadcaster );
							   ~SfxEvents_Impl();

	//  --- XNameReplace ---
    virtual void SAL_CALL		replaceByName( const OUSTRING & aName, const ANY & aElement )
									throw( ILLEGALARGUMENTEXCEPTION, NOSUCHELEMENTEXCEPTION,
										   WRAPPEDTARGETEXCEPTION, RUNTIMEEXCEPTION );

	//  --- XNameAccess ( parent of XNameReplace ) ---
    virtual ANY SAL_CALL		getByName( const OUSTRING& aName )
									throw( NOSUCHELEMENTEXCEPTION, WRAPPEDTARGETEXCEPTION,
										   RUNTIMEEXCEPTION );
    virtual SEQUENCE< OUSTRING > SAL_CALL getElementNames() throw ( RUNTIMEEXCEPTION );
    virtual sal_Bool SAL_CALL	hasByName( const OUSTRING& aName ) throw ( RUNTIMEEXCEPTION );

	//  --- XElementAccess ( parent of XNameAccess ) ---
    virtual UNOTYPE SAL_CALL	getElementType() throw ( RUNTIMEEXCEPTION );
    virtual sal_Bool SAL_CALL	hasElements() throw ( RUNTIMEEXCEPTION );

    // --- ::document::XEventListener ---
    virtual void SAL_CALL		notifyEvent( const DOCEVENTOBJECT& aEvent )
									throw( RUNTIMEEXCEPTION );

    // --- ::lang::XEventListener ---
    virtual void SAL_CALL		disposing( const EVENTOBJECT& Source )
									throw( RUNTIMEEXCEPTION );

    static SvxMacro*            ConvertToMacro( const ANY& rElement, SfxObjectShell* pDoc, BOOL bBlowUp );
	static void					BlowUpMacro( const ANY& rIn, ANY& rOut, SfxObjectShell* pDoc );
};

class SfxGlobalEvents_Impl : public ::cppu::WeakImplHelper3< ::com::sun::star::document::XEventsSupplier,
						::com::sun::star::document::XEventBroadcaster, ::com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
	SfxEvents_Impl* 			pImp;
	REFERENCE < XNAMEREPLACE >  m_xEvents;
    WEAKREFERENCE < XJOBEXECUTOR > m_xJobsBinding;
	OINTERFACECONTAINERHELPER	m_aInterfaceContainer;
	::osl::Mutex				m_aMutex;

								void Notify( SfxBroadcaster& aBC, const	SfxHint& aHint );
public:
                                SfxGlobalEvents_Impl( const ::com::sun::star::uno::Reference < ::com::sun::star::lang::XMultiServiceFactory >& xSmgr );
								~SfxGlobalEvents_Impl();
    SFX_DECL_XSERVICEINFO
    virtual REFERENCE< XNAMEREPLACE > SAL_CALL getEvents() throw( RUNTIMEEXCEPTION );
    virtual void SAL_CALL addEventListener( const REFERENCE< XDOCEVENTLISTENER >& xListener ) throw( RUNTIMEEXCEPTION );
    virtual void SAL_CALL removeEventListener( const REFERENCE< XDOCEVENTLISTENER >& xListener ) throw( RUNTIMEEXCEPTION ) {DBG_BF_ASSERT(0, "STRIP");}//STRIP001 virtual void SAL_CALL removeEventListener( const REFERENCE< XDOCEVENTLISTENER >& xListener ) throw( RUNTIMEEXCEPTION );
};

}//end of namespace binfilter
#endif
