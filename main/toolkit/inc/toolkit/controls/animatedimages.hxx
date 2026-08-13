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



#ifndef TOOLKIT_ANIMATEDIMAGES_HXX
#define TOOLKIT_ANIMATEDIMAGES_HXX

#include "toolkit/controls/unocontrolbase.hxx"
#include "toolkit/controls/unocontrolmodel.hxx"

/** === begin UNO includes === **/
#include <com/sun/star/awt/XAnimation.hpp>
#include <com/sun/star/awt/XAnimatedImages.hpp>
#include <com/sun/star/container/XContainerListener.hpp>
/** === end UNO includes === **/

#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase2.hxx>

#include <boost/scoped_ptr.hpp>

//......................................................................................................................
namespace toolkit
{
//......................................................................................................................

	//==================================================================================================================
	//=
	//==================================================================================================================
    typedef ::cppu::AggImplInheritanceHelper2   <   UnoControlBase
                                                ,   ::com::sun::star::awt::XAnimation
                                                ,   ::com::sun::star::container::XContainerListener
                                                >   AnimatedImagesControl_Base;

    class AnimatedImagesControl : public AnimatedImagesControl_Base
    {
    public:
								    AnimatedImagesControl( ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > const & i_factory );
	    ::rtl::OUString				GetComponentServiceName();

	    // XAnimation
        virtual void SAL_CALL startAnimation(  );
        virtual void SAL_CALL stopAnimation(  );
        virtual ::sal_Bool SAL_CALL isAnimationRunning(  );

	    // XServiceInfo
        ::rtl::OUString SAL_CALL getImplementationName(  );
        ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

        // XControl
        sal_Bool SAL_CALL setModel( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >& i_rModel );
        void SAL_CALL createPeer( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XToolkit >& i_toolkit, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer >& i_parentPeer );


        // XContainerListener
        virtual void SAL_CALL elementInserted( const ::com::sun::star::container::ContainerEvent& Event );
        virtual void SAL_CALL elementRemoved( const ::com::sun::star::container::ContainerEvent& Event );
        virtual void SAL_CALL elementReplaced( const ::com::sun::star::container::ContainerEvent& Event );

        // XEventListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& i_event );
    };

    //==================================================================================================================
    //= AnimatedImagesControlModel
    //==================================================================================================================
    struct AnimatedImagesControlModel_Data;
    typedef ::cppu::AggImplInheritanceHelper1   <   UnoControlModel
                                                ,   ::com::sun::star::awt::XAnimatedImages
                                                >   AnimatedImagesControlModel_Base;
    class AnimatedImagesControlModel : public AnimatedImagesControlModel_Base
    {
    public:
                                        AnimatedImagesControlModel( ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > const & i_factory );
                                        AnimatedImagesControlModel( const AnimatedImagesControlModel& i_copySource );

        virtual UnoControlModel*        Clone() const;

        // XPropertySet
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );

        // XPersistObject
        ::rtl::OUString SAL_CALL getServiceName();

        // XServiceInfo
        ::rtl::OUString SAL_CALL getImplementationName(  );
        ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

        // XAnimatedImages
        virtual ::sal_Int32 SAL_CALL getStepTime();
        virtual void SAL_CALL setStepTime( ::sal_Int32 _steptime );
        virtual ::sal_Bool SAL_CALL getAutoRepeat();
        virtual void SAL_CALL setAutoRepeat( ::sal_Bool _autorepeat );
        virtual ::sal_Int16 SAL_CALL getScaleMode();
        virtual void SAL_CALL setScaleMode( ::sal_Int16 _scalemode );
        virtual ::sal_Int32 SAL_CALL getImageSetCount(  );
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getImageSet( ::sal_Int32 i_index );
        virtual void SAL_CALL insertImageSet( ::sal_Int32 i_index, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& i_imageURLs );
        virtual void SAL_CALL replaceImageSet( ::sal_Int32 i_index, const ::com::sun::star::uno::Sequence< ::rtl::OUString >& i_imageURLs );
        virtual void SAL_CALL removeImageSet( ::sal_Int32 i_index );

        // XAnimatedImages::XContainer
        virtual void SAL_CALL addContainerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& i_listener );
        virtual void SAL_CALL removeContainerListener( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& i_listener );

    protected:
                                        ~AnimatedImagesControlModel();

        ::com::sun::star::uno::Any		ImplGetDefaultValue( sal_uInt16 nPropId ) const;
        ::cppu::IPropertyArrayHelper&	SAL_CALL getInfoHelper();
	    void SAL_CALL                   setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue );

    private:
        ::boost::scoped_ptr< AnimatedImagesControlModel_Data >
                m_pData;
    };

//......................................................................................................................
} // namespace toolkit
//......................................................................................................................

#endif // TOOLKIT_ANIMATEDIMAGES_HXX
