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



#ifndef __FRAMEWORK_CLASSES_PROPERTYSETHELPER_HXX_
#define __FRAMEWORK_CLASSES_PROPERTYSETHELPER_HXX_

//_________________________________________________________________________________________________________________
//	my own includes

#include <threadhelp/threadhelpbase.hxx>
#include <threadhelp/transactionbase.hxx>
#include <macros/debug.hxx>
#include <general.h>
#include <stdtypes.h>

//_________________________________________________________________________________________________________________
//	interface includes
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/beans/PropertyExistException.hpp>
#include <com/sun/star/beans/UnknownPropertyException.hpp>

//_________________________________________________________________________________________________________________
//	other includes

#include <cppuhelper/weakref.hxx>
#include <fwidllapi.h>

//_________________________________________________________________________________________________________________
//	namespace

namespace framework{

//_________________________________________________________________________________________________________________

/** supports the API XPropertySet and XPropertySetInfo.
 *
 *  It must be used as baseclass. The internal list of supported
 *  properties can be changed everytimes so dynamic property set's
 *  can be implemented.
 *
 *  Further the derived and this base class share the same lock.
 *  So it's possible to be threadsafe if it's needed.
*/
class FWI_DLLPUBLIC PropertySetHelper : public css::beans::XPropertySet
                        , public css::beans::XPropertySetInfo
{
    //-------------------------------------------------------------------------
    /* types */
    protected:

        typedef BaseHash< css::beans::Property > TPropInfoHash;

    //-------------------------------------------------------------------------
    /* member */
    protected:

        css::uno::Reference< css::lang::XMultiServiceFactory > m_xSMGR;

        PropertySetHelper::TPropInfoHash m_lProps;

        ListenerHash m_lSimpleChangeListener;
        ListenerHash m_lVetoChangeListener;

        sal_Bool m_bReleaseLockOnCall;

        // hold it weak ... otherwise this helper has to be "killed" explicitly .-)
        css::uno::WeakReference< css::uno::XInterface > m_xBroadcaster;

        LockHelper& m_rLock;
        TransactionManager& m_rTransactionManager;

    //-------------------------------------------------------------------------
    /* native interface */
    public:

        //---------------------------------------------------------------------
        /** initialize new instance of this helper.
         *
         *  @param  xSMGR
         *          points to an uno service manager, which is used internally to create own
         *          needed uno services.
         *
         *  @param  pExternalLock
         *          this helper must be used as a baseclass ...
         *          but then it should synchronize its own calls
         *          with the same lock then it's superclass uses.
         *
         *  @param  pExternalTransactionManager
         *          this helper must be used as a baseclass ...
         *          but then it should synchronize its own calls
         *          with the same transaction manager then it's superclass.
         *
         *  @param  bReleaseLockOnCall
         *          see member m_bReleaseLockOnCall
         */
        PropertySetHelper(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR                       ,
                                LockHelper*                                             pExternalLock               ,
                                TransactionManager*                                     pExternalTransactionManager ,
                                sal_Bool                                                bReleaseLockOnCall          );

        //---------------------------------------------------------------------
        /** free all needed memory.
         */
        virtual ~PropertySetHelper();

        //---------------------------------------------------------------------
        /** set a new owner for this helper.
         *
         *  This owner is used as source for all broadcasted events.
         *  Further we hold it weak, because we dont wish to be disposed() .-)
         */
        void impl_setPropertyChangeBroadcaster(const css::uno::Reference< css::uno::XInterface >& xBroadcaster);

        //---------------------------------------------------------------------
        /** add a new property info to the set of supported ones.
         *
         *  @param  aProperty
         *          describes the new property.
         *
         *  @throw  [com::sun::star::beans::PropertyExistException]
         *          if a property with the same name already exists.
         *
         *  Note:   The consistence of the whole set of properties is not checked here.
         *          Means e.g. ... a handle which exists more then once is not detected.
         *          The owner of this class has to be sure, that every new property does
         *          not clash with any existing one.
         */
        virtual void SAL_CALL impl_addPropertyInfo(const css::beans::Property& aProperty)
            throw(css::beans::PropertyExistException,
                  css::uno::Exception               );

        //---------------------------------------------------------------------
        /** remove an existing property info from the set of supported ones.
         *
         *  @param  sProperty
         *          the name of the property.
         *
         *  @throw  [com::sun::star::beans::UnknownPropertyException]
         *          if no property with the specified name exists.
         */
        virtual void SAL_CALL impl_removePropertyInfo(const ::rtl::OUString& sProperty)
            throw(css::beans::UnknownPropertyException,
                  css::uno::Exception                 );

        //---------------------------------------------------------------------
        /** mark the object as "useable for working" or "dead".
         *
         *  This correspond to the lifetime handling implemented by the base class TransactionBase.
         *  There is no chance to reactive a "dead" object by calling impl_enablePropertySet()
         *  again!
         */
        virtual void SAL_CALL impl_enablePropertySet();
        virtual void SAL_CALL impl_disablePropertySet();

        //---------------------------------------------------------------------
        /**
         */
        virtual void SAL_CALL impl_setPropertyValue(const ::rtl::OUString& sProperty,
                                                          sal_Int32        nHandle  ,
                                                    const css::uno::Any&   aValue   ) = 0;

        virtual css::uno::Any SAL_CALL impl_getPropertyValue(const ::rtl::OUString& sProperty,
                                                                   sal_Int32        nHandle  ) = 0;

    //-------------------------------------------------------------------------
    /* uno interface */
    public:

        // XPropertySet
        virtual css::uno::Reference< css::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo()
            throw(css::uno::RuntimeException);

        virtual void SAL_CALL setPropertyValue(const ::rtl::OUString& sProperty,
                                               const css::uno::Any&   aValue   )
            throw(css::beans::UnknownPropertyException,
                  css::beans::PropertyVetoException   ,
                  css::lang::IllegalArgumentException ,
                  css::lang::WrappedTargetException   ,
                  css::uno::RuntimeException          );

        virtual css::uno::Any SAL_CALL getPropertyValue(const ::rtl::OUString& sProperty)
            throw(css::beans::UnknownPropertyException,
                  css::lang::WrappedTargetException   ,
                  css::uno::RuntimeException          );

        virtual void SAL_CALL addPropertyChangeListener(const ::rtl::OUString&                                            sProperty,
                                                        const css::uno::Reference< css::beans::XPropertyChangeListener >& xListener)
            throw(css::beans::UnknownPropertyException,
                  css::lang::WrappedTargetException   ,
                  css::uno::RuntimeException          );

        virtual void SAL_CALL removePropertyChangeListener(const ::rtl::OUString&                                            sProperty,
                                                           const css::uno::Reference< css::beans::XPropertyChangeListener >& xListener)
            throw(css::beans::UnknownPropertyException,
                  css::lang::WrappedTargetException   ,
                  css::uno::RuntimeException          );

        virtual void SAL_CALL addVetoableChangeListener(const ::rtl::OUString&                                            sProperty,
                                                        const css::uno::Reference< css::beans::XVetoableChangeListener >& xListener)
            throw(css::beans::UnknownPropertyException,
                  css::lang::WrappedTargetException   ,
                  css::uno::RuntimeException          );

        virtual void SAL_CALL removeVetoableChangeListener(const ::rtl::OUString&                                            sProperty,
                                                           const css::uno::Reference< css::beans::XVetoableChangeListener >& xListener)
            throw(css::beans::UnknownPropertyException,
                  css::lang::WrappedTargetException   ,
                  css::uno::RuntimeException          );

        // XPropertySetInfo
        virtual css::uno::Sequence< css::beans::Property > SAL_CALL getProperties()
            throw(css::uno::RuntimeException);

        virtual css::beans::Property SAL_CALL getPropertyByName(const ::rtl::OUString& sName)
            throw(css::beans::UnknownPropertyException,
                  css::uno::RuntimeException          );

        virtual sal_Bool SAL_CALL hasPropertyByName(const ::rtl::OUString& sName)
            throw(css::uno::RuntimeException);

    //-------------------------------------------------------------------------
    /* internal helper */
    private:

        sal_Bool impl_existsVeto(const css::beans::PropertyChangeEvent& aEvent);

        void impl_notifyChangeListener(const css::beans::PropertyChangeEvent& aEvent);
};

} // namespace framework

#endif // #ifndef __FRAMEWORK_CLASSES_PROPERTYSETHELPER_HXX_
