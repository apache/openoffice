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



#ifndef _FRAMEWORK_UIELEMENT_PROGRESSBARWRAPPER_HXX_
#define _FRAMEWORK_UIELEMENT_PROGRESSBARWRAPPER_HXX_

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________

#include <helper/uielementwrapperbase.hxx>
#include <threadhelp/threadhelpbase.hxx>
#include <macros/generic.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/debug.hxx>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/task/XStatusIndicator.hpp>
#include <com/sun/star/awt/XWindow.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <cppuhelper/weak.hxx>
#include <cppuhelper/weakref.hxx>

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

namespace framework{

class ProgressBarWrapper : public UIElementWrapperBase
{
    public:
        //---------------------------------------------------------------------------------------------------------
		//	constructor / destructor
		//---------------------------------------------------------------------------------------------------------
        ProgressBarWrapper();
        virtual ~ProgressBarWrapper();

        // public interfaces
        void setStatusBar( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& rStatusBar, sal_Bool bOwnsInstance = sal_False );
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > getStatusBar() const;

        // wrapped methods of ::com::sun::star::task::XStatusIndicator
        void start( const ::rtl::OUString& Text, ::sal_Int32 Range );
        void end();
        void setText( const ::rtl::OUString& Text );
        void setValue( ::sal_Int32 Value );
        void reset();

        // UNO interfaces
        // XComponent
        virtual void SAL_CALL dispose();

        // XInitialization
        virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

        // XUpdatable
        virtual void SAL_CALL update();

        // XUIElement
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getRealInterface();

	//-------------------------------------------------------------------------------------------------------------
	//	variables
	//	(should be private everyway!)
	//-------------------------------------------------------------------------------------------------------------
	private:
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >         m_xStatusBar;    // Reference to our status bar XWindow
        ::com::sun::star::uno::WeakReference< ::com::sun::star::uno::XInterface >  m_xProgressBarIfacWrapper;
        sal_Bool                                                                   m_bOwnsInstance; // Indicator that we are owner of the XWindow
        sal_Int32                                                                  m_nRange;
        sal_Int32                                                                  m_nValue;
        rtl::OUString                                                              m_aText;
};      //  class ProgressBarWrapper

}		//	namespace framework

#endif // _FRAMEWORK_UIELEMENT_PROGRESSBARWRAPPER_HXX_
