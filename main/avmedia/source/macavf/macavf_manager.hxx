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

#ifndef MACAVF_MANAGER_HXX
#define MACAVF_MANAGER_HXX

#include "macavf_common.hxx"

#include "com/sun/star/media/XManager.hdl"

// -----------
// - Manager -
// -----------

namespace avmedia { namespace macavf {

class Manager : public ::cppu::WeakImplHelper2 < ::com::sun::star::media::XManager,
                                                 ::com::sun::star::lang::XServiceInfo >
{
public:

    Manager( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext );
    ~Manager();

    // XManager
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::media::XPlayer > SAL_CALL createPlayer( const ::rtl::OUString& aURL );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    // Helper functions
    static ::rtl::OUString SAL_CALL getImplementationName_Static(  );
    static ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_Static(  );

private:

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > mxContext;
};

} // namespace macavf
} // namespace avmedia

#endif // MACAVF_MANAGER_HXX
