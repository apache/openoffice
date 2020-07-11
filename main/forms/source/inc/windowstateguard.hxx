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



#ifndef FORMS_WINDOWSTATEGUARD_HXX
#define FORMS_WINDOWSTATEGUARD_HXX

/** === begin UNO includes === **/
#include <com/sun/star/awt/XWindow2.hpp>
#include <com/sun/star/awt/XControlModel.hpp>
/** === end UNO includes === **/
#include <rtl/ref.hxx>

//........................................................................
namespace frm
{
//........................................................................

	//====================================================================
	//= WindowStateGuard
	//====================================================================
    class WindowStateGuard_Impl;

    /** a helper class which monitors certain states of an XWindow2, and ensures
        that they're consistent with respective properties at an XModel.

        For form controls, window states - such as "Enabled" - can be set by various
        means. You can set the respective control model property, you can directly manipulate
        the XWindow2, or the state can change implicitly due to VCL actions. In any case,
        we need to ensure that the state does not contradict the model property "too much".

        As an example, consider a form control which, according to its model's property, is disabled.
        Now when the parent VCL window of the control's VCL window is enabled, then the control's
        window is enabled, too - which contradicts the model property.

        A WindowStateGuard helps you preventing such inconsistent states.

        The class is not threadsafe.
    */
	class WindowStateGuard
	{
    private:
        ::rtl::Reference< WindowStateGuard_Impl >   m_pImpl;

    public:
        WindowStateGuard();
        ~WindowStateGuard();

        void    attach(
            const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow2 >& _rxWindow,
            const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >& _rxModel
        );
	};

//........................................................................
} // namespace frm
//........................................................................

#endif // FORMS_WINDOWSTATEGUARD_HXX

