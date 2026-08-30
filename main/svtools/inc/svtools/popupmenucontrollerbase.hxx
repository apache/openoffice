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



#ifndef __SVTOOLS_POPUPMENUCONTROLLERBASE_HXX_
#define __SVTOOLS_POPUPMENUCONTROLLERBASE_HXX_

#include "svtools/svtdllapi.h"

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/frame/XPopupMenuController.hpp>
#include <com/sun/star/uri/XUriReferenceFactory.hpp>
#include <com/sun/star/uri/XUriReference.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>
//_________________________________________________________________________________________________________________
//	includes of other projects
//_________________________________________________________________________________________________________________
#include <toolkit/awt/vclxmenu.hxx>
#include <cppuhelper/compbase7.hxx>
#include <comphelper/broadcasthelper.hxx>
#include <cppuhelper/weak.hxx>
#include <rtl/ustring.hxx>

namespace svt
{
	struct PopupMenuControllerBaseDispatchInfo;

	typedef ::cppu::WeakComponentImplHelper7<
						com::sun::star::lang::XServiceInfo		      ,
						com::sun::star::frame::XPopupMenuController ,
						com::sun::star::lang::XInitialization         ,
						com::sun::star::frame::XStatusListener        ,
						com::sun::star::awt::XMenuListener            ,
						com::sun::star::frame::XDispatchProvider      ,
						com::sun::star::frame::XDispatch > PopupMenuControllerBaseType;

    class SVT_DLLPUBLIC PopupMenuControllerBase : protected ::comphelper::OBaseMutex,	// Struct for right initialization of mutex member! Must be first of baseclasses.
												  public PopupMenuControllerBaseType
    {
		public:
		    PopupMenuControllerBase( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager );
		    virtual ~PopupMenuControllerBase();

		    // XServiceInfo
            virtual ::rtl::OUString SAL_CALL getImplementationName(  ) = 0;
            virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
            virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) = 0;

            // XPopupMenuController
            virtual void SAL_CALL setPopupMenu( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XPopupMenu >& PopupMenu );
		    virtual void SAL_CALL updatePopupMenu();

            // XInitialization
            virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

		    // XStatusListener
		    virtual void SAL_CALL statusChanged( const ::com::sun::star::frame::FeatureStateEvent& Event ) = 0;

            // XMenuListener
            virtual void SAL_CALL itemHighlighted( const ::com::sun::star::awt::MenuEvent& rEvent );
            virtual void SAL_CALL itemSelected( const ::com::sun::star::awt::MenuEvent& rEvent );
            virtual void SAL_CALL itemActivated( const ::com::sun::star::awt::MenuEvent& rEvent );
            virtual void SAL_CALL itemDeactivated( const ::com::sun::star::awt::MenuEvent& rEvent );

            // XDispatchProvider
            virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > SAL_CALL queryDispatch( const ::com::sun::star::util::URL& aURL, const ::rtl::OUString& sTarget, sal_Int32 nFlags );
            virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > > SAL_CALL queryDispatches( const ::com::sun::star::uno::Sequence< ::com::sun::star::frame::DispatchDescriptor >& lDescriptor );

            // XDispatch
		    virtual void SAL_CALL dispatch(	const ::com::sun::star::util::URL& aURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& seqProperties );
		    virtual void SAL_CALL addStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >& xControl, const ::com::sun::star::util::URL& aURL );
    	    virtual void SAL_CALL removeStatusListener(	const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >& xControl, const ::com::sun::star::util::URL& aURL );

            // XEventListener
		    virtual void SAL_CALL disposing( const com::sun::star::lang::EventObject& Source );

			void dispatchCommand( const ::rtl::OUString& sCommandURL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& rArgs );

	protected:
			void throwIfDisposed();

			/** helper method to cause statusChanged is called once for the given command url */
			void SAL_CALL updateCommand( const rtl::OUString& rCommandURL );

			/** this function is called upon disposing the component
			*/
			virtual void SAL_CALL disposing();

            virtual void resetPopupMenu( com::sun::star::uno::Reference< com::sun::star::awt::XPopupMenu >& rPopupMenu );
            virtual void impl_setPopupMenu();
            virtual void impl_select(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >& _xDispatch,const ::com::sun::star::util::URL& aURL);
            ::rtl::OUString determineBaseURL( const ::rtl::OUString& aURL );

			DECL_STATIC_LINK( PopupMenuControllerBase, ExecuteHdl_Impl, PopupMenuControllerBaseDispatchInfo* );


            bool                                                                             m_bInitialized;
            rtl::OUString                                                                    m_aCommandURL;
            rtl::OUString                                                                    m_aBaseURL;
            ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >           m_xDispatch;
            ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >              m_xFrame;
            ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xServiceManager;
            ::com::sun::star::uno::Reference< ::com::sun::star::util::XURLTransformer >      m_xURLTransformer;
            ::com::sun::star::uno::Reference< ::com::sun::star::awt::XPopupMenu >            m_xPopupMenu;
    };
}

#endif // __SVTOOLS_POPUPMENUCONTROLLERBASE_HXX_
