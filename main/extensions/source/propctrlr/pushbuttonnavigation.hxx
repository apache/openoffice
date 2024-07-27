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



#ifndef EXTENSIONS_PROPCTRLR_PUSHBUTTONNAVIGATION_HXX
#define EXTENSIONS_PROPCTRLR_PUSHBUTTONNAVIGATION_HXX

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyState.hpp>

//............................................................................
namespace pcr
{
//............................................................................

    //========================================================================
    //= PushButtonNavigation
    //========================================================================
    class PushButtonNavigation
    {
    protected:
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
                    m_xControlModel;
        sal_Bool    m_bIsPushButton;

    public:
        /** ctor
            @param _rxControlModel
                the control model which is or will be bound
        */
        PushButtonNavigation(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControlModel
        );

        /** returns the current value of the "ButtonType" property, taking into account
            the "virtual" button types such as "move-to-next-record button".
        */
        ::com::sun::star::uno::Any
                getCurrentButtonType() const SAL_THROW(());

        /** sets the current value of the "ButtonType" property, taking into account
            the "virtual" button types such as "move-to-next-record button".
        */
        void    setCurrentButtonType( const ::com::sun::star::uno::Any& _rValue ) const SAL_THROW(());

        /** retrieves the current state of the "ButtonType" property, taking into account
            the "virtual" button types such as "move-to-next-record button".
        */
        ::com::sun::star::beans::PropertyState
                getCurrentButtonTypeState( ) const SAL_THROW(());

        /** returns the current value of the "TargetURL" property, taking into account
            that some URLs are special values caused by "virtual" ButtonTypes
        */
        ::com::sun::star::uno::Any
                getCurrentTargetURL() const SAL_THROW(());

        /** sets the current value of the "TargetURL" property, taking into account
            that some URLs are special values caused by "virtual" ButtonTypes
        */
        void    setCurrentTargetURL( const ::com::sun::star::uno::Any& _rValue ) const SAL_THROW(());

        /** retrieves the current state of the "TargetURL" property, taking into account
            that some URLs are special values caused by "virtual" ButtonTypes
        */
        ::com::sun::star::beans::PropertyState
                getCurrentTargetURLState( ) const SAL_THROW(());

        /** determines whether the current button type is FormButtonType_URL
        */
        bool    currentButtonTypeIsOpenURL() const;

        /** determines whether the TargetURL property does currently denote a non-empty string
        */
        bool    hasNonEmptyCurrentTargetURL() const;

    private:
        sal_Int32 implGetCurrentButtonType() const SAL_THROW(( ::com::sun::star::uno::Exception ));
    };

//............................................................................
}   // namespace pcr
//............................................................................

#endif // EXTENSIONS_PROPCTRLR_PUSHBUTTONNAVIGATION_HXX
