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




#ifndef _MCNTTFACTORY_HXX_
#define _MCNTTFACTORY_HXX_

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------

#include <rtl/ustring.hxx>
#include <sal/types.h>
#include <cppuhelper/compbase2.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/datatransfer/XMimeContentTypeFactory.hpp>

//------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// deklarations
//------------------------------------------------------------------------

class CMimeContentTypeFactory : public
	cppu::WeakImplHelper2< com::sun::star::datatransfer::XMimeContentTypeFactory, \
	com::sun::star::lang::XServiceInfo >
{

public:
	CMimeContentTypeFactory( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& rSrvMgr );

	//------------------------------------------------
	// XMimeContentTypeFactory
	//------------------------------------------------

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XMimeContentType > SAL_CALL createMimeContentType( const ::rtl::OUString& aContentType );

	//------------------------------------------------
	// XServiceInfo
	//------------------------------------------------

	virtual ::rtl::OUString SAL_CALL getImplementationName(	 );

	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );

	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

private:
	::osl::Mutex																	       m_aMutex;
	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_SrvMgr;
};


#endif
