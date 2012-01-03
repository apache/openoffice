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



// MARKER(update_precomp.py): autogen include statement, do not remove


#include "docpasswdrequest.hxx"

#ifndef _COM_SUN_STAR_TASK_XINTERACTIONABORT_HPP_
#include <com/sun/star/task/XInteractionAbort.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONPASSWORD_HPP_
#include <com/sun/star/task/XInteractionPassword.hpp>
#endif

namespace binfilter
{

//==========================================================================

class AbortContinuation : public ::cppu::WeakImplHelper1< ::com::sun::star::task::XInteractionAbort >
{
	sal_Bool mbSelected;

public:
	AbortContinuation() : mbSelected( sal_False ) {}
	
	sal_Bool isSelected() { return mbSelected; }
	
	void reset() { mbSelected = sal_False; }

	virtual void SAL_CALL select() throw(::com::sun::star::uno::RuntimeException) { mbSelected = sal_True; }
};

//==========================================================================

class PasswordContinuation : public ::cppu::WeakImplHelper1< ::com::sun::star::task::XInteractionPassword >
{
	sal_Bool mbSelected;
	::rtl::OUString maPassword;

public:
	PasswordContinuation() : mbSelected( sal_False ) {}
	
	sal_Bool isSelected() { return mbSelected; }
	
	void reset() { mbSelected = sal_False; }

	virtual void SAL_CALL select() throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPassword( const ::rtl::OUString& aPass ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getPassword(  ) throw (::com::sun::star::uno::RuntimeException);
};

void SAL_CALL PasswordContinuation::select()
	throw(::com::sun::star::uno::RuntimeException)
{
	mbSelected = sal_True;
}

void SAL_CALL PasswordContinuation::setPassword( const ::rtl::OUString& aPass )
	throw (::com::sun::star::uno::RuntimeException)
{
	maPassword = aPass;
}

::rtl::OUString SAL_CALL PasswordContinuation::getPassword() 
	throw (::com::sun::star::uno::RuntimeException)
{
	return maPassword;
}

//==========================================================================

RequestDocumentPassword::RequestDocumentPassword( ::com::sun::star::task::PasswordRequestMode nMode, ::rtl::OUString aName )
{
	::rtl::OUString temp;
	::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > temp2;
	::com::sun::star::task::DocumentPasswordRequest 
		aDocumentPasswordRequest( temp,
                  		  temp2,
						  ::com::sun::star::task::InteractionClassification_QUERY,
						  nMode,
						  aName );

   	m_aRequest <<= aDocumentPasswordRequest;

   	m_pAbort  = new AbortContinuation;
   	m_pPassword = new PasswordContinuation;

   	m_lContinuations.realloc( 2 );
   	m_lContinuations[0] = ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation >( m_pAbort  );
   	m_lContinuations[1] = ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation >( m_pPassword );
}

sal_Bool RequestDocumentPassword::isPassword()
{
	return m_pPassword->isSelected();
}
		
::rtl::OUString RequestDocumentPassword::getPassword()
{
	return m_pPassword->getPassword();
}
		
::com::sun::star::uno::Any SAL_CALL RequestDocumentPassword::getRequest()
		throw( ::com::sun::star::uno::RuntimeException )
{
	return m_aRequest;
}

::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation > >
	SAL_CALL RequestDocumentPassword::getContinuations()
		throw( ::com::sun::star::uno::RuntimeException )
{
	return m_lContinuations;
}

}

