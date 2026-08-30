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



#ifndef TOOLKIT_ANIMATEDIMAGEPEER_HXX
#define TOOLKIT_ANIMATEDIMAGEPEER_HXX

#include "toolkit/awt/vclxwindow.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/awt/XAnimation.hpp>
#include <com/sun/star/util/XModifyListener.hpp>
/** === end UNO includes === **/

#include <cppuhelper/implbase3.hxx>

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

//......................................................................................................................
namespace toolkit
{
//......................................................................................................................

	//==================================================================================================================
	//= AnimatedImagesPeer
	//==================================================================================================================
    struct AnimatedImagesPeer_Data;
    typedef ::cppu::ImplInheritanceHelper3  <   VCLXWindow
                                            ,   ::com::sun::star::awt::XAnimation
                                            ,   ::com::sun::star::container::XContainerListener
                                            ,   ::com::sun::star::util::XModifyListener
                                            >   AnimatedImagesPeer_Base;

    class AnimatedImagesPeer    :public AnimatedImagesPeer_Base
                                ,public ::boost::noncopyable
	{
    public:
        AnimatedImagesPeer();

    protected:
        ~AnimatedImagesPeer();

    public:
        // XAnimation
        virtual void SAL_CALL startAnimation(  );
        virtual void SAL_CALL stopAnimation(  );
        virtual ::sal_Bool SAL_CALL isAnimationRunning(  );

        // VclWindowPeer
        virtual void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
        virtual ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );

        // XContainerListener
        virtual void SAL_CALL elementInserted( const ::com::sun::star::container::ContainerEvent& Event );
        virtual void SAL_CALL elementRemoved( const ::com::sun::star::container::ContainerEvent& Event );
        virtual void SAL_CALL elementReplaced( const ::com::sun::star::container::ContainerEvent& Event );

        // XEventListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& i_event );

        // XModifyListener
        virtual void SAL_CALL modified( const ::com::sun::star::lang::EventObject& i_event );

        // XComponent
        void SAL_CALL dispose(  );

    protected:
        void ProcessWindowEvent( const VclWindowEvent& i_windowEvent );

    private:
        /** updates our images with the ones from the givem XAnimatedImages component
        */
        void    impl_updateImages_nolck( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& i_animatedImages );

    private:
        ::boost::scoped_ptr< AnimatedImagesPeer_Data >   m_pData;
	};

//......................................................................................................................
} // namespace toolkit
//......................................................................................................................

#endif // TOOLKIT_ANIMATEDIMAGEPEER_HXX
