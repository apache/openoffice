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



#ifndef INCLUDED_SVTOOLS_DOCPASSWDREQUEST_HXX
#define INCLUDED_SVTOOLS_DOCPASSWDREQUEST_HXX

#ifndef _COM_SUN_STAR_TASK_DOCUMENTPASSWORDREQUEST_HPP_
#include <com/sun/star/task/DocumentPasswordRequest.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONREQUEST_HPP_
#include <com/sun/star/task/XInteractionRequest.hpp>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

namespace binfilter {

class AbortContinuation;
class PasswordContinuation;

class  RequestDocumentPassword : public ::cppu::WeakImplHelper1< ::com::sun::star::task::XInteractionRequest >
{
    ::com::sun::star::uno::Any m_aRequest;
		
    ::com::sun::star::uno::Sequence< 
					::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation > 
				> m_lContinuations;
		
    AbortContinuation*	    m_pAbort;
    PasswordContinuation*	m_pPassword;

public:
    RequestDocumentPassword( ::com::sun::star::task::PasswordRequestMode nMode, ::rtl::OUString aName );
	
    sal_Bool isPassword();
		
	::rtl::OUString getPassword();
		
    virtual ::com::sun::star::uno::Any SAL_CALL getRequest() 
		throw( ::com::sun::star::uno::RuntimeException );

    virtual ::com::sun::star::uno::Sequence< 
				::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation > 
			> SAL_CALL getContinuations() 
		throw( ::com::sun::star::uno::RuntimeException );
};  

}

#endif /* INCLUDED_SVTOOLS_DOCPASSWDREQUEST_HXX */
